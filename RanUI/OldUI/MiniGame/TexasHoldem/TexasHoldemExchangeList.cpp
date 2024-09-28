#include "StdAfx.h"
#include "TexasHoldemExchangeList.h"
#include "TexasHoldemExchangeWindow.h"

#include "../../../../RanLogic/TexasHoldem/GLTexasHoldem.h"

#include "../../../../EngineLib/DeviceMan.h"
#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../EngineLib/DxTools/d3dfont.h"
#include "../../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../../RanLogic/Item/GLItemMan.h"
#include "../../../../RanLogic/GLogicEx.h"


#include "../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	//현재 사이즈에 최적화된 수치로 윈도우 사이즈가 변경된다면 조절해야만한다.
	const float fTEXT_PART_INTERVAL = 6.5f;       // 줄간격
	const float fSELBOX_PART_INTERVAL = 2.5f;     // 선택박스 간격 조절
}
namespace TexasHoldem
{

	TexasHoldemExchangeList::TexasHoldemExchangeList( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface  )
	: CBasicLineBoxSmart(pEngineDevice)
	, m_pGaeaClient(pGaeaClient)
	, m_pInterface(pInterface)
	, m_pChipTextBox(NULL)
	, m_pMoneyTextBox(NULL)
	, m_pScrollBar(NULL)
	, m_pSeleted(NULL)
	, m_nSelectedPos(-1)
	, m_eSelectedType(eExchange_None)
	, m_bSelected(false)
	{
	}

	void TexasHoldemExchangeList::CreateSubControl()
	{
		CBasicLineBoxSmart::CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_DARKGRAY" );
		CD3DFontPar* pFont10 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );

		// 선택영역
		CBasicLineBoxSmart* pLinBoxSmartTypeTitle = new CBasicLineBoxSmart( m_pEngineDevice );
		if( pLinBoxSmartTypeTitle )
		{
			pLinBoxSmartTypeTitle->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_LIST_SELECTED", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
			pLinBoxSmartTypeTitle->CreateSubControl( "SMARTLINEBOX_YELLOWLINE" );
			RegisterControl( pLinBoxSmartTypeTitle );
			m_pSeleted = pLinBoxSmartTypeTitle;
			m_rcSeleted = m_pSeleted->GetLocalPos();
		}

		// 텍스트 박스
		CBasicTextBoxEx* pChipTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		if( pChipTextBox )
		{
			pChipTextBox->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_LIST_CHIP", UI_FLAG_DEFAULT, ID_EXCHANGE_CHIP_TEXTLIST );
			pChipTextBox->SetFont( pFont10 );		
			pChipTextBox->SetSensitive( false );
			pChipTextBox->SetPartInterval( fTEXT_PART_INTERVAL );
			RegisterControl( pChipTextBox );
			m_pChipTextBox = pChipTextBox;
		}

		CBasicTextBoxEx* pMoneyTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		if( pMoneyTextBox )
		{
			pMoneyTextBox->CreateSub( this, "MINIGAME_TEXASHOLDEM_EXCHANGE_LIST_MONEY", UI_FLAG_DEFAULT, ID_EXCHANGE_MONEY_TEXTLIST );
			pMoneyTextBox->SetFont( pFont10 );		
			pMoneyTextBox->SetSensitive( false );
			pMoneyTextBox->SetPartInterval( fTEXT_PART_INTERVAL );
			RegisterControl( pMoneyTextBox );
			m_pMoneyTextBox = pMoneyTextBox;
		}

