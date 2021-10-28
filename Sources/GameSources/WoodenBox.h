#pragma once

#include "ObjectDestruction.h"
#include <d3d11.h>

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


	const float UPMAX = 10;


	//�W�I���g���p
	Microsoft::WRL::ComPtr<ID3D11Buffer> destructionBuffer;

	//��ꂽ���ǂ���
	bool dest = false;
	const float ERACEFLOAT = 0.005f;
};