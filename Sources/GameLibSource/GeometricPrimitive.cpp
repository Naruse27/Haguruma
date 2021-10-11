#include "GeometricPrimitive.h"
#include "ResourceManager.h"
#include "misc.h"
#include <map>
#include <vector>

GeometricPrimitive::GeometricPrimitive(ID3D11Device *device, int type, bool isCreateBottom, DirectX::XMFLOAT3* scale)
{
	HRESULT hr = S_OK;

	//���_�f�[�^�̍\�����`
	D3D11_INPUT_ELEMENT_DESC ieDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//���_�V�F�[�_�[�I�u�W�F�N�g�쐬 && �C���v�b�g���C�A�E�g�쐬****************
	ResourceManager::CreateVsFromCso(device, "./Library/Shader/GeometricPrimitiveVS.cso", &vertexShader, &inputLayout, ieDesc, ARRAYSIZE(ieDesc));
	//**************************************************************************

	//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�쐬****************************************
	ResourceManager::CreatePsFromCso(device, "./Library/Shader/GeometricPrimitivePS.cso", &pixelShader);
	//**************************************************************************

	//���X�^���C�U�[�X�e�[�g�I�u�W�F�N�g�쐬************************************
	//���C���[�t���[��
	D3D11_RASTERIZER_DESC rasterizer_desc = {}; //memo:�u = {}�v�ō\���̂�������
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, wireframeRasterizerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�\���b�h
	ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthClipEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, solidRasterizerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//**************************************************************************

	//�[�x�X�e���V�� �X�e�[�g �I�u�W�F�N�g**************************************
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//**************************************************************************

	//���_���W�̎w��icube�쐬�j************************************************
	switch (type)
	{
	case GeometricPrimitiveType::CUBE:
		GeometricCube(device, *scale, isCreateBottom);
		break;
	case GeometricPrimitiveType::SPHERE:
		GeometricSphere(device);
		break;
	case GeometricPrimitiveType::CYLINDER:
		GeometricCylinder(device);
		break;
	default:
		break;
	}
	//**************************************************************************
}

void GeometricPrimitive::Render(ID3D11DeviceContext* deviceContext,
	const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world,
	const DirectX::XMFLOAT4& lightDirection, const DirectX::XMFLOAT4& materialColor,
	bool bSolid)
{
	//�萔�o�b�t�@�̃o�C���h
	//�萔�o�b�t�@�̍쐬
	Cbuffer cb;
	cb.wvp = wvp;
	cb.world = world;
	cb.lightDirection = lightDirection;
	cb.materialColor = materialColor;
	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	//���_�o�b�t�@�̃o�C���h
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	//�C���f�b�N�X�o�b�t�@�̃o�C���h
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//�v���~�e�B�u���[�h�̐ݒ�
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//���̓��C�A�E�g�̃o�C���h
	deviceContext->IASetInputLayout(inputLayout.Get());

	//	���X�^���C�U�[�̐ݒ�
	if (bSolid)	deviceContext->RSSetState(solidRasterizerState.Get());
	else		deviceContext->RSSetState(wireframeRasterizerState.Get());

	//�V�F�[�_�[(2��)�̐ݒ�
	deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	//�[�x�e�X�g�̐ݒ�
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);

	//�v���~�e�B�u�̕`��(index�t��)
	deviceContext->DrawIndexed(numIndex, 0, 0);
}

