#pragma once

#include "Character.h"

class PlayerTemp : public Character
{
public:
	PlayerTemp(ID3D11Device* device);
	~PlayerTemp();

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
