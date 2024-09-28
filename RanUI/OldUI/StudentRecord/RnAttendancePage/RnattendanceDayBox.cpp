#include "StdAfx.h"
#include "RnattendanceDayBox.h"
#include "../../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

RnattendanceDayBox::RnattendanceDayBox( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pInterface(pInterface)
{
}

RnattendanceDayBox::~RnattendanceDayBox()
{

}

void RnattendanceDayBox::CreateSubControl()
{
	m_pDayEventImage.clear();
	//이벤트 이미지
	for (int i= 0; i< 3; i++)
	{
		CSwapImage* pDayEventImage = new CSwapImage( m_pEngineDevice );
		pDayEventImage->CreateSub( this, "RNATTENDANCE_DAY_EVENT_1", UI_FLAG_RIGHT | UI_FLAG_TOP,ATTENDANCE_DAY_EVENT_1 + i );
		RegisterControl( pDayEventImage );
		m_pDayEventImage.push_back(pDayEventImage);
	}
	m_pDayEventImage[0]->SetImage("RNATTENDANCEPAGE_LEAD_CLUB_BATTLE");
}

void RnattendanceDayBox::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void RnattendanceDayBox::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage(  ControlID,  dwMsg );
	switch (ControlID)
	{
	case ATTENDANCE_DAY_EVENT_1:
	case ATTENDANCE_DAY_EVENT_2:
	case ATTENDANCE_DAY_EVENT_3:
		{
			if (CHECK_MOUSEIN_RBUPLIKE(dwMsg))
			{
				m_pDayEventImage[0]->SetVisibleSingle(TRUE);
				int a = rand()%5;
				switch(a)
				{
				case 0:
					m_pDayEventImage[0]->SetImage("RNATTENDANCEPAGE_CLUB_DEATH_MACH");
					break;
				case 5:
					m_pDayEventImage[0]->SetImage("RNATTENDANCEPAGE_ES_A");
					break;
				case 1:
					m_pDayEventImage[0]->SetImage("RNATTENDANCEPAGE_ES_B");
					break;
				case 2:
					m_pDayEventImage[0]->SetImage("RNATTENDANCEPAGE_ES_C");
					break;
				case 3:
					m_pDayEventImage[0]->SetImage("RNATTENDANCEPAGE_ES_S");
					break;
				case 4:
					m_pDayEventImage[0]->SetImage("RNATTENDANCEPAGE_LEAD_CLUB_BATTLE");
					break;
				}
			}
			if (CHECK_MOUSEIN_LBUPLIKE(dwMsg))
			{
				m_pDayEventImage[0]->SetVisibleSingle(FALSE);
			}
		}
		break;
	}
}

void RnattendanceDayBox::SetDayBackImage()
{

}

void RnattendanceDayBox::SetDayImage()
{

}

void RnattendanceDayBox::SetEventImage()
{

}
