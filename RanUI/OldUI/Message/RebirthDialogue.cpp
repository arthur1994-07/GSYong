#include "StdAfx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"

#include "../../InnerInterface.h"
#include "RebirthDialogue.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float CRebirthDialogue::fDEFAULT_TIME_LEFT = 5.0f;
const float CRebirthDialogue::fDEFAULT_SELECT_TIME_LEFT = 10.0f;
const float CRebirthDialogue::fDEFAULT_REVIVE_TIME_LEFT = 15.0f;


CRebirthDialogue::CRebirthDialogue(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindow(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
	, m_pTextBox(NULL)
    , m_pCheckPointButton(NULL)
	, m_pCurrentPositionButton(NULL)
	, m_pRecoveryExpButton(NULL)
    , m_pReviveSkillButton(NULL)
	, m_pDummy_1BUTTON(NULL)
	, m_pDummy_2BUTTON_L(NULL)
	, m_pDummy_2BUTTON_R(NULL)
	, m_bDO_ACTION(false)
	, m_CallerID(NO_ID)
	, m_fTIME_LEFT(fDEFAULT_TIME_LEFT)
	, m_fSelTIME_LEFT( fDEFAULT_SELECT_TIME_LEFT )
	, m_nACTION(NO_ID)
	, m_bDONE_ACTION(false)
	, m_bAutoRebirth( false )
    , m_bReviveSkill( false ) 
{
}

CRebirthDialogue::~CRebirthDialogue ()
{
}

void CRebirthDialogue::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "REBIRTH_DIALOGUE_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "REBIRTH_DIALOGUE_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pTextBox->SetFont ( pFont9 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	pTextBox->SetPartInterval ( 5.0f );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;

	m_pCheckPointButton = CreateTextButton ( "REBIRTH_DIALOGUE_CHECKPOINT", REBIRTH_CHECK_POINT, (char*)ID2GAMEWORD ( "REBIRTH_BUTTON", 0 ) );
	m_pCurrentPositionButton = CreateTextButton ( "REBIRTH_DIALOGUE_CURRENT_POSITION", REBIRTH_CURRENT_POSITION, (char*)ID2GAMEWORD ( "REBIRTH_BUTTON", 1 ) );
	m_pRecoveryExpButton = CreateTextButton ( "REBIRTH_DIALOGUE_RECOVERY_EXP", REBIRTH_RECOVERY_EXP, (char*)ID2GAMEWORD ( "REBIRTH_BUTTON", 2 ) );
    m_pReviveSkillButton = CreateTextButton ( "REBIRTH_DIALOGUE_REVIVE_SKILL", REBIRTH_REVIVE_SKILL, (char*)ID2GAMEWORD ( "REBIRTH_BUTTON", 3 ) );

	m_pDummy_1BUTTON = CreateDummyControl ( "REBIRTH_DIALOGUE_1_BUTTON" );
	m_pDummy_2BUTTON_L = CreateDummyControl ( "REBIRTH_DIALOGUE_2_BUTTON_L" );
	m_pDummy_2BUTTON_R = CreateDummyControl ( "REBIRTH_DIALOGUE_2_BUTTON_R" );
}

CUIControl*	CRebirthDialogue::CreateDummyControl ( char* szKeyword )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szKeyword );
	pControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextButton* CRebirthDialogue::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

void CRebirthDialogue::SET_ITEMREBIRTH ( const bool& bItemRebirth, bool bAutoRebirth, bool bCantUseRebirth )
{
	m_bDO_ACTION = false;
	m_bDONE_ACTION = false;
	m_fTIME_LEFT = fDEFAULT_TIME_LEFT;
	m_fSelTIME_LEFT = fDEFAULT_SELECT_TIME_LEFT;

	m_bAutoRebirth = false;   

	if (bCantUseRebirth)
	{
        EMSERVICE_PROVIDER ServiceProvider = m_pEngineDevice->GetServiceProvider();
        // 경험치회복_정의_Normal
        if (ServiceProvider == SP_OFFICE_TEST ||
            ServiceProvider == SP_KOREA ||
            ServiceProvider == SP_KOREA_TEST ||
            ServiceProvider == SP_INDONESIA ||
            ServiceProvider == SP_WORLD_BATTLE)
        {
		    m_pTextBox->SetText ( ID2GAMEINTEXT("REBIRTH_DIALOGUE_TEXT3") );
        }
        else
        {
		    m_pTextBox->SetText ( ID2GAMEINTEXT("REBIRTH_DIALOGUE_TEXT2") );
        }
	}
    else
    {
		m_pTextBox->SetText ( ID2GAMEINTEXT("REBIRTH_DIALOGUE_TEXT") );
	}

	if ( m_bAutoRebirth )
	{
		CString strLeftTime;
		strLeftTime.Format ( "%1.0f%s", floor(m_fSelTIME_LEFT) + 1.0f, ID2GAMEWORD("WAITSERVER_TIMELEFT_UNIT"));		
		m_pTextBox->AddText ( strLeftTime );
	}

	bool bGuidBattleMap(false);
	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	if( pLand )
	{
		bGuidBattleMap = (pLand->m_bClubBattle || pLand->m_bClubDeathMatch ); 
	}

	ShowButton ();

	if ( bItemRebirth && !bGuidBattleMap )	// 귀혼주, 선도불가지역x, 귀혼불가지역x
	{
		{
			const UIRECT& rc2ButtonL_G = m_pDummy_2BUTTON_L->GetGlobalPos();
			const UIRECT& rc2ButtonL_L = m_pDummy_2BUTTON_L->GetLocalPos();
			
			m_pCheckPointButton->SetGlobalPos( rc2ButtonL_G );
			m_pCheckPointButton->SetLocalPos( rc2ButtonL_L );
			m_pCheckPointButton->SetFlip( TRUE );
		}
		{
			const UIRECT& rc2ButtonR_G = m_pDummy_2BUTTON_R->GetGlobalPos();
			const UIRECT& rc2ButtonR_L = m_pDummy_2BUTTON_R->GetLocalPos();

			m_pCurrentPositionButton->SetGlobalPos( rc2ButtonR_G );
			m_pCurrentPositionButton->SetLocalPos( rc2ButtonR_L );
			m_pCurrentPositionButton->SetFlip( TRUE );
		}

		m_pRecoveryExpButton->SetVisibleSingle ( FALSE );

		m_bAutoRebirth = bAutoRebirth;

	}    
	else if (!bCantUseRebirth && !bGuidBattleMap) // 귀혼주x, 귀혼불가지역x, 선도전지역x
	{
        EMSERVICE_PROVIDER ServiceProvider = m_pEngineDevice->GetServiceProvider();
        if (ServiceProvider == SP_OFFICE_TEST || 
            ServiceProvider == SP_KOREA || 
            ServiceProvider == SP_KOREA_TEST || 
            ServiceProvider == SP_INDONESIA ||
            ServiceProvider == SP_WORLD_BATTLE)
        {
            // 경험치회복_정의_Normal
		    {
			    const UIRECT& rc2ButtonL_G = m_pDummy_2BUTTON_L->GetGlobalPos();
			    const UIRECT& rc2ButtonL_L = m_pDummy_2BUTTON_L->GetLocalPos();
    			
			    m_pCheckPointButton->SetGlobalPos( rc2ButtonL_G );
			    m_pCheckPointButton->SetLocalPos( rc2ButtonL_L );
			    m_pCheckPointButton->SetFlip( TRUE );
		    }		
		    {
			    const UIRECT& rc2ButtonR_G = m_pDummy_2BUTTON_R->GetGlobalPos();
			    const UIRECT& rc2ButtonR_L = m_pDummy_2BUTTON_R->GetLocalPos();

			    m_pRecoveryExpButton->SetGlobalPos( rc2ButtonR_G );
			    m_pRecoveryExpButton->SetLocalPos( rc2ButtonR_L );
			    m_pRecoveryExpButton->SetFlip( TRUE );
		    }

		    {
			    m_pCurrentPositionButton->SetVisibleSingle ( FALSE );
		    }
        }
	}
	else
	{
		{
			const UIRECT& rc1BUTTON = m_pDummy_1BUTTON->GetGlobalPos ();
			D3DXVECTOR2 vPos ( rc1BUTTON.left, rc1BUTTON.top );
			m_pCheckPointButton->SetGlobalPos ( vPos );
			m_pCheckPointButton->SetFlip ( TRUE );
		}

		{
			m_pCurrentPositionButton->SetVisibleSingle ( FALSE );
			m_pRecoveryExpButton->SetVisibleSingle ( FALSE );
		}
	}


    if ( m_bReviveSkill ) 
        m_pReviveSkillButton->SetVisibleSingle( TRUE );
    else
        m_pReviveSkillButton->SetVisibleSingle( FALSE );
}

void CRebirthDialogue::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case REBIRTH_CHECK_POINT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
                m_nACTION = REBIRTH_CHECK_POINT;
				DoACTION ();
				HideButton ();
			}
		}
        break;

	case REBIRTH_CURRENT_POSITION:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_nACTION = REBIRTH_CURRENT_POSITION;
				DoACTION ();
                HideButton ();
			}
		}
		break;
	case REBIRTH_RECOVERY_EXP:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				
				m_nACTION = REBIRTH_RECOVERY_EXP;
				DoACTION ();
                HideButton ();				
			}
		}
		break;
    case REBIRTH_REVIVE_SKILL:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				
				m_nACTION = REBIRTH_REVIVE_SKILL;
				DoACTION ();
                HideButton ();				
			}
        }
        break;
	}
}

