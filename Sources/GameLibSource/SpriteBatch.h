#ifndef SPRITEBATCH
#define SPRITEBATCH

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

class SpriteBatch
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

	const size_t maxVertices;
	std::vector<Vertex> vertices;
public:
	SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t maxSprites);
	~SpriteBatch() {}

	void Begin(ID3D11DeviceContext* immediateContext, ID3D11PixelShader* replacedPixelShader, ID3D11ShaderResourceView* replacedResourceShader);
	void End(ID3D11DeviceContext* immediateContext);

	void Render(ID3D11DeviceContext* immediateContext,
		float dx, float dy, float dw, float dh,
		float sx, float sy, float sw, float sh,
		float angle/*degree*/,
		float r, float g, float b, float a
	);

	void Render(ID3D11DeviceContext* immediateContext,
		float dx, float dy, float dw, float dh,
		float sx, float sy, float sw, float sh,
		float angle/*degree*/
	);

	void Render(ID3D11DeviceContext* immediateContext,
		float dx, float dy, float dw, float dh,
		float angle/*degree*/
	);

	void Render(ID3D11DeviceContext* immediateContext,
		float dx, float dy, float dw, float dh
	);
};

#endif //!SPRITEBATCH