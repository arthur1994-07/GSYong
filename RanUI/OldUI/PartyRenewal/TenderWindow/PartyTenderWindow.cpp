#include "StdAfx.h"

#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../../RanLogic/Party/GLPartyDefine.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/Party/GLPartyTenderManagerClient.h"
#include "../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../Util/RnButton.h"

#include "./TenderSlot/TenderSlot.h"
#include "PartyTenderWindow.h"

PartyTenderWindow::PartyTenderWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_dwCurrentBeginIndex(0)
, m_dwCurrentFilterOption(0)
{	
}
PartyTenderWindow::~PartyTenderWindow(void)
{
}

void PartyTenderWindow::CreateSubControl(void)
{
	RnButton::CreateArg Arg;
	Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
	Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
	Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";

	{ // 입찰 외 포기 버튼;
		Arg.text = ID2GAMEWORD( "PARTY_DICE_TENDER_WINDOW_TEXT", PARTY_TENDER_WINDOW_ABANDON);
		m_pAbandonButton = new RnButton(m_pEngineDevice);
		m_pAbandonButton->CreateSub(this, "PARTY_TENDER_WINDOW_ABANDON_BUTTON", UI_FLAG_DEFAULT, PARTY_TENDER_WINDOW_ABANDON_BUTTON);
		m_pAbandonButton->CreateSubControl(Arg);
		RegisterControl(m_pAbandonButton);		
	}

	{ // 입찰 버튼;
		Arg.text = ID2GAMEWORD( "PARTY_DICE_TENDER_WINDOW_TEXT", PARTY_TENDER_WINDOW_TENDER);

		m_pTenderButton = new RnButton(m_pEngineDevice);
		m_pTenderButton->CreateSub(this, "PARTY_TENDER_WINDOW_TENDER_BUTTON", UI_FLAG_DEFAULT, PARTY_TENDER_WINDOW_TENDER_BUTTON);
		m_pTenderButton->CreateSubControl(Arg);
		RegisterControl(m_pTenderButton);		
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
		CString strItemSlotInfo[PARTY_TENDER_WINDOW_ITEM_SLOT_NSIZE] = 
		{		
			"PARTY_TENDER_ITEM_SLOT_1", "PARTY_TENDER_ITEM_SLOT_2",
			"PARTY_TENDER_ITEM_SLOT_3", "PARTY_TENDER_ITEM_SLOT_4", "PARTY_TENDER_ITEM_SLOT_5",
		};

		for ( DWORD _i = 0; _i < PARTY_TENDER_WINDOW_ITEM_SLOT_NSIZE; ++_i )
		{
			m_pTenderSlot[_i] = new TenderSlot(m_pGaeaClient, m_pInterface, m_pEngineDevice);
			m_pTenderSlot[_i]->CreateSub(this, strItemSlotInfo[_i].GetString(), UI_FLAG_DEFAULT, PARTY_TENDER_WINDOW_ITEM_SLOT_1 + _i );
			m_pTenderSlot[_i]->CreateSubControl();
			RegisterControl(m_pTenderSlot[_i]);
			m_pTenderSlot[_i]->SetVisibleSingle(FALSE);
		}
	}

	m_nTenderWarningID = m_pInterface->AddPartyYesNoComponent(std::string(ID2GAMEINTEXT("PARTY_TENDER_ITEM_WARNING_BTN")), boost::BOOST_BIND(&PartyTenderWindow::TranslateTenderWarningCallback, this, _1));
}

void PartyTenderWindow::RefreshItemList(void)
{		
	GLPartyTenderManagerClient* const pTenderItemManager = m_pGaeaClient->GetPartyTenderManager();
	CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();

	const int nItemSize = int(pTenderItemManager->GetNItem());
	pThumbFrame->SetState(nItemSize, PARTY_TENDER_WINDOW_ITEM_SLOT_NSIZE);

	const int nMovableLine = nItemSize - PARTY_TENDER_WINDOW_ITEM_SLOT_NSIZE;
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

	pTenderItemManager->SetBeginIndex(m_dwCurrentBeginIndex);
		
	const bool bNeedRefresh = (dwPrevBegineIndex != m_dwCurrentBeginIndex);	
	if ( (pTenderItemManager->isNeedRefresh() == false) && (bNeedRefresh == false) )
	{
		DWORD dwIndex = 0;
		for ( DWORD _i = 0; _i < PARTY_TENDER_WINDOW_ITEM_SLOT_NSIZE; ++_i )
		{			
			const GLTenderItemCustomClient* const pItemCustom = pTenderItemManager->GetTenderItemCustom(_i + m_dwCurrentBeginIndex);
			if ( pItemCustom == NULL )
				continue;

			m_pTenderSlot[dwIndex]->SetProgress(pItemCustom->m_fPercent);
			++dwIndex;
		}
	}
	else
	{
		DWORD dwIndex = 0;
		for ( DWORD _i = 0; _i < PARTY_TENDER_WINDOW_ITEM_SLOT_NSIZE; ++_i )
		{
			m_pTenderSlot[_i]->SetVisibleSingle(FALSE);
			GLTenderItemCustomClient* const pItemCustom = pTenderItemManager->GetTenderItemCustom(_i + m_dwCurrentBeginIndex);
			if ( pItemCustom == NULL )			
				continue;

			m_pTenderSlot[dwIndex]->SetVisibleSingle(TRUE);
			m_pTenderSlot[dwIndex]->SetSlotInfo(pItemCustom);
			m_pTenderSlot[dwIndex]->SetProgress(pItemCustom->m_fPercent);
			if ( pItemCustom->m_bSelected == true )
				m_pTenderSlot[dwIndex]->SetSelect(true);
			++dwIndex;
		}
		if ( dwIndex == 0 )
			SetVisibleSingle(FALSE);
		pTenderItemManager->DoneRefresh();	
	}	

	pTenderItemManager->RefreshTenderItemIsNew();
}

