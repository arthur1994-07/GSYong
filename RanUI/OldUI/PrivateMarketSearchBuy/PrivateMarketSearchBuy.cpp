#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/PrivateMarketSearchBuy/PrivateMarketClient.h"

#include "../../InnerInterface.h"

#include "MarketSearchPage.h"
#include "MarketSearchWhishPage.h"
#include "PrivateMarketSearchBuy.h"
#include "MarketHistoryPage.h"
#include "MarketRPPage.h" 

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace
{
	const char* ID2GAMEWORD_TITLE = "개인상점검색";
	const char* ID2GAMEWORD_SERACH_TAB = "검색";
	const char* ID2GAMEWORD_WISH_LIST_TAB= "찜바구니";
	const char* ID2GAMEWORD_SHOPPING_HISTORY_TAB = "거래내역";
}


PrivateMarketSearchBuy::PrivateMarketSearchBuy(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx( pInterface, pEngineDevice )
, m_pGaeaClient( pGaeaClient )
, m_pInterface( pInterface )
, m_pSerchPage( NULL )
, m_fFlickeringTime( 0 )
, m_pCurrentPage( NULL )
, bIsWhishPageEvent(false)
, m_pWhishPage( NULL )
, m_pHistoryPage( NULL )
, m_pRanPointPage( NULL )
, m_pWhishPageEvent( NULL )
{
	ID2GAMEWORD_TITLE = ID2GAMEWORD("MARKET_SEARCH_BUY_PRIVATE_MAKET_SEARCH_BUY",0);//"개인상점검색";
	ID2GAMEWORD_SERACH_TAB = ID2GAMEWORD("MARKET_SEARCH_BUY_SEARCH_BUTTON", 0);//검색
	ID2GAMEWORD_WISH_LIST_TAB= ID2GAMEWORD("MARKET_SEARCH_BUY_WISH_PAGE",0);//"찜바구니";
	ID2GAMEWORD_SHOPPING_HISTORY_TAB = ID2GAMEWORD("MARKET_SEARCH_BUY_HISTORY_PAGE",0);//"거래내역";
} 


PrivateMarketSearchBuy::~PrivateMarketSearchBuy()
{
}

void PrivateMarketSearchBuy::CreateSubControl()
{
	{
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(GetEngineDeviceMan());
		pLineBox->CreateSub( GetUIGroupThis(), "MARKET_SEARCH_SEARCH_WINDOW_WHITE" );
		pLineBox->CreateSubControl( "ANOTHER_CHAR_REGION_WINDOW_TEXTUREINFO" );
		RegisterControl( pLineBox );
	}

	{   // 검색 탭 버튼
		STAPBUTTON* pSerach = new STAPBUTTON(m_pEngineDevice);
		pSerach->CreateSub( this, "MARKET_SEARCH_SEARCH_BUTTON", UI_FLAG_DEFAULT, ID_SEARCH_TAB );
		pSerach->CreateSubControl( ID2GAMEWORD_SERACH_TAB,
			"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
			"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
			"CHARACTER_WINDOW_TAP_BASE");
		RegisterControl ( pSerach );
		RegisterTapButton( pSerach );
	}

	{   // 찜바구니 탭 버튼
		STAPBUTTON* pWishList = new STAPBUTTON(m_pEngineDevice);
		pWishList->CreateSub ( this, "MARKET_SEARCH_WISH_LIST_BUTTON", UI_FLAG_DEFAULT, ID_WISH_LIST_TAB );
		pWishList->CreateSubControl( ID2GAMEWORD_WISH_LIST_TAB,
			"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
			"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
			"CHARACTER_WINDOW_TAP_BASE");
		RegisterControl ( pWishList );
		RegisterTapButton( pWishList );
	}

	{   // 꺼래내역 탭 버튼
		STAPBUTTON* pShooppingHistory = new STAPBUTTON(m_pEngineDevice);
		pShooppingHistory->CreateSub ( this, "MARKET_SEARCH_SHOPPING_BUTTON", UI_FLAG_DEFAULT, ID_SHOPPING_HISTORY_TAB );
		pShooppingHistory->CreateSubControl( ID2GAMEWORD_SHOPPING_HISTORY_TAB ,
			"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
			"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
			"CHARACTER_WINDOW_TAP_BASE");
		RegisterControl ( pShooppingHistory );
		RegisterTapButton( pShooppingHistory );
	}

    if ( GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
    {
        {   // 란포인트(RP) 탭 버튼
            STAPBUTTON* pStapButton = new STAPBUTTON(m_pEngineDevice);
            pStapButton->CreateSub ( this, "MARKET_SEARCH_RANPOINT_BUTTON", UI_FLAG_DEFAULT, ID_RAN_POINT_TAB );
            pStapButton->CreateSubControl( 
                ID2GAMEWORD("MARKET_RPPAGE_TEXT", 31)
                ,"CHARACTER_WINDOW_TAP_BASE"
                ,"CHARACTER_WINDOW_TAP_ACTIVE"
                ,"CHARACTER_WINDOW_TAP_BASE"
                ,"CHARACTER_WINDOW_TAP_INACTIVE"
                ,"CHARACTER_WINDOW_TAP_BASE" );
            RegisterControl ( pStapButton );
            RegisterTapButton( pStapButton );
        }

        {   // 란포인트(RP) 페이지
            MarketRPPage* pRanPoint = new MarketRPPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
            pRanPoint->CreateSub(this,"MARKET_SEARCH_PAGE_BG",UI_FLAG_DEFAULT, ID_RAN_POINT_PAGE);
            pRanPoint->CreateSubControl();
            pRanPoint->SetVisibleSingle( FALSE );
            RegisterControl( pRanPoint );
            m_pRanPointPage = pRanPoint;
        }

    }


	{   // 검색 페이지
		MarketSearchPage* pSearchPage = new MarketSearchPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pSearchPage->CreateSub(this,"MARKET_SEARCH_PAGE_BG",UI_FLAG_DEFAULT, ID_SEARCH_PAGE);
		pSearchPage->CreateSubControl();
		pSearchPage->SetVisibleSingle( FALSE );
		RegisterControl( pSearchPage );
		m_pSerchPage = pSearchPage;
	}

	{   // 위시리스트 페이지
		MarketSearchWhishPage* pWhishPage = new MarketSearchWhishPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pWhishPage->CreateSub(this,"MARKET_SEARCH_PAGE_BG",UI_FLAG_DEFAULT, ID_WHISH_LIST_PAGE);
		pWhishPage->CreateSubControl();
		pWhishPage->SetVisibleSingle( FALSE );
		RegisterControl( pWhishPage );
		m_pWhishPage = pWhishPage;
	}

	{   // 구매내역 페이지
		MarketHistoryPage* pHistoryPage = new MarketHistoryPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pHistoryPage->CreateSub(this,"MARKET_SEARCH_PAGE_BG",UI_FLAG_DEFAULT, ID_HISTORY_PAGE);
		pHistoryPage->CreateSubControl();
		pHistoryPage->SetVisibleSingle( FALSE );
		RegisterControl( pHistoryPage );
		m_pHistoryPage = pHistoryPage;
	}

	{//선택 상자
		m_pWhishPageEvent = new CBasicLineBoxSmart(m_pEngineDevice);
		m_pWhishPageEvent->CreateSub( this, "MARKET_SEARCH_WHISH_EVENT" );
		m_pWhishPageEvent->CreateSubControl( "LINEBOX_Y" );
		m_pWhishPageEvent->SetVisibleSingle(FALSE);
		RegisterControl( m_pWhishPageEvent );
	}

	// 도움말
	{
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = "POINT_SHOP_HELP_BUTTON";
		arg.mouseOverTextureKey = "POINT_SHOP_HELP_BUTTON_F";
		arg.mouseClickTextureKey = "POINT_SHOP_HELP_BUTTON";
		arg.dwColor = NS_UITEXTCOLOR::WHITE;

		RnButton* pHelpButton = new RnButton(m_pEngineDevice);
		pHelpButton->CreateSub(this, "MARKET_HELP_BUTTON", UI_FLAG_DEFAULT, ID_HELP_BUTTON );
		pHelpButton->CreateSubControl(arg);
		RegisterControl(pHelpButton);	
	}


	m_pGaeaClient->GetPrivateMarketClient()->ClearMarketSearchBuyData();
	EventSelectedTap( ID_SEARCH_TAB );

}

void PrivateMarketSearchBuy::RanPointRefresh()
{
    if( m_pRanPointPage )
        m_pRanPointPage->Refresh();
}

CUIGroup* PrivateMarketSearchBuy::GetTapWindow( UIGUID controlID )
{
	switch ( controlID )
	{
	case ID_SEARCH_TAB:
		return m_pSerchPage;
		break;
	case ID_WISH_LIST_TAB:
		{
			SetWhishPageEvent(false);
		}
		return m_pWhishPage;
		break;
	case ID_SHOPPING_HISTORY_TAB:
		return m_pHistoryPage;
		break;
    case ID_RAN_POINT_TAB:
        return m_pRanPointPage;
	}

	return NULL;
}


void PrivateMarketSearchBuy::EventSelectedTap( UIGUID controlID )
{
	if( controlID < ID_SEARCH_TAB && ID_RAN_POINT_TAB < controlID )
		return;

	CUIGroup* pUI = GetTapWindow( controlID );

	if( pUI != NULL )
	{
		if( m_pCurrentPage == NULL )
			m_pCurrentPage = pUI;
		else
			m_pCurrentPage->SetVisibleSingle( FALSE );

		m_pCurrentPage = pUI;
		m_pCurrentPage->SetVisibleSingle( TRUE );
		m_pCurrentPage->Refresh();       
	}

}

void PrivateMarketSearchBuy::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( bIsWhishPageEvent )
	{
		WhishPageEvent( fElapsedTime );
	}
}


void PrivateMarketSearchBuy::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( cID, dwMsg, emResult );
	CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	if( cID == ID_HELP_BUTTON )
	{
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
			m_pInterface->MultiLineCommentUtil(
				ID2GAMEINTEXT( "PRIVATE_MARKET_WINDOW_HELP_TEXT1" ),
				pFont,
				800.0f,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,
				NS_UITEXTCOLOR::WHITE );
		}
	}
}

