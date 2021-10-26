#ifndef STAGEMANAGER
#define STAGEMANAGER

#include "Stage.h"
#include "Collision.h"

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
ClassName(const ClassName&); \
ClassName& operator=(const ClassName&); \
ClassName(const ClassName&&); \
ClassName& operator=(const ClassName&&);

class StageManager
{
private:
	StageManager() = default;
	~StageManager() = default;
	static StageManager* instance;

public:
	DISALLOW_COPY_AND_ASSIGN(StageManager);

	static StageManager& Instance() // ƒVƒ“ƒOƒ‹ƒgƒ“
	{
		return *instance;
	}

	static void Create() {
		if (!instance) instance = new StageManager;
	}

	static void Destory() {
		delete instance;
		instance = nullptr;
	}

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc, float elapsedTime);

	void Register(Stage* stage);

	void Clear();

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	void getGimmickState();

private:
	std::vector<Stage*> stages;
};

#endif // !STAGEMANAGER