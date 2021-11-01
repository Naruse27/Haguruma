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

    // ���͍X�V����
    UpdateVelocity(elapsedTime);

    // ���G���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

	//�I�u�W�F�N�g�s��X�V
	UpdateTransform();
	
	// ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime, true);
	
	//���f���s��X�V
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
		// �g�����X�t�H�[��
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
    // ���͏�������
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
    const DirectX::XMFLOAT3& cameraRight = CameraManager::Instance().mainView.GetRight();
    const DirectX::XMFLOAT3& cameraFront = CameraManager::Instance().mainView.GetFront();

    // �ړ��x�N�g����xz���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���
    // �J�����E�����x�N�g����xz�P�ʃx�N�g���ɕϊ�
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        // �P�ʃx�N�g����
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    // �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        // �P�ʃx�N�g����
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    // �X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
    // �X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
    // �i�s�x�N�g�����v�Z����
    Vector3 vec;
    vec.z = cameraFrontZ * ay + cameraRightZ * ax;
    vec.x = cameraFrontX * ay + cameraRightX * ax;
    // Y�������ɂ͈ړ����Ȃ�
    vec.y = 0.0f;

    return vec;
}

bool PlayerTemp::InputMove(float elapsedTime)
{
    // �i�s�x�N�g������
    Vector3 moveVec = GetMoveVec();

    // �ړ�����
    Move(moveVec.x, moveVec.z, moveSpeed);

    // ���񏈗�
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

    // �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
    return moveVec.x || moveVec.y || moveVec.z;
}