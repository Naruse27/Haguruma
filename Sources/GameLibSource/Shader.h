#ifndef SHADER
#define SHADER

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

class Shader
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	geometryShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

public:
	Shader() {}
	~Shader() {}

public:
	void LoadCSO(ID3D11Device* device,
		const char* csoNameOfVertexShader, const char* csoNameOfPixelShader,
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements,
		const char* csoNameOfGeometryShader = nullptr);
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		GetVertexShader() { return vertexShader; }
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		GetPixelShader() { return pixelShader; }
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	GetGeometryShader() { return geometryShader; }
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		GetInputLayout() { return inputLayout; }
};

class ShaderSystem
{
public:
	enum ShaderOfSkinnedMesh
	{
		DEFAULT,
		PHONE,
		NORMAL_MAP,
		END
	};

private:
	Shader shaderOfSkinnedMesh[ShaderOfSkinnedMesh::END];
	Shader shaderOfSkinnedMeshBatch;
	Shader shaderOfStaticMeshBatch;
	Shader shadowMapShader;
	Shader destructionShader;

public:
	ShaderSystem() {}
	~ShaderSystem() {}

	static ShaderSystem* GetInstance()
	{
		static ShaderSystem instance;
		return &instance;
	}

public:
	void Init(ID3D11Device* device);
	void UnInit() {}
	Shader GetShaderOfSkinnedMesh(ShaderOfSkinnedMesh no) { return shaderOfSkinnedMesh[no]; }
	Shader GetShaderOfSkinnedMeshBatch() { return shaderOfSkinnedMeshBatch; }
	Shader GetShaderOfStaticMeshBatch() { return shaderOfStaticMeshBatch; }
	Shader GetShadowMapShader() { return shadowMapShader; }
	Shader GetDestructionShader() { return destructionShader; }
};

#endif // !SHADER
