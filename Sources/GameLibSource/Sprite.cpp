#include "misc.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Sprite.h"
#include "Framework.h"

Sprite::Sprite(ID3D11Device* device, const wchar_t* filename)
{
    HRESULT hr{ S_OK };

    // ���_���̃Z�b�g// texcoord�͕`�悷��ʒu�̏��ԉE���
    Vertex vertices[]
    {
    { { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },
    { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },
    { { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },
    { { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },
    };

    // ���_�o�b�t�@�I�u�W�F�N�g�̐���
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = sizeof(vertices);                        //�o�C�g�P�ʂ̃o�b�t�@�T�C�Y
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;              //�o�b�t�@�̓ǂݎ��Ə������݂̕��@�����
                                                          // D3D11_USAGE_DEFAULT GPU�ɂ��ǂݎ�肨�Ƃя������݃A�N�Z�X��K�v�Ƃ��郊�\�[�X
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //�o�b�t�@���o�C���h�������@�����
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  //CPU�A�N�Z�X�t���OD3D11_CPU_ACCESS_WRITE�@�s�v�͂O
    bufferDesc.MiscFlags = 0;                                             // ���̑��̃t���OD3D11_RESOURCE_MISC_FLAG�@�s�v�͂O
    bufferDesc.StructureByteStride = 0;                              // �o�b�t�@�[���\�����o�b�t�@�[��\���ꍇ�́A�o�b�t�@�[�\�����̊e�v�f�̃T�C�Y

    D3D11_SUBRESOURCE_DATA subresourceData{};
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0;                 //�e�N�X�`����1�s�̐擪���玟�̍s�܂ł̋����i�o�C�g�P�ʁj
    subresourceData.SysMemSlicePitch = 0;          //����[�x���x���̊J�n���玟�̐[�x���x���܂ł̋����i�o�C�g�P�ʁj
     //�@���_�V�F�[�_�I�u�W�F�N�g�̐���
    hr = device->CreateBuffer(
        &bufferDesc,                                               // �o�b�t�@�[���L�q����D3D11_BUFFER_DESC�\���̂ւ̃|�C���^�[
        &subresourceData,                                      // �������f�[�^���L�q����D3D11_SUBRESOURCE_DATA�\���̂ւ̃|�C���^�[
        vertexBuffer.GetAddressOf()                        // �쐬���ꂽ�o�b�t�@ �I�u�W�F�N�g��ID3D11Buffer�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // ���̓��C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
       D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
       D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
       D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    ResourceManager::CreateVsFromCso(device, "./Data/Shader/SpriteVS.cso", vertexShader.GetAddressOf(),
        inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
    ResourceManager::CreatePsFromCso(device, "./Data/Shader/SpritePS.cso", pixelShader.GetAddressOf());

    Texture::LoadTextureFromFile(device, filename, shaderResourceView.GetAddressOf(), &texture2dDesc);
}

Sprite::Sprite(ID3D11Device* device, const wchar_t* fileName,int aaaa, int nBufSize)
{
    //VertexBuffer�̍쐬
    //vertex vertices[4] = {};
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * nBufSize;   // ���_�o�b�t�@�̃T�C�Y
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // ���_�o�b�t�@
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // GPU���ǂݎ��̂݁@CPU���������݂̂�
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;
    //D3D11_SUBRESOURCE_DATA initData = {};
    //initData.pSysMem = vertices;                // ���_�̃A�h���X
    //initData.SysMemPitch = 0;
    //initData.SysMemSlicePitch = 0;
    if (FAILED(device->CreateBuffer(&bd, nullptr, &vertexBuffer)))
    {
        assert(!"���_�o�b�t�@�̍쐬�Ɏ��s(Sprite)");
        return;
    }

    // ���_�錾
    // ���̓��C�A�E�g�̒�`
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,     D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 4 * 7, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    //	���_�V�F�[�_�[�̓ǂݍ���
    ResourceManager::CreateVsFromCso(device, "./Data/Shader/SpriteVS.cso", vertexShader.GetAddressOf(),
        inputLayout.GetAddressOf(), layout, _countof(layout));
    ResourceManager::CreatePsFromCso(device, "./Data/Shader/SpritePS.cso", pixelShader.GetAddressOf());

    //	���X�^���C�U�X�e�[�g�̐ݒ�
    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID;//
    rsDesc.CullMode = D3D11_CULL_NONE;//	�J�����O
    rsDesc.FrontCounterClockwise = false;
    rsDesc.DepthBias = 0;
    rsDesc.DepthBiasClamp = 0;
    rsDesc.SlopeScaledDepthBias = 0;
    rsDesc.DepthClipEnable = false;
    rsDesc.ScissorEnable = false;
    rsDesc.MultisampleEnable = false;
    rsDesc.AntialiasedLineEnable = false;
    if (FAILED(device->CreateRasterizerState(&rsDesc, &rasterizerState)))
    {
        assert(!"���X�^���C�U�X�e�[�g�̍쐬�Ɏ��s(Sprite)");
        return;
    }

    //	�e�N�X�`���摜�ǂݍ���
    Texture::LoadTextureFromFile(device, fileName, shaderResourceView.GetAddressOf(), &texture2dDesc);


    //	�T���v���[�X�e�[�g�ݒ�
    D3D11_SAMPLER_DESC	samplerDesc;
    SecureZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    if (FAILED(device->CreateSamplerState(&samplerDesc, &samplerState)))
    {
        assert(!"�T���v���[�X�e�[�g�̐����Ɏ��s(Sprite)");
        return;
    }

    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.StencilEnable = FALSE;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    if (FAILED(device->CreateDepthStencilState(&dsDesc, &depthStencilState)))
    {
        assert(!"�f�v�X�X�e���V���X�e�[�g�̍쐬�Ɏ��s(Sprite)");
        return;
    }
}

Sprite::Sprite(ID3D11Device* device)
{
    HRESULT hr = S_OK;
    // Get Device ********************************************************************

    //Microsoft::WRL::ComPtr<ID3D11Device> device = Framework::GetInstance().GetDevice();

    // *******************************************************************************

    // Create Vertex Buffer **********************************************************

    Vertex vertices[] = {
        { DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
        { DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
        { DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
        { DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
    };


    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(Vertex) * 4;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = vertices;

    hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
    assert(!hr && "CreateBuffer		Error");

    // *******************************************************************************

    // Create Vertex Shader and Input Layout *****************************************

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    ResourceManager::CreateVsFromCso(device, "./Data/Shader/SpriteVS.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));

    // *******************************************************************************

    // Create Pixel Shader ***********************************************************

    ResourceManager::CreatePsFromCso(device, "./Data/Shader/SpritePS.cso", pixelShader.GetAddressOf());

    // *******************************************************************************

    // Create States *****************************************************************

    CreateStates(device);

    // ******************************************************************************
}

void Sprite::Render(ID3D11DeviceContext* immediateContext,
    Vector2 position, Vector2 scale, Vector2 size,
    float sx, float sy, float sw, float sh,
    float angle/*degree*/,
    Vector4 color)
{
    if (scale.x == 0.0f || scale.y == 0.0f) return;

    // �X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾����
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediateContext->RSGetViewports(&num_viewports, &viewport);

    // �X�N���[�����W�n
    // �X�v���C�g���\������S���_�̃X�N���[�����W���v�Z����
    DirectX::XMFLOAT2 positions[] = {
        DirectX::XMFLOAT2(position.x,      position.y),		                        // ����
        DirectX::XMFLOAT2(position.x + size.x, position.y),	                // �E��
        DirectX::XMFLOAT2(position.x, position.y + size.y),   	            // ����
        DirectX::XMFLOAT2(position.x + size.x, position.y + size.y),	// �E��
    };

    // �X�v���C�g���\������S���_�̃e�N�X�`�����W���v�Z����
    DirectX::XMFLOAT2 texcoords[] = {
        DirectX::XMFLOAT2(sx,      sy),			// ����
        DirectX::XMFLOAT2(sx + sw, sy),		// �E��
        DirectX::XMFLOAT2(sx,      sy + sh),	// ����
        DirectX::XMFLOAT2(sx + sw, sy + sh),// �E��
    };

    // �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mappedSubresource{}; // �T�u���\�[�X�f�[�^�ւ̃A�N�Z�X���
    hr = immediateContext->Map(
        vertexBuffer.Get(),                           // ID3D11Resource�C���^�[�t�F�[�X�ւ̃|�C���^�[
        0,                                                          // �C���f�b�N�X�ԍ�
        D3D11_MAP_WRITE_DISCARD,  // ���\�[�X�ɑ΂���CPU�̓ǂݎ�肨��я������݌������w�肷��D3D11_MAP�^�C�v�̒l
        0,                                                          // GPU���r�W�[�̂Ƃ���CPU���������邩���w��
        &mappedSubresource                   // �}�b�v���ꂽ�T�u���\�[�X��D3D11_MAPPED_SUBRESOURCE�\���̂ւ̃|�C���^�[
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    Vertex* vertices{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };
    if (vertices != nullptr)
    {
        auto rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            // Translate sprite's centre to origin (rotate centre)
            x -= cx;
            y -= cy;

            // Rotate each sprite's vertices by angle
            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx + -sin * ty;
            y = sin * tx + cos * ty;

            // Translate sprite's centre to original position
            x += cx;
            y += cy;
        };
        //��]�̒��S����`�̒��S�_�ɂ����ꍇ
        float cx = position.x + size.x * 0.5f; //���S���v�Z����
        float cy = position.y + size.y * 0.5f;

        // ��]�ʂɍ��킹������
        for (auto& p : positions) rotate(p.x, p.y, cx, cy, angle);

        // �X�N���[�����W�n����NDC�ւ̍��W�ϊ��������Ȃ�
        for (auto& p : positions) {
            p.x = 2.0f * p.x / viewport.Width - 1.0f;
            p.y = 1.0f - 2.0f * p.y / viewport.Height;
        }

        for (int i = 0; i < 4; i++) {
            vertices[i].position = { positions[i].x, positions[i].y, 0 };
            vertices[i].color = { color.x, color.y, color.z, color.w };
            vertices[i].texcoord = { texcoords[i].x / texture2dDesc.Width, texcoords[i].y / texture2dDesc.Height };
        }
        //int num = 0;
        //for (auto& p : positions) {
        //    vertices[num].position = { p.x, p.y, 0 };
        //    vertices[num].color = { r, g, b, a };
        //    num++;
        //}
    }
    immediateContext->Unmap(vertexBuffer.Get(), 0); // ���\�[�X�ւ̃|�C���^�[�𖳌��ɂ��A���̃��\�[�X�ւ� GPU �̃A�N�Z�X���ēx�L���ɂ�����

    {
        // ���_�o�b�t�@�[�̃o�C���h
        UINT stride{ sizeof(Vertex) };
        UINT offset{ 0 };
        immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

        // �v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ���f�[�^�̃o�C���h
        //�v���~�e�B�u�^�Ɋւ�����ƁA���̓A�Z���u���X�e�[�W�̓��̓f�[�^���L�q����f�[�^�������o�C���h
        immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        // ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
        immediateContext->IASetInputLayout(inputLayout.Get());

        // �V�F�[�_�̃o�C���h
        immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
        immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

        // �V�F�[�_�[ ���\�[�X�̃o�C���h Texture��SamplerState��
        immediateContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

        // �v���~�e�B�u�̕`��
        immediateContext->Draw(4, 0);
    }
}

void Sprite::Render( ID3D11DeviceContext* immediateContext,
    Vector2 position, Vector2 scale, Vector2 size,
    float sx, float sy, float sw, float sh,
    float angle/*degree*/)
{
    Render(immediateContext, position, scale, size, sx, sy, sw, sh, angle, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void Sprite::Render(ID3D11DeviceContext* immediateContext,
    Vector2 position, Vector2 scale, Vector2 size,
    float angle/*degree*/)
{
    Render(immediateContext,
        position, scale, size,
        0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height),
        angle,
        {1.0f, 1.0f, 1.0f, 1.0f});
}

void Sprite::Render(ID3D11DeviceContext* immediateContext,
    Vector2 position, Vector2 scale, Vector2 size)
{
    Render(immediateContext,
        position, scale, size,
        0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height),
        0.0f,
        {1.0f, 1.0f, 1.0f, 1.0f});
}

void Sprite::Texout(ID3D11DeviceContext* immediateContext,
    std::string s,
    float x, float y, float w, float h, Vector4 color)
{
    float sw = static_cast<float>(texture2dDesc.Width / 16);
    float sh = static_cast<float>(texture2dDesc.Height / 16);

    float cursor = 0;

    for (auto c : s)
    {
        LONG sx = c % 0x0F;
        Render(immediateContext, { x + cursor, y }, {1.0f, 1.0f}, { w, h }, sw* (c & 0x0F), sh* (c >> 4), sw, sh, 0, color);
        cursor += w;
    }
}

void Sprite::CreateStates(ID3D11Device* device)
{

    HRESULT hr = S_OK;


    // Create Sampler State **********************************************************

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    //samplerDesc.Filter			= D3D11_FILTER_ANISOTROPIC;
    //samplerDesc.AddressU		= D3D11_TEXTURE_ADDRESS_BORDER;
    //samplerDesc.AddressV		= D3D11_TEXTURE_ADDRESS_BORDER;
    //samplerDesc.AddressW		= D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    //samplerDesc.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
    assert(!hr && "CreateSamplerState	Error");

    // *******************************************************************************

    // Create Rasterizer State *******************************************************

    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.DepthClipEnable = FALSE;

    hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
    assert(!hr && "CreateRasterizerState	Error");

    // *******************************************************************************

    // Create Depth Stencil State ****************************************************

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = FALSE;
    depthStencilDesc.StencilEnable = FALSE;

    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
    assert(!hr && "CreateDepthStencilState		Error");

    // *******************************************************************************

}

#include <utility>
void Sprite::render(ID3D11DeviceContext* context,
    const Vector2& position, const Vector2& scale,
    const Vector2& texPos, const Vector2& texSize,
    const Vector2& center, float angle,
    const Vector4& color) const
{
    if (scale.x == 0.0f || scale.y == 0.0f) return;

    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport);

    float tw = texSize.x;
    float th = texSize.y;
    if (tw == FLT_MIN && th == FLT_MIN)
    {
        tw = (float)texture2dDesc.Width;
        th = (float)texture2dDesc.Height;
    }

    Vertex vertices[4] = {};
#ifdef GAMELIB_PLUS_UP
    vertices[0] = { VECTOR3(0.0f, 1.0f, 0), color, VECTOR2(0, 0) };
    vertices[1] = { VECTOR3(1.0f, 1.0f, 0), color, VECTOR2(1, 0) };
    vertices[2] = { VECTOR3(0.0f, 0.0f, 0), color, VECTOR2(0, 1) };
    vertices[3] = { VECTOR3(1.0f, 0.0f, 0), color, VECTOR2(1, 1) };
#else
    vertices[0] = { Vector3(0.0f, 1.0f, 0), color, Vector2(0, 1) };
    vertices[1] = { Vector3(1.0f, 1.0f, 0), color, Vector2(1, 1) };
    vertices[2] = { Vector3(0.0f, 0.0f, 0), color, Vector2(0, 0) };
    vertices[3] = { Vector3(1.0f, 0.0f, 0), color, Vector2(1, 0) };
#endif

    float sinValue = sinf(angle);
    float cosValue = cosf(angle);
    float mx = (tw * scale.x) / tw * center.x;
    float my = (th * scale.y) / th * center.y;
    for (int i = 0; i < 4; i++)
    {
        vertices[i].position.x *= (tw * scale.x);
        vertices[i].position.y *= (th * scale.y);

        vertices[i].position.x -= mx;
        vertices[i].position.y -= my;

        float rx = vertices[i].position.x;
        float ry = vertices[i].position.y;
        vertices[i].position.x = rx * cosValue - ry * sinValue;
        vertices[i].position.y = rx * sinValue + ry * cosValue;

        vertices[i].position.x += mx;
        vertices[i].position.y += my;

        vertices[i].position.x += (position.x - scale.x * center.x);
        vertices[i].position.y += (position.y - scale.y * center.y);

        vertices[i].position.x = -1.0f + vertices[i].position.x * 2 / viewport.Width;
        vertices[i].position.y = 1.0f - vertices[i].position.y * 2 / viewport.Height;
#ifdef GAMELIB_PLUS_UP
        vertices[i].position.y = -vertices[i].position.y;
#endif

        // UV���W�̒���
        vertices[i].texcoord.x = (std::min)(vertices[i].texcoord.x, UV_ADJUST);
        vertices[i].texcoord.y = (std::min)(vertices[i].texcoord.y, UV_ADJUST);

        vertices[i].texcoord.x = (texPos.x + vertices[i].texcoord.x * tw) / texture2dDesc.Width;
        vertices[i].texcoord.y = (texPos.y + vertices[i].texcoord.y * th) / texture2dDesc.Height;
    }

    D3D11_MAPPED_SUBRESOURCE msr;
    context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    memcpy(msr.pData, vertices, sizeof(vertices));
    context->Unmap(vertexBuffer.Get(), 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->IASetInputLayout(inputLayout.Get());
    context->RSSetState(rasterizerState.Get());
    context->VSSetShader(vertexShader.Get(), nullptr, 0);
    context->PSSetShader(pixelShader.Get(), nullptr, 0);

    context->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
    context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

    context->OMSetDepthStencilState(depthStencilState.Get(), 1);

    context->Draw(4, 0);
}