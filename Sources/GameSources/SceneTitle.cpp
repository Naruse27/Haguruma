#include "SceneTitle.h"
#include "../GameLibSource/Framework.h"
#include "CameraManager.h"

// 初期化
void SceneTitle::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    this->device = device;
    this->deviceContext = deviceContext;
    //ID3D11Device* device = Framework::GetInstance().GetDevice().Get();
    cameraController = new CameraControl();
    player = new PlayerTemp(this->device);
    stageMain = new StageMain(this->device);
    //titleSprite = new Sprite(device, L"./Data/Sprite/screenshot.jpg");
}

// 終了化
void SceneTitle::Finalize()
{
    delete player;
    delete stageMain;
    delete cameraController;
    //delete titleSprite;
}

// 更新処理
void SceneTitle::Update(float elapsedTime)
{
    // カメラコントローラー更新
    DirectX::XMFLOAT3 target = player->GetPosition();
    target.y += 0.5f;
    cameraController->SetTarget(target);
    cameraController->Update(elapsedTime, &CameraManager::Instance().mainView);

    player->Update(elapsedTime);
    stageMain->Update(elapsedTime);
}

// 描画処理
void SceneTitle::Render(float elapsedTime)
{
    // モデル描画
    {
        player->Render(deviceContext);
        stageMain->Render(deviceContext, elapsedTime);
    }

    // 2D描画
    {
        //titleSprite->Render(Framework::GetInstance().GetContext().Get(), { 0, 0 }, {1600, 900});
    }

    // デバック
    {
        //player->DebugImGui();
    }
}