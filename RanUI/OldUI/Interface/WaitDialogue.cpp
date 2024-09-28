#include "StdAfx.h"
#include "WaitDialogue.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../OuterInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CWaitDialogue::CWaitDialogue ( COuterInterface* pInterface, EngineDeviceMan* pEngineDevice ) 
    : CUIModal(pEngineDevice)
	, m_pInterface ( pInterface )
    , m_pTextBox ( NULL )
    , m_pMoveOKButton ( NULL )
    , m_fTIME_LEFT ( 0.0f )
    , m_bDO_ACTION ( false )
    , m_nACTION ( WAITSERVER_DUMMY )
    , m_PrevPageID ( NO_ID )
{
}

CWaitDialogue::~CWaitDialogue ()
{
}

void CWaitDialogue::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "WAITSERVER_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "WAITSERVER_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pTextBox->SetFont ( pFont9 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	pTextBox->SetLineInterval ( 6.0f );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;

	m_pMoveOKButton = CreateTextButton ( "WAITSERVER_OK", WAITSERVER_OKBUTTON, (char*)ID2GAMEWORD ( "WAITSERVER_OKBUTTON" ) );	
	m_pMoveOKButton->SetShortcutKey ( DIK_RETURN );
}

CBasicTextButton* CWaitDialogue::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

void CWaitDialogue::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;
	if ( m_nACTION == WAITSERVER_DUMMY )
	{
		m_pInterface->WAITSERVER_DIALOGUE_CLOSE ();
	}

	m_fTIME_LEFT -= fElapsedTime;

	if ( m_fTIME_LEFT < 1.0f ) DoACTION ();

	CUIModal::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

	CString strLeftTime;
	strLeftTime.Format ( "%1.0f%s", floor(m_fTIME_LEFT), ID2GAMEWORD("WAITSERVER_TIMELEFT_UNIT"));

	m_pTextBox->ClearText ();
	m_pTextBox->AddText ( m_strMessage );
	m_pTextBox->AddText ( strLeftTime );
}

void CWaitDialogue::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case WAITSERVER_OKBUTTON:
		{
			bool bBUTTONOK = false;
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_MOUSEIN_OKBUTTON1 );

				if ( UIMSG_LB_UP & dwMsg )
				{
					bBUTTONOK = true;
				}
			}

			if ( CHECK_KEYFOCUSED ( dwMsg ) || m_bDO_ACTION ) bBUTTONOK = true;

			if ( bBUTTONOK )
			{
				switch ( m_nACTION )
				{
				case WAITSERVER_DEL_CHARACTER:
					{
						//	친구에게 이동일때는 '취소'임.
						if ( !m_bDO_ACTION ) 
						{
							m_pInterface->WAITSERVER_DIALOGUE_CLOSE ();
							return ;
						}

						//	NOTE						
						//		캐릭터를 삭제한다.
						m_pInterface->DeleteCharacter ();
						m_pInterface->WAITSERVER_DIALOGUE_CLOSE ();

						m_pInterface->DoModal( ID2GAMEEXTEXT("CHAR_DELETE_ON"), UI::MODAL_INFOMATION, UI::NO_CONTROL );
					}
					break;
				}
			}
		}
        break;
	}
}

void	CWaitDialogue::SetDisplayMessage ( const CString& strDisplay, const int nAction, const float fTimer )
{
	if ( m_pTextBox )
	{
		m_nACTION = nAction;
		m_fTIME_LEFT = fTimer;

		m_strMessage = strDisplay;

		m_bDO_ACTION = false;

		if ( nAction == WAITSERVER_DEL_CHARACTER )
			m_pMoveOKButton->SetOneLineText ( (char*)ID2GAMEWORD ( "WAITSERVER_CANCELBUTTON" ) );
	}
}