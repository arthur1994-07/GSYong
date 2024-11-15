#include "StdAfx.h"
#include "AttendanceBookWindow.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "Calendar.h"
#include "DayBox.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CAttendanceBookWindow::CAttendanceBookWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_nCurDay(0)
    , m_pMonthImage(NULL)
    , m_pMonthImageTen(NULL)
    , m_pMonthImageEng(NULL)
    , m_pComboImage(NULL)
    , m_pComboImageTen(NULL)
    , m_bSixLine(false)
    , m_nFirstDay(0)
    , m_bAttend(false)
    , m_pd3dDevice(NULL)
	, m_bLockUI(true)
{
	memset(m_pDayBox, NULL, sizeof(CDayBox*)* DAY_BOX_COUNT);
}

CAttendanceBookWindow::~CAttendanceBookWindow()
{
}


CUIControl*	CAttendanceBookWindow::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

CBasicTextBox* CAttendanceBookWindow::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}


void CAttendanceBookWindow::CreateSubControl ()
{

	CD3DFontPar * pFont18 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 24 );
	CD3DFontPar * pFont12 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 18 );

	//	뒷배경
	CreateControl( "ATTENDANCE_BOOK_BACK" );

	// 연속 일자
	m_pComboImage = CreateControl( "ATTENDANCE_BOOK_COMBO" );
	m_pComboImageTen = CreateControl( "ATTENDANCE_BOOK_COMBO_TEN" );

	// 년월일
	m_pMonthImage = CreateControl( "ATTENDANCE_BOOK_MONTH" );
	m_pMonthImageTen = CreateControl( "ATTENDANCE_BOOK_MONTH_TEN" );
	m_pMonthImageEng = CreateControl( "ATTENDANCE_BOOK_MONTH_ENG" );

	//	날짜칸
	CString strTemp;

	for ( int i = 0; i < DAY_BOX_COUNT; ++i )
	{
		strTemp.Format( "ATTENDANCE_BOOK_DAYBOX_%d", i );
		m_pDayBox[i] = new CDayBox(m_pEngineDevice);
		m_pDayBox[i]->CreateSub ( this, strTemp.GetString(), UI_FLAG_DEFAULT, DAY_BOX_IMAGE+i );
		m_pDayBox[i]->CreateSubControl();
		RegisterControl ( m_pDayBox[i] );
	}	
}


void CAttendanceBookWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	if ( DAY_BOX_IMAGE <= ControlID && (DAY_BOX_IMAGE+DAY_BOX_COUNT) > ControlID )
	{
		if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			int nIndex = ControlID - DAY_BOX_IMAGE;
			if ( !m_pDayBox[nIndex]->GetAttendance() && !m_pDayBox[nIndex]->GetAttendReward() )
			{
				int nDay = m_pDayBox[nIndex]->GetDay();
				if ( nDay == m_nCurDay )
				{
					// 출석부 마크
					if ( S_OK == m_pGaeaClient->GetCharacter()->ReqAttendance() )
						m_bAttend = true;
				}
			}
		}
		
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CAttendanceBookWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if ( bVisible )
	{
		m_pGaeaClient->GetCharacter()->ReqAttendList();		
	}

}

void CAttendanceBookWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	CTime cTime( m_pGaeaClient->GetCurrentTime() );

	//	서버 타임으로 변환( TimeZone 계산 )
	m_pGaeaClient->GetConvertServerTime( cTime );

	// 날짜가 바꼈음
	if ( m_nCurDay != cTime.GetDay() )
	{
		SetDayBox();
		m_pGaeaClient->GetCharacter()->ReqAttendList( true );
	}

}

void CAttendanceBookWindow::ResetDayBox()
{
	for ( int i = 0; i < DAY_BOX_COUNT; ++i )
	{
		m_pDayBox[i]->SetVisibleSingle( FALSE );
		m_pDayBox[i]->SetAttendance(false);
		m_pDayBox[i]->SetAttendReward(false);
	}

	ReSetSixLinePos();
}

void CAttendanceBookWindow::SetDayBox()
{
	ResetDayBox();

	CTime	cTime( m_pGaeaClient->GetCurrentTime() );

	//	서버 타임으로 변환( TimeZone 계산 )
	m_pGaeaClient->GetConvertServerTime( cTime );
	
	CCalendar cCal( cTime.GetYear(), cTime.GetMonth(), cTime.GetDay() );

	m_nCurDay = cTime.GetDay();
	m_bSixLine = false;

	int nMonth = cTime.GetMonth();

	//	월 텍스쳐 위치 조정
	SetMonthTexPos( nMonth );
	
	int nDay = 1;
	m_nFirstDay = cCal.GetDayOfWeek_Month();
	int nDayOfWeek = m_nFirstDay;
	int nLastDay = cCal.GetLastDayOfMonth();	

	//	일자 셋팅
	for ( int i = 0; i < nLastDay+m_nFirstDay; ++i )
	{
		if ( i >= m_nFirstDay )
		{
			if ( i >= DAY_BOX_NUM )
			{
				// 6줄일경우 처리
				m_bSixLine = true;
				SetSixLinePos(i-7);
				m_pDayBox[i-7]->SetMarkPos();
			}

			m_pDayBox[i]->SetDay( nDay, nDayOfWeek );
			nDay++;
			nDayOfWeek++;
			nDayOfWeek = nDayOfWeek % 7;
			m_pDayBox[i]->SetMarkPos();
			m_pDayBox[i]->SetVisibleSingle( TRUE );
		}		
	}	
}


