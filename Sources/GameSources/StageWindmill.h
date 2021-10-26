#include "Stage.h"
#include "ImGui/imgui.h"


class StageWindmill : public Stage
{
public:
	StageWindmill(ID3D11Device* device);
	~StageWindmill();

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


	//��]�p�x
	float angle = 0.0f;
	//��]�̔��a
	float r = 4.0f;
	//����
	const int FLOORNUM = 4;
	//�ݒu�ꏊ
	DirectX::XMFLOAT3 pos{};
	float speed = 2;

	//�e��]��
	DirectX::XMFLOAT3 axisX = { 1,0,0 };
	DirectX::XMFLOAT3 axisY = { 0,1,0 };
	DirectX::XMFLOAT3 axisZ = { 0,0,1 };

	std::vector<std::shared_ptr<Model>> models;

};