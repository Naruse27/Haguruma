#ifndef GRAPHICS
#define GRAPHICS

#include <mutex>
#include <memory>
#include <d3d11.h>
#include <wrl.h>

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
ClassName(const ClassName&); \
ClassName& operator=(const ClassName&); \
ClassName(const ClassName&&); \
ClassName& operator=(const ClassName&&);

class Graphics
{
private:
	Graphics() {};
	~Graphics() {};
	inline static Graphics* instance;
public:
	DISALLOW_COPY_AND_ASSIGN(Graphics);

	static Graphics& Instance() // シングルトン
	{
		return *instance;
	}

	static void Create() {
		if (!instance) instance = new Graphics;
	}

	static void Destory() {
		delete instance;
		instance = nullptr;
	}

	void Initiraize(HWND hWnd);

	//// デバイス取得
	//ID3D11Device* GetDevice() const { return device.Get(); }
	//inline Microsoft::WRL::ComPtr<ID3D11Device> GetDevice2() { return device; };
	//
	//// デバイスコンテキスト取得
	//ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }
	//
	//// スワップチェーン取得
	//IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

	// レンダーターゲットビュー取得
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// デプスステンシルビュー取得
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }
	// スクリーン幅取得
	float GetScreenWidth() const { return screenWidth; }

	// スクリーン高さ取得
	float GetScreenHeight() const { return screenHeight; }

	// ミューテックス取得
	std::mutex& GetMutex() { return mutex; }
private:
	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;

	float screenWidth = 0.0f;
	float screenHeight = 0.0f;

	std::mutex mutex;
};

#endif // !GRAPHICS

