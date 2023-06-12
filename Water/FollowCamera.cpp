
#include "FollowCamera.h"
#include "Actor.h"
#include "ColliderComponent.h"

FollowCamera::FollowCamera(Actor* owner)
    : CameraComponent(owner)
    , horzDist(100.0f)        // 所有アクターとの距離
    , vertDist(20.0f)        // 所有アクターとの距離（高さ）
    , targetDist(50.0f)      // 視点は所有アクターよりどのくらい前か
    , springConstant(16.0f)  // バネ定数
{
    
}

void FollowCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);
    CameraPosition = actualPos;
    
	// バネ定数から減衰を計算
    float dampening = 2.0f * Math::Sqrt(springConstant);
    
	// 理想の位置
	Vector3 idealPos = ComputeCameraPos();
	// 実際の位置と理想の位置の差
    Vector3 diff = actualPos - idealPos;
	// バネによる加速度を計算
    Vector3 acel = -springConstant * diff - dampening * velocity;
	// 加速度を更新
    velocity += acel * deltaTime;
	// カメラの場所を更新
    actualPos += velocity * deltaTime;
	// ターゲットは所有アクターの前方
    Vector3 target = owner->GetPosition() + owner->GetForward() * targetDist;
    
    
    // 地表に合わせる
    if(actualPos.y < collComp->GetGroundHeight() +15)
    {
        actualPos.y = collComp->GetGroundHeight() +15;
    }
	// ビューマトリックスを生成
    Matrix4 view = Matrix4::CreateLookAt(actualPos, target, Vector3::UnitY);
	SetViewMatrix(view);
    
    cameraActor->SetPosition(CameraPosition);
}

// 理想の位置に向けて調整
void FollowCamera::SnapToIdeal()
{
	// 実際の位置
    actualPos = ComputeCameraPos();
	// 速度ゼロ
    velocity = Vector3::Zero;
	// ターゲットとビューを計算
    Vector3 target = owner->GetPosition() + owner->GetForward() * targetDist;
	// ビューマトリックスを生成
    Matrix4 view = Matrix4::CreateLookAt(actualPos, target, Vector3::UnitY);
	SetViewMatrix(view);
}

Vector3 FollowCamera::ComputeCameraPos() const
{
	// カメラの位置を所有アクターの上のほうにセット
	Vector3 cameraPos = owner->GetPosition();
    cameraPos -= owner->GetForward() * horzDist;
    cameraPos += Vector3::UnitY * vertDist;
	return cameraPos;
}
