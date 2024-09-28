#include "StdAfx.h"
#include "SystemMessageWindow.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h" // by luxes
#include "../../InnerInterface.h"
#include "../Tutorial/TutorialDialogue.h" // by luxes.

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSystemMessageWindow::CSystemMessageWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pGaeaClient(pGaeaClient)
    , m_pTextBox ( NULL )
    , m_fElapsedTime ( 0.0f )
{
}

CSystemMessageWindow::~CSystemMessageWindow ()
{
}

void CSystemMessageWindow::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxSystemMessage ( "BASIC_SYSTEM_MESSAGE_LINE_BOX" );
	pLineBox->ResetAlignFlag ();
	RegisterControl ( pLineBox );	

	pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxSystemMessageUp ( "BASIC_SYSTEM_MESSAGE_LINE_TEXTBOX" );
	pLineBox->ResetAlignFlag ();
	RegisterControl ( pLineBox );

	CBasicTextBox* pMapMoveTextBox = new CBasicTextBox(m_pEngineDevice);
	pMapMoveTextBox->CreateSub ( this, "BASIC_SYSTEM_MESSAGE_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pMapMoveTextBox->SetFont ( pFont9 );
	pMapMoveTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
	pMapMoveTextBox->ResetAlignFlag ();
	RegisterControl ( pMapMoveTextBox );
	m_pTextBox = pMapMoveTextBox;
}

void CSystemMessageWindow::SetSystemMessage(const CString& strMessage, const D3DCOLOR& dwColor)
{
	if (m_pTextBox)
	{
		m_pTextBox->SetText(strMessage, dwColor);
  		m_fElapsedTime = 6.0f;
		m_pInterface->UiShowGroupTop(GetWndID (), true);
	}
}

void CSystemMessageWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if (!IsVisible())
        return;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	m_fElapsedTime -= fElapsedTime;
	ResetMessageEx ();

	if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
	{
		if ( ( m_pGaeaClient->GetTutorial()->IsQuestNHuntStep()
			&& m_pGaeaClient->GetTutorial()->IsSubThreeStep() )
			|| ( m_pGaeaClient->GetTutorial()->IsQuestNHuntStep()
			&& m_pGaeaClient->GetTutorial()->IsSubFourStep() )
			|| ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
			&& m_pGaeaClient->GetTutorial()->IsSubTwoStep() ) )
		{
			if ( m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
			{
				m_pInterface->UiHideGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
			}
		}
	}

	if ( m_fElapsedTime <= 0.0f )
	{
		m_pInterface->UiHideGroup ( GetWndID(), true );

		if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
		{
			if ( ( m_pGaeaClient->GetTutorial()->IsQuestNHuntStep()
				&& m_pGaeaClient->GetTutorial()->IsSubThreeStep() )
				|| ( m_pGaeaClient->GetTutorial()->IsQuestNHuntStep()
				&& m_pGaeaClient->GetTutorial()->IsSubFourStep() )
				|| ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
				&& m_pGaeaClient->GetTutorial()->IsSubTwoStep() ) )
			{
				if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW ) )
				{
					m_pInterface->UiShowGroupFocus( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );
				}
			}
		}
	}
}
