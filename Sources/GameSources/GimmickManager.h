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

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Render(ID3D11DeviceContext* deviceContext, float elapsedTIme);

	// 初期化
	void Clear();

	// 格納
	void Register(Gimmick* gimmick, Identity type);

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	int GetGimmickCount()const { return static_cast<int>(gimmicks.size()); }

	void CollisionGimmickGimmicks(Gimmick* gimmck);

	// エネミー取得
	Gimmick* GetGimmick(int index) { return gimmicks.at(index); }

	// 付与したIDからエネミーを取得
	Gimmick* GetGimmickFromId(int id);
private:
	//std::vector<std::unique_ptr<Gimmick>> gimmicks;
	std::vector<Gimmick*> gimmicks;

public:
	int identity = 0;	// 付与するIDの値(この値にMetaAI::Identity::Enemyを加算して付与する)
};

#endif // !GIMMICKMANAGER