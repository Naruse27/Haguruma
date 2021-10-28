#pragma once

#include "ObjectDestruction.h"
#include <d3d11.h>

class WoodenBox : public ObjectDestruction
{
public:
	WoodenBox(ID3D11Device* device, DestructionManager* mgr);
	virtual ~WoodenBox();

	// 更新
	void Update(float elapsedTime)override;
	// 描画
	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime)override;
	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)override;

private:


	const float UPMAX = 10;


	//ジオメトリ用
	Microsoft::WRL::ComPtr<ID3D11Buffer> destructionBuffer;

	//壊れたかどうか
	bool dest = false;
	const float ERACEFLOAT = 0.005f;
};