#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/gassert.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/Common/SUBPATH.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Attendance/DayN.h"
#include "../../RanLogic/Attendance/TaskN.h"

#include "../GLGaeaClient.h"
#include "./GLAttendanceClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace attendance;

//--------------------------------------------------------------------------------------------//
GLAttendanceClient::GLAttendanceClient( GLGaeaClient* pGaeaClient )
: m_pGaeaClient(pGaeaClient)
, m_pXmlDoc(NULL)
, m_pLuaStateUi(NULL)
, m_strUIFilePath("")
, m_bLock(false)
{
	m_UIUpdatedTime = 0;
}

GLAttendanceClient::~GLAttendanceClient()
{
    UIClear();
}

void GLAttendanceClient::ResetData ()
{
    SetN(0);
    SetNumOfTasks(0);

    for ( UINT i=0; i<MAX_TASKS_PER_ONE_DAY; ++i )
    {
        m_arrTasks[i] = TaskN();
    }
}

HRESULT GLAttendanceClient::FrameMove ( float fTime, float fElapsedTime )
{
    if ( !GLUseFeatures::GetInstance().IsUsingAttendance() )
    {
        return S_FALSE;
    }

    bool bUpdate   = false;
    UINT nMaxTasks = GetMaxTasks();
	DWORD dwCurTime = ::GetTickCount();

    for ( UINT i=0; i<nMaxTasks; ++i )
    {
        if ( 0<i && !IsCompletedTask (0) )
        {
            break;
        }

        TaskN& sTask = GetTaskN(i);

        switch ( m_arrTasks[i].GetType() )
        {
        case ATTENDANCE_TYPE_CONNECTION:
        case ATTENDANCE_TYPE_SPARRING_SOLO:
        case ATTENDANCE_TYPE_SPARRING_PARTY:	
        case ATTENDANCE_TYPE_WHISPER:
        case ATTENDANCE_TYPE_PARTY:
        case ATTENDANCE_TYPE_ENCHANTW:
        case ATTENDANCE_TYPE_ENCHANTWC:
        case ATTENDANCE_TYPE_ENCHANTA:
        case ATTENDANCE_TYPE_ENCHANTAC:
        case ATTENDANCE_TYPE_POSTSEND:	
            {
                // 기존에는 출석부창을 필드서버 이동인 경우에만 다시 그리도록 되어있는데
				// 사용자가 출석부창을 열고 있다면 일정시간마다 계속해서 다시 드리도록 설정함 ( 출석부 데이터랑 화면이랑 일치하지 않는 문제때문에 )
				// 만약 해당 기능으로 인해서 클라이언트가 느려진다면 사용자가 버튼을 눌러서 갱신하도록 수정할 필요가 있음
				if( dwCurTime - m_UIUpdatedTime > 1000 )
				{
					m_UIUpdatedTime = dwCurTime;
					bUpdate = true;
				}
            }
            break;

        // 포인트샵 장바구니
        case ATTENDANCE_TYPE_POINTITEM:
            {
                if ( !sTask.IsCompleted() )
                {
					__int64 i64Price = GLWidgetScript::GetInstance().LuaCallFunc< __int64 >( 
						NSWIDGET_SCRIPT::g_strFunc_PointShop_GetCartItemPrice );
                    UINT    nPrice   = static_cast<UINT>(i64Price);

                    if ( sTask.GetValue() != nPrice )
                    {
                        sTask.SetValue( nPrice );

                        if ( sTask.GetValueBase() <= sTask.GetValue() )
                        {
                            m_pGaeaClient->ReqAttendanceDonePointshopItem(i);
                        }

                        bUpdate = true;
                    }
                }
            }
            break;
        };
    }

    if ( bUpdate )
    {
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_AttendanceContents );
    }

    return S_OK;
}

const UINT GLAttendanceClient::GetMaxTasks ()
{
    return GetNumOfTasks();
}

