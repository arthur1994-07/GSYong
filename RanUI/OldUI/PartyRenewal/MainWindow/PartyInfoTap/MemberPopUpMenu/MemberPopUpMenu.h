#pragma once

#include "../../../../../InnerInterface.h"
#include "../../../../Util/UIGroupHelper.h"

class CBasicComboBoxRollOver;
class MemberPopUpMenu : public CUIGroupHelper
{
private:
	enum
	{
		MEMBER_MENU_DELEGATE = NO_ID + 1, // ��Ƽ�� ����;
		MEMBER_MENU_WHISPER, // �ӼӸ�;
		MEMBER_MENU_VIEW_INFO, // ���� ����;
		MEMBER_MENU_REGIST_FRIEND, // ģ�� ���;
		MEMBER_MENU_BLOCK, // ���� ���;
		MEMBER_MENU_BLOCK_DELETE, // ���� ����;
		MEMBER_MENU_EXPULSION, // �߹�;
		MEMBER_MENU_NSIZE = (MEMBER_MENU_EXPULSION - MEMBER_MENU_DELEGATE + 1),
		MEMBER_MENU_POPUP,
	};

public:
	virtual void CreateSubControl(void);
	virtual void SetVisibleSingle(BOOL bVisible);

public:
	MemberPopUpMenu(EngineDeviceMan* pEngineDevice);
	virtual ~MemberPopUpMenu(void);

private:
	CBasicComboBoxRollOver*	m_pPopUpMenu; // ��Ƽ�� �˾� �޴�;
};