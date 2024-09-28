#include "stdafx.h"
#include "./OldUI/StudentRecord/StudentRecordWindow.h"
#include "./OldUI/StudentRecord/StudentRecordNotifyButton.h"
#include "./InnerInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

const bool CInnerInterface::IsStudentRecordOpenable ()
{
    if ( !m_pStudentRecordWindow || !m_pStudentRecordWindow->IsOpenable() )
    {
        return false;
    }

    return true;
}

void CInnerInterface::UpdateActivityWindow()
{
    if ( m_pStudentRecordWindow && m_pStudentRecordWindow->IsOpenableActivityPage() && UiIsVisibleGroup(STUDENTRECORD_WINDOW) ) 
    {
        m_pStudentRecordWindow->UpdateActivity();
    }
}

void CInnerInterface::UpdateActivityPoint()
{   
    if ( m_pStudentRecordWindow && m_pStudentRecordWindow->IsOpenableActivityPage() && UiIsVisibleGroup(STUDENTRECORD_WINDOW) ) 
    {
        m_pStudentRecordWindow->UpdateActivityPoint();
    }
}

const bool CInnerInterface::IsActivityProgressDisplay ()
{   
    if ( !m_pStudentRecordWindow ) 
    {
        return false;
    }

    return m_pStudentRecordWindow->IsActivityProgressDisplay();
}

void CInnerInterface::UpdateAttendance()
{
    if ( m_pStudentRecordWindow && m_pStudentRecordWindow->IsOpenableAttendancePage() && UiIsVisibleGroup(STUDENTRECORD_WINDOW) ) 
    {
        m_pStudentRecordWindow->UpdateAttendance();
    }
}

void CInnerInterface::UpdateAttendanceTask()
{
    if ( m_pStudentRecordWindow && m_pStudentRecordWindow->IsOpenableAttendancePage() && UiIsVisibleGroup(STUDENTRECORD_WINDOW) ) 
    {
        m_pStudentRecordWindow->UpdateAttendanceTask();
    }
}

void CInnerInterface::UnlockAttendanceUI()
{
	m_pStudentRecordWindow->UnlockAttendanceUI();
}

void CInnerInterface::UpdateStudentRecordNotifyButton()
{
    if ( m_pStudentRecordNotifyButton )
    {
        m_pStudentRecordNotifyButton->Update();
    }
}

void CInnerInterface::OpenAttendanceWindow()
{
    if ( !m_pStudentRecordWindow || !m_pStudentRecordWindow->IsOpenable() )
    {
        return;
    }

    if ( m_pStudentRecordWindow->IsOpenableAttendancePage() )
    {
        m_pStudentRecordWindow->OpenAttendanceWindow();
    }
}


void CInnerInterface::RnAttendanceSetDayInfoText( const CString strEventText, const D3DCOLOR& TextColor /*= NS_UITEXTUTIL::ENABLE */ )
{
	if ( m_pStudentRecordWindow && m_pStudentRecordWindow->IsOpenableAttendancePage() && UiIsVisibleGroup(STUDENTRECORD_WINDOW) ) 
	{
		m_pStudentRecordWindow->RnAttendanceSetDayInfoText( strEventText, TextColor );
	}
}

void CInnerInterface::RnAttendanceClerText()
{
	if ( m_pStudentRecordWindow && m_pStudentRecordWindow->IsOpenableAttendancePage() && UiIsVisibleGroup(STUDENTRECORD_WINDOW) ) 
	{
		m_pStudentRecordWindow->RnAttendanceClerText();
	}
}

void CInnerInterface::REFRESH_RnattendancePage()
{
	m_pStudentRecordWindow->REFRESH_RnattendancePage();
}

