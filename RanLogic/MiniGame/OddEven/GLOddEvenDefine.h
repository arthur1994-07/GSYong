#pragma once

#include "../IMiniGame.h"

namespace MiniGame
{
	enum EMODDEVEN
	{
		EMODDEVEN_MAX_DICE = 2,
		EMODDEVEN_MAX_REWARD = 4,
		EMODDEVEN_REWARD_FAIL = -1, // ���� ��� ���� ������ �ε���;
		EMODDEVEN_REWARD_PRECISION = 100000, // ������ ���̺� ���е�;

		EMODDEVEN_NOT_SELECTED = 0, // ���� ���� ����;		
		EMODDEVEN_EVEN = 1, // ¦��;
		EMODDEVEN_ODD = 2, // Ȧ��;
		EMODDEVEN_NSIZE = 3, // ��ȿ�� �˻��;
	};

	enum EMODDEVEN_STAGE
	{
		EMODDEVEN_STAGE_READY, // "�����Ѵ�/���µ���" ������ �� �ܰ�;
		EMODDEVEN_STAGE_SELECT, // Ȧ��/¦�� ���� �� "������" �� �ܰ�;
		EMODDEVEN_STAGE_ROLL, // ������ ��;
		EMODDEVEN_STAGE_RESULT, // ��� ����;
		EMODDEVEN_STAGE_SELECT_REWARD, // ���� ����, ��ư�� ��Ȱ��ȭ;
		EMODDEVEN_STAGE_RECEIVE_REWARD, // ���� �ޱ�;
		EMODDEVEN_STAGE_NSIZE,
	};	
}