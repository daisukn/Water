#pragma once

#include "CameraComponent.h"


// Actorについていくカメラ
class FollowCamera : public CameraComponent
{
public:
    // コンストラクタ
	FollowCamera(class Actor* owner);

    // Override
	void Update(float deltaTime) override;
    // 理想の位置に向けて調整
	void SnapToIdeal();

    // カメラ位置の設定
    void SetHorzDist(float dist) { horzDist = dist; }
    void SetVertDist(float dist) { vertDist = dist; }
    void SetTargetDist(float dist) { targetDist = dist; }
    void SetSpringConstant(float spring) { springConstant = spring; }
    //const Vector3 GetActualPos () const { return actualPos; }
    
private:
    // カメラの位置を計算
	Vector3 ComputeCameraPos() const;
    // 現在の位置
	Vector3 actualPos;
    // 速度
	Vector3 velocity;
    
    // カメラ位置
    // 後ろ
	float horzDist;
    // 高さ
	float vertDist;
    // 視点とActorの距離
	float targetDist;
    
    // バネ定数
	float springConstant;
};

