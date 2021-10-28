#ifndef TELEGRAM
#define TELEGRAM

// 送信するメッセージの種類
//enum class MESSAGE_TYPE : int
//{
//	MSG_CALL_HELP,			                	// 仲間を呼ぶ(Enemy、メタAIの両方で使用)
//	MSG_CHANGE_ATTACK_RIGHT,	// 攻撃権の移譲(EnemyからメタAIへのメッセージ用)
//	MSG_GIVE_ATTACK_RIGHT,	    	// 攻撃権を与える（メタAIからEnemyへのメッセージ用）
//	MSG_ASK_ATTACK_RIGHT,	    	// 攻撃権を要求(EnemyからメタAIへのメッセージ用)
//};

enum class Identity : int
{
	Meta = 0,
	Player = 1,	// プレイヤーIDは1～4

	//Enemy = 5	,// エネミーIDは5以降を付与
	Start = 5,
	Save,
	End,
	Gear = 15,
	Stand = 30,
};

enum class MESSAGE_TYPE : int
{
	MSG_PLAYER_DEATH,                                                  // プレイヤーの死亡
	MSG__CALL_REVIVAL_POSSIBLE,                            // 復活可能 
	MSG_CALL_START_POINT,
	MSG_CALL_ARRIVAL_CHECCK_POINT,
	MSG_SIGNAL_GIAR,			                                           	// ギアの信号
	MSG_SIGNAL_STAND,	                                                // 台の信号
	MSG_SIGNAL_GIAR_COALESCENCE,              	    	// ギアからの合体信号
	MSG_SIGNAL_STAND_COALESCENCE,	    	            // 攻撃権を要求(EnemyからメタAIへのメッセージ用)
};

class Telegram
{
public:
	int sender;			// 送り手のID
	int receiver;		// 受けてのID
	MESSAGE_TYPE msg;	// メッセージNO

	//コンストラクタ
	Telegram(int sender, int receiver, MESSAGE_TYPE msg) :sender(sender), receiver(receiver), msg(msg)
	{}
};

#endif // !TELEGRAM

