#include "Gear.h"
#include "Collision.h"
#include "GameLibSource/Framework.h"
#include "GimmickManager.h"
#include "StageManager.h"
#include "WoodenBox.h"

Gear::Gear(ID3D11Device* device)
{
    model.reset(new Model(device, "Data/Model/Gimmick/gear2.fbx", true, 0, TRUE));
    //model.reset( new Model(device, "Data/Model/Player/test1.fbx", true, 0, TRUE));

    setFlag = false;

    //scale = { 0.5f, 0.5f, 0.5f };

    GearFunction[STATE::Wait] = &Gear::WaitGear;
    GearFunction[STATE::Strait] = &Gear::StraitThrow;
    GearFunction[STATE::FlyIdle] = &Gear::FryIdeleGear;
    GearFunction[STATE::Back] = &Gear::ComeBackStrait;
    GearFunction[STATE::Set] = &Gear::AdhesionGear;

    //木箱の初期値も設定
    {
        WoodenBox* obj = new WoodenBox(device, &destructionManager);
    }
}

Gear::~Gear()
{
}

// 更新
void Gear::Update(float elapsedTime)
{ 
    (this->*GearFunction[state])(elapsedTime);

    //オブジェクト行列更新
    UpdateTransform();

    // モデルアニメーション更新処理
    //model->UpdateAnimation(elapsedTime);
    angle.y += 1.0f * elapsedTime;

    //モデル行列更新
    model->UpdateTransform(transform);

    destructionManager.update(elapsedTime);
}

// 描画
void Gear::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::NORMAL_MAP), true);
    model->Render(deviceContext);

    destructionManager.render(deviceContext, elapsedTime);
}

bool Gear::OnMessage(const Telegram& telegram)
{
    return false;
}

void Gear::Launch(const Vector3& direction, const Vector3& position)
{
    this->direction = direction;
    this->position = position;
    firingFlag = true;
    state = STATE::Strait;
}

// 台と合体したよう
void Gear::IsSetPosition(const Vector3& position)
{
    this->position = position;
    firingFlag = false;
    state = STATE::Set;
}

// 収納
void Gear::Collection()
{
    firingFlag = false;
    setFlag = false;
    this->position = target;
    state = STATE::Wait;
}

// 直線に飛ばす
void Gear::StraitThrow(float elapsedTime)
{
    GimmickManager::Instance().CollisionGimmickGimmicks(this);
    //ギミックと木箱
    {
        // すべての敵と総当たり判定
        int destCount = destructionManager.getDestructionCount();
        for (int j = 0; j < destCount; ++j)
        {
            ObjectDestruction* objD = destructionManager.getDestruction(j);
            // 衝突処理
            if (Collision::AabbVsAabb(
                GetPosition(),
                GetWidth(),
                GetHeight(),
                objD->GetPosition(),
                objD->GetWidth(),
                objD->GetHeight()))
            {
                objD->setDest();
            }
        }
    }

    // 移動先
    Vector3 destination;
    float speed = this->speed * elapsedTime;
    destination += direction * speed;
    destination += position;

    // レイの開始位置と終点位置
    Vector3 start = { position.x , position.y, position.z };
    Vector3 end = { destination.x, destination.y, destination.z };

    HitResult hit;
    if (StageManager::Instance().RayCast(start, end, hit)) {
            state = STATE::FlyIdle;
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
            state = STATE::FlyIdle;
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

    // 移動
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

// 空中待機
void Gear::FryIdeleGear(float elapsedTime)
{
    flyIdleTimer -= elapsedTime;
    if (flyIdleTimer < 0) {
        flyIdleTimer = 4.0f;
        state = STATE::Back;
    }
}