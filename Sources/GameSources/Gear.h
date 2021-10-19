#ifndef GEAR
#define GEAR

#include "Gimmick.h"

class Gear : public Gimmick
{
public:
	enum STATE
	{
		Strait,
		Wait,
		Back,
		Set,
		End,
	};

public:
	Gear(ID3D11Device* device);
	~Gear() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) { return false; }

	bool OnMessage(const Telegram& telegram) override;

	void Launch(const Vector3& direction, const Vector3& position);

	// ターゲット位置検索
	void SetTarget(const Vector3& target) { this->target = target; }

	// 台と合体したよう
	//void SetStandPosition(const Vector3& position);

	void IsSetPosition(const Vector3& position) override;

private:
	// 前に飛ばす
	void StraitThrow(float elapsedTime);

	// 戻ってくる
	void ComeBackStrait(float elapsedTime);

	// 待機
	void WaitGear(float elapsedTme);

	// 付着
	void AdhesionGear(float elapsedTime);

private:
	STATE state = STATE::Wait;

	Vector3 direction = { 0,0,1 };
	Vector3 target = { 0,0,0 };
	float speed = 10.0f;
	float straitTimer = 4.0f;
	float moveSpeed = 10.0f;
	bool tracking_check = false;
	float turnSpeed = DirectX::XMConvertToRadians(360);



	template<class Type, typename Return, typename ...Args>
	using Temp = Return(Type::*)(Args...);
	Temp<Gear, void, float> GearFunction[STATE::End];
};

#endif // !GEAR