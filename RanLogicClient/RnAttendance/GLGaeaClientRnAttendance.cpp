#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"

#include "RnAttendanceClient.h"
#include "../GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
std::vector<SATTENDANCEQUESTDATA> GLGaeaClient::GetAttendanceQuestData()
{
	return m_pRnAttendanceClient->GetAttendanceQuestData();
}

void GLGaeaClient::GetAttendanceCompleted()
{
	// Note : Rnattendance
	if ( !GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		return;
	}

	m_pRnAttendanceClient->GetAttendanceCompleted();
}

void GLGaeaClient::MsgRnattendanceProcess(NET_MSG_GENERIC* pMsg)
{
	// Note : Rnattendance
	if ( !GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		return;
	}

	m_pRnAttendanceClient->MsgRnattendanceProcess( pMsg );
}

int GLGaeaClient::GetAttendanceCount()
{
	// Note : Rnattendance
	if ( !GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		return 0;
	}
	return m_pRnAttendanceClient->GetAttendanceCount();
}


void GLGaeaClient::SetAttendanceQuestData()
{
	// Note : Rnattendance
	if ( !GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		return;
	}

	m_pRnAttendanceClient->SetAttendanceQuest();
}

void GLGaeaClient::ClearAttendanceQuestIdServerToClient()
{
	// Note : Rnattendance
	if ( !GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		return;
	}

	m_pRnAttendanceClient->ClearAttendanceQuestIdServerToClient();
}

int GLGaeaClient::GetDayCount()
{
	// Note : Rnattendance
	if ( !GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		return 0;
	}

	return m_pRnAttendanceClient->GetDayCount();
}


void GLGaeaClient::SetSelectDay( int nDayofWeek, int nDays, int nMonth, int nYear, int nAnyDay )
{
	// Note : Rnattendance
	if ( !GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		return;
	}
	m_pRnAttendanceClient->SetSelectDay( nDayofWeek, nDays, nMonth, nYear, nAnyDay );
}

SDAYDATA GLGaeaClient::GetSelectDay()
{
	// Note : Rnattendance
	if ( !GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		SDAYDATA sSdaydata;
		return sSdaydata;
	}
	return m_pRnAttendanceClient->GetSelectDay();
}