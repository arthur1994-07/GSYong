#include "StdAfx.h"
#include "./RnInventoryWindow.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"

//#include "../../../RanLogicClient/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"

#include "../../InnerInterface.h"

#include "./RnPageInven.h"
#include "./RnPageBank.h"
#include "./RnPageWish.h"

#include "../../../SigmaCore/DebugInclude.h"

CRnInventoryWindow::CRnInventoryWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx( pInterface, pEngineDevice )

	, m_pGaeaClient( pGaeaClient )

	, m_pCurrent( NULL )
	, m_pInven( NULL )
	, m_pBank( NULL )
	, m_pWish( NULL )
{
}

void CRnInventoryWindow::CreateUIWindowAndRegisterOwnership()
{
	Create( INVENTORY_WINDOW_RENEW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CreateSubControl();
    SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
    m_pInterface->UiRegisterControl( this, true );
    m_pInterface->UiShowGroupFocus( GetWndID() );
}

void CRnInventoryWindow::CreateSubControl()
{
	CreateBaseWindowLightGray( "INVENTORY_WINDOW_RENEW", ID2GAMEWORD( "INVENTORY_NAME_STATIC" ) );
	CreateLineBox( "INVENTORY_WINDOW_REGION", "CHARACTER_WINDOW_REGION_TEXTUREINFO" );

	{
		STAPBUTTON* pTab = new STAPBUTTON( m_pEngineDevice );
		pTab->CreateSub( this, "INVENTORY_WINDOW_INVEN_TAP", UI_FLAG_DEFAULT, ETapInven );
		pTab->CreateSubControl(
			ID2GAMEWORD( "INVENTORY_TAP_NAME", 0 ),
			"INVENTORY_WINDOW_TAP_BASE", "CHARACTER_WINDOW_TAP_ACTIVE",
			"INVENTORY_WINDOW_TAP_BASE", "CHARACTER_WINDOW_TAP_INACTIVE",
			"INVENTORY_WINDOW_TAP_BASE" );
		RegisterControl( pTab );
		RegisterTapButton( pTab );
	}

	{
		STAPBUTTON* pTab = new STAPBUTTON( m_pEngineDevice );
		pTab->CreateSub ( this, "INVENTORY_WINDOW_BANK_TAP", UI_FLAG_DEFAULT, ETapBank );
		pTab->CreateSubControl(
			ID2GAMEWORD( "INVENTORY_TAP_NAME", 1 ),
			"INVENTORY_WINDOW_TAP_BASE", "CHARACTER_WINDOW_TAP_ACTIVE",
			"INVENTORY_WINDOW_TAP_BASE", "CHARACTER_WINDOW_TAP_INACTIVE",
			"INVENTORY_WINDOW_TAP_BASE");
		RegisterControl( pTab );
		RegisterTapButton( pTab );
	}

	{
		STAPBUTTON* pTab = new STAPBUTTON( m_pEngineDevice );
		pTab->CreateSub ( this, "INVENTORY_WINDOW_WISH_TAP", UI_FLAG_DEFAULT, ETapWish );
		pTab->CreateSubControl(
			ID2GAMEWORD( "INVENTORY_TAP_NAME", 2 ),
			"INVENTORY_WINDOW_TAP_BASE", "CHARACTER_WINDOW_TAP_ACTIVE",
			"INVENTORY_WINDOW_TAP_BASE", "CHARACTER_WINDOW_TAP_INACTIVE",
			"INVENTORY_WINDOW_TAP_BASE");
		RegisterControl( pTab );
		RegisterTapButton( pTab );
	}

	m_pInven = new CRnPageInven( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pInven->CreateSub( this, "INVENTORY_WINDOW_REGION", UI_FLAG_DEFAULT, EPageInven );
	m_pInven->CreateSubControl();
	m_pInven->SetVisibleSingle( FALSE );
	RegisterControl( m_pInven );

	m_pBank = new CRnPageBank( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pBank->CreateSub( this, "INVENTORY_WINDOW_REGION", UI_FLAG_DEFAULT, EPageBank );
	m_pBank->CreateSubControl();
	m_pBank->SetVisibleSingle( FALSE );
	RegisterControl( m_pBank );

	m_pWish = new CRnPageWish( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pWish->CreateSub( this, "INVENTORY_WINDOW_REGION", UI_FLAG_DEFAULT, EPageWish );
	m_pWish->CreateSubControl();
	m_pWish->SetVisibleSingle( FALSE );
	m_pWish->SetSlotExchange(0,0,true);
	RegisterControl( m_pWish );

	EventSelectedTap( ETapInven );
}

void CRnInventoryWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	ResetBorderItemSlot();
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CRnInventoryWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if( ( dwMsg & UIMSG_LB_DUP ) && CHECK_MOUSE_IN( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( INVENTORY_WINDOW_RENEW );
			}
		}
		break;

	case ET_CONTROL_BUTTON:
		if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
            if( m_pInterface->UiIsVisibleGroup( STORAGE_WINDOW ) )
                m_pInterface->SetStorageWindowClose();

            if( m_pInterface->UiIsVisibleGroup( CLUB_STORAGE_WINDOW ) )
                m_pInterface->SetClubStorageWindowClose();

			if( m_pInterface->UiIsVisibleGroup( TRADE_WINDOW ) )
				m_pInterface->SetTradeWindowCloseReq();

			if( m_pInterface->UiIsVisibleGroup( MARKET_WINDOW ) )
                m_pInterface->SetMarketWindowClose();
		}
		break;
	}
}

void CRnInventoryWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	if( !bVisible )
    {
		m_pInterface->GetInvenItemTooltipGroup()->CloseItemToolTipMultiAll();
        m_pInterface->GetWishItemTooltipGroup()->CloseItemToolTipMultiAll();
    }

    if( m_pCurrent )
        m_pCurrent->SetVisibleSingle( bVisible );
}

