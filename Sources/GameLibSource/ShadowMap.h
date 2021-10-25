#ifndef SHADOWMAP
#define SHADOWMAP

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include "high_resolution_timer.h"
#include "misc.h"
#include <DirectXMath.h>

class ShadowMap
{
public:
	// �V���h�E�}�b�v
	struct ShadowmapConstants
	{
		DirectX::XMFLOAT4X4 lightViewProjection;	// ���C�g�̈ʒu���猩���ˉe�s��
		DirectX::XMFLOAT3	shadowColor;			// �e�F
		float				shadowBias;			// �[�x�o�C�A�X
	};

	//DepthStencilState
	enum { DS_FALSE, DS_TRUE };

public:
	ShadowMap(ID3D11Device* device);
	~ShadowMap();


private:
	CONST LONG SHADOWMAP_WIDTH{ 1024 };
	CONST LONG SHADOWMAP_HEIGHT{ 1024 };
	CONST float SHADOWMAP_DRAWRECT{ 30 };


	//std::unique_ptr<Sprite> pTex;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		renderTargetView;
	//Microsoft::WRL::ComPtr<ID3D11SamplerState>			sampler;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState[2];


	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowmapConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shadowmapCasterVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> shadowmapCasterInputLayout;
	DirectX::XMFLOAT4X4 lightViewProjection{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
	float				shadowBias{ 0.008f };
	DirectX::XMFLOAT3	shadowColor{ 0.3f, 0.3f, 0.3f };
};

#endif // !SHADOWMAP
