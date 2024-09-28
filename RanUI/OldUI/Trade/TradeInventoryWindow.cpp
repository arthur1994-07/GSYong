#include "StdAfx.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

//#include "../../ModalCallerID.h"
#include "../../InnerInterface.h"
#include "TradeInventoryWindow.h"
#include "TradeInventoryPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTradeInventoryWindow::CTradeInventoryWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
	CUIWindowEx( pInterface, pEngineDevice ),
    m_pGaeaClient(pGaeaClient),
	m_pPage( NULL ),
	m_pMoneyTextBox( NULL ),
	m_nONE_VIEW_SLOT( 4 )
{
}

CTradeInventoryWindow::~CTradeInventoryWindow()
{
}

void CTradeInventoryWindow::CreateSubControl()
{
	CTradeInventoryPage* pTradeInventoryPage = new CTradeInventoryPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pTradeInventoryPage->CreateSub( this, "TRADEINVENTORY_PAGE", UI_FLAG_YSIZE, TRADEINVENTORY_PAGE );
	pTradeInventoryPage->CreateSubControl();
	RegisterControl( pTradeInventoryPage );
	m_pPage = pTradeInventoryPage;

	CUIControl* pControl = CreateControl( "TRADEINVENTORY_MONEY_BACK" );
	pControl->SetAlignFlag( UI_FLAG_BOTTOM );

	CBasicButton* pButton = CreateFlipButton( "TRADEINVENTORY_MONEY_BUTTON", "TRADEINVENTORY_MONEY_BUTTON_F", TRADEINVENTORY_MONEY_BUTTON, CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver( "TRADEINVENTORY_MONEY_BUTTON_F" );
	pButton->SetUseDynamic( TRUE );
	pButton->SetAlignFlag( UI_FLAG_BOTTOM );
//	CreateFlipButton( "TRADEINVENTORY_PLUS_MONEY_BUTTON", "TRADEINVENTORY_PLUS_MONEY_BUTTON_F", TRADEINVENTORY_PLUS_MONEY_BUTTON, CBasicButton::CLICK_FLIP );
//	CreateFlipButton( "TRADEINVENTORY_MINUS_MONEY_BUTTON", "TRADEINVENTORY_MINUS_MONEY_BUTTON_F", TRADEINVENTORY_MINUS_MONEY_BUTTON, CBasicButton::CLICK_FLIP );

	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pMoneyTextBox = CreateStaticControl( "TRADEINVENTORY_MONEYTEXT", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMoneyTextBox->SetAlignFlag( UI_FLAG_BOTTOM );
}

void CTradeInventoryWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_bTRADE_MODE = (m_pInterface->UiIsVisibleGroup(TRADE_WINDOW))?true:false;

	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_bTRADE_MODE )
	{
		GLInventory& ref_ItemData = m_pGaeaClient->GetTradeClient()->GetMyTradeBox();
		m_pPage->UpdateFlipItem( ref_ItemData );
	}
	else
	{
		m_pPage->ResetAllFlipItem ();
//		GLInventory& ref_ItemData = m_pGaeaClient->GetCharacter()->m_sPMarket.GetInven();
//		m_pPage->UpdateFlipItem( ref_ItemData );
	}

	GLCHARLOGIC& ref_CharData = m_pGaeaClient->GetCharacterLogic();
	SetMoney( ref_CharData.GetInvenMoney() );

	GLInventory& ref_Inventory = ref_CharData.m_cInventory;	
	m_pPage->LoadItemPage( ref_Inventory );
}

void CTradeInventoryWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	//	의도적으로 윈도우 이동을 막음
//	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case TRADEINVENTORY_PAGE:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				int nPosX, nPosY;
				m_pPage->GetItemIndex( &nPosX, &nPosY );

				CDebugSet::ToView( 1, 1, "인벤 : %d %d", nPosX, nPosY );				

				if ( nPosX < 0 || nPosY < 0 ) return;

