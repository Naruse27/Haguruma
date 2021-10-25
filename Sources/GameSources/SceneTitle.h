#ifndef SCENETITLE
#define SCENETITLE

#include "Scene.h"
#include "PlayerTemp.h"
#include "../GameLibSource/Sprite.h"
#include "StageMain.h"
#include "CameraControl.h"

// タイトルシーン
class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}

	// 初期化
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;

	// 終了化
	virtual void Finalize() override;

	// 更新処理
	virtual void Update(float elapsedTime) override;

	// 描画処理
	virtual void Render(float elapsedTime) override;
private:
	Sprite* titleSprite = nullptr;
};

#endif // !SCENETITLE
