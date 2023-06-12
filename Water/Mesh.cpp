#include "Mesh.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Bone.h"
#include "Polygon.h"


// Assimp用
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#ifndef ASSIMP_LOAD_FLAGS
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate  | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_MakeLeftHanded)
#endif //ASSIMP_LOAD_FLAGS


#include <vector>
#include <iostream>
#include <cassert>


// aiMatrix4x4からMatrix4へ変換
static void MatrixAi2Gl(Matrix4& mat, const aiMatrix4x4 aim)
{
    mat.mat[0][0] = aim.a1;
    mat.mat[0][1] = aim.b1;
    mat.mat[0][2] = aim.c1;
    mat.mat[0][3] = aim.d1;

    mat.mat[1][0] = aim.a2;
    mat.mat[1][1] = aim.b2;
    mat.mat[1][2] = aim.c2;
    mat.mat[1][3] = aim.d2;

    mat.mat[2][0] = aim.a3;
    mat.mat[2][1] = aim.b3;
    mat.mat[2][2] = aim.c3;
    mat.mat[2][3] = aim.d3;

    mat.mat[3][0] = aim.a4;
    mat.mat[3][1] = aim.b4;
    mat.mat[3][2] = aim.c4;
    mat.mat[3][3] = aim.d4;    

}


// コンストラクタ
Mesh::Mesh()
    : pScene(nullptr)
    , numBones(0)
    , specPower(1.1f)
    , animID(0)
    , prevAnimID(0)
    , numAnimations(0)
    , playTime(0.0f)
    , mode(PLAY_ONCE)
    , bPlaying(false)
{
    
}

Mesh::~Mesh()
{
}


void Mesh::SetAnimID(int id, PlayMode m)
{
    if(prevAnimID != id)
    {
        playTime = 0.0f;
        bPlaying = true;
    }

    prevAnimID = animID;
    animID = id;
    mode = m;
}

// アニメーション時間の反映
void Mesh::BoneTransform(float deltaTime, std::vector<Matrix4> &Transforms)
{
    
    playTime += deltaTime;
    float TicksPerSecond = (float)(pScene->mAnimations[animID]->mTicksPerSecond != 0 ? pScene->mAnimations[animID]->mTicksPerSecond : 25.0f);
    float TimeInTicks = playTime * TicksPerSecond;


    if (mode == PLAY_ONCE)
    {
        if (TimeInTicks > durations[animID])
        {
            // 再生が終わっている場合は、最後のポーズに固定
            playTime = durations[animID];
            bPlaying = false;
        }
    }
    
    Matrix4 identity = Matrix4::Identity;
    

    
    float AnimationTime = fmod(TimeInTicks, (float)pScene->mAnimations[animID]->mDuration);
    
    ReadNodeHeirarchy(AnimationTime, pScene->mRootNode, identity);

    Transforms.resize(numBones);

    for (unsigned int i = 0; i < numBones; i++)
    {
         Transforms[i] = boneInfo[i].FinalTransformation;
    }
}

// 階層を辿ってノードの変換行列を得る
void Mesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4& ParentTransform)
{
    std::string NodeName(pNode->mName.data);

    
    const aiAnimation* pAnimation = pScene->mAnimations[animID];
    
        
    Matrix4 NodeTransformation;
    MatrixAi2Gl(NodeTransformation, pNode->mTransformation);
     
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
    
    if (pNodeAnim)
    {
        // スケーリング
        Vector3 Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        Matrix4 ScalingM = Matrix4::CreateScale(Scaling);

        // 回転マトリックス
        Quaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        Matrix4 RotationM = Matrix4::CreateFromQuaternion(RotationQ);
    
        // 移動マトリックス
        Vector3 Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        Matrix4 TranslationM = Matrix4::CreateTranslation(Translation);

        // ローカルマトリックスを生成
        NodeTransformation =  RotationM * TranslationM * ScalingM;

    }

    
    
    // グローバルマトリックスを生成
    Matrix4 GlobalTransformation = NodeTransformation * ParentTransform * GlobalInverseTransform;
    
    if (boneMapping.find(NodeName) != boneMapping.end())
    {
        unsigned int BoneIndex = boneMapping[NodeName];
        boneInfo[BoneIndex].FinalTransformation =  boneInfo[BoneIndex].BoneOffset * GlobalTransformation;
    }

    for (unsigned int i = 0 ; i < pNode->mNumChildren ; i++)
    {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

// アニメーションデータを取得
const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
    for (unsigned int i = 0 ; i < pAnimation->mNumChannels ; i++)
    {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        
        if (std::string(pNodeAnim->mNodeName.data) == NodeName)
        {
            return pNodeAnim;
        }
    }
    return nullptr;
}


void Mesh::CalcInterpolatedPosition(Vector3& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1)
    {
        Out.Set(pNodeAnim->mPositionKeys[0].mValue.x,
                pNodeAnim->mPositionKeys[0].mValue.y,
                pNodeAnim->mPositionKeys[0].mValue.z);
        return;
    }
            
    unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    unsigned int NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);

    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    aiVector3D tr = Start + Factor * Delta;
    Out.Set(tr.x, tr.y, tr.z);
}


