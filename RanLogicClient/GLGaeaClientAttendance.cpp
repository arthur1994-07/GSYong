#include "pch.h"
#include "../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"

#include "./Attendance/GLAttendanceClient.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::AttendanceUIReload()
{
    m_pAttendance->UIClear();
    m_pAttendance->UILoad();
}

void GLGaeaClient::AttendanceUILoad()
{
    m_pAttendance->UILoad();
}

std::string GLGaeaClient::AttendanceUIGetStringCaption_1 ( TaskN* pTask )
{
    return m_pAttendance->UIGetStringCaption_1 ( pTask );
}

std::string GLGaeaClient::AttendanceUIGetStringCaption_2 ( TaskN* pTask )
{
    return m_pAttendance->UIGetStringCaption_2 ( pTask );
}

std::string GLGaeaClient::AttendanceUIGetStringProgress  ( TaskN* pTask )
{
    return m_pAttendance->UIGetStringProgress ( pTask );
}

const UINT GLGaeaClient::AttendanceGetProgressDays ()
{
    return m_pAttendance->GetProgressDays ();
}

const UINT GLGaeaClient::AttendanceGetItemTakeAbleCount ()
{
    return m_pAttendance->GetItemTakeAbleCount ();
}

void GLGaeaClient::MsgAttendanceSync(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ATTENDANCE_SYNC_FAC *pPacket = (GLMSG::SNET_ATTENDANCE_SYNC_FAC *)pMsg;

    m_pAttendance->SetProgressDays ( pPacket->nDayN );
    m_pAttendance->SetTask ( pPacket->Tasks, pPacket->nTasks );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Attendance );
}

void GLGaeaClient::MsgAttendanceReset(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ATTENDANCE_RESET_FAC *pPacket = (GLMSG::SNET_ATTENDANCE_RESET_FAC *)pMsg;

	if( pPacket->nTasks != 4 )
	{
		sc::writeLogError(sc::string::format("ATTENDANCE ERROR - Task Count %1%", pPacket->nTasks));
	}

    m_pAttendance->SetProgressDays ( 0 );
    m_pAttendance->SetTask ( pPacket->Tasks, pPacket->nTasks );

	if( EMSTAGE_BG_GAME != GfxInterfaceBase::GetInstance()->GetPrevStageBG() )
	{
		if ( GLUseFeatures::GetInstance().IsUsingActivity() )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Open );
		}
	}
}

void GLGaeaClient::MsgAttendanceUpgrade(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ATTENDANCE_UPGRADE_FAC *pPacket = (GLMSG::SNET_ATTENDANCE_UPGRADE_FAC *)pMsg;

    m_pAttendance->SetProgressDays ( pPacket->nDayN );
    m_pAttendance->SetTask ( pPacket->Tasks, pPacket->nTasks );

	/*if( EMSTAGE_BG_GAME != GfxInterfaceBase::GetInstance()->GetPrevStageBG() )
	{
		if ( GLUseFeatures::GetInstance().IsUsingActivity() )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Open );
		}
	}*/
}

void GLGaeaClient::MsgAttendanceTaskUpdate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ATTENDANCE_TASK_UPDATE_FC *pPacket = (GLMSG::SNET_ATTENDANCE_TASK_UPDATE_FC *)pMsg;

    m_pAttendance->SetTaskValueUpdate ( pPacket->idx, pPacket->nValue );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_AttendanceContents );
}

void GLGaeaClient::MsgAttendanceTaskReset(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ATTENDANCE_TASK_RESET_FC *pPacket = (GLMSG::SNET_ATTENDANCE_TASK_RESET_FC *)pMsg;

    m_pAttendance->SetTaskValueReset ( pPacket->idx );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_AttendanceContents );
}

void GLGaeaClient::MsgAttendanceTaskCompleted(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ATTENDANCE_TASK_COMPLETED_FC *pPacket = (GLMSG::SNET_ATTENDANCE_TASK_COMPLETED_FC *)pMsg;

    m_pAttendance->SetTaskCompleted ( pPacket->idx );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_AttendanceContents );

    if ( GLUseFeatures::GetInstance().IsUsingAttendance() )
    {
        if (pPacket->idx < MAX_TASKS_PER_ONE_DAY)
        {
            std::string strText = AttendanceUIGetStringCaption_1( &m_pAttendance->GetTaskN( pPacket->idx ) );
            strText = strText.substr( 0, strText.find_last_of(':') );
            strText = sc::string::format ( ID2GAMEINTEXT("ATTENDANCE_UPDATE_COMPLETE_PROGRESSDISPLAY"), strText );

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_AttendanceComplete_Show,
				"-s", strText.c_str() );
        }
    }
}

void GLGaeaClient::MsgAttendancePopup(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ATTENDANCE_POPUP_COMPLETE_FC *pPacket = (GLMSG::SNET_ATTENDANCE_POPUP_COMPLETE_FC *)pMsg;

}

void GLGaeaClient::MsgAttendanceTakeReward(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ATTENDANCE_TAKEREWARD_FB *pPacket = (GLMSG::SNET_ATTENDANCE_TAKEREWARD_FB *)pMsg;
    m_pAttendance->SetRequest(NET_MSG_ATTENDANCE_TAKEREWARD_CF, false);

    m_pAttendance->SetTookRewards ( pPacket->idxTask, pPacket->bTook ? true : false );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_AttendanceContents );
}

void GLGaeaClient::MsgAttendanceDonePointshopItem(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ATTENDANCE_DONE_POINTITEM_FB *pPacket = (GLMSG::SNET_ATTENDANCE_DONE_POINTITEM_FB *)pMsg;
    m_pAttendance->SetRequest(NET_MSG_ATTENDANCE_DONE_POINTITEM_CF, false);
}

void GLGaeaClient::MsgAttendanceLoadEnd(NET_MSG_GENERIC* pMsg)
{
	m_pAttendance->SetLock( false );
}

void GLGaeaClient::ReqAttendanceTakeReward(const UINT nTaskIndex)
{
    if ( !GLUseFeatures::GetInstance().IsUsingAttendance() )
    {
        return;
    }

    if ( m_pAttendance->IsRequested(NET_MSG_ATTENDANCE_TAKEREWARD_CF) )
    {
        return;
    }

    m_pAttendance->SetRequest(NET_MSG_ATTENDANCE_TAKEREWARD_CF, true);

    GLMSG::SNET_ATTENDANCE_TAKEREWARD_CF Packet;
    Packet.idxTask = nTaskIndex;
    NETSENDTOFIELD(&Packet);
}

void GLGaeaClient::ReqAttendanceDonePointshopItem(const UINT nTaskIndex)
{
    if ( !GLUseFeatures::GetInstance().IsUsingAttendance() )
    {
        return;
    }

    if ( m_pAttendance->IsRequested(NET_MSG_ATTENDANCE_DONE_POINTITEM_CF) )
    {
        return;
    }

    m_pAttendance->SetRequest(NET_MSG_ATTENDANCE_DONE_POINTITEM_CF, true);

    GLMSG::SNET_ATTENDANCE_DONE_POINTITEM_CF Packet;
    Packet.idx = nTaskIndex;
    NETSENDTOFIELD(&Packet);
}

