#include "OrbitCamera.h"
#include "Actor.h"
#include "ColliderComponent.h"
#include "InputSystem.h"

OrbitCamera::OrbitCamera(Actor* actor)
    : CameraComponent(actor)
    , offset(-0.0f, 30.0f, -50.0f)
    , upVector(Vector3::UnitY)
    , pitchSpeed(0.0f)
    , yawSpeed(0.0f)
{
}

void OrbitCamera::ProcessInput( const struct InputState& state )
{
    float angularSpeed = 2.0f;
    
    SetYawSpeed( angularSpeed * state.Controller.GetRightStick().x );
    ChangeHeight( angularSpeed * state.Controller.GetRightStick().y );


    // キーボード（カメラ）
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_D) == EHeld)
    {
        SetYawSpeed( angularSpeed );
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_A) == EHeld)
    {
        SetYawSpeed( -angularSpeed );
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_W) == EHeld)
    {
        ChangeHeight(angularSpeed);
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_S) == EHeld)
    {
        ChangeHeight(-angularSpeed);
    }
    
}

void OrbitCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);
	// ヨー回転のクォータニオン
	Quaternion yaw(Vector3::UnitY, yawSpeed * deltaTime);
	// オフセットを計算
    offset = Vector3::Transform(offset, yaw);
	upVector = Vector3::Transform(upVector, yaw);

	// カメラのforwardとrightを計算
	// Forward owner.position - (owner.position + offset) = -offset
    Vector3 forward = -1.0f * offset;
	forward.Normalize();
	Vector3 right = Vector3::Cross(upVector, forward);
	right.Normalize();
	
	// ピッチ回転のクォータニオン
	Quaternion pitch(right, pitchSpeed * deltaTime);
	// オフセットを計算
	offset = Vector3::Transform(offset, pitch);
	upVector = Vector3::Transform(upVector, pitch);

	// Ownerを向くビューマトリックスを生成
    Vector3 target = owner->GetPosition() + Vector3(0, 20, 0);
	Vector3 cameraPos = target + offset;
    
    
    offset.y += changeOffset;
    
    // 地表に合わせる
    if(cameraPos.y < collComp->GetGroundHeight() +15)
    {
        cameraPos.y = collComp->GetGroundHeight() +15;
        if( changeOffset < 0.0f)
        {
            offset.y -= changeOffset;
        }
        
    }
    CameraPosition = cameraPos;

	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, upVector);
	SetViewMatrix(view);
    
    
    cameraActor->SetPosition(cameraPos);
}
