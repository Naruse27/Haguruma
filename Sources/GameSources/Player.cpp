#include "Player.h"
#include "../GameLibSource/Framework.h"
#include "../GameLibSource/Shader.h"
#include "Input/Input.h"
#include "CameraManager.h"
#include "Input/Mouse.h"
#include "GimmickManager.h"
#include "Gear.h"
#include "StageManager.h"

Player::Player(ID3D11Device* device)
{
    model = new Model(device, "Data/Model/Player/character_tanomuzo.fbx", true, 0, TRUE);

    //model = new Model(device, "Data/Model/Gimmick/test.fbx", true, 0, TRUE);
    for (int i = 0; i < GEAR_NUM; i++) {
        gear[i] = new Gear(device);
        GimmickManager::Instance().Register(gear[i], Identity::Gear);
    }
    position = { 0.0f, 0.0f, 0.0f };


    scale = { 0.05f, 0.05f, 0.05f };

    blackOut.reset(new Sprite(device, L"./Data/Sprite/BlackOut.png", 4));
}

Player::~Player()
{
    delete model;
}

void Player::Update(float elapsedTime)
{
    setPosition = position;
    setPosition.y += height;
    for (int i = 0; i < GEAR_NUM; i++) {
        gear[i]->SetTarget(setPosition);
    }

     InputMove(elapsedTime);

     InputJump();

    // ���͍X�V����
    UpdateVelocity(elapsedTime);

    // ���G���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    //�I�u�W�F�N�g�s��X�V
    UpdateTransform();

    // ���f���A�j���[�V�����X�V����
    //model->UpdateAnimation(elapsedTime);

    //���f���s��X�V
    model->UpdateTransform(transform);

   

    if (deathFlag) {
        scale2d.x += 1.0f * elapsedTime;
        scale2d.y += 1.0f * elapsedTime;
    }
}

void Player::Render(ID3D11DeviceContext* deviceContext)
{


    //ID3D11DeviceContext* deviceContext = Framework::GetInstance().GetContext().Get();
    model->Preparation(deviceContext, ShaderSystem::GetInstance()->GetShaderOfSkinnedMesh(ShaderSystem::ShaderOfSkinnedMesh::NORMAL_MAP), true);
    model->Render(deviceContext);

    if (deathFlag) blackOut->render(deviceContext, { 860, 540 }, { scale2d.x, scale2d.y }, { 0.0f,0.0f }, { 256, 256 }, { 128,128 }, 0, { 1,1,1,1 });

}

bool Player::OnMessage(const Telegram& msg)
{
    if (MESSAGE_TYPE::MSG__CALL_REVIVAL_POSSIBLE == msg.msg) {
        deathFlag = false;
        //position = { 0,0,0 };
        return true;
    }

    return false;
}

void Player::DebugImGui()
{
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    //ImGui::PushStyleColor(ImGuiCond_FirstUseEver, ImVec4(0.0f, 0.7f, 0.2f, 1.0f));
    
    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            Vector3 pos = gear[0]->GetPosition();
            ImGui::SliderFloat("Gear Position X", &pos.x ,-2000, 2000);
            ImGui::SliderFloat("Gear Position Y", &pos.y, -2000, 2000);
            ImGui::SliderFloat("Gear Position Z", &pos.z, -2000, 2000);

            ImGui::SliderFloat("Position X", &position.x, -2000, 2000);
            ImGui::SliderFloat("Position Y", &position.y, -2000, 2000);
            ImGui::SliderFloat("Position Z", &position.z, -2000, 2000);
        }
    
        ImGui::RadioButton("death", deathFlag);
   

        ImGui::End();
    }
#endif
}

Vector3 Player::GetMoveVec() const
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

bool Player::InputMove(float elapsedTime)
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

bool Player::InputJump()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_SPACE) {
        if (jumpCount < jumpLimit) {
            ++jumpCount;
            Jump(jumpSpeed);

            // �W�����v���͂���
            return true;
        }
    }
    return false;
}

