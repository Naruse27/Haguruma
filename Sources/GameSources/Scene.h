#ifndef SCENE
#define SCENE

#include <d3d11.h>

// シーン
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	// 初期化
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) = 0;

	// 終了化
	virtual void Finalize() = 0;

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(float elapsedTime) = 0;

	// 準備完了しているか
	bool IsReady() const { return ready; }

	// 準備完了設定
	void SetReady() { ready = true; }

protected:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

private:
	bool ready = false;
};

#endif // !SCENE

