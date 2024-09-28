#include "stdafx.h"

#include "./LottoDrawingDateUIComp.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../Util/RnButton.h"
#include "../Util/CheckBox.h"

#include "../../../SigmaCore/DebugInclude.h"

#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"

CLottoDrawingDateUIComp::CLottoDrawingDateUIComp( CInnerInterface* pInterface
								 , EngineDeviceMan* pEngineDevice )
								 : CUIGroupHelper( pEngineDevice )
								 , m_pInterface( pInterface )
								 , m_pDateTextBox( NULL )
								 , m_pStartTimeTextBox( NULL )
								 , m_pEndTimeTextBox( NULL )
								 , m_pProgressTimeTextBox( NULL )
								 , m_pBuyStateTextBox( NULL )
{

}

CLottoDrawingDateUIComp::~CLottoDrawingDateUIComp() { }

void CLottoDrawingDateUIComp::CreateSubControl()
{
	// Line Box;
	{
		CreateLineBox( "LOTTO_DRAWING_DATE_COMP_DRAWING_DATE_REGION", "WHITE_LINE_BLACK_BACKGROUND" );
	}

	// Text Box;
	{
		CreateText( ID2GAMEWORD( "LOTTO_DRAWING_DATE_TEXT", 0 ),
			"LOTTO_DRAWING_DATE_COMP_DAY_TEXTBOX",
			TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );

		CreateText( ID2GAMEWORD( "LOTTO_DRAWING_DATE_TEXT", 1 ),
			"LOTTO_DRAWING_DATE_COMP_START_TIME_TEXTBOX",
			TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );

		CreateText( ID2GAMEWORD( "LOTTO_DRAWING_DATE_TEXT", 2 ),
			"LOTTO_DRAWING_DATE_COMP_END_TIME_TEXTBOX",
			TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );

		CreateText( ID2GAMEWORD( "LOTTO_DRAWING_DATE_TEXT", 3 ),
			"LOTTO_DRAWING_DATE_COMP_PROGRESS_TIME_TEXTBOX",
			TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );

		CreateText( ID2GAMEWORD( "LOTTO_DRAWING_DATE_TEXT", 4 ),
			"LOTTO_DRAWING_DATE_COMP_STATE_TEXTBOX",
			TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
	}

	// Changable Text Box;
	{
		m_pDateTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pDateTextBox->CreateSub ( this, "LOTTO_DRAWING_DATE_COMP_DAY_TEXTBOX_CHANGABLE", UI_FLAG_YSIZE );
		m_pDateTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pDateTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
		m_pDateTextBox->AddText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_DAY_TEXT" ), 1 ).c_str(),
			NS_UITEXTCOLOR::WHITE );
		RegisterControl ( m_pDateTextBox );

		m_pStartTimeTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pStartTimeTextBox->CreateSub ( this, "LOTTO_DRAWING_DATE_COMP_START_TIME_TEXTBOX_CHANGABLE", UI_FLAG_YSIZE );
		m_pStartTimeTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pStartTimeTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
		m_pStartTimeTextBox->AddText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_TIME_TEXT" ), "00", "00" ).c_str(),
			NS_UITEXTCOLOR::WHITE );
		RegisterControl ( m_pStartTimeTextBox );

		m_pEndTimeTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pEndTimeTextBox->CreateSub ( this, "LOTTO_DRAWING_DATE_COMP_END_TIME_TEXTBOX_CHANGABLE", UI_FLAG_YSIZE );
		m_pEndTimeTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pEndTimeTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
		m_pEndTimeTextBox->AddText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_TIME_TEXT" ), "00", "00" ).c_str(),
			NS_UITEXTCOLOR::WHITE );
		RegisterControl ( m_pEndTimeTextBox );

		m_pProgressTimeTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pProgressTimeTextBox->CreateSub ( this, "LOTTO_DRAWING_DATE_COMP_PROGRESS_TIME_TEXTBOX_CHANGABLE", UI_FLAG_YSIZE );
		m_pProgressTimeTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pProgressTimeTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
		m_pProgressTimeTextBox->AddText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_TIME_TEXT" ), "00", "00" ).c_str(),
			NS_UITEXTCOLOR::WHITE );
		RegisterControl ( m_pProgressTimeTextBox );

		m_pBuyStateTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pBuyStateTextBox->CreateSub ( this, "LOTTO_DRAWING_DATE_COMP_STATE_TEXTBOX_CHANGABLE", UI_FLAG_YSIZE );
		m_pBuyStateTextBox->SetFont ( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pBuyStateTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
		m_pBuyStateTextBox->AddText(
			ID2GAMEWORD( "LOTTO_DRAWING_DATE_STATE", 0 ),
			NS_UITEXTCOLOR::GREENYELLOW );
		RegisterControl ( m_pBuyStateTextBox );
	}
}

void CLottoDrawingDateUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoDrawingDateUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );
}

