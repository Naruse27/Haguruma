#ifndef PLAYER
#define PLAYER

#include "Character.h"
#include "Gear.h"
#include "GameLibSource/Sprite.h"

#define GEAR_NUM 3

class Player : public Character
{
private:
	enum class State : int
	{
		Idle,
		Move,
		End,
	};

public:
	Player(ID3D11Device* device);
	~Player();

	// ������
	void Init();

	// �X�V
	void Update(float elapsedTime);

	// �`��
	void Render(ID3D11DeviceContext* deviceContext);

	void MouseRay(
		ID3D11Device* device,
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

	// Debug�p
	void DebugImGui();

	// ���b�Z�[�W�󂯎��
	bool OnMessage(const Telegram& msg) override;

	void CollisionPlayerVSGimmick();

	bool GetDeathFlag() { return deathFlag; }

	bool GetCheckPointFlag() { return checkPoint; }
	void SetCheckPointFlag(bool ok) { checkPoint = ok; }

	bool GetGoalPoint() { return goalPoint; }

	void SetStartGimmickID(int id) { startGimmickID = id; }
	const int GetStartGimmickID() const { return startGimmickID; }

	void SetCheckPointID(int id) { checkPointID = id; }
	const int GetCheckPointID() const { return checkPointID; }

private:
	// �X�e�B�b�N���͒l����ړ��x�N�g��������
	Vector3 GetMoveVec() const;

	// �ړ����͏���
	bool InputMove(float elapsedTime);

	// �W�����v����
	bool InputJump();

	// ���n�������ɌĂ΂��
	void OnLanding() override;

	// �n�ʂ��痎�������ɌĂ΂��
	void DropProcessing() override;

	// ��ԑJ�ځ�
	// �ҋ@�X�e�[�g�J��
	void TransitionIdleState();

	// �ҋ@�X�e�[�g�X�V����
	void UpdateIdleState(float elapsedTime);

	// �ړ��X�e�[�g�֑J��
	void TransitionMoveState();

	// �ړ��X�e�[�g�X�V����
	void UpdateMoveState(float elapsedTime);

private:
	template<class Type, typename Return, typename ...Args>
	using Temp = Return(Type::*)(Args...);
	Temp<Player, void, float> UpdateState[static_cast<int>(State::End)];

	State state = State::Idle;
private:
	std::unique_ptr<Sprite> blackOut;
	float moveSpeed = 10.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);

	Gear* gear[GEAR_NUM];

	Vector3 setPosition = { position.x, position.y + height, position.z };

	float distance = 5.0f;

	float jumpSpeed = 50.0f;
	int jumpCount = 0;
	int jumpLimit = 1;

	int deathCount = 0;

	int startGimmickID = 0;
	int checkPointID = 0;

	bool checkPoint = false;
	bool goalPoint = false;

	bool animationLoop = true;

	// 2d�p
	Vector2 scale2d = { 1.0f, 1.0f };
	float scaleMax = 10.0f;

	// debug
	bool check = false;

};


#endif // !PLAYER

