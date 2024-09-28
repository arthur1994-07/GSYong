#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/StaticHoldItem.h"

#include "../../InnerInterface.h"
#include "../Display/NameDisplayMan.h"

#include "PrivateMarketWindow.h"
#include "PrivateMarketPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPrivateMarketWindow::CPrivateMarketWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
{
}

CPrivateMarketWindow::~CPrivateMarketWindow()
{
}

void	CPrivateMarketWindow::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList( "PRIVATE_MARKET_ADVERTISE_LINEBOX" );
	RegisterControl( pBasicLineBox );		

	m_pTextBox = new CBasicTextBox(m_pEngineDevice);
	m_pTextBox->CreateSub( this, "PRIVATE_MARKET_ADVERTISE_TEXTBOX" );
	m_pTextBox->SetFont( pFont );
	RegisterControl( m_pTextBox );

	m_pPage = new CPrivateMarketPage ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pPage->CreateSub( this, "PRIVATE_MARKET_PAGE", UI_FLAG_DEFAULT, PRIVATE_MARKET_PAGE );
	m_pPage->CreateSubControl();	
	RegisterControl( m_pPage );

	m_pSTART = CreateTextButton( "PRIVATE_MARKET_START", PRIVATE_MARKET_START, const_cast<char*>(ID2GAMEWORD("PRIVATE_MARKET_BUTTON",0)) );
	m_pSTOP = CreateTextButton( "PRIVATE_MARKET_STOP", PRIVATE_MARKET_STOP, const_cast<char*>(ID2GAMEWORD("PRIVATE_MARKET_BUTTON",1)) );
}

void CPrivateMarketWindow::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
	//	의도적으로 윈도우 이동을 막음
	// CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case PRIVATE_MARKET_START:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if ( m_bOPENER )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("PRIVATE_MARKET_ONLY_UI"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_PRIVATE_MARKET_ONLY_UI );
				}
				else
				{
					m_pInterface->SetPrivateMarketClose();
				}

				//	시작
				//m_pGaeaClient->GetCharacter()->ReqPMarketOpen();
			}
		}
		break;

	case PRIVATE_MARKET_PAGE:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				int nPosX, nPosY;
				m_pPage->GetItemIndex( &nPosX, &nPosY );

				//CDebugSet::ToView ( 1, 3, "[private market] Page:%d %d / %d", nPosX, nPosY );

				if ( nPosX < 0 || nPosY < 0 ) return ;

				SetItemIndex( nPosX, nPosY );

				const SINVENITEM* pInvenItem = m_pPage->GetItem(nPosX, nPosY);
				if (pInvenItem && pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL() )
				{
					m_pInterface->SHOW_ITEM_INFO(
						pInvenItem->sItemCustom,
						FALSE, FALSE, TRUE, TRUE, FALSE,
						pInvenItem->wPosX, pInvenItem->wPosY );
				}

				if ( dwMsg & UIMSG_LB_UP )
				{
					WORD wPosX = static_cast<WORD>(nPosX);
					WORD wPosY = static_cast<WORD>(nPosY);

					if (m_bOPENER && pInvenItem)
					{
						GLPrivateMarket &sPMarket = m_pGaeaClient->GetCharacter()->m_sPMarket;
						if (sPMarket.IsOpen())
                            return ;

						if (pInvenItem->sItemCustom.GetNativeID() == NATIVEID_NULL())
                            return ;

						m_pGaeaClient->GetCharacter()->ReqPMarketDisItem( wPosX, wPosY );
					}
					else
					{
						//	이미 다 팔린 상태라면 사기를 진행하지 않음
						if ( m_pPage->IsFlipItem( wPosX, wPosY ) )
							return ;

						//	NOTE
						//		아이템 바로 사기
						//if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_PRESSED )
						//{
						//	if ( dwMsg & UIMSG_LB_UP )
						//	{
						//		SNATIVEID sNativeID = m_pGaeaClient->GetCharacter()->GET_HOLD_ITEM().sNativeID;
						//		if ( sNativeID != NATIVEID_NULL () ) return ;

						//		m_pGaeaClient->GetCharacter()->ReqPMarketBuy ( m_dwGaeaID, nPosX, nPosY, 1 );								
						//		return ;
						//	}
						//}

                        if ( holditem::HaveHoldItem() || !pInvenItem) 
                            return;

						const SITEM* pITEM = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
						if ( !pITEM )
							return;

						if( DxInputDevice::GetInstance().GetKeyState( DIK_LSHIFT ) & DXKEY_PRESSED )
						{
							std::tr1::shared_ptr<GLCharClient> pClient = m_pGaeaClient->GetChar( m_dwGaeaID );
							if( !pClient )
								return;

							SNATIVEID sSalePos( wPosX, wPosY );
							const SSALEITEM* pSaleItem = pClient->m_sPMarket.GetItem( sSalePos );
							if( !pSaleItem )
								return;

							WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
							if( !pWishList )
								return;

							pWishList->NewPrivatePersonItem(
								pInvenItem->sItemCustom,
								pSaleItem->llPRICE,
								std::string( pClient->GetName() ) );

							return;
						}

						//	Note : 겹침이 가능하고 한개씩 파는 아이템의 경우 구입 수량 입력. ( 화살묶음, 부적묶음. )
						if ( pITEM->ISPILE() && (pITEM->GETAPPLYNUM()==1) )
						{
							m_pInterface->ModalWindowSetModalData( nPosX, nPosY );
							m_pInterface->DoModal( ID2GAMEINTEXT( "BUY_PILE_ITEM" ), UI::MODAL_INPUT, UI::EDITBOX_NUMBER, UI::MODAL_PRIVATE_MARKET_PILE );
							return;
						}
						else
						{
							//	NOTE
							//		물건 사기
							const std::string& strItemName = pInvenItem->sItemCustom.GETNAME();
							m_pInterface->ModalWindowSetModalData( nPosX, nPosY );
							m_pInterface->DoModal( sc::string::format( ID2GAMEINTEXT("BUY_ITEM_CONFIRM"), strItemName.c_str() ), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_PRIVATE_MARKET_BUY );							
							return ;
						}

						//	DoModal ( "BUYING" );
					}

					return ;
				}
			}
		}
		break;

	case ET_CONTROL_BUTTON:
	case PRIVATE_MARKET_STOP:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{				
				if ( m_bOPENER )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("PRIVATE_MARKET_QUIT"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_PRIVATE_MARKET_QUIT );
				}
				else
				{
					m_pInterface->SetPrivateMarketClose();
				}
			}
		}
		break;
	}
}

