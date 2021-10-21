#pragma once
#include "Camera.h"

class CameraControl
{
public:
	enum Select
	{
		FIXED,
		PAD_CONTOROL,
		END,
	};

public:
	CameraControl();
	~CameraControl() {}

	void FixedCameraUpdate(float elapsedTime, Camera* camera);
	void MouseControlUpdate( Camera* camera );
	void PadControlUpdate(float elapsedTime, Camera* camera );
	void CameraRotation( Camera* camera );


	void Update(float elapsedTime, Camera* camera);

	// ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	bool CameraRay();

	const DirectX::XMFLOAT3& GetEye() const { return eye; }

	void RenderDebugGui(Camera* camera);

private:
	DirectX::XMFLOAT3 target = { 0, 0, 0 };
	DirectX::XMFLOAT3 angle = { DirectX::XMConvertToRadians(20), 0, 0 };
	DirectX::XMFLOAT3 eye = { 0, 0, 0 };
	DirectX::XMFLOAT3 oldEye = { 0, 0, 0 };
	float             rollSpeed = DirectX::XMConvertToRadians(90);
	float             range = 65.0f;
	float rangeMax = 100.0f;
	float rangeMin = 65.0f;

	float maxAngleX = DirectX::XMConvertToRadians(45);
	float minAngleX = DirectX::XMConvertToRadians(-45);

	template<class Type, typename Return, typename ...Args>
	using Temp = Return(Type::*)(Args...);
	Temp<CameraControl, void,  float, Camera*> ConttollerFunction[Select::END];
};