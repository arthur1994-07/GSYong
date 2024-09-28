#include "StdAfx.h"
#include "MarketPage.h"
#include "../Item/ItemSlot.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CMarketPage::nSTARTLINE = 0;
const int CMarketPage::nOUTOFRANGE = -1;

const DWORD	FAILED_CASTING	= 0x01;

CMarketPage::CMarketPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pScrollBar ( NULL )
    , m_nCurPos ( -1 )
	, m_MarketType(GLInventorySale::CROW_SALE_TYPE_VERSION::CROW_SALE_NONE)

{
	memset ( m_pItemSlotArray, 0, sizeof ( CItemSlot* ) * ESaleY );
	memset ( m_pItemSlotArrayDummy, 0, sizeof ( CUIControl* ) * nMAX_ONE_VIEW_SLOT );
}

CMarketPage::~CMarketPage ()
{
}

CUIControl*	CMarketPage::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void CMarketPage::CreateSubControl ()
{
	CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"MARKET_ITEM_SLOT0",
		"MARKET_ITEM_SLOT1",
		"MARKET_ITEM_SLOT2",
		"MARKET_ITEM_SLOT3",
		"MARKET_ITEM_SLOT4",
		"MARKET_ITEM_SLOT5",
		"MARKET_ITEM_SLOT6",
		"MARKET_ITEM_SLOT7",
	};

	{	//	더미 슬롯
		for ( int i = 0; i < nMAX_ONE_VIEW_SLOT; i++ )
		{
			m_pItemSlotArrayDummy[i] = CreateControl ( strSlotKeyword[i].GetString () );
		}
	}

	const int nExampleCount = 8;
	{	//	실제 데이타
		for ( int i = 0; i < nExampleCount; i++ )
		{
			m_pItemSlotArray[i] = CreateItemSlot ( strSlotKeyword[0], ITEM_SLOT0 + i );
		}
	}

	{	//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
		pScrollBar->CreateBaseScrollBar ( "MARKET_PAGE_SCROLL" );
		pScrollBar->GetThumbFrame()->SetState ( nExampleCount, nMAX_ONE_VIEW_SLOT );
		RegisterControl ( pScrollBar );
		m_pScrollBar = pScrollBar;	
	}

	SetItemSlotRender ( nSTARTLINE, nMAX_ONE_VIEW_SLOT );
}

void CMarketPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetItemIndex ( nOUTOFRANGE, nOUTOFRANGE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pScrollBar )
	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBar->GetThumbFrame ();
		int nTotal = pThumbFrame->GetTotal ();

		if ( nTotal <= nMAX_ONE_VIEW_SLOT ) return ;

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

			ResetAllItemSlotRender ( nTotal );
			SetItemSlotRender ( nCurPos, nCurPos + nViewPerPage );
		}
	}

}

void CMarketPage::ResetAllItemSlotRender ( int nTotal )
{
	if ( nTotal < 0 ) return ;

	for ( int i = 0; i < nTotal; i++ )
	{
		CItemSlot* pItemSlot = m_pItemSlotArray[i];
		if ( pItemSlot )
		{
			pItemSlot->SetVisibleSingle ( FALSE );
		}
	}
}

void CMarketPage::SetItemSlotRender ( int nStartIndex, int nTotal )
{
	if ( nTotal < 0 ) return ;

	const UIRECT& rcParentPos = GetGlobalPos ();

	for ( int i = nStartIndex; i < nTotal; i++ )
	{
		CItemSlot* pItemSlot = m_pItemSlotArray[i];
		if ( pItemSlot )
		{
			int nAbsoluteIndex = i - nStartIndex;

			CUIControl* pDummyControl = m_pItemSlotArrayDummy[nAbsoluteIndex];			
			const UIRECT& rcSlotPos = pDummyControl->GetGlobalPos ();
			const UIRECT& rcSlotLocalPos = pDummyControl->GetLocalPos ();

			pItemSlot->SetLocalPos ( rcSlotLocalPos );
			pItemSlot->SetGlobalPos ( rcSlotPos );

			pItemSlot->SetVisibleSingle ( TRUE );
		}
	}
}

CItemSlot* CMarketPage::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlot* pItemSlot = new CItemSlot(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( ESaleX );
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

void CMarketPage::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void CMarketPage::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}

void CMarketPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( ITEM_SLOT0 <= ControlID && ControlID < ITEM_SLOT_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			const int nPosY = ControlID - ITEM_SLOT0;
			CItemSlot* pSlot = m_pItemSlotArray[nPosY];
			const int nPosX = pSlot->GetItemIndex ();

			SetItemIndex ( nPosX, nPosY );
		}		

		if( dwMsg == UIMSG_EXCHANGE_LIST && m_MarketType == GLInventorySale::CROW_SALE_V_2 )
		{
			if(GetParent()) GetParent()->TranslateUIMessage(GetWndID(), dwMsg );
		}
	}
}

void CMarketPage::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlotArray[nPosY]->SetItemImage ( nPosX, ref_InvenItem );

	if( m_MarketType == GLInventorySale::CROW_SALE_V_2 )
		m_pItemSlotArray[nPosY]->SetExchangeSlot( true );
	else
		m_pItemSlotArray[nPosY]->SetExchangeSlot( false );
}

void CMarketPage::LoadItemPage ( GLInventory &ItemData )
{
//	SITEMCUSTOM sItemCustom = m_pGaeaClient->GetCharacter()->GET_HOLD_ITEM();

	const GLInventory::CELL_MAP *ItemList = ItemData.GetItemList();

	if ( ItemList->size () > ESaleTotalSlot )
	{
		GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
		return ;
	}

	GLInventory::CELL_MAP_CITER iter = ItemList->begin();
	GLInventory::CELL_MAP_CITER iter_end = ItemList->end();

	//	int nPosX, nPosY;
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEMCUSTOM &ref_ItemCustom = pInvenItem->sItemCustom;
		const SITEM* pItemData = GLogicData::GetInstance().GetItem ( ref_ItemCustom.GetNativeID() );
		if ( !pItemData )	continue;

		//		nPosX = (*iter).second->wPosX;
		//		nPosY = (*iter).second->wPosY;

		LoadItem ( *pInvenItem );
	}	
}

void CMarketPage::LoadItemPage ( const GLInventorySale *pItemInven )
{
	if( pItemInven == NULL )	return;

	const GLInventory::CELL_MAP *ItemList = pItemInven->GetItemList();

	if ( ItemList->size () > ESaleTotalSlot )
	{
		GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
		return ;
	}

	m_MarketType = pItemInven->GetSaleFuncType();

	GLInventory::CELL_MAP_CITER iter = ItemList->begin();
	GLInventory::CELL_MAP_CITER iter_end = ItemList->end();

	//	int nPosX, nPosY;
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEMCUSTOM &ref_ItemCustom = pInvenItem->sItemCustom;
		const SITEM* pItemData = GLogicData::GetInstance().GetItem ( ref_ItemCustom.GetNativeID() );
		if ( !pItemData )	continue;

		//		nPosX = (*iter).second->wPosX;
		//		nPosY = (*iter).second->wPosY;

		LoadItem ( *pInvenItem );
	}	
}


void CMarketPage::UnLoadItemPage ()
{
	for ( int y = 0; y < ESaleY; y++ )
	{
		for ( int x = 0; x < ESaleX; x++ )
		{
			UnLoadItem ( x, y );
		}
	}
}

const SINVENITEM* CMarketPage::GetItem(int nPosX, int nPosY) const
{
	return m_pItemSlotArray[nPosY]->GetItemImage(nPosX);
}

void CMarketPage::UnLoadItem ( int nPosX, int nPosY )
{
	m_pItemSlotArray[nPosY]->ResetItemImage ( nPosX );
}

void CMarketPage::SetVisibleScrollbar( bool bVisible )
{
    if ( bVisible )
        m_pScrollBar->SetVisibleSingle( TRUE  );
    else
        m_pScrollBar->SetVisibleSingle( FALSE );
}

CItemSlot* CMarketPage::GetItemSlot( int nRow )
{
    if ( nRow < 0 || nRow >= ESaleY )
        return NULL;

    return m_pItemSlotArray[ nRow ];
}

BOOL CMarketPage::IsMarketType( GLInventorySale::CROW_SALE_TYPE_VERSION type )
{
	if( m_MarketType == type ) return TRUE;

	return FALSE;
}