#ifndef FRAMEWORK
#define FRAMEWORK

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include "high_resolution_timer.h"
#include "misc.h"
#include "Sprite.h"
#include "SkinnedMesh.h"
#include "Model.h"


#include <d3d11_1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")

#ifdef USE_IMGUI
//#if 1
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

CONST LONG SCREEN_WIDTH = 1920;
CONST LONG SCREEN_HEIGHT = 1080;
CONST BOOL FULLSCREEN = FALSE;
CONST LPCWSTR APPLICATION_NAME = L"Rebel";

#include "Input/Input.h"
#include "Graphics/Graphics.h"

class Framework
{
public:
	CONST HWND hwnd;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;


	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

	//Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1_device_context;
	//Microsoft::WRL::ComPtr<IDWriteTextFormat> dwrite_text_format;
	//Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> d2d_solid_color_brush;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[3];
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaders[8];
	//
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[5];
	//
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[4];

	enum class SAMPLER_STATE {
		POINT,
		LINEAR,
		ANISOTROPIC
	};
	enum class DEPTH_STATE {
		ZT_ON_ZW_ON,
		ZT_ON_ZW_OFF,
		ZT_OFF_ZW_ON,
		ZT_OFF_ZW_OFF
	};
	enum class RASTER_STATE {
		SOLID,
		WIREFRAME,
		CULL_NONE,
		WIREFRAME_CULL_NONE,
		AAA,
	};

	const DirectX::XMFLOAT4X4 coordinateSystemTransforms[4] = {
		{ -1, 0, 0, 0,   0, 1,  0, 0,   0, 0, 1, 0,   0, 0, 0, 1 }, // 0:RHS Y-UP
		{  1, 0, 0, 0,   0, 1,  0, 0,   0, 0, 1, 0,   0, 0, 0, 1 }, // 1:LHS Y-UP
		{ -1, 0, 0, 0,   0, 0, -1, 0,   0, 1, 0, 0,   0, 0, 0, 1 }, // 2:RHS Z-UP
		{  1, 0, 0, 0,   0, 0,  1, 0,   0, 1, 0, 0,   0, 0, 0, 1 }, // 3:LHS Z-UP
	};

	//ImGUI
	DirectX::XMFLOAT4 cameraPosition = { 424.242f, 0.0f, -10.0f, 1.0f };
	DirectX::XMFLOAT4 lightDirection = { 0.0f, 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT4 scale = { 1.0f,1.0f,1.0f,0.0f };
	DirectX::XMFLOAT4 rotate = { 0.0f,0.0f,0.0f,0.0f };
	DirectX::XMFLOAT4 translate = { 0.0f,0.0f,-5.0f,0.0f };
	DirectX::XMFLOAT4 materialColor = { 1.0f,1.0f,1.0f,1.0f };

	DirectX::XMFLOAT4 rotationAxis = { 1.0f,0.0f,0.0f,0.0f };
	
	Input input;
	//Graphics graphics;
	
public:

	Framework(HWND hwnd);
	~Framework();

	//static Framework& GetInstance(HWND hwnd = nullptr)
	//{
	//	static Framework instance(hwnd);
	//	return instance;
	//}

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;

	int Run() {
		MSG msg{};

		if (!Initialize()) {
			return 0;
		}

#ifdef USE_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
		ImGui::StyleColorsDark();
#endif

		while (WM_QUIT != msg.message) {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				tictoc.tick();
				CalculateFrameStats();
				Update(tictoc.time_interval());
				Render(tictoc.time_interval());
			}
		}

#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

#if 1
		BOOL fullscreen = 0;
		swapChain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen) {
			swapChain->SetFullscreenState(FALSE, 0);
		}
#endif


		return Uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		int zelta = 0;
		switch (msg) {
		case WM_PAINT: {
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
					 break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) {
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.stop();
			break;
		case WM_MOUSEWHEEL:
			zelta = GET_WHEEL_DELTA_WPARAM(wparam);

			input.GetMouse().SetWheel(zelta);
			break;
		case WM_EXITSIZEMOVE:
			tictoc.start();
			break;
		default:
			return DefWindowProcW(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool Initialize();
	void Update(float elapsedTime/*Elapsed seconds from last frame*/);
	void Render(float elapsedTime/*Elapsed seconds from last frame*/);
	bool Uninitialize();

private:
	high_resolution_timer tictoc;
	uint32_t frames = 0;
	float elapsedTime = 0.0f;
	void CalculateFrameStats() {
		if (++frames, (tictoc.time_stamp() - elapsedTime) >= 1.0f) {
			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(hwnd, outs.str().c_str());

			frames = 0;
			elapsedTime += 1.0f;
		}
	}
//public:
//	inline static Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() { return Framework::GetInstance().device; };
//	inline static Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() { return Framework::GetInstance().deviceContext; };
};

#endif // !FRAMEWORK
