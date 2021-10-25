#ifndef SCENESELECT2
#define SCENESELECT2

#include "Scene.h"
#include "PlayerTemp.h"
#include "../GameLibSource/Sprite.h"
#include "StageMain.h"
#include "CameraControl.h"

// タイトルシーン
class SceneSelect2 : public Scene
{
public:
	SceneSelect2() {}
	~SceneSelect2() override {}

	// 初期化
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;

	// 終了化
	virtual void Finalize() override;

	// 更新処理
	virtual void Update(float elapsedTime) override;

	// 描画処理
	virtual void Render(float elapsedTime) override;
private:
	Sprite* selectSprite = nullptr;
	Sprite* frameSprite = nullptr;
};



#endif // !SCENESELECT2
