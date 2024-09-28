#include "StdAfx.h"
#include "../../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../Util/RnButton.h"
#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "RnattendanceCalender.h"
#include "RnattendanceDayBox.h"
#include "RnattendanceDateBox.h"

RnattendanceCalender::RnattendanceCalender(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_nYunbonus(0)
, m_nDayCount(0)
, m_pMonthImage(NULL)
{
	m_nMonth_days[0] = 31; // 1월
	m_nMonth_days[1] = 28; // 2월
	m_nMonth_days[2] = 31; // 3월
	m_nMonth_days[3] = 30; // 4월
	m_nMonth_days[4] = 31; // 5월
	m_nMonth_days[5] = 30; // 6월
	m_nMonth_days[6] = 31; // 7월
	m_nMonth_days[7] = 31; // 8월
	m_nMonth_days[8] = 30; // 9월
	m_nMonth_days[9] = 31; // 10월
	m_nMonth_days[10] = 30;// 11월
	m_nMonth_days[11] = 31;// 12월


	CTime cTime = CTime::GetCurrentTime();
	m_nYear = cTime.GetYear();
	m_nMonth = cTime.GetMonth();
}

RnattendanceCalender::~RnattendanceCalender()
{

}

void RnattendanceCalender::CreateSubControl()
{
	//테스트용 코드
	SetCalenderValue();

	m_pDayBox.clear();
	m_pDateBox.clear();

	CUIGroup* pCalenderBG = new CUIGroup(m_pEngineDevice);
	pCalenderBG->CreateSub(this,"RNATTENDANCEPAGE_BG",UI_FLAG_DEFAULT);
	RegisterControl( pCalenderBG );

	// 년도
	{
		CSwapImage* pImage;

		pImage = new CSwapImage(m_pEngineDevice);
		pImage->CreateSub( this, "RNATTENDANCEPAGE_TOTAL_DAY_1" );
		RegisterControl( pImage );
		m_pTotalDayImage[1] = pImage;

		pImage = new CSwapImage(m_pEngineDevice);
		pImage->CreateSub( this, "RNATTENDANCEPAGE_TOTAL_DAY_10" );
		RegisterControl( pImage );
		m_pTotalDayImage[0] = pImage;
	}

	// 년도
	{
		CSwapImage* pImage;

		pImage = new CSwapImage(m_pEngineDevice);
		pImage->CreateSub( this, "RNATTENDANCEPAGE_YEAR_1" );
		RegisterControl( pImage );
		m_pYearImage[3] = pImage;

		pImage = new CSwapImage(m_pEngineDevice);
		pImage->CreateSub( this, "RNATTENDANCEPAGE_YEAR_10" );
		RegisterControl( pImage );
		m_pYearImage[2] = pImage;

		pImage = new CSwapImage(m_pEngineDevice);
		pImage->CreateSub( this, "RNATTENDANCEPAGE_YEAR_100" );
		RegisterControl( pImage );
		m_pYearImage[1] = pImage;

		pImage = new CSwapImage(m_pEngineDevice);
		pImage->CreateSub( this, "RNATTENDANCEPAGE_YEAR_1000" );
		RegisterControl( pImage );
		m_pYearImage[0] = pImage;
	}

	//달
	{
		CSwapImage* pMonthImage = new CSwapImage( m_pEngineDevice );
		pMonthImage->CreateSub( this, "RNATTENDANCEPAGE_MONTH");
		RegisterControl( pMonthImage );
		m_pMonthImage = pMonthImage;
	}

	//요일 표시 SUN,MON,.....SAT
	for ( int i = 0; i < ATTENDANCE_WEEK_MAX; i++ )
	{
		RnattendanceDayBox* pDayBox = new RnattendanceDayBox( m_pInterface, m_pEngineDevice );
		pDayBox->CreateSub( this, "RNATTENDANCE_DAY_BOX", UI_FLAG_DEFAULT, ATTENDANCE_DAY_BOX_START+i  );
		pDayBox->CreateSubControl();
		RegisterControl( pDayBox );
		m_pDayBox.push_back(pDayBox);

		if (i > 0)
		{
			UIRECT rcOldPos = m_pDayBox[i - 1]->GetLocalPos();
			m_pDayBox[i]->SetLocalPos( D3DXVECTOR2( rcOldPos.right + 2, rcOldPos.top));
		}
	}


	//날짜 표시 1,2,3,4,5,6,
	for ( int i = 0; i < ATTENDDANCE_MONTH_MAX; i++ )
	{
		RnattendanceDateBox* pDateBox = new RnattendanceDateBox( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pDateBox->CreateSub( this, "RNATTENDANCE_DAYTE_BOX", UI_FLAG_DEFAULT, ATTENDANCE_DATE_BOX_START+i  );
		pDateBox->CreateSubControl();
		RegisterControl( pDateBox );
		m_pDateBox.push_back(pDateBox);

		if (i == 0)
		{
			UIRECT rcOldPos = m_pDateBox[0]->GetLocalPos();
			m_pDateBox[0]->SetLocalPos( D3DXVECTOR2( rcOldPos.left, rcOldPos.top ));
		}

		if ( i > 0)
		{
			if ( i % 7 == 0 )
			{
				UIRECT rcOldPos = m_pDateBox[i]->GetLocalPos();
				m_pDateBox[i]->SetLocalPos( D3DXVECTOR2( rcOldPos.left,  rcOldPos.top + ( rcOldPos.sizeY * i / 7 ) + ( 2 * i / 7 ) ) );
			}
			else
			{
				UIRECT rcOldPos = m_pDateBox[i - 1]->GetLocalPos();
				m_pDateBox[i]->SetLocalPos( D3DXVECTOR2( rcOldPos.right + 2, rcOldPos.top));
			}
		}
	}

	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	//리셋버튼
	{
		RnButton* pLineBoxComboButton = NULL;
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
		arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
		arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
		//////////////////////////////////////////////////////////////////////////

		arg.pFont = pFont8;
		arg.dwColor = NS_UITEXTCOLOR::WHITE;
		arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
		arg.text = "<";

		pLineBoxComboButton = new RnButton(m_pEngineDevice);
		pLineBoxComboButton->CreateSub(this, "RNATTENDANCE_DAY_LEFT_BUTTON", UI_FLAG_DEFAULT, ATTENDANCE_DAY_LEFT_BUTTON );
		pLineBoxComboButton->CreateSubControl(arg);
		RegisterControl(pLineBoxComboButton);
	}

	//검색 버튼
	{
		RnButton* pLineBoxComboButton = NULL;
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
		arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
		arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
		//////////////////////////////////////////////////////////////////////////

		arg.pFont = pFont8;
		arg.dwColor = NS_UITEXTCOLOR::WHITE;
		arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
		arg.text = ">";

		pLineBoxComboButton = new RnButton(m_pEngineDevice);
		pLineBoxComboButton->CreateSub(this, "RNATTENDANCE_DAY_RIGHT_BUTTON", UI_FLAG_DEFAULT, ATTENDANCE_DAY_RIGHT_BUTTON );
		pLineBoxComboButton->CreateSubControl(arg);
		RegisterControl(pLineBoxComboButton);
	}

	//달력에 숫자 요일 세팅
	SetCalender();
}

void RnattendanceCalender::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void RnattendanceCalender::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage(  ControlID,  dwMsg );

	SetMonthImage( m_nMonth );
	SetYearImage( m_nYear );
	SetTotalDay(m_pGaeaClient->GetDayCount());

	switch(ControlID)
	{
	case ATTENDANCE_DAY_LEFT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				m_nMonth--;
				if ( m_nMonth < 1)
				{
					m_nYear--;

					if (m_nYear < 0)
						m_nYear = 0;

					m_nMonth = 12;
				}

				SetCalenderValue();
				SetCalender();
			}

		}
		break;
	case ATTENDANCE_DAY_RIGHT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				m_nMonth++;
				if ( m_nMonth > 12)
				{
					m_nYear++;

					if (m_nYear > 9999)
						m_nYear = 9999;

					m_nMonth = 1;
				}

				SetCalenderValue();
				SetCalender();
			}

		}
		break;
	}

		//if (CHECK_MOUSEIN_LBUPLIKE(dwMsg))
		//{
		//	m_nMonth++;
		//	if ( m_nMonth > 12)
		//		m_nMonth = 1;

		//	SetCalenderValue();
		//	SetCalender();
		//}

}

