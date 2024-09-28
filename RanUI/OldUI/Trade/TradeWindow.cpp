#include "StdAfx.h"
#include "TradeWindow.h"
#include "TradeControlPage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../InnerInterface.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTradeWindow::CTradeWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
	CUIWindowEx( pInterface, pEngineDevice ),
    m_pGaeaClient(pGaeaClient),
	m_pTarPage( NULL ),
	m_pMyPgae( NULL )
{
}

CTradeWindow::~CTradeWindow()
{
}

void CTradeWindow::CreateSubControl()
{
	m_pTarPage = CreateTradeControlPage( "TRADE_CONTROL_TARPAGE", FALSE, TAR_TRADE_PAGE );
	CreateControl( "TRADE_BLACK_LINE" );
	m_pMyPgae = CreateTradeControlPage( "TRADE_CONTROL_MYPAGE", TRUE, MY_TRADE_PAGE );
}

CTradeControlPage*	CTradeWindow::CreateTradeControlPage( char *szControl, BOOL bMyPage, UIGUID ControlID )
{
	CTradeControlPage* pTradeControlPage = new CTradeControlPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pTradeControlPage->CreateSub( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pTradeControlPage->CreateSubControl( bMyPage );
	RegisterControl( pTradeControlPage );
	return pTradeControlPage;
}

void CTradeWindow::SetTradeInfo( CString strPlayerName, DWORD dwMasterID )
{		
	m_dwMasterID = dwMasterID;
	m_strPlayerName = strPlayerName.GetString();

	if ( m_pMyPgae ) 
	{
		CString strMyName = m_pGaeaClient->GetCharacter()->m_szName;
		m_pMyPgae->SetPlayerName( strMyName );
	}
	if ( m_pTarPage )
	{
		m_pTarPage->SetPlayerName( strPlayerName );
	}
}

DWORD CTradeWindow::GetMasterID ()
{
	return m_dwMasterID;
}

void CTradeWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	//	의도적으로 윈도우 이동을 막음
//	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
	switch ( ControlID )
	{
	case ET_CONTROL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->SetTradeWindowCloseReq();
			}
		}
		break;

	case MY_TRADE_PAGE:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				int nPosX, nPosY;
				m_pMyPgae->GetItemIndex( &nPosX, &nPosY );

				if ( nPosX < 0 || nPosY < 0 ) return ;
//				if ( CUIMan::GetFocusControl () == this )
				{

					const SINVENITEM* pInvenItem = m_pMyPgae->GetItem ( nPosX, nPosY );
					if (pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()))
					{
						m_pInterface->SHOW_ITEM_INFO(
							pInvenItem->sItemCustom,
							FALSE, FALSE, FALSE, TRUE, FALSE,
							pInvenItem->wPosX, pInvenItem->wPosY );
					}
				}
			}
		}
		break;

	case TAR_TRADE_PAGE:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				int nPosX, nPosY;
				m_pTarPage->GetItemIndex( &nPosX, &nPosY );

				if ( nPosX < 0 || nPosY < 0 )
					return ;

				const SINVENITEM* pInvenItem = m_pTarPage->GetItem ( nPosX, nPosY );
				if ( !pInvenItem )
					return;
				
				if( pInvenItem->sItemCustom.GetNativeID() == NATIVEID_NULL() )
					return;

				m_pInterface->SHOW_ITEM_INFO(
					pInvenItem->sItemCustom,
					FALSE, FALSE, FALSE, TRUE, FALSE,
					pInvenItem->wPosX, pInvenItem->wPosY );

				if( dwMsg & UIMSG_LB_UP )
				{
					if( DxInputDevice::GetInstance().GetKeyState( DIK_LSHIFT ) & DXKEY_PRESSED )
					{
						WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
						if( pWishList )
							pWishList->NewTradeItem( pInvenItem->sItemCustom, m_strPlayerName );
						return;
					}
				}
			}
		}
		break;
	}
}

HRESULT CTradeWindow::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIWindowEx::RestoreDeviceObjects( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const UIRECT& rcWindow = GetGlobalPos();

	const long lResolution = m_pInterface->UiGetResolution();
	WORD X_RES = HIWORD( lResolution );
	WORD Y_RES = LOWORD( lResolution );

	D3DXVECTOR2 vPos;
	vPos.x = ((X_RES) / 2.0f) - rcWindow.sizeX;
	vPos.y = (Y_RES - rcWindow.sizeY) / 2.0f;
	SetGlobalPos( vPos );

	return S_OK;
}

MyTradeWindow::MyTradeWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CTradeWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyTradeWindow::CreateUIWindowAndRegisterOwnership()
{
    CTradeWindow::Create( TRADE_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CTradeWindow::CreateBaseWindowLightGray( "TRADE_WINDOW", (char*)ID2GAMEWORD("TRADE_NAME_STATIC") );
    CTradeWindow::CreateSubControl();
    CTradeWindow::SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
    CTradeWindow::m_pInterface->UiRegisterControl( this, true );
    CTradeWindow::m_pInterface->UiShowGroupFocus( TRADE_WINDOW );
}

void MyTradeWindow::SetTradeInfo( UI::String strPlayerName, DWORD dwMasterID )
{
    CTradeWindow::SetTradeInfo( UI::ToString( strPlayerName ), dwMasterID );
}

const UIRECT& MyTradeWindow::GetUIWindowGlobalPos()
{
    return CTradeWindow::GetGlobalPos();
}

void MyTradeWindow::SetUIWindowGlobalPos( const D3DXVECTOR2& vPos )
{
    CTradeWindow::SetGlobalPos( vPos );
}

DWORD MyTradeWindow::GetMasterID()
{
    return CTradeWindow::GetMasterID();
}
