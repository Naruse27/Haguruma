#include "misc.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "SpriteBatch.h"

SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t maxSprites) : maxVertices(maxSprites * 6)
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
    bufferDesc.ByteWidth = sizeof(Vertex) * maxVertices;                        //バイト単位のバッファサイズ
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

    // シェーダーのバインド
    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    replacedPixelShader ? immediateContext->PSSetShader(replacedPixelShader, nullptr, 0) : immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    // シェーダー リソースのバインド
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
    D3D11_MAPPED_SUBRESOURCE mappedSubresource{}; // サブリソースデータへのアクセスを提供
    hr = immediateContext->Map(
        vertexBuffer.Get(),     // ID3D11Resourceインターフェースへのポインター
        0,                       // インデックス番号
        D3D11_MAP_WRITE_DISCARD, // リソースに対するCPUの読み取りおよび書き込み権限を指定するD3D11_MAPタイプの値
        0,                       // GPUがビジーのときにCPUが何をするかを指定
        &mappedSubresource      // マップされたサブリソースのD3D11_MAPPED_SUBRESOURCE構造体へのポインター
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

    // 頂点バッファーのバインド
    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };
    immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

    // プリミティブタイプおよびデータの順序に関する情報のバインド
    //プリミティブ型に関する情報と、入力アセンブラステージの入力データを記述するデータ順序をバインド
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 入力レイアウトオブジェクトのバインド
    immediateContext->IASetInputLayout(inputLayout.Get());

    // プリミティブの描画
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
    // スクリーン（ビューポート）のサイズを取得する
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediateContext->RSGetViewports(&num_viewports, &viewport);

    // スクリーン座標系
    // スプライトを構成する４頂点のスクリーン座標を計算する
    DirectX::XMFLOAT2 positions[] = {
        DirectX::XMFLOAT2(dx,      dy),		        // 左上
        DirectX::XMFLOAT2(dx + dw, dy),			// 右上
        DirectX::XMFLOAT2(dx,      dy + dh),   	// 左下
        DirectX::XMFLOAT2(dx + dw, dy + dh),	// 右下
    };

    // スプライトを構成する４頂点のテクスチャ座標を計算する
    DirectX::XMFLOAT2 texcoords[] = {
        DirectX::XMFLOAT2(sx,      sy),			// 左上
        DirectX::XMFLOAT2(sx + sw, sy),		// 右上
        DirectX::XMFLOAT2(sx,      sy + sh),	// 左下
        DirectX::XMFLOAT2(sx + sw, sy + sh),// 右下
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
    //回転の中心を矩形の中心点にした場合
    float cx = dx + dw * 0.5f; //中心を計算する
    float cy = dy + dh * 0.5f;

    // 回転量に合わせた調整
    for (auto& p : positions) rotate(p.x, p.y, cx, cy, angle);

    // スクリーン座標系からNDCへの座標変換をおこなう
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
//immediateContext->Unmap(vertexBuffer.Get(), 0); // リソースへのポインターを無効にし、そのリソースへの GPU のアクセスを再度有効にしする
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