#include "StageWindmill.h"

#include "Input/Input.h"


StageWindmill::StageWindmill(ID3D11Device* device)
{
    model = std::make_unique<Model>(device, "Data/Model/Stage/Cube.fbx", true, 0, TRUE);

    for (int i = 0; i < FLOORNUM; i++) {
        Model* m = new Model(device, "Data/Model/Stage/Cube.fbx", true, 0, TRUE);
        models.emplace_back(m);
    }

    DirectX::XMStoreFloat3(&axisX, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&axisX)));
    DirectX::XMStoreFloat3(&axisY, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&axisY)));
    DirectX::XMStoreFloat3(&axisZ, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&axisZ)));

    scale = { 2.0f, 0.5f, 2.0f };

    //position = { 0,r,0 };
    pos = { 0,r,0 };

    

    //回転させたい軸
    DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&axisZ);
    //それ以外のどれか
    DirectX::XMVECTOR axis2 = DirectX::XMLoadFloat3(&axisY);

    for (int i = 0; i < FLOORNUM; i++) {
        DirectX::XMMATRIX Mat = DirectX::XMMatrixRotationAxis(axis, DirectX::XMConvertToRadians(angle + (360 / FLOORNUM * i)));

        DirectX::XMFLOAT3 p{};
        DirectX::XMStoreFloat3(&p, DirectX::XMVectorScale(DirectX::XMVector3Transform(axis2, Mat), r));
        position = { p.x + pos.x,  p.y + pos.y,  p.z + pos.z };

        UpdateTransform();
        models.at(i)->UpdateTransform(transform);
    }
}

StageWindmill::~StageWindmill()
{
}

void StageWindmill::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A) gear = !gear;

    gear ? gimmickOn(elapsedTime) : gimmickOff(elapsedTime);

}

bool StageWindmill::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    for (auto& m : models) {
        if (Collision::IntersectRayVsModel(start, end, m.get(), hit)) return true;
    }

    return false;
    //return Collision::IntersectRayVsModel(start, end, model.get(), hit);
}

void StageWindmill::gimmickOn(float elapsedTime)
{
    angle += speed;
    //回転させたい軸
    DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&axisZ);
    //それ以外のどれか
    DirectX::XMVECTOR axis2 = DirectX::XMLoadFloat3(&axisY);

    for (int i = 0; i < FLOORNUM; i++) {
        DirectX::XMMATRIX Mat = DirectX::XMMatrixRotationAxis(axis, DirectX::XMConvertToRadians(angle + (360 / FLOORNUM * i)));

        DirectX::XMFLOAT3 p{};
        DirectX::XMStoreFloat3(&p, DirectX::XMVectorScale(DirectX::XMVector3Transform(axis2, Mat), r));
        position = { p.x + pos.x,  p.y + pos.y,  p.z + pos.z };

        UpdateTransform();
        models.at(i)->UpdateTransform(transform);
    }
    // モデルアニメーション更新処理
    //model->UpdateAnimation(elapsedTime);
    //UpdateTransform();
    //model->UpdateTransform(transform);
}

void StageWindmill::gimmickOff(float elapsedTime)
{
    if (DOWNMAX < position.y) position.y -= elapsedTime;
}

void StageWindmill::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::DEFAULT), true);
    //model->Render(deviceContext);

    for (int i = 0; i < FLOORNUM; i++) {
        models.at(i)->Render(deviceContext);
    }
}