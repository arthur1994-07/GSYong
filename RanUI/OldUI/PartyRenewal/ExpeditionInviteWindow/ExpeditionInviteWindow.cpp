#include "StdAfx.h"

#include "../../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../RanLogic/Friend/FriendDefine.h"
#include "../../../../RanLogic/Club/GLClubDefine.h"
#include "../../../../RanLogicClient/Friend/GLFriendClient.h"
#include "../../../../RanLogicClient/Club/GLClubClient.h"
#include "../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../InnerInterface.h"

#include "./ExpeditionInviteSlot/ExpeditionInviteSlot.h"
#include "ExpeditionInviteWindow.h"

// ----------------------------------------------------------------------------
#include "../../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SListSlot::SListSlot(club::GLCLUBMEMBER_CLIENT* pClubMember)
: m_bSelected(false)
{
	m_dwDbNum = pClubMember->GetChaDbNum();
	StringCchCopy(m_szName, CHAR_SZNAME, pClubMember->GetName());
}
SListSlot::SListSlot(SFRIEND* pFriend)
: m_bSelected(false)
{
	m_dwDbNum = pFriend->GetChaDbNum();
	StringCchCopy(m_szName, CHAR_SZNAME, pFriend->GetName().c_str());
}


ExpeditionInviteWindow::ExpeditionInviteWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_nListMember(0)
, m_dwCurrentBeginIndex(-1)
{
}

ExpeditionInviteWindow::~ExpeditionInviteWindow(void)
{
}

void ExpeditionInviteWindow::CreateSubControl(void)
{
	CreateLineBox("EXPEDITION_INVITE_WINDOW_REGION", "PARTY_WINDOW_RENEWAL_TEXTURE_INFO");
	CreateLineBox("EXPEDITION_INVITE_LIST_REGION", "PARTY_INFO_LINEBOX_TEXTUREINFO");

	{ // 탭 버튼 및 탭 윈도우 설정;
		CTapSelectorEx::BUTTON* pFriendTap = new CTapSelectorEx::BUTTON(m_pEngineDevice);
		pFriendTap->CreateSub( this, "FRIEND_LIST_TAP", UI_FLAG_DEFAULT, FRIEND_LIST_TAP);
		pFriendTap->CreateSubControl( (char*)ID2GAMEWORD("EXPEDITION_INVITE_TAP_NAME_STATIC", 0),
			"EXPEDITION_INVITE_LIST_TAP_BASE","PARTY_TAP_ACTIVE",
			"EXPEDITION_INVITE_LIST_TAP_BASE","PARTY_TAP_INACTIVE",
			"EXPEDITION_INVITE_LIST_TAP_BASE");

		RegisterControl(pFriendTap);
		RegisterTapButton(pFriendTap);

		CTapSelectorEx::BUTTON* pClubTap = new CTapSelectorEx::BUTTON(m_pEngineDevice);
		pClubTap->CreateSub( this, "CLUB_LIST_TAP", UI_FLAG_DEFAULT, CLUB_LIST_TAP);
		pClubTap->CreateSubControl( (char*)ID2GAMEWORD("EXPEDITION_INVITE_TAP_NAME_STATIC", 1),
			"EXPEDITION_INVITE_LIST_TAP_BASE","PARTY_TAP_ACTIVE",
			"EXPEDITION_INVITE_LIST_TAP_BASE","PARTY_TAP_INACTIVE",
			"EXPEDITION_INVITE_LIST_TAP_BASE");

		RegisterControl(pClubTap);
		RegisterTapButton(pClubTap);
	}

	{ // 스크롤 바;
		m_pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		m_pScrollBar->CreateSub(this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE);
		m_pScrollBar->CreateBaseScrollBar("EXPEDITION_INVITE_WINDOW_SCROLLBAR");
		m_pScrollBar->GetThumbFrame()->SetScrollParent(this);
		m_pScrollBar->GetThumbFrame()->SetScrollMoveLine(1);
		RegisterControl(m_pScrollBar);		
	}	

	{
		for ( DWORD _i = 0; _i < INVITE_LIST_SLOT_NSIZE; ++_i )
		{
			std::string strSlotList = sc::string::format("EXPEDITION_INVITE_LIST_%1%", _i);
			ExpeditionInviteSlot* pListSlot = new ExpeditionInviteSlot(m_pEngineDevice, m_pGaeaClient);
			pListSlot->CreateSub(this, strSlotList.c_str(), UI_FLAG_DEFAULT, INVITE_LIST_SLOT0 + _i);
			pListSlot->CreateSubControl();
			RegisterControl(pListSlot);
			m_pListSlot[_i] = pListSlot;
		}

	}

	{ // 초대 및 닫기 버튼 설정;
		RnButton::CreateArg Arg;
		Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
		Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
		Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";
		{ // 초대하기 버튼;
			Arg.text = ID2GAMEWORD( "EXPEDITION_INVITE_TAP_NAME_STATIC", 2);

			m_pInviteButton = new RnButton(m_pEngineDevice);
			m_pInviteButton->CreateSub(this,"EXPEDITION_INVITE_LIST_INVITE_BUTTON", UI_FLAG_DEFAULT, INVITE_BUTTON);
			m_pInviteButton->CreateSubControl(Arg);
			RegisterControl(m_pInviteButton);
		}

		{ // 닫기 버튼;		
			Arg.text = ID2GAMEWORD( "EXPEDITION_INVITE_TAP_NAME_STATIC", 3);

			m_pCloseButton = new RnButton(m_pEngineDevice);
			m_pCloseButton->CreateSub(this,"EXPEDITION_INVITE_LIST_CLOSE_BUTTON", UI_FLAG_DEFAULT, CLOSE_BUTTON);
			m_pCloseButton->CreateSubControl(Arg);
			RegisterControl(m_pCloseButton);
		}
	}
}

