#ifndef GIMMICKMANAGER
#define GIMMICKMANAGER

#include "Gimmick.h"
#include "Telegram.h"

class GimmickManager
{
public:
	GimmickManager() {}
	~GimmickManager() {}

	static GimmickManager& Instance()
	{
		static GimmickManager instance;
		return instance;
	}

	// �X�V
	void Update(float elapsedTime);

	// �`��
	void Render(ID3D11DeviceContext* deviceContext, float elapsedTIme);

	// ������
	void Clear();

	// �i�[
	void Register(Gimmick* gimmick, Identity type);

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	int GetGimmickCount()const { return static_cast<int>(gimmicks.size()); }

	void CollisionGimmickGimmicks(Gimmick* gimmck);

	// �G�l�~�[�擾
	Gimmick* GetGimmick(int index) { return gimmicks.at(index); }

	// �t�^����ID����G�l�~�[���擾
	Gimmick* GetGimmickFromId(int id);
private:
	//std::vector<std::unique_ptr<Gimmick>> gimmicks;
	std::vector<Gimmick*> gimmicks;

public:
	int identity = 0;	// �t�^����ID�̒l(���̒l��MetaAI::Identity::Enemy�����Z���ĕt�^����)
};

#endif // !GIMMICKMANAGER