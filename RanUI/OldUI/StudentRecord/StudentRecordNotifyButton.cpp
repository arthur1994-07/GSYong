#include "StdAfx.h"
#include "./StudentRecordNotifyButton.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogicClient/Attendance/GLAttendanceClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CStudentRecordNotifyButton::CStudentRecordNotifyButton(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient ( pGaeaClient )
    , m_pInterface  ( pInterface )
    , m_pAlarm      ( NULL )
    , m_bAlarm      ( false )
{
}

CStudentRecordNotifyButton::~CStudentRecordNotifyButton ()
{
}

void CStudentRecordNotifyButton::CreateSubControl ()
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "STUDENTRECORD_NOTIFY_BUTTON_IMAGE", UI_FLAG_DEFAULT, NOTIFY_BUTTON );
    pButton->SetUseGlobalAction ( TRUE );
    RegisterControl ( pButton );

    m_pAlarm = new CUIControl(m_pEngineDevice);
    m_pAlarm->CreateSub( this, "STUDENTRECORD_NOTIFY_ALARM" );
    m_pAlarm->SetVisibleSingle( FALSE );
    RegisterControl ( m_pAlarm );

    Update ();
}

void CStudentRecordNotifyButton::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case NOTIFY_BUTTON:
        {   
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( m_pInterface->BEGIN_COMMON_LINEINFO_MULTI( TEXT_ALIGN_LEFT ) )
                {
                    for ( ITER_VEC_STRING iter = m_vecInfo.begin(); iter != m_vecInfo.end(); ++iter )
                    {
                        m_pInterface->ADD_COMMON_LINEINFO_MULTI( (*iter).c_str(), NS_UITEXTCOLOR::WHITE );
                    }

                    m_pInterface->END_COMMON_LINEINFO_MULTI();
                }            

                if ( CHECK_LB_UP_LIKE ( dwMsg ) )
                {
                    if ( m_pInterface->IsStudentRecordOpenable() )
                    {
                        if ( m_pInterface->UiIsVisibleGroup ( STUDENTRECORD_WINDOW ) )
                        {
                            m_pInterface->UiHideGroup ( STUDENTRECORD_WINDOW );
                        }
                        else
                        {
                            m_pInterface->UiShowGroupFocus ( STUDENTRECORD_WINDOW );
                        }
                    }
                }
            }
        }
    }
}

void CStudentRecordNotifyButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


void CStudentRecordNotifyButton::Update ()
{
    m_vecInfo.clear();
    m_bAlarm = false;

    // 일반 툴팁
    m_vecInfo.push_back( sc::string::format(ID2GAMEWORD("ATTENDANCE_TOOLTIP_TEXT", 0), m_pInterface->GetdwKeyToString(DIK_Y).GetString()) );

    // 보상받을 수 있는 개수
    {
        UINT nCount = m_pGaeaClient->AttendanceGetItemTakeAbleCount();

        if ( nCount )
        {
            m_vecInfo.push_back( sc::string::format( ID2GAMEINTEXT("STUDENTRECORD_NOTIFY_ATTENDANCE_TAKEITEMABLE"), nCount) );
            m_bAlarm = true;
        }
    }

    if ( m_bAlarm )
    {
        m_pAlarm->SetVisibleSingle( TRUE );
    }
    else
    {
        m_pAlarm->SetVisibleSingle( FALSE );
    }
}
