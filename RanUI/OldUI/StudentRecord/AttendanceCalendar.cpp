#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../InnerInterface.h"

#include "./AttendanceCalendar.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CAttendanceCalendar::CAttendanceCalendar(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_nDays (0)
{
    memset( m_pDays, 0, sizeof(m_pDays) );
}

CAttendanceCalendar::~CAttendanceCalendar ()
{
    //Blank
}

void CAttendanceCalendar::CreateSubControl ()
{
    // 뒷 배경
    {
        CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pLineBox->CreateSub( this, "ATTENDANCE_PAGE_CALENDAR_LINEBOX" );
        pLineBox->CreateSubControl( "ATTENDANCE_PAGE_CALENDAR_LINEBOX_TEXINFO" );
        RegisterControl( pLineBox );
    }

    // 달력 이미지
    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "ATTENDANCE_PAGE_CONTENTS_PANEL" );
        RegisterControl( pControl );
    }

    // 날짜
    {
        CSwapImage* pImage;

        pImage = new CSwapImage(m_pEngineDevice);
        pImage->CreateSub( this, "ATTENDANCE_PAGE_CONTENTS_DAYS_10" );
        RegisterControl( pImage );
        m_pDays[0] = pImage;

        pImage = new CSwapImage(m_pEngineDevice);
        pImage->CreateSub( this, "ATTENDANCE_PAGE_CONTENTS_DAYS_1" );
        RegisterControl( pImage );
        m_pDays[1] = pImage;
    }
}

void CAttendanceCalendar::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CAttendanceCalendar::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CAttendanceCalendar::SetDays ( const UINT nDays )
{
    m_nDays = nDays;

    CUIControl BaseControl(m_pEngineDevice); 
    BaseControl.Create ( 1, "ATTENDANCE_PAGE_CONTENTS_NUMBER_BASE" );
    const UIRECT& rcBaseLocal = BaseControl.GetLocalPos();

    // 10 자리
    {
        UINT nNumber = m_nDays/10;
        m_pDays[0]->SetImageEx( "ATTENDANCE_PAGE_CONTENTS_NUMBER", (UINT)(rcBaseLocal.sizeX)*nNumber, 0, (UINT)rcBaseLocal.sizeX, (UINT)rcBaseLocal.sizeY );
    }

    // 1자리
    {
        UINT nNumber = m_nDays%10;
        m_pDays[1]->SetImageEx( "ATTENDANCE_PAGE_CONTENTS_NUMBER", (UINT)(rcBaseLocal.sizeX)*nNumber, 0, (UINT)rcBaseLocal.sizeX, (UINT)rcBaseLocal.sizeY );
    }
}
