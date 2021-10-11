#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include <thread>

void SceneLoading::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    this->device = device;
    this->deviceContext = deviceContext;

    // スレッド開始
    std::thread thread(LoadingThread, this);

    // スレッドの管理を破棄
    thread.detach();
}

void SceneLoading::Finalize()
{
}

void SceneLoading::Update(float elapsedTime)
{
    // constexpr 変数の定義、関数や関数テンプレートの宣言、リテラル型の静的データメンバの宣言に適用する
    constexpr float speed = 180;
    angle += speed * elapsedTime;

    // 次のシーンの準備が完了したらシーンを切り替える
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
    // COM関連の初期化でスレッド毎に呼ぶ必要がある
    CoInitialize(nullptr);

    // 次のシーンの初期化を行う
    scene->nextScene->Initialize(scene->device, scene->deviceContext);

    // スレッドが終わる前にCOM関連の終了化
    CoUninitialize();

    // 次のシーンの準備完了
    scene->nextScene->SetReady();
}