#ifndef SCENEGAME
#define SCENEGAME

#include "Scene.h"
#include "Player.h"
#include "CameraControl.h"
#include "StageMain.h"

class SceneGame : public Scene
{
public:
	SceneGame() {};
	~SceneGame() override {};

	// ‰Šú‰»
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;

	// I—¹‰»
	virtual void Finalize() override;

	// XVˆ—
	virtual void Update(float elapsedTime) override;

	// •`‰æˆ—
	virtual void Render(float elapsedTime) override;
private:
	Player* player = nullptr;
	StageMain* stageMain = nullptr;
	CameraControl* cameraController = nullptr;

};

#endif // !SCENEGAME