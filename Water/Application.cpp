#include "Application.h"
#include "Actor.h"
#include "Renderer.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "SkeletalMeshComponent.h"
#include "ParticleComponent.h"
#include "RPGActor.h"
#include "FollowActor.h"
#include "InputSystem.h"
#include "PhysWorld.h"
#include "WolfActor.h"
#include "FieldOvjectActor.h"

#include <algorithm>



#define WINDOW_TITLE "Water"
#define SCREEN_W 1280.0f
#define SCREEN_H 720.0f

// コンストラクタ
Application::Application()
    : bActive(false)
    , bUpdatingActors(false)
{
    renderer = new Renderer();
    inputSys = new InputSystem();
    physWorld = new PhysWorld();
}

// デストラクタ
Application::~Application()
{
    delete physWorld;
    
    delete inputSys;
    // Rendererのdeleteタイミング注意
    delete renderer;

}


// アプリ初期化
bool Application::Initialize()
{
    
    // SDL初期化
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "Failed to init SDL\n";
        return false;
    }
    
    // Renderer初期化
    renderer->Initialize(WINDOW_TITLE, SCREEN_W, SCREEN_H);

    // 入力システム初期化
    inputSys->Initialize();
    
    // データ ロード、主にRendererに登録されるもの。
    LoadData();
    
    bActive = true;
    ticksCount = SDL_GetTicks();
    return true;
}


// メインループ
void Application::RunLoop()
{
    while (bActive)
    {
        ProcessInput();
        UpdateGame();
        Draw();
    }
}

// 描画処理 Rendererが描画する
void Application::Draw()
{
    renderer->Draw();
}

// 終了処理
void Application::Shutdown()
{
    UnloadData();
    // Unloadしなくて良いか？
    inputSys->Shutdown();
    renderer->Shutdown();
    SDL_Quit();
}

// 入力受付
void Application::ProcessInput(){
    
    // 前の状態を保存
    inputSys->PrepareForUpdate();
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
                
        case SDL_QUIT:
            bActive = false;
            break;
        }
    }

    inputSys->Update();
    const InputState& state = inputSys->GetState();
        
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_ESCAPE) == EReleased)
    {
        bActive = false;
    }

    //UpdatingActors = true;

    //UpdatingActors = false;
    
    
    // 配下のActorsに渡す
    for (auto actor : actors)
    {
        actor->ProcessInput(state);
    }
}


// Actor追加
void Application::AddActor(Actor* actor)
{
    // メインのActorsがUpdate中はPendingに追加
    if (bUpdatingActors)
    {
        pendingActors.emplace_back(actor);
    }
    else
    {
        actors.emplace_back(actor);
    }
}

// Actor削除
void Application::RemoveActor(Actor* actor)
{
    // Pendingにないか確認
    auto iter = std::find(pendingActors.begin(), pendingActors.end(), actor);
    if (iter != pendingActors.end())
    {
        std::iter_swap(iter, pendingActors.end() - 1);
        pendingActors.pop_back();
    }

    // actorsにある場合は削除
    iter = std::find(actors.begin(), actors.end(), actor);
    if (iter != actors.end())
    {
        std::iter_swap(iter, actors.end() - 1);
        actors.pop_back();
    }
}


// データ解放
void Application::UnloadData()
{
    while (actors.empty())
    {
        delete actors.back();
    }

    if (renderer)
    {
        renderer->UnloadData();
    }
}

// Actors, Renderer関連
void Application::LoadData()
{
    
    RPGActor* ra = new RPGActor(this);
    //FollowActor* fa = new FollowActor(this);
    WolfActor* wa = new WolfActor(this);
    
    
    // 地面
    Actor* a = new Actor(this);
    MeshComponent* mc = new MeshComponent(a);
    mc->SetMesh(renderer->GetMesh("Assets/mountain.fbx"));
    //mc->SetVisible(false);
    mc->SetToonRender(true, 1.002f);
    
    
    
    physWorld->SetGroundVertex(renderer->GetMesh("Assets/mountain.fbx")->GetVertexArray()[0]);

    // パーティクルテスト用
    a = new Actor(this);
    ParticleComponent* pc = new ParticleComponent(a);
    pc->SetTexture(renderer->GetTexture("Assets/parts.jpg"));
    a->SetPosition(Vector3(100,480,0));
    pc->CreateParticles(Vector3(0,0,0), 80, 1000, 3.0f, 4.0f, false);

    // パーティクルテスト用
    a = new Actor(this);
    ParticleComponent* pc2 = new ParticleComponent(a);
    pc2->SetTexture(renderer->GetTexture("Assets/explosions.png"));
    //pc2->SetTexture(renderer->GetTexture("Assets/parts.jpg"));
    a->SetPosition(Vector3(100,480, -40));
    pc2->CreateParticles(Vector3(0,0,0), 80, 1000, 3.0f, 20.0f, true);

    
    
    FieldObjectActor* fo = new FieldObjectActor(this);
    fo->Create("Assets/house.x");
    fo->SetPosition(Vector3(-150, 440, -200));
    fo->SetScale(2.0f);
    //Quaternion q = Quaternion(Vector3::UnitY, 60);
    //fo->SetRotation(q);
    
    /*
    a = new Actor(this);
    MeshComponent* houseMesh = new MeshComponent(a);
    houseMesh->SetMesh(renderer->GetMesh("Assets/house.x"));
    houseMesh->SetToonRender(true);
    houseMesh->SetContourFactor(1.001f);
    a->SetPosition(Vector3(0, 440, 0));
    a->SetScale(2.0f);
    */
    
    
    
    
    // UI elements
    //Actor*
    a = new Actor(this);
    a->SetPosition(Vector3(-450.0f, -330.0f, 0.0f));
    SpriteComponent* sc = new SpriteComponent(a);
    sc->SetTexture(renderer->GetTexture("Assets/HealthBar.png"));
    

    // ライト
    renderer->SetAmbientLight(Vector3(0.8f, 0.8f, 0.8f));
    DirectionalLight& dir = renderer->GetDirectionalLight();
    dir.Direction = Vector3(0.7f, -7.0f, 7.0f);
    dir.DiffuseColor = Vector3(0.5f, 0.5f, 0.5f);
    dir.SpecColor = Vector3(0.5f, 0.5f, 0.5f);


    // カメラ設定
//    cameraAct = new CameraActor(this);
//    cameraAct->SetPosition(Vector3(0, 30, -150));
//    Quaternion q(Vector3::UnitY, -Math::PiOver2);
//    q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitY, Math::Pi + Math::Pi / 2.0f));
//    cameraAct->SetRotation(q);
 
}

// ゲームメインルーチン
void Application::UpdateGame()
{
    
    // FPS60固定
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksCount + 16))
        ;

    float deltaTime = (SDL_GetTicks() - ticksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }
    ticksCount = SDL_GetTicks();

    // 物理計算系
    physWorld->ComputeGroundHeight();
    physWorld->Test();
    
    
    // Actorsメイン呼び出し
    bUpdatingActors = true;
    for (auto a : actors)
    {
        a->Update(deltaTime);
    }
    bUpdatingActors = false;

    // Pendingがある場合はActorsに移動
    for (auto p : pendingActors)
    {
        p->ComputeWorldTransform();
        actors.emplace_back(p);
    }
    pendingActors.clear();

    // EDeadフラグのアクターは削除
    std::vector<Actor*> deadActors;
    for (auto a : actors)
    {
        if (a->GetState() == Actor::EDead)
        {
            deadActors.emplace_back(a);
        }
    }
    
    for (auto a : deadActors)
    {
        delete a;
    }

}
