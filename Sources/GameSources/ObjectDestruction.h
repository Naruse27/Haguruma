#pragma once
#include "GameLibSource/Model.h"
#include "GameLibSource/Vector.h"
#include "Telegram.h"
#include "Collision.h"

class DestructionManager;

class ObjectDestruction
{
public:
	ObjectDestruction(ID3D11Device* device, DestructionManager* mgr);
	virtual ~ObjectDestruction() {}

	// �X�V
	virtual void Update(float elapsedTime) = 0;
	// �`��
	virtual void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) = 0;
	// ���C�L���X�g
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	void Destroy();

protected:
	// �s��X�V����
	void UpdateTransform();
protected:
	std::unique_ptr<Model> model;

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
	struct Destruct
	{
		float scale;
		float angle;
		float position;
		float destruction;
	};
	float scaleFactor = 1.0f;
	float rotationFactor = 1.0f;
	float positionFactor = 1.0f;
	float destruction = 0.0f;

	DestructionManager* mgr{};
};