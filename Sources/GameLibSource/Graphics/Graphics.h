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

	static Graphics& Instance() // �V���O���g��
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

	//// �f�o�C�X�擾
	//ID3D11Device* GetDevice() const { return device.Get(); }
	//inline Microsoft::WRL::ComPtr<ID3D11Device> GetDevice2() { return device; };
	//
	//// �f�o�C�X�R���e�L�X�g�擾
	//ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }
	//
	//// �X���b�v�`�F�[���擾
	//IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }
	// �X�N���[�����擾
	float GetScreenWidth() const { return screenWidth; }

	// �X�N���[�������擾
	float GetScreenHeight() const { return screenHeight; }

	// �~���[�e�b�N�X�擾
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

