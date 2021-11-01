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

// �I����
void SceneResult::Finalize()
{
    //delete player;
    //delete stageMain;
    //delete cameraController;
    //delete titleSprite;
}

// �X�V����
void SceneResult::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    // �Ȃɂ��{�^������������Q�[���V�[���؂�ւ�
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

    //�}�E�X���N���b�N�Ń}�b�v�I��
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
// �`�揈��
void SceneResult::Render(float elapsedTime)
{
    // ���f���`��
    {
        //player->Render(deviceContext);
        //stageMain->Render(deviceContext, elapsedTime);
    }

    // 2D�`��
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

    // �f�o�b�N
    {
        //player->DebugImGui();
    }
}