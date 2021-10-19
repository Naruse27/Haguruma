#include "Gimmick.h"

void Gimmick::UpdateTransform()
{
    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    // ��]�s����쐬
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = Y * X * Z;

    // �ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    // 3�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    // �v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);
}

bool Gimmick::HandleMessage(const Telegram& msg)
{
	// ���b�Z�[�W�̎�M���̏��������s�B�߂�l�Ő����L���𔻒f
	if (OnMessage(msg)) return true;
	
	return false;
}

bool Gimmick::OnMessage(const Telegram& telegram)
{
    return true;
	// ���ꂼ��̃��b�Z�[�W�ɕ������������L�q�B
	switch (telegram.msg)
	{
    case MESSAGE_TYPE::MSG_SIGNAL_GIAR:
    case MESSAGE_TYPE::MSG_SIGNAL_STAND:
		break;
	}
	return false;
}