int RnattendanceCalender::CheckYUN( int nYear )
{
	if((nYear % 4 == 0 && nYear % 100 != 0) || nYear % 400 == 0)
		return 29;
	else
		return 28;
}

EM_DAY_OF_WEEK RnattendanceCalender::GetfirstDayofWeek()
{
	int nWeek = ( m_nDayCount % 7 )+ 1;
	
	if (nWeek == 7)
		nWeek = 0;

	return static_cast<EM_DAY_OF_WEEK>(nWeek);
}

EM_DAY_OF_WEEK RnattendanceCalender::GetLastDayofWeek()
{
	int nWeek = ( (m_nDayCount % 7) + m_nMonth_days[ m_nMonth - 1 ] - 1) % 7 + 1;

	if (nWeek == 7)
		nWeek = 0;

	return static_cast<EM_DAY_OF_WEEK>(nWeek);
}

void RnattendanceCalender::SetCalenderValue()
{
	if ( m_nYear < 1 )
		m_nYear = 1;

	// 1년~입력한 년도까지의 윤년횟수(윤년공식)
	m_nYunbonus = ( ( m_nYear-1 ) >> 2 ) - ( m_nYear - 1 ) / 100 + ( m_nYear - 1 ) / 400;

	m_nMonth_days[1] = CheckYUN(m_nYear);

	m_nDayCount = 0;

	for (int i = 0; i < m_nMonth - 1 ; i++)
	{
		m_nDayCount += m_nMonth_days[i];
	}

	m_nDayCount += ( m_nYear - 1 ) * 365 + m_nYunbonus;
}

