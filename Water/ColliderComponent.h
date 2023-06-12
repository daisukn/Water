#pragma once

#include "Component.h"

#include <vector>

// 通知される相手のタイプ
enum ColliderType
{
    C_NONE,     // 
    C_PLAYER,   // プレーヤー
    C_ENEMY,    // 敵
    C_ATTACK,
    C_BULLET,   // 弾丸
    C_OBSTACLE  // 障害物
};

class ColliderComponent : public Component
{
public:
    ColliderComponent(class Actor* a);
    ~ColliderComponent();

    // 自分のタイプ
    void SetColliderType(const ColliderType t) { type = t; }
    ColliderType GetColliderType() const { return type; }
    // 衝突した
    void Collided(ColliderComponent* c) { targetColliders.emplace_back(c); }
    std::vector<ColliderComponent*> GetTargetColliders() const { return targetColliders; }
    void ClearCollidBuffer() { targetColliders.clear(); }
    
    void Update(float deltaTime) override;
    
    class Actor* GetActor() { return owner; }
    class BoundingVolumeComponent* GetBoundingVolume() const { return boundingVolume; }
    
    
    // 地表の高さを設定
    void SetGroundHeight(const float f) { groundHeight = f; }
    float GetGroundHeight() const{ return groundHeight; }
    
    bool GetOnGround() const { return bOnGround; }
    void SetOnGround(bool b) { bOnGround = b; }
    
    
private:
    class BoundingVolumeComponent* boundingVolume;
    
    ColliderType type;
    std::vector<ColliderComponent*> targetColliders;
    
    float groundHeight;
    bool bOnGround;
};
