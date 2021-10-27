#include "MetaAI.h"

Meta::Meta(Player* pl, GimmickManager* gManager) : player(pl), gimmickManager(gManager)
{
}

void Meta::Update()
{
	// �����Ŏ����I�Ƀ��^AI���Ď����Ă��鏈��������΋L�ڂ���
	// ���ۑ�ł̓��^AI�̃C�x���g�g���K�[�̓��b�Z�[�W����M�����Ƃ��̂��ߋL�q���鏈���͖���
	if (player->GetDeathFlag()) SendMessaging(static_cast<int>(Identity::Player), static_cast<int>(Identity::Meta), MESSAGE_TYPE::MSG_PLAYER_DEATH);
	if (player->GetCheckPointFlag()) SendMessaging(static_cast<int>(Identity::Player), static_cast<int>(Identity::Meta), MESSAGE_TYPE::MSG_CALL_ARRIVAL_CHECCK_POINT);
}

// ���̃��\�b�h��telegram�N���X���쐬����
// Enemy���p������N���X�̃��b�Z�[�W���V�[�o�[�̏������Ăяo���B
void Meta::Discharge(Gimmick* receiver, const Telegram& telegram)
{
	//��M�ł��Ȃ������Ƃ��̏���
	if (!receiver->HandleMessage(telegram)) _RPT0(_CRT_WARN, "\n error:Enemy Receive Failed");
}

void Meta::Discharge(Character* receiver, const Telegram& telegram)
{
	//��M�ł��Ȃ������Ƃ��̏���
	if (!receiver->HandleMessage(telegram)) _RPT0(_CRT_WARN, "\n error:Player Receive Failed");
}

// ���^AI�̃��V�[�o�[�������Ăяo��
void Meta::Discharge(const Telegram& telegram)
{
	//��M�ł��Ȃ������Ƃ��̏���
	if (!HandleMessage(telegram)) _RPT0(_CRT_WARN, "\n error:Meta Receive Failed");
}

// ���b�Z�[�W��M���Ă��邩
bool Meta::HandleMessage(const Telegram& msg)
{
	// ���b�Z�[�W����M������
	if (OnMessage(msg)) return true;
	
	return false;
}

// ���b�Z�[�W��M�����Ƃ��̏���
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
	{// MetaAI���̎�
		//���b�Z�[�W�f�[�^���쐬
		Telegram telegram(sender, receiver, msg);
		//�f�B���C�������b�Z�[�W�i�����z�����b�Z�[�W�j
		Discharge(telegram);
	}
	else if (receiver == static_cast<int>(Identity::Player))
	{// �G�l�~�[����M�҂̂Ƃ��ǂ̃G�l�~�[�ɑ��M���邩
		//���V�[�o�[���Ȃ��Ƃ��֐����I������
		//���b�Z�[�W�f�[�^���쐬
		Telegram telegram(sender, receiver, msg);
		//�f�B���C�������b�Z�[�W�i�����z�����b�Z�[�W�j
		Discharge(player, telegram);
	}
	else
	{// �G�l�~�[����M�҂̂Ƃ��ǂ̃G�l�~�[�ɑ��M���邩
		// ��M�҂̃|�C���^���擾
		Gimmick* receiveGimmick = gimmickManager->GetGimmickFromId(receiver);
		//���V�[�o�[���Ȃ��Ƃ��֐����I������
		if (receiveGimmick == nullptr) return;
		//���b�Z�[�W�f�[�^���쐬
		Telegram telegram(sender, receiver, msg);
		//�f�B���C�������b�Z�[�W�i�����z�����b�Z�[�W�j
		Discharge(receiveGimmick, telegram);
	}
}