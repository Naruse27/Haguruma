#include "EffectManager.h"

// 初期化
void EffectManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    // Effekseerレンダラ生成
    effekseerRender = EffekseerRendererDX11::Renderer::Create(device, deviceContext, 2048);

    // Effekseerマネージャー生成
    effekseerManager = Effekseer::Manager::Create(2048);

    // Effekseerレンダラの各種設定(特別なカスタマイズをしない場合は定期的に以下の設定でOK)
    effekseerManager->SetSpriteRenderer(effekseerRender->CreateSpriteRenderer());
    effekseerManager->SetRibbonRenderer(effekseerRender->CreateRibbonRenderer());
    effekseerManager->SetRingRenderer(effekseerRender->CreateRingRenderer());
    effekseerManager->SetTrackRenderer(effekseerRender->CreateTrackRenderer());
    effekseerManager->SetModelRenderer(effekseerRender->CreateModelRenderer());
    // Effekseer内でのローダーの設定(特別なカスタマイズをしない場合は以下の設定でOK)
    effekseerManager->SetTextureLoader(effekseerRender->CreateTextureLoader());
    effekseerManager->SetModelLoader(effekseerRender->CreateModelLoader());
    effekseerManager->SetMaterialLoader(effekseerRender->CreateMaterialLoader());

    // Effekseerを左手座標系で計算する
    effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

// 終了化
void EffectManager::Finalize()
{
    if (effekseerManager) {
        effekseerManager->Destroy();
        effekseerManager = nullptr;
    }
    if (effekseerRender) {
        effekseerRender->Destroy();
        effekseerRender = nullptr;
    }
}

// 更新
void EffectManager::Update(float elapsedTime)
{
    // エフェクト更新処理（引数にはフレーム経過時間を渡す）
    effekseerManager->Update(elapsedTime * 60.0f);
}

// 描画
void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    // ビュー＆プロジェクション行列をEffekseerレンダラに設定
    // reinterpret_cast = ポインターが他のポインター型に変換されることを許可します
    effekseerRender->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view)); // キャスト
    effekseerRender->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

    // Effekseer描画開始
    effekseerRender->BeginRendering();

    // マネージャー単位で描画するので描画順を制御する場合はマネージャーを複数個作成し、
    // Draw()関数を実行する順序で制御できそう
    effekseerManager->Draw();

    // Effekseer描画終了
    effekseerRender->EndRendering();
}