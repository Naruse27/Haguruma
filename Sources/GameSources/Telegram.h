#ifndef TELEGRAM
#define TELEGRAM

// ���M���郁�b�Z�[�W�̎��
//enum class MESSAGE_TYPE : int
//{
//	MSG_CALL_HELP,			                	// ���Ԃ��Ă�(Enemy�A���^AI�̗����Ŏg�p)
//	MSG_CHANGE_ATTACK_RIGHT,	// �U�����̈ڏ�(Enemy���烁�^AI�ւ̃��b�Z�[�W�p)
//	MSG_GIVE_ATTACK_RIGHT,	    	// �U������^����i���^AI����Enemy�ւ̃��b�Z�[�W�p�j
//	MSG_ASK_ATTACK_RIGHT,	    	// �U������v��(Enemy���烁�^AI�ւ̃��b�Z�[�W�p)
//};

enum class Identity : int
{
	Meta = 0,
	Player = 1,	// �v���C���[ID��1�`4

	//Enemy = 5	,// �G�l�~�[ID��5�ȍ~��t�^
	Start = 5,
	Save,
	End,
	Gear = 15,
	Stand = 30,
};

enum class MESSAGE_TYPE : int
{
	MSG_PLAYER_DEATH,                                                  // �v���C���[�̎��S
	MSG__CALL_REVIVAL_POSSIBLE,                            // �����\ 
	MSG_CALL_START_POINT,
	MSG_CALL_ARRIVAL_CHECCK_POINT,
	MSG_SIGNAL_GIAR,			                                           	// �M�A�̐M��
	MSG_SIGNAL_STAND,	                                                // ��̐M��
	MSG_SIGNAL_GIAR_COALESCENCE,              	    	// �M�A����̍��̐M��
	MSG_SIGNAL_STAND_COALESCENCE,	    	            // �U������v��(Enemy���烁�^AI�ւ̃��b�Z�[�W�p)
};

class Telegram
{
public:
	int sender;			// ������ID
	int receiver;		// �󂯂Ă�ID
	MESSAGE_TYPE msg;	// ���b�Z�[�WNO

	//�R���X�g���N�^
	Telegram(int sender, int receiver, MESSAGE_TYPE msg) :sender(sender), receiver(receiver), msg(msg)
	{}
};

#endif // !TELEGRAM