void PrivateMarketSearchBuy::PrivatemarketRefreshSearchResult()
{
	m_pSerchPage->RefreshSearchResult();
	m_pWhishPage->Refresh();
}

void PrivateMarketSearchBuy::CloseAllRollOver()
{
	m_pSerchPage->CloseAllRollOver();
}

void PrivateMarketSearchBuy::SetVisibleSingle( BOOL bVisible )
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if ( !pChar )
		return;

    if ( !bVisible )
    {
        //열려있는 찜아이템 정보비교창 닫기
        m_pInterface->GetMarketWishItemTooltipGroup()->CloseItemToolTipMultiAll();
        
        //개인상점 검색 카드 사용 끝냄 서버에 알림        
        pChar->ReqPMarketSearchClose();
		
		if (m_pRanPointPage)
			m_pRanPointPage->Reset();

// 		const SINVEN_POS sPreItem = pChar->GetPreHoldItemPos();
// 		if ( sPreItem.VALID() )
// 		{
// 			const SINVENITEM* const pInvenItem = pChar->m_cInventory.FindPosItem(sPreItem.wPosX, sPreItem.wPosY);
// 			if ( pInvenItem )
// 			{
// 				holditem::HoldItem( holditem::INVENTORY, pInvenItem->CustomItem(), sPreItem.wPosX, sPreItem.wPosY);
// 			}
// 			sPreItem.RESET();
// 		}
    }

    CUIWindowEx::SetVisibleSingle( bVisible );

	if ( !bVisible )
	{
//		const SINVEN_POS& sPreItem = pChar->GetPreHoldItemPos();
		if ( pChar->m_sPreInventoryItem.VALID() )
		{
			pChar->ReqInvenTo(pChar->m_sPreInventoryItem.wPosX,pChar->m_sPreInventoryItem.wPosY);
			pChar->m_sPreInventoryItem.RESET();
		}
	}
}


