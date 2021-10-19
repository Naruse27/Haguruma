#include "GimmickManager.h"
#include "MetaAI.h"
#include "Collision.h"

void GimmickManager::Update(float elapsedTime)
{
    for (Gimmick* gimmick : gimmicks) gimmick->Update(elapsedTime);

    CollisionGimmickGimmicks();
}

void GimmickManager::Render(ID3D11DeviceContext* deviceContext, float elapsedTIme)
{
    for (Gimmick* gimmick : gimmicks) gimmick->Render(deviceContext, elapsedTIme);
}

void GimmickManager::Clear()
{
    for (Gimmick* gimmick : gimmicks) delete gimmick;
    gimmicks.clear();
}

void GimmickManager::Register(Gimmick* gimmick, Identity type)
{
    // ID��ݒ�
    gimmick->SetId(identity + static_cast<int>(type));
    identity++;//�ݒ肵����C���N�������g����
    // �o�^
    gimmicks.emplace_back(gimmick);
}

bool GimmickManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    bool result = false;
    hit.distance = FLT_MAX;

    for (Gimmick* gimmick : gimmicks) {
        HitResult tmp;
        if (gimmick->RayCast(start, end, tmp)) {
            if (tmp.distance < hit.distance) {
                hit = tmp;
                result = true;
            }
        }
    }
    return result;
}

void GimmickManager::CollisionGimmickGimmicks()
{
    GimmickManager& enemyManager = GimmickManager::Instance();

    // ���ׂĂ̓G�Ƒ������蔻��
    int gimmickCount = enemyManager.GetGimmickCount();
    for (int i = 0; i < gimmickCount; ++i) {
        Gimmick* gimmick = enemyManager.GetGimmick(i);

        if (gimmick->GetSetFlag()) continue;

        for (int j = i + 1; j < gimmickCount; ++j)
        {
            Gimmick* gimmick2 = enemyManager.GetGimmick(j);

            if (gimmick->GetIdentity() == gimmick2->GetIdentity()) continue;
            if (gimmick2->GetSetFlag()) continue;

            // �Փˏ���
            if (Collision::AabbVsAabb(
                gimmick->GetPosition(),
                gimmick->GetWidth(),
                gimmick->GetHeight(),
                gimmick2->GetPosition(),
                gimmick2->GetWidth(),
                gimmick2->GetHeight()))
            {
                // �����o����̈ʒu�ݒ�
                if (static_cast<int>(Identity::Stand) == gimmick->GetIdentity()) {
                    Vector3 pos = gimmick->GetPosition();
                    pos.y += 5.0f;//gimmick->GetHeight();
                    gimmick2->IsSetPosition(pos);
                    gimmick->IsSetFlag(true);
                    gimmick2->IsSetFlag(true);
                }
                if (static_cast<int>(Identity::Stand) == gimmick2->GetIdentity()) {
                    Vector3 pos = gimmick2->GetPosition();
                    pos.y += 5.0f;//gimmick2->GetHeight();
                    gimmick->IsSetPosition(pos);
                    gimmick->IsSetFlag(true);
                    gimmick2->IsSetFlag(true);
                }
            }
        } 
    }
}

// ID����G�l�~�[���擾����
Gimmick* GimmickManager::GetGimmickFromId(int id)
{
    for (Gimmick* gimmick : gimmicks)
    {
        if (gimmick->GetId() == id)
            return gimmick;
    }
    return nullptr;
}