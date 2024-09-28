
#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../RanLogic/Item/GLItemMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"

#include "../Item/FlipItemSlot.h"
#include "./SelectiveformBoxPage.h"

const int SelectiveformBoxPage::nSTARTLINE = 0;
const int SelectiveformBoxPage::nOUTOFRANGE = -1;

SelectiveformBoxPage::SelectiveformBoxPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_pGaeaClient ( pGaeaClient )
, m_pInterface(pInterface)
, m_pScrollBar ( NULL )
, m_nCurPos(-1)
, m_nSelectPosX(-1)
, m_nSelectPosY(-1)
{
	ZeroMemory( m_pItemSlotArray, sizeof(CItemSlot*)*SELFORMBOXP_MAX_LINE );
	ZeroMemory( m_pItemSlotDummy, sizeof(CUIControl*)*SELFORMBOXP_HEIGHT );
}

SelectiveformBoxPage::~SelectiveformBoxPage(void)
{
}

// 아이템 더미;
CUIControl*	SelectiveformBoxPage::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

// 아이템 슬롯;
CFlipItemSlot* SelectiveformBoxPage::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CFlipItemSlot* pItemSlot = new CFlipItemSlot ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( SELFORMBOXP_WIDTH, TRUE );
	pItemSlot->CreateFlipImage ( SELFORMBOXP_WIDTH, true, NS_UITEXTCOLOR::GREEN );
	RegisterControl ( pItemSlot );

	return pItemSlot;
}

BOOL SelectiveformBoxPage::GetSelectPos(INT& nPos) const
{
	if ( m_nSelectPosX == -1 || m_nSelectPosY == -1 )
		return FALSE;

	nPos = m_nSelectPosY * SELFORMBOXP_WIDTH + m_nSelectPosX;
	return TRUE;
}

void SelectiveformBoxPage::SetItemSlotRender ( int nStartIndex, int nTotal )
{
	if ( nTotal < 0 ) return ;

	const UIRECT& rcParentPos = GetGlobalPos ();

	for ( int i = nStartIndex; i < nTotal; i++ )
	{
		CFlipItemSlot* pItemSlot = m_pItemSlotArray[i];
		if ( pItemSlot )
		{
			int nAbsoluteIndex = i - nStartIndex;

			CUIControl* pDummyControl = m_pItemSlotDummy[nAbsoluteIndex];			
			const UIRECT& rcSlotPos = pDummyControl->GetGlobalPos ();
			const UIRECT& rcSlotLocalPos = pDummyControl->GetLocalPos ();

			pItemSlot->SetLocalPos ( rcSlotLocalPos );
			pItemSlot->SetGlobalPos ( rcSlotPos );

			pItemSlot->SetVisibleSingle ( TRUE );
		}
	}
}

void SelectiveformBoxPage::ResetAllItemSlotRender( int nTotal )
{
	if ( nTotal < 0 ) return ;

	for ( int i = 0; i < nTotal; i++ )
	{
		CFlipItemSlot* pItemSlot = m_pItemSlotArray[i];
		if ( pItemSlot )
		{
			pItemSlot->SetVisibleSingle ( FALSE );
		}
	}
}

void SelectiveformBoxPage::CreateSubControl(void)
{
	// 한번에 초기화하기위하여;
	CString strSlotKeyword[SELFORMBOXP_MAX_LINE] =
	{
		"ITEMBANK_ITEM_SLOT0",
		"ITEMBANK_ITEM_SLOT1",
		"ITEMBANK_ITEM_SLOT2",
		"ITEMBANK_ITEM_SLOT3",
	};

	// 슬롯 데이터 및 더미 슬롯;
	for ( int i = 0; i < SELFORMBOXP_HEIGHT; ++i )
		m_pItemSlotDummy[i] = CreateControl(strSlotKeyword[i].GetString());

	for ( int i = 0; i < SELFORMBOXP_MAX_LINE; ++i )
		m_pItemSlotArray[i] = CreateItemSlot(strSlotKeyword[0], ITEM_SLOT0 + i);

	// 스크롤바;
	m_pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	m_pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
	m_pScrollBar->CreateBaseScrollBar ( "SELFORMBOX_PAGE_SCROLL" );
	m_pScrollBar->GetThumbFrame()->SetState ( SELFORMBOXP_MAX_LINE, SELFORMBOXP_HEIGHT );
	RegisterControl ( m_pScrollBar );

	SetItemSlotRender(nSTARTLINE, SELFORMBOXP_HEIGHT);
}

void SelectiveformBoxPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	// 스크롤 위치에 따른 슬롯 갱신;
	if ( m_pScrollBar )
	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBar->GetThumbFrame ();
		int nTotal = pThumbFrame->GetTotal ();

		if ( nTotal <= SELFORMBOXP_HEIGHT ) return ;

		const int nViewPerPage = pThumbFrame->GetViewPerPage ();
		if ( nViewPerPage < nTotal )
		{
			int nCurPos = nSTARTLINE;
			const int nMovableLine = nTotal - nViewPerPage;
			float fPercent = pThumbFrame->GetPercent ();

			nCurPos = (int)floor(fPercent * nMovableLine);
			if ( nCurPos < nSTARTLINE ) nCurPos = nSTARTLINE;

			if ( m_nCurPos == nCurPos ) return;

			m_nCurPos = nCurPos;

			ResetAllItemSlotRender( nTotal );
			SetItemSlotRender( nCurPos, nCurPos + nViewPerPage );
		}
	}
}

void SelectiveformBoxPage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage(ControlID, dwMsg);
	if ( ITEM_SLOT0 <= ControlID && ControlID < ITEM_SLOT_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			const int nPosY = ControlID - ITEM_SLOT0;
			CFlipItemSlot* pSlot = m_pItemSlotArray[nPosY];
			const int nPosX = pSlot->GetItemIndex ();

			// 툴팁 출력;
			const SINVENITEM* pInvenItem = m_pItemSlotArray[nPosY]->GetItemImage(nPosX);
			if (pInvenItem && pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL() )
			{
				m_pInterface->SHOW_ITEM_INFO(
					pInvenItem->sItemCustom,
					FALSE, FALSE, FALSE, TRUE, FALSE,
					pInvenItem->wPosX, pInvenItem->wPosY );
			}

			// 선택시 정보 갱신;
			if ( (UIMSG_LBMUP_ITEMSLOT & dwMsg) == UIMSG_LBMUP_ITEMSLOT )
			{				
				if ( nPosY < 0 || nPosX < 0 ) 
					return;

				// 이전 플립 없앤다;
				if ( m_nSelectPosX != -1 || m_nSelectPosY != -1 )
					m_pItemSlotArray[m_nSelectPosY]->SetFlipItem(m_nSelectPosX,FALSE);

				if (pInvenItem && pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL() )
				{
					m_nSelectPosX = nPosX;
					m_nSelectPosY = nPosY;
					m_pItemSlotArray[m_nSelectPosY]->SetFlipItem(m_nSelectPosX,TRUE);
				}
			}
		}
	}
}

void SelectiveformBoxPage::ResetState(void)
{
	// 이전 플립 없앤다;
	if ( m_nSelectPosX != -1 || m_nSelectPosY != -1 )
		m_pItemSlotArray[m_nSelectPosY]->SetFlipItem(m_nSelectPosX,FALSE);

	if ( m_pScrollBar )
		m_pScrollBar->GetThumbFrame()->SetState ( SELFORMBOXP_MAX_LINE, SELFORMBOXP_HEIGHT );

	m_nSelectPosX = m_nSelectPosY = m_nCurPos = -1;
}

void SelectiveformBoxPage::LoadItemPage(std::vector<ITEM::SBOX_ITEM>& vecItemID)
{
	ResetState();

	// 전체 리셋;
	for ( int col = 0; col < SELFORMBOXP_MAX_LINE; ++col )
	{
		for ( int row = 0; row < SELFORMBOXP_WIDTH; ++row )
		{
			m_pItemSlotArray[col]->ResetItemImage(row)	;
		}
	}

	// 등록;
	INT nItemCount((INT)vecItemID.size())
		, nCurrentCount(0);
	for ( int col = 0; col < SELFORMBOXP_MAX_LINE && nCurrentCount < nItemCount; ++col )
	{
		for ( int row = 0; row < SELFORMBOXP_WIDTH && nCurrentCount < nItemCount; ++row )
		{
			SINVENITEM sInvenItem(row, col);
			sInvenItem.sItemCustom = SITEMCUSTOM(vecItemID[nCurrentCount].nidITEM);
			sInvenItem.sItemCustom.wTurnNum = vecItemID[nCurrentCount].dwAMOUNT;
			m_pItemSlotArray[col]->SetItemImage(row, sInvenItem);
		
			++nCurrentCount;		
		}
	}
}