void CRebirthDialogue::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if( m_bDO_ACTION )
	{
		m_fTIME_LEFT -= fElapsedTime;

		bool bTimeEvent = false;
		if ( m_fTIME_LEFT < -1.0f ) bTimeEvent = true;

		if ( bTimeEvent )
		{
			if ( m_bDONE_ACTION ) return ;

			switch ( m_nACTION )
			{
			case REBIRTH_CHECK_POINT:
				{
					AddMessageEx ( UIMSG_MODAL_OK );
					m_pInterface->UiHideGroup ( GetWndID (), true );

					m_bDONE_ACTION = true;
				}
				break;

			case REBIRTH_CURRENT_POSITION:
				{
					AddMessageEx ( UIMSG_MODAL_CANCEL );
					m_pInterface->UiHideGroup ( GetWndID (), true );

					m_bDONE_ACTION = true;
				}
				break;
			case REBIRTH_RECOVERY_EXP:
				{
					AddMessageEx ( UIMSG_MODAL_CANCEL );
                    m_pInterface->UiHideGroup ( GetWndID (), true );

					m_bDONE_ACTION = true;
				}
				break;
            case REBIRTH_REVIVE_SKILL:
                {
                    AddMessageEx ( UIMSG_MODAL_CANCEL );
                    m_pInterface->UiHideGroup ( GetWndID (), true );

					m_bDONE_ACTION = true;
                }
                break;
            }
		}
		else
		{
			CString strLeftTime;
			strLeftTime.Format ( "%1.0f%s", floor(m_fTIME_LEFT) + 1.0f, ID2GAMEWORD("WAITSERVER_TIMELEFT_UNIT"));

			m_pTextBox->ClearText ();
			m_pTextBox->AddText ( ID2GAMEINTEXT("WAITSERVER_MESSAGE") );
			m_pTextBox->AddText ( strLeftTime );
		}	
	}

	else if ( m_bAutoRebirth )
	{
		m_fSelTIME_LEFT -= fElapsedTime;

		bool bTimeEvent = false;
		if ( m_fSelTIME_LEFT < -1.0f ) bTimeEvent = true;

		if ( bTimeEvent )
		{
			// Default 귀혼주 사용
			m_nACTION = REBIRTH_CURRENT_POSITION;
			
			DoACTION ();
			HideButton ();
		}
		else 
		{
			CString strLeftTime;
			strLeftTime.Format ( "%1.0f%s", floor(m_fSelTIME_LEFT) + 1.0f, ID2GAMEWORD("WAITSERVER_TIMELEFT_UNIT"));
	
			m_pTextBox->SetText ( ID2GAMEINTEXT("REBIRTH_DIALOGUE_TEXT") );
			m_pTextBox->AddText ( strLeftTime );
		}
	}

    if ( m_bReviveSkill ) 
    {
        m_fReviveTIME_LEFT -= fElapsedTime;

        if ( m_fReviveTIME_LEFT < -1.0f )
        {
            m_bReviveSkill = false;
            m_pReviveSkillButton->SetVisibleSingle( FALSE );
        }
    }

}


