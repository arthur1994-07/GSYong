#include "StdAfx.h"
#include "../../../SigmaCore/String/basestring.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"

#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
//#include "../../ModalCallerID.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../Item/ItemSlot.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/G-Logic/GLogic.h"

#include ".\shopitemsearchwindow.h"
#include ".\itemsearchresultwindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CItemSearchResultWindow::CItemSearchResultWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
{
	m_pItemNameList = NULL;
	m_pShopNameList	= NULL;
	m_pPriceList	= NULL;
	m_pPosList		= NULL;
	m_pPageStatic   = NULL;

	
	m_bNextPage = FALSE;
	m_bPrePage  = FALSE;

	m_dwCurrentPage = 0;
}

CItemSearchResultWindow::~CItemSearchResultWindow(void)
{
}


void CItemSearchResultWindow::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG/*D3DFONT_SHADOW | D3DFONT_ASCII*/ );

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList( "ITEM_SEARCH_RESULT_LINEBOX1" );
		RegisterControl( pBasicLineBox );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList( "ITEM_SEARCH_RESULT_LINEBOX2" );
		RegisterControl( pBasicLineBox );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList( "ITEM_SEARCH_RESULT_LINEBOX3" );
		RegisterControl( pBasicLineBox );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList( "ITEM_SEARCH_RESULT_LINEBOX4" );
		RegisterControl( pBasicLineBox );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList( "ITEM_SEARCH_RESULT_LINEBOX5" );
		RegisterControl( pBasicLineBox );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList( "ITEM_SEARCH_RESULT_LINEBOX6" );
		RegisterControl( pBasicLineBox );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList( "ITEM_SEARCH_RESULT_LINEBOX7" );
		RegisterControl( pBasicLineBox );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList( "ITEM_SEARCH_RESULT_LINEBOX8" );
		RegisterControl( pBasicLineBox );
	}

	{
		CBasicTextBox*	  pTextStatic = NULL;
		pTextStatic = CreateStaticControl( "ITEM_SEARCH_RESULT_STATIC1", pFont, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		pTextStatic->AddText( ID2GAMEWORD("ITEM_SEARCH_RESULT_TEXT",0), NS_UITEXTCOLOR::SILVER );

		pTextStatic = CreateStaticControl( "ITEM_SEARCH_RESULT_STATIC2", pFont, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		pTextStatic->AddText( ID2GAMEWORD("ITEM_SEARCH_RESULT_TEXT",1), NS_UITEXTCOLOR::SILVER );

		pTextStatic = CreateStaticControl( "ITEM_SEARCH_RESULT_STATIC3", pFont, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		pTextStatic->AddText( ID2GAMEWORD("ITEM_SEARCH_RESULT_TEXT",2), NS_UITEXTCOLOR::SILVER );		

		pTextStatic = CreateStaticControl( "ITEM_SEARCH_RESULT_STATIC4", pFont, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		pTextStatic->AddText( ID2GAMEWORD("ITEM_SEARCH_RESULT_TEXT",3), NS_UITEXTCOLOR::SILVER );		

		m_pPageStatic = CreateStaticControl( "ITEM_SEARCH_RESULT_STATIC5", pFont, TEXT_ALIGN_LEFT );
	}

	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub( this, "ITEM_SEARCH_RESULT_LIST1", UI_FLAG_DEFAULT, ITEM_SEARCH_RESULT_LIST1 );
		pTextBox->SetFont( pFont );		
		pTextBox->SetLineInterval( 3.0f );
		pTextBox->SetSensitive( true );
		pTextBox->SetLimitLine( 100 );
		RegisterControl( pTextBox );
		m_pItemNameList = pTextBox;
	}

	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub( this, "ITEM_SEARCH_RESULT_LIST2", UI_FLAG_DEFAULT, ITEM_SEARCH_RESULT_LIST2 );
		pTextBox->SetFont( pFont );		
		pTextBox->SetLineInterval( 3.0f );
		pTextBox->SetSensitive( true );
		pTextBox->SetLimitLine( 10 );
		RegisterControl( pTextBox );
		m_pShopNameList = pTextBox;
	}

	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub( this, "ITEM_SEARCH_RESULT_LIST3", UI_FLAG_DEFAULT, ITEM_SEARCH_RESULT_LIST3 );
		pTextBox->SetFont( pFont );		
		pTextBox->SetLineInterval( 3.0f );
		pTextBox->SetSensitive( true );
		pTextBox->SetLimitLine( 10 );
		RegisterControl( pTextBox );
		m_pPosList = pTextBox;
	}	

	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub( this, "ITEM_SEARCH_RESULT_LIST4", UI_FLAG_DEFAULT, ITEM_SEARCH_RESULT_LIST4 );
		pTextBox->SetFont( pFont );		
		pTextBox->SetLineInterval( 3.0f );
		pTextBox->SetSensitive( true );
		pTextBox->SetLimitLine( 10 );
		RegisterControl( pTextBox );
		m_pPriceList = pTextBox;
	}	

	{
		CBasicTextButton *pSearchButton[6];
		pSearchButton[0] = CreateTextButton( "ITEM_SEARCH_RESULT_BUTTON1", ITEM_SEARCH_RESULT_BUTTON1, const_cast<char*>(ID2GAMEWORD("ITEM_SEARCH_RESULT_BUTTON",0)) );
		pSearchButton[1] = CreateTextButton( "ITEM_SEARCH_RESULT_BUTTON2", ITEM_SEARCH_RESULT_BUTTON2, const_cast<char*>(ID2GAMEWORD("ITEM_SEARCH_RESULT_BUTTON",1)) );
		pSearchButton[2] = CreateTextButton( "ITEM_SEARCH_RESULT_BUTTON3", ITEM_SEARCH_RESULT_BUTTON3, const_cast<char*>(ID2GAMEWORD("ITEM_SEARCH_RESULT_BUTTON",2)) );
		pSearchButton[3] = CreateTextButton( "ITEM_SEARCH_RESULT_BUTTON4", ITEM_SEARCH_RESULT_BUTTON4, const_cast<char*>(ID2GAMEWORD("ITEM_SEARCH_RESULT_BUTTON",3)) );
		pSearchButton[4] = CreateTextButton( "ITEM_SEARCH_RESULT_BUTTON5", ITEM_SEARCH_RESULT_BUTTON5, const_cast<char*>(ID2GAMEWORD("ITEM_SEARCH_RESULT_BUTTON",4)) );
	}

}

