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

	// �X�V
	virtual void Update(float elapsedTime) = 0;

	// �`��
	virtual void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) = 0;

	// ���C�L���X�g
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	// �|�W�V�����Q�b�^�[
	const Vector3& GetPosition() const { return position; }
	// �|�W�V�����Z�b�^�[
	void SetPosition(const Vector3& position) { this->position = position; }

	// �|�W�V�����Q�b�^�[
	const Vector3& GetSetPosition() const { return setPosition; }
	// �|�W�V�����Z�b�^�[
	virtual void IsSetPosition(const Vector3& position) {}

	const float GetWidth() const { return width;}
	const float GetHeight() const { return height;}

	// �ڒ��ʒu�̃Q�b�^�[
	const Vector3& GetBondingPosition() const { return Vector3(position.x, position.y + height, position.z); }

	void	    SetId(int id) { this->id = id; }
	int		GetId() { return id; }

	const int GetIdentity() { return static_cast<int>(identitiy); }

	bool GetSetFlag() { return setFlag; }
	void IsSetFlag(bool set) { setFlag = set; }



	// ���b�Z�[�W��M�����Ƃ��̏���
	virtual bool OnMessage(const Telegram& telegram);
	bool HandleMessage(const Telegram& msg);


protected:
	// �s��X�V����
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
	float height = 5.0f; // �Ƃ肠����
	float width = 5.0f;

	bool setFlag = false;
	int id = 0;
};

#endif // !GIMMICK