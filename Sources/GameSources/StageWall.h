#pragma once

#include "Stage.h"
#include "ImGui/imgui.h"


class StageWall : public Stage
{
public:
	StageWall(ID3D11Device* device);
	~StageWall();

	void Update(float elapsedTime) override;
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	void gimmickOn(float elapsedTime);
	void gimmickOff(float elapsedTime);

	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

private:


	const float UPMAX = 10;
	const float DOWNMAX = 0;
	float speed = 2;
};