CBasicTextButton* CPrivateMarketWindow::CreateTextButton( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl( pTextButton );
	return pTextButton;
}

void CPrivateMarketWindow::InitPrivateMarket( const bool& bOPENER, const DWORD& dwGaeaID )
{
	INameDisplayMan* pNAME_DISP = m_pInterface->GetDispName();
	if ( !pNAME_DISP )	return;

	m_bOPENER = bOPENER;
	m_dwGaeaID = dwGaeaID;

	if ( m_bOPENER )
	{
		{	//	광고글

			const CString& strTitle = m_pInterface->GetPrivateMarketTitle();

			CString strTemp;
			strTemp.Format( "%s", m_pGaeaClient->GetCharacter ()->GetName() );
			m_pTextBox->ClearText();
			m_pTextBox->AddText( strTemp, UINT_MAX );
			m_pTextBox->AddText( strTitle, UINT_MAX );
		}

		m_pSTART->SetVisibleSingle( TRUE );
		m_pSTOP->SetVisibleSingle( TRUE );
	}
	else
	{
		{	//	광고글
			const CROWREN* pINFO = pNAME_DISP->GetDisplayInfo( CROW_PC, dwGaeaID );
			if ( !pINFO )	 return;

			CString strTemp;
			strTemp.Format( "%s", pINFO->m_szNAME );
			m_pTextBox->ClearText();
			m_pTextBox->AddText( strTemp, NS_UITEXTCOLOR::GOLD );
			m_pTextBox->AddText( pINFO->m_szPMARKET, UINT_MAX );
		}

		m_pSTART->SetVisibleSingle( FALSE );
		m_pSTOP->SetVisibleSingle( FALSE );	

		if ( m_pPage )
		{
			m_pPage->UnLoadItemPage();

			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
			pCharacter->ReqPMarketInfo( dwGaeaID );
		}
	}
}

HRESULT CPrivateMarketWindow::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
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

void CPrivateMarketWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

	if ( m_bOPENER )
	{
		GLPrivateMarket &sPMarket = m_pGaeaClient->GetCharacter()->m_sPMarket;		

		m_pPage->LoadItemPage( sPMarket.GetInven(), m_bOPENER );
		m_pPage->DoFlipItemPage( sPMarket );		
		
		const bool bOPEN = sPMarket.IsOpen();
		if (bOPEN)
            m_pSTART->SetVisibleSingle(FALSE);
	}
	else
	{
		std::tr1::shared_ptr<GLCharClient> pCLIENT = m_pGaeaClient->GetChar(m_dwGaeaID);
		if (!pCLIENT)
            return;

		GLPrivateMarket& sPMarket = pCLIENT->m_sPMarket;
		m_pPage->LoadItemPage(sPMarket.GetInven(), m_bOPENER);
		m_pPage->DoFlipItemPage(sPMarket);	

		const bool bOPEN = sPMarket.IsOpen();
		if (!bOPEN)
            m_pInterface->SetPrivateMarketClose();
	}
}

void CPrivateMarketWindow::GetItemIndex( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}

void CPrivateMarketWindow::SetItemIndex( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

const SINVENITEM* CPrivateMarketWindow::GetItem(int nPosX, int nPosY) const
{
	return m_pPage->GetItem(nPosX, nPosY);
}
