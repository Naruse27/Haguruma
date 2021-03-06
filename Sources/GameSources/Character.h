#ifndef CHARACTER
#define CHARACTER

#include "GameLibSource/Vector.h"
#include "GameLibSource/Model.h"
#include "GimmickManager.h"

class Character
{
public:
	Character() {}
	virtual ~Character() {}

	// 行列更新処理
	void UpdateTransform();

	const Vector3& GetPosition() const { return position; }
	void SetPosition(const Vector3& position) { this->position = position; }
	const Vector3& GetAngle() const { return angle; }
	void SetAngle(const Vector3& angle) { this->angle = angle; }
	const Vector3& GetScale() const { return scale; }
	void SetScale(const Vector3& scale) { this->scale = scale; }

	const Vector3& GetVelocity() const { return velocity; }

	// 高さ処理
	float GetHeight() const { return height; }

	// 半径取得
	float GetWidth() const { return width; }

	// 地面に接地しているか
	bool IsGround() const { return isGround; }

	// 健康状態を取得
	int GetHealth() const { return health; }

	//	最大健康状態を取得
	int GetMaxHealth() const { return maxHealth; }

	// 衝撃を与える
	void AddImpulse(const Vector3& impulse);

	// ダメージを与える
	bool ApplyDamage(int damage, float invincibleTime);

	// メッセージ受信したときの処理
	bool HandleMessage(const Telegram& msg);
	// 受け取り
	virtual bool OnMessage(const Telegram& telegram);

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
	void Move(float vx, float vz, float speed);
	void Turn(float elapsedTime, float vx, float vz, float speed);
	void Jump(float speed);
	void UpdateVelocity(float elapsedTime);
	void UpdateInvincibleTimer(float elapsedTime);

	virtual void OnLanding() {}

	// ダメージを受けた時に呼ばれる
	virtual void OnDamaged() {}

	// 死亡した時に呼ばれる
	virtual void OnDead() {}

	// 地面から落ちた時に呼ばれる
	virtual void DropProcessing() {}

protected:
	Model* model = nullptr;

	Vector3 position = { 0,0,0 };
	Vector3 angle = { 0,0,0 };
	Vector3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	Vector3 normal = { 0,0,0 };

	float gravity = -1.0f;

	Vector3 velocity = { 0,0,0 };
	Vector3 velocityMax = { 0, -20, 0 };

	bool isGround = false;
	float height = 4.0f;
	float width = 4.0f;

	int health = 5;
	int maxHealth = 5;
	float invincibleTimer = 1.0f;
	float friction = 0.5f;
	float acceleration = 1.0f;
	float maxMoveSpeed = 30.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;
	float airControl = 0.3f;
	float stepOffset = 1.0f;
	float slopeRate = 1.0f;



	float deathbed = -40.0f;
	bool deathFlag = false;

};

#endif // !CHARACTER