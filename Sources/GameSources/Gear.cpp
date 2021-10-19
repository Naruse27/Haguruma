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
}

// 描画
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

// 台と合体したよう
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
    // 移動
    float speed = this->speed * elapsedTime;
    position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;
}

void Gear::ComeBackStrait(float elapsedTime)
{
    // 移動
    float moveSpeed = this->moveSpeed * elapsedTime;
    position.x += direction.x * moveSpeed;
    position.y += direction.y * moveSpeed;
    position.z += direction.z * moveSpeed;

    // 旋回
    {
        float turnSpeed = this->turnSpeed * elapsedTime;

        // ターゲットまでのベクトルを算出
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        // ゼロベクトルでないなら回転処理
        DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
        float lengthSq;
        DirectX::XMStoreFloat(&lengthSq, LengthSq);
        float length = sqrtf(lengthSq);
        if (length < 1.0f) tracking_check = true;

        if (!tracking_check)
        {
            if (lengthSq > 0.00001f) {
                // ターゲットまでのベクトルを単位ベクトル化
                Vec = DirectX::XMVector3Normalize(Vec);

                // 向いている方向ベクトルを算出
                DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

                // 前方向ベクトルとターゲットまでのベクトルの内積(角度)を算出
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);

                float dot;
                DirectX::XMStoreFloat(&dot, Dot);

                // 2つの単位ベクトルの角度が小さいほど
                // 1.0に近づく性質を利用して回転速度を調節する
                float rot = 1.0f - dot;
                if (rot > turnSpeed) {
                    rot *= turnSpeed;
                }

                // 回転角度があるなら回転処理をする
                if (fabsf(rot) > 0.01f) {

                    // 回転軸を算出
                    DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);

                    // 回転軸と回転量から回転行列を算出
                    DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

                    // 現在の行列を回転させる
                    DirectX::XMMATRIX Tranceform = DirectX::XMLoadFloat4x4(&transform);
                    Tranceform = Tranceform * Rotation;
                    //DirectX::xmmarutipuly

                    // 回転後の前方方向を取り出し、単位ベクトル化する
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