void CItemSearchResultWindow::SetVisibleSingle( BOOL bVisible )
{
	if( bVisible )
	{
		m_reSearchTime = m_pGaeaClient->GetCurrentTime();
	}
	CUIGroup::SetVisibleSingle( bVisible );
}





CBasicTextButton* CItemSearchResultWindow::CreateTextButton( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl( pTextButton );
	return pTextButton;
}

CBasicTextBox* CItemSearchResultWindow::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont( pFont );
	pStaticText->SetTextAlign( nAlign );	
	RegisterControl( pStaticText );

	return pStaticText;
}

void CItemSearchResultWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CItemSearchResultWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
		case ITEM_SEARCH_RESULT_LIST1:
		case ITEM_SEARCH_RESULT_LIST2:
		case ITEM_SEARCH_RESULT_LIST3:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{				
				int nIndex = -1;
				if( ControlID == ITEM_SEARCH_RESULT_LIST1 )
				{
					nIndex = m_pItemNameList->GetSelectPos();
					if ( nIndex < 0 || m_pItemNameList->GetCount() <= nIndex ) return ;

					SITEMCUSTOM sItemCustom(m_vecSarchResult[nIndex].nSearchItemID);
					m_pInterface->SHOW_ITEM_INFO(
						sItemCustom,
						FALSE, FALSE, FALSE, FALSE, FALSE,
						0, 0 );
				}
				else if( ControlID == ITEM_SEARCH_RESULT_LIST2 )
				{
					nIndex = m_pShopNameList->GetSelectPos();
					if ( nIndex < 0 || m_pShopNameList->GetCount() <= nIndex ) return ;
				}
				else if( ControlID == ITEM_SEARCH_RESULT_LIST3 )
				{
					nIndex = m_pPosList->GetSelectPos();
					if ( nIndex < 0 || m_pPosList->GetCount() <= nIndex ) return ;
				}
				else if( ControlID == ITEM_SEARCH_RESULT_LIST4 )
				{
					nIndex = m_pPriceList->GetSelectPos ();
					if ( nIndex < 0 || m_pPriceList->GetCount() <= nIndex ) return ;
				}

				m_pItemNameList->SetUseOverColor( TRUE );
				m_pItemNameList->SetOverColor( nIndex, NS_UITEXTCOLOR::ORANGE );
				m_pShopNameList->SetUseOverColor( TRUE );
				m_pShopNameList->SetOverColor( nIndex, NS_UITEXTCOLOR::ORANGE );
				m_pPosList->SetUseOverColor( TRUE );
				m_pPosList->SetOverColor( nIndex, NS_UITEXTCOLOR::ORANGE );
				m_pPriceList->SetUseOverColor( TRUE );
				m_pPriceList->SetOverColor( nIndex, NS_UITEXTCOLOR::ORANGE );


				if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
					if ( pCharacter )
					{
						pCharacter->ActionMoveTo( 0.0f, m_vecSarchResult[nIndex].vShopPos+D3DXVECTOR3(0,+5,0), 
                            m_vecSarchResult[nIndex].vShopPos+D3DXVECTOR3(0,-5,0), FALSE, TRUE );
                        m_pInterface->SetItemSearchResultWindowClose();
					}
				}
			}
			break;
		}
	

		case ITEM_SEARCH_RESULT_BUTTON1:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if( m_bPrePage )
				{
					m_dwCurrentPage--;
					GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ NetReqMsg;
					NetReqMsg.dwPageNum = m_dwCurrentPage;
					m_pGaeaClient->NETSENDTOFIELD(&NetReqMsg);
				}
			}
			break;
		}
		

		case ITEM_SEARCH_RESULT_BUTTON2:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if ( m_bNextPage )
				{
					m_dwCurrentPage++;
					GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ NetReqMsg;
					NetReqMsg.dwPageNum = m_dwCurrentPage;
					m_pGaeaClient->NETSENDTOFIELD(&NetReqMsg);
				}
			}
			break;
		}		

		case ITEM_SEARCH_RESULT_BUTTON3:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				const CTime& currentTime = m_pGaeaClient->GetCurrentTime();
				CTimeSpan timeSpan = currentTime - m_reSearchTime;
				if( timeSpan.GetTotalSeconds() < 5 ) 
				{
					m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_SEARCH_DELAY") );
					return;
				}

				m_pInterface->SetItemSearchResultWindowClose();
				m_pInterface->SetItemShopSearchWindowOpen();
			}
			break;
		}

		case ITEM_SEARCH_RESULT_BUTTON4:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				const CTime& currentTime = m_pGaeaClient->GetCurrentTime();
				CTimeSpan timeSpan = currentTime - m_reSearchTime;
				if( timeSpan.GetTotalSeconds() < 5 ) 
				{
					m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_SEARCH_DELAY") );
					return;
				}

				GLMSG::SNETPC_PMARKET_SEARCH_ITEM NetMsg;
				NetMsg.sSearchData = m_pInterface->GetShopItemSearchWindow()->GetSearchData();

				if (!NetMsg.sSearchData.m_ItemName.empty())
				{
                    msgpack::sbuffer SendBuffer;
                    msgpack::pack(SendBuffer, NetMsg);
					m_pGaeaClient->NETSENDTOFIELD(NET_MSG_GCTRL_PMARKET_SEARCH_ITEM, SendBuffer);
				}

                m_pInterface->SetItemSearchResultWindowClose();
			}
			break;
		}

		case ITEM_SEARCH_RESULT_BUTTON5:
		case ET_CONTROL_BUTTON:
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                m_pInterface->SetItemSearchResultWindowClose();
			}
			break;

	}
}

