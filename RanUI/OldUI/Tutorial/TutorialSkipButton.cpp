#include "StdAfx.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/File/StringFile.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../EngineLib/GUInterface/NSGUI.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"

#include "../../InnerInterface.h"

#include "TutorialSkipButton.h"

//#include "../../ModalCallerID.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTutorialSkipButton::CTutorialSkipButton(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pSkipButton(NULL)
    , m_pSkipText(NULL)
    , m_bSkipEnable(true)
{
}

CTutorialSkipButton::~CTutorialSkipButton(void)
{
}

void CTutorialSkipButton::SetSkipEnable( bool bSkipEnable )
{
	m_bSkipEnable = bSkipEnable;
	m_pSkipButton->SetFlip( m_bSkipEnable );
}


void CTutorialSkipButton::CreateSubControl ()
{
	{
		m_pSkipButton = new CBasicButton(m_pEngineDevice);
		m_pSkipButton->CreateSub ( this, "TUTORIAL_SKIP_BUTTON", UI_FLAG_DEFAULT, TUTORIAL_SKIP_BUTTON );
		m_pSkipButton->CreateFlip ( "TUTORIAL_SKIP_BUTTON_F", CBasicButton::KEEP_FLIP ); // RADIO_FLIP
		m_pSkipButton->SetUseGlobalAction ( TRUE );
		m_pSkipButton->SetFlip( m_bSkipEnable );
		RegisterControl ( m_pSkipButton );
	}

	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

		CBasicTextBox* pSkipText = new CBasicTextBox(m_pEngineDevice);
		pSkipText->CreateSub ( this, "TUTORIAL_SKIP_BUTTON_TEXT" );
		pSkipText->SetFont ( pFont );
		pSkipText->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl ( pSkipText );
		m_pSkipText = pSkipText;

		if ( m_pSkipText )
		{
			CString strCombine;
			strCombine = ID2GAMEINTEXT("TUTORIAL_SKIP");
			m_pSkipText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
			m_pSkipText->SetVisibleSingle( TRUE );
		}
	}
}

void CTutorialSkipButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


void CTutorialSkipButton::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case TUTORIAL_SKIP_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pSkipButton->SetFlip( m_bSkipEnable );
			}
			else
			{
				m_pSkipButton->SetFlip( !m_bSkipEnable );
			}

			if ( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
			{
				NSGUI::SetCharMoveBlock();
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pSkipButton->SetFlip( !m_bSkipEnable );

				NSGUI::SetCharMoveBlock();

				m_pInterface->DoModal( ID2GAMEINTEXT("TUTORIAL_SKIP_TEXT"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_TUTORIAL_SKIP );
			}
		}
		break;
	}
}
