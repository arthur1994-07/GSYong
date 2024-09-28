#pragma once

#include "../../../InnerInterface.h"
#include "../../Util/UIWindowEx.h"
#include "../../../../RanLogicClient/Party/GLPartyTenderManagerClient.h"


class GLGaeaCLient;
class RnButton;
class CBasicScrollBarEx;
class TenderSlot;
class PartyTenderWindow : public CUIWindowEx
{
	enum
	{
		PARTY_TENDER_WINDOW_TIME, // 입찰 시간;
		PARTY_TENDER_WINDOW_ABANDON, // 입찰 외 포기;
		PARTY_TENDER_WINDOW_TENDER , // 입찰;
	};

	enum
	{
		PARTY_TENDER_WINDOW_ABANDON_BUTTON = ET_CONTROL_NEXT + 1, // 입찰 외 포기 버튼;
		PARTY_TENDER_WINDOW_TENDER_BUTTON, // 입찰 버튼;
		PARTY_TENDER_WINDOW_SCROLL_BAR, // 스크롤 바;
		PARTY_TENDER_WINDOW_ITEM_SLOT_1,
		PARTY_TENDER_WINDOW_ITEM_SLOT_2,
		PARTY_TENDER_WINDOW_ITEM_SLOT_3,
		PARTY_TENDER_WINDOW_ITEM_SLOT_4,
		PARTY_TENDER_WINDOW_ITEM_SLOT_5,
	};
public:
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void SetVisibleSingle(BOOL bVisible) override;

	void CreateSubControl(void);

private:
	void TranslateUIMessage_SelectItemSlot(const DWORD dwSlotIndex);
	void TranslateUIMessage_Tender(void);
	void TranslateUIMessage_Abandon(void);

	void TranslateTenderWarningCallback(const bool bYes = true);
	void RefreshItemList(void);

public:
	PartyTenderWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~PartyTenderWindow(void);

private:
	GLGaeaClient* m_pGaeaClient;

	CBasicScrollBarEx* m_pScrollBar;
	RnButton* m_pAbandonButton;
	RnButton* m_pTenderButton;		
	
	TenderSlot* m_pTenderSlot[PARTY_TENDER_WINDOW_ITEM_SLOT_NSIZE];	
	DWORD m_dwCurrentBeginIndex;
	DWORD m_dwCurrentFilterOption;

	DWORD m_nTenderWarningID;
};


class MyPartyTenderWindow : public IPartyTenderWindow, public PartyTenderWindow
{
public:
	virtual void CreateUIWindowAndRegisterOwnership(void) override;	

public:
	MyPartyTenderWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);	
};