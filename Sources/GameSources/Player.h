#ifndef PLAYER
#define PLAYER

#include "Character.h"

class Player : public Character
{
public:
	Player(ID3D11Device* device);
	~Player();

	// �X�V
	void Update(float elapsedTime);

	// �`��
	void Render(ID3D11DeviceContext* deviceContext);

	// Debug�p
	void DebugImGui();
private:
	// �X�e�B�b�N���͒l����ړ��x�N�g��������
	Vector3 GetMoveVec() const;

	// �ړ����͏���
	bool InputMove(float elapsedTime);

private:
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
};


#endif // !PLAYER

