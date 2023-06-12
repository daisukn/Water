#include "MoveComponent.h"
#include "Actor.h"

// コンストラクタ
MoveComponent::MoveComponent(class Actor* a, int updateOrder)
    : Component(a, updateOrder)
    , angularSpeed(0.0f)
    , forwardSpeed(0.0f)
    , rightSpeed(0.0f)
    , dropSpeed(0.0f)
{
	
}

// マトリックスに適用する
void MoveComponent::Update(float deltaTime)
{
    // 向きを決める　スピードがほぼゼロなら無視
    if (!Math::NearZero(angularSpeed))
	{
        // 回転マトリックスを設定
        Quaternion rot = owner->GetRotation();
        float angle = angularSpeed * deltaTime;

		Quaternion inc(Vector3::UnitY, angle);
		rot = Quaternion::Concatenate(rot, inc);
        owner->SetRotation(rot);
	}
	
    
    // スピードがセットされていたら
    if ( !Math::NearZero(forwardSpeed) )
	{
        // 移動マトリックスを設定
        Vector3 pos = owner->GetPosition();
        pos += owner->GetForward() * forwardSpeed * deltaTime;

        owner->SetPosition(pos);
        
	}
    
    // スピードがセットされていたら
    if ( !Math::NearZero(rightSpeed) )
    {
        // 移動マトリックスを設定
        Vector3 pos = owner->GetPosition();
        pos += owner->GetRight() * rightSpeed * deltaTime;

        owner->SetPosition(pos);
        
    }
    
    
    if ( !Math::NearZero(dropSpeed) )
    {
        // 移動マトリックスを設定
        Vector3 pos = owner->GetPosition();
        pos += owner->GetUpward() * dropSpeed * deltaTime;
        owner->SetPosition(pos);
    }
}
