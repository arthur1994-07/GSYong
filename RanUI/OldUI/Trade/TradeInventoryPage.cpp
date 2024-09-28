#include "StdAfx.h"
#include "TradeInventoryPage.h"
#include "../Item/ItemSlotEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/Inventory/GLInventory.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CTradeInventoryPage::nSTARTLINE = 0;
const int CTradeInventoryPage::nOUTOFRANGE = -1;

CTradeInventoryPage::CTradeInventoryPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pScrollBar ( NULL )
    , m_nONE_VIEW_SLOT ( nONE_VIEW_SLOT_DEFAULT )
    , m_nCurPos ( -1 )
{
	SecureZeroMemory ( m_pItemSlotArray, sizeof( m_pItemSlotArray ) );
	SecureZeroMemory ( m_pItemSlotArrayDummy, sizeof( m_pItemSlotArrayDummy ) );
}

CTradeInventoryPage::~CTradeInventoryPage ()
{
}

CUIControl*	CTradeInventoryPage::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

void CTradeInventoryPage::CreateSubControl ()
{
	CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"TRADEINVENTORYSLOT_ITEM_SLOT0",
		"TRADEINVENTORYSLOT_ITEM_SLOT1",
		"TRADEINVENTORYSLOT_ITEM_SLOT2",
		"TRADEINVENTORYSLOT_ITEM_SLOT3",
		"TRADEINVENTORYSLOT_ITEM_SLOT4",
		"TRADEINVENTORYSLOT_ITEM_SLOT5",
		"TRADEINVENTORYSLOT_ITEM_SLOT6",
		"TRADEINVENTORYSLOT_ITEM_SLOT7",
		"TRADEINVENTORYSLOT_ITEM_SLOT8",
		"TRADEINVENTORYSLOT_ITEM_SLOT9"
	};

	//	더미 슬롯
	for ( int i = 0; i < nMAX_ONE_VIEW_SLOT; i++ )
	{
		m_pItemSlotArrayDummy[i] = CreateControl ( strSlotKeyword[i].GetString () );
	}
	
	//	실제 데이타
	for ( int i = 0; i < EInvenY; i++ )
	{
		m_pItemSlotArray[i] = CreateItemSlot ( strSlotKeyword[0], ITEM_SLOT0 + i );
	}
	
	{	//	스크롤바
		m_pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		m_pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
		m_pScrollBar->CreateBaseScrollBar ( "TRADEINVENTORY_PAGE_SCROLL" );
		m_pScrollBar->GetThumbFrame()->SetState ( EInvenY, m_nONE_VIEW_SLOT );
		RegisterControl ( m_pScrollBar );
	}

	SetItemSlotRender ( nSTARTLINE, m_nONE_VIEW_SLOT );
}

void CTradeInventoryPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetItemIndex ( nOUTOFRANGE, nOUTOFRANGE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	//	NOTE
	//		사용/가부 설정
	UpdateBLOCK ();

	if ( m_pScrollBar )
	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBar->GetThumbFrame ();
		int nTotal = pThumbFrame->GetTotal ();

		if ( nTotal < m_nONE_VIEW_SLOT )
			return;
	
		const int nViewPerPage = pThumbFrame->GetViewPerPage ();
		if ( nViewPerPage <= nTotal )
		{
			int nCurPos = nSTARTLINE;
			const int nMovableLine = nTotal - nViewPerPage;
			float fPercent = pThumbFrame->GetPercent ();

			nCurPos = (int)floor(fPercent * nMovableLine);
			if ( nCurPos < nSTARTLINE ) nCurPos = nSTARTLINE;

			if ( m_nCurPos == nCurPos )
				return;

			m_nCurPos = nCurPos;

			ResetAllItemSlotRender ( nTotal );
			SetItemSlotRender ( nCurPos, nCurPos + nViewPerPage );
		}
	}
}

void CTradeInventoryPage::ResetAllItemSlotRender ( int nTotal )
{
	if ( nTotal < 0 )
		return ;

	for ( int i = 0; i < nTotal; i++ )
	{
		CItemSlot* pItemSlot = m_pItemSlotArray[i];
		if ( pItemSlot )
		{
			pItemSlot->SetVisibleSingle ( FALSE );
		}
	}
}

void CTradeInventoryPage::SetItemSlotRender ( int nStartIndex, int nTotal )
{
	if ( nTotal < 0 )
		return ;

	const UIRECT& rcParentPos = GetGlobalPos ();

	for ( int i = nStartIndex; i < nTotal; i++ )
	{
		CItemSlotEx* pItemSlot = m_pItemSlotArray[i];
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

CItemSlotEx* CTradeInventoryPage::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlotEx* pItemSlot = new CItemSlotEx ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubTradeInventory ( EInvenX );
	RegisterControl ( pItemSlot );

	return pItemSlot;
}



void CTradeInventoryPage::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void CTradeInventoryPage::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}

void CTradeInventoryPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( ITEM_SLOT0 <= ControlID && ControlID < ITEM_SLOT_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			const int nPosY = ControlID - ITEM_SLOT0;
			CItemSlot* pSlot = m_pItemSlotArray[nPosY];
			const int nPosX = pSlot->GetItemIndex ();

			if ( nPosY < 0 || nPosX < 0 )
				return;

			SetItemIndex ( nPosX, nPosY );
		}
	}
}

