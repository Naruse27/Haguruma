#ifndef PLAYER
#define PLAYER

#include "Character.h"
#include "Gear.h"

#define GEAR_NUM 3

class Player : public Character
{
public:
	Player(ID3D11Device* device);
	~Player();

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Render(ID3D11DeviceContext* deviceContext);

	void MouseRay(
		ID3D11Device* device,
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

	// Debug用
	void DebugImGui();
private:
	// スティック入力値から移動ベクトルを所得
	Vector3 GetMoveVec() const;

	// 移動入力処理
	bool InputMove(float elapsedTime);

	// 着地した時に呼ばれる
	void OnLanding() override;

	

private:
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);

	Gear* gear[GEAR_NUM];

	float distance = 4.0f;

	// debug
	bool check = false;
};


#endif // !PLAYER

