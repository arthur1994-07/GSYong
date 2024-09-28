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
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"

#include "MarketCalculationRow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace
{
	const char* ID2GAMEWORD_ITEM_NAME = "달빛섬광의 목걸이";
	const char* ID2GAMEWORD_COUNT= "(999)";
	const char* ID2GAMEWORD_COST = "14,000,000,000";
}

MarketCalculationRow::MarketCalculationRow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx( pInterface, pEngineDevice )
, m_pITEM_NAME(NULL)
, m_pITEM_COUNT(NULL)
, m_pITEM_COST(NULL)
, m_pScrollBar(NULL)
{

}

MarketCalculationRow::~MarketCalculationRow()
{
}

void MarketCalculationRow::CreateSubControl()
{
	// 아이템 이름
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CBasicTextBox* pITEM_NAME = new CBasicTextBox(m_pEngineDevice);
	pITEM_NAME->CreateSub( this, "MARKET_WHISH_CALCULATION_ITEM_NAME" );
	pITEM_NAME->SetFont( pFont );
	pITEM_NAME->SetTextAlign( TEXT_ALIGN_LEFT );
	RegisterControl( pITEM_NAME );
	m_pITEM_NAME = pITEM_NAME;

	// 아이템 개수
	CBasicTextBox* pITEM_COUNT  = new CBasicTextBox(m_pEngineDevice);
	pITEM_COUNT->CreateSub( this, "MARKET_WHISH_CALCULATION_ITEM_COUNT" );
	pITEM_COUNT->SetFont( pFont );
	pITEM_COUNT->SetTextAlign( TEXT_ALIGN_RIGHT );
	RegisterControl( pITEM_COUNT );
	m_pITEM_COUNT = pITEM_COUNT;

	// 아이템 가격
	CBasicTextBox* pITEM_COST = new CBasicTextBox(m_pEngineDevice);
	pITEM_COST->CreateSub( this, "MARKET_WHISH_CALCULATION_ITEM_COST" );
	pITEM_COST->SetFont( pFont );
	//pSellerInfoBox->SetText( ID2GAMEWORD_COST,NS_UITEXTCOLOR::WHITE );
	pITEM_COST->SetTextAlign( TEXT_ALIGN_RIGHT );
	RegisterControl( pITEM_COST );
	m_pITEM_COST = pITEM_COST;

	//	스크롤바
	CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
	pScrollBar->CreateBaseScrollBar( "MARKET_WHISH_CALCULATION_SCROLLBAR" );
	pScrollBar->GetThumbFrame()->SetState( 1, m_pITEM_NAME->GetTotalLine ());
	pScrollBar->GetThumbFrame ()->SetScrollParent(this);
	RegisterControl( pScrollBar );
	m_pScrollBar = pScrollBar;
}

void MarketCalculationRow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	//m_pITEM_NAME->ClearText();
	//m_pITEM_COUNT->ClearText();
	//m_pITEM_COST->ClearText();

	//for( int i = 0; i< 10; i++)
	//{
	//	m_pITEM_NAME->AddText("피자 통닭 탕수육",NS_UITEXTCOLOR::GOLD);
	//	m_pITEM_COUNT->AddText("(999)",NS_UITEXTCOLOR::RED);
	//	m_pITEM_COST->AddText("14,000,000,000",NS_UITEXTCOLOR::WHITE);
	//}

	//for( int i = 0; i< 10; i++)
	//{
	//	m_pITEM_NAME->AddText("삼겹살떡볶이마시",NS_UITEXTCOLOR::IVORY);
	//	m_pITEM_COUNT->AddText("(000)",NS_UITEXTCOLOR::GREENYELLOW);
	//	m_pITEM_COST->AddText("99,000,000,000",NS_UITEXTCOLOR::AQUAMARINE);
	//}

	//for( int i = 0; i< 10; i++)
	//{
	//	m_pITEM_NAME->AddText("아아아아아이스크",NS_UITEXTCOLOR::DODGERBLUE);
	//	m_pITEM_COUNT->AddText("(111)",NS_UITEXTCOLOR::AQUABLUE);
	//	m_pITEM_COST->AddText("14,333,777,000",NS_UITEXTCOLOR::WHITE);
	//}

	{// 아이템 이름 스크롤
		CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pITEM_NAME->GetTotalLine ();
		const int nLinePerOneView = m_pITEM_NAME->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pITEM_NAME->SetCurLine ( nPos );			
		}
	}
	{//아이템 개수 스크롤
		CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pITEM_COUNT->GetTotalLine ();
		const int nLinePerOneView = m_pITEM_COUNT->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pITEM_COUNT->SetCurLine ( nPos );			
		}
	}
	{//아이템가격 스크롤
		CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pITEM_COST->GetTotalLine ();
		const int nLinePerOneView = m_pITEM_COST->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pITEM_COST->SetCurLine ( nPos );			
		}
	}
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void MarketCalculationRow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}

void MarketCalculationRow::SetItemName( std::string strName, DWORD dwColor )
{
	m_pITEM_NAME->AddOneLineText( strName.c_str(),true ,dwColor );
}

void MarketCalculationRow::SetItemCount( WORD wTurnNum )
{
	std::string strItemName = sc::string::format( "(%d)", wTurnNum );
	m_pITEM_COUNT->AddText( strItemName.c_str() );
}

void MarketCalculationRow::SetItemPrice( LONGLONG lnPrice, private_market::eSellType eSellType  )
{
    if( eSellType == private_market::eSellType_ConsignmentSale_RanPoint )
    {
        CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( lnPrice, 3, "," );
        m_pITEM_COST->AddText( strText , NS_UITEXTCOLOR::ORANGE );
    }
    else
    {
	    CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( lnPrice, 3, "," );
	    m_pITEM_COST->AddText( strText, MONEYCOLOR::GetMoneyColor(lnPrice) );
    }
}

void MarketCalculationRow::ClearMarketCalculation()
{
	m_pITEM_NAME->ClearText();
	m_pITEM_COUNT->ClearText();
	m_pITEM_COST->ClearText();
}
