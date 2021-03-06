#include "Stand.h"
#include "GameLibSource/Shader.h"
#include "GimmickManager.h"

Stand::Stand(ID3D11Device* device)
{
    model.reset(new Model(device, "Data/Model/Gimmick/dai2.fbx", true, 0, TRUE));

    identitiy = Identity::Stand;

    scale = { 0.5f, 0.5f, 0.5f };

    position = { 0.0, 0.0f, -40.0f };
}

Stand::~Stand()
{
}

// 更新
void Stand::Update(float elapsedTime)
{
    if (setFlag) SenderSetFragCheck();

    //オブジェクト行列更新
    UpdateTransform();

    //モデル行列更新
    model->UpdateTransform(transform);
}

// 描画
void Stand::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
      model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::NORMAL_MAP), true);
      model->Render(deviceContext);
}

bool Stand::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    // 前回のワールドと逆行列を求める
    DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(&transform);
    DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, World); // 行列の逆行列を計算

    // 前回のローカル空間でのレイに変換
    DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
    // 指定された行列によって 3D ベクトルを変換し、結果を w = 1 に投影 ↓
    DirectX::XMVECTOR LocalStart = DirectX::XMVector3TransformCoord(Start, InverseWorldTransform);
    DirectX::XMVECTOR LocalEnd = DirectX::XMVector3TransformCoord(End, InverseWorldTransform);

    // ローカル空間でのレイとの交点を求める
    DirectX::XMFLOAT3 localStart, localEnd;
    DirectX::XMStoreFloat3(&localStart, LocalStart);
    DirectX::XMStoreFloat3(&localEnd, LocalEnd);

    HitResult localHit;
    if (Collision::IntersectRayVsModel(localStart, localEnd, model.get(), localHit)) {
        // 前回のローカル空間から今回のワールド空間へ変換
        // 前回から今回にかけて変更された内容が乗っているオブジェクトに反映される
        World = DirectX::XMLoadFloat4x4(&transform);
        DirectX::XMVECTOR LocalPosition = DirectX::XMLoadFloat3(&localHit.position);
        DirectX::XMVECTOR Position = DirectX::XMVector3TransformCoord(LocalPosition, World);
        DirectX::XMVECTOR LocalNormal = DirectX::XMLoadFloat3(&localHit.normal);
        DirectX::XMVECTOR Normal = DirectX::XMVector3TransformNormal(LocalNormal, World);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Position, Start);
        DirectX::XMVECTOR Dist = DirectX::XMVector3Length(Vec);
        DirectX::XMStoreFloat3(&hit.position, Position);
        DirectX::XMStoreFloat3(&hit.normal, Normal);
        DirectX::XMStoreFloat(&hit.distance, Dist);
        hit.materialIndex = localHit.materialIndex;

        return true;
    }
    return false;
}

void Stand::SenderSetFragCheck()
{
    Gimmick* gimmick = GimmickManager::Instance().GetGimmickFromId(senderId);

    if (!gimmick->GetSetFlag()) setFlag = false;
    else setFlag = true;
}