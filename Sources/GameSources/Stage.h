#ifndef STAGE
#define STAGE

#include "../GameLibSource/Model.h"
#include "Collision.h"

class Stage
{
public:
	Stage() {}
	~Stage() {}

	virtual void Update(float elapsedTime) = 0;

	virtual void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) = 0;

	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	int getID() { return id; }
	void setID(int i) { id = i; }
	void setGear(bool b) { gear = b; }

protected:
	// 行列更新処理
	void UpdateTransform();

protected:
	std::unique_ptr<Model> model;

	Vector3 position = { 0, 0, 0 };
	Vector3 angle = { 0, 0 ,0 };
	Vector3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	
	int id;
	bool gear = false;
};

#endif // !STAGE