int RnattendanceCalender::GetDayCount()
{
	return m_nMonth_days[ m_nMonth - 1 ];
}

int RnattendanceCalender::GetDayCountBeforeMonth()
{
	int nMonth = m_nMonth - 2;

	if ( nMonth == -1 )
	{
		nMonth = 11;
	}

	return m_nMonth_days[ nMonth ];
}

EM_DAY_OF_WEEK RnattendanceCalender::GetLastDayofWeekBeforeMont()
{
	int nWeek = ( (m_nDayCount % 7)  - 1) % 7 + 1;

	if (nWeek == 7)
		nWeek = 0;

	return static_cast<EM_DAY_OF_WEEK>(nWeek);
}

void RnattendanceCalender::SetMonthImage( int nMonth )
{
	m_pMonthImage->SetImage(sc::string::format("RNATTENDANCEPAGE_MONTH_%d", nMonth).c_str());
}

void RnattendanceCalender::SetYearImage( int nYear, bool bBlockAndWhite )
{

	std::string strNumberColor;
	if (bBlockAndWhite)
	{
		strNumberColor= "RNATTENDANCEPAGE_DAY_BLACK_NUMBER_%d";
	}
	else
	{
		strNumberColor = "RNATTENDANCEPAGE_DAY_WHITE_NUMBER_%d";
	}

	{
		int nNumber = nYear/1000; // 1000의 자리
			m_pYearImage[0]->SetImage( sc::string::format(strNumberColor.c_str(), nNumber).c_str() );

			nNumber = (nYear%1000)/100; // 100의 자리
			m_pYearImage[1]->SetImage( sc::string::format(strNumberColor.c_str(), nNumber).c_str() );

			nNumber = (nYear%100)/10; // 10의 자리
			m_pYearImage[2]->SetImage( sc::string::format(strNumberColor.c_str(), nNumber).c_str() );

			nNumber = nYear%10; // 1의자리
			m_pYearImage[3]->SetImage( sc::string::format(strNumberColor.c_str(), nNumber).c_str() );

	}
}

void RnattendanceCalender::SetTotalDay( int nDay, bool bBlockAndWhite )
{

	std::string strNumberColor;
	if (bBlockAndWhite)
	{
		strNumberColor= "RNATTENDANCEPAGE_DAY_BLACK_NUMBER_%d";
	}
	else
	{
		strNumberColor = "RNATTENDANCEPAGE_DAY_WHITE_NUMBER_%d";
	}


	{


	}

	int nNumber = nDay/10; //10의자리

	if (nNumber > 0)
	{
		m_pTotalDayImage[0]->SetImage( sc::string::format(strNumberColor.c_str(), nNumber).c_str() );

		nNumber = nDay%10; // 1의자리
		m_pTotalDayImage[1]->SetImage( sc::string::format(strNumberColor.c_str(), nNumber).c_str() );
	}
	else
	{
		nNumber = nDay%10; // 1의자리
		m_pTotalDayImage[1]->SetImage( sc::string::format(strNumberColor.c_str(), nNumber).c_str() );
	}
}

