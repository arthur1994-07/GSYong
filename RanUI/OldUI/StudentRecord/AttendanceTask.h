#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

//-----------------------------------------------------------------------------------------------//

class GLGaeaClient;
class CInnerInterface;
class CBasicLineBoxSmart;
class CBasicTextBox;
class CAttendanceAward;
class CD3DFontPar;
class TaskN;

//-----------------------------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_TASKBUTTON_PUSHUP = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CAttendanceTask : public CUIGroup
{

public:
    CAttendanceTask(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CAttendanceTask ();

public:
    enum 
    {
        ATTENDANCE_TASK = NO_ID,
        ATTENDANCE_TASK_BUTTON,
    };

public:
    enum
    {
        TITLE_HEAD  = 0,
        TITLE_BODY  = 1,
        TITLE_TAIL  = 2,
        TITLE_MAX,
    };

public:
    enum EMSTATE
    {
        EMSTATE_ENABLE,
        EMSTATE_DISABLE,
        EMSTATE_COMPLETE,
        EMSTATE_EMPTY,
		EMSTATE_LOCKUI,
    };

public:
    struct SBUTTON : public CUIGroup
    {
        static const D3DCOLOR COLOR_BUTTON_RELEASE  = D3DCOLOR_ARGB(255, 255, 255, 255);
        static const D3DCOLOR COLOR_BUTTON_PUSH     = D3DCOLOR_ARGB(255, 100, 100, 100);
        static const D3DCOLOR COLOR_TEXT_RELEASE    = D3DCOLOR_ARGB(255, 253, 200, 0);
        static const D3DCOLOR COLOR_TEXT_PUSH       = D3DCOLOR_ARGB(255, 100, 100, 100);

        CUIControl*     pHEAD;
        CUIControl*     pBODY;
        CUIControl*     pTAIL;
        CBasicTextBox*  pTEXT;
        bool            bKEYDOWN;

        enum { ID_TEXT_BUTTON = NO_ID+1 };
        SBUTTON(EngineDeviceMan* pEngineDevice)
            : CUIGroup(pEngineDevice)
            , pHEAD(NULL)
            , pBODY(NULL)
            , pTAIL(NULL)
            , pTEXT(NULL)
            , bKEYDOWN(false)
        {
        }

        void CreateSubControl   ( CD3DFontPar* pFont, const std::string& strText );
        void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
        void Push    ();
        void Release ();
    };

public:
    static const D3DCOLOR COLOR_LINE = D3DCOLOR_ARGB(255, 100, 100, 100);

public:
    void	            CreateSubControl ();           

public:
    virtual	void	    Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void	    TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    void                Update ( TaskN* pTask, const UINT nIndex, const bool bLock, bool bLockUI );

public:
    void                SetState ( const EMSTATE emState );
    const EMSTATE       GetState () { return m_emState; }

private:
    void                UpdateState ();

private:
    CBasicLineBoxSmart* CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID=NO_ID ); 

private:
    CUIControl*         m_pTitle[TITLE_MAX];
    CUIControl*         m_pLine;
    SBUTTON*            m_pButton;
    CBasicTextBox*      m_pTaskCaption;
    CBasicTextBox*      m_pTaskCaption_Disable;
    CBasicTextBox*      m_pTaskProgress;
    CBasicTextBox*      m_pAwardReceived;
    CAttendanceAward*   m_pAward;

private:
    UINT                m_nIndex;
    bool                m_bReceived;
    EMSTATE             m_emState;

private:
    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;

};

//-----------------------------------------------------------------------------------------------//

