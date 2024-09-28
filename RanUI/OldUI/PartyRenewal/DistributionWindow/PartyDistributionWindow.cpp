#include "StdAfx.h"

#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../../RanLogic/Party/GLPartyDefine.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/Party/GLPartyTenderManagerClient.h"
#include "../../Util/RnButton.h"

#include "../TenderWindow/TenderSlot/TenderSlot.h"
#include "PartyDistributionWindow.h"

PartyDistributionWindow::PartyDistributionWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_dwCurrentBeginIndex(0)
, m_dwCurrentSelectedItemID(GAEAID_NULL)
{
}

PartyDistributionWindow::~PartyDistributionWindow(void)
{
}

void PartyDistributionWindow::CreateSubControl(void)
{
	RnButton::CreateArg Arg;
	Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
	Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
	Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";

	{ // 버리기 버튼;
		Arg.text = ID2GAMEWORD( "PARTY_DISTRIBUTION_WINDOW_TEXT", 0);
		m_pJunkButton = new RnButton(m_pEngineDevice);
		m_pJunkButton->CreateSub(this, "PARTY_DISTRIBUTION_WINDOW_JUNK_BUTTON", UI_FLAG_DEFAULT, PARTY_DISTRIBUTION_WINDOW_JUNK_BUTTON);
		m_pJunkButton->CreateSubControl(Arg);
		RegisterControl(m_pJunkButton);		
	}

	{ // 스크롤 바;
		m_pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		m_pScrollBar->CreateSub(this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE);
		m_pScrollBar->CreateBaseScrollBar("PARTY_TENDER_WINDOW_SCROLLBAR");
		m_pScrollBar->GetThumbFrame()->SetScrollParent(this);
		m_pScrollBar->GetThumbFrame()->SetScrollMoveLine(1);
		RegisterControl(m_pScrollBar);
		m_pScrollBar->SetVisibleSingle(FALSE);
	}	

	{ // 아이템 슬롯;
		CString strItemSlotInfo[PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_NSIZE] = 
		{		
			"PARTY_TENDER_ITEM_SLOT_1", "PARTY_TENDER_ITEM_SLOT_2",
			"PARTY_TENDER_ITEM_SLOT_3", "PARTY_TENDER_ITEM_SLOT_4", "PARTY_TENDER_ITEM_SLOT_5",
		};

		for ( DWORD _i = 0; _i < PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_NSIZE; ++_i )
		{
			m_pDistributionSlot[_i] = new TenderSlot(m_pGaeaClient, m_pInterface, m_pEngineDevice);
			m_pDistributionSlot[_i]->CreateSub(this, strItemSlotInfo[_i].GetString(), UI_FLAG_DEFAULT, PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_1 + _i );
			m_pDistributionSlot[_i]->CreateSubControl();
			RegisterControl(m_pDistributionSlot[_i]);
			m_pDistributionSlot[_i]->SetVisibleSingle(FALSE);
		}
	}

	m_nJunkID = m_pInterface->AddPartyYesNoComponent(
		std::string(ID2GAMEINTEXT("PARTY_DISTRIBUTION_ITEM_JUNK_BTN")), 
		boost::bind(&PartyDistributionWindow::TranslateJunkCallback, this, _1));	
}

