#include "stdafx.h"

#include "./LottoWinManListPropertyUIComp.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../../../SigmaCore/DebugInclude.h"

CLottoWinManListPropertyUIComp::CLottoWinManListPropertyUIComp(  CInnerInterface* pInterface
															   , EngineDeviceMan* pEngineDevice )
															   : CUIGroupHelper( pEngineDevice )
															   , m_pInterface( pInterface )
															   , m_pRankingTextBox( NULL )
															   , m_pNameTextBox( NULL )
															   , m_pCountTextBox( NULL )
															   , m_pWinMoneyTextBox( NULL )
{
}

CLottoWinManListPropertyUIComp::~CLottoWinManListPropertyUIComp() { }

void CLottoWinManListPropertyUIComp::CreateSubControl()
{
	{
		m_pRankingTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pRankingTextBox->CreateSub( this, "LOTTO_WIN_MAN_LIST_BOX_PROPERTY_RANK_TEXT_REGION", UI_FLAG_YSIZE );
		m_pRankingTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pRankingTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
		m_pRankingTextBox->AddText( "", NS_UITEXTCOLOR::WHITE );
		m_pRankingTextBox->SetVisibleSingle( TRUE );
		RegisterControl( m_pRankingTextBox );
	}

	{
		m_pNameTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pNameTextBox->CreateSub( this, "LOTTO_WIN_MAN_LIST_BOX_PROPERTY_WIN_MAN_TEXT_REGION", UI_FLAG_YSIZE );
		m_pNameTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pNameTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
		m_pNameTextBox->AddText( "", NS_UITEXTCOLOR::WHITE );
		m_pNameTextBox->SetVisibleSingle( TRUE );
		RegisterControl( m_pNameTextBox );
	}

	{
		m_pCountTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pCountTextBox->CreateSub( this, "LOTTO_WIN_MAN_LIST_BOX_PROPERTY_WIN_COUNT_TEXT_REGION", UI_FLAG_YSIZE );
		m_pCountTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pCountTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
		m_pCountTextBox->AddText( "", NS_UITEXTCOLOR::WHITE );
		m_pCountTextBox->SetVisibleSingle( TRUE );
		RegisterControl( m_pCountTextBox );
	}

	{
		m_pWinMoneyTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pWinMoneyTextBox->CreateSub( this, "LOTTO_WIN_MAN_LIST_BOX_PROPERTY_WIN_MONEY_TEXT_REGION", UI_FLAG_YSIZE );
		m_pWinMoneyTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pWinMoneyTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
		m_pWinMoneyTextBox->AddText( "", NS_UITEXTCOLOR::WHITE );
		m_pWinMoneyTextBox->SetVisibleSingle( TRUE );
		RegisterControl( m_pWinMoneyTextBox );
	}
}

void CLottoWinManListPropertyUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoWinManListPropertyUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );
}

void CLottoWinManListPropertyUIComp::SetPropertyInfo( const LottoSystem::LOTTO_WINMAN_LIST_PROPERTY_MEMBER propertyInfo )
{
	if( m_pRankingTextBox )
	{
		m_pRankingTextBox->SetOneLineText(
			propertyInfo.ranking.c_str(), NS_UITEXTCOLOR::WHITE );
	}

	if( m_pNameTextBox )
	{
		m_pNameTextBox->SetOneLineText(
			propertyInfo.name.c_str(), NS_UITEXTCOLOR::WHITE );
	}

	if( m_pCountTextBox )
	{
		m_pCountTextBox->SetOneLineText(
			propertyInfo.count.c_str(), NS_UITEXTCOLOR::WHITE );
	}

	if( m_pWinMoneyTextBox )
	{
		m_pWinMoneyTextBox->SetOneLineText(
			propertyInfo.winMoney.c_str(), NS_UITEXTCOLOR::WHITE );
	}
}