#include "Fog.h"
#include "GameLibSource/misc.h"

Fog::Fog(ID3D11Device* device)
{
	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	buffer_desc.ByteWidth = sizeof(FogConstants);
	hr = device->CreateBuffer(&buffer_desc, nullptr, fogConstantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
}

Fog::~Fog()
{
}

void Fog::UpdateRendrer(ID3D11DeviceContext* deviceContext)
{
	FogConstants fogs{};
	fogs.fogColor = fogColor;
	fogs.fogRange = fogRange;
	deviceContext->UpdateSubresource(fogConstantBuffer.Get(), 0, 0, &fogs, 0, 0);
	deviceContext->VSSetConstantBuffers(5, 1, fogConstantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(5, 1, fogConstantBuffer.GetAddressOf());
}