void CRebirthDialogue::HideButton ()
{
	m_pCheckPointButton->SetVisibleSingle ( FALSE );
	m_pCurrentPositionButton->SetVisibleSingle ( FALSE );
	m_pRecoveryExpButton->SetVisibleSingle( FALSE );
    m_pReviveSkillButton->SetVisibleSingle( FALSE );
}

void CRebirthDialogue::ShowButton ()
{
	m_pCheckPointButton->SetVisibleSingle ( TRUE );
	m_pCurrentPositionButton->SetVisibleSingle ( TRUE );
	m_pRecoveryExpButton->SetVisibleSingle( TRUE );
    m_pReviveSkillButton->SetVisibleSingle( TRUE );
}

void CRebirthDialogue::SetReviveSkill( bool bAct )
{
    if ( bAct ) 
    {
        m_bReviveSkill = true;
        m_fReviveTIME_LEFT = fDEFAULT_REVIVE_TIME_LEFT;

        //  액션이 진행중일때는 표시하지 않는다.
        if ( !m_bDO_ACTION )
        {
            m_pReviveSkillButton->SetVisibleSingle( TRUE );        
        }
    }
    else
    {
        m_bReviveSkill = false;
        m_pReviveSkillButton->SetVisibleSingle( FALSE );        
    }
}
