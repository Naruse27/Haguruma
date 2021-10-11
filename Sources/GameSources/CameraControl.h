#pragma once
#include "Camera.h"

class CameraControl
{
public:
	static void MouseControlUpdate( Camera* camera );
	static void PadControlUpdate( Camera* camera );
	static void CameraRotation( Camera* camera );

	void Update(float elapsedTime, Camera* camera);

	// ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	//bool CameraRay();

	const DirectX::XMFLOAT3& GetEye() const { return eye; }

private:
	DirectX::XMFLOAT3 target = { 0, 0, 0 };
	DirectX::XMFLOAT3 angle = { 0, 0, 0 };
	DirectX::XMFLOAT3 eye = { 0, 0, 0 };
	DirectX::XMFLOAT3 oldEye = { 0, 0, 0 };
	float             rollSpeed = DirectX::XMConvertToRadians(90);
	float             range = 25.0f;

	float maxAngleX = DirectX::XMConvertToRadians(45);
	float minAngleX = DirectX::XMConvertToRadians(-45);
};