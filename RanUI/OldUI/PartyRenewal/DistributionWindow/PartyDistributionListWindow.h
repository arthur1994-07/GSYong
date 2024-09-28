#pragma once

#include "../../../InnerInterface.h"
#include "../../Util/UIWindowEx.h"

class DistributionMemberSlot;
class CBasicScrollBarEx;
class RnButton;
class PartyDistributionListWindow : public CUIWindowEx
{
	enum
	{
		PARTY_DISTRIBUTION_WINDOW_TRANSFER_BUTTON = ET_CONTROL_NEXT + 1, // 양도 버튼;
		PARTY_DISTRIBUTION_WINDOW_SCROLL_BAR, // 스크롤 바;
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_1,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_2,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_3,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_4,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_5,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_6,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_7,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_8,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_9,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_10,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_11,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_12,
		PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_NSIZE = (PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_12 - PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_1 + 1),
	};

public:
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;

	void CreateSubControl(void);

private:
	void Update_DistributionSlot(const DWORD dwPartyIndex, DWORD& dwCount);
	void TranslateUIMessage_SelectMemberSlot(const DWORD dwSlotIndex);

	void TranslateTransferCallback(const bool bYes = true); // 아이템 양도;

public:
	PartyDistributionListWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~PartyDistributionListWindow(void);

private:
	GLGaeaClient* m_pGaeaClient;

	CBasicScrollBarEx* m_pScrollBar;
	RnButton* m_pTransferButton;

	DistributionMemberSlot* m_pMemberSlot[PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_NSIZE];
	DWORD m_dwCurrentBeginIndex;

	DWORD m_nTransferID;
};

class MyPartyDistributionListWindow : public IPartyDistributionListWindow, public PartyDistributionListWindow
{
public:
	virtual void CreateUIWindowAndRegisterOwnership(void) override;

public:
	MyPartyDistributionListWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);	
};