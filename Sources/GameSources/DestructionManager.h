#pragma once
#include <vector>
#include "ObjectDestruction.h"
#include "Collision.h"

class DestructionManager
{
public:
	DestructionManager() {}
	~DestructionManager();
	void update(float elapsedTime);
	void render(ID3D11DeviceContext* dc, float elapsedTime);
	void addition(ObjectDestruction* objectDestruction);
	void remove(ObjectDestruction* objectDestruction);
	void clear();
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	int getDestructionCount() const { return static_cast<int>(destructions.size()); }
	ObjectDestruction* getDestruction(int index) { return destructions.at(index); }

private:
	std::vector<ObjectDestruction*> destructions;
	std::vector<ObjectDestruction*> removes;
};