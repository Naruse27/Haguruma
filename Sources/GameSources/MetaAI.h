#ifndef METAAI
#define METAAI

#include <set>
#include "Telegram.h"
#include "Player.h"
#include "GimmickManager.h"

//using MSG_ID = int;

class Meta
{
private:
	// PlayerやEnemyとの通信機能(Messaging)
	// std::setは順序付けされたデータを複数保持することができる順序付集合
	std::set<Telegram> priority;
	Player* player = nullptr;
	GimmickManager* gimmickManager = nullptr;

public:
	//enum class Identity : int
	//{
	//	Meta = 0,
	//	Player = 1,	// プレイヤーIDは1〜4
	//	//Enemy = 5	,// エネミーIDは5以降を付与
	//	Gear = 5,
	//	Stand = 25,
	//};

	// コンストラクタ
	Meta(Player* pl, GimmickManager* gManager);// : player(pl), gimmickManager(gManager) {}
	~Meta() {}

	//// インスタンス取得
	//static Meta& Instance();
	// MetaAIの機能を追加していく
	// 更新処理
	void Update();

	// このメソッドはtelegramクラスを作成して
	// Enemyを継承するクラスのメッセージレシーバーの処理を呼び出す。
	void Discharge(Gimmick* receiver, const Telegram& telegram);
	void Discharge(Character* receiver, const Telegram& telegram);
	// MetaAI用のレシーブ処理を指定
	void Discharge(const Telegram& telegram);
	// メッセージ受信しているか
	bool HandleMessage(const Telegram& msg);
	// メッセージ受信したときの処理
	bool OnMessage(const Telegram& msg);
	// メッセージ送信関数
	void SendMessaging(int sender, int receiver, MESSAGE_TYPE msg);

private:
	int deathCount = 0;
	Vector3 setPosition = {0,0,0};
};

#endif // !METAAI