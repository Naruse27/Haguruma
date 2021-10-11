#include "SceneManager.h"

// ������ 
void SceneManager::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    this->device = device;
    this->deviceContext = deviceContext;
}

//�@�X�V
void SceneManager::Update(float elapsedTime)
{
    if (currentScene) currentScene->Update(elapsedTime);
}

// �`��
void SceneManager::Render(float elapsedTime)
{
    if (currentScene) currentScene->Render(elapsedTime);
}

// �N���A
void SceneManager::Clear()
{
    if (currentScene) {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

void SceneManager::ChangeScene(Scene* scene)
{
    // �Â��V�[�����I������
    Clear();
    // �V�����V�[����ݒ�
    currentScene = scene;
    // �V�[��������
    if (!currentScene->IsReady()) currentScene->Initialize(this->device, this->deviceContext);
}