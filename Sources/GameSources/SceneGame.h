#ifndef SCENEGAME
#define SCENEGAME

#include "Scene.h"
#include "Player.h"
#include "CameraControl.h"
#include "StageMain.h"
#include "StageWall.h"
#include "WoodenBox.h"
#include "MetaAI.h"
#include "GameLibSource/Graphics/Fog.h"
<<<<<<< HEAD
#include "GameLibSource/Blender.h"

struct UI
{
	std::shared_ptr<Sprite> uiSpr;
	Vector2 pos;
	Vector2 size;
	Vector2 scale;
	Vector2 pivot;
};
=======
#include "DestructionManager.h"
>>>>>>> 070702319ab3ccce4f6a49339f966afc094a1dd4

class SceneGame : public Scene
{
public:
	SceneGame() {};
	~SceneGame() override {};

	// 初期化
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;

	// 終了化
	virtual void Finalize() override;

	// 更新処理
	virtual void Update(float elapsedTime) override;

	// 描画処理
	virtual void Render(float elapsedTime) override;

	// ゲームリセット
	void Reset();

	// UI用↓
	void UIupdate();

	void UiSetting();

	void UiCheng(int num);

private:
	Player* player = nullptr;
	StageMain* stageMain = nullptr;
	StageWall* stageWall = nullptr;
	CameraControl* cameraController = nullptr;
<<<<<<< HEAD
	std::shared_ptr<Meta> metaAi;
	std::shared_ptr<Fog> fog;

	std::shared_ptr<Blender> blender;

	// UI用
	bool select = false;
	UI ui[3];
	int startId = 0;
=======

	std::unique_ptr<Meta> metaAi;
	std::shared_ptr<Fog> fog;
>>>>>>> 070702319ab3ccce4f6a49339f966afc094a1dd4
};

#endif // !SCENEGAME