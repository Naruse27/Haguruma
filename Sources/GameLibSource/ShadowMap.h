#ifndef SHADOWMAP
#define SHADOWMAP

#include <DirectXMath.h>

#include <memory>
#include <vector>
#include "Sprite.h"

class ShadowMap
{
public:
	//コンスタントバッファ構造体
	struct ConstantShadow
	{
		DirectX::XMFLOAT4X4  LightViewProjection;
		DirectX::XMFLOAT4	ShadowColor;
	};
	ConstantShadow constantShadow;

	//DepthStencilState
	enum { DS_FALSE, DS_TRUE };

public:
	ShadowMap();
	~ShadowMap();


private:
	std::unique_ptr<Sprite> pTex;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			sampler;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState[2];
	DirectX::XMFLOAT4X4 lightViewProjection;
};

#endif // !SHADOWMAP
