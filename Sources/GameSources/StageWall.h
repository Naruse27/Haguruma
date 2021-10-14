#pragma once

#include "Stage.h"

#if 2
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

	void setGear(bool b) { gear = b; }
	bool getGear() { return gear; }

private:


	const float UPMAX = 10;
	const float DOWNMAX = 0;
	bool gear = false;

	//âÒì]äpìx
	float angle = 0.0f;
	//âÒì]ÇÃîºåa
	float r = 4.0f;
	//è∞êî
	const int FLOORNUM = 4;
	//ê›íuèÍèä
	DirectX::XMFLOAT3 pos{};


	//äeâÒì]é≤
	DirectX::XMFLOAT3 axisX = { 1,0,0 };
	DirectX::XMFLOAT3 axisY = { 0,1,0 };
	DirectX::XMFLOAT3 axisZ = { 0,0,1 };

	std::vector<std::unique_ptr<Model>> models;

};

#else
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

	void setGear(bool b) { gear = b; }
	bool getGear() { return gear; }

private:


	const float UPMAX = 10;
	const float DOWNMAX = 0;
	bool gear = false;
	float speed = 2;
};

#endif