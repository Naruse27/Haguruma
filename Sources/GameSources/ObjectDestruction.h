#pragma once
#include "GameLibSource/Model.h"
#include "GameLibSource/Vector.h"
#include "Telegram.h"
#include "Collision.h"

class ObjectDestruction
{
public:
	ObjectDestruction() {};
	virtual ~ObjectDestruction() {}

	// 更新
	virtual void Update(float elapsedTime) = 0;
	// 描画
	virtual void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) = 0;
	// レイキャスト
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;



protected:
	// 行列更新処理
	void UpdateTransform() {
		// スケール行列を作成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

		// 回転行列を作成
		//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
		DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
		DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
		DirectX::XMMATRIX R = Y * X * Z;

		// 位置行列を作成
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

		// 3つの行列を組み合わせ、ワールド行列を作成
		DirectX::XMMATRIX W = S * R * T;

		// 計算したワールド行列を取り出す
		DirectX::XMStoreFloat4x4(&transform, W);
	}
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

	float scaleFactor = 1.0f;
	float rotationFactor = 1.0f;
	float positionFactor = 0.2f;
	float destruction = 0.0f;
};