#include "SceneTitle.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "SceneSelect.h"
#include "Input/Input.h"
#include "../GameLibSource/Framework.h"
#include "CameraManager.h"

// ������
void SceneTitle::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    this->device = device;
    this->deviceContext = deviceContext;
    //ID3D11Device* device = Framework::GetInstance().GetDevice().Get();
    //cameraController = new CameraControl();
    //player = new PlayerTemp(this->device);
    //stageMain = new StageMain(this->device);
    titleSprite = new Sprite(device, L"./Data/Sprite/title.png");
}

// �I����
void SceneTitle::Finalize()
{
    //delete player;
    //delete stageMain;
    //delete cameraController;
    delete titleSprite;
}

// �X�V����
void SceneTitle::Update(float elapsedTime)
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

    //// �J�����R���g���[���[�X�V
    //DirectX::XMFLOAT3 target = player->GetPosition();
    //target.y += 0.5f;
    //cameraController->SetTarget(target);
    //cameraController->Update(elapsedTime, &CameraManager::Instance().mainView);
    //
    //player->Update(elapsedTime);
    //stageMain->Update(elapsedTime);
}

// �`�揈��
void SceneTitle::Render(float elapsedTime)
{
    // ���f���`��
    {
        //player->Render(deviceContext);
        //stageMain->Render(deviceContext, elapsedTime);
    }

    // 2D�`��
    {
        titleSprite->Render(deviceContext, { 0, 0 }, {1.0f,1.0f}, {1280, 720});
    }

    // �f�o�b�N
    {
        //player->DebugImGui();
    }
}