const bool GLAttendanceClient::IsCompletedTask ( const UINT nIndex )
{
    if ( GetMaxTasks() <= nIndex || nIndex >= MAX_TASKS_PER_ONE_DAY)
    {
        return false;
    }

    if ( ATTENDANCE_STATUS_COMPLETED == m_arrTasks[nIndex].GetStatus() )
    {
        return true;
    }

    return false;
}

const UINT GLAttendanceClient::GetItemTakeAbleCount ()
{
    UINT nMaxTasks = GetMaxTasks();
    UINT nCount    = 0;

    for ( UINT i=0; i<nMaxTasks; ++i )
    {
        if ( ATTENDANCE_STATUS_COMPLETED == m_arrTasks[i].GetStatus() )
        {
            if ( !m_arrTasks[i].IsTookRewards() ) 
            {
                ++nCount;
            }
        }
    }

    return nCount;
}

void GLAttendanceClient::SetTask ( const TaskN* pTaskArray, const UINT nCount )
{
    if (nCount > MAX_TASKS_PER_ONE_DAY)
        return;

    SetNumOfTasks(nCount);

    for ( UINT i=0; i<nCount; ++i )
    {
        m_arrTasks[i] = pTaskArray[i];
    }
}

void GLAttendanceClient::SetProgressDays ( const UINT nDays )
{
    m_N = nDays;
}

const UINT GLAttendanceClient::GetProgressDays ()
{
    return GetN();
}

void GLAttendanceClient::SetTaskValueUpdate ( const UINT nIndex, const UINT nValue )
{
    if ( MAX_TASKS_PER_ONE_DAY <= nIndex )
    {
        GASSERT( !"GLAttendanceClient::SetTaskValueUpgrade() - nIndex exceed the maximum");
        return;
    }

    TaskN& cTask = m_arrTasks[nIndex];
    cTask.Update( cTask.GetType(), nValue );
}

void GLAttendanceClient::SetTookRewards ( const UINT nIndex, const bool bTook )
{
    if ( MAX_TASKS_PER_ONE_DAY <= nIndex )
    {
        GASSERT( !"GLAttendanceClient::SetTookRewards() - nIndex exceed the maximum");
        return;
    }

    TaskN& cTask = m_arrTasks[nIndex];
    cTask.TookRewards( bTook ? TRUE : FALSE );
}

void GLAttendanceClient::SetTaskValueReset ( const UINT nIndex )
{
    if ( MAX_TASKS_PER_ONE_DAY <= nIndex )
    {
        GASSERT( !"GLAttendanceClient::SetTaskValueReset() - nIndex exceed the maximum");
        return;
    }

    TaskN& cTask = m_arrTasks[nIndex];
    cTask.Reset( cTask.GetType() );
}

void GLAttendanceClient::SetTaskCompleted ( const UINT nIndex )
{
    if ( MAX_TASKS_PER_ONE_DAY <= nIndex )
    {
        GASSERT( !"GLAttendanceClient::SetTaskCompleted() - nIndex exceed the maximum");
        return;
    }

    TaskN& cTask = m_arrTasks[nIndex];
    cTask.SetValue( cTask.GetValueBase() );
    cTask.Completed();
}

std::string GLAttendanceClient::AttendanceUIGetStringCaption_1( UINT nTaskIndex )
{
	std::string strCaption = "";
	if( nTaskIndex > MAX_TASKS_PER_ONE_DAY )
		return strCaption;

	TaskN* pTask = &m_arrTasks[nTaskIndex];
	if( !pTask )
		return strCaption;

	strCaption = m_pGaeaClient->AttendanceUIGetStringCaption_1( pTask );
	return strCaption;
}

std::string GLAttendanceClient::AttendanceUIGetStringCaption_2( UINT nTaskIndex )
{
	std::string strCaption = "";
	if( nTaskIndex > MAX_TASKS_PER_ONE_DAY )
		return strCaption;

	TaskN* pTask = &m_arrTasks[nTaskIndex];
	if( !pTask )
		return strCaption;

	strCaption = m_pGaeaClient->AttendanceUIGetStringCaption_2( pTask );
	return strCaption;
}

