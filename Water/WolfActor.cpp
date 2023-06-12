#include "WolfActor.h"
#include "Application.h"
#include "SkeletalMeshComponent.h"
#include "MoveComponent.h"
#include "Mesh.h"
#include "BoundingVolumeComponet.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "ColliderComponent.h"


WolfActor::WolfActor(Application* app)
    : Actor(app)
{
    
    meshComp = new SkeletalMeshComponent(this);
    meshComp->SetMesh(app->GetRenderer()->GetMesh("Assets/wolf.fbx"));
    meshComp->SetAnimID(2, PLAY_CYCLIC);
    

    SetPosition(Vector3(0.0f, 800.0f, 400.0f));
    SetScale(0.25);
    
    
    //app->GetPhysWorld()->AddActor(this);
    moveComp = new MoveComponent(this);


    // コライダー
    collComp = new ColliderComponent(this);
    collComp->SetColliderType(C_ENEMY);
    collComp->GetBoundingVolume()->ComputeBoundingVolume(app->GetRenderer()->GetMesh("Assets/wolf.fbx")->GetVertexArray());
     // 手動で調整
    collComp->GetBoundingVolume()->AdjustBoundingBox(Vector3(0.0f, 35, 30), Vector3(0.9, 0.9, 0.6));
    collComp->GetBoundingVolume()->CreateVArray();
    
    
    //boundingVolume = new BoundingVolumeComponent(this);
    //boundingVolume->ComputeBoundingVolume(app->GetRenderer()->GetMesh("Assets/wolf.fbx")->GetVertexArray());
    //boundingVolume->AdjustBoundingBox(Vector3(0.0f, 35, 30), Vector3(0.9, 0.9, 0.6));
    // デバッグ用に表示
    //boundingVolume->CreateVArray();

}


void WolfActor::UpdateActor(float deltaTime)
{
    float forwardSpeed = 0.0f;
    float angularSpeed = 0.0f;
    float dropSpeed = 0.0f;
    
    Vector3 v = GetPosition();
    if(v.y > collComp->GetGroundHeight() + 10)
    {
        dropSpeed -= 200;

    }
    else
    {
        SetPosition(Vector3(v.x, collComp->GetGroundHeight(), v.z));
    }
    
    moveComp->SetForwardSpeed(forwardSpeed);
    moveComp->SetAngularSpeed(angularSpeed);
    moveComp->SetDropSpeed(dropSpeed);
    

    SetPosition(GetPosition());
    SetRotation(GetRotation());
    SetScale(GetScale());
    
    
    
}


