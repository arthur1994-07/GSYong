#pragma once

#include "../../../../InnerInterface.h"
#include "../../../Util/UIGroupHelper.h"
#include "../../../Util/PopupMenuComponent.h"
#include "../../../../../RanLogic/Party/GLPartyDefine.h"

class CBasicComboBox;
class CBasicComboBoxRollOver;
class CBasicTextBox;
class CCheckBox;
class RnButton;

struct SwapSlotState
{
	void Reset(void)
	{
		fTimer = 0.0f;
		bSwapSlotMode = false;
		bBlank = false;
	}
	const bool isSwapSlotMode(void) const
	{
		return bSwapSlotMode;
	}
	void SetSwapSlotFrom(const DWORD dwGaeaID, const DWORD dwPartyIndex)
	{
		bSwapSlotMode = true;
		SwapSlotFrom.dwGaeaID = dwGaeaID;
		SwapSlotFrom.nPartyIndex = dwPartyIndex;
	}

	SwapSlotState(void)
		: bSwapSlotMode(false), bBlank(false)
		, fTimer(0.0f)
	{
	}

	bool bSwapSlotMode;
	bool bBlank;
	float fTimer;
	GLSwapSlotInformation SwapSlotFrom;
};

class CInnerInterface;
class GLGaeaClient;
class EngineDeviceMan;
class ExpeditionInfoGroup;
class ExpeditionInfoTap : public CUIGroupHelper
{
	enum
	{
		EXPEDITION_INFO_ITEM_COMBO_OPEN = NO_ID + 1,  // 원정대 아이템분배 방식 콤보박스;
        EXPEDITION_INFO_ITEM_COMBO_ROLLOVER,  // 원정대 아이템 분배 방식 콤보 박스 롤오버 메뉴;
		EXPEDITION_INFO_ITEM_FILTER_COMBO_OPEN, // 원정대 아이템분배 등급 콤보박스;
        EXPEDITION_INFO_ITEM_FILTER_COMBO_ROLLOVER,  // 원정대 아이템분배 등급 콤보박스 롤오버 메뉴;
		EXPEDITION_INFO_MONEY_COMBO_OPEN,  // 원정대 골드획득방식 콤보박스;
        EXPEDITION_INFO_MONEY_COMBO_ROLLOVER,  // 원정대 골드획득방식 콤보박스 롤오버 메뉴;
		EXPEDITION_INFO_INVITE_BUTTON, // 원정대 초대 버튼;
		EXPEDITION_INFO_RELEASE_EXPEDITION_BUTTON, // 원정대 해산 버튼;
		EXPEDITION_INFO_FORM_EXPEDITION_BUTTON, // 원정대 구성 버튼;
		EXPEDITION_INFO_SECEDE_EXPEDITION_BUTTON, // 원정대 탈퇴 버튼;

		EXPEDITION_INFO_SLOT0,
		EXPEDITION_INFO_SLOT1,
		EXPEDITION_INFO_SLOT2,
		EXPEDITION_INFO_SLOT3,
		EXPEDITION_INFO_SLOT4,		
	};

	enum EMPOPUP_MENU
	{
		POPUP_MENU_SWAP_SLOT, // 자리 이동;
		POPUP_MENU_DELEGATE, // 원정대장 위임;
		POPUP_MENU_NOMINATE, // 원정대부장 위임;
		POPUP_MENU_CHECK_READY, // 준비 확인;
		POPUP_MENU_MASTER_CHAT, // 마스터 채팅;
		POPUP_MENU_WHISPER, // 귓속말;
		POPUP_MENU_VIEW_INFO, // 정보 보기;
		POPUP_MENU_ADD_FRIEND, // 친구 등록;
		POPUP_MENU_BLOCK, // 차단 등록;
		POPUP_MENU_BLOCK_DELETE, // 차단 해제;
		POPUP_MENU_EXPULSION, // 추방;
		POPUP_MENU_NSIZE,
	};
public:
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);
	virtual void SetVisibleSingle(BOOL bVisible);

	void CreateSubControl(void);
	void CreateSubControl_ExpeditionInfoGroup(void);
	void CreateSubControl_Button(void);
	void CreateSubControl_Text(void);
	void CreateSubControl_ComboBox(void);

	void SetPartyOption(const SPartyOption& PartyOption);
	void SetMasterChatting(const bool bMasterChatting);

