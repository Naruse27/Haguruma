#include "StageMain.h"

StageMain::StageMain(ID3D11Device* device)
{
    model.reset(new Model(device, "Data/Model/Stage/stage.fbx", true, 0, TRUE));

    //scale = { 1.0f, 1.0f, 1.0f };
}

StageMain::~StageMain()
{
}

void StageMain::Update(float elapsedTime)
{

    model->UpdateTransform(transform);
}

void StageMain::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::NORMAL_MAP), true);
    model->Render(deviceContext);
}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model.get(), hit);
}