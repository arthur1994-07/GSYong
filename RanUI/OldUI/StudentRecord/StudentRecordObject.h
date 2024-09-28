#pragma	once

#include "../Util/UIWindowObject.h"
#include "../Util/TapSelectorEx.h"

class CInnerInterface;
class CBasicLineBoxSmart;
class CBasicTextBox;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_STUDENTRECORD_OBJECT_TAP_CHANGE = UIMSG_WINDOW_OBJECT_NEXT;
////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------//

// struct SSTUDENT_RECORD_OBJECT_TAPBUTTON : public CUIGroup
// {
//     CBasicLineBoxSmart* ACTIVE;
//     CBasicLineBoxSmart* INACTIVE;
//     CBasicTextBox*      TEXTBOX;
//     bool                ENABLE;
// 
//     SSTUDENT_RECORD_OBJECT_TAPBUTTON(EngineDeviceMan* pEngineDevice)
//         : CUIGroup(pEngineDevice)
//         , ACTIVE(NULL)
//         , INACTIVE(NULL)
//         , TEXTBOX(NULL)
//         , ENABLE(FALSE)
//     {
//     }
// 
//     void CreateSubControl ( const std::string& strLabel );
//     void SetEnable        ( const bool bEnable );
// };

//---------------------------------------------------------------------//

class CStudentRecordObject : public CUIWindowObject, private CTapSelectorEx<DEFAULT_TAPBUTTON>
{

protected:
	typedef CTapSelectorEx<DEFAULT_TAPBUTTON>::BUTTON			  STAPBUTTON;
    typedef CTapSelectorEx<STAPBUTTON>							  TAPSELECTOR;

protected:
    enum
    {
        STUDENTRECORD_OBJECT_NONE = WINDOW_OBJECT_NEXT,

        STUDENTRECORD_OBJECT_TAP_ACTIVITY,
        STUDENTRECORD_OBJECT_TAP_ATTENDANCE,
    };

public:
    enum
    {
        TAP_ACTIVITY   = STUDENTRECORD_OBJECT_TAP_ACTIVITY,
        TAP_ATTENDANCE = STUDENTRECORD_OBJECT_TAP_ATTENDANCE,

        TAP_MAX     = 2,
        TAP_DEFAULT = TAP_ACTIVITY,
    };

public:
    CStudentRecordObject ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CStudentRecordObject ();

public:
    virtual void		Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void		TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    void		        SelectTapAttendance () { TAPSELECTOR::TapSelect(TAP_ATTENDANCE); }
    const UIGUID		GetSelectedTapID    () { return TAPSELECTOR::GetTapID(); }

protected:
    virtual void		CreateSubControlEx ();

private:
//     virtual void	    TapEnable  ( TAPSELECTOR::BUTTON* );
//     virtual void		TapDisable ( TAPSELECTOR::BUTTON* );

private:
//    CBasicLineBoxSmart* CreateLineBox( const std::string& strControl, const std::string& strTexInfo );

private:
    STAPBUTTON*         m_pTapButton[TAP_MAX];

};
