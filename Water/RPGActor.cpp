#include "RPGActor.h"
#include "MeshComponent.h"
#include "Application.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "SkeletalMeshComponent.h"
#include "Mesh.h"
#include "InputSystem.h"
#include "BoundingVolumeComponet.h"
#include "OrbitCamera.h"
#include "MoveComponent.h"
#include "ColliderComponent.h"
#include "ParticleComponent.h"
#include "Polygon.h"



RPGActor::RPGActor(Application* app)
    : Actor(app)
    , animID(H_Stand)
    , bMovable(true)
    , prevPos(Vector3::Zero)
{
    // メッシュ初期化
    meshComp = new SkeletalMeshComponent(this);
    meshComp->SetMesh(app->GetRenderer()->GetMesh("Assets/hero_f.fbx"));
    meshComp->SetAnimID(H_Run, PLAY_CYCLIC);
//    meshComp->SetToonRender(true,1.00001f);
    meshComp->SetToonRender(false,0.0f);

    //meshComp->SetToonRender(true);
    
    // 場所調整
    SetPosition(Vector3(0.0f, 600.0f, -50.0f));
    SetScale(0.01);
    
    // カメラ初期化
    cameraComp = new OrbitCamera(this);
    
    // 移動コンポーネント
    moveComp = new MoveComponent(this);

    collComp = new ColliderComponent(this);
    collComp->SetColliderType(C_PLAYER);
    collComp->GetBoundingVolume()->ComputeBoundingVolume(app->GetRenderer()->GetMesh("Assets/hero_f.fbx")->GetVertexArray());
    collComp->GetBoundingVolume()->AdjustBoundingBox(Vector3(0, 0, 0), Vector3(0.4, 1, 0.3));
    collComp->GetBoundingVolume()->CreateVArray();

    // 攻撃が届く範囲を定義
    attackArea = new ColliderComponent(this);
    attackArea->GetBoundingVolume()->ComputeBoundingVolume(Vector3(-500, 0, 100), Vector3(500, 2000, 2000));
    attackArea->GetBoundingVolume()->CreateVArray();
    
    // パーティクルエフェクト
    Actor* a = new Actor(app);
    partComp = new ParticleComponent(a);
    partComp->SetTexture(app->GetRenderer()->GetTexture("Assets/explosions.png"));
    
    
}

void RPGActor::FieldMove(const InputState &state)
{
    float forwardSpeed = 0.0f;
    float rightSpeed = 0.0f;
    float dropSpeed = 0.0f;
    
    
    float speed = 70.0f;

    

    

      
      
    if(bMovable)
    {
        forwardSpeed = speed * state.Controller.GetLeftStick().y;
        rightSpeed = speed * state.Controller.GetLeftStick().x;
        
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_LEFT) == EHeld)
        {
            rightSpeed = -speed;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_RIGHT) == EHeld)
        {
            rightSpeed = speed;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_UP) == EHeld)
        {
            forwardSpeed = speed;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_DOWN) == EHeld)
        {
            forwardSpeed = -speed;
        }
        animID = H_Run;
        if ( forwardSpeed == 0.0f && rightSpeed == 0.0f )
            animID = H_Stand;

    }
    

    
    if(bMovable)
    {
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_Z) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_A) == EPressed )
        {
            bMovable = false;
            animID = H_Jump;

        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_X) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_B) == EPressed )
        {
            bMovable = false;
            animID = H_Slash;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_C) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_X) == EPressed )
        {
            bMovable = false;
            animID = H_Spin;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_V) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_Y) == EPressed )
        {
            bMovable = false;
            animID = H_Stab;
        }
        
    }
    else
    {
        if ( !meshComp->GetIsPlaing() )
        {
            if (animID == H_Slash)
            {
                Vector3 v = GetPosition() + Vector3::Transform(Vector3::UnitZ, GetRotation()) * 20;
                v += Vector3::Transform(Vector3::UnitY, GetRotation()) * 20;
                partComp->GetOwner()->SetPosition(v);
                partComp->CreateParticles(Vector3(0,0,0), 10, 0.8f, 0.5f, 20.0f, false);

            }
            bMovable = true;
        }

    }


    // 地表を調整
    Vector3 v = GetPosition();
    if (v.y > collComp->GetGroundHeight() + 10)
    {
        dropSpeed -= 200;

    }
    else
    {
        SetPosition(Vector3(v.x, collComp->GetGroundHeight(), v.z));
    }

    moveComp->SetForwardSpeed(forwardSpeed);
    moveComp->SetRightSpeed(rightSpeed);
    moveComp->SetDropSpeed(dropSpeed);
      
      
}


void RPGActor::ApplyMotion()
{
    if (animID == H_Run || animID == H_Stand)
    {
        meshComp->SetAnimID(animID, PLAY_CYCLIC);
    }
    else
    {
        meshComp->SetAnimID(animID, PLAY_ONCE);
    }

}


void RPGActor::ActorInput(const InputState &state)
{
   
    FieldMove(state);
    

        
  
}

void RPGActor::UpdateActor(float deltaTime) {

    ApplyMotion();

    
    // 移動ベクトルから向きを調整
    Vector3 moveVal = GetPosition() - prevPos;
    if(moveVal.x != 0.0f || moveVal.z != 0.0f){
        
        float rot = Math::Atan2(moveVal.x, moveVal.z);
        
        Quaternion q = Quaternion(Vector3::UnitY, rot);
        
        SetRotation(q);
        
        if ( !collComp->GetOnGround() )
        {
            SetPosition(prevPos);
        }
        
    }


    
    for (auto c : collComp->GetTargetColliders())
    {
        if (c->GetColliderType() ==  C_ENEMY)
        {
            std::cout << "ウルフに当たりました" << std::endl;

            SetPosition(prevPos);
        }
        else if (c->GetColliderType() == C_OBSTACLE)
        {
//            std::cout << "障害物に当たりました" << std::endl;

            //SetPosition(prevPos);

            /* 壁ズリ実験*/

            Polygon* p = c->GetBoundingVolume()->GetPlygons();

            
            Vector3 v1 = p[2].b - p[2].a;
            Vector3 v2 = p[2].c - p[2].b;
            Vector3 norm = Vector3::Cross(v1, v2);
            norm.Normalize();

            Vector3 pos = GetPosition() + norm *70.0f*0.015f;
            SetPosition( Vector3(pos.x, GetPosition().y, pos.z));
            
            
            
            
        }
        else
        {
            std::cout << "ウルフにあたってません" << std::endl;
        }
    }
    
/*
    
    if(moveVal.y > 2)
    {
        if( Math::NearZero(GetPosition().y - collComp->GetGroundHeight()))
        {
            SetPosition(prevPos);

        }
        std::cout << moveVal.y << std::endl;
    }
  */
    
    
    prevPos = GetPosition();
}



void RPGActor::SetVisible(bool visible)
{
    meshComp->SetVisible(visible);
}


// 前方を取得（Z方向）
Vector3 RPGActor::GetForward()
{
    return GetApp()->GetRenderer()->GetInvViewMatrix().GetZAxis();
}
// 右方を取得（X方向）
Vector3 RPGActor::GetRight()
{
   return GetApp()->GetRenderer()->GetInvViewMatrix().GetXAxis();
}
