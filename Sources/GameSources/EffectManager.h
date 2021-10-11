#ifndef EFFECTMANAGER
#define EFFECTMANAGER

#include <d3d11.h>
#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

// �G�t�F�N�ƃ}�l�[�W���[
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

	// �B��̃C���X�^���X�擾
	static EffectManager& Instance() // �V���O���g��
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

	// ������
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	// �I����
	void Finalize();

	// �X�V
	void Update(float elapsedTime);

	// �`��
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	Effekseer::Manager* GetEffekseerManager() { return effekseerManager; }

private:
	Effekseer::Manager* effekseerManager = nullptr;
	EffekseerRenderer::Renderer* effekseerRender = nullptr;
};


#endif // !EFFECTMANAGER

