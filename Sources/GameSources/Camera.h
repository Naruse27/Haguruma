#pragma once

#include	<DirectXMath.h>
#include "Camera.h"

#define PI 3.14159265358979f

// カメラ
class Camera
{
public:
	Camera() {}
	~Camera() {}

	// 指定方向を向く
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	// パースペクティブ(視点)設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	// ビュー行列所得
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	// プロジェクション行列所得
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	// プロジェクション行列セット
	void SetProjection(DirectX::XMFLOAT4X4& projection) { this->projection = projection; }

	// 視点所得 position
	const DirectX::XMFLOAT3& GetEye() const { return eye; }

	// 注視点所得
	const DirectX::XMFLOAT3& GetForcus() const { return focus; }

	// 上方向所得
	const DirectX::XMFLOAT3& GetUp() const { return up; }

	// 前方向所得
	const DirectX::XMFLOAT3& GetFront() const { return front; }

	// 右方向取得
	const DirectX::XMFLOAT3& GetRight() const { return right; }

	// 視点セット
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