		// 스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		if( pScrollBar )
		{
			pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, ID_PRODUCT_TYPE_LIST_SCROOL );
			pScrollBar->CreateBaseScrollBar( "MINIGAME_TEXASHOLDEM_EXCHANGE_LIST_SCROOL" );
			pScrollBar->GetThumbFrame()->SetScrollParent( m_pChipTextBox );
			pScrollBar->GetThumbFrame()->SetScrollMoveLine( 1 );
			RegisterControl ( pScrollBar );
			m_pScrollBar = pScrollBar;
		}
	}

	void TexasHoldemExchangeList::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
	{
		CUIGroup::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);

		if( !m_bSelected )
		{
			m_pSeleted->SetVisibleSingle( FALSE );
		}

		CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();
		const int nTotal       = m_pChipTextBox->GetTotalLine();
		const int nVisibleLine = m_pChipTextBox->GetVisibleLine();
		m_pScrollBar->GetThumbFrame()->SetState( nTotal, nVisibleLine );

		int nCurPos = 0;
		const int nMovableLine = nTotal - nVisibleLine;
		float fPercent = pThumbFrame->GetPercent();
		nCurPos = (int)floor(fPercent * nMovableLine);
		if ( nCurPos < 0 ) nCurPos = 0;
		m_pChipTextBox->SetCurLine(nCurPos);
		m_pMoneyTextBox->SetCurLine(nCurPos);

		int nScrollRelativePos = m_nSelectedPos-nCurPos;
		if( 0 <= nScrollRelativePos && nScrollRelativePos < nVisibleLine)
		{
			m_pSeleted->SetVisibleSingle( TRUE );

			UIRECT rcPart;
			if( !m_pChipTextBox->GetPartInfo(0, rcPart) )
				return;

			float fHeight = rcPart.sizeY + m_pChipTextBox->GetPartInterval();
			float fSelectSizeY = fHeight + fSELBOX_PART_INTERVAL;

			D3DXVECTOR2 vLocalPos, vGlobalPos;
			vLocalPos.x = m_rcSeleted.left;
			vLocalPos.y = m_rcSeleted.top + fHeight * (nScrollRelativePos);
			vGlobalPos = vLocalPos;

			vGlobalPos.x += this->GetGlobalPos().left;
			vGlobalPos.y += this->GetGlobalPos().top;

			m_pSeleted->SetControlSize( (int)m_rcSeleted.sizeX, (int)fSelectSizeY );
			m_pSeleted->SetLocalPos( vLocalPos );
			m_pSeleted->SetGlobalPos( vGlobalPos );
		}
		else
		{
			m_pSeleted->SetVisibleSingle( FALSE );
		}
	}

	void TexasHoldemExchangeList::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
	{
		CUIGroup::TranslateUIMessage(ControlID, dwMsg);

		switch( ControlID )
		{
		case ID_EXCHANGE_CHIP_TEXTLIST:
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bSelected = true;
				m_nSelectedPos = m_pChipTextBox->GetSelectPos();
			}
			break;
		case ID_EXCHANGE_MONEY_TEXTLIST:
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bSelected = true;
				m_nSelectedPos = m_pMoneyTextBox->GetSelectPos();
			}
			break;
		}
	}

	void TexasHoldemExchangeList::SetSeletedRadioType( eExchange_Type emType )
	{
		m_eSelectedType = emType;
		m_bSelected = false;
		ResetTextBox();
	}

	void TexasHoldemExchangeList::ResetTextBox()
	{
		int nTotalLine = static_cast<int>( m_PtrVectorExchangeList.size() );
		m_pChipTextBox->ClearText();
		m_pChipTextBox->SetLimitLine( nTotalLine );
		m_pMoneyTextBox->ClearText();
		m_pMoneyTextBox->SetLimitLine( nTotalLine );

		for( int i=0; i<nTotalLine; ++i )
		{
			CString strChip = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( m_PtrVectorExchangeList[i].Chip, 3, "," );
			DWORD nTextColor = NS_UITEXTCOLOR::GREENYELLOW;
			int nIndex = m_pChipTextBox->AddText( strChip, nTextColor );
			m_pChipTextBox->SetTextImage(nIndex, "MINIGAME_TEXASHOLDEM_EXCHANGE_ICON_CHIP" );

			CString strMoney = "";
			CString strType = "";
			switch( m_eSelectedType )
			{
			case eExchange_Gamemoney:
				strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( m_PtrVectorExchangeList[i].GameMoney, 3, "," );
				nTextColor = NS_UITEXTCOLOR::WHITE;
				nIndex = m_pMoneyTextBox->AddText( strMoney, nTextColor );
				strType = "INVENTORY_MONEY_BUTTON_F";
				break;
			case eExchange_Ranpoint:
				strMoney.Format ( "%I64d", m_PtrVectorExchangeList[i].Point );
				nTextColor = NS_UITEXTCOLOR::WHITE;
				nIndex = m_pMoneyTextBox->AddText( strMoney, nTextColor );
				strType = "INVENTORY_POINT_BUTTON_RENEW_F";
				break;
			}
			m_pMoneyTextBox->SetTextImage( nIndex, strType );
		}
	}

	const bool TexasHoldemExchangeList::SetList( const std::vector< sChipPrice >& chipMap )
	{
		m_PtrVectorExchangeList.clear();
		if( chipMap.empty() )
			return false;

		m_PtrVectorExchangeList = chipMap;
		ResetTextBox();
		return true;
	}

	void TexasHoldemExchangeList::ClearText()
	{
		if( m_pChipTextBox )
			m_pChipTextBox->ClearText();
		if( m_pMoneyTextBox )
			m_pMoneyTextBox->ClearText();
	}

}