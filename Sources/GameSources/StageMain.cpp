#include "StageMain.h"

StageMain::StageMain(ID3D11Device* device)
{
    model = new Model(device, "Data/Model/Stage/stage.fbx", true, 0, TRUE);

    scale = { 1.0f, 1.0f, 1.0f };
}

StageMain::~StageMain()
{
    delete model;
}

void StageMain::Update(float elapsedTime)
{

    UpdateTransform();

    // モデルアニメーション更新処理
    //model->UpdateAnimation(elapsedTime);

    model->UpdateTransform(transform);
}

void StageMain::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::DEFAULT), true);
    model->Render(deviceContext);
}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}