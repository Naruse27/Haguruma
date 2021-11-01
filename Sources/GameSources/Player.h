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

	// 初期化
	void Init();

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

	// メッセージ受け取り
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
	// スティック入力値から移動ベクトルを所得
	Vector3 GetMoveVec() const;

	// 移動入力処理
	bool InputMove(float elapsedTime);

	// ジャンプ処理
	bool InputJump();

	// 着地した時に呼ばれる
	void OnLanding() override;

	// 地面から落ちた時に呼ばれる
	void DropProcessing() override;

	// 状態遷移↓
	// 待機ステート遷移
	void TransitionIdleState();

	// 待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
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

	// 2d用
	Vector2 scale2d = { 1.0f, 1.0f };
	float scaleMax = 10.0f;

	// debug
	bool check = false;

};


#endif // !PLAYER

