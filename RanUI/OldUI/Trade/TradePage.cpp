#include "StdAfx.h"
#include "TradePage.h"
#include "../Item/ItemSlotEx.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CTradePage::nOUTOFRANGE = -1;

CTradePage::CTradePage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient ( pGaeaClient )
	, m_pInterface	( pInterface )
{
	memset ( m_pItemSlotArray, 0, sizeof ( CItemSlot* ) * ETradeY );
}

CTradePage::~CTradePage ()
{
}

CUIControl*	CTradePage::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void CTradePage::CreateSubControl ()
{
	CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"TRADESLOT_ITEM_SLOT0",
		"TRADESLOT_ITEM_SLOT1",
	};

	{	//	실제 데이타
		for ( int i = 0; i < ETradeY; i++ )
		{
			m_pItemSlotArray[i] = CreateItemSlot ( strSlotKeyword[i], ITEM_SLOT0 + i );
		}
	}
}

CItemSlotEx* CTradePage::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlotEx* pItemSlot = new CItemSlotEx ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubTrade ( ETradeX );
	RegisterControl ( pItemSlot );
	return pItemSlot;
}


void CTradePage::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlotArray[nPosY]->SetItemImage ( nPosX, ref_InvenItem );
}

void CTradePage::LoadItemPage ( GLInventory &ItemData )
{
	const GLInventory::CELL_MAP *pItemList = ItemData.GetItemList();
	
	if ( pItemList->size () > ETradeTotalSlot )
	{
		GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
		return ;
	}


	{	//	삭제

		GLInventory::CELL_MAP_CITER iter = pItemList->begin();
		GLInventory::CELL_MAP_CITER iter_end = pItemList->end();

		for ( WORD y = 0; y < ETradeY; y++ )
		{
			for ( WORD x = 0; x < ETradeX; x++ )
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
			if (pRefInvenItemOld && (*pInvenItem != *pRefInvenItemOld))
			{
				LoadItem ( *pInvenItem );
			}	
		}
	}
}


void CTradePage::UnLoadItemPage ()
{
	for ( int y = 0; y < ETradeY; y++ )
	{
		for ( int x = 0; x < ETradeX; x++ )
		{
			UnLoadItem ( x, y );
		}
	}
}

const SINVENITEM* CTradePage::GetItem(int nPosX, int nPosY) const
{
	return m_pItemSlotArray[nPosY]->GetItemImage(nPosX);
}

void CTradePage::UnLoadItem ( int nPosX, int nPosY )
{
	m_pItemSlotArray[nPosY]->ResetItemImage ( nPosX );
}

void CTradePage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
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

void CTradePage::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void CTradePage::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}

void CTradePage::SetAllFlipItem ( BOOL bFlip )
{
	for ( int y = 0; y < ETradeY; y++ )
	{
		for ( int x = 0; x < ETradeX; x++ )
		{			
			if ( bFlip )
			{
				const SINVENITEM* pRefInvenItem = m_pItemSlotArray[y]->GetItemImage ( x );
				if (pRefInvenItem && pRefInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL() )
				{
					m_pItemSlotArray[y]->SetFlipItem ( x, TRUE );
				}
			}
			else
			{
				m_pItemSlotArray[y]->SetFlipItem ( x, FALSE );
			}
		}
	}
}

void CTradePage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetItemIndex ( nOUTOFRANGE, nOUTOFRANGE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}