private:	
	void TranslateUIMessage_SelectPartySlot(const DWORD dwSlotIndex);
	void TranslateUIMessage_SwapSlotSelect(const DWORD dwSlotIndex);
	void TranslateUIMessage_ViewPopUp(const DWORD dwSlotIndex);
    void TranslateUIMessage_ItemCombo(void);
    void TranslateUIMessage_ItemComboRollOver(void);
    void TranslateUIMessage_ItemFilterCombo(void);
    void TranslateUIMessage_ItemFilterComboRollOver(void);
    void TranslateUIMessage_MoneyCombo(void);
    void TranslateUIMessage_MoneyComboRollOver(void);
	void TranslateUIMessage_Invite(void);
	void TranslateUIMessage_FormExpedition(void);	
	
	void TranslateUIMessage_SwapSlot(void); // 자리 이동;
	void TranslateUIMessage_Whisper(void); // 귓속말;
	void TranslateUIMessage_ViewInfo(void); // 정보 보기;
	void TranslateUIMessage_AddFriend(void); // 친구 등록;
	void TranslateUIMessage_BlockFriend(const bool bBlock); // 차단/차단 해제;

private:
	void TranslateDelegateCallback(const bool bYes); // CALLBACK;
	void TranslateExpulsionCallback(const bool bYes); // CALLBACK;
	void TranslateReleaseExpeditionCallback(const bool bYes); // CALLBACK;
	void TranslateSecedeCallback(const bool bYes);	// CALLBACK;
	void TranslateNominateCallback(const bool bYes); // 원정대 부장 임명;

	void TranslateSlotPopupCallback(int nIndex);  // CALLBACK;

public:
	ExpeditionInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;

	popupmenu::SP_POPUP_MENU m_pExpeditionPopup;
    popupmenu::SP_POPUP_MENU m_pExpeditionPopupSub[POPUP_MENU_NSIZE];

	CBasicComboBox* m_pComboBoxItem; // 아이템 분배 콤보 박스;
    CBasicComboBoxRollOver* m_pComboRollOverItem; // 아이템 분배 콤보 박스 롤오버 메뉴;
	CBasicComboBox* m_pComboBoxItemFilter; // 아이템 분배 필터 콤보 박스;
    CBasicComboBoxRollOver* m_pComboRollOverItemFilter; // 아이템 분배 필터 콤보 박스 롤오버 메뉴;
	CBasicComboBox* m_pComboBoxMoney; // 게임머니 분배 콤보 박스;
    CBasicComboBoxRollOver* m_pComboRollOverMoney; // 게임머니 분배 콤보 박스 롤오버 메뉴;

	CBasicTextBox* m_pTextBoxItemFilter;	// 필터 텍스트 박스
	
	RnButton* m_pInviteButton; // 원정대 초대 버튼;
	RnButton* m_pReleaseExpeditionButton; // 원정대 해산 버튼;
	RnButton* m_pConstructExpeditionButton; // 원정대 구성 버튼;
	RnButton* m_pSecedeExpeditionButton; // 원정대 탈퇴 버튼;

	ExpeditionInfoGroup* m_pExpeditionInfoGroup[MAX_PARTYGROUP];	
	SPartyOption m_PartyOption;

	// 파티원의 슬롯이 아닌 그룹 슬롯 번호;
	DWORD m_dwSelectedPartySlot; // 현재 선택 된 그룹 슬롯;
	DWORD m_dwSelectedPartySlotByPopup; // 팝업 활성화(우클릭) 당시 선택된 그룹 슬롯;
	DWORD m_dwSelectedMemberSlotByPopup; // 팝업 활성화(우클릭) 당신 선택된 그룹의 멤버 슬롯;
	DWORD m_dwSelectedGaeaIDByPopup; // 팝업 활성화(우클릭) 당시 선택된 슬롯의 GaeaID;
	SwapSlotState m_SwapSlotState;

	DWORD m_nSecedeID;
	DWORD m_nExpulsionID;
	DWORD m_nReleasePartyID;
	DWORD m_nDelegateID;
	DWORD m_nNominateID;

    bool m_bChangeMasterChatState;

    bool m_bLockedClick;
};


