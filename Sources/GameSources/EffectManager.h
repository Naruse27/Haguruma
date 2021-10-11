#ifndef EFFECTMANAGER
#define EFFECTMANAGER

#include <d3d11.h>
#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

// エフェクとマネージャー
class EffectManager
{
private:
	EffectManager() = default;
	~EffectManager() = default;
	static EffectManager* instance;

public:
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;
	EffectManager(EffectManager&&) = delete;
	EffectManager& operator=(EffectManager&&) = delete;

	// 唯一のインスタンス取得
	static EffectManager& Instance() // シングルトン
	{
		return *instance;
	}

	static void Create() {
		if (!instance) instance = new EffectManager;
	}

	static void Destory() {
		delete instance;
		instance = nullptr;
	}

	// 初期化
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	// 終了化
	void Finalize();

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	Effekseer::Manager* GetEffekseerManager() { return effekseerManager; }

private:
	Effekseer::Manager* effekseerManager = nullptr;
	EffekseerRenderer::Renderer* effekseerRender = nullptr;
};


#endif // !EFFECTMANAGER

