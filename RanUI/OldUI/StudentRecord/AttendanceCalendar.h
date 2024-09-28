#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

//-----------------------------------------------------------------------------------------------//

class CAttendanceCalendar : public CUIGroup
{

public:
    CAttendanceCalendar(EngineDeviceMan* pEngineDevice);
    virtual ~CAttendanceCalendar();

public:
    enum 
    {
        ATTENDANCE_CALENEDAR = NO_ID,
    };

public:
    void	        CreateSubControl ();           

public:
    virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    void            SetDays ( const UINT nDays );

private:
    CSwapImage*     m_pDays[2];

private:
    UINT            m_nDays;

};