//				if ( CUIMan::GetFocusControl() == this )
//				{
					const SINVENITEM* pInvenItem = m_pPage->GetItem ( nPosX, nPosY );
					if (pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()))
					{
						m_pInterface->SHOW_ITEM_INFO(
							pInvenItem->sItemCustom,
							FALSE, FALSE, FALSE, TRUE, FALSE,
							pInvenItem->wPosX, pInvenItem->wPosY );
					}
//				}

				if ( dwMsg & UIMSG_LB_UP )
				{   
					if ( m_bTRADE_MODE )	//	거래모드
					{
						m_pGaeaClient->GetCharacter()->ReqInvenTo( nPosX, nPosY );
					}
					else					//	상점모드
					{
						if (m_bOPENER && pInvenItem)
						{
							GLPrivateMarket &sPMarket = m_pGaeaClient->GetCharacter()->m_sPMarket;
							if (sPMarket.IsOpen())
                                return;

							if (pInvenItem->sItemCustom.GetNativeID() == NATIVEID_NULL())
                                return;
							const bool bCOUNTABLE = IsTurnITEM (pInvenItem->sItemCustom);
							WORD wPosX = static_cast<WORD>(nPosX);
							WORD wPosY = static_cast<WORD>(nPosY);

							m_pInterface->MODAL_PRIVATE_MARKET_SELLITEM( bCOUNTABLE, wPosX, wPosY );
						}
						else
						{
//							m_pGaeaClient->GetCharacter()->ReqInvenTo( nPosX, nPosY );
						}
					}
				}
				if ( dwMsg & UIMSG_RB_UP )
				{
//					m_pGaeaClient->GetCharacter()->ReqInvenDrug( nPosX, nPosY );
				}
			}
		}
		break;

	case TRADEINVENTORY_MONEY_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				//	거래(TRADE)일 경우에만 모달이 뜬다.
				if ( m_bTRADE_MODE )
				{
					if ( m_pGaeaClient->GetTradeClient()->GetMyTrade().IsLock() )
					{	// 거래 창 잠겨 있으면 돈 올릴 수 없음. by luxes.
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_LOCK_MESSAGE") );
					}
					else
					{
						m_pInterface->DoModal( ID2GAMEINTEXT("TRADE_MONEY"), UI::MODAL_INPUT, UI::EDITBOX_MONEY, UI::MODAL_TRADEMONEY );
					}
				}
			}
		}
		break;

	case ET_CONTROL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{				
				if ( m_bTRADE_MODE )	m_pInterface->SetTradeWindowCloseReq();
				else
				{
					bool bOPENER;
					DWORD dwGaeaID;
					m_pInterface->GetPrivateMarketInfo( bOPENER, dwGaeaID );
					if ( bOPENER )
					{
						m_pInterface->DoModal( ID2GAMEINTEXT("PRIVATE_MARKET_QUIT"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_PRIVATE_MARKET_QUIT );
					}
					else
					{
						m_pInterface->SetPrivateMarketClose();
					}
				}
			}
		}
		break;
	}
}


void CTradeInventoryWindow::SetMoney( LONGLONG Money )
{
	//	돈자리수
	CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( Money, 3, "," );
	m_pMoneyTextBox->SetOneLineText( strTemp );
}

HRESULT CTradeInventoryWindow::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIWindowEx::RestoreDeviceObjects( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = m_pInterface->UiGetResolution();
	WORD X_RES = HIWORD( lResolution );
	WORD Y_RES = LOWORD( lResolution );

	const UIRECT& rcWindow = GetGlobalPos();

	D3DXVECTOR2 vPos;
	vPos.x = ((X_RES) / 2.0f);
	vPos.y = (Y_RES - rcWindow.sizeY) / 2.0f;
	SetGlobalPos( vPos );

	return S_OK;
}

bool CTradeInventoryWindow::IsTurnITEM (const SITEMCUSTOM& sItemCustom )
{
	const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
	if ( !pItemData )
	{
		GASSERT( 0 && "아이템 정보를 찾을 수 없습니다." );
		return false;
	}

	const WORD& wPileNum = pItemData->sDrugOp.wPileNum;
	if ( wPileNum > 1 )
	{
		const WORD& wTurnNum = sItemCustom.wTurnNum;        
		if ( wTurnNum > 1 )
            return true;
	}

	return false;
}

