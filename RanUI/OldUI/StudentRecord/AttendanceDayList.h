#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

//-----------------------------------------------------------------------------------------------//

class CBasicLineBoxSmart;
class CBasicTextBox;
class CInnerInterface;

//-----------------------------------------------------------------------------------------------//

class CAttendanceDayList : public CUIGroup
{

public:
    CAttendanceDayList(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CAttendanceDayList();

public:
    enum 
    {
        ATTENDANCE_DAYLIST = NO_ID,
        ATTENDANCE_BUTTON,
    };

    enum
    {
        MAX_DAYS = 7,
    };

public:
    struct SDAYBUTTON  : public CUIGroup
    {
        enum EMSTYLE { EMSTYLE_DEFAULT, EMSTYLE_COMPLETE, EMSTYLE_TODAY };

        CBasicLineBoxSmart*  LINEBOX[2];
        CBasicTextBox*       TEXTBOX[2];
        CUIControl*          TODAY[3];
        CUIControl*          CHECK;
        EMSTYLE              STYLE;
        UINT                 DAYS;

        SDAYBUTTON(EngineDeviceMan* pEngineDevice)
            : CUIGroup(pEngineDevice)
            , CHECK(NULL)
            , STYLE(EMSTYLE_DEFAULT)
            , DAYS(0)
        {
            memset(LINEBOX, 0, sizeof(LINEBOX));
            memset(TEXTBOX, 0, sizeof(TEXTBOX));
            memset(TODAY,   0, sizeof(TODAY));
        }

        void CreateSubControl ( const UINT nDay );
        void SetStyle ( const EMSTYLE emStyle );
    };

public:
    void	         CreateSubControl ();           

public:
    virtual	void	 Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void	 TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    void             SetProgress( const UINT nDays );

private:
    SDAYBUTTON*      CreateButton( const UIGUID cID, const UINT nDays ); 

private:
    SDAYBUTTON*      m_pButton[MAX_DAYS];

private:
    UINT             m_nProgress;

private:
    CInnerInterface* m_pInterface;

};
