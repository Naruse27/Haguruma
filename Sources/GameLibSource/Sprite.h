#ifndef SPRITE
#define SPRITE

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <string>
#include "Vector.h"

static const float UV_ADJUST = 1.0f;// 0.99994f;

class Sprite
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>    vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>     pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>     inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>          vertexBuffer;
	// シェーダ リソース ビュー インターフェイス
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2dDesc;            // 2D テクスチャ

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

public:
	Sprite(ID3D11Device* device);
	Sprite(ID3D11Device* device, const wchar_t* filename);
	Sprite(ID3D11Device*, const wchar_t*, int aaaa, int nBufSize = (4));
	~Sprite() {}

	void Render(ID3D11DeviceContext* immediateContext,
		Vector2 position,Vector2 scale, Vector2 size,
		float sx, float sy, float sw, float sh,
		float angle/*degree*/,
		Vector4 color
	);

	void Render(ID3D11DeviceContext* immediateContext,
		Vector2 position, Vector2 scale, Vector2 size,
		float sx, float sy, float sw, float sh,
		float angle/*degree*/
	);

	void Render(ID3D11DeviceContext* immediateContext,
		Vector2 position, Vector2 scale, Vector2 size,
		float angle/*degree*/);

	void Render(ID3D11DeviceContext* immediateContext, Vector2 position, Vector2 scale, Vector2 size);

	void Texout(ID3D11DeviceContext* immediateContext,
		std::string s,
		float x, float y, float w, float h, Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f });

	void render(ID3D11DeviceContext* context,
		const Vector2& position, const Vector2& scale,
		const Vector2& texPos, const Vector2& texSize,
		const Vector2& center, float angle,
		const Vector4& color
	) const;
private:
	void CreateStates(ID3D11Device* device);

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		  samplerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	  rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>   depthStencilState;

};


#endif // !SPRITE