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

	// �X�V
	void Update(float elapsedTime) override;

	// �`��
	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) { return false; }

	bool OnMessage(const Telegram& telegram) override;

	void Launch(const Vector3& direction, const Vector3& position);

	// �^�[�Q�b�g�ʒu����
	void SetTarget(const Vector3& target) { this->target = target; }

	// ��ƍ��̂����悤
	//void SetStandPosition(const Vector3& position);

	void IsSetPosition(const Vector3& position) override;

private:
	// �O�ɔ�΂�
	void StraitThrow(float elapsedTime);

	// �߂��Ă���
	void ComeBackStrait(float elapsedTime);

	// �ҋ@
	void WaitGear(float elapsedTme);

	// �t��
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