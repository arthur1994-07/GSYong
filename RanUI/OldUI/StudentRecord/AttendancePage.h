#pragma once

#include "../Util/UIPage.h"

//-----------------------------------------------------------------------------------------------//

class CInnerInterface;
class GLGaeaClient;
class CAttendanceContents;
class CAttendanceDayList;
class CAttendanceCalendar;

class CAttendancePage : public CUIPage
{

public:
    enum
    {
        ATTENDANCE_PAGE = PAGE_NEXT,
        ATTENDANCE_PAGE_CONTENTS,
        ATTENDANCE_PAGE_DAYLIST,
        ATTENDANCE_PAGE_CALENDAR,
    };

public:
    CAttendancePage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~CAttendancePage();

public:
    void	             CreateSubControl ();

public:
    virtual void         Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void         TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void                 UpdateContents   ();
    void                 UpdateDayList    ();
    void                 UpdateCalendar   ();
	void				UnlockUI();

protected:
    virtual void         BeginSubControl();

protected:
    CInnerInterface*     m_pInterface;
    GLGaeaClient*        m_pGaeaClient;

protected:
    CAttendanceContents* m_pContents;
    CAttendanceDayList*  m_pDayList;
    CAttendanceCalendar* m_pCalendar;
};
