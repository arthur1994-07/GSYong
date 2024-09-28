#include "StdAfx.h"

#include "../../../InnerInterface.h"
#include "../../../../enginelib/GUInterface/GameTextControl.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../Interface/BasicTextBoxExLink.h"
#include "../../../../RanLogicClient/RnAttendance/RnAttendanceClient.h"

#include "RnattendanceDateBox.h"

const char* RNATTENDANCE_DATE_BOX_DATE = "%1%년 %1%월 %1%일";
const char* RNATTENDANCE_DATE_BOX_ANYDAY = "출석 %1%일차";

RnattendanceDateBox::RnattendanceDateBox(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pDateBackImage(NULL)
, m_pAttendanceImage(NULL)
, m_pDateMouseEventImage(NULL)
, m_pDateBoxMouseOverLine(NULL)
{
	for ( int i = 0; i < DAYNUM; i++ )
	{
		m_pDateImage[i] = NULL;
	}
	RNATTENDANCE_DATE_BOX_DATE = ID2GAMEINTEXT("RNATTENDANCE_DATE_BOX_DATE");
	RNATTENDANCE_DATE_BOX_ANYDAY = ID2GAMEINTEXT("RNATTENDANCE_DATE_BOX_ANYDAY");
}
RnattendanceDateBox::~RnattendanceDateBox()
{

}

void RnattendanceDateBox::CreateSubControl()
{

	//날짜 배경 이미지
	CSwapImage* pDateBackImage = new CSwapImage( m_pEngineDevice );
	pDateBackImage->CreateSub( this, "RNATTENDANCE_CALENDER_BOX", UI_FLAG_RIGHT | UI_FLAG_TOP, ATTENDANCE_DAY_BACK_IMAGE );
	//pDateBackImage->SetUseRender(TRUE);
	RegisterControl( pDateBackImage );
	m_pDateBackImage = pDateBackImage;

	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	//이름창 텍스트 박스
	CBasicTextBoxExLink* pEventTextBox = new CBasicTextBoxExLink(m_pEngineDevice);
	pEventTextBox->CreateSub ( this, "RNATTENDANCE_EVENT_TEXTBOX", UI_FLAG_YSIZE, ATTENDANCE_DAY_EVENTTEXTBOX );
	pEventTextBox->SetFont ( pFont );		
	pEventTextBox->SetLineInterval ( 2.0f );
	pEventTextBox->SetSensitive ( true );
	RegisterControl ( pEventTextBox );
	m_pEventTextBox = pEventTextBox;

	//출석,결석 이미지
	CSwapImage* pAttendanceImage = new CSwapImage( m_pEngineDevice );
	pAttendanceImage->CreateSub( this, "RNATTENDANCE_CALENDER_BOX", UI_FLAG_RIGHT | UI_FLAG_TOP );
	pAttendanceImage->SetImage("RNATTENDANCEPAGE_DAY_OK");
	//pAttendanceImage->SetUseRender(TRUE);
	//pAttendanceImage->SetDiffuseAlpha(64);
	pAttendanceImage->SetVisibleSingle(FALSE);
	RegisterControl( pAttendanceImage );
	m_pAttendanceImage = pAttendanceImage;

	CSwapImage* pDateMouseEventImage = new CSwapImage( m_pEngineDevice );
	pDateMouseEventImage->CreateSub( this, "RNATTENDANCE_CALENDER_BOX", UI_FLAG_RIGHT | UI_FLAG_TOP );
	RegisterControl( pDateMouseEventImage );
	m_pDateMouseEventImage = pDateMouseEventImage;



	// 날짜
	{
		CSwapImage* pImage;

		pImage = new CSwapImage(m_pEngineDevice);
		pImage->CreateSub( this, "RNATTENDANCEPAGE_DAY_10" );
		RegisterControl( pImage );
		m_pDateImage[0] = pImage;

		pImage = new CSwapImage(m_pEngineDevice);
		pImage->CreateSub( this, "RNATTENDANCEPAGE_DAY_1" );
		RegisterControl( pImage );
		m_pDateImage[1] = pImage;
	}

	// 뒷 배경
	{
		CBasicLineBoxSmart* pDateBoxMouseOverLine  = new CBasicLineBoxSmart(m_pEngineDevice);
		pDateBoxMouseOverLine ->CreateSub( this, "RNATTENDANCE_CALENDER_BOX", UI_FLAG_DEFAULT, ATTENDANCE_DAY_MOUSE_UP_LINE );
		pDateBoxMouseOverLine ->CreateSubControl( "LINEBOX_Y" );
		pDateBoxMouseOverLine->SetVisibleSingle(FALSE);
		RegisterControl( pDateBoxMouseOverLine  );
		m_pDateBoxMouseOverLine = pDateBoxMouseOverLine ;
	}

}

void RnattendanceDateBox::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void RnattendanceDateBox::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch (ControlID)
	{
	case ATTENDANCE_DAY_BACK_IMAGE:
		{
			if (CHECK_MOUSEIN_LBUPLIKE(dwMsg))
			{
				SetTodayInfo();
				
				m_pGaeaClient->SetSelectDay( m_sDayData.m_nDayofWeek, 
											 m_sDayData.m_nDays, 
											 m_sDayData.m_nMonth, 
											 m_sDayData.m_nYear,
											 m_sDayData.m_nAnyDay);
				m_pInterface->REFRESH_RnattendancePage();
				m_sDayData.m_emBoxState |= EM_SELECT_DAY;

			}

			if (CHECK_MOUSE_IN(dwMsg))
			{
				m_pDateMouseEventImage->SetImage("RNATTENDANCEPAGE_DAY_OVER");
				m_pDateMouseEventImage->SetVisibleSingle(TRUE);
			}
			else
			{
				m_pDateMouseEventImage->SetVisibleSingle(FALSE);

				if (CHECK_LB_UP_LIKE(dwMsg))
				{
					if (m_sDayData.m_emBoxState & EM_SELECT_DAY)
					{
						m_sDayData.m_emBoxState &= ~EM_SELECT_DAY;
					}
				}
			}

			//if (CHECK_MOUSEIN_RBUPLIKE(dwMsg))
			//{
			//	if (m_sDayData.m_bIsAttend)
			//	{
			//		m_sDayData.m_bIsAttend = false;
			//	}
			//	else
			//	{
			//		m_sDayData.m_bIsAttend = true;
			//	}
			//}
		}
		break;
	}
	SetDateBackColor();
}

