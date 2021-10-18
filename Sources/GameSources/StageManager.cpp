#include "StageManager.h"

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