std::string GLAttendanceClient::AttendanceUIGetStringProgress( UINT nTaskIndex )
{
	std::string strProgress = "";
	if( nTaskIndex > MAX_TASKS_PER_ONE_DAY )
		return strProgress;

	TaskN* pTask = &m_arrTasks[nTaskIndex];
	if( !pTask )
		return strProgress;

	strProgress = m_pGaeaClient->AttendanceUIGetStringProgress( pTask );
	return strProgress;
}

lua_tinker::table GLAttendanceClient::GetPointTableInTask( UINT nTaskIndex )
{
	lua_tinker::table tbPoint( GLWidgetScript::GetInstance().GetLuaState() );
	tbPoint.set( 1, 0 );
	tbPoint.set( 2, 0 );
	tbPoint.set( 3, 0 );

	if( nTaskIndex > MAX_TASKS_PER_ONE_DAY )
		return tbPoint;

	TaskN* pTask = &m_arrTasks[nTaskIndex];
	if( !pTask )
		return tbPoint;

	tbPoint.set( 1, pTask->GetRewardExp() );
	tbPoint.set( 2, pTask->GetRewardPoint() );
	tbPoint.set( 3, pTask->GetRewardMoney() );

	return tbPoint;
}

const EMSTATE GLAttendanceClient::GetState( UINT nTaskIndex )
{
	if( nTaskIndex > MAX_TASKS_PER_ONE_DAY )
		return EMSTATE_EMPTY;

	TaskN* pTask = &m_arrTasks[nTaskIndex];
	if( !pTask )
		return EMSTATE_EMPTY;

	bool bLock = (nTaskIndex == 0) ? false : !( IsCompletedTask(0) );

	if( bLock )
		return EMSTATE_DISABLE;
	else if( m_bLock )
		return EMSTATE_LOCKUI;

	ENUM_ATTENDANCE_STATUS emStatus = pTask->GetStatus();
	if( emStatus == ATTENDANCE_STATUS_INPROGRESS )
		return EMSTATE_ENABLE;
	else if( emStatus == ATTENDANCE_STATUS_COMPLETED)
		return EMSTATE_COMPLETE;

	return EMSTATE_ENABLE;
}


const UINT GLAttendanceClient::GetCalendar()
{
	time_t    ltime;
	struct tm today;

	time(&ltime);
	localtime_s( &today, &ltime);

	return (UINT)today.tm_mday;
}

const DWORD GLAttendanceClient::AttendanceUIGetAwardItem( UINT nTaskIndex )
{
	if( nTaskIndex > MAX_TASKS_PER_ONE_DAY )
		return NATIVEID_NULL().dwID;

	TaskN* pTask = &m_arrTasks[nTaskIndex];
	if( !pTask )
		return NATIVEID_NULL().dwID;

	INT nMaxItem = (INT)pTask->GetNumberOfRewardItems();

	for ( INT i=nMaxItem; i>0; --i )
	{
		SNATIVEID sItemID = (DWORD)pTask->GetRewardItem( i-1 );

		if ( sItemID == NATIVEID_NULL() )
		{
			continue;
		}

		return sItemID.dwID;
	}

	return NATIVEID_NULL().dwID;
}

const bool GLAttendanceClient::AttendanceIsReceivedAwardItem( UINT nTaskIndex )
{
	if( nTaskIndex > MAX_TASKS_PER_ONE_DAY )
		return false;

	TaskN* pTask = &m_arrTasks[nTaskIndex];
	if( !pTask )
		return false;

	return pTask->IsTookRewards();
}

void GLAttendanceClient::ReqAttendanceTakeReward(const UINT nTaskIndex)
{
	m_pGaeaClient->ReqAttendanceTakeReward( nTaskIndex );
}
