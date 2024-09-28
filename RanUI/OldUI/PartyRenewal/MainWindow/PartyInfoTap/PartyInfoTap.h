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
        PARTY_INFO_ITEM_COMBO_OPEN = NO_ID + 1,  // ��Ƽ �����ۺй� ��� �޺��ڽ�;
        PARTY_INFO_ITEM_COMBO_ROLLOVER,  // ��Ƽ ������ �й� ��� �޺� �ڽ� �ѿ��� �޴�;
        PARTY_INFO_ITEM_FILTER_COMBO_OPEN, // ��Ƽ �����ۺй� ��� �޺��ڽ�;
        PARTY_INFO_ITEM_FILTER_COMBO_ROLLOVER,  // ��Ƽ �����ۺй� ��� �޺��ڽ� �ѿ��� �޴�;
        PARTY_INFO_MONEY_COMBO_OPEN,  // ��Ƽ ���ȹ���� �޺��ڽ�;
        PARTY_INFO_MONEY_COMBO_ROLLOVER,  // ��Ƽ ���ȹ���� �޺��ڽ� �ѿ��� �޴�;

		PARTY_INFO_BUFF_CHECK, // ������ ���� ����;
		PARTY_INFO_INFO_CHECK, // ��Ƽ�� ���� ����
		PARTY_INFO_RELEASE_PARTY_BUTTON, //��Ƽ �ػ�;
		PARTY_INFO_EXPULSION_BUTTON, // �߹� ��ư;
		PARTY_INFO_DELEGATE_BUTTON, // ���� ��ư;
		PARTY_INFO_SECEDE_BUTTON, // ��Ƽ Ż�� ��ư;
		PARTY_INFO_MENU_POPUP, // �˾� �޴�;

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
		POPUP_MENU_DELEGATE, // ��Ƽ�� ����;
		POPUP_MENU_WHISPER, // �ӼӸ�;
		POPUP_MENU_VIEW_INFO, // ���� ����;
		POPUP_MENU_ADD_FRIEND, // ģ�� ���;
		POPUP_MENU_BLOCK, // ���� ���;
		POPUP_MENU_BLOCK_DELETE, // ���� ����;
		POPUP_MENU_EXPULSION, // �߹�;
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
	void TranslateUIMessage_BuffCheck(void); // ��Ƽ�� ���� ����;	
	void TranslateUIMessage_InfoCheck(void); // ��Ƽ�� ���� ����;	
	void TranslateUIMessage_Whisper(void); // �ӼӸ�;
	void TranslateUIMessage_ViewInfo(void); // ���� ����;
	void TranslateUIMessage_AddFriend(void); // ģ�� ���;
	void TranslateUIMessage_BlockFriend(const bool bBlock); // ����/���� ����;

// 	void TranslateUIMessage_Delegate(void); // ��Ƽ�� ����;
// 	void TranslateUIMessage_Expulsion(void); // ��Ƽ�� �߹�;
// 	void TranslateUIMessage_ReleaseParty(void); // ��Ƽ �ػ�;

	void TranslateDelegateCallback(const bool bYes = true); // ��Ƽ�� ����;
	void TranslateExpulsionCallback(const bool bYes = true); // ��Ƽ�� �߹�;
	void TranslateReleasePartyCallback(const bool bYes = true); // ��Ƽ �ػ�;
	void TranslateSecedeCallback(const bool bYes = true); // ��Ƽ Ż��;

	void TranslateSlotPopupCallback(int nIndex); // ��Ƽ�� ���� �˾�;

public:
	PartyInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~PartyInfoTap(void);

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;	

	popupmenu::SP_POPUP_MENU m_pSlotPopup;
    popupmenu::SP_POPUP_MENU m_pSlotPopupSub[POPUP_MENU_NSIZE];
    CBasicComboBox* m_pComboBoxItem; // ������ �й� �޺� �ڽ�;
    CBasicComboBoxRollOver* m_pComboRollOverItem; // ������ �й� �޺� �ڽ� �ѿ��� �޴�;
    CBasicComboBox* m_pComboBoxItemFilter; // ������ �й� ���� �޺� �ڽ�;
    CBasicComboBoxRollOver* m_pComboRollOverItemFilter; // ������ �й� ���� �޺� �ڽ� �ѿ��� �޴�;
    CBasicComboBox* m_pComboBoxMoney; // ���ӸӴ� �й� �޺� �ڽ�;
    CBasicComboBoxRollOver* m_pComboRollOverMoney; // ���ӸӴ� �й� �޺� �ڽ� �ѿ��� �޴�;
	CBasicTextBox*	m_pTextBoxItemFilter;

	CCheckBox* m_pBuffCheckBox; // ���� ���� üũ �ڽ�;
	CCheckBox* m_pInfoCheckBox; // ��Ƽ�� ���� üũ �ڽ�;

	RnButton* m_pInviteButton; // ��Ƽ �ʴ� ��ư;
	RnButton* m_pReleasePartyButton; // ��Ƽ �ػ� ��ư;
	RnButton* m_pExpulsionButton; // �߹� ��ư;
	RnButton* m_pDelegateButton; // ���� ��ư;
	RnButton* m_pInfoButton; // ���� ���� ��ư;
	RnButton* m_pSecedeButton; // Ż�� ��ư;
	RnButton* m_pAddFriendButton; // ģ�� �߰� ��ư;
	RnButton* m_pShowInfoButton; // ���� ���� ��ư;
	
    CBasicTextBox* m_pNonPartyText; //��Ƽ�������������� �ؽ�Ʈ;

	PartyInfoSlot* m_pMemberSlot[MAXPARTY];
	SPartyOption m_PartyOption;	
	DWORD m_dwSelectedSlot;

	DWORD m_nSecedeID;
	DWORD m_nExpulsionID;
	DWORD m_nReleasePartyID;
	DWORD m_nDelegateID;

    bool  m_bLockedClick;
};