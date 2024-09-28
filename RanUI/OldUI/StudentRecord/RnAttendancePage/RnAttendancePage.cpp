#include "StdAfx.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/RnAttendance/RnAttendanceClient.h"
#include "RnattendanceCalender.h"
#include "RnattendanceDayInfo.h"
#include "RnAttendancePage.h"

RnattendancePage::RnattendancePage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIPage(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pCalender(NULL)
, m_pDayInfo(NULL)
{

}

RnattendancePage::~RnattendancePage()
{

}

void RnattendancePage::CreateSubControl()
{
	//ดÞทย
	RnattendanceCalender* pCalender = new RnattendanceCalender(m_pGaeaClient,m_pInterface,m_pEngineDevice);
	pCalender->CreateSub(this,"RNATTENDANCE_CALENDER_BG",UI_FLAG_DEFAULT,ATTENDANCE_CALENDER);
	pCalender->CreateSubControl();
	RegisterControl(pCalender);
	m_pCalender = pCalender;

	//Day Info 
	RnattendanceDayInfo* pDayInfo = new RnattendanceDayInfo(m_pGaeaClient,m_pInterface,m_pEngineDevice);
	pDayInfo->CreateSub(this,"INBOX_NAME_TEXT",UI_FLAG_DEFAULT,ATTENDANCE_DAY_INFO);
	pDayInfo->CreateSubControl();
	RegisterControl(pDayInfo);
	m_pDayInfo = pDayInfo;

}

void RnattendancePage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIPage::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void RnattendancePage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIPage::TranslateUIMessage( ControlID, dwMsg );
	int c = 300;
}

void RnattendancePage::ResetCalender()
{
	m_pCalender->ResetCalender();
}

void RnattendancePage::SetDayInfoText( const CString strEventText, const D3DCOLOR& TextColor /*= NS_UITEXTUTIL::ENABLE */ )
{
	m_pDayInfo->SetDayInfoText( strEventText, TextColor );
}

void RnattendancePage::ClearDayInfo()
{
	m_pDayInfo->ClearDayInfo();
}

void RnattendancePage::SetVisibleSingle( BOOL bVisible )
{
	CUIPage::SetVisibleSingle( bVisible );
	if( bVisible )
	{
		//m_pGaeaClient->GetAttendanceQuestData();
	}
}

void RnattendancePage::SetSlotLScroolBarUpDate()
{
	m_pGaeaClient->SetAttendanceQuestData();
	m_pDayInfo->SetSlotLScroolBarUpDate();
	m_pDayInfo->UpdateSlot();

}