void Mesh::CalcInterpolatedRotation(Quaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1)
    {
        Out.Set(pNodeAnim->mRotationKeys[0].mValue.x,
                pNodeAnim->mRotationKeys[0].mValue.y,
                pNodeAnim->mRotationKeys[0].mValue.z,
                pNodeAnim->mRotationKeys[0].mValue.w);
        return;
    }
    
    unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    unsigned int NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    
    
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

    
    
    aiQuaternion q;
    aiQuaternion::Interpolate(q, StartRotationQ, EndRotationQ, Factor);
    q = q.Normalize();
    Out.Set(q.x, q.y, q.z, q.w);

}


void Mesh::CalcInterpolatedScaling(Vector3& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1)
    {
        Out.Set(pNodeAnim->mScalingKeys[0].mValue.x,
                pNodeAnim->mScalingKeys[0].mValue.y,
                pNodeAnim->mScalingKeys[0].mValue.z);
        return;
    }

    unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    unsigned int NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    aiVector3D sc = Start + Factor * Delta;
    Out.Set(sc.x, sc.y, sc.z);
}


unsigned int Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
        {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


unsigned int Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
        {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


unsigned int Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    
    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
        {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


// アニメーションデータ読み込み
void Mesh::LoadAnimation()
{
    numAnimations = pScene->mNumAnimations;
    durations.resize(numAnimations);
    for (int i = 0; i < numAnimations; i++)
    {
        durations[i] = (float)pScene->mAnimations[i]->mDuration;
        
    }
    
}

// ボーン読み込み
void Mesh::LoadBones(const aiMesh* m, std::vector<VertexBoneData>& bones)
{
    for (unsigned int i = 0 ; i < m->mNumBones ; i++)
    {
        unsigned int boneIndex = 0;
        std::string boneName(m->mBones[i]->mName.data);
        
        if (boneMapping.find(boneName) == boneMapping.end())
        {
            // Allocate an index for a new bone
            boneIndex = numBones;
            numBones++;
            BoneInfo bi;
            boneInfo.push_back(bi);
            
            // オフセットマトリックスのコピー
            Matrix4 mat;
            MatrixAi2Gl(mat, m->mBones[i]->mOffsetMatrix);
            boneInfo[boneIndex].BoneOffset = mat;
            boneMapping[boneName] = boneIndex;
        }
        else
        {
            boneIndex = boneMapping[boneName];
        }
        
        for (unsigned int j = 0 ; j < m->mBones[i]->mNumWeights ; j++)
        {
            unsigned int VertexID = m->mBones[i]->mWeights[j].mVertexId;

            float weight  = m->mBones[i]->mWeights[j].mWeight;
            bones[VertexID].AddBoneData(boneIndex, weight);
        }
    }
}

// Bone付き
void Mesh::CreateMeshBone(const aiMesh* m)
{

    std::vector<float> vertexBuffer;            // ３個ずつ（XYZ）
    std::vector<float> normalBuffer;            // ３個ずつ
    std::vector<float> uvBuffer;                // ２個ずつ
    std::vector<unsigned int> boneIDs;          // ４個ずつ
    std::vector<float> boneWeights;             // ４個ずつ
    std::vector<unsigned int> indexBuffer;
    
    std::vector<VertexBoneData> bones;
    // Boneバッファ
    bones.resize(m->mNumVertices);

    LoadBones(m, bones);
    
    for (unsigned int i = 0 ; i < m->mNumVertices ; i++)
    {
        vertexBuffer.push_back(m->mVertices[i].x);
        vertexBuffer.push_back(m->mVertices[i].y);
        vertexBuffer.push_back(m->mVertices[i].z);
            
        normalBuffer.push_back(-m->mNormals[i].x);
        normalBuffer.push_back(m->mNormals[i].y);
        normalBuffer.push_back(m->mNormals[i].z);

        if (m->HasTextureCoords(0))
        {
            uvBuffer.push_back(m->mTextureCoords[0][i].x);
            uvBuffer.push_back(m->mTextureCoords[0][i].y);
        }
        else
        {
            uvBuffer.push_back(0.0f);
            uvBuffer.push_back(0.0f);
        }

        boneIDs.push_back(bones[i].IDs[0]);
        boneIDs.push_back(bones[i].IDs[1]);
        boneIDs.push_back(bones[i].IDs[2]);
        boneIDs.push_back(bones[i].IDs[3]);
        
        boneWeights.push_back(bones[i].Weights[0]);
        boneWeights.push_back(bones[i].Weights[1]);
        boneWeights.push_back(bones[i].Weights[2]);
        boneWeights.push_back(bones[i].Weights[3]);
    }

    // インデックスバッファ取り出し
    for (unsigned int i = 0; i < m->mNumFaces; i++)
    {
        const aiFace& Face = m->mFaces[i];
        assert(Face.mNumIndices == 3);
        indexBuffer.push_back((Face.mIndices[0]));
        indexBuffer.push_back((Face.mIndices[1]));
        indexBuffer.push_back((Face.mIndices[2]));
    }
    
    
    VertexArray* va = new VertexArray(static_cast<unsigned int>(vertexBuffer.size()) / 3,
                                  vertexBuffer.data(),
                                  normalBuffer.data(),
                                  uvBuffer.data(),
                                  boneIDs.data(),
                                  boneWeights.data(),
                                  static_cast<unsigned int>(indexBuffer.size()),
                                  indexBuffer.data());
    
    va->SetTextureID(m->mMaterialIndex);
    
    vArray.push_back(va);
    
    LoadAnimation();
    
    

}

// メッシュをロード（Boneなし）
void Mesh::CreateMesh(const aiMesh* m)
{
    
    std::vector<float> vertexBuffer;            // ３個ずつ（XYZ）
    std::vector<float> normalBuffer;            // ３個ずつ
    std::vector<float> uvBuffer;                // ２個ずつ
    std::vector<unsigned int> indexBuffer;

    for (unsigned int i = 0 ; i < m->mNumVertices ; i++)
    {
        vertexBuffer.push_back(m->mVertices[i].x);
        vertexBuffer.push_back(m->mVertices[i].y);
        vertexBuffer.push_back(m->mVertices[i].z);
        
        normalBuffer.push_back(m->mNormals[i].x);
        normalBuffer.push_back(m->mNormals[i].y);
        normalBuffer.push_back(m->mNormals[i].z);

        if (m->HasTextureCoords(0))
        {
            uvBuffer.push_back(m->mTextureCoords[0][i].x);
            uvBuffer.push_back(m->mTextureCoords[0][i].y);
        }
        else
        {
            uvBuffer.push_back(0.0f);
            uvBuffer.push_back(0.0f);
        }
    }
    
    // インデックスバッファ取り出し
    for (unsigned int i = 0 ; i < m->mNumFaces ; i++)
    {

        const aiFace& Face = m->mFaces[i];
        assert(Face.mNumIndices == 3);
        indexBuffer.push_back(Face.mIndices[0]);
        indexBuffer.push_back(Face.mIndices[1]);
        indexBuffer.push_back(Face.mIndices[2]);
    }

    VertexArray* va = new VertexArray(static_cast<unsigned int>(vertexBuffer.size()) / 3,
                                      vertexBuffer.data(),
                                      normalBuffer.data(),
                                      uvBuffer.data(),
                                      static_cast<unsigned int>(indexBuffer.size()),
                                      indexBuffer.data());
    
    va->SetTextureID(m->mMaterialIndex);
    vArray.push_back(va);
    
}

// Assimpを使ったモデルデータロード
bool Mesh::Load(const std::string& fileName, class Renderer* r)
{
    
    // 読み込み（全データはaiSceneに格納される）
    pScene = importer.ReadFile(fileName.c_str(), ASSIMP_LOAD_FLAGS);
    if(!pScene)
    {
        return false;
    }


    MatrixAi2Gl(GlobalInverseTransform, pScene->mRootNode->mTransformation);
    GlobalInverseTransform.Invert();
    

    // 全メッシュ読み込み
    for (int cnt = 0; cnt < pScene->mNumMeshes; cnt++)
    {
        aiMesh* m = pScene->mMeshes[cnt];
        
        // ボーン情報有無
        if (m->HasBones())
        {
            CreateMeshBone(m);
            
        }
        else
        {
            CreateMesh(m);
        }
        
        
    }
    

    // テクスチャ読み込み
    for (unsigned int cnt = 0; cnt < pScene->mNumMaterials; cnt++){
    
        aiString Path;
        aiMaterial* pMaterial = pScene->mMaterials[cnt];
        Texture* t;
        if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
        {
            std::string texturefilename = Path.data;
            // テクスチャ読み込み
            t = r->GetTexture("Assets/" + texturefilename);
        }
        else
        {
            t = r->GetTexture("Assets/Default.png");
        }
        textures.push_back(t);
    }
    
    
    return true;
}


// データ破棄
void Mesh::Unload()
{
    pScene = nullptr;
    vArray.clear();
}

// テクスチャIDからGetter
Texture* Mesh::GetTexture(size_t index)
{
    if (index < textures.size())
    {
        return textures[index];
    }
    else
    {
        return nullptr;
    }
}