void RnattendanceDateBox::SetDateBackColor()
{
	if (m_sDayData.m_emBoxState & EM_ACTIVE_DAY)
	{
		m_pDateBackImage->SetImage("RNATTENDANCEPAGE_DAY_ACTIVE_DAY");
	}
	else
	{
		m_pDateBackImage->SetImage("RNATTENDANCEPAGE_DAY_INACTIVE_DAY");
	}

	if (m_sDayData.m_emBoxState & EM_TO_DAY)
	{
		m_pDateBackImage->SetImage("RNATTENDANCEPAGE_DAY_TODAY");
	}

	if (m_sDayData.m_emBoxState & EM_SELECT_DAY)
	{
		m_pDateBackImage->SetImage("RNATTENDANCEPAGE_DAY_SELECT");
	}

	m_pAttendanceImage->SetVisibleSingle(m_sDayData.m_bIsAttend);

}

void RnattendanceDateBox::SetDays ( const int nDays, bool bIsThisMonth, bool bIsAttendance )
{
	m_sDayData.m_nAnyDay = 0;
	m_sDayData.m_nDays = nDays;
	m_sDayData.m_bIsAttend = false;
	m_pAttendanceImage->SetVisibleSingle(m_sDayData.m_bIsAttend);

	if ( bIsAttendance )
	{
		int nAnyDay = m_pGaeaClient->GetRnAttendanceClient()->IsAttendanceDay(nDays);
		if (nAnyDay)
		{
			m_sDayData.m_nAnyDay = nAnyDay;
			m_sDayData.m_bIsAttend = true;
		}
		m_pAttendanceImage->SetVisibleSingle(m_sDayData.m_bIsAttend);
	}

	CTime TToday = CTime::GetCurrentTime();
	if (!m_sDayData.m_bIsAttend && 
		m_sDayData.m_nDays >= TToday.GetDay() && 
		m_sDayData.m_nMonth == TToday.GetMonth() &&
		m_sDayData.m_nYear == TToday.GetYear())
	{
		int i = 0;
		int nAnyDay = 0;

		for( i = 0; i<31; i++)
		{
			//마지막 출석일의 누적 출석일`
			nAnyDay = m_pGaeaClient->GetRnAttendanceClient()->IsAttendanceDay(31 - i);
			if (nAnyDay)
				break;
		}

		if ( nDays > ( 31 - i ))
		{
			m_sDayData.m_nAnyDay =  nDays - TToday.GetDay() + nAnyDay;

			// 오늘 Today 출석 여부
			if (!m_pGaeaClient->GetRnAttendanceClient()->IsAttendanceDay(TToday.GetDay()))
			{
				m_sDayData.m_nAnyDay += 1;
			}
		}
	}

	
	m_pDateImage[0]->SetImage( "RNATTENDANCEPAGE_DAY_BLACK_NUMBER_EMPTY" );
	m_pDateImage[1]->SetImage( "RNATTENDANCEPAGE_DAY_BLACK_NUMBER_EMPTY" );

	std::string strNumberColor;
	if (bIsThisMonth)
	{
		strNumberColor= "RNATTENDANCEPAGE_DAY_BLACK_NUMBER_%d";
	}
	else
	{
		strNumberColor = "RNATTENDANCEPAGE_DAY_WHITE_NUMBER_%d";
	}
	

	{
		int nNumber = nDays/10; //10의자리

		if (nNumber > 0)
		{
			m_pDateImage[0]->SetImage( sc::string::format(strNumberColor.c_str(), nNumber).c_str() );
			nNumber = nDays%10;// 1의자리
			m_pDateImage[1]->SetImage( sc::string::format(strNumberColor.c_str(), nNumber).c_str() );
		}
		else
		{
			nNumber = nDays%10;
			m_pDateImage[0]->SetImage( sc::string::format(strNumberColor.c_str(), nNumber).c_str() );
		}
	}

}

void RnattendanceDateBox::SetEventText( const CString strEventText, const D3DCOLOR& TextColor /*= NS_UITEXTUTIL::ENABLE*/ )
{
	m_pEventTextBox->AddText( strEventText, TextColor );
}

void RnattendanceDateBox::SetTodayInfo()
{
	m_pInterface->RnAttendanceClerText();
	m_sDayData.m_nDays;
	std::string strDays = "";
	strDays = sc::string::format( RNATTENDANCE_DATE_BOX_DATE, m_sDayData.m_nYear,m_sDayData.m_nMonth,m_sDayData.m_nDays, m_sDayData.m_nDayofWeek );

	m_pInterface->RnAttendanceSetDayInfoText( strDays.c_str(), NS_UITEXTCOLOR::WHITE);

	m_sDayData.m_nAnyDay;
	std::string strAnyDay = "";
	strAnyDay = sc::string::format( RNATTENDANCE_DATE_BOX_ANYDAY, m_sDayData.m_nAnyDay );

	m_pInterface->RnAttendanceSetDayInfoText( strAnyDay.c_str(), NS_UITEXTCOLOR::WHITE);
}

