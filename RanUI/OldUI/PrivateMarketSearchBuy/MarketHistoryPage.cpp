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
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../Util/CheckBox.h"
#include "../util/SmartComboBoxEx.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../RanLogicClient/PrivateMarketSearchBuy/PrivateMarketClient.h"

#include "../../InnerInterface.h"

#include "Pagenavigation.h"
#include "MarketHistoryRow.h"
#include "MarketHistoryPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const char* ID2GAMEWORD_TITLE = "기록";
	const char* ID2GAMEWORD_DATE_PURCHASE = "구입날짜";
	const char* ID2GAMEWORD_TIME = "시간";
	const char* ID2GAMEWORD_TRANSACTION = "거래";
	const char* ID2GAMEWORD_ITEM_NAME = "아이템명";
	const char* ID2GAMEWORD_ITEM_COUNT = "개수";
	const char* ID2GAMEWORD_ITEM_PRICE = "거래금액";
	const char* ID2GAMEWORD_SELLER_NAME = "거래캐릭터명";
	const char* ID2GAMEWORD_ITEM_SELL= "판매";
	const char* ID2GAMEWORD_ITEM_BUY = "구매";

	const char* ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO = "POSTBOX_PAGE_SEND_COMBOBOX_DESIGN_TEXINFO";
	const char* ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS = "MARKET_SEARCH_PAGE_BASIC_WEAR_CATEGORY_COMBO_BUTTON";
	const char* ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_NORMAL";
	const char* ID2GAMEWORD_COMBOBOX_MOUSE_OVER = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_OVER";
	const char* ID2GAMEWORD_COMBOBOX_MOUSE_CLICK = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_DOWN";
}

MarketHistoryPage::MarketHistoryPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx( pInterface, pEngineDevice )
, m_pGaeaClient( pGaeaClient )
, m_pInterface( pInterface )
, m_pTitleLineBox( NULL )
, m_pMiddleLineBox( NULL )
, m_pTitleBox ( NULL )
, m_pPagenavigation ( NULL )
{
	ID2GAMEWORD_TITLE = ID2GAMEWORD("MARKET_SEARCH_BUY_HISTORY", 0);//"기록";
	ID2GAMEWORD_DATE_PURCHASE = ID2GAMEWORD("MARKET_SEARCH_BUY_DATE", 0);//"구입날짜";
	ID2GAMEWORD_TIME = ID2GAMEWORD("MARKET_SEARCH_BUY_BUY_TIME", 0);//"시간";
	ID2GAMEWORD_TRANSACTION = ID2GAMEWORD("MARKET_SEARCH_BUY_TRANSACTION", 0);//"거래";
	ID2GAMEWORD_ITEM_NAME = ID2GAMEWORD("MARKET_SEARCH_BUY_ITEM_NAME", 0);//"아이템명";
	ID2GAMEWORD_ITEM_COUNT = ID2GAMEWORD("MARKET_SEARCH_BUY_ITEM_COUNT", 0);//"개수";
	ID2GAMEWORD_ITEM_PRICE = ID2GAMEWORD("MARKET_SEARCH_BUY_ITEM_PRICE", 0);//"거래금액";
	ID2GAMEWORD_SELLER_NAME = ID2GAMEWORD("MARKET_SEARCH_BUY_SELLER_NAME", 0);//"거래캐릭터명";
	ID2GAMEWORD_ITEM_SELL= ID2GAMEWORD("MARKET_SEARCH_BUY_SELL", 0);//"판매";
	ID2GAMEWORD_ITEM_BUY = ID2GAMEWORD("MARKET_SEARCH_BUY_BUY", 0);//"구매";
}

MarketHistoryPage::~MarketHistoryPage()
{
}

