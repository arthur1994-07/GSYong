#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../Util/UITextButton.h"

#include "./CompetitionObject.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCompetitionObject::CCompetitionObject ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowObject ( pInterface, pEngineDevice )
{
	memset(m_pTapButton, 0, sizeof(m_pTapButton));
}

CCompetitionObject::~CCompetitionObject ()
{
	// Blank
}

void CCompetitionObject::CreateSubControlEx ()
{	
	// Note : 페이지 프레임
	{
		CreateLineBox  ( "COMPETITION_OBJECT_PAGE", "COMPETITION_OBJECT_PAGE_LINEBOX_TEXINFO" );
		CreatePageFrame( "COMPETITION_OBJECT_PAGE" );
	}

	// Note : 탭 버튼
	{
		std::string strTapControlFormat("COMPETITION_OBJECT_TAP_%1%");
		UINT        nTapCount = 0;

		CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        int i = 0;

		if ( GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
		{
			std::string strTapControl = sc::string::format( strTapControlFormat, i );
			STAPBUTTON* pTapButton = new STAPBUTTON(m_pEngineDevice);
			pTapButton->CreateSub ( this, strTapControl.c_str(), UI_FLAG_DEFAULT, COMPETITION_OBJECT_TAP_CTF + i );
			pTapButton->CreateSubControl( ID2GAMEWORD("COMPETITION_PAGE", i),
				"COMPETITION_OBJECT_TAP_BASE","COMPETITION_OBJECT_TAP_TEXINFO_ACTIVE",
				"COMPETITION_OBJECT_TAP_BASE","COMPETITION_OBJECT_TAP_TEXINFO_INACTIVE",
				"COMPETITION_OBJECT_TAP_BASE");

			RegisterControl ( pTapButton );
			m_pTapButton[i] = pTapButton;
			RegisterTapButton( m_pTapButton[i] );
		}

        ++i;

        if ( GLUseFeatures::GetInstance().IsUsingInfinityStairs() )
        {
            std::string strTapControl = sc::string::format( strTapControlFormat, i );
            STAPBUTTON* pTapButton = new STAPBUTTON(m_pEngineDevice);
            pTapButton->CreateSub ( this, strTapControl.c_str(), UI_FLAG_DEFAULT, COMPETITION_OBJECT_TAP_CTF + i );
            pTapButton->CreateSubControl( ID2GAMEWORD("COMPETITION_PAGE", i),
                "COMPETITION_OBJECT_TAP_BASE","COMPETITION_OBJECT_TAP_TEXINFO_ACTIVE",
                "COMPETITION_OBJECT_TAP_BASE","COMPETITION_OBJECT_TAP_TEXINFO_INACTIVE",
                "COMPETITION_OBJECT_TAP_BASE");

            RegisterControl ( pTapButton );
            m_pTapButton[i] = pTapButton;
            RegisterTapButton( m_pTapButton[i] );
        }

		++i;

		if (GLUseFeatures::GetInstance().IsUsingTournament())
		{
			std::string strTapControl = sc::string::format( strTapControlFormat, i );
			STAPBUTTON* pTapButton = new STAPBUTTON(m_pEngineDevice);
			pTapButton->CreateSub ( this, strTapControl.c_str(), UI_FLAG_DEFAULT, COMPETITION_OBJECT_TAP_CTF + i );
			pTapButton->CreateSubControl( ID2GAMEWORD("COMPETITION_PAGE", i),
				"COMPETITION_OBJECT_TAP_BASE","COMPETITION_OBJECT_TAP_TEXINFO_ACTIVE",
				"COMPETITION_OBJECT_TAP_BASE","COMPETITION_OBJECT_TAP_TEXINFO_INACTIVE",
				"COMPETITION_OBJECT_TAP_BASE");

			RegisterControl ( pTapButton );
			m_pTapButton[i] = pTapButton;
			RegisterTapButton( m_pTapButton[i] );
		}


		TapSelect( TAP_DEFAULT );
	}

	// Note : 닫기 버튼
	{
		m_pCloseButton = new CBasicTextButton(m_pEngineDevice);
		m_pCloseButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, COMPETITION_OBJECT_CLOSE);
		m_pCloseButton->CreateBaseButton( "COMPETITION_OBJECT_CLOSE", CBasicTextButton::SIZE22, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_WINDOW", 1) );
		RegisterControl( m_pCloseButton );
	}
}

void CCompetitionObject::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case COMPETITION_OBJECT_TAP_CTF :
	case COMPETITION_OBJECT_TAP_PVE :
	case COMPETITION_OBJECT_TAP_TOURNAMENT:
		{
			EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
			TapSelectorUIMessage( ControlID, dwMsg, emResult );

			if ( emResult == EMTAPSELECTORRESULT_SELECT )
			{
				AddMessageEx ( UIMSG_COMPETITION_OBJECT_TAP_CHANGE );
			}
		}
		break;
	case COMPETITION_OBJECT_CLOSE :
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_WINDOW_OBJECT_CLOSE );
			}
		}
		break;
	}

	CUIWindowObject::TranslateUIMessage ( ControlID, dwMsg );
}

