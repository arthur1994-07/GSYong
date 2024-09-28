#pragma once

#include "../../RanLogic/Attendance/DayN.h"

//--------------------------------------------------------------------------------------------//

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

//--------------------------------------------------------------------------------------------//
#include "../Widget/GLWidgetScript.h"

namespace LuaPlus
{
    class LuaState;
}

namespace ticpp
{
    class Document;
}

class TaskN;
class GLGaeaClient;

//--------------------------------------------------------------------------------------------//

namespace attendance
{
	enum EMSTATE
	{
		EMSTATE_ENABLE,
		EMSTATE_DISABLE,
		EMSTATE_COMPLETE,
		EMSTATE_EMPTY,
		EMSTATE_LOCKUI,
	};

    struct SOBJECTTEXT
    {
        typedef boost::function<std::string             (TaskN*)>  FUNCSTR;
        typedef boost::function<std::wstring            (TaskN*)>  FUNCWSTR;
        typedef boost::function<const unsigned short*   (TaskN*)>  FUNCLUAWSTR;

        UINT    nID;
        FUNCSTR funcCAPTION_1;
        FUNCSTR funcCAPTION_2;
        FUNCSTR funcPROGRESS;

        SOBJECTTEXT ();
        static FUNCSTR GETNULLFUNCSTR    ();
        static FUNCSTR GETDEFAULTFUNCSTR ( const std::string& strFormat );
        static FUNCSTR GETFUNCSTR        ( const std::string& strFormat, FUNCSTR& funcStr );
        static FUNCSTR GETFUNCSTR        ( FUNCSTR&  funcStr1, FUNCSTR&     funcStr2     );
        static FUNCSTR GETFUNCSTR        ( FUNCSTR&  funcStr1, FUNCWSTR&    funcWStr2    );
        static FUNCSTR GETFUNCSTR        ( FUNCSTR&  funcStr1, FUNCLUAWSTR& funcLuaWStr2 );

        enum EMSTYLE { EMSTYLE_CAPTION=0, EMSTYLE_PROGRESS=1, };
        static FUNCSTR MAKE_TYPE_FUNC    ( const std::string& strType, const EMSTYLE emStyle );

        static std::string CAPTION_TYPE_TIME       ( TaskN* );
        static std::string CAPTION_TYPE_CASH       ( TaskN* );
        static std::string CAPTION_TYPE_ITEM       ( TaskN* );
        static std::string CAPTION_TYPE_ITEM_NUMS  ( TaskN* );
        static std::string PROGRESS_TYPE_TIME      ( TaskN* );
        static std::string PROGRESS_TYPE_CASH      ( TaskN* );
        static std::string PROGRESS_TYPE_ITEM      ( TaskN* );
        static std::string PROGRESS_TYPE_ITEM_NUMS ( TaskN* );
    };

    class GLAttendanceClient : private DayN, private boost::noncopyable
    {

    public:
        GLAttendanceClient  ( GLGaeaClient* pGaeaClient );
        ~GLAttendanceClient ();

    //---------------- UI ----------------//
    public:
        typedef std::map<UINT,SOBJECTTEXT>  SOBJTEXTMAP;
        typedef SOBJTEXTMAP::iterator       SOBJTEXTMAP_ITER;

    public:
        const std::string&  UIGetFileName()  { return m_strUIFilePath; }

    public:
        const bool          UILoad();
        void                UIClear();

    public:
        std::string         UIGetStringCaption_1 ( TaskN* pTask );
        std::string         UIGetStringCaption_2 ( TaskN* pTask );
        std::string         UIGetStringProgress  ( TaskN* pTask );

    private:
        const bool          UIParseLua ();
        const bool          UIParseXML ();

    private:
        SOBJECTTEXT*        UIFindObjectText ( const UINT nID );

    private:
        ticpp::Document*    m_pXmlDoc;
        LuaPlus::LuaState*  m_pLuaStateUi;
        SOBJTEXTMAP         m_ObjectTextMap;

    private:
        std::string		    m_strUIFilePath;

    private:
		bool				m_bLock;
    //---------------- DATA ----------------//
    private:
        typedef std::map<UINT,bool>  SREQUESTTBLE;

    public:
        HRESULT     	    FrameMove ( float fTime, float fElapsedTime );

        void                ResetData ();

        const UINT          GetMaxTasks ();
        const UINT          GetItemTakeAbleCount ();
        const bool          IsCompletedTask ( const UINT nIndex );

        void                SetTask  ( const TaskN* pTaskArray, const UINT nCount );
        TaskN&              GetTaskN ( const UINT nIndex ) { return m_arrTasks[nIndex]; }

        void                SetProgressDays ( const UINT nDays );
        const UINT          GetProgressDays ();

        void                SetTaskValueUpdate ( const UINT nIndex, const UINT nValue );
        void                SetTookRewards     ( const UINT nIndex, const bool bTook );
        void                SetTaskValueReset  ( const UINT nIndex );
        void                SetTaskCompleted   ( const UINT nIndex );
    
        const bool          IsRequested ( const UINT nMsg ) { return m_ReqTable[nMsg]; }
        void                SetRequest  ( const UINT nMsg, const bool bEnable ) { m_ReqTable[nMsg] = bEnable; }

		std::string AttendanceUIGetStringCaption_1( UINT nTaskIndex );
		std::string AttendanceUIGetStringCaption_2( UINT nTaskIndex );
		std::string AttendanceUIGetStringProgress( UINT nTaskIndex );
		
		lua_tinker::table GetPointTableInTask( UINT nTaskIndex );
		const DWORD AttendanceUIGetAwardItem( UINT nTaskIndex );
		const bool AttendanceIsReceivedAwardItem( UINT nTaskIndex );

		const EMSTATE GetState( UINT nTaskIndex );
		const UINT GetCalendar();

		void SetLock( const bool bLock ) { m_bLock = bLock; }

		void ReqAttendanceTakeReward(const UINT nTaskIndex);
    private:
        SREQUESTTBLE        m_ReqTable;

    private:
        GLGaeaClient*       m_pGaeaClient;

    };

};