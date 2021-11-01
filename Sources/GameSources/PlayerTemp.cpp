#include "PlayerTemp.h"
#include "../GameLibSource/Framework.h"
#include "../GameLibSource/Shader.h"
#include "Input/Input.h"
#include "CameraManager.h"

PlayerTemp::PlayerTemp(ID3D11Device* device)
{
	model = new Model(device, "Data/Model/Player/nico.fbx", false, 0, TRUE);

	scale = { 0.01f, 0.01f, 0.01f };
}

PlayerTemp::~PlayerTemp()
{
	delete model;
}

void PlayerTemp::Update(float elapsedTime)
{
    InputMove(elapsedTime);

    // 速力更新処理
    UpdateVelocity(elapsedTime);

    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);

	//オブジェクト行列更新
	UpdateTransform();
	
	// モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime, true);
	
	//モデル行列更新
	model->UpdateTransform(transform);
}

void PlayerTemp::Render(ID3D11DeviceContext* deviceContext)
{
	model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::DEFAULT), true);
	model->Render(deviceContext);
}

void PlayerTemp::DebugImGui()
{
#if 0
#ifdef USE_IMGUI
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	//ImGui::PushStyleColor(ImGuiCond_FirstUseEver, ImVec4(0.0f, 0.7f, 0.2f, 1.0f));

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		// トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::SliderFloat("Position X", &position.x, -2000, 2000);
			ImGui::SliderFloat("Position Y", &position.y, -2000, 2000);
			ImGui::SliderFloat("Position Z", &position.z, -2000, 2000);

			ImGui::SliderFloat("Scale X", &scale.x, 0.01f, 1.0f);
			ImGui::SliderFloat("Scale Y", &scale.y, 0.01f, 1.0f);
			ImGui::SliderFloat("Scale Z", &scale.z, 0.01f, 1.0f);

			ImGui::SliderFloat("Angle X", &angle.x, DirectX::XMConvertToRadians(-180), DirectX::XMConvertToRadians(180));
			ImGui::SliderFloat("Angle Y", &angle.y, DirectX::XMConvertToRadians(-180), DirectX::XMConvertToRadians(180));
			ImGui::SliderFloat("Angle Z", &angle.z, DirectX::XMConvertToRadians(-180), DirectX::XMConvertToRadians(180));
		}

		ImGui::End();
	}
#endif
#endif
}

Vector3 PlayerTemp::GetMoveVec() const
{
    // 入力情報を所得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // カメラ方向とスティックの入力値によって進行方向を計算する
    const DirectX::XMFLOAT3& cameraRight = CameraManager::Instance().mainView.GetRight();
    const DirectX::XMFLOAT3& cameraFront = CameraManager::Instance().mainView.GetFront();

    // 移動ベクトルはxz平面に水平なベクトルになるようにする
    // カメラ右方向ベクトルをxz単位ベクトルに変換
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        // 単位ベクトル化
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    // カメラ前方向ベクトルをXZ単位ベクトルに変換
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        // 単位ベクトル化
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    // スティックの水平入力値をカメラ右方向に反映し、
    // スティックの垂直入力値をカメラ前方向に反映し、
    // 進行ベクトルを計算する
    Vector3 vec;
    vec.z = cameraFrontZ * ay + cameraRightZ * ax;
    vec.x = cameraFrontX * ay + cameraRightX * ax;
    // Y軸方向には移動しない
    vec.y = 0.0f;

    return vec;
}

bool PlayerTemp::InputMove(float elapsedTime)
{
    // 進行ベクトル所得
    Vector3 moveVec = GetMoveVec();

    // 移動処理
    Move(moveVec.x, moveVec.z, moveSpeed);

    // 旋回処理
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

    // 進行ベクトルがゼロベクトルでない場合は入力された
    return moveVec.x || moveVec.y || moveVec.z;
}