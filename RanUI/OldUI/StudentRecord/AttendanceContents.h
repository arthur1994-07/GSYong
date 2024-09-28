#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

//-----------------------------------------------------------------------------------------------//

#include <vector>

//-----------------------------------------------------------------------------------------------//

class GLGaeaClient;
class CInnerInterface;
class CBasicLineBoxSmart;
class CBasicTextBox;
class CAttendanceTask;

namespace attendance
{
    class GLAttendanceClient;
};

//-----------------------------------------------------------------------------------------------//

class CAttendanceContents : public CUIGroup
{

public:
    CAttendanceContents(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CAttendanceContents ();

public:
    typedef std::vector<CAttendanceTask*>  CTASKVEC;
    typedef CTASKVEC::iterator             CTASKVEC_ITER;

public:
    enum 
    {
        ATTENDANCE_CONTENTS = NO_ID,
        ATTENDANCE_TASK_BASE,
    };

    enum
    {
        MAX_TASK_SLOT = 4
    };

public:
    void	            CreateSubControl ();           

public:
    virtual	void	    Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void	    TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    void                Update ( attendance::GLAttendanceClient* pAttendance );       
	void				UnlockUI();

private:
    CBasicLineBoxSmart* CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID=NO_ID );
    CAttendanceTask*    CreateTask    ( const UINT nIndex ); 

private:
    CTASKVEC            m_TaskList;
    //UINT            m_nProgress;

private:
    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;
	bool m_bLock;
	CRITICAL_SECTION	m_CriticalSection;
};
