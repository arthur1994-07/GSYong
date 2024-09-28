#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../RanLogic/Item/GLItemMan.h"

#include "../../../RanLogicClient/Club/GLClubClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "../Item/ItemSlot.h"
#include "StoragePage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CStoragePage::nSTARTLINE = 0;
const int CStoragePage::nOUTOFRANGE = -1;

CStoragePage::CStoragePage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pInterface(pInterface)
    , m_pScrollBar(NULL)
    , m_nCurPos(-1)
{
	memset ( m_pItemSlotArray, 0, sizeof ( CItemSlot* ) * EStorageY );
	memset ( m_pItemSlotArrayDummy, 0, sizeof ( CUIControl* ) * nMAX_ONE_VIEW_SLOT );
}

CStoragePage::~CStoragePage ()
{
}

CUIControl*	CStoragePage::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void CStoragePage::CreateSubControl ()
{
	CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"STORAGE_ITEM_SLOT0",
		"STORAGE_ITEM_SLOT1",
		"STORAGE_ITEM_SLOT2",
		"STORAGE_ITEM_SLOT3",
		"STORAGE_ITEM_SLOT4",
		"STORAGE_ITEM_SLOT5",
		"STORAGE_ITEM_SLOT6",
		"STORAGE_ITEM_SLOT7",
	};

	{	//	더미 슬롯
		for ( int i = 0; i < nMAX_ONE_VIEW_SLOT; i++ )
		{
			m_pItemSlotArrayDummy[i] = CreateControl ( strSlotKeyword[i].GetString () );
		}
	}

	const int nExampleCount = EStorageY;
	{	//	실제 데이타
		for ( int i = 0; i < nExampleCount; i++ )
		{
			m_pItemSlotArray[i] = CreateItemSlot ( strSlotKeyword[0], ITEM_SLOT0 + i );
		}
	}

	{	//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
		pScrollBar->CreateBaseScrollBar ( "STORAGE_PAGE_SCROLL" );
		pScrollBar->GetThumbFrame()->SetState ( nExampleCount, nMAX_ONE_VIEW_SLOT );
		RegisterControl ( pScrollBar );
		m_pScrollBar = pScrollBar;	
	}

	SetItemSlotRender ( nSTARTLINE, nMAX_ONE_VIEW_SLOT );
}

