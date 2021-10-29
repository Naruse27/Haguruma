#include "DestructionManager.h"

DestructionManager::DestructionManager()
{
}

DestructionManager::~DestructionManager() {
	clear();
}

void DestructionManager::update(float elapsedTime) {
	for (auto p : destructions) {
		p->Update(elapsedTime);
	}

	//破棄処理
	//更新処理が終わったのちに破棄リストのオブジェクトを削除
	for (ObjectDestruction* p : removes) {
		std::vector<ObjectDestruction*>::iterator it = std::find(destructions.begin(), destructions.end(), p);
		if (it != destructions.end()) {
			destructions.erase(it);
		}
		delete p;
	}
	//破棄リストをクリア
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
	//破棄リストに追加
	removes.emplace_back(projectile);
}

void DestructionManager::clear() {
	for (auto p : destructions) {
		delete p;
	}
}