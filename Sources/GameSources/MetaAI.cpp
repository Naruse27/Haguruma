#include "MetaAI.h"

Meta::Meta(Player* pl, GimmickManager* gManager) : player(pl), gimmickManager(gManager)
{
}

void Meta::Update()
{
	// ここで自発的にメタAIが監視している処理があれば記載する
	// 今課題ではメタAIのイベントトリガーはメッセージを受信したときのため記述する処理は無し
	if (player->GetDeathFlag()) SendMessaging(static_cast<int>(Identity::Player), static_cast<int>(Identity::Meta), MESSAGE_TYPE::MSG_PLAYER_DEATH);
	if (player->GetCheckPointFlag()) SendMessaging(static_cast<int>(Identity::Player), static_cast<int>(Identity::Meta), MESSAGE_TYPE::MSG_CALL_ARRIVAL_CHECCK_POINT);
}

// このメソッドはtelegramクラスを作成して
// Enemyを継承するクラスのメッセージレシーバーの処理を呼び出す。
void Meta::Discharge(Gimmick* receiver, const Telegram& telegram)
{
	//受信できなかったときの処理
	if (!receiver->HandleMessage(telegram)) _RPT0(_CRT_WARN, "\n error:Enemy Receive Failed");
}

void Meta::Discharge(Character* receiver, const Telegram& telegram)
{
	//受信できなかったときの処理
	if (!receiver->HandleMessage(telegram)) _RPT0(_CRT_WARN, "\n error:Player Receive Failed");
}

// メタAIのレシーバー処理を呼び出す
void Meta::Discharge(const Telegram& telegram)
{
	//受信できなかったときの処理
	if (!HandleMessage(telegram)) _RPT0(_CRT_WARN, "\n error:Meta Receive Failed");
}

// メッセージ受信しているか
bool Meta::HandleMessage(const Telegram& msg)
{
	// メッセージを受信した時
	if (OnMessage(msg)) return true;
	
	return false;
}

// メッセージ受信したときの処理
bool Meta::OnMessage(const Telegram& receiveGimmick)
{
	Gimmick* gimiick = nullptr;
	switch (receiveGimmick.msg)
	{
	case MESSAGE_TYPE::MSG_PLAYER_DEATH:
		++deathCount;

		gimiick = gimmickManager->GetGimmickFromId(player->GetStartGimmickID());
		player->SetPosition(gimiick->GetPosition());

		SendMessaging(static_cast<int>(Identity::Meta), static_cast<int>(Identity::Player), MESSAGE_TYPE::MSG__CALL_REVIVAL_POSSIBLE);
		return true;
		break;

	case MESSAGE_TYPE::MSG_CALL_ARRIVAL_CHECCK_POINT:
		player->SetStartGimmickID(player->GetCheckPointID());
		//gimiick = gimmickManager->GetGimmickFromId(player->GetCheckPointID());
		//player->SetPosition(gimiick->GetPosition());

		SendMessaging(static_cast<int>(Identity::Meta), static_cast<int>(Identity::Player), MESSAGE_TYPE::MSG__CALL_REVIVAL_POSSIBLE);
		return true;
		break;
	}
	return false;
}

void Meta::SendMessaging(int sender, int receiver, MESSAGE_TYPE msg)
{
	if (receiver == static_cast<int>(Identity::Meta))
	{// MetaAI宛の時
		//メッセージデータを作成
		Telegram telegram(sender, receiver, msg);
		//ディレイ無しメッセージ（即時配送メッセージ）
		Discharge(telegram);
	}
	else if (receiver == static_cast<int>(Identity::Player))
	{// エネミーが受信者のときどのエネミーに送信するか
		//レシーバー居ないとき関数を終了する
		//メッセージデータを作成
		Telegram telegram(sender, receiver, msg);
		//ディレイ無しメッセージ（即時配送メッセージ）
		Discharge(player, telegram);
	}
	else
	{// エネミーが受信者のときどのエネミーに送信するか
		// 受信者のポインタを取得
		Gimmick* receiveGimmick = gimmickManager->GetGimmickFromId(receiver);
		//レシーバー居ないとき関数を終了する
		if (receiveGimmick == nullptr) return;
		//メッセージデータを作成
		Telegram telegram(sender, receiver, msg);
		//ディレイ無しメッセージ（即時配送メッセージ）
		Discharge(receiveGimmick, telegram);
	}
}