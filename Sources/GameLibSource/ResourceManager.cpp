#include "ResourceManager.h"

#include <wrl.h>
#include <map>
#include <filesystem>
using namespace std;

// VertexShader <- vsCSO
void ResourceManager::CreateVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader,
    ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements)
{
    FILE* fp{ nullptr };
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "VertexShaderCSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    unique_ptr<unsigned char[]> cso_data{ make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertexShader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    if (inputLayout)
    {
        hr = device->CreateInputLayout(inputElementDesc, numElements,
            cso_data.get(), cso_sz, inputLayout);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}

// PixcelShader <- psCSO
void ResourceManager::CreatePsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader)
{
    FILE* fp{ nullptr };
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "PixcelShaderCSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    unique_ptr<unsigned char[]> cso_data{ make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixelShader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void ResourceManager::CreateCsFromCso(ID3D11Device* device, const char* csoName, ID3D11ComputeShader** computeShader)
{
    HRESULT hr = S_OK;

    static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11ComputeShader>> cache;

    auto it = cache.find(csoName);
    if (it != cache.end())
    {
        *computeShader = it->second.Get();
        (*computeShader)->AddRef();
    }

    //Load CSO (ps)
    FILE* fp = 0;
    fopen_s(&fp, csoName, "rb");


    fseek(fp, 0, SEEK_END);
    long csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);


    std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    fp = nullptr;

    hr = device->CreateComputeShader(csoData.get(), csoSize, NULL, computeShader);
    _ASSERT_EXPR_A(fp, "ComputeShaderCSO File not found");

    cache.insert(std::make_pair(csoName, *computeShader));

}