#include "StdAfx.h"

#include ".\vngainsyspage.h"
#include "../Item/ItemSlot.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/Item/GLItemMan.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CVNGainInvenPage::nSTARTLINE = 0;
const int CVNGainInvenPage::nOUTOFRANGE = -1;

CVNGainInvenPage::CVNGainInvenPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    //: m_pScrollBar ( NULL )
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_nCurPos(-1)
{
	memset( m_pItemSlotArray, 0, sizeof( m_pItemSlotArray ) );
	memset( m_pItemSlotArrayDummy, 0, sizeof( m_pItemSlotArrayDummy ) );
}

CVNGainInvenPage::~CVNGainInvenPage ()
{
}

CUIControl*	CVNGainInvenPage::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void CVNGainInvenPage::CreateSubControl ()
{
	static CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"VNGAINSYS_ITEM_SLOT0",
		"VNGAINSYS_ITEM_SLOT1",
		"VNGAINSYS_ITEM_SLOT2",
		"VNGAINSYS_ITEM_SLOT3",
		"VNGAINSYS_ITEM_SLOT4",
		"VNGAINSYS_ITEM_SLOT5",
		"VNGAINSYS_ITEM_SLOT6",
		"VNGAINSYS_ITEM_SLOT7",
		"VNGAINSYS_ITEM_SLOT8",
		"VNGAINSYS_ITEM_SLOT9"
	};

	//	더미 슬롯
	for ( int i = 0; i < nMAX_ONE_VIEW_SLOT; i++ )
	{
		m_pItemSlotArrayDummy[i] = CreateControl ( strSlotKeyword[i].GetString () );
	}

	//	실제 데이타
	for ( int i = 0; i < VNGAINSYS_INVEN_Y; i++ )
	{
		m_pItemSlotArray[i] = CreateItemSlot ( strSlotKeyword[0], ITEM_SLOT0 + i );
	}

	SetItemSlotRender ( nSTARTLINE, nMAX_ONE_VIEW_SLOT );
}

void CVNGainInvenPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetItemIndex ( nOUTOFRANGE, nOUTOFRANGE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CVNGainInvenPage::UpdateBLOCK ()
{
}

void CVNGainInvenPage::ResetAllItemSlotRender ( int nTotal )
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

void CVNGainInvenPage::SetItemSlotRender ( int nStartIndex, int nTotal )
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

CItemSlot* CVNGainInvenPage::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlot* pItemSlot = new CItemSlot ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( VNGAINSYS_INVEN_X, TRUE );
	RegisterControl ( pItemSlot );

	return pItemSlot;
}

void CVNGainInvenPage::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlotArray[nPosY]->SetItemImage ( nPosX, ref_InvenItem );
}

void CVNGainInvenPage::LoadItemPage ( GLInventory &ItemData )
{
	const GLInventory::CELL_MAP *pItemList = ItemData.GetItemList();

	if ( pItemList->size () > (VNGAINSYS_INVEN_Y * VNGAINSYS_INVEN_X) )
	{
		GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
		return ;
	}

	{	//	삭제

		GLInventory::CELL_MAP_CITER iter = pItemList->begin();
		GLInventory::CELL_MAP_CITER iter_end = pItemList->end();

		for ( WORD y = 0; y < VNGAINSYS_INVEN_Y; y++ )
		{
			for ( WORD x = 0; x < VNGAINSYS_INVEN_X; x++ )
			{
				const SINVENITEM* pRefInvenItem = GetItem(x, y);
				if (pRefInvenItem && (pRefInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()))
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

	//	등록
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

void CVNGainInvenPage::UnLoadItemPage ()
{
	for ( int y = 0; y < VNGAINSYS_INVEN_Y; y++ )
	{
		for ( int x = 0; x < VNGAINSYS_INVEN_X; x++ )
		{
			UnLoadItem ( x, y );
		}
	}
}

const SINVENITEM* CVNGainInvenPage::GetItem(int nPosX, int nPosY) const
{
	return m_pItemSlotArray[nPosY]->GetItemImage ( nPosX );
}

void CVNGainInvenPage::UnLoadItem ( int nPosX, int nPosY )
{
	m_pItemSlotArray[nPosY]->ResetItemImage ( nPosX );
}

void CVNGainInvenPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
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

void CVNGainInvenPage::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void CVNGainInvenPage::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}