void MarketHistoryPage::CreateSubControl()
{
	{// 거래내역 창 배경& 라인박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_HISTORY_PAGE_BG" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );
	}
	{// 거래내역창 타이틀 박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_HISTORY_PAGE_TITLE" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );
		// [기록] 텍스트 박스
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		m_pTitleBox = new CBasicTextBox(m_pEngineDevice);
		m_pTitleBox->CreateSub( this, "MARKET_HISTORY_PAGE_TITLE" );
		m_pTitleBox->SetFont( pFont );
		m_pTitleBox->SetText( ID2GAMEWORD_TITLE,NS_UITEXTCOLOR::WHITE );
		m_pTitleBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pTitleBox );
	}
	{// 거래내역 창 배경& 라인박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_HISTORY_PAGE_OBJECT_BAR" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );
	}
	{	//거래내역창 내 [구입날짜, 시간, 거래, 아이템명, 개수, 거래금액, 거래캐릭터명] 제목 상자
		CUIControl* pObjectBar = new CUIControl(m_pEngineDevice);
		pObjectBar->CreateSub ( this, "MARKET_HISTORY_PAGE_OBJECT_BAR",UI_FLAG_XSIZE | UI_FLAG_BOTTOM );
		pObjectBar->SetUseRender ( TRUE );
		pObjectBar->SetDiffuse ( D3DCOLOR_ARGB(130,74,69,42) );
		RegisterControl ( pObjectBar );

		// [구입날짜] 텍스트 박스
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CBasicTextBox* pDatePurchaseTextBox = new CBasicTextBox(m_pEngineDevice);
		pDatePurchaseTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_OBJECT_DATE_PURCHASE" );
		pDatePurchaseTextBox->SetFont( pFont );
		pDatePurchaseTextBox->SetText( ID2GAMEWORD_DATE_PURCHASE,NS_UITEXTCOLOR::WHITE );
		pDatePurchaseTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pDatePurchaseTextBox );

		// [시간] 텍스트 박스
		CBasicTextBox* pTimeTextBox  = new CBasicTextBox(m_pEngineDevice);
		pTimeTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_OBJECT_TIME" );
		pTimeTextBox->SetFont( pFont );
		pTimeTextBox->SetText( ID2GAMEWORD_TIME,NS_UITEXTCOLOR::WHITE );
		pTimeTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pTimeTextBox );

		// [거래] 텍스트 박스
		CBasicTextBox* pTransactionTextBox = new CBasicTextBox(m_pEngineDevice);
		pTransactionTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_OBJECT_TRANSACTION" );
		pTransactionTextBox->SetFont( pFont );
		pTransactionTextBox->SetText( ID2GAMEWORD_TRANSACTION,NS_UITEXTCOLOR::WHITE );
		pTransactionTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pTransactionTextBox );

		// [아이템명] 텍스트 박스
		CBasicTextBox* pItemNameBox= new CBasicTextBox(m_pEngineDevice);
		pItemNameBox->CreateSub( this, "MARKET_HISTORY_PAGE_OBJECT_ITEM_NAME" );
		pItemNameBox->SetFont( pFont );
		pItemNameBox->SetText( ID2GAMEWORD_ITEM_NAME,NS_UITEXTCOLOR::WHITE );
		pItemNameBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pItemNameBox );

		// [개수] 텍스트 박스
		CBasicTextBox* pItemCountTextBox = new CBasicTextBox(m_pEngineDevice);
		pItemCountTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_OBJECT_ITEM_COUNT" );
		pItemCountTextBox->SetFont( pFont );
		pItemCountTextBox->SetText( ID2GAMEWORD_ITEM_COUNT,NS_UITEXTCOLOR::WHITE );
		pItemCountTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pItemCountTextBox );

		// [거래금액] 텍스트 박스
		CBasicTextBox* pItemPriceTextBox = new CBasicTextBox(m_pEngineDevice);
		pItemPriceTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_OBJECT_ITEM_PRICE" );
		pItemPriceTextBox->SetFont( pFont );
		pItemPriceTextBox->SetText( ID2GAMEWORD_ITEM_PRICE,NS_UITEXTCOLOR::WHITE );
		pItemPriceTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pItemPriceTextBox );

		// [거래캐릭터명] 텍스트 박스
		CBasicTextBox* pSellerNameTextBox = new CBasicTextBox(m_pEngineDevice);
		pSellerNameTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_OBJECT_SELLER_NAME" );
		pSellerNameTextBox->SetFont( pFont );
		pSellerNameTextBox->SetText( ID2GAMEWORD_SELLER_NAME,NS_UITEXTCOLOR::WHITE );
		pSellerNameTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pSellerNameTextBox );

		
		{// 검색된 아이템 행
			for (int i = 0; i < 12; i ++)
			{
				MarketHistoryhRow* pSearchRow = new MarketHistoryhRow( m_pGaeaClient, m_pInterface, m_pEngineDevice );
				pSearchRow->CreateSub(this,"MARKET_HISTORY_PAGE_HISTORY_ROW_BG",UI_FLAG_DEFAULT);
				pSearchRow->CreateSubControl();
				//pSearchRow->SetVisibleSingle( FALSE );
				RegisterControl( pSearchRow );
				m_pHistoryRow.push_back(pSearchRow);

				if ( i > 0)
				{
					UIRECT rcOldPos = m_pHistoryRow[i - 1]->GetLocalPos();
					UIRECT rcNewPos = m_pHistoryRow[i]->GetLocalPos();
					m_pHistoryRow[i]->SetLocalPos( D3DXVECTOR2( rcNewPos.left, rcOldPos.bottom - 1));
				}

			}
		}

		Pagenavigation* pPagenavigation = new Pagenavigation( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pPagenavigation->CreateSub(this,"HISTORY_PAGE_NAVIGATION_PAGE_POS");
		pPagenavigation->SetFont( pFont );
		pPagenavigation->CreateSubControl();
		RegisterControl(pPagenavigation);
		m_pPagenavigation = pPagenavigation;
		
	}
}

void MarketHistoryPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	m_pPagenavigation->PagenavigationUpdate( m_pGaeaClient->GetPrivateMarketClient()->GetLogPageCount() );
	if (m_pGaeaClient->GetPrivateMarketClient()->ISLogEnd() || m_pPagenavigation->IsClickEvent())
	{
		m_sLogData.clear();
		int nPage = m_pPagenavigation->GetPageIndex();
		m_sLogData = m_pGaeaClient->GetPrivateMarketClient()->GetLogtData();

		if (m_sLogData.size()<= 0)
		{
			m_pGaeaClient->GetPrivateMarketClient()->SetLogEnd(FALSE);
			return;
		}

		BOOST_FOREACH(MarketHistoryhRow* vecRow, m_pHistoryRow)
		{
			vecRow->SetVisibleSingle(FALSE);
		}

		for ( unsigned i = 0; i < 12 ; i ++)
		{
			if (i+(12*nPage) >= m_sLogData.size())
				return;
			m_pHistoryRow[i]->SetDatePurchaseText( m_sLogData[i+(12*nPage)]->strHour.c_str());
			m_pHistoryRow[i]->SetTimeTextBox( m_sLogData[i+(12*nPage)]->strDate.c_str() );
			m_pHistoryRow[i]->SetItiemNamText( m_sLogData[i+(12*nPage)]->sItemID );
			m_pHistoryRow[i]->SetItemCountText( m_sLogData[i+(12*nPage)]->dwItemTurnNum );
			//m_pHistoryRow[i]->SetItemPriceText( m_sLogData[i+(12*nPage)]->llMoney, m_sLogData[i+(12*nPage)]->fCommission );
			
			std::string strtemp(m_pGaeaClient->GetCharacter()->GetName());
			if (strtemp == m_sLogData[i]->strSellerChaName )
			{
				m_pHistoryRow[i]->SetTransactionText(ID2GAMEWORD_ITEM_SELL,NS_UITEXTCOLOR::AQUAMARINE);
				m_pHistoryRow[i]->SetSellerNameText( m_sLogData[i]->strBuyerChaName.c_str() );
				m_pHistoryRow[i]->SetItemPriceText( m_sLogData[i+(12*nPage)]->llMoney, m_sLogData[i+(12*nPage)]->nSellType, true, m_sLogData[i+(12*nPage)]->fCommission );
			}
			else
			{
				m_pHistoryRow[i]->SetTransactionText(ID2GAMEWORD_ITEM_BUY,NS_UITEXTCOLOR::PALEGREEN);
				m_pHistoryRow[i]->SetSellerNameText( m_sLogData[i]->strSellerChaName.c_str() );
				m_pHistoryRow[i]->SetItemPriceText( m_sLogData[i+(12*nPage)]->llMoney, m_sLogData[i+(12*nPage)]->nSellType, false );
			}

			m_pHistoryRow[i]->SetVisibleSingle(TRUE);
		}
		m_pGaeaClient->GetPrivateMarketClient()->SetLogEnd(FALSE);
	}
	
}

void MarketHistoryPage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}

void MarketHistoryPage::Refresh()
{

	m_pGaeaClient->GetPrivateMarketClient()->SetLogEnd(FALSE);
	//m_pGaeaClient->ReqPrivateMarketLog();

	if ( m_pGaeaClient->ReqPrivateMarketLog() == S_OK )
	{
		for (unsigned i = 0; i < m_pHistoryRow.size(); i ++)
			m_pHistoryRow[i]->SetVisibleSingle(FALSE);
	}

	//private_market::sSaleLog sLog;

	//for ( int i = 0; i < 12 ; i ++)
	//{
	//	m_pHistoryRow[i]->SetDatePurchaseText( sLog.strDate);
	//	m_pHistoryRow[i]->SetTimeTextBox( sLog.strHour );
	//	m_pHistoryRow[i]->SetTransactionText();
	//	m_pHistoryRow[i]->SetItiemNamText( sLog.sItemID );
	//	m_pHistoryRow[i]->SetItemCountText( sLog.dwItemTurnNum );
	//	m_pHistoryRow[i]->SetItemPriceText( sLog.llMoney );
	//	m_pHistoryRow[i]->SetSellerNameText( sLog.strSellerChaName.c_str() );
	//}

}
