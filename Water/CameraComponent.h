#pragma once


#include "Component.h"
#include "Math.h"

// カメラ管理コンポーネント
class CameraComponent : public Component
{
public:
    // コンストラクタ
	CameraComponent(class Actor* owner, int updateOrder = 200);
    void Update(float deltaTime) override;

    
protected:
    // カメラの座標
    Vector3 CameraPosition;
    // ビューマトリックスを設定
	void SetViewMatrix(const Matrix4& view);
    
    
    // カメラの場所計算用
    class Actor* cameraActor;
    class ColliderComponent* collComp;
    
};
