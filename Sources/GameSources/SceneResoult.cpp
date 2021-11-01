#include "SceneResoult.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneResoult.h"
#include "SceneSelect.h"
#include <DirectXMath.h>
#include "GameLibSource/FontManager.h"

void SceneResult::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    this->device = device;
    this->deviceContext = deviceContext;
    //ID3D11Device* device = Framework::GetInstance().GetDevice().Get();
    //cameraController = new CameraControl();
    //player = new PlayerTemp(this->device);
    //stageMain = new StageMain(this->device);
    //titleSprite = new Sprite(device, L"./Data/Sprite/title.png");
}

// 終了化
void SceneResult::Finalize()
{
    //delete player;
    //delete stageMain;
    //delete cameraController;
    //delete titleSprite;
}

// 更新処理
void SceneResult::Update(float elapsedTime)
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

    //マウス左クリックでマップ選択
    const MouseButton mouseClick =
        Mouse::BTN_LEFT;

    //if (gamePad.GetButtonDown() & anyButton) SceneManager::Instance().ChangeScene(new SceneTitle);
    //if (gamePad.GetButtonDown() & anyButton) SceneManager::Instance().ChangeScene(new SceneGame);
    if (screenPosition.x > 0 && screenPosition.x < 1280 &&
        screenPosition.y > 0 && screenPosition.y < 720)
    {
        if (mouse.GetButtonDown()) SceneManager::Instance().ChangeScene(new SceneSelect);
    }

}

#include <string>
// 描画処理
void SceneResult::Render(float elapsedTime)
{
    // モデル描画
    {
        //player->Render(deviceContext);
        //stageMain->Render(deviceContext, elapsedTime);
    }

    // 2D描画
    {
        int a = static_cast<int>(SceneManager::Instance().GetTimet());
       std::wstring aaaa = L"Time :" + std::to_wstring(a);

       FontManager::Instance().TexOut(
          // elapsedTime,
           { 0,0 },
           { 1980, 2000 },
           { aaaa.c_str() },
           aaaa.size()// 20
        );
        //titleSprite->Render(deviceContext, { 0, 0 }, { 1.0f,1.0f }, { 1280, 720 });
    }

    // デバック
    {
        //player->DebugImGui();
    }
}