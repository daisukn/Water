#pragma once

#include "Component.h"
#include "Math.h"

class MoveComponent : public Component
{
    
public:
    MoveComponent(class Actor* owner, int updateOrder = 10);
    void Update(float deltaTime) override;
    
    // Getter/Setter
    float GetAngularSpeed() const { return angularSpeed; }
    float GetForwardSpeed() const { return forwardSpeed; }
    float GetDropSpeed() const { return dropSpeed; }
    void SetAngularSpeed(float speed) { angularSpeed = speed; }
    void SetForwardSpeed(float speed) { forwardSpeed = speed; }
    void SetRightSpeed(float speed) { rightSpeed = speed;}
    void SetDropSpeed(float speed) { dropSpeed = speed; }


private:
    float angularSpeed;     // 回転スピード
    float forwardSpeed;     // 移動スピード
    float rightSpeed;       // 横移動スピード
    float dropSpeed;        // 落下スピード
    

};
