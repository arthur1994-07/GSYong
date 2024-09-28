#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../InnerInterface.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogicClient/Char/GLCharacter.h"
#include "../../RanLogicClient/Char/GLCharClient.h"
#include "../Util/UIWindowEx.h"

#include "./CSetitemPreviewlistSlot.h"
#include "./CSetitemPreviewWindow.h"

struct ___sortSuit_Setitem {
	bool operator()(SNATIVEID& sNID00, SNATIVEID& sNID01)
	{
		SITEM* pItem00 = GLItemMan::GetInstance().GetItem(sNID00)
			,* pItem01 = GLItemMan::GetInstance().GetItem(sNID01);
		if ( !pItem00 || !pItem01 )
			 return false;

		return (pItem00->Suit() < pItem01->Suit());
	}
};

struct ___sortAttackT_Setitem {
	bool operator()(SNATIVEID& sNID00, SNATIVEID& sNID01)
	{
		SITEM* pItem00 = GLItemMan::GetInstance().GetItem(sNID00)
			,* pItem01 = GLItemMan::GetInstance().GetItem(sNID01);
		if ( !pItem00 ||
			!pItem01 )
			return false;

		return (pItem00->sSuitOp.emAttack < pItem01->sSuitOp.emAttack);
	}
};

CSetitemPreviewWindow::CSetitemPreviewWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pSetitemListBox(NULL)
, m_pListScrollBar(NULL)
, m_pSetitem(NULL)
{
}

CSetitemPreviewWindow::~CSetitemPreviewWindow(void)
{
}

void CSetitemPreviewWindow::CreateUIWindowAndRegisterOwnership(void)
{
	// Window Attribute
	Create(SETITEM_PREVIEWWINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	CreateBaseWindowLightGray("SETITEMPREVIEW_WINDOW"
		, (char*)ID2GAMEINTEXT("ITEM_INFO_SET_PREVIEW_TITLE"));

	this->CreateSubControl();

	SetAlignFlag(UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X);
	m_pInterface->UiRegisterControl(this, true);
	m_pInterface->UiShowGroupFocus(SETITEM_PREVIEWWINDOW);
}

void CSetitemPreviewWindow::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIWindowEx::TranslateUIMessage(ControlID, dwMsg);
	switch ( ControlID )
	{
	case IDD_SETITEM_PREVIEW_LIST_SLOT_0:
	case IDD_SETITEM_PREVIEW_LIST_SLOT_1:
	case IDD_SETITEM_PREVIEW_LIST_SLOT_2:
	case IDD_SETITEM_PREVIEW_LIST_SLOT_3:
	case IDD_SETITEM_PREVIEW_LIST_SLOT_4:
	case IDD_SETITEM_PREVIEW_LIST_SLOT_5:
	case IDD_SETITEM_PREVIEW_LIST_SLOT_6:
	case IDD_SETITEM_PREVIEW_LIST_SLOT_7:
	case IDD_SETITEM_PREVIEW_LIST_SLOT_8:
	case IDD_SETITEM_PREVIEW_LIST_SLOT_9:
		{
			if ( SETITEM_UIMSG_DEFINE::dwSETITEM_UIMSG_CHECK_MOUSE_IN & dwMsg )
			{
				CSetitemPreviewlistSlot* const pListSlot = static_cast<CSetitemPreviewlistSlot*>(this->FindControl(ControlID));
				const int nCheckSlot(pListSlot->GetSlotIndex());
				if ( nCheckSlot < static_cast<int>(m_vecItemList.size()) )
				{
					const SNATIVEID& sItemNID = m_vecItemList[nCheckSlot];
					SITEM* pItem = NULL;
					if ( sItemNID != NATIVEID_NULL() && 
						(pItem = GLItemMan::GetInstance().GetItem(sItemNID)) )
					{
						SITEMCUSTOM sCustomItem(sItemNID);
						m_pInterface->SHOW_ITEM_INFO(sCustomItem, FALSE, FALSE, FALSE, TRUE, FALSE, 0, 0);
					}
				}
			}
		}
		break;
	}
}

void CSetitemPreviewWindow::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);	
	{
		this->UpdateList();
	}
}