void CRnInventoryWindow::CallTapSelect( UIGUID controlID )
{
	TapSelect( controlID );
}

UIGUID CRnInventoryWindow::GetTapSelected()
{
	return GetTapID();
}

const UIRECT& CRnInventoryWindow::GetUIWindowGlobalPos()
{
	return GetGlobalPos();
}

const UIRECT& CRnInventoryWindow::GetInventoryPageGlobalPos()
{
	return m_pInven->GetGlobalPos();
}

void CRnInventoryWindow::LockTrade()
{
	m_pInven->SetLock( true );
}

void CRnInventoryWindow::UnLockTrade()
{
	m_pInven->SetLock( false );
}

void CRnInventoryWindow::LockInven( UIGUID ControlID )
{
	m_pInven->SetLock( ControlID, true );
}

void CRnInventoryWindow::UnLockInven( UIGUID ControlID )
{
	m_pInven->SetLock( ControlID, false );
}

const bool CRnInventoryWindow::IsLocked()
{
	return m_pInven->IsLock();
}

const SINVENITEM* CRnInventoryWindow::GetFocusItem()
{
	if( m_pCurrent )
		return m_pCurrent->GetFocusItem();

	return NULL;
}

void CRnInventoryWindow::GetConsumeSplitInvenPos( WORD& nPosX, WORD& nPosY )
{
	m_pInven->GetConsumeSplitInvenPos( nPosX, nPosY );
}

bool CRnInventoryWindow::IsCurrentInvenPage( int& nX, int& nY )
{
	if ( m_pInven )
	{
		int nPage = 0;
		return m_pInven->IsCurrentPage( nX, nY );
	}

	return false;
}

void CRnInventoryWindow::ChangeInvenPage( int& nX, int& nY )
{
	if ( m_pInven )
	{
		int nPage = 0;
		m_pInven->LogicPosToUIPos( nX, nY, nPage );
		m_pInven->ChangePage( nPage );
	}
}

void CRnInventoryWindow::SetBorderItemSlot( int nX, int nY )
{
	if ( m_pCurrent )
	{
		m_pCurrent->SetBorderItemSlot( nX, nY );
	}
}

void CRnInventoryWindow::SetWindowOpenBox( int nX, int nY, BOOL bShow )
{
	if ( m_pCurrent )
	{
		m_pCurrent->SetWindowOpenBox(nX, nY, bShow );
	}
}

void CRnInventoryWindow::ResetBorderItemSlot()
{
	if ( m_pCurrent )
	{
		m_pCurrent->ResetBorderItemSlot();
	}
}

bool CRnInventoryWindow::IsItemDecompose()
{
	if ( m_pCurrent->GetWndID() == EPageInven )
	{
		return m_pInven->IsItemDecompose();
	}
	return false;
}

void CRnInventoryWindow::GetItemDecomposePos(WORD& nPosX, WORD& nPosY)
{
	if ( m_pCurrent->GetWndID() == EPageInven )
	{
		m_pInven->GetConsumeSplitInvenPos(nPosX, nPosY);
	}
	else
	{
		nPosX = -1;
		nPosY = -1;
	}
}

void CRnInventoryWindow::EventSelectedTap( UIGUID controlID )
{
	if( m_pCurrent )
		m_pCurrent->SetVisibleSingle( FALSE );

	switch( controlID )
	{
	case ETapInven:
		m_pCurrent = m_pInven;
		break;

	case ETapBank:
		m_pCurrent = m_pBank;
		break;

	case ETapWish:
		m_pCurrent = m_pWish;
		break;
	}

	if( m_pCurrent )
		m_pCurrent->SetVisibleSingle( TRUE );
}

void CRnInventoryWindow::SetFocusControl()
{
	CUIWindow::SetFocusControl();

	if( IsLocked() )
	{
		UIGUID controlID = m_pInven->GetToLockID();
		if( controlID == NO_ID )
			return;

		m_pInterface->UiShowGroupFocus( controlID );
	}
}
