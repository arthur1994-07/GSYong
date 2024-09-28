#include "StdAfx.h"

#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../../RanLogic/GLogicDataMan.h"
#include "../../../../RanLogic/Party/GLPartyDefine.h"

#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/Party/GLPartyTenderManagerClient.h"
#include "../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../Util/RnButton.h"

#include "./DistributionMemberSlot/DistributionMemberSlot.h"
#include "PartyDistributionListWindow.h"


PartyDistributionListWindow::PartyDistributionListWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_dwCurrentBeginIndex(0)
{
}
PartyDistributionListWindow::~PartyDistributionListWindow(void)
{
}

void PartyDistributionListWindow::CreateSubControl(void)
{
	CString strMemberSlot[PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_NSIZE] = 
	{
		"PARTY_DISTRIBUTION_MEMBER_SLOT0",
		"PARTY_DISTRIBUTION_MEMBER_SLOT1",
		"PARTY_DISTRIBUTION_MEMBER_SLOT2",
		"PARTY_DISTRIBUTION_MEMBER_SLOT3",
		"PARTY_DISTRIBUTION_MEMBER_SLOT4",
		"PARTY_DISTRIBUTION_MEMBER_SLOT5",
		"PARTY_DISTRIBUTION_MEMBER_SLOT6",
		"PARTY_DISTRIBUTION_MEMBER_SLOT7",
		"PARTY_DISTRIBUTION_MEMBER_SLOT8",
		"PARTY_DISTRIBUTION_MEMBER_SLOT9",
		"PARTY_DISTRIBUTION_MEMBER_SLOT10",
		"PARTY_DISTRIBUTION_MEMBER_SLOT11",
	};
	for ( unsigned int _i = 0; _i < PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_NSIZE; ++_i )
	{
		DistributionMemberSlot* pMemberSlot = new DistributionMemberSlot(m_pEngineDevice);
		pMemberSlot->CreateSub( this, strMemberSlot[_i].GetString (), UI_FLAG_DEFAULT, PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_1 + _i );
		pMemberSlot->CreateSubControl();		
		RegisterControl( pMemberSlot );
		m_pMemberSlot[_i] = pMemberSlot;
	}

	RnButton::CreateArg Arg;
	Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
	Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
	Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";

	{ // 양도 버튼;
		Arg.text = ID2GAMEWORD( "PARTY_DISTRIBUTION_WINDOW_TEXT", 1);
		m_pTransferButton = new RnButton(m_pEngineDevice);
		m_pTransferButton->CreateSub(this, "PARTY_DISTRIBUTION_WINDOW_TRANSFER_BUTTON", UI_FLAG_DEFAULT, PARTY_DISTRIBUTION_WINDOW_TRANSFER_BUTTON);
		m_pTransferButton->CreateSubControl(Arg);
		RegisterControl(m_pTransferButton);		
	}

	m_nTransferID = m_pInterface->AddPartyYesNoComponent(
		std::string(ID2GAMEINTEXT("PARTY_DISTRIBUTION_ITEM_TRANSFER_BTN")), 
		boost::bind(&PartyDistributionListWindow::TranslateTransferCallback, this, _1));	
}

void PartyDistributionListWindow::Update_DistributionSlot(const DWORD dwPartyIndex, DWORD& dwCount)
{
	const GLPartyTenderManagerClient* const pManager = m_pGaeaClient->GetPartyTenderManager();
	const GLTenderItemCustomClient* const pCustomItem = pManager->GetSelectedItemCustom();	
	const SITEM* const pItemData = pCustomItem == NULL ? NULL : GLogicData::GetInstance().GetItem(pCustomItem->m_ItemCustom.GetNativeID());

	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	GLPartyClient* const pParty = pPartyManager->GetParty(dwPartyIndex);	

	GLPartySlotClient* pMaster = pParty->GetMember(MASTER_PARTY);
	if ( (pMaster == NULL) || (pMaster->isValid() == false) )
		return;
	
	const DWORD nSlotIndex = dwCount - m_dwCurrentBeginIndex;
	if ( nSlotIndex < PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_NSIZE )
		dwCount += (m_pMemberSlot[nSlotIndex]->SetSlot(pMaster, pItemData, pMaster->m_sMapID.getBaseMapID()) ? 1 : 0);

	for ( DWORD _i = 1; _i < MAXPARTY; ++_i )
	{
		GLPartySlotClient* pMember = pParty->GetMember(_i);
		if ( (pMember == NULL) || (pMember->isValid() == false) )
			continue;

		const DWORD nSlotIndex = dwCount - m_dwCurrentBeginIndex;
		if ( nSlotIndex < PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_NSIZE )
            dwCount += (m_pMemberSlot[nSlotIndex]->SetSlot(pMember, pItemData, pMaster->m_sMapID.getBaseMapID()) ? 1 : 0);
	}		
}

