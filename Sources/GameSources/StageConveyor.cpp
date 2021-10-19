#include "StageConveyor.h"
#include "Input/Input.h"

StageConveyor::StageConveyor(ID3D11Device* device)
{
    model = std::make_unique<Model>(device, "Data/Model/Stage/beruto.fbx", true, 0, TRUE);

    scale = { 3.0f, 0.5f, 1.0f };

    position = { 0,0,0 };


    //シーン定数バッファオブジェクト生成
    D3D11_BUFFER_DESC bufferDesc{};
    {
        bufferDesc.ByteWidth = sizeof(CbConveyor);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    }
    HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

StageConveyor::~StageConveyor()
{
}

void StageConveyor::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A) gear = !gear;

    gear ? gimmickOn(elapsedTime) : gimmickOff(elapsedTime);


    //経過時間
    timer += elapsedTime;
    // モデルアニメーション更新処理
    UpdateTransform();
    model->UpdateTransform(transform);
}

bool StageConveyor::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model.get(), hit);
}

void StageConveyor::gimmickOn(float elapsedTime)
{
}

void StageConveyor::gimmickOff(float elapsedTime)
{
}

void StageConveyor::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{

    data.time = timer * SCROLLSPEED;
    data.scrollDirection = direction;
    data.power = power;
    deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
    deviceContext->VSSetConstantBuffers(2, 1, constantBuffer.GetAddressOf());
    model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::DEFAULT), true);
    model->Render(deviceContext);




#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    //ImGui::PushStyleColor(ImGuiCond_FirstUseEver, ImVec4(0.0f, 0.7f, 0.2f, 1.0f));

    if (ImGui::Begin("Stage", nullptr, ImGuiWindowFlags_None))
    {
        // トランスフォーム
        if (ImGui::CollapsingHeader("shader", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderFloat2("scroll", &direction.x, -1, 1);
        }

        ImGui::End();
    }
#endif
}