#include "../GameLibSource/misc.h"
#include "CameraManager.h"


void CameraManager::Init(ID3D11Device* device)
{
    CreateBuffer(device);

    //D3D11_VIEWPORT viewport;
    //UINT numViewports = 1;
    ////deviceContext->RSGetViewports(&numViewports, &viewport);
    //
    //float aspectRatio = viewport.Width / viewport.Height;

 

    mainView.SetLookAt(DirectX::XMFLOAT3(424.242f, 0.0f, -10.0f),
        DirectX::XMFLOAT3(0, 0, 0),
        DirectX::XMFLOAT3(0, 1, 0));

    enemyEditorView.SetLookAt(DirectX::XMFLOAT3(0, 10, -10),
        DirectX::XMFLOAT3(0, 0, 0),
        DirectX::XMFLOAT3(0, 1, 0));

    bgEditorView.SetLookAt(DirectX::XMFLOAT3(0, 10, -10),
        DirectX::XMFLOAT3(0, 0, 0),
        DirectX::XMFLOAT3(0, 1, 0));
}

void CameraManager::CreateBuffer(ID3D11Device* device)
{
    HRESULT hr{ S_OK };

    //シーン定数バッファオブジェクト生成
    D3D11_BUFFER_DESC bufferDesc{};
    {
        bufferDesc.ByteWidth = sizeof(SceneConstants);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
    }
    hr = device->CreateBuffer(&bufferDesc, nullptr, this->constntBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void CameraManager::SetCameraView(ID3D11DeviceContext* deviceContext, Camera* camera)
{
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    deviceContext->RSGetViewports(&numViewports, &viewport);

    float aspectRatio = viewport.Width / viewport.Height;
    mainView.SetPerspectiveFov(
        DirectX::XMConvertToRadians(30),
        aspectRatio,
        0.1f,
        1000.0f
    );
    //DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspectRatio, 0.1f, 1000.0f);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&camera->GetProjection());
    DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&camera->GetEye());
    DirectX::XMVECTOR focus = DirectX::XMVectorSet(camera->GetForcus().x, camera->GetForcus().y, camera->GetForcus().z, 0.0f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(camera->GetUp().x, camera->GetUp().y, camera->GetUp().z, 0.0f);
    DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(eye, focus, up);

    DirectX::XMFLOAT4 c;// = camera->GetEye();
    c.x = camera->GetEye().x;
    c.y = camera->GetEye().y;
    c.z = camera->GetEye().z;
    c.w = 0.0f;

    SceneConstants data{};
    DirectX::XMStoreFloat4x4(&data.viewProjection, V * P);
    //camera->SetProjection(data.viewProjection);
    data.lightDirection = lightDirection;
    data.cameraPosition = c;
    deviceContext->UpdateSubresource(this->constntBuffer.Get(), 0, 0, &data, 0, 0);
    deviceContext->VSSetConstantBuffers(1, 1, this->constntBuffer.GetAddressOf());
    deviceContext->PSSetConstantBuffers(1, 1, this->constntBuffer.GetAddressOf());
    deviceContext->GSSetConstantBuffers(1, 1, this->constntBuffer.GetAddressOf());
}