void CSetitemPreviewWindow::CreateSubControl(void)
{
	// Name Box set, 아래 두 함수는 내부에서 Regist call됨;
	CreateLineBox("SETITEMPREVIEW_NAME_LINEBOX", "PARTY_INFO_LINEBOX_TEXTUREINFO");
	CreateText("ITEM_INFO_SET_PREVIEW_TITLE"
		, "SETITEMPREVIEW_NAME_TEXTBOX"
		, TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y
		, ITEM_INFOR_COLOR::dwSetItem
		, IDD_SETITEM_PREVIEW_ITEMNAME); 

	// List
	CreateLineBox("SETITEMPREVIEW_LIST_RECTANGE", "PARTY_INFO_LINEBOX_TEXTUREINFO");

	m_pListScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	m_pListScrollBar->CreateSub( this
		, "BASIC_SCROLLBAR"
		, UI_FLAG_RIGHT | UI_FLAG_YSIZE
		, IDD_SETITEM_PREVIEW_SCROLLBAR );
	m_pListScrollBar->CreateBaseScrollBar( "SETITEMPREVIEW_LIST_SCROLL" );
	m_pListScrollBar->GetThumbFrame()->SetScrollParent( this );
	m_pListScrollBar->GetThumbFrame()->SetScrollMoveLine( 1 );
	RegisterControl ( m_pListScrollBar );

	for ( int i = 0; i < LIST_VISIBLE_LINE; ++i )
	{
		CString strKeyword;
		strKeyword.Format("SETITEMPREVIEW_LIST_SLOT_RECTANGLE_%d", i);

		m_pListSlot[i] = new CSetitemPreviewlistSlot(m_pEngineDevice, i);
		m_pListSlot[i]->CreateSub(this
			, strKeyword.GetString()
			, UI_FLAG_DEFAULT
			, IDD_SETITEM_PREVIEW_LIST_SLOT_0+i);
		m_pListSlot[i]->CreateSubControl();
		RegisterControl(m_pListSlot[i]);
	}
}

// GLCharacter가 상속받은 GLCHARLOGIC과;
// GLCharClient가 상속받은 GLCHARLOGIC은 데이터 보유방식이 다르다;
// GLCharacter->GLCHARLOGIC->m_PutOnItems;
// GLCharClient->SDROP_CHAR->m_PutOnItems;
// 위의 putonitem역시 형이다르다;
// ItemImage, ItemSlot 두군데에서 기본적으로 단축키 입력에 대한 동작이 있어 콜됨;
// 기타 다르게 동작해야될게 있다면 상단 유아이에서 제어하면된다;
// 어디서는 Image만 사용하고, 어디서는 Slot만 사용하고, 어디서는 둘다 사용한다;
void CSetitemPreviewWindow::SetitemGroupComposition(SSETITEM* pSetitem, STARGETID sTargetID) 
{
	if ( !pSetitem )
		return;

	DWORD dwCharClass(0);
	if ( this->ComposePutonItem( sTargetID, dwCharClass, m_sPutonItem ) == FALSE )
	{
		m_pInterface->UiHideGroup(SETITEM_PREVIEWWINDOW);
		return;
	}

	// 극강부일 경우만 예외처리한다; 
	// 극강부가 낄 수 있는 무기가 많아 UI에 너무많이 출력된다;
	const DWORD dwExceptionClass( 
		((sTargetID.emCrow != CROW_ITEM) && ((dwCharClass & GLCC_EXTREME) != 0)) ? GLCC_EXTREME:0 );


	// 전체 세트 아이템 목록에서 착용가능아이템만 추려낸다;
	m_vecItemList.clear();
	const VNATIVEID& vecItemList = pSetitem->vecItemList;
 	int nWoreSetitemCount = 0, nSetitemTotal = vecItemList.size();
	for ( int i = 0; i < nSetitemTotal; ++i )
	{
		const SNATIVEID& sItemNID = vecItemList[i];
		SITEM* pItem = GLItemMan::GetInstance().GetItem(sItemNID);
		if ( !pItem || ((pItem->sBasicOp.dwReqCharClass & (dwCharClass^(~dwExceptionClass+1))) == 0))
			continue;

		const BOOL bWoreSetitem(this->IsWoreItem(pItem->Suit(), sItemNID, m_sPutonItem));
		if ( bWoreSetitem == TRUE )
			nWoreSetitemCount++;

		m_vecItemList.push_back(vecItemList[i]);
	}

	const int nTotal = static_cast<int>(m_vecItemList.size());

	// 착용 부위별 정렬;
	VNATIVEID::iterator& itrBegin = m_vecItemList.begin(), &itrEnd = m_vecItemList.end();
	std::sort(itrBegin, itrEnd, ___sortSuit_Setitem());

	// 무기 아이템, 공격속성에 따른 정렬;
	VNATIVEID::iterator& itrAttackBegin = m_vecItemList.begin(), &forwardEnd = m_vecItemList.end();
	VNATIVEID::reverse_iterator &itrAttackEnd = m_vecItemList.rbegin(), &reverseEnd = m_vecItemList.rend();
	for ( ; itrAttackBegin != forwardEnd; ++itrAttackBegin )
	{
		SITEM* pItem = GLItemMan::GetInstance().GetItem(*itrAttackBegin);
		if (!pItem || pItem->sSuitOp.emAttack != ITEMATT_NOTHING)
			break;
	}
	for (; itrAttackEnd != reverseEnd; ++itrAttackEnd )
	{
		SITEM* pItem = GLItemMan::GetInstance().GetItem(*itrAttackEnd);
		if (!pItem || pItem->sSuitOp.emAttack != ITEMATT_NOTHING)
			break;
	}
	std::sort(itrAttackBegin, itrAttackEnd.base(), ___sortAttackT_Setitem());

	// 세트 명칭;
	CBasicTextBox* pTBName = static_cast<CBasicTextBox*>(this->FindControl(IDD_SETITEM_PREVIEW_ITEMNAME));
	std::string strTitleName = sc::string::format("%1%(%2%/%3%)", pSetitem->strName.c_str(), nWoreSetitemCount, nTotal).c_str();
	pTBName->SetTextToLimit(strTitleName.c_str(), TRUE, ITEM_INFOR_COLOR::dwSetItem);

	m_pSetitem = pSetitem;
	m_sTargetID = sTargetID;
}

