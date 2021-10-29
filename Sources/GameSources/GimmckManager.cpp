#include "GimmickManager.h"
#include "MetaAI.h"
#include "Collision.h"
#include "ImGui/imgui.h"

// debug
#include <string>

void GimmickManager::Update(float elapsedTime)
{
    for (Gimmick* gimmick : gimmicks) gimmick->Update(elapsedTime);
}

void GimmickManager::Render(ID3D11DeviceContext* deviceContext, float elapsedTIme)
{
    for (Gimmick* gimmick : gimmicks) gimmick->Render(deviceContext, elapsedTIme);
}

void GimmickManager::DebugRender()
{
#ifdef _DEBUG
    ImGui::Begin("Gimmick");

    for (auto& gimmick : gimmicks) {
        std::string p = "gimmick" + std::to_string(gimmick->GetId());
        ImGui::Separator();
        if (ImGui::TreeNode(p.c_str()))
        {
            Vector3 pos = gimmick->GetPosition();
            ImGui::SliderFloat3("translation", &pos.x, -250.0f, +250.0f);
            gimmick->SetPosition(pos);
            ImGui::TreePop();
        }
    }
    ImGui::End();
#endif // _DEBUG
}

void GimmickManager::Clear()
{
    for (Gimmick* gimmick : gimmicks) delete gimmick;
    gimmicks.clear();
}

void GimmickManager::Register(Gimmick* gimmick, Identity type)
{
    // IDを設定
    gimmick->SetId(identity + static_cast<int>(type));
    identity++;//設定したらインクリメントする
    // 登録
    gimmicks.emplace_back(gimmick);
}

bool GimmickManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    bool result = false;
    hit.distance = FLT_MAX;

    for (Gimmick* gimmick : gimmicks) {
        HitResult tmp;
        if (gimmick->GetIdentity() == static_cast<int>(Identity::Gear)) continue;
        if (gimmick->RayCast(start, end, tmp)) {
            if (tmp.distance < hit.distance) {
                hit = tmp;
                result = true;
            }
        }
    }
    return result;
}

void GimmickManager::CollisionGimmickGimmicks(Gimmick* gimmick)
{
    GimmickManager& enemyManager = GimmickManager::Instance();

    // すべての敵と総当たり判定
    int gimmickCount = enemyManager.GetGimmickCount();

    for (int j = 0; j < gimmickCount; ++j)
    {
        Gimmick* gimmick2 = enemyManager.GetGimmick(j);

        if (gimmick2->GetIdentity() != static_cast<int>(Identity::Stand))  continue;
        if (gimmick2->GetSetFlag()) continue;

        // 衝突処理
        if (Collision::AabbVsAabb(
            gimmick->GetPosition(),
            gimmick->GetWidth(),
            gimmick->GetHeight(),
            gimmick2->GetPosition(),
            gimmick2->GetWidth(),
            gimmick2->GetHeight()))
        {
                Vector3 pos = gimmick2->GetPosition();
                pos.y += 5.0f;//gimmick2->GetHeight();
                gimmick->IsSetPosition(pos);
                gimmick->IsSetFlag(true);
                gimmick2->IsSetFlag(true);
                gimmick2->SetSenderID(gimmick->GetId());
        }
    }
}

// IDからエネミーを取得する
Gimmick* GimmickManager::GetGimmickFromId(int id)
{
    for (Gimmick* gimmick : gimmicks)
    {
        if (gimmick->GetId() == id)
            return gimmick;
    }
    return nullptr;
}