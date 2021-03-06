#pragma once

#include "Scene.h"

// タイトルシーン
class SceneResult : public Scene
{
public:
	SceneResult() {}
	~SceneResult() override {}

	// 初期化
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;

	// 終了化
	virtual void Finalize() override;

	// 更新処理
	virtual void Update(float elapsedTime) override;

	// 描画処理
	virtual void Render(float elapsedTime) override;
private:
};