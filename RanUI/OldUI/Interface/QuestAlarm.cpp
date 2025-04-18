#include "StdAfx.h"
#include "QuestAlarm.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float CQuestAlarm::fBLINK_TIME_LOOP = 0.2f;

CQuestAlarm::CQuestAlarm(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
	, m_pAlarmBlink ( NULL )
    , m_fBLINK_TIME ( 0.0f )
    , m_bAlarm ( false )
{
}

CQuestAlarm::~CQuestAlarm ()
{
}

void CQuestAlarm::CreateSubControl ()
{
	CBasicButton* pQuestAlarm = new CBasicButton(m_pEngineDevice);
	pQuestAlarm->CreateSub ( this, "QUEST_ALARM_BUTTON", UI_FLAG_DEFAULT, QUEST_ALARM_BUTTON );
//#ifdef CH_PARAM // 중국 인터페이스 변경
//	pQuestAlarm->CreateMouseOver( "QUEST_ALARM_BUTTON_F" );
//#else
	//  [12/28/2012 gbgim]
	//pQuestAlarm->CreateFlip ( "QUEST_ALARM_BUTTON_F", CBasicButton::CLICK_FLIP );
	pQuestAlarm->SetUseDynamic ( TRUE );
//#endif
	pQuestAlarm->SetUseGlobalAction ( TRUE );
	RegisterControl ( pQuestAlarm );

	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "QUEST_ALARM_BLINK" );
	pControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pControl );
	m_pAlarmBlink = pControl;
}

void CQuestAlarm::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_bAlarm )
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

void CQuestAlarm::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if ( !bVisible )
	{
		m_pAlarmBlink->SetVisibleSingle ( FALSE );
		m_bAlarm = false;		
	}
}

void CQuestAlarm::SET_ALARM( bool bAlram )
{
	m_bAlarm = bAlram;

	if( false == m_bAlarm )
		m_pAlarmBlink->SetVisibleSingle( FALSE );
}