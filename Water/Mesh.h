#pragma once

#include "Math.h"
#include "Animation.h"

#include <vector>
#include <string>
#include <map>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>




// Mesh管理（アニメーション対応）
class Mesh
{
public:
    Mesh();
    ~Mesh();

    // メッシュファイルロード
    virtual bool Load(const std::string& fileName, class Renderer* renderer);
    // アンロード
    void Unload();

    // VertexArrayを取得
    std::vector<class VertexArray*> GetVertexArray() { return vArray; }
    // Textureを取得
    class Texture* GetTexture(size_t index);
    // シェーダー名取得
    const std::string& GetShaderName() const { return shaderName; }
    // Specurer（未実装
    float GetSpecPower() const { return specPower; }
    
    // ボーン変形を反映
    void BoneTransform(float TimeInSeconds, std::vector<Matrix4>& Transforms);
    // モーションIDを設定
    void SetAnimID(int id, PlayMode m);//{ animID = id; mode = m; playTime = 0.0f; }
    
    // 再生中かどうか
    bool GetIsPlaying() { return bPlaying; }
    

private:

    // Assimpオブジェクト
    const aiScene* pScene;
    Assimp::Importer importer;
    
    // メッシュ生生(Boneなし)
    void CreateMesh(const aiMesh* m);
    // メッシュ生成（Boneあり)
    void CreateMeshBone(const aiMesh* m);
    // Bone情報読み込み
    void LoadBones(const aiMesh* m, std::vector<struct VertexBoneData>& bones);
    // Bone階層を辿る
    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4& ParentTransform);
    // アニメーション情報を取得
    const aiNodeAnim* FindNodeAnim (const aiAnimation* pAnimation, const std::string NodeName);

    // モーションID
    unsigned int animID;
    unsigned int prevAnimID;
    
    // 再生中かどうか
    bool bPlaying;
    
    // 再生中の時間
    float playTime;
    PlayMode mode;
    // モーションごとのフレーム数
    unsigned int numAnimations;
    std::vector<float> durations;
    void LoadAnimation();
    

    // フレームごとのBone姿勢を計算
    void CalcInterpolatedScaling (Vector3& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation (Quaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition (Vector3& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    
    unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);


    // Bone情報格納用
    std::map<std::string,unsigned int> boneMapping;
    // Bone数
    unsigned int numBones;
    // BoneとWeightを格納
    std::vector<struct BoneInfo> boneInfo;
    // 逆行列を記憶
    Matrix4 GlobalInverseTransform;

    
    // TextureのVector
    std::vector<class Texture*> textures;
    // VertexArrayのVector
    std::vector<class VertexArray*> vArray;

    // シェーダー名
    std::string shaderName;

    // Specurer
    float specPower;
    

};