void CCompetitionObject::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIWindowObject::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


// void CCompetitionObject::TapEnable  ( TAPSELECTOR::BUTTON* pButton )
// {
// 	if ( pButton )
// 	{
// 		pButton->SetEnable( true );
// 	}
// }

// void CCompetitionObject::TapDisable ( TAPSELECTOR::BUTTON* pButton )
// {
// 	if ( pButton )
// 	{
// 		pButton->SetEnable( false );
// 	}
// }

CBasicLineBoxSmart* CCompetitionObject::CreateLineBox( const std::string& strControl, const std::string& strTexInfo )
{
	CBasicLineBoxSmart* pLineBox;
	pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub( this, strControl.c_str() );
	pLineBox->CreateSubControl( strTexInfo.c_str() );
	RegisterControl( pLineBox );

	return pLineBox;
}

//----------------------------------------------------------------------------------------------------//

// void CCompetitionObject::STAPBUTTON::CreateSubControl ( const std::string& strLabel )
// {
// 	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
// 
// 	ACTIVE = new CBasicLineBoxSmart(m_pEngineDevice);
// 	ACTIVE->CreateSub( this, "COMPETITION_OBJECT_TAP_BASE" );
// 	ACTIVE->CreateSubControl( "COMPETITION_OBJECT_TAP_TEXINFO_ACTIVE" );
// 	RegisterControl( ACTIVE );
// 
// 	INACTIVE = new CBasicLineBoxSmart(m_pEngineDevice);
// 	INACTIVE->CreateSub( this, "COMPETITION_OBJECT_TAP_BASE" );
// 	INACTIVE->CreateSubControl( "COMPETITION_OBJECT_TAP_TEXINFO_INACTIVE" );
// 	RegisterControl( INACTIVE );
// 
// 	TEXTBOX = new CBasicTextBox(m_pEngineDevice);
// 	TEXTBOX->CreateSub( this, "COMPETITION_OBJECT_TAP_BASE" );
// 	TEXTBOX->SetFont( pFont );
// 	TEXTBOX->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
// 	TEXTBOX->SetText( strLabel.c_str() );
// 	TEXTBOX->SetUseTextColor( 0, TRUE );
// 	RegisterControl( TEXTBOX );
// }
// 
// void CCompetitionObject::STAPBUTTON::SetEnable ( const bool bEnable )
// {
// 	ENABLE = bEnable;
// 
// 	if ( bEnable )
// 	{
// 		ACTIVE->SetVisibleSingle( TRUE );
// 		INACTIVE->SetVisibleSingle( FALSE );
// 		TEXTBOX->SetTextColor(0, NS_UITEXTCOLOR::BLACK);
// 	}
// 	else
// 	{
// 		ACTIVE->SetVisibleSingle( FALSE );
// 		INACTIVE->SetVisibleSingle( TRUE );
// 		TEXTBOX->SetTextColor(0, NS_UITEXTCOLOR::WHITE);
// 	}
// }
