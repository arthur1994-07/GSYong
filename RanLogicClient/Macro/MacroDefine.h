
#pragma once

namespace MACRO
{
	enum TYPE{
		MACRO_NONE = -1,
		MACRO_AUTOMATICSKILLUSE,
		MACRO_RETURNBYOPTION,
		MACRO_NEARESTTARGETTRACE,
		
		
		MACRO_CLOSE,		// ��ȯ �� Ư�� ��ũ�� ���� ����Ǿߵɰ�� ����;
		MACRO_SIZE,
	};
	typedef std::vector<MACRO::TYPE> VEC_TYPE;

	enum MACRO_STATE{
		EMMACRO_STATE_NONE				= 0x00,
		EMMACRO_STATE_ACTIVE			= 0x01,		// ��ũ�� Ȱ��ȭ;
		EMMACRO_STATE_LOCK_USERINPUT	= 0x02,		// ��ũ�� Ȱ��ȭ �Ǿ����� ���� �������� ���ؼ� ���ɸ�;
	};

	enum MACRO_MSGTYPE{
		EMMACRO_MSG_ONOFF = 1,
		EMMACRO_MSG_LOCK_USERINPUT,
		EMMACRO_MSG_SERVEMSG_CLOSE,
		EMMACRO_MSG_REQSERVER_ONOFF,

		EMMACRO_MSG_RETURNBYOPTION_CHECKDRUG,
		EMMACRO_MSG_RETURNBYOPTION_CHECKTIME,
		EMMACRO_MSG_TRACETARGETREQ_FEEDBACK,
		EMMACRO_MSG_AUTOMATICSKILLTAB_ENABLE,
		EMMACRO_MSG_AUTOMATICSKILLTAB_DISABLE,
		EMMACRO_MSG_AUTOMATICSKILLSLOTINFO_UPDATE,
		EMMACRO_MSG_RESERVEACTIONMOVE_FAIL,
	};

	enum MACRO_MSGVALUE{
		// EMMACRO_MSG_LOCK_USERINPUT
		EMMACRO_MSGVALUE_LOCK_USERINPUT_SKILL = 0,
		EMMACRO_MSGVALUE_LOCK_USERINPUT_MOVE = 1,
	};

	struct MACRO_MSG
	{
		MACRO_MSGTYPE type;
		DWORD dwMessage;
	};
	typedef std::queue<MACRO::MACRO_MSG> MSG_QUEUE;
	
	struct ReservedSkillID_Macro
	{
		SNATIVEID sSkillID;
		WORD wSkillQuickSlot;
		ReservedSkillID_Macro()
			: sSkillID(NATIVEID_NULL()), wSkillQuickSlot(-1)
		{
		}
		ReservedSkillID_Macro(SNATIVEID sNID, WORD wSkillSlot)
			:sSkillID(sNID), wSkillQuickSlot(wSkillSlot)
		{
		}
	};
};