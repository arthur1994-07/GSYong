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
		EXPEDITION_INFO_ITEM_COMBO_OPEN = NO_ID + 1,  // ������ �����ۺй� ��� �޺��ڽ�;
        EXPEDITION_INFO_ITEM_COMBO_ROLLOVER,  // ������ ������ �й� ��� �޺� �ڽ� �ѿ��� �޴�;
		EXPEDITION_INFO_ITEM_FILTER_COMBO_OPEN, // ������ �����ۺй� ��� �޺��ڽ�;
        EXPEDITION_INFO_ITEM_FILTER_COMBO_ROLLOVER,  // ������ �����ۺй� ��� �޺��ڽ� �ѿ��� �޴�;
		EXPEDITION_INFO_MONEY_COMBO_OPEN,  // ������ ���ȹ���� �޺��ڽ�;
        EXPEDITION_INFO_MONEY_COMBO_ROLLOVER,  // ������ ���ȹ���� �޺��ڽ� �ѿ��� �޴�;
		EXPEDITION_INFO_INVITE_BUTTON, // ������ �ʴ� ��ư;
		EXPEDITION_INFO_RELEASE_EXPEDITION_BUTTON, // ������ �ػ� ��ư;
		EXPEDITION_INFO_FORM_EXPEDITION_BUTTON, // ������ ���� ��ư;
		EXPEDITION_INFO_SECEDE_EXPEDITION_BUTTON, // ������ Ż�� ��ư;

		EXPEDITION_INFO_SLOT0,
		EXPEDITION_INFO_SLOT1,
		EXPEDITION_INFO_SLOT2,
		EXPEDITION_INFO_SLOT3,
		EXPEDITION_INFO_SLOT4,		
	};

	enum EMPOPUP_MENU
	{
		POPUP_MENU_SWAP_SLOT, // �ڸ� �̵�;
		POPUP_MENU_DELEGATE, // �������� ����;
		POPUP_MENU_NOMINATE, // ��������� ����;
		POPUP_MENU_CHECK_READY, // �غ� Ȯ��;
		POPUP_MENU_MASTER_CHAT, // ������ ä��;
		POPUP_MENU_WHISPER, // �ӼӸ�;
		POPUP_MENU_VIEW_INFO, // ���� ����;
		POPUP_MENU_ADD_FRIEND, // ģ�� ���;
		POPUP_MENU_BLOCK, // ���� ���;
		POPUP_MENU_BLOCK_DELETE, // ���� ����;
		POPUP_MENU_EXPULSION, // �߹�;
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
	
	void TranslateUIMessage_SwapSlot(void); // �ڸ� �̵�;
	void TranslateUIMessage_Whisper(void); // �ӼӸ�;
	void TranslateUIMessage_ViewInfo(void); // ���� ����;
	void TranslateUIMessage_AddFriend(void); // ģ�� ���;
	void TranslateUIMessage_BlockFriend(const bool bBlock); // ����/���� ����;

private:
	void TranslateDelegateCallback(const bool bYes); // CALLBACK;
	void TranslateExpulsionCallback(const bool bYes); // CALLBACK;
	void TranslateReleaseExpeditionCallback(const bool bYes); // CALLBACK;
	void TranslateSecedeCallback(const bool bYes);	// CALLBACK;
	void TranslateNominateCallback(const bool bYes); // ������ ���� �Ӹ�;

	void TranslateSlotPopupCallback(int nIndex);  // CALLBACK;

public:
	ExpeditionInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;

	popupmenu::SP_POPUP_MENU m_pExpeditionPopup;
    popupmenu::SP_POPUP_MENU m_pExpeditionPopupSub[POPUP_MENU_NSIZE];

	CBasicComboBox* m_pComboBoxItem; // ������ �й� �޺� �ڽ�;
    CBasicComboBoxRollOver* m_pComboRollOverItem; // ������ �й� �޺� �ڽ� �ѿ��� �޴�;
	CBasicComboBox* m_pComboBoxItemFilter; // ������ �й� ���� �޺� �ڽ�;
    CBasicComboBoxRollOver* m_pComboRollOverItemFilter; // ������ �й� ���� �޺� �ڽ� �ѿ��� �޴�;
	CBasicComboBox* m_pComboBoxMoney; // ���ӸӴ� �й� �޺� �ڽ�;
    CBasicComboBoxRollOver* m_pComboRollOverMoney; // ���ӸӴ� �й� �޺� �ڽ� �ѿ��� �޴�;

	CBasicTextBox* m_pTextBoxItemFilter;	// ���� �ؽ�Ʈ �ڽ�
	
	RnButton* m_pInviteButton; // ������ �ʴ� ��ư;
	RnButton* m_pReleaseExpeditionButton; // ������ �ػ� ��ư;
	RnButton* m_pConstructExpeditionButton; // ������ ���� ��ư;
	RnButton* m_pSecedeExpeditionButton; // ������ Ż�� ��ư;

	ExpeditionInfoGroup* m_pExpeditionInfoGroup[MAX_PARTYGROUP];	
	SPartyOption m_PartyOption;

	// ��Ƽ���� ������ �ƴ� �׷� ���� ��ȣ;
	DWORD m_dwSelectedPartySlot; // ���� ���� �� �׷� ����;
	DWORD m_dwSelectedPartySlotByPopup; // �˾� Ȱ��ȭ(��Ŭ��) ��� ���õ� �׷� ����;
	DWORD m_dwSelectedMemberSlotByPopup; // �˾� Ȱ��ȭ(��Ŭ��) ��� ���õ� �׷��� ��� ����;
	DWORD m_dwSelectedGaeaIDByPopup; // �˾� Ȱ��ȭ(��Ŭ��) ��� ���õ� ������ GaeaID;
	SwapSlotState m_SwapSlotState;

	DWORD m_nSecedeID;
	DWORD m_nExpulsionID;
	DWORD m_nReleasePartyID;
	DWORD m_nDelegateID;
	DWORD m_nNominateID;

    bool m_bChangeMasterChatState;

    bool m_bLockedClick;
};


