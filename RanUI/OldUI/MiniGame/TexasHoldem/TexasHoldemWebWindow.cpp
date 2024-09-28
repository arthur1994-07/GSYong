#include "StdAfx.h"

#include <afxdisp.h>
#include <MsHTML.h>

#include "../../../../EngineLib/GUInterface/UIWindowBody.h"
#include "../../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/Common/CommonWeb.h"

#include "../../../InnerInterface.h"
#include "../../Interface/BasicChat.h"

#include "./TexasHoldemWebWindow.h"

#include "../../../../RanLogic/GLUseFeatures.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/TexasHoldem/GLTexasHoldemClient.h"

#include <WinInet.h>

// ----------------------------------------------------------------------------
#include "../../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace TexasHoldem
{

	TexasHoldemWebWindow::TexasHoldemWebWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CWebWindowBase( pInterface, pEngineDevice )
	, m_pGaeaClient( pGaeaClient )
	{
		reset();
	}

	TexasHoldemWebWindow::~TexasHoldemWebWindow()
	{
	}

	void TexasHoldemWebWindow::CreateUIWindowAndRegisterOwnership( void )
	{
		Create ( MINIGAME_TEXASHOLDEM_WEB_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		CreateBaseWindowBlack ( "MINIGAME_TEXASHOLDEM_WEB_WINDOW_BASE", (char*)ID2GAMEWORD("TEXASHOLDEM_WINDOW_TEXT", 0) );
		SetAlignFlag ( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
		m_pInterface->UiRegisterControl ( this, true );	
		m_pInterface->UiShowGroupFocus ( MINIGAME_TEXASHOLDEM_WEB_WINDOW );
		CWebWindowBase::SetVisibleSingle(FALSE);
	}

	void TexasHoldemWebWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
	{
		CWebWindowBase::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		if ( FALSE == IsVisible() )
		{
			return;
		}

		if ( m_bLoadComplete )
		{
			return;
		}

		if ( FALSE == IsCompleteLoad() )
		{
			return;
		}

		m_bLoadComplete = true;
		processCertificate();
	}

	void TexasHoldemWebWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
	{
		CWebWindowBase::TranslateUIMessage ( ControlID, dwMsg );
	}

	void TexasHoldemWebWindow::RunTexasHoldemWindow( const bool bVisible )
	{
		if( !GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() )
		{
			CWebWindowBase::SetAddress( NULL );
			CWebWindowBase::SetPostArgument( NULL );
			CWebWindowBase::SetVisibleSingle( FALSE );
			return;
		}

		if( bVisible && !IsVisible() )
		{
			CWebWindowBase::SetVisibleSingle( TRUE );
			m_pInterface->SetBlockModalUI( TRUE );
		}
		else
		{
			CWebWindowBase::SetAddress( NULL );
			CWebWindowBase::SetPostArgument( NULL );
			CWebWindowBase::SetVisibleSingle( FALSE );
			CWebWindowBase::SetCompleteLoad( FALSE );
			reset();
			m_pInterface->SetBlockModalUI( FALSE );
		}
	}

	void TexasHoldemWebWindow::SetVisibleSingle ( BOOL bVisible )
	{
		if( !GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() )
		{
			CWebWindowBase::SetAddress( NULL );
			CWebWindowBase::SetPostArgument( NULL );
			CWebWindowBase::SetVisibleSingle( FALSE );
			return;
		}

		if ( FALSE == bVisible && false == m_strPostArgument.empty() && false == m_strAddress.empty() )
		{
			// 텍사스홀덤 web ui 닫힘 감지
			m_pGaeaClient->GetTexasHoldemManager()->EndGame();
		}
	}

	void TexasHoldemWebWindow::SetURL( const std::string& strURL, const std::string& strPOST )
	{
		SetAddress( strURL.c_str() );
		SetPostArgument( strPOST.c_str() );
	}

	void TexasHoldemWebWindow::processCertificate()
	{
		IWebBrowser2* pBrowser = ( IWebBrowser2* )GetWebBrowser();
		if ( NULL == pBrowser )
		{
			return;
		}

		IDispatch *document_dispatch = NULL;
		HRESULT hr = pBrowser->get_Document( &document_dispatch );
		if ( FAILED( hr ) || ( NULL == document_dispatch ) )
		{
			return;
		}

		IHTMLDocument2 *document = NULL;
		hr = document_dispatch->QueryInterface( IID_IHTMLDocument2, ( void** )&document );
		if ( FAILED( hr ) || ( NULL == document ) )
		{
			document_dispatch->Release();
			return;
		}

		IHTMLElementCollection* pCollection = NULL;
		hr = document->get_all( &pCollection );
		if ( FAILED( hr ) || ( NULL == pCollection ) )
		{
			document_dispatch->Release();
			document->close();
			document->Release();
			return;
		}

		CString strCODE;
		long nLength = 0;
		pCollection->get_length ( &nLength );
		for ( int i = 0; i < nLength; i++ )
		{
			COleVariant vIdx( ( long )i, VT_I4 );
			IDispatch* pElemDispatch = NULL;
			hr = pCollection->item( vIdx, vIdx, &pElemDispatch );
			if ( SUCCEEDED ( hr ) && ( pElemDispatch != NULL ) )
			{
				IHTMLElement* pElem = NULL;
				hr = pElemDispatch->QueryInterface( IID_IHTMLElement, ( void** )&pElem );
				if ( SUCCEEDED ( hr ) && ( pElem != NULL ) )
				{
					BSTR bstrID = 0;
					hr = pElem->get_id( &bstrID ); 
					CString strID( bstrID );
					if ( "RtnCode" == strID  )
					{
						CComVariant var;
						var.vt = VT_EMPTY;
						if ( S_OK == pElem->getAttribute( CComBSTR( "value" ), 0, &var ) )
						{               
							strCODE = var.bstrVal;
						}
					}
					pElem->Release();
				}
				pElemDispatch->Release();
			}        
		}
		
		pCollection->Release();
		document_dispatch->Release();
		document->close();
		document->Release();

		/*if ( pBrowser != NULL ) 
		{
			IDispatch *document_dispatch = NULL;
			HRESULT hr = pBrowser->get_Document( &document_dispatch );

			if ( SUCCEEDED( hr ) && ( document_dispatch != NULL ) )
			{
				hr = document_dispatch->QueryInterface( IID_IHTMLDocument2, ( void** )&document );
				IHTMLElementCollection* pCollection = NULL;
				hr = document->get_all( &pCollection );
				if ( SUCCEEDED ( hr ) && ( pCollection != NULL ) )
				{
					long nLength = 0;
					pCollection->get_length ( &nLength );
					for ( int i = 0; i < nLength; i++ )
					{
						COleVariant vIdx( ( long )i, VT_I4 );
						IDispatch* pElemDispatch = NULL;
						IHTMLElement* pElem = NULL;
						hr = pCollection->item( vIdx, vIdx, &pElemDispatch );
						if ( SUCCEEDED ( hr ) && ( pElemDispatch != NULL ) )
						{
							hr = pElemDispatch->QueryInterface( IID_IHTMLElement, ( void** )&pElem );
							if ( SUCCEEDED ( hr ) && ( pElem != NULL ) )
							{
								BSTR bstrID = 0;
								hr = pElem->get_id( &bstrID ); 
								CString strID( bstrID );
								if ( "RtnCode" == strID  )
								{
									CComVariant var;
									var.vt = VT_EMPTY;
									if( pElem->getAttribute( CComBSTR( "value" ), 0, &var )==S_OK )
									{               
										strCODE = var.bstrVal;
									}
								}
								pElem->Release();
							}
							pElemDispatch->Release();
						}        
					}
					pCollection->Release();
				}
				document_dispatch->Release();
			}
		}

		document->close();
		document->Release();
		document = NULL;*/

		GLTexasHoldemClient* pTexasHoldeMgr = m_pGaeaClient->GetTexasHoldemManager();
		if ( NULL == pTexasHoldeMgr )
		{
			return;
		}

		if ( "100" == strCODE )
		{
			// 정상
			pTexasHoldeMgr->DisplayMsg( eSuccess );
		}
		else if ( "-100" == strCODE )
		{
			// 유저 정보 없음
			pTexasHoldeMgr->DisplayMsg( eWebErrorUser );
		}
		else if ( "-200" == strCODE )
		{
			// 인증 오류
			pTexasHoldeMgr->DisplayMsg( eWebErrorAuth );
		}
		else if ( "-300" == strCODE )
		{
			// 칩 부족
			pTexasHoldeMgr->DisplayMsg( eWebErrorChip );
		}
		else
		{
			pTexasHoldeMgr->DisplayMsg( eWebErrorUnknown );
		}
	}

	void TexasHoldemWebWindow::reset()
	{
		m_bLoadComplete = false;
	}
}