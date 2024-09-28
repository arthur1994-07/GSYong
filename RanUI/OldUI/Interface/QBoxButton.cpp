#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../InnerInterface.h"
#include ".\qboxbutton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../Display/MiniGauge.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CQBoxButton::CQBoxButton(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_bQBoxEnable(TRUE)
    , m_bQBoxAlarm(FALSE)
    , m_fBLINK_TIME(0.0f)
    , m_nType(QBOX_NONE)
    , m_pQBoxButtonLock(NULL)
    , m_pAlarmBlink(NULL)
    , m_pGauge(NULL)
{
	for ( int i = 0; i < QBOX_TYPE_SIZE; i++ )
		m_pQBoxButton[ i ] = NULL;
}

CQBoxButton::~CQBoxButton(void)
{
}

void CQBoxButton::SetQBoxEnable( bool bQBoxEnable )
{
	if( m_bQBoxEnable != bQBoxEnable )
	{
		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if( pCharacter->IsPartyMem() && pCharacter->IsPartyMaster() == FALSE )
		{
			m_bQBoxAlarm = TRUE;
		}
	}

	m_bQBoxEnable = bQBoxEnable;
	m_pQBoxButtonLock->SetVisibleSingle( !m_bQBoxEnable );
	//m_pQBoxButton[ m_nType ]->SetFlip( m_bQBoxEnable );
}


