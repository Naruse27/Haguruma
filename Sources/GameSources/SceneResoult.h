#pragma once

#include "Scene.h"

// �^�C�g���V�[��
class SceneResult : public Scene
{
public:
	SceneResult() {}
	~SceneResult() override {}

	// ������
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;

	// �I����
	virtual void Finalize() override;

	// �X�V����
	virtual void Update(float elapsedTime) override;

	// �`�揈��
	virtual void Render(float elapsedTime) override;
private:
};