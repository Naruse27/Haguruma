#pragma once
#include "GameLibSource/Vector.h"
#include "Collision.h"

class ObjectDestruction
{
public:
	ObjectDestruction() {};
	virtual ~ObjectDestruction() {}

	// �X�V
	virtual void Update(float elapsedTime) = 0;
	virtual void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) = 0;
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;



	const Vector3& GetPosition() const { return position; }
	const float GetWidth() const { return width; }
	const float GetHeight() const { return height; }

	void setDest() { dest = true; }

protected:
	// �s��X�V����
	void UpdateTransform() {
		// �X�P�[���s����쐬
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

		// ��]�s����쐬
		//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
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
protected:

	Vector3 position = { 0, 0, 0 };
	Vector3 angle = { 0, 0 ,0 };
	Vector3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

<<<<<<< HEAD
	float scaleFactor = 1.0f;
	float rotationFactor = 1.0f;
	float positionFactor = 0.2f;
	float destruction = 0.0f;
=======
	DestructionManager* mgr{};

	float height = 5.0f; // �Ƃ肠����
	float width = 5.0f;

	//��ꂽ���ǂ���
	bool dest = false;
>>>>>>> 070702319ab3ccce4f6a49339f966afc094a1dd4
};