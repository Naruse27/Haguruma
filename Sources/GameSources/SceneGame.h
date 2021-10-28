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
#include "DestructionManager.h"

class SceneGame : public Scene
{
public:
	SceneGame() {};
	~SceneGame() override {};

	// ������
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;

	// �I����
	virtual void Finalize() override;

	// �X�V����
	virtual void Update(float elapsedTime) override;

	// �`�揈��
	virtual void Render(float elapsedTime) override;
private:
	Player* player = nullptr;
	StageMain* stageMain = nullptr;
	StageWall* stageWall = nullptr;
	CameraControl* cameraController = nullptr;

	std::unique_ptr<Meta> metaAi;
	std::shared_ptr<Fog> fog;
};

#endif // !SCENEGAME