void ExpeditionInviteWindow::TranslateUIMessage_Invite(void)
{
	GLPartyManagerClient* pPartyManager = m_pGaeaClient->GetPartyManager();
	if ( (pPartyManager->isExpeditionMaster() == false) && (pPartyManager->isExpeditionSubMaster() == false) )
		return;

	const DWORD nMember = m_InviteListVector.size();
	for ( DWORD _i = nMember; _i; --_i )
	{
		SListSlot& ListSlot = m_InviteListVector[_i - 1];
		if ( ListSlot.m_bSelected == false )
			continue;

		pPartyManager->LureByDbNum(ListSlot.m_dwDbNum);
	}
}

void ExpeditionInviteWindow::TranslateUIMessage_Close(void)
{
	SetVisibleSingle(FALSE);	
}

void ExpeditionInviteWindow::TranslateUIMessage_SelectSlot(const DWORD nIndex)
{
	const DWORD nVectorIndex = m_dwCurrentBeginIndex + nIndex;
	if ( !(nVectorIndex < m_InviteListVector.size()) )
		return;

	SListSlot& ListSlot = m_InviteListVector[nVectorIndex];
	ListSlot.m_bSelected = !ListSlot.m_bSelected;
	m_pListSlot[nIndex]->Refresh(ListSlot.m_bSelected);
}

void ExpeditionInviteWindow::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case INVITE_LIST_SLOT0:
	case INVITE_LIST_SLOT1:
	case INVITE_LIST_SLOT2:
	case INVITE_LIST_SLOT3:
	case INVITE_LIST_SLOT4:
	case INVITE_LIST_SLOT5:
	case INVITE_LIST_SLOT6:
	case INVITE_LIST_SLOT7:
	case INVITE_LIST_SLOT8:
	case INVITE_LIST_SLOT9:
	case INVITE_LIST_SLOT10:
	case INVITE_LIST_SLOT11:
	case INVITE_LIST_SLOT12:
	case INVITE_LIST_SLOT13:
	case INVITE_LIST_SLOT14:
	case INVITE_LIST_SLOT15:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			TranslateUIMessage_SelectSlot(ControlID - INVITE_LIST_SLOT0);
		break;
	case INVITE_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			TranslateUIMessage_Invite();
		break;
	case CLOSE_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			TranslateUIMessage_Close();
		break;
	}
}