void PrivateMarketSearchBuy::SetPageWaiting( bool bIsWaiting )
{
	m_pSerchPage->SetPageWaiting(bIsWaiting);
}

bool PrivateMarketSearchBuy::IsPageWaiting()
{
	return m_pSerchPage->IsPageWaiting();
}

void PrivateMarketSearchBuy::WhishPageEvent(float fElapsedTime )
{
	m_fFlickeringTime += fElapsedTime;

	if (m_fFlickeringTime > 0.5f)
	{
		if ( m_pWhishPageEvent->IsVisible() )
		{
			m_pWhishPageEvent->SetVisibleSingle(FALSE);
		}
		else
		{
			m_pWhishPageEvent->SetVisibleSingle(TRUE);
		}
		m_fFlickeringTime = 0;
	}
}

void PrivateMarketSearchBuy::SetWhishPageEvent(bool bBool)
{
	bIsWhishPageEvent = bBool;
	m_pWhishPageEvent->SetVisibleSingle(bBool);
}

void MyPrivateMarketSearchBuy::CreateUIWindowAndRegisterOwnership()
{
	PrivateMarketSearchBuy::Create( PRIVATE_MARKET_SEARCH_BUY, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	PrivateMarketSearchBuy::CreateBaseWindowLightGray( "PRIVATE_MARKET_SEARCH_BUY_BASE_WINDOW", ID2GAMEWORD_TITLE );
	PrivateMarketSearchBuy::CreateSubControl();
	PrivateMarketSearchBuy::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );		
	PrivateMarketSearchBuy::m_pInterface->UiRegisterControl( this, true );
	PrivateMarketSearchBuy::m_pInterface->UiShowGroupFocus( GetWndID() );
}

void MyPrivateMarketSearchBuy::PrivatemarketRefreshSearchResult()
{
	PrivateMarketSearchBuy::PrivatemarketRefreshSearchResult();
}

void MyPrivateMarketSearchBuy::CloseAllRollOver()
{
	PrivateMarketSearchBuy::CloseAllRollOver();
}

void MyPrivateMarketSearchBuy::SetPageWaiting( bool bIsWaiting )
{
	PrivateMarketSearchBuy::SetPageWaiting(bIsWaiting);
}

bool MyPrivateMarketSearchBuy::IsPageWaiting()
{
	return PrivateMarketSearchBuy::IsPageWaiting();
}

void MyPrivateMarketSearchBuy::SetWhishPageEvent( bool bBOOL )
{
	PrivateMarketSearchBuy::SetWhishPageEvent(bBOOL);
}

void MyPrivateMarketSearchBuy::Refresh()
{
    // 중간에 삽입된 코드이다.
    PrivateMarketSearchBuy::RanPointRefresh();
}
