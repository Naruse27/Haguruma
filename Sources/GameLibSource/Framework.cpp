#include "Framework.h"
#include "../GameSources/SceneManager.h"
#include "../GameSources/SceneTitle.h"
#include "Shader.h"
#include "FileLoad.h"
//#include </
//#include <DirectXMath.h>
#include "FontManager.h"
#include "../GameSources/CameraManager.h"
#include "GameSources/EffectManager.h"

EffectManager* EffectManager::instance = nullptr;


Framework::Framework(HWND hwnd) : hwnd(hwnd) , input(hwnd)
{
}

bool Framework::Initialize()
{
    {
        HRESULT hr = S_OK;

        UINT createDeviceFlags = 0;

        createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT; // UNIT.99:Required to use Direct2D on DirectX11.
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 

        Microsoft::WRL::ComPtr<IDXGIFactory1> dxgi_factory1;
        hr = CreateDXGIFactory1(IID_PPV_ARGS(dxgi_factory1.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> enum_adapters1;
        Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgi_adapter1;
        for (UINT adapter_index = 0; S_OK == dxgi_factory1->EnumAdapters1(adapter_index, dxgi_adapter1.ReleaseAndGetAddressOf()); ++adapter_index) {
            DXGI_ADAPTER_DESC1 adapter_desc_desc;
            dxgi_adapter1->GetDesc1(&adapter_desc_desc);
            OutputDebugStringW(std::wstring(std::to_wstring(adapter_index) + L':' + adapter_desc_desc.Description + L'\n').c_str());
            OutputDebugStringA(std::string("\tVendorId:" + std::to_string(adapter_desc_desc.VendorId) + '\n').c_str());
            OutputDebugStringA(std::string("\tDeviceId:" + std::to_string(adapter_desc_desc.DeviceId) + '\n').c_str());
            OutputDebugStringA(std::string("\tSubSysId:" + std::to_string(adapter_desc_desc.SubSysId) + '\n').c_str());
            OutputDebugStringA(std::string("\tRevision:" + std::to_string(adapter_desc_desc.Revision) + '\n').c_str());
            OutputDebugStringA(std::string("\tDedicatedVideoMemory:" + std::to_string(adapter_desc_desc.DedicatedVideoMemory) + '\n').c_str());
            OutputDebugStringA(std::string("\tDedicatedSystemMemory:" + std::to_string(adapter_desc_desc.DedicatedSystemMemory) + '\n').c_str());
            OutputDebugStringA(std::string("\tSharedSystemMemory:" + std::to_string(adapter_desc_desc.SharedSystemMemory) + '\n').c_str());
            OutputDebugStringA(std::string("\tAdapterLuid.HighPart:" + std::to_string(adapter_desc_desc.AdapterLuid.HighPart) + '\n').c_str());
            OutputDebugStringA(std::string("\tAdapterLuid.LowPart:" + std::to_string(adapter_desc_desc.AdapterLuid.LowPart) + '\n').c_str());
            OutputDebugStringA(std::string("\tFlags:" + std::to_string(adapter_desc_desc.Flags) + '\n').c_str());
            enum_adapters1.push_back(dxgi_adapter1);
        }
        size_t selected_adapter_index{ 0 };
        dxgi_adapter1 = enum_adapters1.at(selected_adapter_index);

        D3D_FEATURE_LEVEL feature_levels[]
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };
        D3D_FEATURE_LEVEL feature_level;
        hr = D3D11CreateDevice(dxgi_adapter1.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0, createDeviceFlags, feature_levels, _countof(feature_levels), D3D11_SDK_VERSION, device.ReleaseAndGetAddressOf(), &feature_level, deviceContext.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        _ASSERT_EXPR(!(feature_level < D3D_FEATURE_LEVEL_11_0), L"Direct3D Feature Level 11 unsupported.");

        Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory2;
        hr = dxgi_adapter1.Get()->GetParent(IID_PPV_ARGS(&dxgi_factory2));
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1{};
        swap_chain_desc1.Width = SCREEN_WIDTH;
        swap_chain_desc1.Height = SCREEN_HEIGHT;
        swap_chain_desc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swap_chain_desc1.Stereo = FALSE;
        swap_chain_desc1.SampleDesc.Count = 1;
        swap_chain_desc1.SampleDesc.Quality = 0;
        swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc1.BufferCount = 2;
        swap_chain_desc1.Scaling = DXGI_SCALING_STRETCH;
        swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        hr = dxgi_factory2->CreateSwapChainForHwnd(device.Get(), hwnd, &swap_chain_desc1, nullptr, nullptr, swapChain.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        Microsoft::WRL::ComPtr<ID3D11Texture2D> frame_buffer;
        hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&frame_buffer));
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        hr = device->CreateRenderTargetView(frame_buffer.Get(), nullptr, renderTargetView.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        Microsoft::WRL::ComPtr<IDXGIDevice2> dxgi_device2;
        hr = device.As(&dxgi_device2);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        Microsoft::WRL::ComPtr<ID2D1Factory1> d2d_factory1;
        D2D1_FACTORY_OPTIONS factory_options{};

#ifdef _DEBUG
        factory_options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factory_options, d2d_factory1.GetAddressOf());

        Microsoft::WRL::ComPtr<ID2D1Device> d2d_device;
        hr = d2d_factory1->CreateDevice(dxgi_device2.Get(), d2d_device.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, FontManager::Instance().GetID2D1DeviceContext().GetAddressOf());//     d2d1_device_context.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = dxgi_device2->SetMaximumFrameLatency(1);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        Microsoft::WRL::ComPtr<IDXGISurface2> dxgi_surface2;
        hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgi_surface2));
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d_bitmap1;
        hr = FontManager::Instance().GetID2D1DeviceContext()->CreateBitmapFromDxgiSurface(dxgi_surface2.Get(),
            D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)), d2d_bitmap1.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        FontManager::Instance().GetID2D1DeviceContext()->SetTarget(d2d_bitmap1.Get());

        Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory;
        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(dwrite_factory.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = dwrite_factory->CreateTextFormat(
            L"Meiryo",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            48,
            L"",
            FontManager::Instance().GetIDWriteTextFormat().GetAddressOf());

        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        hr = FontManager::Instance().GetIDWriteTextFormat()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = FontManager::Instance().GetID2D1DeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::AntiqueWhite), FontManager::Instance().GetID2D1SolidColorBrush().GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        //深度ステンシルビューの設定
        Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer{};
        D3D11_TEXTURE2D_DESC texture2dDesc{};
        {
            texture2dDesc.Width = SCREEN_WIDTH;
            texture2dDesc.Height = SCREEN_HEIGHT;
            texture2dDesc.MipLevels = 1;
            texture2dDesc.ArraySize = 1;
            texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            texture2dDesc.SampleDesc.Count = 1;
            texture2dDesc.SampleDesc.Quality = 0;
            texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
            texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            texture2dDesc.CPUAccessFlags = 0;
            texture2dDesc.MiscFlags = 0;
        }
        hr = device->CreateTexture2D(&texture2dDesc, NULL, depthStencilBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
        {
            depthStencilViewDesc.Format = texture2dDesc.Format;
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            depthStencilViewDesc.Texture2D.MipSlice = 0;
        }
        hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, this->depthStencilView.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        //ビューポートの設定
        D3D11_VIEWPORT viewport{};
        {
            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            viewport.Width = static_cast<float>(SCREEN_WIDTH);
            viewport.Height = static_cast<float>(SCREEN_HEIGHT);
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
        }
        deviceContext->RSSetViewports(1, &viewport);


        //サンプラーステートオブジェクト生成
        D3D11_SAMPLER_DESC samplerDesc{};
        {
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;//D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;//D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;// D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.MipLODBias = 0;
            samplerDesc.MaxAnisotropy = 16;
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
            samplerDesc.BorderColor[0] = 0;
            samplerDesc.BorderColor[1] = 0;
            samplerDesc.BorderColor[2] = 0;
            samplerDesc.BorderColor[3] = 0;
            samplerDesc.MinLOD = 0;
            samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        }
        hr = device->CreateSamplerState(&samplerDesc, this->samplerStates[0].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        hr = device->CreateSamplerState(&samplerDesc, this->samplerStates[1].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        hr = device->CreateSamplerState(&samplerDesc, this->samplerStates[2].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



        //深度ステンシルステートオブジェクトの作成
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
        {
            depthStencilDesc.DepthEnable = TRUE;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        }
        hr = device->CreateDepthStencilState(&depthStencilDesc, this->depthStencilStates[0].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        {
            depthStencilDesc.DepthEnable = TRUE;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        }
        hr = device->CreateDepthStencilState(&depthStencilDesc, this->depthStencilStates[1].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        {
            depthStencilDesc.DepthEnable = FALSE;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        }
        hr = device->CreateDepthStencilState(&depthStencilDesc, this->depthStencilStates[2].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        {
            depthStencilDesc.DepthEnable = FALSE;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        }
        hr = device->CreateDepthStencilState(&depthStencilDesc, this->depthStencilStates[3].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        //ラスタライザステートオブジェクト生成
        D3D11_RASTERIZER_DESC rasterizerDesc{};
        {
            rasterizerDesc.FillMode = D3D11_FILL_SOLID;
            rasterizerDesc.CullMode = D3D11_CULL_BACK;
            rasterizerDesc.FrontCounterClockwise = TRUE;
            rasterizerDesc.DepthBias = 0;
            rasterizerDesc.DepthBiasClamp = 0;
            rasterizerDesc.SlopeScaledDepthBias = 0;
            rasterizerDesc.DepthClipEnable = TRUE;
            rasterizerDesc.ScissorEnable = FALSE;
            rasterizerDesc.MultisampleEnable = FALSE;
            rasterizerDesc.AntialiasedLineEnable = FALSE;
        }
        hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTER_STATE::SOLID)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        {
            rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
            rasterizerDesc.CullMode = D3D11_CULL_BACK;
            rasterizerDesc.AntialiasedLineEnable = TRUE;
        }
        hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTER_STATE::WIREFRAME)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        {
            rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
            rasterizerDesc.CullMode = D3D11_CULL_NONE;
            rasterizerDesc.AntialiasedLineEnable = TRUE;
        }
        hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTER_STATE::CULL_NONE)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        {
            rasterizerDesc.FillMode = D3D11_FILL_SOLID;
            rasterizerDesc.CullMode = D3D11_CULL_NONE;
            rasterizerDesc.AntialiasedLineEnable = FALSE;
        }
        hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTER_STATE::AAA)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        CameraManager::Create();
        CameraManager::Instance().Init(device.Get());

        EffectManager::Create();
        EffectManager::Instance().Initialize(device.Get(), deviceContext.Get());

        // シーン初期化
        SceneManager::Instance().Init(device.Get(), deviceContext.Get());
        SceneManager::Instance().ChangeScene(new SceneTitle);

        ShaderSystem::GetInstance()->Init(device.Get());

    }
    return true;
}

void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/) 
{
    // 入力更新処理
    input.Update();

    // シーン更新処理
    SceneManager::Instance().Update(elapsedTime);



#ifdef USE_IMGUI
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif
}

void Framework::Render(float elapsedTime/*Elapsed seconds from last frame*/) {
    HRESULT hr = S_OK;

    FLOAT color[] = { 0.2f,0.4f,1.0f,1.0f };
    deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), color);
    deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

    //サンプラーステートオブジェクトバインド
    deviceContext->PSSetSamplers(0, 1, this->samplerStates[static_cast<size_t>(SAMPLER_STATE::POINT)].GetAddressOf());
    deviceContext->PSSetSamplers(1, 1, this->samplerStates[static_cast<size_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
    deviceContext->PSSetSamplers(2, 1, this->samplerStates[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());

    //深度ステンシルステートオブジェクトを設定
    //deviceContext->OMSetDepthStencilState(this->depthStencilStates[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_OFF)].Get(), 1);

    //blending state object 設定
    //ラスタライザステート設定
    deviceContext->RSSetState(rasterizerStates[static_cast<size_t>(RASTER_STATE::AAA)].Get());

    CameraManager::Instance().SetCameraView(deviceContext.Get(), &CameraManager::Instance().mainView);

    //深度ステンシルステートオブジェクトを設定
    //deviceContext->OMSetDepthStencilState(this->depthStencilStates[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 1);

    // シーン描画処理
    SceneManager::Instance().Render(elapsedTime);

    //FontManager::Instance().TexOut(
    //    elapsedTime,
    //    { 0,0 },
    //    { SCREEN_WIDTH, SCREEN_HEIGHT },
    //    { L"海賊王を夢見る少年モンキー・D・ルフィを主人公とする、「ひとつなぎの大秘宝（ワンピース）」を巡る海洋冒険ロマン。夢への冒険・仲間たちとの友情といったテーマを前面に掲げ、バトルやギャグシーン、感動エピソードをメインとする少年漫画の王道を行く物語として人気を博している。また、長年にわたりながら深く練り込まれた壮大な世界観・巧緻な設定のストーリーも特徴。" },
    //    176
    //);

#ifdef USE_IMGUI
    ImGui::Begin("RenderedWindow");
    static int a = 0;
    static int b = 0;
    ImGui::SliderInt("a", &a, 0, 10);
    ImGui::SliderInt("b", &b, 0, 1);
    //ImGui::Image(framebuffers[a].get()->shaderResourceViews[b].Get(), ImGui::GetWindowSize());
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    UINT syncInterval = 0;

    //裏の画面を持ってくる
    swapChain->Present(syncInterval, 0);
}

bool Framework::Uninitialize() {
    //ReleaseAllTextures();
    EffectManager::Instance().Finalize();
    EffectManager::Destory();
    CameraManager::Destory();

    return true;
}

Framework::~Framework() {
    SceneManager::Instance().Clear();
}