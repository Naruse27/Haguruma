#ifndef CAMERAMANAGER
#define CAMERAMANAGER

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
ClassName(const ClassName&); \
ClassName& operator=(const ClassName&); \
ClassName(const ClassName&&); \
ClassName& operator=(const ClassName&&);

#include <d3d11.h>
#include <wrl.h>
#include "Camera.h"

class CameraManager
{
private:
	CameraManager() = default;
	~CameraManager() = default;
	inline static CameraManager* instance;

public:
	//CameraManager(const CameraManager&) = delete;
	//CameraManager& operator=(const CameraManager&) = delete;
	//CameraManager(CameraManager&&) = delete;
	//CameraManager& operator=(CameraManager&&) = delete;
	DISALLOW_COPY_AND_ASSIGN(CameraManager);

	struct SceneConstants
	{
		DirectX::XMFLOAT4X4 viewProjection; //ビュー・プロジェクション変換行列
		DirectX::XMFLOAT4 lightDirection; //ライトの向き
		DirectX::XMFLOAT4 lightColor; // ライトの色
		DirectX::XMFLOAT4 cameraPosition;//カメラ位置
	};


	static CameraManager& Instance() // シングルトン
	{
		return *instance;
	}

	static void Create() {
		if (!instance) instance = new CameraManager;
	}

	static void Destory() {
		delete instance;
		instance = nullptr;
	}

	// 初期設定
	void Init(ID3D11Device* device);

	void SetCameraView(ID3D11DeviceContext* deviceContext, Camera* camera);

private:
	void CreateBuffer(ID3D11Device* device);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> constntBuffer;

	DirectX::XMFLOAT4 lightDirection = { 0.0f, 0.0f, 1.0f, 0.0f };

public:
	Camera mainView;
	Camera enemyEditorView;
	Camera bgEditorView;
};

#endif // !CAMERAMANAGER

