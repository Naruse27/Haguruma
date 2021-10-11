#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include <thread>

void SceneLoading::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    this->device = device;
    this->deviceContext = deviceContext;

    // �X���b�h�J�n
    std::thread thread(LoadingThread, this);

    // �X���b�h�̊Ǘ���j��
    thread.detach();
}

void SceneLoading::Finalize()
{
}

void SceneLoading::Update(float elapsedTime)
{
    // constexpr �ϐ��̒�`�A�֐���֐��e���v���[�g�̐錾�A���e�����^�̐ÓI�f�[�^�����o�̐錾�ɓK�p����
    constexpr float speed = 180;
    angle += speed * elapsedTime;

    // ���̃V�[���̏���������������V�[����؂�ւ���
    if (nextScene->IsReady()) {
        SceneManager::Instance().ChangeScene(nextScene);
        nextScene = nullptr;
    }
}

void SceneLoading::Render(float elapsedTime)
{
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
    // COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
    CoInitialize(nullptr);

    // ���̃V�[���̏��������s��
    scene->nextScene->Initialize(scene->device, scene->deviceContext);

    // �X���b�h���I���O��COM�֘A�̏I����
    CoUninitialize();

    // ���̃V�[���̏�������
    scene->nextScene->SetReady();
}