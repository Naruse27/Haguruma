#ifndef SCENELOADING
#define SCENELOADING

#include "Scene.h"

// ローディングシーン
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene) {}
	~SceneLoading() override {}

	// 初期化
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;

	// 終了化
	virtual void Finalize() override;

	// 更新処理
	virtual void Update(float elapsedTime) override;

	// 描画処理
	virtual void Render(float elapsedTime) override;

private:
	// ローディングスレッド
	static void LoadingThread(SceneLoading* scene);

private:
	float angle = 0.0f;
	Scene* nextScene = nullptr;
};


#endif // !SCENELOADING

