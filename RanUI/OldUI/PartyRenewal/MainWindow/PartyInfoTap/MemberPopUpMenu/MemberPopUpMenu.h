#pragma once

#include "../../../../../InnerInterface.h"
#include "../../../../Util/UIGroupHelper.h"

class CBasicComboBoxRollOver;
class MemberPopUpMenu : public CUIGroupHelper
{
private:
	enum
	{
		MEMBER_MENU_DELEGATE = NO_ID + 1, // 파티장 위임;
		MEMBER_MENU_WHISPER, // 귓속말;
		MEMBER_MENU_VIEW_INFO, // 정보 보기;
		MEMBER_MENU_REGIST_FRIEND, // 친구 등록;
		MEMBER_MENU_BLOCK, // 차단 등록;
		MEMBER_MENU_BLOCK_DELETE, // 차단 해제;
		MEMBER_MENU_EXPULSION, // 추방;
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
	CBasicComboBoxRollOver*	m_pPopUpMenu; // 파티원 팝업 메뉴;
};