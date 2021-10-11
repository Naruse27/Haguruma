#ifndef CHARACTER
#define CHARACTER

#include "GameLibSource/Vector.h"
#include "GameLibSource/Model.h"

class Character
{
public:
	Character() {}
	~Character() {}

	// 行列更新処理
	void UpdateTransform();

	// 座標のゲッター
	const Vector3& GetPosition() const { return position; }
	// 座標のセッター
	void SetPosition(const Vector3& position) { this->position = position; }
	const Vector3& GetAngle() const { return angle; }
	void SetAngle(const Vector3& angle) { this->angle = angle; }
	const Vector3& GetScale() const { return scale; }
	void SetScale(const Vector3& scale) { this->scale = scale; }
	//const DirectX::XMFLOAT3& GetVelocity() const { return velocity; }

private:
	// 垂直速力更新処理
	void UpdateVerticalVelocitiy(float elapsedFrame);

	// 垂直移動更新処理
	void UpdateVerticalMove(float elapsedTime);

	// 水平速力更新処理
	void UpdateHorizontalVelocity(float elapsedTime);

	// 水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);

protected:
	// 移動
	void Move(float vx, float vz, float speed);
	// 回転
	void Turn(float elapsedTime, float vx, float vz, float speed);
	// ジャンプ
	void Jump(float speed);

	void UpdateVelocity(float elapsedTime);
	void UpdateInvincibleTimer(float elapsedTime);

	virtual void OnLanding() {}

protected:
	Model* model = nullptr;

	Vector3 position = { 0, 0, 0 };
	Vector3 angle = { 0, 0 ,0 };
	Vector3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	Vector3 velocity = { 0,0,0 };

	bool isGround = false;

	float radius = 0.5f;
	float gravity = -1.0f;
	float height = 2.0f;
	float invincibleTimer = 1.0f;
	float friction = 0.5f;
	float acceleration = 1.0f;
	float maxMoveSpeed = 5.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;
	float airControl = 0.3f;
	float stepOffset = 1.0f;
	float slopeRate = 1.0f;
};


#endif // !CHARACTER