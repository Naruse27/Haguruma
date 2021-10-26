#pragma once

#include "Stage.h"
#include "ImGui/imgui.h"

class StageConveyor : public Stage
{
public:
	StageConveyor(ID3D11Device* device);
	~StageConveyor();

	void Update(float elapsedTime) override;
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	void gimmickOn(float elapsedTime);
	void gimmickOff(float elapsedTime);

	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

private:


	const float UPMAX = 10;
	const float DOWNMAX = 0;
	float power = 0.02;


	DirectX::XMFLOAT2 direction = { 0,1 };
	float timer;
	float SCROLLSPEED = 0.1f;


	struct CbConveyor
	{
		DirectX::XMFLOAT2 scrollDirection;
		float time;
		float power;
	};
	CbConveyor data{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

public:
	CbConveyor getPower() { return data; }
};