#ifndef RESOURCEMANAGER
#define RESOURCEMANAGER

#include "misc.h"
#include <d3d11.h>

class ResourceManager
{
public:
    // VertexShader <- CSO
    static void CreateVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader,
        ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);

    // PixelShader <- CSO
    static void CreatePsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader);

	static void CreateCsFromCso(ID3D11Device* device, const char* csoName, ID3D11ComputeShader** computeShader);
};


#endif // !RESOURCEMANAGER