void PartyDistributionWindow::RefreshItemList(void)
{		
	GLPartyTenderManagerClient* const pTenderItemManager = m_pGaeaClient->GetPartyTenderManager();
	CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();

	const int nItemSize = int(pTenderItemManager->GetNItem());
	pThumbFrame->SetState(nItemSize, PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_NSIZE);

	const int nMovableLine = nItemSize - PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_NSIZE;
	const float fPercent = pThumbFrame->GetPercent();
	const DWORD dwPrevBegineIndex = m_dwCurrentBeginIndex;
	if ( nMovableLine > 0 )
	{
		m_dwCurrentBeginIndex =  (DWORD)floor(fPercent * float(nMovableLine));
		m_pScrollBar->SetVisibleSingle(TRUE);
	}
	else
	{
		m_dwCurrentBeginIndex = 0;
		m_pScrollBar->SetVisibleSingle(FALSE);
	}	
	
	const bool bNeedRefresh = (dwPrevBegineIndex != m_dwCurrentBeginIndex);	
	if ( (pTenderItemManager->isNeedRefresh() == false) && (bNeedRefresh == false) )
	{
		DWORD dwIndex = 0;
		for ( DWORD _i = 0; _i < PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_NSIZE; ++_i )
		{			
			const GLTenderItemCustomClient* const pItemCustom = pTenderItemManager->GetTenderItemCustom(_i + m_dwCurrentBeginIndex);
			if ( pItemCustom == NULL )
				continue;

			m_pDistributionSlot[dwIndex]->SetProgress(pItemCustom->m_fPercent);
			++dwIndex;
		}
	}
	else
	{
		DWORD dwIndex = 0;
		for ( DWORD _i = 0; _i < PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_NSIZE; ++_i )
		{
			m_pDistributionSlot[_i]->SetVisibleSingle(FALSE);
			GLTenderItemCustomClient* const pItemCustom = pTenderItemManager->GetTenderItemCustom(_i + m_dwCurrentBeginIndex);
			if ( pItemCustom == NULL )			
				continue;

			m_pDistributionSlot[dwIndex]->SetVisibleSingle(TRUE);
			m_pDistributionSlot[dwIndex]->SetSlotInfo(pItemCustom);
			m_pDistributionSlot[dwIndex]->SetProgress(pItemCustom->m_fPercent);		
			m_pDistributionSlot[dwIndex]->SetSelect(pItemCustom->m_bSelected);

			++dwIndex;
		}
		if ( dwIndex == 0 )
			SetVisibleSingle(FALSE);
		pTenderItemManager->DoneRefresh();	
	}

	pTenderItemManager->RefreshTenderItemIsNew();
}

void PartyDistributionWindow::TranslateUIMessage_SelectItemSlot(const DWORD dwSlotIndex)
{
	if ( m_pDistributionSlot[dwSlotIndex]->IsVisible() == FALSE )
		return;

	GLPartyTenderManagerClient* const pTenderManager = m_pGaeaClient->GetPartyTenderManager();
	if ( m_dwCurrentSelectedItemID != GAEAID_NULL )
	{		
		pTenderManager->ResetSelectByItemID(m_dwCurrentSelectedItemID);
		m_dwCurrentSelectedItemID = GAEAID_NULL;
		if ( m_pDistributionSlot[dwSlotIndex]->isSelected() == true )
			return;
	}

	const DWORD nItemIndex = dwSlotIndex + m_dwCurrentBeginIndex;
	const bool bSelected = pTenderManager->SetSelect(nItemIndex);
	m_dwCurrentSelectedItemID = bSelected ? pTenderManager->GetItemID(nItemIndex) : GAEAID_NULL;
	
}

void PartyDistributionWindow::TranslateJunkCallback(const bool bYes /* = true */)
{
	if ( bYes == false )
		return;

	GLPartyTenderManagerClient* const pTenderManager = m_pGaeaClient->GetPartyTenderManager();
	pTenderManager->JunkItem();
}

void PartyDistributionWindow::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	switch ( ControlID )
	{
	case PARTY_DISTRIBUTION_WINDOW_JUNK_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			m_pInterface->ToggleWindowYesNoParty(m_nJunkID);
		break;
	case PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_1:
	case PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_2:
	case PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_3:
	case PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_4:
	case PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_5:
		{
			const DWORD dwSlotIndex = DWORD(ControlID - PARTY_DISTRIBUTION_WINDOW_ITEM_SLOT_1);
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
				TranslateUIMessage_SelectItemSlot(dwSlotIndex);
		}
		break;
	}
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}
void PartyDistributionWindow::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);	
	RefreshItemList();
}


MyPartyDistributionWindow::MyPartyDistributionWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: PartyDistributionWindow(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyPartyDistributionWindow::CreateUIWindowAndRegisterOwnership(void)
{
	Create(PARTY_DISTRIBUTION_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	CreateBaseWindowLightGray( "PARTY_DICE_TENDER_WINDOW", (char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", EMPARTY_ITEM_LEADER_SHARE) );		
	SetControlNameEx((char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", EMPARTY_ITEM_DICE ));
	CreateLineBox("PARTY_DICE_TENDER_WINDOW_REGION", "PARTY_WINDOW_RENEWAL_TEXTURE_INFO" );
	CreateLineBox("PARTY_DICE_TENDER_WINDOW_SLOT_REGION", "PARTY_INFO_LINEBOX_TEXTUREINFO");
	CreateSubControl();
    SetAlignFlag( UI_FLAG_RIGHT | UI_FLAG_TOP );
	m_pInterface->UiRegisterControl(this);
	m_pInterface->UiShowGroupFocus(PARTY_DISTRIBUTION_WINDOW);
}