void	CTradeInventoryWindow::SetOPENER( bool bOPENER )
{
	m_bOPENER = bOPENER;
}

void	CTradeInventoryWindow::SetOneViewSlot( const int& nONE_VIEW_SLOT )
{
	CUIControl TempControl(m_pEngineDevice);
	TempControl.Create( 1, "TRADEINVENTORYSLOT_ITEM_SLOT0" );

	CUIControl TempControl2(m_pEngineDevice);
	TempControl2.Create( 2, "TRADEINVENTORYSLOT_ITEM_SLOT1" );

	const UIRECT& rcTempGlobalPos = TempControl.GetGlobalPos();
	D3DXVECTOR2 vONE_SLOT_SIZE( rcTempGlobalPos.sizeX, rcTempGlobalPos.sizeY );

	const UIRECT& rcTempGlobalPos2 = TempControl2.GetGlobalPos();

	const float fSlotGap = rcTempGlobalPos2.top - rcTempGlobalPos.bottom;

	const float fOldSizeY = (vONE_SLOT_SIZE.y+fSlotGap) * m_nONE_VIEW_SLOT;
	const float fNewSizeY = (vONE_SLOT_SIZE.y+fSlotGap) * nONE_VIEW_SLOT;
    const float fDIFF_SIZE_Y = fNewSizeY - fOldSizeY;

	const UIRECT& rcGlobalPos = GetGlobalPos();
	UIRECT rcNewGlobalPos( rcGlobalPos.left, rcGlobalPos.top,
		rcGlobalPos.sizeX, rcGlobalPos.sizeY + fDIFF_SIZE_Y );

	WORD wFlagAlignBACK = GetAlignFlag();
	SetAlignFlag( UI_FLAG_YSIZE );
	
    AlignSubControl( rcGlobalPos, rcNewGlobalPos );
	SetGlobalPos( rcNewGlobalPos );
	
	SetAlignFlag( wFlagAlignBACK );

	m_pPage->SetOneViewSlot( nONE_VIEW_SLOT );
	m_nONE_VIEW_SLOT = nONE_VIEW_SLOT;
}

const int& CTradeInventoryWindow::GetOneViewSlot() const
{
	return m_nONE_VIEW_SLOT;
}

MyTradeInventoryWindow::MyTradeInventoryWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CTradeInventoryWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyTradeInventoryWindow::CreateUIWindowAndRegisterOwnership()
{
   CTradeInventoryWindow::Create ( TRADEINVENTORY_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
   CTradeInventoryWindow::CreateBaseWindowLightGray ( "TRADEINVENTORY_WINDOW", (char*)ID2GAMEWORD("TRADEINVENTORY_NAME_STATIC") );
   CTradeInventoryWindow::CreateSubControl ();
   CTradeInventoryWindow::SetAlignFlag ( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
   CTradeInventoryWindow::m_pInterface->UiRegisterControl ( this, true );
   CTradeInventoryWindow::m_pInterface->UiShowGroupFocus ( TRADEINVENTORY_WINDOW );
   CTradeInventoryWindow::m_pInterface->UiHideGroup ( TRADEINVENTORY_WINDOW );
}

const UIRECT& MyTradeInventoryWindow::GetUIWindowGlobalPos()
{
    return CTradeInventoryWindow::GetGlobalPos();
}

void MyTradeInventoryWindow::SetUIWindowGlobalPos( const D3DXVECTOR2& vPos )
{
    CTradeInventoryWindow::SetGlobalPos( vPos );
}

void MyTradeInventoryWindow::SetOneViewSlot( const int& nONE_VIEW_SLOT )
{
    CTradeInventoryWindow::SetOneViewSlot( nONE_VIEW_SLOT );
}

void MyTradeInventoryWindow::SetOpener( bool bOPENER )
{
    CTradeInventoryWindow::SetOPENER( bOPENER );
}
