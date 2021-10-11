#ifndef TEXTURE
#define TEXTURE

#include <d3d11.h>
#include <memory>
#include <string>
#include <map>
#include <wrl.h>
using namespace Microsoft::WRL;


class Texture
{
public:
    static void LoadTextureFromFile(
        ID3D11Device* device,
        const wchar_t* filename,
        ID3D11ShaderResourceView** shaderResourceView,
        D3D11_TEXTURE2D_DESC* texture2dDesc);

    static void LoadTextureFromFile
    (
        ID3D11Device* device,
        const char* fileName,
        ID3D11ShaderResourceView** shaderResourceView);

    static void MakeDumyTexture(
        ID3D11Device* device,
        ID3D11ShaderResourceView** shaderResourceView,
        DWORD value/*0xAABBGGRR*/,
        UINT dimension);

    static void ReleaseAllTextures();

    static ID3D11ShaderResourceView* QueryTexture(ID3D11Device* device, const wchar_t* filename, D3D11_TEXTURE2D_DESC* texture2dDesc);

    static ID3D11ShaderResourceView* QueryDummyTexture(ID3D11Device* device, DWORD value/*0xAABBGGRR*/, UINT dimension);

    static void CreateTexture( ID3D11Device* device, u_int width, u_int height, DXGI_FORMAT format,
        ID3D11ShaderResourceView** shaderResourceView,
        ID3D11Texture2D** texture2d
    );
};

#endif // !TEXTURE

