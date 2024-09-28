#pragma once

#include "../../Util/TapSelectorEx.h"
#include "../../Util/UIWindowEx.h"

enum EMLIST_MODE
{
	EMLIST_MODE_FRIEND,
	EMLIST_MODE_CLUB,
};

struct SListSlot
{
	DWORD m_dwDbNum;
	bool m_bSelected;
	char m_szName[CHAR_SZNAME];	
	SListSlot(club::GLCLUBMEMBER_CLIENT* pClubMember);
	SListSlot(SFRIEND* pFriend);
};

class CUIGroup;
class CBasicScrollBarEx;
class RnButton;

class GLGaeaClient;
class ExpeditionInviteSlot;
class ExpeditionInviteWindow : public CUIWindowEx, private CTapSelectorEx<DEFAULT_TAPBUTTON>
{	
	typedef std::vector<SListSlot> InviteListVector;
	enum
	{
		FRIEND_LIST_TAP_WINDOW = ET_CONTROL_NEXT + 1,
		CLUB_LIST_TAP_WINDOW,
		
		FRIEND_LIST_TAP,
		CLUB_LIST_TAP,

		INVITE_BUTTON,
		CLOSE_BUTTON,
		LIST_SCROLL_BAR,

		INVITE_LIST_SLOT0,
		INVITE_LIST_SLOT1,
		INVITE_LIST_SLOT2,
		INVITE_LIST_SLOT3,
		INVITE_LIST_SLOT4,
		INVITE_LIST_SLOT5,
		INVITE_LIST_SLOT6,
		INVITE_LIST_SLOT7,
		INVITE_LIST_SLOT8,
		INVITE_LIST_SLOT9,
		INVITE_LIST_SLOT10,
		INVITE_LIST_SLOT11,
		INVITE_LIST_SLOT12,
		INVITE_LIST_SLOT13,
		INVITE_LIST_SLOT14,
		INVITE_LIST_SLOT15,
		INVITE_LIST_SLOT_NSIZE = (INVITE_LIST_SLOT15 - INVITE_LIST_SLOT0 + 1),
	};

public:
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
	virtual void SetVisibleSingle(BOOL bVisible) override;	

	void CreateSubControl(void);

private:
	virtual void EventSelectedTap(UIGUID controlID) override;

	void TranslateUIMessage_Invite(void);
	void TranslateUIMessage_Close(void);
	void TranslateUIMessage_SelectSlot(const DWORD nIndex);

private:
// 	void UpdateFriendList(void);
// 	void UpdateClubList(void);
	void RefreshFriendList(void);
	void RefreshClubList(void);

public:
	ExpeditionInviteWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~ExpeditionInviteWindow(void);

protected:
	GLGaeaClient* m_pGaeaClient;

	CBasicScrollBarEx* m_pScrollBar;
	RnButton* m_pInviteButton;
	RnButton* m_pCloseButton;

	ExpeditionInviteSlot* m_pListSlot[INVITE_LIST_SLOT_NSIZE];
	InviteListVector m_InviteListVector;	
	EMLIST_MODE m_emListMode;

	DWORD m_nListMember;
	DWORD m_dwCurrentBeginIndex;
};

class MyExpeditionInviteWindow : public IExpeditionInviteWindow, public ExpeditionInviteWindow
{
public:
	MyExpeditionInviteWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~MyExpeditionInviteWindow(void){};

public:
	virtual void CreateUIWindowAndRegisterOwnership(void) override;
};