void GeometricPrimitive::CreateBuffer(ID3D11Device *device, Vertex* vertices, unsigned int* indices, int numV, int numI)
{
	HRESULT hr = S_OK;

	//���_�o�b�t�@�[�I�u�W�F�N�g�쐬(���_�f�[�^��Direct3D�̃p�C�v���C���ɗ������ވׂ̃o�b�t�@�[�쐬)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(Vertex) * numV;//�o�b�t�@�[�̃T�C�Y���w��(���_���ɉ����ĂS�̐���ς���)
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;//���_�o�b�t�@�[�Ƃ��ăo�C���h����� D3D11_BIND_VERTEX_BUFFER���w��
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;//���\�[�X�f�[�^�̃|�C���^���w��
	InitData.SysMemPitch = 0; //���̑��̍��ڂ̓e�N�X�`���f�[�^�̏ꍇ�ɂ����g�����̂Ȃ̂Łu0�v���w��
	InitData.SysMemSlicePitch = 0; //���̑��̍��ڂ̓e�N�X�`���f�[�^�̏ꍇ�ɂ����g�����̂Ȃ̂Łu0�v���w��

	hr = device->CreateBuffer(&bd, &InitData, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�C���f�b�N�X�o�b�t�@�I�u�W�F�N�g�쐬
	if (indices)
	{
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(unsigned int) * numI;
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = indices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&bd, &InitData, indexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		numIndex = numI;
	}
	//�萔�o�b�t�@�I�u�W�F�N�g�쐬
	ZeroMemory(&bd, sizeof(bd));

	bd.ByteWidth = sizeof(Cbuffer);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	hr = device->CreateBuffer(&bd, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void GeometricPrimitive::GeometricCube(ID3D11Device* device, DirectX::XMFLOAT3 scale, bool isCreateBottom)
{
	Vertex vertices[4 * 6] = {}; // 4���_ * 6��
	unsigned int indices[3 * 2 * 6] = {};//�@3���_ * 2�� * 6�� �i�O�p�`�|���S���j

	int numV = 0, numI = 0;

	if (!isCreateBottom)
	{
		//	���
		vertices[numV + 0].pos = DirectX::XMFLOAT3(-scale.x, +scale.y, +scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+scale.x, +scale.y, +scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-scale.x, +scale.y, -scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(+scale.x, +scale.y, -scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
		numV += 4;	numI += 6;

		//	����
		vertices[numV + 0].pos = DirectX::XMFLOAT3(-scale.x, -scale.y, +scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+scale.x, -scale.y, +scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-scale.x, -scale.y, -scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(+scale.x, -scale.y, -scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
		numV += 4;	numI += 6;

		//	�E��
		vertices[numV + 0].pos = DirectX::XMFLOAT3(+scale.x, +scale.y, -scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+scale.x, +scale.y, +scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(+scale.x, -scale.y, -scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(+scale.x, -scale.y, +scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
		numV += 4;	numI += 6;

		//	����
		vertices[numV + 0].pos = DirectX::XMFLOAT3(-scale.x, +scale.y, -scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(-scale.x, +scale.y, +scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-scale.x, -scale.y, -scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(-scale.x, -scale.y, +scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
		numV += 4;	numI += 6;

		//	���
		vertices[numV + 0].pos = DirectX::XMFLOAT3(+scale.x, -scale.y, +scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+scale.x, +scale.y, +scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-scale.x, -scale.y, +scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(-scale.x, +scale.y, +scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
		numV += 4;	numI += 6;

		//	�O��
		vertices[numV + 0].pos = DirectX::XMFLOAT3(+scale.x, -scale.y, -scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+scale.x, +scale.y, -scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-scale.x, -scale.y, -scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(-scale.x, +scale.y, -scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
		numV += 4;	numI += 6;
	}
	else
	{
		//	���
		vertices[numV + 0].pos = DirectX::XMFLOAT3(-scale.x, +scale.y * 2.0f, +scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+scale.x, +scale.y * 2.0f, +scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-scale.x, +scale.y * 2.0f, -scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(+scale.x, +scale.y * 2.0f, -scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
		numV += 4;	numI += 6;

		//	����
		vertices[numV + 0].pos = DirectX::XMFLOAT3(-scale.x, 0.0f, +scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+scale.x, 0.0f, +scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-scale.x, 0.0f, -scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(+scale.x, 0.0f, -scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
		numV += 4;	numI += 6;

		//	�E��
		vertices[numV + 0].pos = DirectX::XMFLOAT3(+scale.x, +scale.y * 2.0f, -scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+scale.x, +scale.y * 2.0f, +scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(+scale.x, 0.0f, -scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(+scale.x, 0.0f, +scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
		numV += 4;	numI += 6;

		//	����
		vertices[numV + 0].pos = DirectX::XMFLOAT3(-scale.x, +scale.y * 2.0f, -scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(-scale.x, +scale.y * 2.0f, +scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-scale.x, 0.0f, -scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(-scale.x, 0.0f, +scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
		numV += 4;	numI += 6;

		//	���
		vertices[numV + 0].pos = DirectX::XMFLOAT3(+scale.x, 0.0f, +scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+scale.x, +scale.y * 2.0f, +scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-scale.x, 0.0f, +scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(-scale.x, +scale.y * 2.0f, +scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
		numV += 4;	numI += 6;

		//	�O��
		vertices[numV + 0].pos = DirectX::XMFLOAT3(+scale.x, 0.0f, -scale.z);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+scale.x, +scale.y * 2.0f, -scale.z);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-scale.x, 0.0f, -scale.z);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(-scale.x, +scale.y * 2.0f, -scale.z);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
			vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
		numV += 4;	numI += 6;
	}
	//�o�b�t�@�[�쐬
	CreateBuffer(device, vertices, indices, numV, numI);
}

void GeometricPrimitive::GeometricCylinder(ID3D11Device* device)
{
	constexpr int plate = 36;
	Vertex vertices[3 * 2 * plate + 4 * plate] = {};
	unsigned int indices[3 * 2 * plate + 3 * 2 * plate] = {};

	int numV = 0, numI = 0;

	float angle = 0.0f;
	float sin = sinf(angle);
	float cos = cosf(angle);

	//	���
	for (int i = 0; i < plate; i++)
	{
		sin = sinf(angle);
		cos = cosf(angle);
		vertices[numV + 0].pos = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+0.1f * cos - -1.0f * sin, +1.0f, +0.1f * sin + -1.0f * cos);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-0.1f * cos - -1.0f * sin, +1.0f, -0.1f * sin + -1.0f * cos);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
		angle += 0.35f / 2.0f;
		numV += 3;	numI += 3;
	}

	angle = 0.0f;
	sin = sinf(angle);
	cos = cosf(angle);

	//	����
	for (int i = 0; i < plate; i++)
	{
		sin = sinf(angle);
		cos = cosf(angle);
		vertices[numV + 0].pos = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+0.1f * cos - -1.0f * sin, -1.0f, +0.1f * sin + -1.0f * cos);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-0.1f * cos - -1.0f * sin, -1.0f, -0.1f * sin + -1.0f * cos);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
		indices[numI + 0] = numV + 2;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 0;
		angle += 0.35f / 2.0f;
		numV += 3;	numI += 3;
	}

	angle = 0.0f;
	sin = sinf(angle);
	cos = cosf(angle);

	//	����
	for (int i = 0; i < plate; i++)
	{
		sin = sinf(angle);
		cos = cosf(angle);
		vertices[numV + 0].pos = DirectX::XMFLOAT3(+0.1f * cos - -1.0f * sin, -1.0f, +0.1f * sin + -1.0f * cos);
		vertices[numV + 1].pos = DirectX::XMFLOAT3(+0.1f * cos - -1.0f * sin, +1.0f, +0.1f * sin + -1.0f * cos);
		vertices[numV + 2].pos = DirectX::XMFLOAT3(-0.1f * cos - -1.0f * sin, -1.0f, -0.1f * sin + -1.0f * cos);
		vertices[numV + 3].pos = DirectX::XMFLOAT3(-0.1f * cos - -1.0f * sin, +1.0f, -0.1f * sin + -1.0f * cos);
		vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
		angle += 0.35f / 2.0f;
		numV += 4;	numI += 6;
	}

	//�o�b�t�@�[�쐬
	CreateBuffer(device, vertices, indices, numV, numI);
}

void GeometricPrimitive::GeometricSphere(ID3D11Device* device, u_int slices, u_int stacks)
{
	std::vector<Vertex> vertices;
	std::vector<u_int> indices;


	float r = 1.0f;		//	���a 1.0f = ���a 2.0f

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	Vertex topVertex;
	topVertex.pos = DirectX::XMFLOAT3(0.0f, +r, 0.0f);
	topVertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);

	Vertex bottomVertex;
	bottomVertex.pos = DirectX::XMFLOAT3(0.0f, -r, 0.0f);
	bottomVertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

	vertices.push_back(topVertex);

	float phiStep = DirectX::XM_PI / stacks;
	float thetaStep = 2.0f*DirectX::XM_PI / slices;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (u_int i = 1; i <= stacks - 1; ++i)
	{
		float phi = i * phiStep;
		float rs_phi = r * sinf(phi), rc_phi = r * cosf(phi);

		// Vertices of ring.
		for (u_int j = 0; j <= slices; ++j)
		{
			float theta = j * thetaStep;

			Vertex v;

			// spherical to cartesian
			v.pos.x = rs_phi * cosf(theta);
			v.pos.y = rc_phi;
			v.pos.z = rs_phi * sinf(theta);

			DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.pos);
			DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

			vertices.push_back(v);
		}
	}

	vertices.push_back(bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//
	for (UINT i = 1; i <= slices; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	u_int baseIndex = 1;
	u_int ringVertexCount = slices + 1;
	for (u_int i = 0; i < stacks - 2; ++i)
	{
		u_int i_rvc = i * ringVertexCount;
		u_int i1_rvc = (i + 1)*ringVertexCount;

		for (u_int j = 0; j < slices; ++j)
		{
			indices.push_back(baseIndex + i_rvc + j);
			indices.push_back(baseIndex + i_rvc + j + 1);
			indices.push_back(baseIndex + i1_rvc + j);

			indices.push_back(baseIndex + i1_rvc + j);
			indices.push_back(baseIndex + i_rvc + j + 1);
			indices.push_back(baseIndex + i1_rvc + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	u_int southPoleIndex = (u_int)vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (u_int i = 0; i < slices; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}

	//�o�b�t�@�[�쐬
	CreateBuffer(device, vertices.data(), indices.data(), static_cast<int>(vertices.size()), static_cast<int>(indices.size()));
}

/*memo

struct texture
{
	cher filename[256];
	ID3D11ShaderResoursuUieus * srv;
}

textrue textrue[100];


for(int i = 0;i < 100;i++)
{
	if(textrue[i].filename == filename)
	reture textrue[o].srv;
}

Load textrue

���\�[�X�}�l�[�W���[�́ALOAD�������̂��c��
���\�[�X�}�l�[�W���[��A�|�C���^���I�[��Clear���Ă�I�I�I
*/
