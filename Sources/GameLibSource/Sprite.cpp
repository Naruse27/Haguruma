#include "misc.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Sprite.h"
#include "Framework.h"

Sprite::Sprite(ID3D11Device* device, const wchar_t* filename)
{
    HRESULT hr{ S_OK };

    // 頂点情報のセット// texcoordは描画する位置の順番右回り
    Vertex vertices[]
    {
    { { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },
    { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },
    { { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },
    { { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },
    };

    // 頂点バッファオブジェクトの生成
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = sizeof(vertices);                        //バイト単位のバッファサイズ
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;              //バッファの読み取りと書き込みの方法を特定
                                                          // D3D11_USAGE_DEFAULT GPUによる読み取りおとび書き込みアクセスを必要とするリソース
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //バッファがバインドされる方法を特定
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  //CPUアクセスフラグD3D11_CPU_ACCESS_WRITE　不要は０
    bufferDesc.MiscFlags = 0;                                             // その他のフラグD3D11_RESOURCE_MISC_FLAG　不要は０
    bufferDesc.StructureByteStride = 0;                              // バッファーが構造化バッファーを表す場合の、バッファー構造内の各要素のサイズ

    D3D11_SUBRESOURCE_DATA subresourceData{};
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0;                 //テクスチャの1行の先頭から次の行までの距離（バイト単位）
    subresourceData.SysMemSlicePitch = 0;          //ある深度レベルの開始から次の深度レベルまでの距離（バイト単位）
     //　頂点シェーダオブジェクトの生成
    hr = device->CreateBuffer(
        &bufferDesc,                                               // バッファーを記述するD3D11_BUFFER_DESC構造体へのポインター
        &subresourceData,                                      // 初期化データを記述するD3D11_SUBRESOURCE_DATA構造体へのポインター
        vertexBuffer.GetAddressOf()                        // 作成されたバッファ オブジェクトのID3D11Bufferインターフェイスへのポインタのアドレス
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // 入力レイアウトオブジェクトの生成
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
    Vector2 position, Vector2 size,
    float sx, float sy, float sw, float sh,
    float angle/*degree*/,
    Vector4 color)
{
    // スクリーン（ビューポート）のサイズを取得する
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediateContext->RSGetViewports(&num_viewports, &viewport);

    // スクリーン座標系
    // スプライトを構成する４頂点のスクリーン座標を計算する
    DirectX::XMFLOAT2 positions[] = {
        DirectX::XMFLOAT2(position.x,      position.y),		                        // 左上
        DirectX::XMFLOAT2(position.x + size.x, position.y),	                // 右上
        DirectX::XMFLOAT2(position.x, position.y + size.y),   	            // 左下
        DirectX::XMFLOAT2(position.x + size.x, position.y + size.y),	// 右下
    };

    // スプライトを構成する４頂点のテクスチャ座標を計算する
    DirectX::XMFLOAT2 texcoords[] = {
        DirectX::XMFLOAT2(sx,      sy),			// 左上
        DirectX::XMFLOAT2(sx + sw, sy),		// 右上
        DirectX::XMFLOAT2(sx,      sy + sh),	// 左下
        DirectX::XMFLOAT2(sx + sw, sy + sh),// 右下
    };

    // 計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mappedSubresource{}; // サブリソースデータへのアクセスを提供
    hr = immediateContext->Map(
        vertexBuffer.Get(),                     // ID3D11Resourceインターフェースへのポインター
        0,                                             // インデックス番号
        D3D11_MAP_WRITE_DISCARD,  // リソースに対するCPUの読み取りおよび書き込み権限を指定するD3D11_MAPタイプの値
        0,                                             // GPUがビジーのときにCPUが何をするかを指定
        &mappedSubresource                // マップされたサブリソースのD3D11_MAPPED_SUBRESOURCE構造体へのポインター
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
        //回転の中心を矩形の中心点にした場合
        float cx = position.x + size.x * 0.5f; //中心を計算する
        float cy = position.y + size.y * 0.5f;

        // 回転量に合わせた調整
        for (auto& p : positions) rotate(p.x, p.y, cx, cy, angle);

        // スクリーン座標系からNDCへの座標変換をおこなう
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
    immediateContext->Unmap(vertexBuffer.Get(), 0); // リソースへのポインターを無効にし、そのリソースへの GPU のアクセスを再度有効にしする

    {
        // 頂点バッファーのバインド
        UINT stride{ sizeof(Vertex) };
        UINT offset{ 0 };
        immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

        // プリミティブタイプおよびデータの順序に関するデータのバインド
        //プリミティブ型に関する情報と、入力アセンブラステージの入力データを記述するデータ順序をバインド
        immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        // 入力レイアウトオブジェクトのバインド
        immediateContext->IASetInputLayout(inputLayout.Get());

        // シェーダのバインド
        immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
        immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

        // シェーダー リソースのバインド TextureをSamplerStateへ
        immediateContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

        // プリミティブの描画
        immediateContext->Draw(4, 0);
    }
}

void Sprite::Render( ID3D11DeviceContext* immediateContext,
    Vector2 position, Vector2 size,
    float sx, float sy, float sw, float sh,
    float angle/*degree*/)
{
    Render(immediateContext, position, size, sx, sy, sw, sh, angle, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void Sprite::Render(ID3D11DeviceContext* immediateContext,
    Vector2 position, Vector2 size,
    float angle/*degree*/)
{
    Render(immediateContext,
        position, size,
        0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height),
        angle,
        {1.0f, 1.0f, 1.0f, 1.0f});
}

void Sprite::Render(ID3D11DeviceContext* immediateContext,
    Vector2 position, Vector2 size)
{
    Render(immediateContext,
        position, size,
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
        Render(immediateContext, { x + cursor, y }, { w, h }, sw * (c & 0x0F), sh * (c >> 4), sw, sh, 0, color);
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