void CSetitemPreviewWindow::ResetSetitemComposition(void)
{
	if ( IsVisible() )
	{
		this->SetitemGroupComposition(m_pSetitem, m_sTargetID);
	}
}

void CSetitemPreviewWindow::UpdateList(void)
{
	const unsigned int nTotalLine = static_cast<unsigned int>(m_vecItemList.size()),//GLFlyCameraControl::GetInstance().GetKeySize(),
		nMoveableLine = nTotalLine - LIST_VISIBLE_LINE;
	// 전체 라인, 한번에 보이는 라인;
	m_pListScrollBar->GetThumbFrame()->SetState(nTotalLine, LIST_VISIBLE_LINE);
	CBasicScrollThumbFrame* const pThumbFrame = m_pListScrollBar->GetThumbFrame();
	const float fViewPosPercent = pThumbFrame->GetPercent();

	unsigned int nVisiblePos = (unsigned int)floor(fViewPosPercent*nMoveableLine);		
	if ( nVisiblePos < 0 )
		nVisiblePos = 0;

	int dwWoreSetitemCount = 0;
	for ( int i = 0; i < LIST_VISIBLE_LINE; ++i )
	{
		const unsigned int nRelativePos = nVisiblePos + i;
		if ( nRelativePos < nTotalLine )
		{
			if ( nRelativePos != -1 )
			{
				const SNATIVEID& sItemNID = m_vecItemList[nRelativePos];
				SITEM* pItem = GLItemMan::GetInstance().GetItem(sItemNID);
				if ( !pItem )
					continue;

				// 착용하고있는 아이템인 경우 슬롯 이름 색깔을 변경한다;
				const BOOL bWoreSetitem(this->IsWoreItem(pItem->Suit(), sItemNID, m_sPutonItem));
				std::string strItemName = GLItemMan::GetInstance().GetItemName(sItemNID);
				m_pListSlot[i]->SetKeySlotInfo(nRelativePos, strItemName, bWoreSetitem);
				m_pListSlot[i]->SetVisibleSingle(TRUE);
				if ( bWoreSetitem == TRUE )
					++dwWoreSetitemCount;
			}
		}
		else
		{
			m_pListSlot[i]->SetVisibleSingle(FALSE);
		}
	}
}