void CItemSearchResultWindow::UpdateSearchResult( SSEARCHITEMESULT *pSearchItemResult, DWORD dwUpdateNum, DWORD dwPageNum )
{
	m_vecSarchResult.clear();
	m_pItemNameList->ClearText();
	m_pShopNameList->ClearText();
	m_pPosList->ClearText();
	m_pPriceList->ClearText();

	if( pSearchItemResult == NULL) return;

	CString str;

	DWORD dwSearchNum = dwUpdateNum - (dwPageNum * MAX_SEARCH_RESULT);
	if( dwSearchNum > MAX_SEARCH_RESULT )
	{
		dwSearchNum = MAX_SEARCH_RESULT;
		m_bNextPage = TRUE;
	}else{
		m_bNextPage = FALSE;
	}
	if( dwPageNum > 0 )				    m_bPrePage  = TRUE;
	else								m_bPrePage  = FALSE;

	for( DWORD i = 0; i < dwSearchNum; i++ )	{
		
		m_vecSarchResult.push_back( pSearchItemResult[i] );

		const SITEM* pITEM = GLogicData::GetInstance().GetItem( pSearchItemResult[i].nSearchItemID );
		if( pITEM == NULL) continue;
		m_pItemNameList->AddOneLineText( pITEM->GetName() );
		m_pShopNameList->AddOneLineText( pSearchItemResult[i].szShopName );

		int nPosX(0), nPosY(0);
		GLLandManClient* pLandClient = m_pGaeaClient->GetActiveMap();
		if ( pLandClient )
		{
			const GLMapAxisInfo &sMapAxisInfo = pLandClient->GetMapAxisInfo();
			sMapAxisInfo.Convert2MapPos( pSearchItemResult[i].vShopPos.x, pSearchItemResult[i].vShopPos.z, nPosX, nPosY );
		}	
        
		str.Format( "%d/%d", nPosX, nPosY );
		m_pPosList->AddOneLineText( str, FALSE );

		// Note : µ·ÀÚ¸®¼ö
		CString strMoney;
		strMoney.Format( "%I64d", pSearchItemResult[i].llPRICE );

		CString strMoneyFormat;
		strMoneyFormat = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pSearchItemResult[i].llPRICE, 3, "," );

		m_pPriceList->AddOneLineText( strMoneyFormat, FALSE, MONEYCOLOR::GetMoneyColor(pSearchItemResult[i].llPRICE) );
	}

	str.Format( "%d", dwPageNum + 1 );
	m_pPageStatic->SetText( str );

	m_dwCurrentPage = dwPageNum;

}




MyItemSearchResultWindow::MyItemSearchResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CItemSearchResultWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyItemSearchResultWindow::CreateUIWindowAndRegisterOwnership()
{
    CItemSearchResultWindow::Create( ITEM_SEARCH_RESULT_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CItemSearchResultWindow::CreateBaseWindowLightGray( "ITEM_SEARCH_RESULT_WINDOW", (char*)ID2GAMEWORD("ITEM_SHOP_SEARCH_STATIC",0 ) );
    CItemSearchResultWindow::CreateSubControl();
    CItemSearchResultWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
    CItemSearchResultWindow::m_pInterface->UiRegisterControl( this, true );
    CItemSearchResultWindow::m_pInterface->UiShowGroupFocus( ITEM_SEARCH_RESULT_WINDOW );
}

void MyItemSearchResultWindow::UpdateSearchResult( SSEARCHITEMESULT* pSearchItemResult, DWORD dwUpdateNum, DWORD dwPageNum )
{
    CItemSearchResultWindow::UpdateSearchResult( pSearchItemResult, dwUpdateNum, dwPageNum );
}
