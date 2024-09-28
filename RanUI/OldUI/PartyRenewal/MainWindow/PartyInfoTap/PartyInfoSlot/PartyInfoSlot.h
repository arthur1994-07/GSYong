#pragma once

#include "../../../../../InnerInterface.h"
#include "../../../../Util/UIGroupHelper.h"

class CUIControl;

class PartyInfoSlotPortrait;
class PartyInfoSlotInformation;
struct GLPartySlotClient;

class CInnerInterface;
class GLGaeaClient;
class PartyInfoSlot : public CUIGroupHelper
{
private:
	enum
	{
		PARTY_INFO_SLOT_PORTRAIT = NO_ID + 1,
		PARTY_INFO_SLOT_INFORMATION,
		PARTY_INFO_SLOT_ADD_FRIEND,
		PARTY_INFO_SLOT_SHOW_INFO,
        PARTY_INFO_SLOT_BLOCK_FRIEND,
        PARTY_INFO_SLOT_MASTER,
        PARTY_INFO_SLOT_SUBMASTER,
	};
public:
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;	

	void SetPartyInfoSlot(const GLPartySlotClient* const pPartyClient);
	void SetPartyInfoMasterSlot(const GLPartySlotClient* const pPartyClient, const bool bMaster);
	void SetSelect(void);

	const CString& GetName(void) const;
	const DWORD GetGaeaID(void) const;
	void RESET(void);

public:
	virtual void CreateSubControl(void);		

private:
	void TranslateUIMessage_AddFriend(void);
	void TranslateUIMessage_ShowInfo(void);
	
public:
	PartyInfoSlot(EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface, GLGaeaClient* pGaeaClient, const DWORD _iPartySlot);
	virtual ~PartyInfoSlot(void);

private:
	GLGaeaClient* const m_pGaeaClient;
	CInnerInterface* const m_pInterface;

	CUIControl* m_pMasterCrown[2];
	CUIControl* m_pSelectBox;
	CUIControl* m_pBlockFriend;
	RnButton* m_pAddFriend;
	RnButton* m_pShowInfo;

	PartyInfoSlotPortrait* m_pPartySlotPortrait;	
	PartyInfoSlotInformation* m_pPartySlotInformation;
	
	const DWORD m_dwPartySlot;
};