#ifndef GIMMICK
#define GIMMICK

#include "GameLibSource/Model.h"
#include "GameLibSource/Vector.h"
#include "Telegram.h"
#include "Collision.h"

class Gimmick
{
public:
	Gimmick() {};
	virtual ~Gimmick() {}

	// 更新
	virtual void Update(float elapsedTime) = 0;

	// 描画
	virtual void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) = 0;

	// レイキャスト
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	// ポジションゲッター
	const Vector3& GetPosition() const { return position; }
	// ポジションセッター
	void SetPosition(const Vector3& position) { this->position = position; }

	// ポジションゲッター
	const Vector3& GetSetPosition() const { return setPosition; }
	// ポジションセッター
	virtual void IsSetPosition(const Vector3& position) {}

	const float GetWidth() const { return width;}
	const float GetHeight() const { return height;}

	// 接着位置のゲッター
	const Vector3& GetBondingPosition() const { return Vector3(position.x, position.y + height, position.z); }

	void	    SetId(int id) { this->id = id; }
	int		GetId() { return id; }

	const int GetIdentity() { return static_cast<int>(identitiy); }

	bool GetSetFlag() { return setFlag; }
	void IsSetFlag(bool set) { setFlag = set; }



	// メッセージ受信したときの処理
	virtual bool OnMessage(const Telegram& telegram);
	bool HandleMessage(const Telegram& msg);


protected:
	// 行列更新処理
	void UpdateTransform();

protected:
	std::unique_ptr<Model> model;

	Identity identitiy = Identity::Gear;

	Vector3 position = { 0, 0, 0 };
	Vector3 setPosition = { 0,0,0 };
	Vector3 angle = { 0, 0 ,0 };
	Vector3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	float height = 5.0f; // とりあえず
	float width = 5.0f;

	bool setFlag = false;
	int id = 0;
};

#endif // !GIMMICK