void CLottoDrawingDateUIComp::UpdateDrawingDateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_DRAWING_DATE startTime = pLottoSystem->GetStartTime();
	LottoSystem::LOTTO_DRAWING_DATE endTime = pLottoSystem->GetEndTime();

	if( m_pDateTextBox )
	{
		unsigned int date = static_cast<unsigned int>( startTime.sTime.GetDay() );
		std::string tempStrData;
		if( date >= 0 && date < 10 )
			tempStrData = sc::string::format( "0%d", date );
		else
			tempStrData = sc::string::format( "%d", date );

		std::string strDate = sc::string::format(
			ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_DAY_TEXT" ), date );
		m_pDateTextBox->SetOneLineText( strDate.c_str() );
	}

	if( m_pStartTimeTextBox )
	{
		unsigned int hour = startTime.GetHour();
		unsigned int minute = startTime.GetMinute();
		std::string tempStrHour;
		std::string tempStrMinute;

		if( hour >= 0 && hour < 10 )
			tempStrHour = sc::string::format( "0%d", hour );
		else
			tempStrHour = sc::string::format( "%d", hour );

		if( minute >= 0 && minute < 10 )
			tempStrMinute = sc::string::format( "0%d", minute );
		else
			tempStrMinute = sc::string::format( "%d", minute );

		std::string strDate = sc::string::format(
			ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_TIME_TEXT" ),
			tempStrHour, tempStrMinute );
		m_pStartTimeTextBox->SetOneLineText( strDate.c_str() );
	}

	if( m_pEndTimeTextBox )
	{
		unsigned int hour = endTime.GetHour();
		unsigned int minute = endTime.GetMinute();
		std::string tempStrHour;
		std::string tempStrMinute;

		if( hour >= 0 && hour < 10 )
			tempStrHour = sc::string::format( "0%d", hour );
		else
			tempStrHour = sc::string::format( "%d", hour );

		if( minute >= 0 && minute < 10 )
			tempStrMinute = sc::string::format( "0%d", minute );
		else
			tempStrMinute = sc::string::format( "%d", minute );

		std::string strDate = sc::string::format(
			ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_TIME_TEXT" ),
			tempStrHour, tempStrMinute );
		m_pEndTimeTextBox->SetOneLineText( strDate.c_str() );
	}
}

void CLottoDrawingDateUIComp::UpdateProgressTimeProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	if( m_pProgressTimeTextBox )
	{
		if( !pLottoSystem->GetCurProgressState() )
		{
			m_pProgressTimeTextBox->SetOneLineText(
				sc::string::format(
				ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_TIME_TEXT" ),
				"00", "00" ).c_str() );

			return;
		}

		std::string curProgressTime;
		LottoSystem::LOTTO_DRAWING_DATE progressState = 
			pLottoSystem->GetCurProgressTime();

		unsigned int hour = progressState.GetHour();
		unsigned int minute = progressState.GetMinute();
		std::string tempStrHour;
		std::string tempStrMinute;

		static unsigned int preMinute = 0;;
		if( hour == 0 && minute > 0 && minute <= 10 )
		{
			if( preMinute != minute )
			{
				preMinute = minute;

				m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_LOTTO_START,
					sc::string::format( ID2GAMEINTEXT("LOTTO_DRAWING_STATE_READY_TEXT"),
					minute, pLottoSystem->GetCurTurnNum()+1 ).c_str() );
			}
		}

		if( hour >= 0 && hour < 10 )
			tempStrHour = sc::string::format( "0%d", hour );
		else
			tempStrHour = sc::string::format( "%d", hour );

		if( minute >= 0 && minute < 10 )
			tempStrMinute = sc::string::format( "0%d", minute );
		else
			tempStrMinute = sc::string::format( "%d", minute );

		switch( pLottoSystem->GetLottoSystemState() )
		{
		case LottoSystem::LOTTO_STATE_GATHERING:
			{
				std::string temp = sc::string::format(
					ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_TIME_TEXT" ),
					"00",
					"00" );

				curProgressTime = sc::string::format(
					ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_PROGRESS_TEXT" ),
					temp );
			}
			break;
		case LottoSystem::LOTTO_STATE_BASIC:
		case LottoSystem::LOTTO_STATE_WAIT:
			{
				std::string temp = sc::string::format(
					ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_TIME_TEXT" ),
					tempStrHour,
					tempStrMinute );

				curProgressTime = sc::string::format(
					ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_PROGRESS_TEXT" ),
					temp );
			}
			break;
		case LottoSystem::LOTTO_STATE_DRAWING:
			{
				curProgressTime = sc::string::format(
					ID2GAMEINTEXT( "LOTTO_DRAWING_DATE_TIME_TEXT" ),
					tempStrHour,
					tempStrMinute );
			}
			break;
		}

		m_pProgressTimeTextBox->SetOneLineText(
			curProgressTime.c_str() );
	}
}

void CLottoDrawingDateUIComp::UpdateStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	if( m_pBuyStateTextBox )
	{
		switch( pLottoSystem->GetLottoSystemState() )
		{
		case LottoSystem::LOTTO_STATE_BASIC:
		case LottoSystem::LOTTO_STATE_GATHERING:
			m_pBuyStateTextBox->SetOneLineText( ID2GAMEWORD( "LOTTO_DRAWING_DATE_STATE", 0 ), NS_UITEXTCOLOR::GREENYELLOW );
			break;
		case LottoSystem::LOTTO_STATE_WAIT:
			m_pBuyStateTextBox->SetOneLineText( ID2GAMEWORD( "LOTTO_DRAWING_DATE_STATE", 1 ), NS_UITEXTCOLOR::GREENYELLOW );
			break;
		case LottoSystem::LOTTO_STATE_DRAWING:
			m_pBuyStateTextBox->SetOneLineText( ID2GAMEWORD( "LOTTO_DRAWING_DATE_STATE", 2 ), NS_UITEXTCOLOR::GREENYELLOW );
			break;
		}
	}
}