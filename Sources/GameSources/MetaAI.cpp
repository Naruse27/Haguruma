#include "MetaAI.h"
//
//static Meta* instance = nullptr;
//
//Meta& Meta::Instance()
//{
//	return *instance;
//}
//
//Meta::Meta(PlayerTemp* pl, PlayerTemp* manager) : player(pl), manager(manager)
//{
//	instance = this;
//}
//
//void Meta::Update()
//{
//	// �����Ŏ����I�Ƀ��^AI���Ď����Ă��鏈��������΋L�ڂ���
//	// ���ۑ�ł̓��^AI�̃C�x���g�g���K�[�̓��b�Z�[�W����M�����Ƃ��̂��ߋL�q���鏈���͖���
//}
//
//// ���̃��\�b�h��telegram�N���X���쐬����
//// Enemy���p������N���X�̃��b�Z�[�W���V�[�o�[�̏������Ăяo���B
//void Meta::Discharge(PlayerTemp* receiver, const Telegram& telegram)
//{
//	//��M�ł��Ȃ������Ƃ��̏���
//	if (!receiver->HandleMessage(telegram)) _RPT0(_CRT_WARN, "\n error:Enemy Receive Failed");
//}
//
//// ���^AI�̃��V�[�o�[�������Ăяo��
//void Meta::Discharge(const Telegram& telegram)
//{
//	//��M�ł��Ȃ������Ƃ��̏���
//	if (!HandleMessage(telegram)) _RPT0(_CRT_WARN, "\n error:Meta Receive Failed");
//}
//
//// ���b�Z�[�W��M���Ă��邩
//bool Meta::HandleMessage(const Telegram& msg)
//{
//	// ���b�Z�[�W����M������
//	if (OnMessage(msg)) return true;
//	
//	return false;
//}
//
//// ���b�Z�[�W��M�����Ƃ��̏���
//bool Meta::OnMessage(const Telegram& telegram)
//{
//	std::vector<Enemy*> enmVec;
//	enmVec.clear();
//	int enemyCount = enemyManager->GetEnemyCount();
//	switch (telegram.msg)
//	{
//	case MESSAGE_TYPE::MSG_CALL_HELP://���ꂩ���v���C���[�𔭌������̂ő��̓G���Ă�
//		for (int i = 0; i < enemyCount; ++i)//�S�Ă̓G�̃��X�g
//		{
//			int enemyId = enemyManager->GetEnemy(i)->GetId();
//			//���M�҂������G�l�~�[�Ƀ��b�Z�[�W�𑗂�
//			if (telegram.sender != enemyId)
//			{
//				// �N�����v���C���[�𔭌������̂ő��M�҂������G�Ƀ��b�Z�[�W�𑗂�
//				// ���^AI����e�G�ɑ��郁�b�Z�[�W�� MSG_CALL_HELP �ł��B
//				SendMessaging(static_cast<int>(Identity::Meta), enemyId, telegram.msg);
//			}
//		}
//		return true;
//		break;
//	case MESSAGE_TYPE::MSG_CHANGE_ATTACK_RIGHT:
//		//�G�l�~�[�}�l�[�W������ATACK_RANGE�ȓ��̃G�l�~�[�����̔z����쐬��
//		for (int i = 0; i < enemyCount; ++i)
//		{
//			Enemy* enemy = enemyManager->GetEnemy(i);
//			DirectX::XMFLOAT3 position = enemy->GetPosition();
//			DirectX::XMFLOAT3 targetPosition = player->GetPosition();
//			float vx = targetPosition.x - position.x;
//			float vy = targetPosition.y - position.y;
//			float vz = targetPosition.z - position.z;
//			float dist = sqrtf(vx * vx + vy * vy + vz * vz);
//
//			if (dist < enemy->GetAttackRange())
//			{
//				enmVec.push_back(enemy);
//			}
//		}
//		if (enmVec.size() > 0)
//		{
//			// �U������enmVec�ɓo�^���ꂽ�G�̈�l�Ƀ����_���ɗ^����
//			// enmVec�̃T�C�Y�ŗ����őΏۂ�����A���肵���G�l�~�[�ɍU�����̈ڏ����b�Z�[�W�𑗐M�B
//			// ���M���b�Z�[�W�� MESSAGE_TYPE::MSG_GIVE_ATTACK_RIGHT
//			int number = rand() % static_cast<int>(enmVec.size()) + static_cast<int>(Identity::Enemy);
//			SendMessaging(static_cast<int>(Identity::Meta), number, MESSAGE_TYPE::MSG_GIVE_ATTACK_RIGHT);
//		}
//		return true;
//		break;
//	case MESSAGE_TYPE::MSG_ASK_ATTACK_RIGHT: //�G����A�ŏ��̍U������v�����ꂽ
//		int count = 0;//�S�Ă̓G�ɂ��āA�U�����������Ă��Ȃ������ׂ�B
//		for (int i = 0; i < enemyCount; ++i)
//		{
//			Enemy* enemy = enemyManager->GetEnemy(i);
//			if (enemy->GetAttackFlg()) count++;
//		}
//		//���ɍU���������Ă���G�����Ȃ��Ȃ�A�v�����Ă����G�ɍU������^���郁�b�Z�[�W�𑗂�B
//		//�v�����Ă����G��id��telegram����擾�o���܂��B
//		if (count < 1)
//		{
//			// �N���U�����������Ă��Ȃ����MetaAI���瑗�M�҂ɍU�����t�^�̃��b�Z�[�W�𑗂�
//			// ���b�Z�[�W��MSG_GIVE_ATTACK_RIGHT
//			SendMessaging(static_cast<int>(Identity::Meta), telegram.sender, MESSAGE_TYPE::MSG_GIVE_ATTACK_RIGHT);
//		}
//		return true;
//		break;
//	}
//	return false;
//}
//
//void Meta::SendMessaging(int sender, int receiver, MESSAGE_TYPE msg)
//{
//	if (receiver == static_cast<int>(Meta::Identity::Meta))
//	{// MetaAI���̎�
//		//���b�Z�[�W�f�[�^���쐬
//		Telegram telegram(sender, receiver, msg);
//		//�f�B���C�������b�Z�[�W�i�����z�����b�Z�[�W�j
//		Discharge(telegram);
//	}
//	else
//	{// �G�l�~�[����M�҂̂Ƃ��ǂ̃G�l�~�[�ɑ��M���邩
//		// ��M�҂̃|�C���^���擾
//		Enemy* receiveEnemy = enemyManager->GetEnemyFromId(receiver);
//		//���V�[�o�[���Ȃ��Ƃ��֐����I������
//		if (receiveEnemy == nullptr) return;
//		//���b�Z�[�W�f�[�^���쐬
//		Telegram telegram(sender, receiver, msg);
//		//�f�B���C�������b�Z�[�W�i�����z�����b�Z�[�W�j
//		Discharge(receiveEnemy, telegram);
//	}
//}