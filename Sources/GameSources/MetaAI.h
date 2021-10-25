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
	// Player��Enemy�Ƃ̒ʐM�@�\(Messaging)
	// std::set�͏����t�����ꂽ�f�[�^�𕡐��ێ����邱�Ƃ��ł��鏇���t�W��
	std::set<Telegram> priority;
	Player* player = nullptr;
	GimmickManager* gimmickManager = nullptr;

public:
	//enum class Identity : int
	//{
	//	Meta = 0,
	//	Player = 1,	// �v���C���[ID��1�`4
	//	//Enemy = 5	,// �G�l�~�[ID��5�ȍ~��t�^
	//	Gear = 5,
	//	Stand = 25,
	//};

	// �R���X�g���N�^
	Meta(Player* pl, GimmickManager* gManager) : player(pl), gimmickManager(gManager) {}
	~Meta() {}

	//// �C���X�^���X�擾
	//static Meta& Instance();
	// MetaAI�̋@�\��ǉ����Ă���
	// �X�V����
	void Update();

	// ���̃��\�b�h��telegram�N���X���쐬����
	// Enemy���p������N���X�̃��b�Z�[�W���V�[�o�[�̏������Ăяo���B
	void Discharge(Gimmick* receiver, const Telegram& telegram);
	void Discharge(Character* receiver, const Telegram& telegram);
	// MetaAI�p�̃��V�[�u�������w��
	void Discharge(const Telegram& telegram);
	// ���b�Z�[�W��M���Ă��邩
	bool HandleMessage(const Telegram& msg);
	// ���b�Z�[�W��M�����Ƃ��̏���
	bool OnMessage(const Telegram& msg);
	// ���b�Z�[�W���M�֐�
	void SendMessaging(int sender, int receiver, MESSAGE_TYPE msg);

private:
	int deathCount = 0;
};

#endif // !METAAI