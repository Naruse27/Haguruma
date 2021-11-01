#ifndef GIMMICKPOINT

#include "Gimmick.h"


class GimmickPoint : public Gimmick
{
public:
	GimmickPoint(ID3D11Device* device, Identity itentity, bool goal);
	~GimmickPoint() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	// メッセージ所得
	bool OnMessage(const Telegram& telegram) override;

	bool GetRegistration() { return registration; }
	void SetRegistration(bool flag) { registration = flag; }

private:
	bool registration = false;
	bool goalFlag = false; // ゴールポイントかどうか
};



#endif // !GIMMICKPOINT
