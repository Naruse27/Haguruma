#include "Stand.h"
#include "GameLibSource/Shader.h"

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

// �X�V
void Stand::Update(float elapsedTime)
{
    angle.y += 1.0f * elapsedTime;

    //�I�u�W�F�N�g�s��X�V
    UpdateTransform();

    //���f���s��X�V
    model->UpdateTransform(transform);
}

// �`��
void Stand::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
      model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::DEFAULT), true);
      model->Render(deviceContext);
}

bool Stand::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    // �O��̃��[���h�Ƌt�s������߂�
    DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(&transform);
    DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, World); // �s��̋t�s����v�Z

    // �O��̃��[�J����Ԃł̃��C�ɕϊ�
    DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
    // �w�肳�ꂽ�s��ɂ���� 3D �x�N�g����ϊ����A���ʂ� w = 1 �ɓ��e ��
    DirectX::XMVECTOR LocalStart = DirectX::XMVector3TransformCoord(Start, InverseWorldTransform);
    DirectX::XMVECTOR LocalEnd = DirectX::XMVector3TransformCoord(End, InverseWorldTransform);

    // ���[�J����Ԃł̃��C�Ƃ̌�_�����߂�
    DirectX::XMFLOAT3 localStart, localEnd;
    DirectX::XMStoreFloat3(&localStart, LocalStart);
    DirectX::XMStoreFloat3(&localEnd, LocalEnd);

    HitResult localHit;
    if (Collision::IntersectRayVsModel(localStart, localEnd, model.get(), localHit)) {
        // �O��̃��[�J����Ԃ��獡��̃��[���h��Ԃ֕ϊ�
        // �O�񂩂獡��ɂ����ĕύX���ꂽ���e������Ă���I�u�W�F�N�g�ɔ��f�����
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