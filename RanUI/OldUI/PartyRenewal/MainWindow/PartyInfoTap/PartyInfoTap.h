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

class CInnerInterface;
class GLGaeaClient;
class GLPartyManagerClient;
class EngineDeviceMan;
class PartyInfoSlot;
class PartyInfoTap : public CUIGroupHelper
{
	enum
	{	
        PARTY_INFO_ITEM_COMBO_OPEN = NO_ID + 1,  // 파티 아이템분배 방식 콤보박스;
        PARTY_INFO_ITEM_COMBO_ROLLOVER,  // 파티 아이템 분배 방식 콤보 박스 롤오버 메뉴;
        PARTY_INFO_ITEM_FILTER_COMBO_OPEN, // 파티 아이템분배 등급 콤보박스;
        PARTY_INFO_ITEM_FILTER_COMBO_ROLLOVER,  // 파티 아이템분배 등급 콤보박스 롤오버 메뉴;
        PARTY_INFO_MONEY_COMBO_OPEN,  // 파티 골드획득방식 콤보박스;
        PARTY_INFO_MONEY_COMBO_ROLLOVER,  // 파티 골드획득방식 콤보박스 롤오버 메뉴;

		PARTY_INFO_BUFF_CHECK, // 구성원 버프 보기;
		PARTY_INFO_INFO_CHECK, // 파티원 정보 보기
		PARTY_INFO_RELEASE_PARTY_BUTTON, //파티 해산;
		PARTY_INFO_EXPULSION_BUTTON, // 추방 버튼;
		PARTY_INFO_DELEGATE_BUTTON, // 위임 버튼;
		PARTY_INFO_SECEDE_BUTTON, // 파티 탈퇴 버튼;
		PARTY_INFO_MENU_POPUP, // 팝업 메뉴;

		PARTY_INFO_SLOT0,
		PARTY_INFO_SLOT1,
		PARTY_INFO_SLOT2,
		PARTY_INFO_SLOT3,
		PARTY_INFO_SLOT4,
		PARTY_INFO_SLOT5,
		PARTY_INFO_SLOT6,
		PARTY_INFO_SLOT7,		
	};

	enum EMPOPUP_MENU
	{
		POPUP_MENU_DELEGATE, // 파티장 위임;
		POPUP_MENU_WHISPER, // 귓속말;
		POPUP_MENU_VIEW_INFO, // 정보 보기;
		POPUP_MENU_ADD_FRIEND, // 친구 등록;
		POPUP_MENU_BLOCK, // 차단 등록;
		POPUP_MENU_BLOCK_DELETE, // 차단 해제;
		POPUP_MENU_EXPULSION, // 추방;
		POPUP_MENU_NSIZE,
	};
public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;
    virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
    virtual void SetVisibleSingle(BOOL bVisible) override;

	void CreateSubControl(void);	
	void CreateSubControl_PartyInfoSlot(void);
	void CreateSubControl_Button(void);
	void CreateSubControl_CheckBox(void);
	void CreateSubControl_ComboBox(void);
    void CreateSubControl_TextBox(void);

	void SetPartyOption(const SPartyOption& PartyOption);

private:
	void TranslateUIMessage_SelectPartySlot(const DWORD dwSlotIndex);
	void TranslateUIMessage_ViewSlotPopUp(const DWORD dwSlotIndex);

    void TranslateUIMessage_ItemCombo(void);
    void TranslateUIMessage_ItemComboRollOver(void);
    void TranslateUIMessage_ItemFilterCombo(void);
    void TranslateUIMessage_ItemFilterComboRollOver(void);
    void TranslateUIMessage_MoneyCombo(void);
    void TranslateUIMessage_MoneyComboRollOver(void);
	void TranslateUIMessage_BuffCheck(void); // 파티원 버프 보기;	
	void TranslateUIMessage_InfoCheck(void); // 파티원 정보 보기;	
	void TranslateUIMessage_Whisper(void); // 귓속말;
	void TranslateUIMessage_ViewInfo(void); // 정보 보기;
	void TranslateUIMessage_AddFriend(void); // 친구 등록;
	void TranslateUIMessage_BlockFriend(const bool bBlock); // 차단/차단 해제;

// 	void TranslateUIMessage_Delegate(void); // 파티장 위임;
// 	void TranslateUIMessage_Expulsion(void); // 파티원 추방;
// 	void TranslateUIMessage_ReleaseParty(void); // 파티 해산;

	void TranslateDelegateCallback(const bool bYes = true); // 파티장 위임;
	void TranslateExpulsionCallback(const bool bYes = true); // 파티원 추방;
	void TranslateReleasePartyCallback(const bool bYes = true); // 파티 해산;
	void TranslateSecedeCallback(const bool bYes = true); // 파티 탈퇴;

	void TranslateSlotPopupCallback(int nIndex); // 파티원 슬롯 팝업;

public:
	PartyInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~PartyInfoTap(void);

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;	

	popupmenu::SP_POPUP_MENU m_pSlotPopup;
    popupmenu::SP_POPUP_MENU m_pSlotPopupSub[POPUP_MENU_NSIZE];
    CBasicComboBox* m_pComboBoxItem; // 아이템 분배 콤보 박스;
    CBasicComboBoxRollOver* m_pComboRollOverItem; // 아이템 분배 콤보 박스 롤오버 메뉴;
    CBasicComboBox* m_pComboBoxItemFilter; // 아이템 분배 필터 콤보 박스;
    CBasicComboBoxRollOver* m_pComboRollOverItemFilter; // 아이템 분배 필터 콤보 박스 롤오버 메뉴;
    CBasicComboBox* m_pComboBoxMoney; // 게임머니 분배 콤보 박스;
    CBasicComboBoxRollOver* m_pComboRollOverMoney; // 게임머니 분배 콤보 박스 롤오버 메뉴;
	CBasicTextBox*	m_pTextBoxItemFilter;

	CCheckBox* m_pBuffCheckBox; // 버프 보기 체크 박스;
	CCheckBox* m_pInfoCheckBox; // 파티원 정보 체크 박스;

	RnButton* m_pInviteButton; // 파티 초대 버튼;
	RnButton* m_pReleasePartyButton; // 파티 해산 버튼;
	RnButton* m_pExpulsionButton; // 추방 버튼;
	RnButton* m_pDelegateButton; // 위임 버튼;
	RnButton* m_pInfoButton; // 정보 보기 버튼;
	RnButton* m_pSecedeButton; // 탈퇴 버튼;
	RnButton* m_pAddFriendButton; // 친구 추가 버튼;
	RnButton* m_pShowInfoButton; // 정보 보기 버튼;
	
    CBasicTextBox* m_pNonPartyText; //파티속해있지않을때 텍스트;

	PartyInfoSlot* m_pMemberSlot[MAXPARTY];
	SPartyOption m_PartyOption;	
	DWORD m_dwSelectedSlot;

	DWORD m_nSecedeID;
	DWORD m_nExpulsionID;
	DWORD m_nReleasePartyID;
	DWORD m_nDelegateID;

    bool  m_bLockedClick;
};