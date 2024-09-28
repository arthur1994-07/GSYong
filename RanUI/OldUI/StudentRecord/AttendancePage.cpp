
//bjju.StudentRecord

#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"

#include "./AttendancePage.h"
#include "./AttendanceContents.h"
#include "./AttendanceDayList.h"
#include "./AttendanceCalendar.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CAttendancePage::CAttendancePage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIPage(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pContents(NULL)
    , m_pDayList(NULL)
    , m_pCalendar(NULL)
{
}

CAttendancePage::~CAttendancePage ()
{
}

void CAttendancePage::CreateSubControl ()
{
    m_pContents = new CAttendanceContents( m_pGaeaClient, m_pInterface, m_pEngineDevice );
    m_pContents->CreateSub( this, "ATTENDANCE_PAGE_CONTENTS", UI_FLAG_DEFAULT, ATTENDANCE_PAGE_CONTENTS);
    m_pContents->CreateSubControl();
    RegisterControl(m_pContents);

    m_pDayList = new CAttendanceDayList( m_pInterface, m_pEngineDevice );
    m_pDayList->CreateSub( this, "ATTENDANCE_PAGE_DAYLIST", UI_FLAG_DEFAULT, ATTENDANCE_PAGE_DAYLIST);
    m_pDayList->CreateSubControl();
    RegisterControl(m_pDayList);

    m_pCalendar = new CAttendanceCalendar(m_pEngineDevice);
    m_pCalendar->CreateSub( this, "ATTENDANCE_PAGE_CALENDAR", UI_FLAG_DEFAULT, ATTENDANCE_PAGE_CALENDAR);
    m_pCalendar->CreateSubControl();
    RegisterControl(m_pCalendar);
}

void CAttendancePage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CAttendancePage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIPage::TranslateUIMessage ( ControlID, dwMsg );
}

void CAttendancePage::BeginSubControl()
{
    UpdateContents ();
    UpdateDayList  ();
    UpdateCalendar ();
}

void CAttendancePage::UpdateContents ()
{
    attendance::GLAttendanceClient* pAttendance = m_pGaeaClient->GetAttendance ();
    m_pContents->Update ( pAttendance );
}

void CAttendancePage::UpdateDayList ()
{
   UINT nDays = m_pGaeaClient->AttendanceGetProgressDays ();
    m_pDayList->SetProgress( nDays );
}

void CAttendancePage::UpdateCalendar ()
{
    time_t    ltime;
    struct tm today;

    time(&ltime);
    localtime_s( &today, &ltime);

    m_pCalendar->SetDays ( (UINT)today.tm_mday );
}

void CAttendancePage::UnlockUI()
{
	if( m_pContents )
	{
		m_pContents->UnlockUI();
	}
}
