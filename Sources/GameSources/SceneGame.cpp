#include "SceneGame.h"
#include "../GameLibSource/Framework.h"
#include "CameraManager.h"
#include "StageManager.h"
#include "Input/Input.h"
#include "GimmickManager.h"
#include "Stand.h"
#include "Gear.h"
#include "GimmickPoint.h"

// ������
void SceneGame::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    this->device = device;
    this->deviceContext = deviceContext;

    //ID3D11Device* device = Framework::GetInstance().GetDevice().Get();
    StageManager::Create();
    GimmickPoint* point = new GimmickPoint(device);
    GimmickManager::Instance().Register(point, Identity::Start);

    player = new Player(this->device);

    Stand* stand = new Stand(device);
    GimmickManager::Instance().Register(stand, Identity::Stand); 

    cameraController = new CameraControl();
    stageMain = new StageMain(this->device);
    stageWall = new StageWall(this->device);
    
    //titleSprite = new Sprite(device, L"./Data/Sprite/screenshot.jpg");
    StageManager::Instance().Register(stageMain);
    StageManager::Instance().Register(stageWall);

    player->SetStartGimmickID(point->GetId());

    metaAi.reset(new Meta(player, &GimmickManager::Instance()));
}

// �I����
void SceneGame::Finalize()
{
    delete player;
    delete cameraController;
    delete stageMain;
    delete stageWall;
    //StageManager::Instance().Clear();
    GimmickManager::Instance().Clear();

    StageManager::Destory();
    //delete titleSprite;
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
    // �J�����R���g���[���[�X�V
    Vector3 target = player->GetPosition();
    target.y += player->GetHeight() + 2.0f;
    cameraController->SetTarget(target);
    cameraController->Update(elapsedTime, &CameraManager::Instance().mainView);

    player->Update(elapsedTime);
    StageManager::Instance().Update(elapsedTime);
    GimmickManager::Instance().Update(elapsedTime);

    player->MouseRay(device, deviceContext, CameraManager::Instance().mainView.GetView(), CameraManager::Instance().mainView.GetProjection());
    //stageMain->Update(elapsedTime);
    //stageWall->Update(elapsedTime);
    metaAi->Update();
}

// �`�揈��
void SceneGame::Render(float elapsedTime)
{
    // ���f���`��
    {
        player->Render(deviceContext);
        StageManager::Instance().Render(deviceContext, elapsedTime);
        GimmickManager::Instance().Render(deviceContext, elapsedTime);
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
        cameraController->RenderDebugGui(&CameraManager::Instance().mainView);
    }
}