void ExpeditionInviteWindow::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();
	pThumbFrame->SetState(m_nListMember, INVITE_LIST_SLOT_NSIZE);

	const int nMovableLine = m_nListMember - INVITE_LIST_SLOT_NSIZE;
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
	if ( bNeedRefresh == false )
		return;

	const DWORD nLimit = m_nListMember > INVITE_LIST_SLOT_NSIZE ? INVITE_LIST_SLOT_NSIZE : m_nListMember;
	for ( DWORD _i = 0; _i < nLimit; ++_i )
	{
		SListSlot& ListSlot = m_InviteListVector[m_dwCurrentBeginIndex + _i];
		m_pListSlot[_i]->SetSlot(ListSlot.m_szName, ListSlot.m_dwDbNum, ListSlot.m_bSelected);
		m_pListSlot[_i]->SetVisibleSingle(TRUE);
	}

	for ( DWORD _i = nLimit; _i < INVITE_LIST_SLOT_NSIZE; ++_i )
		m_pListSlot[_i]->SetVisibleSingle(FALSE);
}

void ExpeditionInviteWindow::SetVisibleSingle(BOOL bVisible)
{
	GLPartyManagerClient* pPartyManager = m_pGaeaClient->GetPartyManager();
	if ( (pPartyManager->isExpeditionMaster() == false) && (pPartyManager->isExpeditionSubMaster() == false) )
	{
		CUIWindowEx::SetVisibleSingle(FALSE);
		return;
	}

	CUIWindowEx::SetVisibleSingle(bVisible);
	if ( bVisible == FALSE )
		return;

	EventSelectedTap(m_emListMode == EMLIST_MODE_FRIEND ? FRIEND_LIST_TAP : CLUB_LIST_TAP);
}

void ExpeditionInviteWindow::EventSelectedTap(UIGUID controlID)
{
	m_dwCurrentBeginIndex = -1;
	switch ( controlID )
	{
	case FRIEND_LIST_TAP:
		m_emListMode = EMLIST_MODE_FRIEND;
		RefreshFriendList();
		break;
	case CLUB_LIST_TAP:
		m_emListMode = EMLIST_MODE_CLUB;
		RefreshClubList();
		break;
	}	
}

void ExpeditionInviteWindow::RefreshFriendList(void)
{
	m_InviteListVector.clear();
	const friendlogic::FRIENDMAP& FriendMap = m_pGaeaClient->GetFriendClient()->GetList();	
	DWORD nFriend = 0;
	BOOST_FOREACH( const friendlogic::FRIENDMAP_VALUE& iter, FriendMap )
	{
		SFRIEND* pFriend = iter.second.get();
		if ( (pFriend->IsBLOCK() == true) || (pFriend->m_Online == false) )
			continue;

		SListSlot ListSlot(pFriend);
		m_InviteListVector.push_back(ListSlot);
		++nFriend;
	}
	m_nListMember = nFriend;
}

void ExpeditionInviteWindow::RefreshClubList(void)
{
	m_InviteListVector.clear();

	std::vector<club::GLCLUBMEMBER_CLIENT> ClubMembers;
	std::vector<club::ALIGN_ORDER> MemberAlign;
	m_pGaeaClient->GetMyClub()->GetList(ClubMembers, MemberAlign, true);
	m_pGaeaClient->GetMyClub()->GetList(ClubMembers, MemberAlign, false);

	const DWORD dwMyDbNum = m_pGaeaClient->GetCharacter()->GetCharID();

	DWORD nClubMember = 0;
	for ( DWORD _i = 0; _i < ClubMembers.size(); ++_i )
	{
		club::GLCLUBMEMBER_CLIENT& ClubMember = ClubMembers[_i];
		if ( ClubMember.m_Online == false )
			continue;

		if ( ClubMember.GetChaDbNum() == dwMyDbNum )
			continue;


		SListSlot ListSlot(&ClubMember);
		m_InviteListVector.push_back(ListSlot);
		++nClubMember;
	}

	m_nListMember = nClubMember;
}


MyExpeditionInviteWindow::MyExpeditionInviteWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: ExpeditionInviteWindow(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyExpeditionInviteWindow::CreateUIWindowAndRegisterOwnership(void)
{
	Create(EXPEDITION_INVITE_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	CreateBaseWindowLightGray("EXPEDITION_INVITE_WINDOW", (char*)ID2GAMEWORD("EXPEDITION_INVITE_NAME_STATIC"));
	CreateSubControl();
	SetAlignFlag(UI_FLAG_CENTER_Y | UI_FLAG_RIGHT);
	m_pInterface->UiRegisterControl(this, true);
	m_pInterface->UiShowGroupFocus(EXPEDITION_INVITE_WINDOW);
}