#include "WoodenBox.h"

WoodenBox::WoodenBox(ID3D11Device* device, DestructionManager* mgr) :ObjectDestruction(device,mgr)
{
	model.reset(new Model(device, "Data/Model/Stage/stage.fbx", true, 0, TRUE));
	//�V�[���萔�o�b�t�@�I�u�W�F�N�g����
	D3D11_BUFFER_DESC bufferDesc{};
	{
		bufferDesc.ByteWidth = sizeof(Destruct);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	}
	HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, destructionBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//scale = { 1.0f, 1.0f, 1.0f };
}

WoodenBox::~WoodenBox()
{
}

void WoodenBox::Update(float elapsedTime)
{
	if (dest) destruction += ERACEFLOAT;
	if (destruction >= 1.0f)Destroy();
	model->UpdateTransform(transform);
}

void WoodenBox::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
	//�V�F�[�_�p
	{
		Destruct data;
		data.angle = rotationFactor;
		data.scale = scaleFactor;
		data.position = positionFactor;
		data.destruction = destruction;
		deviceContext->UpdateSubresource(destructionBuffer.Get(), 0, 0, &data, 0, 0);
		deviceContext->GSSetConstantBuffers(3, 1, destructionBuffer.GetAddressOf());
	}
	model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::DEFAULT), true);
	model->Render(deviceContext);
}

bool WoodenBox::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model.get(), hit);
}