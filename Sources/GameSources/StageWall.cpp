#include "StageWall.h"

StageWall::StageWall(ID3D11Device* device)
{
    model.reset(new Model(device, "Data/Model/Stage/wall.fbx", true, 0, TRUE));

    scale = { 2.0f, 2.0f, 2.0f };

    position = { 0,0,0 };
}

StageWall::~StageWall()
{
    //delete model;
}

void StageWall::Update(float elapsedTime)
{

    UpdateTransform();


    gimmickOn(elapsedTime);

    // モデルアニメーション更新処理
    //model->UpdateAnimation(elapsedTime);

    model->UpdateTransform(transform);
}

bool StageWall::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model.get(), hit);
}

void StageWall::gimmickOn(float elapsedTime)
{
    position.y += elapsedTime;

}

void StageWall::gimmickOff(float elapsedTime)
{


}

void StageWall::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::DEFAULT), true);
    model->Render(deviceContext);
}