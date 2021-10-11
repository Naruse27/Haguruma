#ifndef SCENEMANAGER
#define SCENEMANAGER

#include "Scene.h"

// シーンマネージャー
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	static SceneManager& Instance() // シングルトン
	{
		static SceneManager instance;
		return instance;
	}

	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(float elapsedTime);

	// クリア
	void Clear();

	// シーン切り替え
	void ChangeScene(Scene* scene);

private:
	Scene* currentScene = nullptr; // 現在のシーン
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

};

#endif // !SCENEMANAGER

