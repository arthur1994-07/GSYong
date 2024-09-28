#pragma once

#include "../../../InnerInterface.h"
#include "../../Util/UIWindowEx.h"

class RnButton;
class CBasicScrollBarEx;
class TenderSlot;
class PartyDistributionWindow : public CUIWindowEx
{
	enum
	{
		PARTY_DISTRIBUTION_WINDOW_JUNK_BUTTON = ET_CONTROL_NEXT + 1, // 버리기 버튼;		
		PARTY_DISTRIBUTION_WINDOW_SCROLL_BAR, // 스크롤 바;
		PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_1,
		PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_2,
		PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_3,
		PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_4,
		PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_5,
		PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_NSIZE = (PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_5 - PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_1 + 1),
	};

public:
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;

 	void CreateSubControl(void);
 
private:
 	void TranslateUIMessage_SelectItemSlot(const DWORD dwSlotIndex);

	void TranslateJunkCallback(const bool bYes = true); // 아이템 버리기;

 	void RefreshItemList(void);

public:
	PartyDistributionWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~PartyDistributionWindow(void);

private:
	GLGaeaClient* m_pGaeaClient;

	CBasicScrollBarEx* m_pScrollBar;
	RnButton* m_pJunkButton;	

	TenderSlot* m_pDistributionSlot[PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_NSIZE];	
	DWORD m_dwCurrentBeginIndex;	
	DWORD m_dwCurrentSelectedItemID;

	DWORD m_nJunkID; 
};


class MyPartyDistributionWindow : public IPartyDistributionWindow, public PartyDistributionWindow
{
public:
	virtual void CreateUIWindowAndRegisterOwnership(void) override;	

public:
	MyPartyDistributionWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);	
};