void PartyDistributionListWindow::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);	
	GLPartyManagerClient* pPartyManager = m_pGaeaClient->GetPartyManager();
	if ( pPartyManager->isParty() == false )
	{
		m_dwCurrentBeginIndex = 0;
		return;
	}

	for ( DWORD _i = PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_NSIZE; _i; --_i )
		m_pMemberSlot[_i - 1]->RESET();
	
	const bool bExpedition = pPartyManager->isExpedition();
	const bool bMaster = bExpedition ? pPartyManager->isExpeditionMaster() : pPartyManager->isPartyMaster();	

	if ( bMaster == false )
	{
		m_dwCurrentBeginIndex = 0;
		return;
	}
	
	DWORD dwCount = 0;
	const DWORD nLoopcount = bExpedition ? MAX_PARTYGROUP : 1;		
	for ( DWORD _i = 0; _i < nLoopcount; ++_i )
		Update_DistributionSlot(_i, dwCount);
}

void PartyDistributionListWindow::TranslateUIMessage_SelectMemberSlot(const DWORD dwSlotIndex)
{
	m_pMemberSlot[dwSlotIndex]->SetSelect();
}

void PartyDistributionListWindow::TranslateTransferCallback(const bool bYes /* = true */)
{
	if ( bYes == false )
		return;

	GLPartyTenderManagerClient* const pTenderManager = m_pGaeaClient->GetPartyTenderManager();
	pTenderManager->TransferItem();
}

void PartyDistributionListWindow::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	switch ( ControlID )
	{
	case PARTY_DISTRIBUTION_WINDOW_TRANSFER_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			m_pInterface->ToggleWindowYesNoParty(m_nTransferID);
		break;
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_1:
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_2:
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_3:
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_4:
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_5:
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_6:
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_7:
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_8:
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_9:
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_10:
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_11:
	case PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_12:
		const DWORD dwSlotIndex = DWORD(ControlID - PARTY_DISTRIBUTION_WINDOW_MEMBER_SLOT_1);
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			TranslateUIMessage_SelectMemberSlot(dwSlotIndex);
		break;
	}
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}


MyPartyDistributionListWindow::MyPartyDistributionListWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: PartyDistributionListWindow(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyPartyDistributionListWindow::CreateUIWindowAndRegisterOwnership(void)
{
	Create(PARTY_DISTRIBUTION_LIST_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	CreateBaseWindowLightGray( "PARTY_DISTRIBUTION_LIST_WINDOW", ID2GAMEWORD("PARTY_DISTRIBUTION_WINDOW_TEXT", 2) );	
	CreateLineBox("PARTY_DISTRIBUTION_LIST_WINDOW_REGION", "PARTY_WINDOW_RENEWAL_TEXTURE_INFO" );
	CreateLineBox("PARTY_DISTRIBUTION_LIST_WINDOW_SLOT_REGION", "PARTY_INFO_LINEBOX_TEXTUREINFO");
	CreateSubControl();
    SetAlignFlag( UI_FLAG_RIGHT | UI_FLAG_TOP );
	m_pInterface->UiRegisterControl(this);
	m_pInterface->UiShowGroupFocus(PARTY_DISTRIBUTION_LIST_WINDOW);
}

