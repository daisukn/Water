#pragma once
#include "CameraComponent.h"
#include "Math.h"

class OrbitCamera : public CameraComponent
{
public:
	OrbitCamera(class Actor* owner);

	void Update(float deltaTime) override;

    float GetPitchSpeed() const { return pitchSpeed; }
    float GetYawSpeed() const { return yawSpeed; }

    void SetPitchSpeed(float speed) { pitchSpeed = speed; }
    void SetYawSpeed(float speed) { yawSpeed = speed; }
    
    void ChangeHeight(float val) { changeOffset = val;}
    
    void ProcessInput( const struct InputState& state ) override;


private:
	// オフセット
	Vector3 offset;
	// Up vector（Y軸）
	Vector3 upVector;
    
    float changeOffset;
	// ピッチ回転のスピード（X軸）
	float pitchSpeed;
	// ヨー回転のスピード（U軸）
	float yawSpeed;

};
