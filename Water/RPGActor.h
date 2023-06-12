#pragma once

#include "Actor.h"


#ifndef __HEROMOTION
#define __HEROMOTION
enum HeroMotion
{
    H_Run = 11,
    H_Dead = 0,
    H_Guard = 1,
    H_Jump = 4,
    H_Stand = 17,
    H_Walk = 18,
    H_Slash = 13,
    H_Spin = 14,
    H_Stab = 15
    
};
#endif


// Followcameraを連れて歩くActor
class RPGActor : public Actor
{
public:
    RPGActor(class Application* app);

    void ActorInput(const struct InputState& state) override;
    void UpdateActor(float deltaTime) override;

    void SetVisible(bool visible);
    
    
    // 前方を取得（Z方向）
    Vector3 GetForward() override;
    // 右方を取得（X方向）
    Vector3 GetRight() override;
    
private:
    //  前フレームの場所
    Vector3 prevPos;
    
    // モーションID
    HeroMotion animID;
    
    class SkeletalMeshComponent* meshComp;
    class ColliderComponent* collComp;
    
    class ColliderComponent* attackArea;
    
    class OrbitCamera* cameraComp;
    class MoveComponent* moveComp;
    
    class ParticleComponent* partComp;
    
    
    // フィールド移動用
    void FieldMove(const struct InputState& state);
    // モーションを適用
    void ApplyMotion();
    
    // 動作可能=true 動作不可能=false
    bool bMovable;

};

