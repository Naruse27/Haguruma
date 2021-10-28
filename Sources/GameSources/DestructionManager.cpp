#include "DestructionManager.h"

DestructionManager::~DestructionManager() {
	clear();
}

void DestructionManager::update(float elapsedTime) {
	for (auto p : destructions) {
		p->Update(elapsedTime);
	}

	//�j������
	//�X�V�������I������̂��ɔj�����X�g�̃I�u�W�F�N�g���폜
	for (ObjectDestruction* p : removes) {
		std::vector<ObjectDestruction*>::iterator it = std::find(destructions.begin(), destructions.end(), p);
		if (it != destructions.end()) {
			destructions.erase(it);
		}
		delete p;
	}
	//�j�����X�g���N���A
	removes.clear();
}

void DestructionManager::render(ID3D11DeviceContext* dc, float elapsedTime) {
	for (auto p : destructions) {
		p->Render(dc, elapsedTime);
	}
}

void DestructionManager::addition(ObjectDestruction* projectile) {
	destructions.emplace_back(projectile);
}

void DestructionManager::remove(ObjectDestruction* projectile) {
	//�j�����X�g�ɒǉ�
	removes.emplace_back(projectile);
}

void DestructionManager::clear() {
	for (auto p : destructions) {
		delete p;
	}
}

bool DestructionManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;
	hit.distance = FLT_MAX;

	for (ObjectDestruction* stage : destructions) {
		HitResult tmp;
		if (stage->RayCast(start, end, tmp)) {
			if (tmp.distance < hit.distance) {
				hit = tmp;
				result = true;
			}
		}
	}

	return result;
}
