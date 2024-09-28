#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "../../../RanLogicClient/Attendance/GLAttendanceClient.h"

#include "../../InnerInterface.h"

#include "./AttendanceContents.h"
#include "./AttendanceTask.h"

#include <boost/foreach.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CAttendanceContents::CAttendanceContents(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient ( pGaeaClient )
    , m_pInterface  ( pInterface  )
{
    //Blank
	InitializeCriticalSection( &m_CriticalSection );
}

CAttendanceContents::~CAttendanceContents ()
{
    //Blank
	DeleteCriticalSection( &m_CriticalSection );
}

void CAttendanceContents::CreateSubControl ()
{
    // 라인박스
    {
        CreateLineBox ( "ATTENDANCE_PAGE_CONTENTS_LINEBOX", "ATTENDANCE_PAGE_CONTENTS_LINEBOX_TEXINFO" );
    }

    // 수행작업
    for ( UINT i=0; i<MAX_TASK_SLOT; ++i )
    {
        CAttendanceTask* pTask = CreateTask ( i );
        m_TaskList.push_back( pTask );
    }
}

void CAttendanceContents::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CAttendanceContents::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CAttendanceContents::Update ( attendance::GLAttendanceClient* pAttendance )
{
    if ( !pAttendance )
    {
        return;
    }

    UINT nMaxTask = pAttendance->GetMaxTasks  ();

    // Task Update
    UINT nTaskIndex = 0;
    for ( CTASKVEC_ITER iter = m_TaskList.begin(); iter != m_TaskList.end(); ++iter, ++nTaskIndex )
    {
        CAttendanceTask* pUITask = (*iter);

        if ( nMaxTask <= nTaskIndex )
        {
            pUITask->Update ( NULL, 0, false, true );
        }
        else
        {
            bool bLock = (nTaskIndex == 0) ? false : !( pAttendance->IsCompletedTask(0) );
            pUITask->Update ( &pAttendance->GetTaskN(nTaskIndex), nTaskIndex, bLock, m_bLock );
        }
    }
}

void CAttendanceContents::UnlockUI()
{
	EnterCriticalSection( &m_CriticalSection );
	m_bLock = false;
	LeaveCriticalSection( &m_CriticalSection );

	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ATTEND_LOAD_END") );
}

CBasicLineBoxSmart* CAttendanceContents::CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID )
{
    CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
    pLineBox->CreateSub( this, strControlName.c_str(), UI_FLAG_DEFAULT, cID );
    pLineBox->CreateSubControl( strTexInfo.c_str() );
    RegisterControl( pLineBox );
    return pLineBox;
}

CAttendanceTask* CAttendanceContents::CreateTask ( const UINT nIndex )
{
    CAttendanceTask* pTask;
    pTask = new CAttendanceTask ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
    pTask->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_BASE", UI_FLAG_DEFAULT, ATTENDANCE_TASK_BASE+nIndex );
    pTask->CreateSubControl ();
    RegisterControl( pTask );

    CUIControl BaseControl(m_pEngineDevice); 
    BaseControl.Create ( 1, "ATTENDANCE_PAGE_CONTENTS_TASK_BASE" );
    const UIRECT& rcBaseLocal = BaseControl.GetLocalPos();

    pTask->AddPosition( D3DXVECTOR2(0.0f, rcBaseLocal.sizeY*(float)nIndex) );
    return pTask;
}