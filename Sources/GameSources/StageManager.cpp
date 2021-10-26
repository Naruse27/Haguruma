#include "StageManager.h"
#include "GimmickManager.h"

StageManager* StageManager::instance = nullptr;

void StageManager::Update(float elapsedTime)
{
    for (Stage* stage : stages) stage->Update(elapsedTime);
}

void StageManager::Render(ID3D11DeviceContext* dc, float elapsedTime)
{
    for (Stage* stage : stages) stage->Render(dc, elapsedTime);
}

void StageManager::Register(Stage* stage)
{
    static int id = 0;
    stage->setID(id++);
    stages.emplace_back(stage);
}

void StageManager::Clear()
{
    for (Stage* stage : stages) delete stage;
    stages.clear();
}

bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    bool result = false;
    hit.distance = FLT_MAX;

    for (Stage* stage : stages) {
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

void StageManager::getGimmickState()
{
    for (auto& stage : stages) {
        int stageID = stage->getID();
        Gimmick* gim = GimmickManager::Instance().GetGimmickFromId(stageID);
        stage->setGear(gim->GetSetFlag());
    }  
}