void CQBoxButton::CreateSubControl ()
{
	{	// QBOX typeº°·Î create. by luxes.
		m_pQBoxButton[ 0 ] = new CBasicButton(m_pEngineDevice);
		m_pQBoxButton[ 0 ]->CreateSub ( this, "QBOX_BUTTON_BUTTON", UI_FLAG_DEFAULT, QBOX_NONE_BUTTON );
		m_pQBoxButton[ 0 ]->CreateFlip ( "QBOX_BUTTON_BUTTON_F", CBasicButton::RADIO_FLIP );
		m_pQBoxButton[ 0 ]->SetUseGlobalAction ( TRUE );
		m_pQBoxButton[ 0 ]->SetFlip( m_bQBoxEnable );
		RegisterControl ( m_pQBoxButton[ 0 ] );

		m_pQBoxButton[ 1 ] = new CBasicButton(m_pEngineDevice);
		m_pQBoxButton[ 1 ]->CreateSub ( this, "Q_BOX_POWER_UP", UI_FLAG_DEFAULT, QBOX_POWER_UP_BUTTON );
		m_pQBoxButton[ 1 ]->CreateFlip ( "Q_BOX_POWER_UP_F", CBasicButton::RADIO_FLIP );
		m_pQBoxButton[ 1 ]->SetUseGlobalAction ( TRUE );
		m_pQBoxButton[ 1 ]->SetFlip( m_bQBoxEnable );
		RegisterControl ( m_pQBoxButton[ 1 ] );

		m_pQBoxButton[ 2 ] = new CBasicButton(m_pEngineDevice);
		m_pQBoxButton[ 2 ]->CreateSub ( this, "Q_BOX_POWER_UP_MAX", UI_FLAG_DEFAULT, QBOX_POWER_UP_MAX_BUTTON );
		m_pQBoxButton[ 2 ]->CreateFlip ( "Q_BOX_POWER_UP_MAX_F", CBasicButton::RADIO_FLIP );
		m_pQBoxButton[ 2 ]->SetUseGlobalAction ( TRUE );
		m_pQBoxButton[ 2 ]->SetFlip( m_bQBoxEnable );
		RegisterControl ( m_pQBoxButton[ 2 ] );

		m_pQBoxButton[ 3 ] = new CBasicButton(m_pEngineDevice);
		m_pQBoxButton[ 3 ]->CreateSub ( this, "Q_BOX_EXP_TIME", UI_FLAG_DEFAULT, QBOX_EXP_TIME_BUTTON );
		m_pQBoxButton[ 3 ]->CreateFlip ( "Q_BOX_EXP_TIME_F", CBasicButton::RADIO_FLIP );
		m_pQBoxButton[ 3 ]->SetUseGlobalAction ( TRUE );
		m_pQBoxButton[ 3 ]->SetFlip( m_bQBoxEnable );
		RegisterControl ( m_pQBoxButton[ 3 ] );

		m_pQBoxButton[ 4 ] = new CBasicButton(m_pEngineDevice);
		m_pQBoxButton[ 4 ]->CreateSub ( this, "Q_BOX_SPEED_UP", UI_FLAG_DEFAULT, QBOX_SPEED_UP_BUTTON );
		m_pQBoxButton[ 4 ]->CreateFlip ( "Q_BOX_SPEED_UP_F", CBasicButton::RADIO_FLIP );
		m_pQBoxButton[ 4 ]->SetUseGlobalAction ( TRUE );
		m_pQBoxButton[ 4 ]->SetFlip( m_bQBoxEnable );
		RegisterControl ( m_pQBoxButton[ 4 ] );

		m_pQBoxButton[ 5 ] = new CBasicButton(m_pEngineDevice);
		m_pQBoxButton[ 5 ]->CreateSub ( this, "Q_BOX_SPEED_UP_MAX", UI_FLAG_DEFAULT, QBOX_SPEED_UP_MAX_BUTTON );
		m_pQBoxButton[ 5 ]->CreateFlip ( "Q_BOX_SPEED_UP_MAX_F", CBasicButton::RADIO_FLIP );
		m_pQBoxButton[ 5 ]->SetUseGlobalAction ( TRUE );
		m_pQBoxButton[ 5 ]->SetFlip( m_bQBoxEnable );
		RegisterControl ( m_pQBoxButton[ 5 ] );

		m_pQBoxButton[ 6 ] = new CBasicButton(m_pEngineDevice);
		m_pQBoxButton[ 6 ]->CreateSub ( this, "Q_BOX_CRAZY_TIME", UI_FLAG_DEFAULT, QBOX_CRAZY_TIME_BUTTON );
		m_pQBoxButton[ 6 ]->CreateFlip ( "Q_BOX_CRAZY_TIME_F", CBasicButton::RADIO_FLIP );
		m_pQBoxButton[ 6 ]->SetUseGlobalAction ( TRUE );
		m_pQBoxButton[ 6 ]->SetFlip( m_bQBoxEnable );
		RegisterControl ( m_pQBoxButton[ 6 ] );

		m_pQBoxButton[ 7 ] = new CBasicButton(m_pEngineDevice);
		m_pQBoxButton[ 7 ]->CreateSub ( this, "Q_BOX_MADNESS_TIME", UI_FLAG_DEFAULT, QBOX_MADNESS_TIME_BUTTON );
		m_pQBoxButton[ 7 ]->CreateFlip ( "Q_BOX_MADNESS_TIME_F", CBasicButton::RADIO_FLIP );
		m_pQBoxButton[ 7 ]->SetUseGlobalAction ( TRUE );
		m_pQBoxButton[ 7 ]->SetFlip( m_bQBoxEnable );
		RegisterControl ( m_pQBoxButton[ 7 ] );

		m_pQBoxButton[ 8 ] = new CBasicButton(m_pEngineDevice);
		m_pQBoxButton[ 8 ]->CreateSub ( this, "Q_BOX_LUCKY", UI_FLAG_DEFAULT, QBOX_LUCKY_BUTTON );
		m_pQBoxButton[ 8 ]->CreateFlip ( "Q_BOX_LUCKY_F", CBasicButton::RADIO_FLIP );
		m_pQBoxButton[ 8 ]->SetUseGlobalAction ( TRUE );
		m_pQBoxButton[ 8 ]->SetFlip( m_bQBoxEnable );
		RegisterControl ( m_pQBoxButton[ 8 ] );

		for ( int i = 0; i < QBOX_TYPE_SIZE; i++ )
			m_pQBoxButton[ i ]->SetVisibleSingle ( FALSE );

		m_pQBoxButton[ m_nType ]->SetVisibleSingle( TRUE );
	}

	m_pGauge = new CMiniGauge ( m_pGaeaClient, m_pEngineDevice );
	m_pGauge->CreateSub( this, "QBOX_GAUGE_BOX", UI_FLAG_DEFAULT, QBOX_GAUGE );
	m_pGauge->CreateSubControl();
	m_pGauge->SetVisibleSingle( FALSE );
	RegisterControl( m_pGauge );

	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "QBOX_BUTTON_BUTTON_LOCK", UI_FLAG_DEFAULT, QBOX_LOCK_BUTTON );
	pControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pControl );
	m_pQBoxButtonLock = pControl;

	pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "QBOX_ALARM_BLINK", UI_FLAG_DEFAULT, QBOX_ALARM );
	pControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pControl );
	m_pAlarmBlink = pControl;
}

void CQBoxButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	const float fBLINK_TIME_LOOP = 0.2f;

	if ( m_bQBoxAlarm )
	{		
		m_fBLINK_TIME += fElapsedTime;
		if ( fBLINK_TIME_LOOP <= m_fBLINK_TIME )
		{
			BOOL bVisible = m_pAlarmBlink->IsVisible ();
			m_pAlarmBlink->SetVisibleSingle ( !bVisible );
			m_fBLINK_TIME = 0.0f;
		}
	}
}


void CQBoxButton::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case QBOX_NONE_BUTTON:
	case QBOX_NONE_BUTTON+1:
	case QBOX_NONE_BUTTON+2:
	case QBOX_NONE_BUTTON+3:
	case QBOX_NONE_BUTTON+4:
	case QBOX_NONE_BUTTON+5:
	case QBOX_NONE_BUTTON+6:
	case QBOX_NONE_BUTTON+7:
	case QBOX_NONE_BUTTON+8:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT( "QBOX_BUTTON_MESSAGE" ), NS_UITEXTCOLOR::WHITE  );				
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
				if( !pCharacter->IsPartyMem() || pCharacter->IsPartyMaster() )
				{
					m_bQBoxEnable = !m_bQBoxEnable;
					m_pQBoxButtonLock->SetVisibleSingle( !m_bQBoxEnable );
					//m_pQBoxButton[ m_nType ]->SetFlip( m_bQBoxEnable );

					if( pCharacter->IsPartyMaster() )
					{
						pCharacter->ReqQBoxEnableState(m_bQBoxEnable);		
					}
				}else{
					m_bQBoxAlarm = FALSE;
					m_pAlarmBlink->SetVisibleSingle ( FALSE );
				}
			}
		}
		break;
	}
}

void CQBoxButton::SetQBoxType( int nType )
{
	switch ( nType )
	{
	case QUESTION_SPEED_UP:		m_nType = QBOX_SPEED_UP;		break;
	case QUESTION_CRAZY:		m_nType = QBOX_CRAZY_TIME;		break;
	case QUESTION_ATTACK_UP:	m_nType = QBOX_POWER_UP;		break;
	case QUESTION_EXP_UP:		m_nType = QBOX_EXP_TIME;		break;
	case QUESTION_LUCKY:		m_nType = QBOX_LUCKY;			break;
	case QUESTION_SPEED_UP_M:	m_nType = QBOX_SPEED_UP_MAX;	break;
	case QUESTION_MADNESS:		m_nType = QBOX_MADNESS_TIME;	break;
	case QUESTION_ATTACK_UP_M:	m_nType = QBOX_POWER_UP_MAX;	break;
	default:					return;
	}

	for ( int i = 0; i < QBOX_TYPE_SIZE; i++ )
		m_pQBoxButton[ i ]->SetVisibleSingle ( FALSE );

	m_pQBoxButton[ m_nType ]->SetVisibleSingle( TRUE );

	m_pGauge->SetVisibleSingle( TRUE );
	m_pGauge->SetRedBar( m_pGaeaClient->GetCharacter()->m_sQITEMFACT.fTime, true );
}

void CQBoxButton::ResetQBox()
{
	m_nType = QBOX_NONE;

	for ( int i = 0; i < QBOX_TYPE_SIZE; i++ )
		m_pQBoxButton[ i ]->SetVisibleSingle ( FALSE );

	m_pQBoxButton[ m_nType ]->SetVisibleSingle( TRUE );

	m_pGauge->SetVisibleSingle( FALSE );
}

void CQBoxButton::QBOXButtonDown()
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter )
	{
		if( !pCharacter->IsPartyMem() || pCharacter->IsPartyMaster() )
		{
			m_bQBoxEnable = !m_bQBoxEnable;
			m_pQBoxButtonLock->SetVisibleSingle( !m_bQBoxEnable );
			//m_pQBoxButton[ m_nType ]->SetFlip( m_bQBoxEnable );

			if( pCharacter->IsPartyMaster() )
			{
				pCharacter->ReqQBoxEnableState(m_bQBoxEnable);		
			}
		}else{
			m_bQBoxAlarm = FALSE;
			m_pAlarmBlink->SetVisibleSingle ( FALSE );
		}
	}
}
