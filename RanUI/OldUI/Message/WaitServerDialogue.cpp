#include "StdAfx.h"

#include "WaitServerDialogue.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CWaitServerDialogue::CWaitServerDialogue(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIModal(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pTextBox ( NULL )
    , m_pMoveOKButton ( NULL )
    , m_fTIME_LEFT ( 0.0f )
{
}

CWaitServerDialogue::~CWaitServerDialogue ()
{
}

void CWaitServerDialogue::CreateSubControl ()
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

CBasicTextButton* CWaitServerDialogue::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

void CWaitServerDialogue::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_fTIME_LEFT -= fElapsedTime;

	if ( m_fTIME_LEFT < 0.0f) m_fTIME_LEFT = 0.0f;

	CUIModal::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

	CString strLeftTime;
	strLeftTime.Format ( "%1.0f%s", floor(m_fTIME_LEFT), ID2GAMEWORD("WAITSERVER_TIMELEFT_UNIT"));

	m_pTextBox->ClearText ();
	m_pTextBox->AddText ( m_strMessage );
	m_pTextBox->AddText ( strLeftTime );

	if ( m_fTIME_LEFT < 1.0f ) DoACTION_PLAY ();
}

void CWaitServerDialogue::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case WAITSERVER_OKBUTTON:
		{
			bool bBUTTONOK = false;
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_MOUSEIN_OKBUTTON );

				if ( UIMSG_LB_UP & dwMsg )
				{
					bBUTTONOK = true;
				}
			}

			if ( bBUTTONOK )
			{
				switch ( m_nACTION )
				{
				case WAITSERVER_CLOSEGAME:
					{
						// 프로그램을 종료한다.
                        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
						pGlobalStage->CloseGame();
						m_pInterface->WAITSERVER_DIALOGUE_CLOSE();
					}
					break;

				case WAITSERVER_TOLOBY:
					{
						// 서버 선택으로 이동한다.
                        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
						pGlobalStage->GameToLobbyStage(false);
						m_pInterface->WAITSERVER_DIALOGUE_CLOSE();
					}
					break;
				case WAITSERVER_TO_WORLDBATTLE:
					{
						// 서버 선택으로 이동한다.
                        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
						pGlobalStage->GameToLobbyStage(true);
						m_pInterface->WAITSERVER_DIALOGUE_CLOSE();
					}
					break;

				case WAITSERVER_TOFRIEND:
					{
							m_pInterface->WAITSERVER_DIALOGUE_CLOSE ();
							return ;
					}
					break;
				}
			}
		}
        break;
	}
}

void	CWaitServerDialogue::SetDisplayMessage ( const CString& strDisplay, const int nAction, const float fTimer )
{
	if ( m_pTextBox )
	{
		m_nACTION = nAction;
		m_fTIME_LEFT = fTimer;

		m_strMessage = strDisplay;

		if ( nAction == WAITSERVER_TOFRIEND )
			m_pMoveOKButton->SetOneLineText ( (char*)ID2GAMEWORD ( "WAITSERVER_CANCELBUTTON" ) );
		else
			m_pMoveOKButton->SetOneLineText ( (char*)ID2GAMEWORD ( "WAITSERVER_OKBUTTON" ) );			
	}
}

void CWaitServerDialogue::DoACTION_PLAY ()
{
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	switch(m_nACTION)
	{
	case WAITSERVER_CLOSEGAME:
		{
			pGlobalStage->CloseGame();
			m_pInterface->WAITSERVER_DIALOGUE_CLOSE();
		}
		break;
	case WAITSERVER_TOLOBY:
		{
			pGlobalStage->GameToLobbyStage(false);
			m_pInterface->WAITSERVER_DIALOGUE_CLOSE();
		}
		break;
	case WAITSERVER_TO_WORLDBATTLE:
		{
			pGlobalStage->GameToLobbyStage(true);
			m_pInterface->WAITSERVER_DIALOGUE_CLOSE();
		}
		break;
	case WAITSERVER_TOFRIEND:
		{
			CString strName = m_pInterface->GetFriendName ();
			m_pGaeaClient->GetCharacter ()->Req2Friend ( strName );
			m_pInterface->WAITSERVER_DIALOGUE_CLOSE ();
		}
		break;
	default:
		break;
	}
}