#include "FollowActor.h"
#include "MeshComponent.h"
#include "Application.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "SkeletalMeshComponent.h"
#include "Mesh.h"
#include "InputSystem.h"
#include "BoundingVolumeComponet.h"
#include "FollowCamera.h"
#include "MoveComponent.h"
#include "ColliderComponent.h"



FollowActor::FollowActor(Application* app)
    : Actor(app)
    , animID(H_Stand)
    , bMovable(true)
    , prevPos(Vector3::Zero)
{
    // メッシュ初期化
	MeshComp = new SkeletalMeshComponent(this);
    MeshComp->SetMesh(app->GetRenderer()->GetMesh("Assets/hero_m.fbx"));
    MeshComp->SetAnimID(H_Run, PLAY_CYCLIC);
    
    MeshComp->SetToonRender(true);
    
    // 場所調整
    SetPosition(Vector3(0.0f, 300.0f, -0.0f));
    SetScale(0.01);
    
    // カメラ初期化
    cameraComp = new FollowCamera(this);
    
    // 移動コンポーネント
    moveComp = new MoveComponent(this);

    collComp = new ColliderComponent(this);
    collComp->SetColliderType(C_PLAYER);
    collComp->GetBoundingVolume()->ComputeBoundingVolume(app->GetRenderer()->GetMesh("Assets/hero_f.fbx")->GetVertexArray());
    collComp->GetBoundingVolume()->AdjustBoundingBox(Vector3(0, 0, 0), Vector3(0.4, 1, 0.3));
    collComp->GetBoundingVolume()->CreateVArray();

}

void FollowActor::FieldMove(const InputState &state)
{
    float forwardSpeed = 0.0f;
    float rightSpeed = 0.0f;
    float dropSpeed = 0.0f;
    float angularSpeed = 0.0f;
    
    
    animID = H_Stand;

    

    float speed = 70;

    
    
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_UP) == EHeld)
    {
        forwardSpeed += speed;
        animID = H_Run;
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_DOWN) == EHeld)
    {
        forwardSpeed -= speed;
        animID = H_Run;
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_LEFT) == EHeld)
    {
        angularSpeed -= 2.0f;
        animID = H_Run;
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_RIGHT) == EHeld)
    {
        angularSpeed += 2.0f;
        animID = H_Run;
    }
    
    
    if (!Math::NearZero(forwardSpeed))
    {
        cameraComp->SetHorzDist(40.0f);
    }
    else
    {
        cameraComp->SetHorzDist(40.0f);
    }
    
    
    MeshComp->SetAnimID(animID, PLAY_CYCLIC);
    

    
      
    // 地表を調整
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
      
      
}


void FollowActor::ApplyMotion()
{
    if (animID == H_Run || animID == H_Stand)
    {
        MeshComp->SetAnimID(animID, PLAY_CYCLIC);
    }
    else
    {
        MeshComp->SetAnimID(animID, PLAY_ONCE);
    }

}


void FollowActor::ActorInput(const InputState &state)
{
   
    FieldMove(state);
    

        
  
}

void FollowActor::UpdateActor(float deltaTime) {


    
    for (auto c : collComp->GetTargetColliders())
    {
        if (c->GetColliderType() == C_ENEMY)
        {
            std::cout << "いた！" << std::endl;
        }
        else if (c->GetColliderType() == C_NONE)
        {
            std::cout << "いたくない" << std::endl;
        }
    }
    
    
    prevPos = GetPosition();
 
}



void FollowActor::SetVisible(bool visible)
{
	MeshComp->SetVisible(visible);
}


