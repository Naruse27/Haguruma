#include "Gimmick.h"

void Gimmick::UpdateTransform()
{
    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    // 回転行列を作成
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = Y * X * Z;

    // 位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    // 3つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;
    // 計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}

bool Gimmick::HandleMessage(const Telegram& msg)
{
	// メッセージの受信時の処理を実行。戻り値で成功有無を判断
	if (OnMessage(msg)) return true;
	
	return false;
}

bool Gimmick::OnMessage(const Telegram& telegram)
{
    return true;
	// それぞれのメッセージに併せた処理を記述。
	switch (telegram.msg)
	{
    case MESSAGE_TYPE::MSG_SIGNAL_GIAR:
    case MESSAGE_TYPE::MSG_SIGNAL_STAND:
		break;
	}
	return false;
}