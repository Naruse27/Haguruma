#include "SceneGame.h"
#include "../GameLibSource/Framework.h"
#include "CameraManager.h"
#include "StageManager.h"
#include "Input/Input.h"
#include "GimmickManager.h"
#include "Stand.h"
#include "Gear.h"
#include "GimmickPoint.h"
#include "GameLibSource/Graphics/DebugRenderer.h"

#include "WoodenBox.h"

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
    player->SetCheckPointID(save->GetId());

    Stand* stand = new Stand(device);
    GimmickManager::Instance().Register(stand, Identity::Stand); 

    cameraController = new CameraControl();
    stageMain = new StageMain(this->device);
    stageWall = new StageWall(this->device);
    
    //titleSprite = new Sprite(device, L"./Data/Sprite/screenshot.jpg");
    StageManager::Instance().Register(stageMain);
    StageManager::Instance().Register(stageWall);

    fog.reset(new Fog(device));

    metaAi.reset(new Meta(player, &GimmickManager::Instance()));
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
        //titleSprite->Render(Framework::GetInstance().GetContext().Get(), { 0, 0 }, {1600, 900});
    }

    // デバック
    {
        player->DebugImGui();
        cameraController->RenderDebugGui(&CameraManager::Instance().mainView);
        GimmickManager::Instance().DebugRender();
    }
}