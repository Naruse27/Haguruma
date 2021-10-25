#ifndef SCENETITLE
#define SCENETITLE

#include "Scene.h"
#include "PlayerTemp.h"
#include "../GameLibSource/Sprite.h"
#include "StageMain.h"
#include "CameraControl.h"

// �^�C�g���V�[��
class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}

	// ������
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;

	// �I����
	virtual void Finalize() override;

	// �X�V����
	virtual void Update(float elapsedTime) override;

	// �`�揈��
	virtual void Render(float elapsedTime) override;
private:
	Sprite* titleSprite = nullptr;
};

#endif // !SCENETITLE
