#include "StageWall.h"

#include "Input/Input.h"



StageWall::StageWall(ID3D11Device* device)
{
    model = std::make_unique<Model>(device, "Data/Model/Stage/Wall.fbx", true, 0, TRUE);

    scale = { 2.0f, 2.0f, 2.0f };

    position = { 0,0,0 };
}

StageWall::~StageWall()
{
}

void StageWall::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A) gear = !gear;

    gear ? gimmickOn(elapsedTime) : gimmickOff(elapsedTime);


    // モデルアニメーション更新処理
    //model->UpdateAnimation(elapsedTime);

    UpdateTransform();
    model->UpdateTransform(transform);
}

bool StageWall::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model.get(), hit);
}

void StageWall::gimmickOn(float elapsedTime)
{
    if (UPMAX > position.y) position.y += elapsedTime * speed;
}

void StageWall::gimmickOff(float elapsedTime)
{
    if (DOWNMAX < position.y) position.y -= elapsedTime * speed;
}

void StageWall::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::DEFAULT), true);
    model->Render(deviceContext);
}
