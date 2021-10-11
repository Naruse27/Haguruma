#include "misc.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "SpriteBatch.h"

SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t maxSprites) : maxVertices(maxSprites * 6)
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
    bufferDesc.ByteWidth = sizeof(Vertex) * maxVertices;                        //�o�C�g�P�ʂ̃o�b�t�@�T�C�Y
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
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
       D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
       D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
       D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    ResourceManager::CreateVsFromCso(device, "./Library/Shader/SpriteVS.cso", vertexShader.GetAddressOf(),
        inputLayout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
    ResourceManager::CreatePsFromCso(device, "./Library/Shader/SpritePS.cso", pixelShader.GetAddressOf());

    Texture::LoadTextureFromFile(device, filename, shaderResourceView.GetAddressOf(), &texture2dDesc);

    Texture::ReleaseAllTextures();
}

void SpriteBatch::Begin(ID3D11DeviceContext* immediateContext, ID3D11PixelShader* replacedPixelShader, ID3D11ShaderResourceView* replacedResourceShader)
{
    vertices.clear();

    // �V�F�[�_�[�̃o�C���h
    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    replacedPixelShader ? immediateContext->PSSetShader(replacedPixelShader, nullptr, 0) : immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    // �V�F�[�_�[ ���\�[�X�̃o�C���h
    if (replacedResourceShader) {
        HRESULT hr{ S_OK };
        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        replacedResourceShader->GetResource(resource.GetAddressOf());

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
        hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        texture2d->GetDesc(&texture2dDesc);

        immediateContext->PSSetShaderResources(0, 1, &replacedResourceShader);
    }
    else immediateContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
}

void  SpriteBatch::End(ID3D11DeviceContext* immediateContext)
{
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mappedSubresource{}; // �T�u���\�[�X�f�[�^�ւ̃A�N�Z�X���
    hr = immediateContext->Map(
        vertexBuffer.Get(),     // ID3D11Resource�C���^�[�t�F�[�X�ւ̃|�C���^�[
        0,                       // �C���f�b�N�X�ԍ�
        D3D11_MAP_WRITE_DISCARD, // ���\�[�X�ɑ΂���CPU�̓ǂݎ�肨��я������݌������w�肷��D3D11_MAP�^�C�v�̒l
        0,                       // GPU���r�W�[�̂Ƃ���CPU���������邩���w��
        &mappedSubresource      // �}�b�v���ꂽ�T�u���\�[�X��D3D11_MAPPED_SUBRESOURCE�\���̂ւ̃|�C���^�[
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    size_t vertexCount = vertices.size();
    _ASSERT_EXPR(maxVertices >= vertexCount, "Buffer overflow");
    Vertex* data{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };
    if (data != nullptr) {
        const Vertex* p = vertices.data();
        memcpy_s(data, maxVertices * sizeof(Vertex), p, vertexCount * sizeof(Vertex));
    }
    immediateContext->Unmap(vertexBuffer.Get(), 0);

    // ���_�o�b�t�@�[�̃o�C���h
    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };
    immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

    // �v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
    //�v���~�e�B�u�^�Ɋւ�����ƁA���̓A�Z���u���X�e�[�W�̓��̓f�[�^���L�q����f�[�^�������o�C���h
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
    immediateContext->IASetInputLayout(inputLayout.Get());

    // �v���~�e�B�u�̕`��
    immediateContext->Draw(static_cast<UINT>(vertexCount), 0);

    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    shaderResourceView.Get()->GetResource(resource.GetAddressOf());

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    texture2d->GetDesc(&texture2dDesc);
}

