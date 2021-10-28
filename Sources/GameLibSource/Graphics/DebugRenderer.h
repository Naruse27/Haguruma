#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
ClassName(const ClassName&); \
ClassName& operator=(const ClassName&); \
ClassName(const ClassName&&); \
ClassName& operator=(const ClassName&&);

class DebugRenderer
{
private:
	DebugRenderer() = default;
	~DebugRenderer() = default;
	inline static DebugRenderer* instance;

public:
	DISALLOW_COPY_AND_ASSIGN(DebugRenderer);

	static DebugRenderer& Instance() // �V���O���g��
	{
		return *instance;
	}

	// DebugRenderer����
	static void Create() {
		if (!instance) instance = new DebugRenderer;
	}

	// DebugRenderer�폜
	static void Destory() {
		delete instance;
		instance = nullptr;
	}

	// ������
	void Init(ID3D11Device* device);

	// �`����s
	void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	// ���`��
	void DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color);

	// �~���`��
	void DrawCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color);

	// myAdd
	void CreateStageMainMesh(ID3D11Device* device, std::vector<DirectX::XMFLOAT3>& vertices);

	void CreateStageMoveMesh(ID3D11Device* device, std::vector<DirectX::XMFLOAT3>& vertices);

private:
	// �����b�V���쐬
	void CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks);

	// �~�����b�V���쐬
	void CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks);

private:
	struct CbMesh
	{
		DirectX::XMFLOAT4X4	wvp;
		DirectX::XMFLOAT4	color;
	};

	struct Sphere
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	center;
		float				radius;
	};

	struct Cylinder
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	position;
		float				radius;
		float				height;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer>			sphereVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cylinderVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		    stageMeshVertexBufferMain;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		    stageMeshVertexBufferMove;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	std::vector<Sphere>		spheres;
	std::vector<Cylinder>	cylinders;

	UINT	sphereVertexCount = 0;
	UINT	cylinderVertexCount = 0;

    // myADD
    UINT stageMeshVertexCount = 0;
	UINT stageMeshVertexCountMove = 0;
	bool stageMeshDraw = false;
	bool stageMeshDrawMove = false;
};
