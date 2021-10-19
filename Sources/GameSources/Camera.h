#pragma once

#include	<DirectXMath.h>
#include "Camera.h"

#define PI 3.14159265358979f

// �J����
class Camera
{
public:
	Camera() {}
	~Camera() {}

	// �w�����������
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	// �p�[�X�y�N�e�B�u(���_)�ݒ�
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	// �r���[�s�񏊓�
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	// �v���W�F�N�V�����s�񏊓�
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	// �v���W�F�N�V�����s��Z�b�g
	void SetProjection(DirectX::XMFLOAT4X4& projection) { this->projection = projection; }

	// ���_���� position
	const DirectX::XMFLOAT3& GetEye() const { return eye; }

	// �����_����
	const DirectX::XMFLOAT3& GetForcus() const { return focus; }

	// ���������
	const DirectX::XMFLOAT3& GetUp() const { return up; }

	// �O��������
	const DirectX::XMFLOAT3& GetFront() const { return front; }

	// �E�����擾
	const DirectX::XMFLOAT3& GetRight() const { return right; }

	// ���_�Z�b�g
	void SetEye(DirectX::XMFLOAT3 e) { eye = e; }

private:
	DirectX::XMFLOAT4X4 view{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	DirectX::XMFLOAT4X4 projection{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};

	DirectX::XMFLOAT3 eye{0,0,0};
	DirectX::XMFLOAT3 focus{0,0,0};

	DirectX::XMFLOAT3 up{0,0,0};
	DirectX::XMFLOAT3 front{0,0,0};
	DirectX::XMFLOAT3 right{0,0,0};
};