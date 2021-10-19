#ifndef STAND
#define STAND

#include "Gimmick.h"

class Stand : public Gimmick
{
public:
	Stand(ID3D11Device* device);
	~Stand();

	// �X�V
	void Update(float elapsedTime);

	// �`��
	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime);

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
private:

};



#endif // !STAND