void PartyTenderWindow::TranslateUIMessage_SelectItemSlot(const DWORD dwSlotIndex)
{
	if ( m_pTenderSlot[dwSlotIndex]->IsVisible() == FALSE )
		return;

	GLPartyTenderManagerClient* const pTenderItemManager = m_pGaeaClient->GetPartyTenderManager();
	const bool bSelected = pTenderItemManager->SetSelect(dwSlotIndex + m_dwCurrentBeginIndex);
	m_pTenderSlot[dwSlotIndex]->SetSelect(bSelected);
}

void PartyTenderWindow::TranslateUIMessage_Tender(void)
{
	GLPartyTenderManagerClient* const pTenderItemManager = m_pGaeaClient->GetPartyTenderManager();
	pTenderItemManager->TenderItem();
}

void PartyTenderWindow::TranslateUIMessage_Abandon(void)
{
	GLPartyTenderManagerClient* const pTenderItemManager = m_pGaeaClient->GetPartyTenderManager();
	pTenderItemManager->AbandonItem();
}

void PartyTenderWindow::TranslateTenderWarningCallback(const bool bYes)
{
	if ( bYes == false )
		return;

	GLPartyTenderManagerClient* const pTenderItemManager = m_pGaeaClient->GetPartyTenderManager();
	pTenderItemManager->TenderItem();
}

void PartyTenderWindow::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	switch ( ControlID )
	{	
	case PARTY_TENDER_WINDOW_ITEM_SLOT_1:
	case PARTY_TENDER_WINDOW_ITEM_SLOT_2:
	case PARTY_TENDER_WINDOW_ITEM_SLOT_3:
	case PARTY_TENDER_WINDOW_ITEM_SLOT_4:
	case PARTY_TENDER_WINDOW_ITEM_SLOT_5:
		{
			const DWORD dwSlotIndex = DWORD(ControlID - PARTY_TENDER_WINDOW_ITEM_SLOT_1);
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
				TranslateUIMessage_SelectItemSlot(dwSlotIndex);
		}
		break;
	case PARTY_TENDER_WINDOW_TENDER_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			GLPartyTenderManagerClient* const pTenderItemManager = m_pGaeaClient->GetPartyTenderManager();
			if ( pTenderItemManager->isExistNoneAvailableItem() == true )
				m_pInterface->ToggleWindowYesNoParty(m_nTenderWarningID);				
			else
				TranslateUIMessage_Tender();
		}
		break;
	case PARTY_TENDER_WINDOW_ABANDON_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			TranslateUIMessage_Abandon();
		break;
	}
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void PartyTenderWindow::SetVisibleSingle(BOOL bVisible)
{
	CUIWindowEx::SetVisibleSingle(bVisible);
	if ( bVisible == TRUE )
	{
		CString strTitleName;
		strTitleName.Format("%s(%s)", 
			ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", EMPARTY_ITEM_DICE ),
			ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", m_pGaeaClient->GetPartyManager()->GetPartyOption().GetItemFilter()));
		SetTitleName(strTitleName);	
	}
}

void PartyTenderWindow::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);	
	RefreshItemList();		
}


MyPartyTenderWindow::MyPartyTenderWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: PartyTenderWindow(pGaeaClient, pInterface, pEngineDevice)
{
}
void MyPartyTenderWindow::CreateUIWindowAndRegisterOwnership(void)
{
	Create(PARTY_TENDER_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	CreateBaseWindowLightGray( "PARTY_DICE_TENDER_WINDOW", (char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", EMPARTY_ITEM_DICE) );	
	CreateLineBox("PARTY_DICE_TENDER_WINDOW_REGION", "PARTY_WINDOW_RENEWAL_TEXTURE_INFO" );
	CreateLineBox("PARTY_DICE_TENDER_WINDOW_SLOT_REGION", "PARTY_INFO_LINEBOX_TEXTUREINFO");
	CreateSubControl();
	SetAlignFlag(UI_FLAG_CENTER_Y | UI_FLAG_RIGHT);
	m_pInterface->UiRegisterControl(this);
	m_pInterface->UiShowGroupFocus(PARTY_TENDER_WINDOW);
}
