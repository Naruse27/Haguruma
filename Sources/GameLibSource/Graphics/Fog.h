#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>


class Fog
{
private:
	struct FogConstants
	{
		DirectX::XMFLOAT4 fogColor;
		DirectX::XMFLOAT4 fogRange;	// x : near, y : far, zw : 空き
	};

public:
	Fog(ID3D11Device* device);
	~Fog();

	void UpdateRendrer(ID3D11DeviceContext* deviceContext);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> fogConstantBuffer;
	DirectX::XMFLOAT4 fogColor{ 1.0f, 1.0f, 1.0f, 1.0f }; // クリアカラーにしています
	DirectX::XMFLOAT4 fogRange{ 0.1f, 200.0f, 0, 0 };
};

