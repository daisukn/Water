#include "ParticleComponent.h"
#include "Actor.h"
#include "Shader.h"
#include "Texture.h"
#include "Application.h"
#include "Renderer.h"
#include "VertexArray.h"

#include <vector>
#include <random>



// コンストラクタ
ParticleComponent::ParticleComponent(Actor* a, int drawOrder)
    : Component(a)
    , bVisible(false)
    , position(Vector3(0.0f, 0.0f, 0.0f))
    , numParts(0)
    , lifeTime(0.0f)
    , totalLife(0.0f)
    , partLifecycle(0.0f)
    , partSize(0.0f)
    , bGrav(false)
{
    owner->GetApp()->GetRenderer()->AddParticleComp(this);
}
// デストラクタ
ParticleComponent::~ParticleComponent()
{
    // レンダラーからRemoveする
    parts.clear();
    owner->GetApp()->GetRenderer()->RemoveParticleComp(this);
}

// 描画（Rendererから呼ばれる）
void ParticleComponent::Draw(Shader *shader)
{
    if (texture)
    {
        // Ownerのマトリックスを取得（Positionでも良いかもしれない。）
        Matrix4 mat = owner->GetWorldTransform();
        
        // ビルボーディング
        // ビューマトリックスの逆行列の座標を差し替えて流用
        Matrix4 invVew = owner->GetApp()->GetRenderer()->GetInvViewMatrix();
        // 座標差し替え
        invVew.mat[3][0] = mat.mat[3][0];
        invVew.mat[3][1] = mat.mat[3][1];
        invVew.mat[3][2] = mat.mat[3][2];
        
        // スケールを復元
        Matrix4 scaleMat = Matrix4::CreateScale(partSize, partSize, 1);
        Matrix4 world = scaleMat * invVew;

        // シェーダー に送る
        shader->SetMatrixUniform("uWorldTransform", world);
        texture->SetActive();
        
        for (int i = 0; i < numParts; i++)
        {
            if(parts[i].bVisible)
            {
                shader->SetVectorUniform("uPosition", parts[i].pos);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            }
        }
    }
    
}

// テクスチャ設定
void ParticleComponent::SetTexture(Texture *tex)
{
    texture = tex;
}

// パーツを生成
void ParticleComponent::GenerateParts()
{
    
    // 乱数初期化
    std::random_device rnd;
    
    if (bVisible)
    {
    
        for (int i = 0; i < numParts; i++)
        {
            if (!parts[i].bVisible)
            {
                // スピードの調整がちょっと強引なので再考する
                //float x  = (float)(rand10(mt) - 5) / 10;
                //float y  = (float)(rand10(mt) - 5) / 10;
                //float z  = (float)(rand10(mt) - 5) / 10;
                float x  = (float)(rnd()%5) / 10;
                float y  = (float)(rnd()%5) / 10;
                float z  = (float)(rnd()%5) / 10;

                // パーツのパラメータをセット
                parts[i].pos = position;
                parts[i].dir = Vector3(x, y, z);
                parts[i].bVisible = true;
                parts[i].lifeTime = 0.0f;
                parts[i].size = partSize;
                
                break;
            }
        }
    }
}

// Update（Actorから呼ばれる）
void ParticleComponent::Update(float deltaTime)
{
    lifeTime += deltaTime;
    
    // 時間経過で消える
    if (lifeTime > totalLife)
    {
        bVisible = false;
    }
    
    
    // パーツの位置を進める
    for (int i = 0; i < numParts; i++)
    {
         
        if(parts[i].bVisible)
        {
            // 重力を反映
            if (bGrav)
            {
                parts[i].dir.y -= 0.05f;
            }
            
            
            parts[i].lifeTime += deltaTime;
            parts[i].pos += parts[i].dir * deltaTime;
        
            if(parts[i].lifeTime > partLifecycle)
            {
                parts[i].bVisible = false;
            }
        }
    }
    
    std::random_device rnd;
    if(rnd()%2 == 0)
    {
        // パーツを追加（乱数で回数を間引くのもあり）
        GenerateParts();
    }
}

// パーティクル初期化
//（アクターとの相対位置、粒の総数、総表示時間、粒の表示時間、粒の大きさ、重力モード（未実装））
void ParticleComponent::CreateParticles(Vector3 pos, unsigned int num, float life, float part_life, float size, bool grav)
{
    // すでに表示されていたらキャンセル
    if(!bVisible)
    {
        position = pos;
        bVisible = true;
        numParts = num;
        lifeTime = 0.0f;
        totalLife = life;
        partLifecycle =  part_life;
        partSize = size;
        
        
        parts.resize(numParts);
        //GenerateParts();
        
        bGrav = grav;
    }
    

}
