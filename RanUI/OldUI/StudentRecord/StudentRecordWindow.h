// bjju.StudentRecord

#pragma	once

#include "../Util/UIWindowObjectController.h"

#include "../../InnerInterface.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CUIWindowEx;
class CStudentRecordObject;

class CActivityWindowPage;
class CAttendancePage;
class CActivityHelpWindow;
class CActivitySalesItemWindow;

//------------------------------------------
class RnattendancePage;
//------------------------------------------

class CStudentRecordWindow : public CUIWindowObjectController
{

protected:
    enum
    {
        STUDENTRECORD_WINDOW_NONE   = NO_ID,
        STUDENTRECORD_WINDOW_OBJECT,

        STUDENTRECORD_WINDOW_PAGE_ACTIVITY,
        STUDENTRECORD_WINDOW_PAGE_ATTENDANCE,
    };

public:
    CStudentRecordWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CStudentRecordWindow();

public:
    virtual void		    Update				( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void		    UpdatePageRequest	( UIGUID ControlID, const SPAGEREQUEST& sRequest );
    virtual	void		    TranslateUIMessage	( UIGUID ControlID, DWORD dwMsg );

public:
    void				    CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
    virtual	void		    SetVisibleSingle ( BOOL bVisible );

public:
    virtual const bool      IsOpenable ();
    virtual const bool      IsOpenableActivityPage ();
    virtual const bool      IsOpenableAttendancePage ();
    virtual const bool      IsOpenActivityPage ();
    virtual const bool      IsOpenAttendancePage ();
    virtual void            UpdateActivity ();
    virtual void            UpdateActivityPoint ();
    virtual void            OpenAttendanceWindow ();

public: // RnAttendance
	void RnAttendanceSetDayInfoText( const CString strEventText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	void RnAttendanceClerText();
	void REFRESH_RnattendancePage();

public:
    virtual void            UpdateAttendance ();
    virtual void            UpdateAttendanceTask ();
	virtual void			UnlockAttendanceUI();
public:
    virtual const bool      IsActivityProgressDisplay ();
    virtual void            SetActivityProgressDisplay( const bool bEnable );

public:
    const bool              IsOpenModalWindow ();
    void                    CloseModalWindow  ();

public:
    CActivityWindowPage*    GetActivityPage ()   { return m_pPage_Activity;   }
	CAttendancePage*        GetAttendancePage () { return m_pPage_Attendance; }
	RnattendancePage*		GetRnattendancePage(){ return m_pRnattendancePage; }


public:
    HRESULT                 InfoFileLoad ();
    HRESULT                 InfoFileSave ();

protected:
    CBasicTextBox*		    CreateStaticControl     (const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
    CUIControl*		        CreateBackgroundControl ( char* szControlKeyword );

private:
    CStudentRecordObject*	    m_pUIObject;

private:
    CActivityWindowPage*        m_pPage_Activity;
    CAttendancePage*            m_pPage_Attendance;
	//------------------------------------------
	RnattendancePage*			m_pRnattendancePage;
	//------------------------------------------

private:
    CActivityHelpWindow*        m_pActivityHelp;
    CActivitySalesItemWindow*   m_pActivitySalesItem;
    CUIWindowEx*                m_pOpenModal;

    bool                        m_bFirst;
protected:
    GLGaeaClient*               m_pGaeaClient;

};

//----------------------------------------------------------------------------------------------------//

class MyStudentRecordWindow : public IStudentRecordWindow, private CStudentRecordWindow
{
public:
    MyStudentRecordWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyStudentRecordWindow()    {};

public:
    virtual void       CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual void       UpdateActivity ();
    virtual void       UpdateActivityPoint ();
    virtual void       UpdateAttendance ();
    virtual void       UpdateAttendanceTask ();
	virtual void		UnlockAttendanceUI();
    virtual const bool IsOpenable ();
    virtual const bool IsOpenableActivityPage ();
    virtual const bool IsOpenableAttendancePage ();
    virtual const bool IsOpenActivityPage ();
    virtual const bool IsOpenAttendancePage ();
    virtual const bool IsActivityProgressDisplay ();
    virtual const bool IsOpenModalWindow ();
    virtual void       OpenAttendanceWindow ();
    virtual void       CloseModalWindow();

public: // RnAttendance
	virtual void	RnAttendanceSetDayInfoText( const CString strEventText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	virtual void	RnAttendanceClerText();
	virtual void	REFRESH_RnattendancePage();

};
