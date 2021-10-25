#include "SceneSelect.h"
#include "SceneSelect2.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "Input/Input.h"
#include "../GameLibSource/Framework.h"
#include "CameraManager.h"

// 初期化
void SceneSelect2::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    this->device = device;
    this->deviceContext = deviceContext;
    //ID3D11Device* device = Framework::GetInstance().GetDevice().Get();
    //cameraController = new CameraControl();
    //player = new PlayerTemp(this->device);
    //stageMain = new StageMain(this->device);
    selectSprite = new Sprite(device, L"./Data/Sprite/select2.png");
    frameSprite = new Sprite(device, L"./Data/Sprite/frame.png");
}

// 終了化
void SceneSelect2::Finalize()
{
    //delete player;
    //delete stageMain;
    //delete cameraController;
    delete selectSprite;
    delete frameSprite;
}

// 更新処理
void SceneSelect2::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    // なにかボタンを押したらゲームシーン切り替え
    const GamePadButton anyButton =
        GamePad::BTN_A
        | GamePad::BTN_B
        | GamePad::BTN_BACK
        | GamePad::BTN_DOWN
        | GamePad::BTN_LEFT
        | GamePad::BTN_LEFT_SHOULDER
        | GamePad::BTN_LEFT_THUMB
        | GamePad::BTN_LEFT_TRIGGER
        | GamePad::BTN_RIGHT
        | GamePad::BTN_RIGHT_SHOULDER
        | GamePad::BTN_RIGHT_THUMB
        | GamePad::BTN_RIGHT_TRIGGER
        | GamePad::BTN_START
        | GamePad::BTN_UP
        | GamePad::BTN_X
        | GamePad::BTN_Y;

    Mouse& mouse = Input::Instance().GetMouse();
    DirectX::XMFLOAT3 screenPosition;
    screenPosition.x = static_cast<float>(mouse.GetPositionX());
    screenPosition.y = static_cast<float>(mouse.GetPositionY());

    /*const MouseButton mouseClick =
        Mouse::BTN_LEFT
        | Mouse::BTN_RIGHT;*/

    //if (gamePad.GetButtonDown() & anyButton) SceneManager::Instance().ChangeScene(new SceneTitle);
    //if (gamePad.GetButtonDown() & anyButton) SceneManager::Instance().ChangeScene(new SceneGame);
    if (screenPosition.x > 130 && screenPosition.x < 230 &&
        screenPosition.y > 340 && screenPosition.y < 440)
    {
        if (mouse.GetButtonDown() & Mouse::BTN_LEFT) SceneManager::Instance().ChangeScene(new SceneSelect);
    }
    //if (mouse.GetButtonDown() & Mouse::BTN_RIGHT) SceneManager::Instance().ChangeScene(new SceneGame);
    if (mouse.GetButtonDown() & Mouse::BTN_MIDDLE) SceneManager::Instance().ChangeScene(new SceneGame);

    //// カメラコントローラー更新
    //DirectX::XMFLOAT3 target = player->GetPosition();
    //target.y += 0.5f;
    //cameraController->SetTarget(target);
    //cameraController->Update(elapsedTime, &CameraManager::Instance().mainView);
    //
    //player->Update(elapsedTime);
    //stageMain->Update(elapsedTime);
}

// 描画処理
void SceneSelect2::Render(float elapsedTime)
{
    // モデル描画
    {
        //player->Render(deviceContext);
        //stageMain->Render(deviceContext, elapsedTime);
    }

    // 2D描画
    {
        selectSprite->Render(deviceContext, { 0, 0 }, {1.0f, 1.0f}, { 1280, 720 });
    }

    // デバック
    {
        //player->DebugImGui();
    }
}