void CStoragePage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetItemIndex ( nOUTOFRANGE, nOUTOFRANGE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	GLCHARLOGIC& ref_CharData = m_pGaeaClient->GetCharacterLogic ();
	GLInventory& ref_Inventory = ref_CharData.m_cStorage[m_nPage];
	LoadItemPage ( ref_Inventory );

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

void CStoragePage::ResetAllItemSlotRender ( int nTotal )
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

void CStoragePage::SetItemSlotRender ( int nStartIndex, int nTotal )
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

CItemSlot* CStoragePage::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlot* pItemSlot = new CItemSlot(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( EStorageX, TRUE );
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

void CStoragePage::InitStoragePage ( int nPage )
{
	m_nPage = nPage;
}

void CStoragePage::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlotArray[nPosY]->SetItemImage ( nPosX, ref_InvenItem );
}

void CStoragePage::LoadItemPage ( GLInventory &ItemData, const bool bUSABLE )
{
	const GLInventory::CELL_MAP *pItemList = ItemData.GetItemList();
	
	if ( pItemList->size () > EStorageTotalSlot )
	{
		GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
		return ;
	}


	{	//	삭제

		GLInventory::CELL_MAP_CITER iter = pItemList->begin();
		GLInventory::CELL_MAP_CITER iter_end = pItemList->end();

		for ( WORD y = 0; y < EStorageY; y++ )
		{
			for ( WORD x = 0; x < EStorageX; x++ )
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


void CStoragePage::UnLoadItemPage ()
{
	for ( int y = 0; y < EStorageY; y++ )
	{
		for ( int x = 0; x < EStorageX; x++ )
		{
			UnLoadItem ( x, y );
		}
	}
}

const SINVENITEM* CStoragePage::GetItem(int nPosX, int nPosY) const
{
	return m_pItemSlotArray[nPosY]->GetItemImage ( nPosX );
}

void CStoragePage::UnLoadItem ( int nPosX, int nPosY )
{
	m_pItemSlotArray[nPosY]->ResetItemImage ( nPosX );
}


void CStoragePage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
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

void CStoragePage::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void CStoragePage::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}


/////////////////////////////////////////////////////////////////////////////////////

const int CClubStoragePage::nSTARTLINE = 0;
const int CClubStoragePage::nOUTOFRANGE = -1;

CClubStoragePage::CClubStoragePage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pInterface(pInterface)
    , m_pScrollBar(NULL)
    , m_nCurPos(-1)
{
	memset ( m_pItemSlotArray, 0, sizeof ( CItemSlot* ) * EStorageY );
	memset ( m_pItemSlotArrayDummy, 0, sizeof ( CUIControl* ) * nMAX_ONE_VIEW_SLOT );
}

CClubStoragePage::~CClubStoragePage ()
{
}

CUIControl*	CClubStoragePage::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void CClubStoragePage::CreateSubControl ()
{
	CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
	{
		"STORAGE_ITEM_SLOT0",
		"STORAGE_ITEM_SLOT1",
		"STORAGE_ITEM_SLOT2",
		"STORAGE_ITEM_SLOT3",
		"STORAGE_ITEM_SLOT4",
		"STORAGE_ITEM_SLOT5",
		"STORAGE_ITEM_SLOT6",
		"STORAGE_ITEM_SLOT7",
	};

	{	//	더미 슬롯
		for ( int i = 0; i < nMAX_ONE_VIEW_SLOT; i++ )
		{
			m_pItemSlotArrayDummy[i] = CreateControl ( strSlotKeyword[i].GetString () );
		}
	}

	const int nExampleCount = EStorageY;
	{	//	실제 데이타
		for ( int i = 0; i < nExampleCount; i++ )
		{
			m_pItemSlotArray[i] = CreateItemSlot ( strSlotKeyword[0], ITEM_SLOT0 + i );
		}
	}

	{	//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
		pScrollBar->CreateBaseScrollBar ( "STORAGE_PAGE_SCROLL" );
		pScrollBar->GetThumbFrame()->SetState ( nExampleCount, nMAX_ONE_VIEW_SLOT );
		RegisterControl ( pScrollBar );
		m_pScrollBar = pScrollBar;	
	}

	SetItemSlotRender ( nSTARTLINE, nMAX_ONE_VIEW_SLOT );
}

void CClubStoragePage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetItemIndex ( nOUTOFRANGE, nOUTOFRANGE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	GLInventory* pInvenPage = pMyClub->GetStorage(m_nPage);
    if (!pInvenPage)
        return;

	LoadItemPage(pInvenPage);

	if ( m_pScrollBar )
	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBar->GetThumbFrame ();
		int nTotal = pThumbFrame->GetTotal ();

		if ( nTotal <= nMAX_ONE_VIEW_SLOT )
            return;

		const int nViewPerPage = pThumbFrame->GetViewPerPage ();
		if ( nViewPerPage < nTotal )
		{
			int nCurPos = nSTARTLINE;
			const int nMovableLine = nTotal - nViewPerPage;
			float fPercent = pThumbFrame->GetPercent ();

			nCurPos = (int)floor(fPercent * nMovableLine);
			if ( nCurPos < nSTARTLINE )
                nCurPos = nSTARTLINE;

			if ( m_nCurPos == nCurPos )
                return;

			m_nCurPos = nCurPos;

			ResetAllItemSlotRender ( nTotal );
			SetItemSlotRender ( nCurPos, nCurPos + nViewPerPage );
		}
	}
}

void CClubStoragePage::ResetAllItemSlotRender ( int nTotal )
{
	if ( nTotal < 0 )
        return;

	for ( int i = 0; i < nTotal; i++ )
	{
		CItemSlot* pItemSlot = m_pItemSlotArray[i];
		if ( pItemSlot )
		{
			pItemSlot->SetVisibleSingle ( FALSE );
		}
	}
}

void CClubStoragePage::SetItemSlotRender ( int nStartIndex, int nTotal )
{
	if ( nTotal < 0 )
        return;

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

CItemSlot* CClubStoragePage::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlot* pItemSlot = new CItemSlot ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( EStorageX, TRUE );
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

void CClubStoragePage::InitStoragePage ( int nPage )
{
	m_nPage = nPage;
}

void CClubStoragePage::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlotArray[nPosY]->SetItemImage ( nPosX, ref_InvenItem );
}

void CClubStoragePage::LoadItemPage(GLInventory* pItemData, const bool bUSABLE)
{
	const GLInventory::CELL_MAP* pItemList = pItemData->GetItemList();
	
	if ( pItemList->size () > EStorageTotalSlot )
	{
		GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
		return ;
	}


	{	//	삭제

		GLInventory::CELL_MAP_CITER iter = pItemList->begin();
		GLInventory::CELL_MAP_CITER iter_end = pItemList->end();

		for ( WORD y = 0; y < EStorageY; y++ )
		{
			for ( WORD x = 0; x < EStorageX; x++ )
			{
				const SINVENITEM* pRefInvenItem = GetItem ( x, y );
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


void CClubStoragePage::UnLoadItemPage ()
{
	for ( int y = 0; y < EStorageY; y++ )
	{
		for ( int x = 0; x < EStorageX; x++ )
		{
			UnLoadItem ( x, y );
		}
	}
}

const SINVENITEM* CClubStoragePage::GetItem(int nPosX, int nPosY) const
{
	return m_pItemSlotArray[nPosY]->GetItemImage ( nPosX );
}

void CClubStoragePage::UnLoadItem ( int nPosX, int nPosY )
{
	m_pItemSlotArray[nPosY]->ResetItemImage ( nPosX );
}


void CClubStoragePage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
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

void CClubStoragePage::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void CClubStoragePage::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}
