#include "SceneGame.h"
#include "../GameLibSource/Framework.h"
#include "CameraManager.h"
#include "StageManager.h"
#include "Input/Input.h"

// ������
void SceneGame::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    this->device = device;
    this->deviceContext = deviceContext;

    //ID3D11Device* device = Framework::GetInstance().GetDevice().Get();
    StageManager::Create();

    cameraController = new CameraControl();
    player = new Player(this->device);
    stageMain = new StageMain(this->device);
    stageWall = new StageWall(this->device);
    //titleSprite = new Sprite(device, L"./Data/Sprite/screenshot.jpg");
    StageManager::Instance().Register(stageMain);
    StageManager::Instance().Register(stageWall);
}

// �I����
void SceneGame::Finalize()
{
    delete player;
    delete cameraController;
    delete stageMain;
    delete stageWall;
    //StageManager::Instance().Clear();
    StageManager::Destory();
    //delete titleSprite;
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
    // �J�����R���g���[���[�X�V
    DirectX::XMFLOAT3 target = player->GetPosition();
    target.y += 0.5f;
    cameraController->SetTarget(target);
    cameraController->Update(elapsedTime, &CameraManager::Instance().mainView);

    player->Update(elapsedTime);
    StageManager::Instance().Update(elapsedTime);
    //stageMain->Update(elapsedTime);
    //stageWall->Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render(float elapsedTime)
{
    // ���f���`��
    {
        player->Render(deviceContext);
        StageManager::Instance().Render(deviceContext, elapsedTime);

        //stageMain->Render(deviceContext, elapsedTime);
        //stageWall->Render(deviceContext, elapsedTime);
    }

    // 2D�`��
    {
        //titleSprite->Render(Framework::GetInstance().GetContext().Get(), { 0, 0 }, {1600, 900});
    }

    // �f�o�b�N
    {
        player->DebugImGui();
    }
}