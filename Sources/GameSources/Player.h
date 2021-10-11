#ifndef PLAYER
#define PLAYER

#include "Character.h"

class Player : public Character
{
public:
	Player(ID3D11Device* device);
	~Player();

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Render(ID3D11DeviceContext* deviceContext);

	// Debug用
	void DebugImGui();
private:
	// スティック入力値から移動ベクトルを所得
	Vector3 GetMoveVec() const;

	// 移動入力処理
	bool InputMove(float elapsedTime);

private:
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
};


#endif // !PLAYER

