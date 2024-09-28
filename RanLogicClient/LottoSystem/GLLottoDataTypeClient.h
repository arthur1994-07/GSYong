#pragma once

#include "../../RanLogicServer/LottoSystem/GLLottoDataTypeServer.h"

// Log ��������� �ּ� ����;
#ifndef NDEBUG
//#define _EVENT_BOX_MESSAGE_LOG
#endif

namespace LottoSystem
{

	// ���� ó���Ǿ�� �ϴ� �̺�Ʈ;
	// ť�� �̿��� �ܺο��� ������ ���õ� �ൿ�� ������ �� �ֵ��� �Ѵ�;
	enum EVENT_BOX
	{
		NONE,							// �̺�Ʈ�� �߻����� ����;
		EUPDATE_INITIALIZE_SYSTEM,		// �ý��� �ʱ�ȭ;
		EUPDATE_TURN,					// ȸ�� ����;
		EUPDATE_DRAWING_DATE,			// ��÷�� ��ü ����;
		EUPDATE_PROGRESS_TIME,			// ������� �ð� ����;
		EUPDATE_CURRENT_STATE,			// ���� ���� ����;
		EUPDATE_CHANGE_STATE,			// ������� ���� ����;
		EUPDATE_DRAWING_NUM,			// ��÷ ��ȣ ����;
		EUPDATE_BUY_LIST,				// ���� ����Ʈ ����;
		EUPDATE_BUY_LIST_COUNT,			// ���� ����Ʈ ���� ����;
		EUPDATE_CONFIRM_BUY_LIST,		// ��Ȯ�� ���� ����Ʈ ����;
		EUPDATE_ACCUMULATE_MONEY,		// ���� ��÷�� ����;
		EUPDATE_TURN_ACCUMULATE_MONEY,	// Ư�� ���� ��÷�� ����;
		EUPDATE_WIN_NUM,				// ��÷ ��ȣ ����;
		EUPDATE_CONFIRM_TURN_WIN_NUM,	// ��÷ Ȯ�� ��÷ ��ȣ ����;
		EUPDATE_TURN_WIN_NUM,			// Ư�� ��÷ ��ȣ ����;
		EUPDATE_WIN_MAN_LIST,			// Ư�� ��÷�� ���� ����;

		EUPDATE_NUM_DSP,				// Num DSP ����;
		EUPDATE_NUM,					// Num ���� Ƚ�� ���;
		EUPDATE_NUM_INIT,				// Num �ʱ�ȭ;
		EUPDATE_NUM_ADD,				// Num �߰�;
		EUPDATE_NUM_DEL,				// Num ����;

		EVIEW_RANDOM_TYPE,				// ���� Ÿ��;
		EVIEW_INVENT_TYPE,				// ���� Ÿ��;
		EVIEW_CORRECT_SET_WIN_NUM,		// �ùٸ� ��÷��ȣ ����;
		EVIEW_INCORRECT_SET_WIN_NUM,	// �߸��� ��÷��ȣ ����;
		EVIEW_POST_MESSAGE,				// ���� ��;
		EVIEW_NUM_DSP,					// Num DSP;
		EVIEW_LOCK_BUY,					// Lock Buy;
		EVIEW_LOCK_CHANGE_TURN,			// Lock Change Turn;

		EUPDATE_HAVE_MONEY				// ������ ����;
	};

	// �ζ� ���� ����Ʈ;
	struct LOTTO_SYSTEM_BUY_LIST
	{
		std::vector<LOTTO_SYSTEM_BUY> buyList;	// ���� ����Ʈ;

		LOTTO_SYSTEM_BUY_LIST() { }

		LOTTO_SYSTEM_BUY_LIST( const LOTTO_SYSTEM_BUY_LIST& me )
		{
			for( unsigned int i=0; i<me.buyList.size(); ++i )
				buyList.push_back( me.buyList.at( i ) );
		}
	};

}