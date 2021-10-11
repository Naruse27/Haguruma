#include "Texture.h"

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <filesystem>
#include "misc.h"
//#include <DirectXTex.h>

using namespace std;
//#include <WICTextureLoader.h>
//using namespace DirectX;
//
//#include <wrl.h>
//using namespace Microsoft::WRL;
//
//#include <memory>
//#include <string>
//#include <map>
//#include <DDSTextureLoader.h>
//#include <filesystem>
//#include "misc.h"
//
//using namespace std;

static std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> resources;

void Texture::LoadTextureFromFile(
    ID3D11Device* device, const wchar_t* filename,
    ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc)
{
    HRESULT hr{ S_OK };
    ComPtr<ID3D11Resource> resource;

    auto it = resources.find(filename);

    filesystem::path ddsFilename(filename);
    ddsFilename.replace_extension("dds");
    if (filesystem::exists(ddsFilename.c_str()))
    {
        ComPtr<ID3D11DeviceContext> immediate_context;
        device->GetImmediateContext(immediate_context.GetAddressOf());
        hr = DirectX::CreateDDSTextureFromFile(device, immediate_context.Get(), ddsFilename.c_str(), resource.GetAddressOf(), shaderResourceView);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    else
    {
        hr = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shaderResourceView);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        resources.insert(make_pair(filename, *shaderResourceView));
    }

    ComPtr<ID3D11Texture2D> texture2d;
    hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    texture2d->GetDesc(texture2dDesc);
}

void Texture::LoadTextureFromFile
 (  ID3D11Device* device,
	const char* fileName,
	ID3D11ShaderResourceView** shaderResourceView )
{
	//HRESULT hr = S_OK;
    //
	//Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    //
	//static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;
    //
	//auto it = cache.find( fileName );
	//if ( it != cache.end() )
	//{
	//	*shaderResourceView = it->second.Get();
	//	( *shaderResourceView )->AddRef();
	//	( *shaderResourceView )->GetResource( resource.GetAddressOf() );
	//}
	//else
	//{
	//	/*hr = DirectX::CreateWICTextureFromFile(device, fileName, resource.GetAddressOf(), shaderResourceView);
	//	assert( !hr && "DirectX::CreateWICTextureFromFile	Error" );*/
    //
	//	char ext[32];
	//	_splitpath_s( fileName, NULL, NULL, NULL, NULL, NULL, NULL, ext, sizeof( ext ) );
    //
	//	wchar_t _fileName[256];
    //
	//	MultiByteToWideChar( CP_ACP, 0, fileName, static_cast<int>( strlen( fileName ) + 1 ), _fileName, 256 );
    //
	//	DirectX::TexMetadata metaData;
	//	DirectX::ScratchImage image;
    //    
    //
	//	if ( strcmp( ext, ".tga" ) == 0 )
	//	{
	//		hr = DirectX::LoadFromTGAFile( _fileName, &metaData, image );
	//		assert( !hr && "DirectX::LoadFromTGAFile	Error" );
	//	}
	//	else if ( strcmp( ext, ".png" ) == 0 )
	//	{
	//		hr = DirectX::LoadFromWICFile( _fileName, 0, &metaData, image );
	//		assert( !hr && "DirectX::LoadFromWICFile	Error" );
	//	}
    //
	//	hr = DirectX::CreateShaderResourceViewEx(
	//		device,
	//		image.GetImages(),
	//		image.GetImageCount(),
	//		metaData,
	//		D3D11_USAGE_DEFAULT,
	//		D3D11_BIND_SHADER_RESOURCE,
	//		0,
	//		D3D11_RESOURCE_MISC_TEXTURECUBE,
	//		true,
	//		shaderResourceView );
	//	assert( !hr && "DirectX::CreateShaderResourceViewEx" );
    //
	//	cache.insert( std::make_pair( fileName, *shaderResourceView ) );
	//}
}

void Texture::MakeDumyTexture(
    ID3D11Device* device,
    ID3D11ShaderResourceView** shaderResourceView,
    DWORD value/*0xAABBGGRR*/,
    UINT dimension)
{
    HRESULT hr{ S_OK };

    D3D11_TEXTURE2D_DESC texture2dDesc{};
    texture2dDesc.Width = dimension;
    texture2dDesc.Height = dimension;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    size_t texels = dimension * dimension;
    unique_ptr<DWORD[]> system{ make_unique<DWORD[]>(texels) };
    for (size_t i = 0; i < texels; ++i) system[i] = value;

    D3D11_SUBRESOURCE_DATA subresourceData{};
    subresourceData.pSysMem = system.get();
    subresourceData.SysMemPitch = sizeof(DWORD) * dimension;

    ComPtr<ID3D11Texture2D> texture2d;
    hr = device->CreateTexture2D(&texture2dDesc, &subresourceData, &texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
    shaderResourceViewDesc.Format = texture2dDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, shaderResourceView);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Texture::ReleaseAllTextures()
{
    resources.clear();
}

ID3D11ShaderResourceView* Texture::QueryTexture(ID3D11Device* device, const wchar_t* filename, D3D11_TEXTURE2D_DESC* texture2dDesc)
{
    ID3D11ShaderResourceView* shaderResourceView{ nullptr };
    LoadTextureFromFile(device, filename, &shaderResourceView, texture2dDesc);
    return shaderResourceView;
}

ID3D11ShaderResourceView* Texture::QueryDummyTexture(ID3D11Device* device, DWORD value/*0xAABBGGRR*/, UINT dimension)
{
    ID3D11ShaderResourceView* shaderResourceView{ nullptr };
    MakeDumyTexture(device, &shaderResourceView, value, dimension);
    return shaderResourceView;
}

void Texture::CreateTexture
(ID3D11Device* device,
    u_int width,
    u_int height,
    DXGI_FORMAT format,
    ID3D11ShaderResourceView** shaderResourceView,
    ID3D11Texture2D** texture2d)
{
    HRESULT hr = S_OK;

    //	テクスチャ作成
    D3D11_TEXTURE2D_DESC texture2dDesc;
    texture2dDesc.Width = width;
    texture2dDesc.Height = height;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = format;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    hr = device->CreateTexture2D(&texture2dDesc, NULL, texture2d);

    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
