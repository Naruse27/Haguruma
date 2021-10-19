#include "Gear.h"
#include "Collision.h"
#include "GameLibSource/Framework.h"
#include "GimmickManager.h"

Gear::Gear(ID3D11Device* device)
{
    model.reset(new Model(device, "Data/Model/Gimmick/gear2.fbx", true, 0, TRUE));
    //model.reset( new Model(device, "Data/Model/Player/test1.fbx", true, 0, TRUE));

    scale = { 0.5f, 0.5f, 0.5f };

    GearFunction[STATE::Wait] = &Gear::WaitGear;
    GearFunction[STATE::Strait] = &Gear::StraitThrow;
    GearFunction[STATE::Back] = &Gear::ComeBackStrait;
    GearFunction[STATE::Set] = &Gear::AdhesionGear;
}

Gear::~Gear()
{
}

// �X�V
void Gear::Update(float elapsedTime)
{ 
    (this->*GearFunction[state])(elapsedTime);

    //�I�u�W�F�N�g�s��X�V
    UpdateTransform();

    // ���f���A�j���[�V�����X�V����
    //model->UpdateAnimation(elapsedTime);
    angle.y += 1.0f * elapsedTime;

    //���f���s��X�V
    model->UpdateTransform(transform);
}

// �`��
void Gear::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::DEFAULT), true);
    model->Render(deviceContext);
}

bool Gear::OnMessage(const Telegram& telegram)
{
    return true;
}

void Gear::Launch(const Vector3& direction, const Vector3& position)
{
    this->direction = direction;
    this->position = position;

    state = STATE::Strait;
}

// ��ƍ��̂����悤
void Gear::IsSetPosition(const Vector3& position)
{
    this->position = position;
    state = STATE::Set;
}

void Gear::Collection()
{
    this->position = target;
    state = STATE::Wait;
}

void Gear::StraitThrow(float elapsedTime)
{
    GimmickManager::Instance().CollisionGimmickGimmicks(this);

    if (setFlag) {
        state = STATE::Set;
        straitTimer = 4.0f;
    }
    else {
        straitTimer -= elapsedTime;
        if (straitTimer < 0.0f) {
            state = STATE::Back;
            straitTimer = 4.0f;
        }
    }
    // �ړ�
    float speed = this->speed * elapsedTime;
    position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;
}

void Gear::ComeBackStrait(float elapsedTime)
{
    // �ړ�
    float moveSpeed = this->moveSpeed * elapsedTime;
    position.x += direction.x * moveSpeed;
    position.y += direction.y * moveSpeed;
    position.z += direction.z * moveSpeed;

    // ����
    {
        float turnSpeed = this->turnSpeed * elapsedTime;

        // �^�[�Q�b�g�܂ł̃x�N�g�����Z�o
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        // �[���x�N�g���łȂ��Ȃ��]����
        DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
        float lengthSq;
        DirectX::XMStoreFloat(&lengthSq, LengthSq);
        float length = sqrtf(lengthSq);
        if (length < 1.0f) tracking_check = true;

        if (!tracking_check)
        {
            if (lengthSq > 0.00001f) {
                // �^�[�Q�b�g�܂ł̃x�N�g����P�ʃx�N�g����
                Vec = DirectX::XMVector3Normalize(Vec);

                // �����Ă�������x�N�g�����Z�o
                DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

                // �O�����x�N�g���ƃ^�[�Q�b�g�܂ł̃x�N�g���̓���(�p�x)���Z�o
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);

                float dot;
                DirectX::XMStoreFloat(&dot, Dot);

                // 2�̒P�ʃx�N�g���̊p�x���������ق�
                // 1.0�ɋ߂Â������𗘗p���ĉ�]���x�𒲐߂���
                float rot = 1.0f - dot;
                if (rot > turnSpeed) {
                    rot *= turnSpeed;
                }

                // ��]�p�x������Ȃ��]����������
                if (fabsf(rot) > 0.01f) {

                    // ��]�����Z�o
                    DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);

                    // ��]���Ɖ�]�ʂ����]�s����Z�o
                    DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

                    // ���݂̍s�����]������
                    DirectX::XMMATRIX Tranceform = DirectX::XMLoadFloat4x4(&transform);
                    Tranceform = Tranceform * Rotation;
                    //DirectX::xmmarutipuly

                    // ��]��̑O�����������o���A�P�ʃx�N�g��������
                    Direction = DirectX::XMVector3Normalize(Tranceform.r[2]);
                    //Direction = DirectX::XMVector3Normalize(Direction);
                    DirectX::XMStoreFloat3(&direction, Direction);
                }
            }
        }
    }

    if (Collision::AabbVsAabb(position, 4.0f, 4.0f, target, 4.0f, 4.0f)) state = STATE::Wait;
}

void Gear::WaitGear(float elapsedTime)
{
    position = target;
}

//template <class T>
//Gear::STATE getState(T& t) {
//    for (auto it = t.begin(); it != t.end();) {
//        if (it->expired())it = t.erase(it)
//        else ++it;
//    }
//}

void Gear::AdhesionGear(float elapsedTime)
{
    //std::vector<std::weak_ptr<Gear>> aaaaaaa;
    //std::vector<std::weak_ptr<float>> aaaaaba;
    //std::vector<std::weak_ptr<std::vector<std::vector<std::shared_ptr<Gear>>>>> aaaaaaca;
    //getState(aaaaaaca);
}

