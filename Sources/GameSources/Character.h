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

	// �s��X�V����
	void UpdateTransform();

	const Vector3& GetPosition() const { return position; }
	void SetPosition(const Vector3& position) { this->position = position; }
	const Vector3& GetAngle() const { return angle; }
	void SetAngle(const Vector3& angle) { this->angle = angle; }
	const Vector3& GetScale() const { return scale; }
	void SetScale(const Vector3& scale) { this->scale = scale; }

	const Vector3& GetVelocity() const { return velocity; }

	// ��������
	float GetHeight() const { return height; }

	// ���a�擾
	float GetRadius() const { return radius; }

	// �n�ʂɐڒn���Ă��邩
	bool IsGround() const { return isGround; }

	// ���N��Ԃ��擾
	int GetHealth() const { return health; }

	//	�ő匒�N��Ԃ��擾
	int GetMaxHealth() const { return maxHealth; }

	// �Ռ���^����
	void AddImpulse(const Vector3& impulse);

	// �_���[�W��^����
	bool ApplyDamage(int damage, float invincibleTime);

private:
	// �������͍X�V����
	void UpdateVerticalVelocitiy(float elapsedFrame);

	// �����ړ��X�V����
	void UpdateVerticalMove(float elapsedTime);

	// �������͍X�V����
	void UpdateHorizontalVelocity(float elapsedTime);

	// �����ړ��X�V����
	void UpdateHorizontalMove(float elapsedTime);

protected:
	void Move(float vx, float vz, float speed);
	void Turn(float elapsedTime, float vx, float vz, float speed);
	void Jump(float speed);
	void UpdateVelocity(float elapsedTime);
	void UpdateInvincibleTimer(float elapsedTime);

	virtual void OnLanding() {}

	// �_���[�W���󂯂����ɌĂ΂��
	virtual void OnDamaged() {}

	// ���S�������ɌĂ΂��
	virtual void OnDead() {}

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

	float radius = 0.5f;
	float gravity = -1.0f;
	Vector3 velocity = { 0,0,0 };
	bool isGround = false;
	float height = 2.0f;
	int health = 5;
	int maxHealth = 5;
	float invincibleTimer = 1.0f;
	float friction = 0.5f;
	float acceleration = 1.0f;
	float maxMoveSpeed = 10.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;
	float airControl = 0.3f;
	float stepOffset = 1.0f;
	float slopeRate = 1.0f;
};


#endif // !CHARACTER