BOOL CSetitemPreviewWindow::IsWoreItem(EMSUIT emSuit, const SNATIVEID& sNID, const SNATIVEID* sPutOnItemCollect) const
{
	switch ( emSuit )
	{
	case SUIT_HEADGEAR:
	case SUIT_UPPER:
	case SUIT_LOWER:
	case SUIT_HAND:
	case SUIT_FOOT:
		return sPutOnItemCollect[emSuit] == sNID;
	case SUIT_HANDHELD:
		return (sPutOnItemCollect[SLOT_RHAND]==sNID || sPutOnItemCollect[SLOT_LHAND]==sNID);
	case SUIT_NECK:
	case SUIT_WRIST:
		return sPutOnItemCollect[emSuit+1]==sNID;	// suit 타입으로 hand는 하나지만 slot은 2개다;
	case SUIT_FINGER:
		return (sPutOnItemCollect[SLOT_RFINGER]==sNID || sPutOnItemCollect[SLOT_LFINGER]==sNID);
	case SUIT_VEHICLE:		// 핑거도 마찬가지;
		return sPutOnItemCollect[SLOT_VEHICLE]==sNID;
	case SUIT_WAISTBAND:
		return sPutOnItemCollect[SLOT_WAISTBAND]==sNID;
	case SUIT_EARRING:
		return sPutOnItemCollect[SLOT_EARRINGS]==sNID;
	case SUIT_ACCESSORIES:
		return (sPutOnItemCollect[SLOT_RACCESSORIES]==sNID || sPutOnItemCollect[SLOT_LACCESSORIES]==sNID);
	case SUIT_DECORATION:
		return sPutOnItemCollect[SLOT_DECORATION]==sNID;
	}
	return false;
}

BOOL CSetitemPreviewWindow::ComposePutonItem(const STARGETID& sTargetID, DWORD& dwCharClass, SNATIVEID* sPutonItem)
{
	const BOOL bIsOtherClient ( (m_pGaeaClient->GetCharacter()->GetGaeaID() != sTargetID.GaeaId) 
		&& sTargetID.GaeaId != 0 
		&& sTargetID.emCrow == CROW_PC );
	if ( bIsOtherClient )
	{
		std::tr1::shared_ptr<GLCharClient> pOther = m_pGaeaClient->GetChar(sTargetID.GaeaId);
		if ( pOther )
		{
			dwCharClass = pOther->GetClass();
			for ( DWORD dwSlot = SLOT_HEADGEAR; dwSlot < SLOT_NSIZE_S_2; ++dwSlot)
				sPutonItem[dwSlot] = pOther->GET_SLOT_ITEM((EMSLOT)dwSlot).sNativeID;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		GLCharacter* pChar = pChar = m_pGaeaClient->GetCharacter();
		if ( pChar )
		{
			for ( DWORD dwSlot = SLOT_HEADGEAR; dwSlot < SLOT_NSIZE_S_2; ++dwSlot)
				sPutonItem[dwSlot] = pChar->m_PutOnItems[dwSlot].GetNativeID();

			dwCharClass = pChar->m_emClass;
		}
		else
		{
			return FALSE;
		}

		if ( sTargetID.emCrow == CROW_ITEM )
		{
			SNATIVEID sItemNID(sTargetID.GaeaId);
			SITEM* pItem = GLItemMan::GetInstance().GetItem(sItemNID);
			if ( !pItem )
				return FALSE;

			dwCharClass = pItem->sBasicOp.dwReqCharClass;
		}
	}
	
	/*
	else if ( sTargetID.emCrow == CROW_ITEM )
	{
		SNATIVEID sItemNID(sTargetID.GaeaId);
		SITEM* pItem = GLItemMan::GetInstance().GetItem(sItemNID);
		if ( !pItem )
			return;

		EMSUIT emSuit(pItem->Suit());
		switch ( emSuit )
		{
		case SUIT_HEADGEAR:
		case SUIT_UPPER:
		case SUIT_LOWER:
		case SUIT_HAND:
		case SUIT_FOOT:
			sPutonItem[emSuit] = sItemNID;	break;
		case SUIT_HANDHELD:
			sPutonItem[SLOT_RHAND]=sItemNID; break;
		case SUIT_NECK:
		case SUIT_WRIST:
			sPutonItem[emSuit+1]=sItemNID;	break;// suit 타입으로 hand는 하나지만 slot은 2개다;
		case SUIT_FINGER:
			sPutonItem[SLOT_RFINGER]=sItemNID; break;
		case SUIT_VEHICLE:		// 핑거도 마찬가지;
			sPutonItem[SLOT_VEHICLE]=sItemNID; break;
		case SUIT_WAISTBAND:
			sPutonItem[SLOT_WAISTBAND]=sItemNID; break;
		case SUIT_EARRING:
			sPutonItem[SLOT_EARRINGS]=sItemNID; break;
		case SUIT_ACCESSORIES:
			sPutonItem[SLOT_RACCESSORIES]=sItemNID; break;
		case SUIT_DECORATION:
			sPutonItem[SLOT_DECORATION]=sItemNID; break;
		}
		dwCharClass = pItem->sBasicOp.dwReqCharClass;
	}
	else
		return FALSE;
		*/

	return TRUE;
}