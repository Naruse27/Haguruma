#include "SceneGame.h"
#include "SceneManager.h"
#include "../GameLibSource/Framework.h"
#include "CameraManager.h"
#include "StageManager.h"
#include "Input/Input.h"
#include "GimmickManager.h"
#include "Stand.h"
#include "Gear.h"
#include "GimmickPoint.h"
#include "GameLibSource/Graphics/DebugRenderer.h"
<<<<<<< HEAD
#include "SceneResoult.h"
#include "SceneSelect.h"
=======

#include "WoodenBox.h"
>>>>>>> 070702319ab3ccce4f6a49339f966afc094a1dd4

// 初期化
void SceneGame::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    this->device = device;
    this->deviceContext = deviceContext;

    DebugRenderer::Create();
    DebugRenderer::Instance().Init(this->device);

    //ID3D11Device* device = Framework::GetInstance().GetDevice().Get();
    StageManager::Create();
    GimmickPoint* point = new GimmickPoint(device, Identity::Start, false);
    point->SetPosition({ 0,0,15 });
    GimmickManager::Instance().Register(point, Identity::Start);
    GimmickPoint* save = new GimmickPoint(device, Identity::Save, false);
    save->SetPosition({ -16, 0, 123 });
    GimmickManager::Instance().Register(save, Identity::Save);
    GimmickPoint* goal = new GimmickPoint(device, Identity::End, true);
    goal->SetPosition({ 0,0, 230 });
    GimmickManager::Instance().Register(goal, Identity::End);


    player = new Player(this->device);
    player->SetPosition(point->GetPosition());
    player->SetStartGimmickID(point->GetId());
<<<<<<< HEAD
    startId = point->GetId();

=======
>>>>>>> 070702319ab3ccce4f6a49339f966afc094a1dd4
    player->SetCheckPointID(save->GetId());

    Stand* stand = new Stand(device);
    GimmickManager::Instance().Register(stand, Identity::Stand); 

    cameraController = new CameraControl();
    stageMain = new StageMain(this->device);
    //stageWall = new StageWall(this->device);
    
    //titleSprite = new Sprite(device, L"./Data/Sprite/screenshot.jpg");
    StageManager::Instance().Register(stageMain);
    //StageManager::Instance().Register(stageWall);

    fog.reset(new Fog(device));

    metaAi.reset(new Meta(player, &GimmickManager::Instance()));


    SceneManager::Instance().TimerClear();
    blender.reset(new Blender(device));

    UiSetting();
}

// 終了化
void SceneGame::Finalize()
{
    delete player;
    delete cameraController;
    delete stageMain;
    delete stageWall;
    //StageManager::Instance().Clear();
    GimmickManager::Instance().Clear();

    StageManager::Destory();

    DebugRenderer::Destory();
    //delete titleSprite;
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();
    if (mouse.GetButtonDown() & Mouse::BTN_MIDDLE) {
        select = !select;
    }

    if (select) {
        UIupdate();
        return;
    }
    SceneManager::Instance().TimerAdd(elapsedTime);

    // カメラコントローラー更新
    Vector3 target = player->GetPosition();
    target.y += player->GetHeight() + 2.0f;
    cameraController->SetTarget(target);
    cameraController->Update(elapsedTime, &CameraManager::Instance().mainView);

    player->Update(elapsedTime);
    GimmickManager::Instance().Update(elapsedTime);
    StageManager::Instance().Update(elapsedTime);
    StageManager::Instance().getGimmickState();
    

    player->MouseRay(device, deviceContext, CameraManager::Instance().mainView.GetView(), CameraManager::Instance().mainView.GetProjection());
    //stageMain->Update(elapsedTime);
    //stageWall->Update(elapsedTime);
    metaAi->Update();

    if (player->GetGoalPoint())SceneManager::Instance().ChangeScene(new SceneResult);
}

// 描画処理
void SceneGame::Render(float elapsedTime)
{
    // モデル描画
    {
        fog->UpdateRendrer(deviceContext);

        player->Render(deviceContext);
        StageManager::Instance().Render(deviceContext, elapsedTime);
        GimmickManager::Instance().Render(deviceContext, elapsedTime);
        //stageMain->Render(deviceContext, elapsedTime);
        //stageWall->Render(deviceContext, elapsedTime);
    }

    // 2D描画
    {
        if (select) {
            blender->SetBlendMode(Blender::BLEND_STATE::BS_ALPHA, deviceContext);
            for (auto& u : ui) u.uiSpr->render(deviceContext, u.pos, u.scale, { 0,0 }, u.size, u.pivot, 0, { 1.0f, 1.0f, 1.0f, 1.0f });
            blender->SetBlendMode(Blender::BLEND_STATE::BS_NONE, deviceContext);
        }
    }

    // デバック
    {
        player->DebugImGui();
        cameraController->RenderDebugGui(&CameraManager::Instance().mainView);
        GimmickManager::Instance().DebugRender();
<<<<<<< HEAD
    }
}

void SceneGame::Reset()
{
    Gimmick* g = GimmickManager::Instance().GetGimmickFromId(startId);
    player->Init();
    player->SetStartGimmickID(startId);
    player->SetPosition(g->GetPosition());
    player->SetCheckPointFlag(false);
}

void SceneGame::UIupdate()
{
    Mouse& mouse = Input::Instance().GetMouse();

    DirectX::XMFLOAT2 screenPosition;
    screenPosition.x = static_cast<float>(mouse.GetPositionX());
    screenPosition.y = static_cast<float>(mouse.GetOldPositionY());

    int num = 0;

    for (auto& u : ui) {
        Vector2 pos = u.pos - u.pivot;
        if (Collision::PointVsRect(screenPosition, pos, u.size)) {
            u.scale = { 1.2f, 1.2f };
            if (mouse.GetButtonDown() & Mouse::BTN_LEFT) UiCheng(num);
        }
        else {
            u.scale = { 1.0f, 1.0f };
        }
        num++;
    }
}

void SceneGame::UiSetting()
{
    //CONST LONG Screen_WIDTH = 1920;
    //CONST LONG Screen_HEIGHT = 1080;

    ui[0].uiSpr.reset(new Sprite(device, L"./Data/Sprite/back.png", 111));
    ui[1].uiSpr.reset(new Sprite(device, L"./Data/Sprite/start.png", 111));
    ui[2].uiSpr.reset(new Sprite(device, L"./Data/Sprite/suteru.png", 111));

    ui[0].pos = { SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.2f };
    ui[1].pos = { SCREEN_WIDTH * 0.3f, SCREEN_WIDTH * 0.3f };
    ui[2].pos = { SCREEN_WIDTH * 0.7f, SCREEN_WIDTH * 0.3f };

    ui[0].size = { 222, 126 };
    ui[0].pivot = { 111, 63 };
    ui[1].size = { 256, 96 };
    ui[1].pivot = { 128, 48 };
    ui[2].size = { 320,  126 };
    ui[2].pivot = { 160, 63 };

    for (auto& u : ui) u.scale = { 1.0f, 1.0f };
    
}

void SceneGame::UiCheng(int num)
{
    switch (num)
    {
    case 0: select = false; break;
    case 1:
        Reset();
        select = false;
        break;
    case 2: SceneManager::Instance().ChangeScene(new SceneSelect); break;
=======
>>>>>>> 070702319ab3ccce4f6a49339f966afc094a1dd4
    }
}