void CTradeInventoryPage::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
	if( nPosX >= EInvenX )
		return;

	int nPosY = ref_InvenItem.wPosY;
	if( nPosY >= EInvenY )
		return;

	m_pItemSlotArray[nPosY]->SetItemImage ( nPosX, ref_InvenItem );
}

void CTradeInventoryPage::UnLoadItemPage ()
{
	for ( int y = 0; y < EInvenY; y++ )
	{		
		for ( int x = 0; x < EInvenX; x++ )
		{
			UnLoadItem ( x, y );
		}
	}
}

const SINVENITEM* CTradeInventoryPage::GetItem(int nPosX, int nPosY) const
{
	if( nPosX >= EInvenX )
		return NULL;

	if( nPosY >= EInvenY )
		return NULL;

	return m_pItemSlotArray[nPosY]->GetItemImage(nPosX);
}

void CTradeInventoryPage::UnLoadItem ( int nPosX, int nPosY )
{
	if( nPosX >= EInvenX )
		return;

	if( nPosY >= EInvenY )
		return;

	m_pItemSlotArray[nPosY]->ResetItemImage ( nPosX );
}

void CTradeInventoryPage::LoadItemPage ( GLInventory &ItemData )
{
	const GLInventory::CELL_MAP *pItemList = ItemData.GetItemList();

	if ( pItemList->size () > EInven1PageSlot )
	{
		GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
		return ;
	}


	{	//	삭제

		GLInventory::CELL_MAP_CITER iter = pItemList->begin();
		GLInventory::CELL_MAP_CITER iter_end = pItemList->end();

		for ( WORD y = 0; y < EInvenY; y++ )
		{
			for ( WORD x = 0; x < EInvenX; x++ )
			{
				const SINVENITEM* pRefInvenItem = GetItem(x, y);
                if (pRefInvenItem && pRefInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL())
				{
					GLInventory::CELL_KEY foundkey ( x, y );
					GLInventory::CELL_MAP_CITER found = pItemList->find ( foundkey );
					if ( found == iter_end )
					{
						UnLoadItem ( x, y );
					}
				}
			}
		}
	}

	{	//	등록
		GLInventory::CELL_MAP_CITER iter = pItemList->begin();
		GLInventory::CELL_MAP_CITER iter_end = pItemList->end();

		for ( ; iter!=iter_end; ++iter )
		{
            SINVENITEM* pInvenItem = iter->second;
			const SINVENITEM* pRefInvenItemOld = GetItem ( pInvenItem->wPosX, pInvenItem->wPosY );
			if (pRefInvenItemOld && (*pInvenItem != *pRefInvenItemOld))
			{
				LoadItem ( *pInvenItem );
			}	
		}
	}
}

void  CTradeInventoryPage::UpdateFlipItem ( GLInventory& ref_TradeInventory )
{
	ResetAllFlipItem ();

	GLInventory::CELL_MAP_CITER iter = ref_TradeInventory.GetItemList()->begin();
	GLInventory::CELL_MAP_CITER iter_end = ref_TradeInventory.GetItemList()->end();

	for ( ; iter != iter_end; iter++ )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		if ( pInvenItem )
		{
			int nPosX = pInvenItem->wBackX;
			if( nPosX >= EInvenX )
				continue;

			int nPosY = pInvenItem->wBackY;
			if( nPosY >= EInvenY )
				continue;

			m_pItemSlotArray[nPosY]->SetFlipItem ( nPosX, TRUE );
		}
	}
}

void CTradeInventoryPage::ResetAllFlipItem ()
{
	for ( WORD y = 0; y < EInvenY; y++ )
	{
		for ( WORD x = 0; x < EInvenX; x++ )
		{
			m_pItemSlotArray[y]->SetFlipItem ( x, FALSE );
		}
	}
}

void CTradeInventoryPage::UpdateBLOCK ()
{
	const WORD& wAddINVENLINE = m_pGaeaClient->GetCharacter()->GetOnINVENLINE();
	int nUSABLE_INVENLINE = GLCONST_CHAR::wInvenDefaultLine + wAddINVENLINE;

	if ( EInvenY < nUSABLE_INVENLINE )
	{		
		GASSERT ( 0 && "데이타 오류입니다." );

		nUSABLE_INVENLINE = EInvenY;
	}
	
	//	사용 가능
	for ( int i = 0; i < nUSABLE_INVENLINE; i++ )
		m_pItemSlotArray[i]->ResetBLOCK ();

	//	사용 불가능
	for ( int i = nUSABLE_INVENLINE; i < EInvenY; i++ )
		m_pItemSlotArray[i]->SetBLOCK ();
}

void CTradeInventoryPage::SetOneViewSlot ( const int& nMAX_ONE_VIEW )
{
	m_nONE_VIEW_SLOT = nMAX_ONE_VIEW;
	m_nCurPos = -1;
	m_pScrollBar->GetThumbFrame()->SetState ( EInvenY, nMAX_ONE_VIEW );
}