void Player::OnLanding()
{
    jumpCount = 0;

    // �������̑��͂����ȏ�Ȃ璅�n�X�e�[�g��
    //if (velocity.y < gravity * 5.0f) TransitionLandState();
}

void Player::DropProcessing()
{
    ++deathCount;
    deathFlag = true;
}


void Player::MouseRay(
    ID3D11Device* device,
    ID3D11DeviceContext* dc,
    const DirectX::XMFLOAT4X4& view,
    const DirectX::XMFLOAT4X4& projection)
{
    // �r���[�|�[�g
    CD3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    dc->RSGetViewports(&numViewports, &viewport);

    // �ϊ��s��
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

    Mouse& mouse = Input::Instance().GetMouse();
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
        // �}�E�X�J�[�\�����W���擾
        DirectX::XMFLOAT3 screenPosition;
        screenPosition.x = static_cast<float>(mouse.GetPositionX());
        screenPosition.y = static_cast<float>(mouse.GetOldPositionY());
        screenPosition.z = 0;
        DirectX::XMVECTOR ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);
        DirectX::XMVECTOR WorldPosition = DirectX::XMVector3Unproject(
            ScreenPosition,
            viewport.TopLeftX,
            viewport.TopLeftY,
            viewport.Width,
            viewport.Height,
            viewport.MinDepth,
            viewport.MaxDepth,
            Projection,
            View,
            World
        );
        DirectX::XMFLOAT3 rayStart;
        DirectX::XMStoreFloat3(&rayStart, WorldPosition);

        screenPosition.z = 1.0f;
        ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);
        WorldPosition = DirectX::XMVector3Unproject(
            ScreenPosition,
            viewport.TopLeftX,
            viewport.TopLeftY,
            viewport.Width,
            viewport.Height,
            viewport.MinDepth,
            viewport.MaxDepth,
            Projection,
            View,
            World
        );
        DirectX::XMFLOAT3 rayEnd;
        DirectX::XMStoreFloat3(&rayEnd, WorldPosition);

        HitResult hit;
        if (StageManager::Instance().RayCast(rayStart, rayEnd, hit)) {
            DirectX::XMFLOAT3 vector;
            vector.x = hit.position.x - position.x;
            vector.y = hit.position.y - position.y;
            vector.z = hit.position.z - position.z;
            DirectX::XMVECTOR Vector = DirectX::XMLoadFloat3(&vector);
            DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vector);
            Vector3 dir;
            //DirectX::XMFLOAT3 dir;
            DirectX::XMStoreFloat3(&dir, Dir);

            for (int i = 0; i < GEAR_NUM; i++)
            {
                if (gear[i]->GetFiringFlag()) continue;
                if (gear[i]->GetSetFlag()) continue;
                else {
                    gear[i]->Launch(dir, setPosition);
                    break;
                }
            }
        }
        else {

            DirectX::XMFLOAT3 vector;
            vector.x = rayEnd.x - position.x;
            vector.y = rayEnd.y - position.y;
            vector.z = rayEnd.z - position.z;
            DirectX::XMVECTOR Vector = DirectX::XMLoadFloat3(&vector);
            DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vector);
            Vector3 dir;
            //DirectX::XMFLOAT3 dir;
            DirectX::XMStoreFloat3(&dir, Dir);

            for (int i = 0; i < GEAR_NUM; i++)
            {
                if (gear[i]->GetFiringFlag()) continue;
                if (gear[i]->GetSetFlag()) continue;
                else {
                    gear[i]->Launch(dir, setPosition);
                    break;
                }
            }
        }
    }

    if (mouse.GetButtonDown() & Mouse::BTN_RIGHT) {
        check = true;
        for (auto& g : gear) {
            if (!g->GetSetFlag()) continue;
        
            Vector3 vec = g->GetPosition() - position;
            float length = sqrtf(vec.x * vec.x + vec.y * vec.y);
            if (length < distance) {
                g->Collection();
            }
        }
    }
}