void SpriteBatch::Render(ID3D11DeviceContext* immediateContext,
    float dx, float dy, float dw, float dh,
    float sx, float sy, float sw, float sh,
    float angle/*degree*/,
    float r, float g, float b, float a)
{
    // �X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾����
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediateContext->RSGetViewports(&num_viewports, &viewport);

    // �X�N���[�����W�n
    // �X�v���C�g���\������S���_�̃X�N���[�����W���v�Z����
    DirectX::XMFLOAT2 positions[] = {
        DirectX::XMFLOAT2(dx,      dy),		        // ����
        DirectX::XMFLOAT2(dx + dw, dy),			// �E��
        DirectX::XMFLOAT2(dx,      dy + dh),   	// ����
        DirectX::XMFLOAT2(dx + dw, dy + dh),	// �E��
    };

    // �X�v���C�g���\������S���_�̃e�N�X�`�����W���v�Z����
    DirectX::XMFLOAT2 texcoords[] = {
        DirectX::XMFLOAT2(sx,      sy),			// ����
        DirectX::XMFLOAT2(sx + sw, sy),		// �E��
        DirectX::XMFLOAT2(sx,      sy + sh),	// ����
        DirectX::XMFLOAT2(sx + sw, sy + sh),// �E��
    };

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
    float cx = dx + dw * 0.5f; //���S���v�Z����
    float cy = dy + dh * 0.5f;

    // ��]�ʂɍ��킹������
    for (auto& p : positions) rotate(p.x, p.y, cx, cy, angle);

    // �X�N���[�����W�n����NDC�ւ̍��W�ϊ��������Ȃ�
    for (auto& p : positions) {
        p.x = 2.0f * p.x / viewport.Width - 1.0f;
        p.y = 1.0f - 2.0f * p.y / viewport.Height;
    }

    float u0{ sx / texture2dDesc.Width };
    float v0{ sy / texture2dDesc.Height };
    float u1{ (sx + sw) / texture2dDesc.Width };
    float v1{ (sy + sh) / texture2dDesc.Height };

    vertices.push_back({ { positions[0].x, positions[0].y, 0 }, { r, g, b, a }, { u0, v0 } });
    vertices.push_back({ { positions[1].x, positions[1].y, 0 }, { r, g, b, a }, { u1, v0 } });
    vertices.push_back({ { positions[2].x, positions[2].y, 0 }, { r, g, b, a }, { u0, v1 } });
    vertices.push_back({ { positions[2].x, positions[2].y, 0 }, { r, g, b, a }, { u0, v1 } });
    vertices.push_back({ { positions[1].x, positions[1].y, 0 }, { r, g, b, a }, { u1, v0 } });
    vertices.push_back({ { positions[3].x, positions[3].y, 0 }, { r, g, b, a }, { u1, v0 } });

    //for (int i = 0; i < 4; i++) {
    //    vertices[i].position = { positions[i].x, positions[i].y, 0 };
    //    vertices[i].color = { r, g, b, a };
    //    vertices[i].texcoord = { texcoords[i].x / texture2dDesc.Width, texcoords[i].y / texture2dDesc.Height };
    //}
    //int num = 0;
    //for (auto& p : positions) {
    //    vertices[num].position = { p.x, p.y, 0 };
    //    vertices[num].color = { r, g, b, a };
    //    num++;
    //}
//immediateContext->Unmap(vertexBuffer.Get(), 0); // ���\�[�X�ւ̃|�C���^�[�𖳌��ɂ��A���̃��\�[�X�ւ� GPU �̃A�N�Z�X���ēx�L���ɂ�����
}

void SpriteBatch::Render(ID3D11DeviceContext* immediateContext,
    float dx, float dy, float dw, float dh,
    float sx, float sy, float sw, float sh,
    float angle/*degree*/)
{
    Render(immediateContext, dx, dy, dw, dh, sx, sy, sw, sh, angle, 1.0f, 1.0f, 1.0f, 1.0f);
}

void SpriteBatch::Render(ID3D11DeviceContext* immediateContext,
    float dx, float dy, float dw, float dh,
    float angle/*degree*/)
{
    Render(immediateContext,
        dx, dy, dw, dh,
        0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height),
        angle,
        1.0f, 1.0f, 1.0f, 1.0f);
}

void SpriteBatch::Render(ID3D11DeviceContext* immediateContext,
    float dx, float dy, float dw, float dh)
{
    Render(immediateContext,
        dx, dy, dw, dh,
        0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height),
        0.0f,
        1.0f, 1.0f, 1.0f, 1.0f);
}