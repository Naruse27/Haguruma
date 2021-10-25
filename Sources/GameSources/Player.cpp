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

    // 速力更新処理
    UpdateVelocity(elapsedTime);

    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);

    //オブジェクト行列更新
    UpdateTransform();

    // モデルアニメーション更新処理
    //model->UpdateAnimation(elapsedTime);

    //モデル行列更新
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
        // トランスフォーム
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

bool Player::InputMove(float elapsedTime)
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

bool Player::InputJump()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_SPACE) {
        if (jumpCount < jumpLimit) {
            ++jumpCount;
            Jump(jumpSpeed);

            // ジャンプ入力した
            return true;
        }
    }
    return false;
}

void Player::OnLanding()
{
    jumpCount = 0;

    // 下方向の速力が一定以上なら着地ステートへ
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
    // ビューポート
    CD3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    dc->RSGetViewports(&numViewports, &viewport);

    // 変換行列
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

    Mouse& mouse = Input::Instance().GetMouse();
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
        // マウスカーソル座標を取得
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