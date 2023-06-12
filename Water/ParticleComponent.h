#pragma once

#include "Component.h"
#include "Math.h"

#include <vector>

// パーティクルの破片
struct ParticleParts
{

    bool bVisible;
    Vector3 pos;
    Vector3 dir;
    float size;
    float lifeTime;

    
    ParticleParts()
        :pos(Vector3(0.0f, 0.0f, 0.0f))
        ,dir(Vector3(0.0f, 0.0f, 0.0f))
        ,bVisible(false)
        ,lifeTime(0.0f)
        ,size(0.0)

    {}
};


// パーティクルコンポーネント（SpriteComponentから継承した方が良いかもしれない）
class ParticleComponent : public Component
{
public:
    ParticleComponent(class Actor* ownner, int drawOrder = 100);
    ~ParticleComponent();
    
    // Rendererから呼ばれる
    virtual void Draw(class Shader* shader);

    // テクスチャセット
    virtual void SetTexture(class Texture* tex);
    
    // アップデート処理
    void Update(float deltaTime) override;

    // パーティクル発生
    void CreateParticles(Vector3 pos, unsigned int num, float life, float part_life, float size, bool grav = false);
    

private:
    // 所有アクターとの相対位置
    Vector3 position;
    bool bVisible;
    unsigned int numParts;
    float lifeTime;
    float totalLife;
    float partLifecycle;
    float partSize;

    bool bGrav;
    
    // パーティクル破片を生成
    void GenerateParts();

    std::vector<ParticleParts> parts;
    
    class Texture* texture;

}; 