void RnattendanceCalender::SetCalender()
{
	CTime cTime = CTime::GetCurrentTime();
	int nToday = cTime.GetDay();
	int nThisMonth = cTime.GetMonth();
	int nThisYear = cTime.GetYear();
	int nfirstDayofWeek = static_cast<int>( GetfirstDayofWeek() );
	int nDayCount = static_cast<int>( GetDayCount() );

	bool bIsAttendance = ( nThisMonth == m_nMonth && nThisYear == m_nYear ? true:false );

	if ( nfirstDayofWeek == 0 )
	{
		nfirstDayofWeek = 7;
	}

	int nDayCountBeforeMonth = static_cast<int>( GetDayCountBeforeMonth() );
	int nBeforeMonthDay = nDayCountBeforeMonth - nfirstDayofWeek;

	for (int i = 0; i < static_cast<int>(m_pDateBox.size()); i++ )
	{
		m_pDateBox[i]->m_sDayData.m_emBoxState = 0;
		if ( i < nfirstDayofWeek )
		{
			m_pDateBox[i]->SetDays( nBeforeMonthDay + i + 1, false); //달력은 1일 부터 시작하기때문에 1더함
			m_pDateBox[i]->m_sDayData.m_emBoxState &= ~EM_ACTIVE_DAY;
			m_pDateBox[i]->m_sDayData.m_nYear = m_nYear;
			m_pDateBox[i]->m_sDayData.m_nMonth = m_nMonth - 1;
			m_pDateBox[i]->m_sDayData.m_nDays;
			m_pDateBox[i]->m_sDayData.m_nDayofWeek = GetDayOfTheWeek(m_nYear,m_nMonth - 1,nBeforeMonthDay + i + 1);
		}
		else
		{
			if ( ( i - nfirstDayofWeek ) < nDayCount )
			{
				m_pDateBox[i]->SetDays( i - nfirstDayofWeek + 1, true, bIsAttendance );
				m_pDateBox[i]->m_sDayData.m_nDayofWeek = GetDayOfTheWeek(m_nYear,m_nMonth,i - nfirstDayofWeek + 1);
				m_pDateBox[i]->m_sDayData.m_nYear = m_nYear;
				m_pDateBox[i]->m_sDayData.m_nMonth = m_nMonth;
				m_pDateBox[i]->m_sDayData.m_nDays;

				m_pDateBox[i]->m_sDayData.m_emBoxState |= EM_ACTIVE_DAY;
				if ( (i - nfirstDayofWeek + 1) == nToday &&
					nThisMonth == m_nMonth &&
					nThisYear == m_nYear )
				{
					m_pDateBox[i]->m_sDayData.m_emBoxState |= EM_TO_DAY;
					//m_pDateBox[i]->SetEventText("오늘");
				}
				if ( (i - nfirstDayofWeek + 1) < nToday &&
					nThisMonth == m_nMonth &&
					nThisYear == m_nYear )
				{
					m_pDateBox[i]->m_sDayData.m_emBoxState &= ~EM_ACTIVE_DAY;
				}
			}
			else
			{
				m_pDateBox[i]->SetDays( i - (nfirstDayofWeek + nDayCount - 1), false );
				m_pDateBox[i]->m_sDayData.m_nYear = m_nYear;
				m_pDateBox[i]->m_sDayData.m_nMonth = m_nMonth + 1;
				m_pDateBox[i]->m_sDayData.m_nDays;
				m_pDateBox[i]->m_sDayData.m_nDayofWeek = GetDayOfTheWeek(m_nYear,m_nMonth + 1, i - (nfirstDayofWeek + nDayCount - 1));
				m_pDateBox[i]->m_sDayData.m_emBoxState &= ~EM_ACTIVE_DAY;
			}
		}

	}
}

void RnattendanceCalender::ResetCalender()
{
	CTime cTime = CTime::GetCurrentTime();
	m_nYear = cTime.GetYear();
	m_nMonth = cTime.GetMonth();
	SetCalenderValue();
	SetCalender();
}

int RnattendanceCalender::GetDayOfTheWeek( int year, int month, int date )
{
	if (month == 0)
	{
		year = year - 1;
		month = 12;
	}

	if (month == 13)
	{ 
		month = 1;
		year = year + 1;
	}

	if ( month <= 2 )
	{
		--year;
		month += 12;
	}

	//return (year+year/4-year/100+year/400+(13+month+8)/5+date)%7;
	return ( (21*(year/100)/4) + (5*(year%100)/4) + (26*(month+1)/10) + date - 1 ) % 7;
}


