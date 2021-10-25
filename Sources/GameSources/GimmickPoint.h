#ifndef GIMMICKPOINT

#include "Gimmick.h"


class GimmickPoint : public Gimmick
{
public:
	GimmickPoint(ID3D11Device* device);
	~GimmickPoint() override;

	// �X�V
	void Update(float elapsedTime) override;

	// �`��
	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	// ���b�Z�[�W����
	bool OnMessage(const Telegram& telegram) override;
private:

};



#endif // !GIMMICKPOINT
