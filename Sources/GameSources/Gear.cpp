#include "Gear.h"
#include "Collision.h"
#include "GameLibSource/Framework.h"
#include "GimmickManager.h"
#include "StageManager.h"

Gear::Gear(ID3D11Device* device)
{
    model.reset(new Model(device, "Data/Model/Gimmick/gear2.fbx", true, 0, TRUE));
    //model.reset( new Model(device, "Data/Model/Player/test1.fbx", true, 0, TRUE));

    setFlag = false;

    //scale = { 0.5f, 0.5f, 0.5f };

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
    firingFlag = true;
    state = STATE::Strait;
}

// ��ƍ��̂����悤
void Gear::IsSetPosition(const Vector3& position)
{
    this->position = position;
    firingFlag = false;
    state = STATE::Set;
}

void Gear::Collection()
{
    firingFlag = false;
    setFlag = false;
    this->position = target;
    state = STATE::Wait;
}

void Gear::StraitThrow(float elapsedTime)
{
    GimmickManager::Instance().CollisionGimmickGimmicks(this);

    // �ړ���
    Vector3 destination;
    float speed = this->speed * elapsedTime;
    destination += direction * speed;
    destination += position;

    // ���C�̊J�n�ʒu�ƏI�_�ʒu
    Vector3 start = { position.x , position.y, position.z };
    Vector3 end = { destination.x, destination.y, destination.z };

    HitResult hit;
    if (StageManager::Instance().RayCast(start, end, hit)) {
            state = STATE::Back;
            straitTimer = 4.0f;
    }
    else position = destination;
    



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
}

void Gear::ComeBackStrait(float elapsedTime)
{
    DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
    DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(Target, Position);
    Direction = DirectX::XMVector3Normalize(Direction);
    DirectX::XMStoreFloat3(&direction, Direction);

    // �ړ�
    float moveSpeed = this->speed * elapsedTime;
    position.x += direction.x * moveSpeed;
    position.y += direction.y * moveSpeed;
    position.z += direction.z * moveSpeed;

    if (Collision::AabbVsAabb(position, 4.0f, 4.0f, target, 4.0f, 4.0f)) state = STATE::Wait;
}

void Gear::WaitGear(float elapsedTime)
{
    firingFlag = false;
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