void CAttendanceBookWindow::RefreshAttend()
{
	m_bAttend = false;
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter ) return;

	VECATTEND& vecAttend = pCharacter->m_vecAttend;

	CTime cCurTime( m_pGaeaClient->GetCurrentTime() );
	//	서버 타임으로 변환( TimeZone 계산 )
	m_pGaeaClient->GetConvertServerTime( cCurTime );

	CTime cTime;

	for ( int i = 0; i < static_cast<int>(pCharacter->m_vecAttend.size()); ++i )
	{
		cTime = vecAttend[i].tAttendTime;

		//	서버 타임으로 변환( TimeZone 계산 )
		m_pGaeaClient->GetConvertServerTime( cTime );
		
		if ( cCurTime.GetYear() != cTime.GetYear() || cCurTime.GetMonth() != cTime.GetMonth() )
			continue;
		int nIndex = m_nFirstDay + cTime.GetDay() - 1;

		m_pDayBox[nIndex]->SetAttendance( true );
		
		// 상품 받은거 따로 체크할때...
//		if ( vecAttend[i].bAttendReward ) m_pDayBox[nIndex]->SetAttendReward( true );
//		else m_pDayBox[nIndex]->SetAttendance( true );
	}

	// Set 연속 출석일수 
	SetComboTexPos ( pCharacter->m_dwComboAttend );
}

void CAttendanceBookWindow::SetMonthTexPos( int nMonth )
{
	UIRECT rcTexPos;
	
	rcTexPos = m_pMonthImage->GetTexturePos();
	rcTexPos.left =  rcTexPos.sizeX * ( nMonth % 10 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pMonthImage->SetTexturePos( rcTexPos );

	rcTexPos = m_pMonthImageTen->GetTexturePos();
	rcTexPos.left = rcTexPos.sizeX * ( nMonth / 10 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pMonthImageTen->SetTexturePos( rcTexPos );

	rcTexPos = m_pMonthImageEng->GetTexturePos();
	rcTexPos.left = rcTexPos.sizeX * (nMonth-1);
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pMonthImageEng->SetTexturePos( rcTexPos );

	if ( nMonth < 10 ) m_pMonthImageTen->SetVisibleSingle( FALSE );
	else m_pMonthImageTen->SetVisibleSingle( TRUE );
}

void CAttendanceBookWindow::SetComboTexPos( int nComboDay )
{
	UIRECT rcTexPos;
	
	rcTexPos = m_pComboImage->GetTexturePos();
	rcTexPos.left =  rcTexPos.sizeX * ( nComboDay % 10 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pComboImage->SetTexturePos( rcTexPos );

	rcTexPos = m_pComboImageTen->GetTexturePos();
	rcTexPos.left = rcTexPos.sizeX * ( nComboDay / 10 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pComboImageTen->SetTexturePos( rcTexPos );

	if ( nComboDay < 10 ) m_pComboImageTen->SetVisibleSingle( FALSE );
	else m_pComboImageTen->SetVisibleSingle( TRUE );
}

void CAttendanceBookWindow::SetSixLinePos( int nIndex )
{
	
	const UIRECT& rcLocalPosOld = m_pDayBox[nIndex]->GetLocalPos();
	const UIRECT& rcGlobalPosOld = m_pDayBox[nIndex]->GetGlobalPos();
	const UIRECT& rcPos = m_pDayBox[nIndex+7]->GetGlobalPos();

	UIRECT rcGlobalPosNew = UIRECT( rcGlobalPosOld.left, rcGlobalPosOld.top,
									rcGlobalPosOld.sizeX, rcPos.sizeY );

	UIRECT rcLocalPosNew = UIRECT( rcLocalPosOld.left, rcLocalPosOld.top,
									rcLocalPosOld.sizeX, rcPos.sizeY );

	m_pDayBox[nIndex]->AlignSubControl ( rcGlobalPosOld, rcGlobalPosNew );

	m_pDayBox[nIndex]->SetLocalPos ( rcLocalPosNew );
	m_pDayBox[nIndex]->SetGlobalPos ( rcGlobalPosNew );

	m_pDayBox[nIndex]->SetTextureName( m_pDayBox[nIndex+7]->GetTextureName() );
	m_pDayBox[nIndex]->LoadTexture( m_pd3dDevice );
	m_pDayBox[nIndex]->SetTexturePos( m_pDayBox[nIndex+7]->GetTexturePos() );
	m_pDayBox[nIndex]->SetUseRender( TRUE );
}


void CAttendanceBookWindow::ReSetSixLinePos()
{
	for ( int i = 28; i < 30; ++i )
	{
		UIRECT rcLocalPosOld, rcGlobalPosOld, rcOtherPos;
		rcLocalPosOld = m_pDayBox[i]->GetLocalPos();
		rcGlobalPosOld = m_pDayBox[i]->GetGlobalPos();
		rcOtherPos = m_pDayBox[0]->GetGlobalPos();

		UIRECT rcGlobalPosNew = UIRECT( rcGlobalPosOld.left, rcGlobalPosOld.top,
										rcGlobalPosOld.sizeX, rcOtherPos.sizeY);

		m_pDayBox[i]->AlignSubControl ( rcGlobalPosOld, rcGlobalPosNew );

		m_pDayBox[i]->SetLocalPos ( D3DXVECTOR2 ( rcLocalPosOld.left, rcLocalPosOld.top ) );
		m_pDayBox[i]->SetGlobalPos ( rcGlobalPosNew);

		m_pDayBox[i]->SetUseRender( FALSE );
	}
}

HRESULT CAttendanceBookWindow::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );
	m_pd3dDevice = pd3dDevice;

	HRESULT hr = S_OK;
	hr = CUIGroup::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}
