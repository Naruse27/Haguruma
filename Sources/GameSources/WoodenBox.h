#pragma once

#include "ObjectDestruction.h"
#include <d3d11.h>
#include <memory>

class WoodenBox : public ObjectDestruction
{
public:
	WoodenBox(ID3D11Device* device, DestructionManager* mgr);
	virtual ~WoodenBox();

	// �X�V
	void Update(float elapsedTime)override;
	// �`��
	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime)override;
	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)override;

private:
	std::unique_ptr<Model> model;

	const float UPMAX = 10;
	struct Destruct
	{
		float scale;
		float angle;
		float position;
		float destruction;
	};
	float scaleFactor = 1.0f;
	float rotationFactor = 1.0f;
	float positionFactor = 1.0f;
	float destruction = 0.0f;

	const float ERACEFLOAT = 0.05f;

	//�W�I���g���p
	Microsoft::WRL::ComPtr<ID3D11Buffer> destructionBuffer;
};