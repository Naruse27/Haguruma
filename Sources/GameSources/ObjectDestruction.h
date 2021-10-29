#pragma once
#include "GameLibSource/Vector.h"
#include "Collision.h"

class DestructionManager;

class ObjectDestruction
{
public:
	ObjectDestruction(ID3D11Device* device, DestructionManager* mgr);
	virtual ~ObjectDestruction() {}

	// �X�V
	virtual void Update(float elapsedTime) = 0;
	virtual void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) = 0;
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	void Destroy();

	const Vector3& GetPosition() const { return position; }
	const float GetWidth() const { return width; }
	const float GetHeight() const { return height; }

	void setDest() { dest = true; }

protected:
	// �s��X�V����
	void UpdateTransform();
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

	DestructionManager* mgr{};

	float height = 5.0f; // �Ƃ肠����
	float width = 5.0f;

	//��ꂽ���ǂ���
	bool dest = false;
	const float ERACEFLOAT = 0.005f;
};