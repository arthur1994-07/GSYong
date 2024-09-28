#include "StdAfx.h"
#include "InventoryPage.h"
#include "../Item/ItemSlot.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../Tutorial/ExplainInterface.h"
#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/TipBox.h"
#include "InventoryWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CInventoryPage::nSTARTLINE = 0;
const int CInventoryPage::nOUTOFRANGE = -1;

CInventoryPage::CInventoryPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pScrollBar ( NULL )
    , m_nCurPos ( -1 )
    , m_nONE_VIEW_SLOT ( nONE_VIEW_SLOT_DEFAULT )
{
	memset ( m_pItemSlotArray, 0, sizeof( m_pItemSlotArray ) );
	memset ( m_pItemSlotArrayDummy, 0, sizeof( m_pItemSlotArrayDummy ) );
}

CInventoryPage::~CInventoryPage ()
{
}

CUIControl*	CInventoryPage::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void CInventoryPage::CreateSubControl ()
{
	CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"INVENTORY_ITEM_SLOT0",
		"INVENTORY_ITEM_SLOT1",
		"INVENTORY_ITEM_SLOT2",
		"INVENTORY_ITEM_SLOT3",
		"INVENTORY_ITEM_SLOT4",
		"INVENTORY_ITEM_SLOT5",
		"INVENTORY_ITEM_SLOT6",
		"INVENTORY_ITEM_SLOT7",
		"INVENTORY_ITEM_SLOT8",
		"INVENTORY_ITEM_SLOT9"
	};

	//	더미 슬롯
	for ( int i = 0; i < nMAX_ONE_VIEW_SLOT; i++ )
	{
		m_pItemSlotArrayDummy[i] = CreateControl ( strSlotKeyword[i].GetString () );
	}
	
	//	실제 데이타
	for ( int i = 0; i < EM_INVENSIZE_Y_FORCE; i++ )
	{
		m_pItemSlotArray[i] = CreateItemSlot ( strSlotKeyword[0], ITEM_SLOT0 + i );
	}
	
	{	//	스크롤바
		m_pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		m_pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
		m_pScrollBar->CreateBaseScrollBar ( "INVENTORY_PAGE_SCROLL" );
		m_pScrollBar->GetThumbFrame()->SetState ( EM_INVENSIZE_Y_FORCE, m_nONE_VIEW_SLOT );
		RegisterControl ( m_pScrollBar );
	}

	SetItemSlotRender ( nSTARTLINE, m_nONE_VIEW_SLOT );
}

void CInventoryPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
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

		if ( nTotal < m_nONE_VIEW_SLOT ) return ;



		const int nViewPerPage = pThumbFrame->GetViewPerPage ();
		if ( nViewPerPage <= nTotal )
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

void CInventoryPage::ResetAllItemSlotRender ( int nTotal )
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

	if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
		m_pGaeaClient->GetTutorial()->IsSubThreeStep() )
	{
		m_pInterface->GetExplainInterface()->SetSlotBoxVisibleSingle(FALSE);
		m_pInterface->GetExplainInterface()->SetSlotBoxVisibleAtInven(FALSE);
	}
}

void CInventoryPage::SetItemSlotRender ( int nStartIndex, int nTotal )
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

CItemSlot* CInventoryPage::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlot* pItemSlot = new CItemSlot(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( EInvenX, TRUE );	
	RegisterControl ( pItemSlot );
	return pItemSlot;
}


void CInventoryPage::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void CInventoryPage::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}

void CInventoryPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( ITEM_SLOT0 <= ControlID && ControlID < ITEM_SLOT_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			const int nPosY = ControlID - ITEM_SLOT0;
			CItemSlot* pSlot = m_pItemSlotArray[nPosY];
			const int nPosX = pSlot->GetItemIndex ();

			if ( nPosY < 0 || nPosX < 0 ) return ;

			SetItemIndex ( nPosX, nPosY );
		}
	}
}

void CInventoryPage::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlotArray[nPosY]->SetItemImage ( nPosX, ref_InvenItem );
}

void CInventoryPage::UnLoadItemPage ()
{
	for ( int y = 0; y < EM_INVENSIZE_Y_FORCE; y++ )
	{
		for ( int x = 0; x < EInvenX; x++ )
		{
			UnLoadItem ( x, y );
		}
	}
}

const SINVENITEM* CInventoryPage::GetItem(int nPosX, int nPosY) const
{
	return m_pItemSlotArray[nPosY]->GetItemImage ( nPosX );
}

void CInventoryPage::UnLoadItem ( int nPosX, int nPosY )
{
	m_pItemSlotArray[nPosY]->ResetItemImage ( nPosX );
}

void CInventoryPage::LoadItemPage ( GLInventory &ItemData )
{
	const GLInventory::CELL_MAP *pItemList = ItemData.GetItemList();
	
	if ( pItemList->size () > (EM_INVENSIZE_Y_FORCE * EInvenX) )
	{
		GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
		return ;
	}


	{	//	삭제

		GLInventory::CELL_MAP_CITER iter = pItemList->begin();
		GLInventory::CELL_MAP_CITER iter_end = pItemList->end();

		for ( WORD y = 0; y < EM_INVENSIZE_Y_FORCE; y++ )
		{
			for ( WORD x = 0; x < EInvenX; x++ )
			{
				const SINVENITEM* pRefInvenItem = GetItem ( x, y );
				if (pRefInvenItem && pRefInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL() )
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
			SINVENITEM* pInvenItem = (*iter).second;
			const SINVENITEM* pRefInvenItemOld = GetItem ( pInvenItem->wPosX, pInvenItem->wPosY );
			if (pRefInvenItemOld && pInvenItem->sItemCustom != pRefInvenItemOld->sItemCustom )
			{
				LoadItem ( *pInvenItem );
			}	
		}
	}
}

void CInventoryPage::UpdateBLOCK ()
{
	const WORD& wAddINVENLINE = m_pGaeaClient->GetCharacter()->GetOnINVENLINE();
	int nUSABLE_INVENLINE = GLCONST_CHAR::wInvenDefaultLine + wAddINVENLINE;

	if ( EM_INVENSIZE_Y_FORCE < nUSABLE_INVENLINE )
	{		
		GASSERT ( 0 && "데이타 오류입니다." );

		nUSABLE_INVENLINE = EM_INVENSIZE_Y_FORCE;
	}
	
	//	사용 가능
	for ( int i = 0; i < nUSABLE_INVENLINE; i++ )
		m_pItemSlotArray[i]->ResetBLOCK ();

	//	사용 불가능
	for ( int i = nUSABLE_INVENLINE; i < EInvenY; i++ )
		m_pItemSlotArray[i]->SetBLOCK ();
}

void CInventoryPage::SetOneViewSlot ( const int& nMAX_ONE_VIEW )
{
	m_nONE_VIEW_SLOT = nMAX_ONE_VIEW;
	m_nCurPos = -1;
	m_pScrollBar->GetThumbFrame()->SetState ( EM_INVENSIZE_Y_FORCE, nMAX_ONE_VIEW );
}