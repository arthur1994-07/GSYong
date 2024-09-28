#include "../pch.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "./AttendanceManager.h"
#include "./AttendanceScriptMan.h"
#include "../Database/DBAction/DbActionGameAttendance.h"
#include "../Character/GLChar.h"
#include "../FieldServer/GLGaeaServer.h"

#include "../../RanLogic/GLUseFeatures.h"

#undef V	// RanLogic/stdafx.h에 V 매크로 정의되어 있음
#include "../../=Squirrel/include/sqrat.h"



// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



//
//this global flag is only set and get by UpdateThread. so lock isn't required.
//
BOOL	g_bAttendanceDailyMaintenance = FALSE;


AttendanceManager::AttendanceManager(GLGaeaServer* pGaeaServer, DWORD &dwOwerChaNum)
    : m_pGaeaServer(pGaeaServer)	
    , m_bLoaded(FALSE)
    , m_dwOwnerChaNum(dwOwerChaNum)
    , m_dwBaseTime(0)
    , m_dwCurrentTime(0)
    , m_dwPrevElapsedMinutes(0)
    , m_pCharOwner(NULL)
{
	if (pGaeaServer)
	{
		m_pScriptMan = pGaeaServer->GetAttendanceScriptMan();

		if (!m_pScriptMan)
		{
			sc::writeLogError(std::string("AttendanceManager() - m_pScriptMan is NULL."));		
		}
	}
	else
	{
		sc::writeLogError(std::string("AttendanceManager() - pGaeaServer is NULL."));		
	}
}


AttendanceManager::~AttendanceManager()
{
    m_pCharOwner = NULL;
}


BOOL AttendanceManager::VerifyDate(IN __time64_t& csStartDate, IN CTime &ctCurrentDate, OUT CTime *pctStartDate)
{
    /*
	int nYear = 0;
	int nMon = 0;
	int nDay = 0;
	int nHour = 0;
	int nMin = 0;
	int nSec = 0;
	
	sscanf(csStartDate.GetString(), "%d-%d-%d %d:%d:%d", &nYear, &nMon, &nDay, &nHour, &nMin, &nSec);
    */
    sc::time::AdjustTime(csStartDate);
	CTime ctStartDate(csStartDate);

	if (pctStartDate)
		*pctStartDate = ctStartDate;	//copy startdate

	CTimeSpan cts1Day(1,0,0,0);
	CTimeSpan cts3Min(0,0,3,0);

	/*
		AgentServer와 FieldServer의 시간이 정확히 맞지 않을 경우를 위해 " 3 분 "의 여유값이 둔다.

		AgentServer에 의해 새벽 6:00시에 AttendaceMaintenance가 발동되었을 때, FieldServer의 시간은 5:59:59 라고 하자.(실제 대만에서 문제가 됨.)
		이 때, VerifyDate가 실패함으로써 해당 캐릭터는 UPGRADE가 아니라 RESET되어 버린다.

		ex> 2011-08-09 06:00:00에 Agent에서 maintenance가 시작되었고, 이 때, UPGRADE 대상 캐릭터들의 StartDate는 전날 새벽 6:00에 +1일이 더해진 시간으로 
		DB SP에 의해 계산되어 저장된다. 즉, StartDate는 2011-08-09 06:00:00로 설정되어 있다.

		이 때, 해당 메시지가 Field로 전달되었다. 이 때, Field의 시간은 2011-08-09 05:59:59 이다.

		StartDate    : 2011-08-09 06:00:00
		CurrentTime  : 2011-08-09 5:59:59
		NextStartDate: 2011-08-10 06:00:00
	*/
	
	CTime ctNextStartDate = ctStartDate + cts1Day + cts3Min;	
	ctStartDate = ctStartDate - cts3Min;

	// 출석부 초기화 관련 버그 분석을 위해서 추가
	bool bRet = false;

	if( (ctStartDate <= ctCurrentDate) && (ctCurrentDate <= ctNextStartDate) )
	{
		bRet = true;
	}
	else
	{
		bRet = false;

		sc::writeLogError(sc::string::format("Attendacne Date is not match - StartDate : %1%, CurrentDate : %2%, NextStartDate : %3%", ctStartDate.Format("%Y-%m-%d %H:%M:%S"), ctCurrentDate.Format("%Y-%m-%d %H:%M:%S"), ctNextStartDate.Format("%Y-%m-%d %H:%M:%S") ));		
	}

	return bRet;
}


BOOL AttendanceManager::VerifyDate(IN CTime &ctStartDate, IN CTime &ctCurrentDate)
{
	CTimeSpan cts1Day(1,0,0,0);

	CTime ctNextStartDate = ctStartDate + cts1Day;

	return( (ctCurrentDate >= ctStartDate) && (ctCurrentDate < ctNextStartDate) );
}


void AttendanceManager::ComputeStartDate(IN CTime &ctCurrentDate)
{
	m_ctStartDate = CTime(ctCurrentDate.GetYear(), ctCurrentDate.GetMonth(), ctCurrentDate.GetDay(),
							ATTENDANCE_RESET_TIME_H, ATTENDANCE_RESET_TIME_M, ATTENDANCE_RESET_TIME_S);

	if (ctCurrentDate.GetHour() < ATTENDANCE_RESET_TIME_H)
	{
		CTimeSpan cts1Day(1,0,0,0);

		m_ctStartDate = m_ctStartDate - cts1Day;
	}
}


void AttendanceManager::SetTask(UINT idx, UINT nType, UINT nValueBase)
{
	if (m_nTasks == 0)
	{
		sc::writeLogError(std::string("AttendanceManager::SetTask() failure. The number of tasks MUST be set first."));
		return;
	}

	if (idx >= m_nTasks)
	{
		sc::writeLogError(std::string("AttendanceManager::SetTask() failure. idx is exceeds the maximum."));
		return;
	}

	GetTasks()[idx].SetCondtion(idx, nType, nValueBase);
}

void AttendanceManager::Load(GLChar *pChar, BOOL bTransfer/*DB를 사용하지 않고 캐릭터 이전*/)
{
	if(!GLUseFeatures::GetInstance().IsUsingAttendance())
	{
		//sc::writeLogWarn(std::string("Load() bypass. Attendance is turned OFF!"));
		return;
	}

	if( pChar == NULL )
	{
		sc::writeLogError(std::string("ActivityManager::Load(): Invalid GLChar"));
		return;
	}

	//점검 중에 Load가 Delay된 상태에서 TakeReward등의 요청을 보내는 클라이언트에 의해 m_pCharOwner->SendClient()등의 처리를 하다가 crash됨을 막기 위해 먼저 세팅
	m_pCharOwner = pChar;

	if (bTransfer)
		return;

	if (g_bAttendanceDailyMaintenance)	// 점검중에 접속한 것이라면, 점검 종료 후에 일괄 Load한다.
	{
		SetReady(FALSE);	//Load 완료시에 SetReady(TRUE)된다.

		sc::writeLogWarn(std::string("Attendance Load is delayed because of Attendance Maintenance."));
		return;
	}

    CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();
    pChar->m_pAttendanceMan->Reset( ctCurrentTime );

	pChar->AddGameAdoJob(
		db::DbActionPtr(
			new CAttendanceLoad(pChar->m_CharDbNum, pChar->m_pAttendanceMan)));
}


void AttendanceManager::Save(GLChar *pChar, BOOL bLastOne4Maintenance)
{
	if(!GLUseFeatures::GetInstance().IsUsingAttendance())
	{
		//sc::writeLogWarn(std::string("Save() bypass. Attendance is turned OFF!"));
		return;
	}

	if( g_bAttendanceDailyMaintenance )	// 점검이 완료되기 이전에 저장을 시도하면, 무시한다.
	{
		SetReady(FALSE);	//Load 완료시에 SetReady(TRUE)된다.

		sc::writeLogWarn(std::string("Attendance Save is ignored because of Attendance Maintenance."));
		return;
	}

	if (pChar)
	{
		m_pCharOwner = pChar;

		pChar->AddGameAdoJob(
			db::DbActionPtr(
				new CAttendanceSave(pChar->ClientSlot(), pChar->m_CharDbNum, pChar->m_sExperience.lnNow , pChar->m_pAttendanceMan, bLastOne4Maintenance)));
	}
	else
	{
		sc::writeLogError(std::string("ActivityManager::Save(): Invalid GLChar"));
	}
}


/*
	-------------------- Reset process --------------------


	[AgentS]
		(1) -> send Msg#1 to FieldS.
		(3) -> recv Msg#2 from all FieldS.
			-> put DBAction#1 for Attendance maintenance after receiving Msg#2 from all FieldS
				and send Msg#3 when the DBAction#1 finishes.


	[FieldS]
		(2) -> recv Msg#1
			-> iterate all GLChars and do following
				-> GLChar::AttendanceMan::SetReady(FALSE)
					-> to notice that all online-characters MUST upgrade or reset its attendance
						and newly connected characters will not load Attendance.
					(set and check only in UpdateThread)
				-> GLChar::AttendanceMan::Save()	-> put DBAction into queue
				-> Last DbAction of this process will send Msg#2 to AgentS after it has finished the job(AttendanceSave()).
			   

		(6) -> recv Msg#3 (DB finished maintenance procedure)
			-> iterate all GLChars and do following
				-> GLChar::AttendanceMan::Load()
				-> GLChar::AttendanceMan::SetReady(TRUE)


	[DBAction#1 in AgentS]
		(4-1) -> if the DayN is expired, delete all of DayN and TaskN rows.
		(4-2) -> if the DayN is completed, update the DayN row and delete all TaskN rows.
			(TaskN will be reset by FieldS when the character connects next time.)
		(5) -> send Msg#3 to all FieldS.



	Note:	Msg#1과 Msg#3 사이에 신규로 접속하는 캐릭터가 있을 수 있다.
			Msg#1과 Msg#3 사이에 접속을 종료하는 캐릭터가 있을 수 있다.

			위에 설명한 Reset Process의 흐름대로라면 이 2가지 경우도 문제될 것이다.
*/

//
// reset DayN -> Day1
//
void AttendanceManager::Reset(CTime &ctCurrentTime)
{
	//
	//There's no row for the attendance.
	// setup new startdate for the attendnace.
	ComputeStartDate(ctCurrentTime);
	

	m_dwBaseTime			= GetTickCount();
	m_dwCurrentTime			= m_dwBaseTime;
	m_dwPrevElapsedMinutes	= 0;

	SetN(0);
	
	//
	// RESET by Squirrel
	//
    if ( !m_pCharOwner )
    {
        sc::writeLogError( std::string("AttendanceManager::Reset - m_pCharOwner is null.") );

        return;
    }

//	sc::writeLogDebug(sc::string::format("SetUpDayN begins for \"%1%\"(%2%)", m_pCharOwner->m_szName, m_pCharOwner->CharDbNum()));
	DayN dayn = m_pScriptMan->SetDayN(m_N/* 0-base in Squirrel*/, (UINT)m_pCharOwner->m_wLevel, m_pCharOwner->GetMaxLevel());

	*((DayN *)this) = dayn;

	// Compute RewardExp based on RewardExpRate
	ComputeRewardExp(m_pCharOwner->GET_LEVELUP_EXP());


	if( m_nTasks != MAX_TASKS_PER_ONE_DAY )
	{
		sc::writeLogError( sc::string::format("[ATTENDANCE ERROR] AttendanceManager::Reset - ChaDBNum %1%, Task Count %2%, Day %3%, Level %4%, MaxLevel %5%", m_pCharOwner->CharDbNum(), m_nTasks, m_N, (UINT)m_pCharOwner->m_wLevel, m_pCharOwner->GetMaxLevel()) );
	}

	GLMSG::SNET_ATTENDANCE_RESET_FAC msg;
	msg.SetData(m_arrTasks, m_nTasks);

	//m_pCharOwner->SendToClient(&msg);
	msg.nChaNum = m_pCharOwner->CharDbNum();
	m_pCharOwner->SendToAgent(&msg);

	SetReady(TRUE);
}


//
// upgrade DayN -> DayN+1
//
void AttendanceManager::Upgrade()
{
	m_dwBaseTime			= GetTickCount();
	m_dwCurrentTime			= m_dwBaseTime;
	m_dwPrevElapsedMinutes	= 0;
	
	//SetN(m_N + 1);

	
	//
	// RESET by Squirrel
	//
    if ( !m_pCharOwner )
    {
        sc::writeLogError( std::string("AttendanceManager::Upgrade - m_pCharOwner is null.") );

        return;
    }

	sc::writeLogDebug(sc::string::format("SetUpDayN begins for \"%1%\"(%2%)", m_pCharOwner->m_szName, m_pCharOwner->CharDbNum()));
	DayN dayn = m_pScriptMan->SetDayN(m_N/* 0-base in Squirrel*/, (UINT)m_pCharOwner->m_wLevel, m_pCharOwner->GetMaxLevel());
	
	*((DayN *)this) = dayn;

	// Compute RewardExp based on RewardExpRate
	ComputeRewardExp(m_pCharOwner->GET_LEVELUP_EXP());

	GLMSG::SNET_ATTENDANCE_UPGRADE_FAC msg;	
	msg.SetData(GetN(), m_arrTasks, m_nTasks);	

	//m_pCharOwner->SendToClient(&msg);
	msg.nChaNum = m_pCharOwner->CharDbNum();
	m_pCharOwner->SendToAgent(&msg);

	SetReady(TRUE);
}


//
// reset DayN -> DayN
//
void AttendanceManager::Sync()
{
	m_dwBaseTime			= GetTickCount();
	m_dwCurrentTime			= m_dwBaseTime;
	m_dwPrevElapsedMinutes	= 0;

	//
	//send a Sync msg based on the current Attendance status loaded by DB.
	//
    if ( !m_pCharOwner )
    {
        sc::writeLogError( std::string("AttendanceManager::Sync - m_pCharOwner is null.") );

        return;
    }

	GLMSG::SNET_ATTENDANCE_SYNC_FAC msg;
	msg.SetData(GetN(), m_arrTasks, m_nTasks);

	//m_pCharOwner->SendToClient(&msg);
	msg.nChaNum = m_pCharOwner->CharDbNum();
	m_pCharOwner->SendToAgent(&msg);

	SetReady(TRUE);
}


//
// wrapping method for task-update is only called by several attendance-check methods.
//
void AttendanceManager::UpdateAttendance(ENUM_ATTENDANCE_TYPE emType, BOOL bSuccess, UINT nValue)
{
	if(!GLUseFeatures::GetInstance().IsUsingAttendance())
	{
		//sc::writeLogWarn(std::string("UpdateAttendance() bypass. Attendance is turned OFF!"));
		return;
	}

	if( !GetReady() )
	{
		return;
	}

	if (emType == ATTENDANCE_TYPE_CONNECTION)
	{
		UpdateTimeTask();
	}
	else
	{
		UpdateNonTimeTask(emType, bSuccess, nValue);
	}
}



/*
						  calls

							|
							|
							|
							v

			   one of the following two methods
			(UpdateTimeTask or UpdateNonTimeTask)
*/

//
// core method for task-update is only called by several attendance-check methods.
//
void AttendanceManager::UpdateNonTimeTask(ENUM_ATTENDANCE_TYPE emType, BOOL bSuccess, UINT nValue)
{
	if (m_status == ATTENDANCE_STATUS_INPROGRESS)	//Mandatory Task MUST be completed first.
		return;
	
	for(UINT i = 1; i<m_nTasks; i++)
	{
		if ( (bSuccess == FALSE) && (emType == ATTENDANCE_TYPE_ENCHANTAC || emType == ATTENDANCE_TYPE_ENCHANTWC))
		{
			ResetTask(i, emType);
		}		
		else
		{
			UpdateTask(i, emType, nValue);
		}
	}
}


//
// core method for time-update is only called by UpdateAttendance().
//
void AttendanceManager::UpdateTimeTask()
{
	m_dwCurrentTime = GetTickCount();

	DWORD dwElapsedMinutes = (m_dwCurrentTime - m_dwBaseTime) / 60000;	//60 sec
	
	if ( (dwElapsedMinutes > 0) && (m_dwPrevElapsedMinutes != dwElapsedMinutes) )
	{
		m_dwPrevElapsedMinutes = dwElapsedMinutes;

		if (m_status == ATTENDANCE_STATUS_INPROGRESS)
		{
			//m_arrTasks[0].Update(ATTENDANCE_TYPE_CONNECTION);			//Mandatory Task
			UpdateTask(0, ATTENDANCE_TYPE_CONNECTION);
		}
		else
		{
			for(UINT i = 1; i<m_nTasks; i++)			//Optional 3 Tasks
			{
				//m_arrTasks[i].Update(ATTENDANCE_TYPE_CONNECTION);
				UpdateTask(i, ATTENDANCE_TYPE_CONNECTION);
			}
		}
	}
}
/*
			UpdateTimeTask and UpdateNonTimeTask

						  calls

							|
							|
							|
							v

						UpdateTask
							or
						ResetTask (only by UpdateNonTimeTask)
*/

//
// wrapping method to call TaskN::Update(emType) for the AttendanceManager.
//
BOOL AttendanceManager::UpdateTask(UINT idx, ENUM_ATTENDANCE_TYPE emType, UINT nValue)
{
    if (idx >= MAX_TASKS_PER_ONE_DAY)
        return FALSE;

	BOOL bUpdated = FALSE;

	bUpdated = m_arrTasks[idx].Update(emType, nValue);

	if(bUpdated)
	{
        if ( !m_pCharOwner )
        {
            sc::writeLogError( std::string("AttendanceManager::UpdateTask - m_pCharOwner is null.") );

            return FALSE;
        }

		GLMSG::SNET_ATTENDANCE_TASK_UPDATE_FC msgUpdate;
		msgUpdate.idx		= idx;
		msgUpdate.nValue	= nValue;

		m_pCharOwner->SendToClient(&msgUpdate);

		if( m_arrTasks[idx].IsCompleted() )
		{
			//
			// Send Reward for the Task and do progress for the next step of the attendance.
			//
			CompletedTask(idx);
		}
	}

	return bUpdated;
}

//
// reset the task in case of failure of ENCHANTWC & ENCHANTAC
//
BOOL AttendanceManager::ResetTask(UINT idx, ENUM_ATTENDANCE_TYPE emType)
{
    if (idx >= MAX_TASKS_PER_ONE_DAY)
        return FALSE;

	BOOL bReset = FALSE;

	bReset = m_arrTasks[idx].Reset(emType);

	if(bReset)
	{
        if ( !m_pCharOwner )
        {
            sc::writeLogError( std::string("AttendanceManager::ResetTask - m_pCharOwner is null.") );

            return FALSE;
        }

		GLMSG::SNET_ATTENDANCE_TASK_RESET_FC msgReset;
		msgReset.idx = idx;

		m_pCharOwner->SendToClient(&msgReset);
	}

	return bReset;
}

/*
						UpdateTask

						  calls

							|
							|
							|
							v

						CompletedTask
*/

void AttendanceManager::CompletedTask(UINT idx)
{
	if(!GLUseFeatures::GetInstance().IsUsingAttendance())
	{
		sc::writeLogError(std::string("CompletedTask() - Wrong use of Attendance while it is OFF!"));
		return;
	}

    if ( !m_pCharOwner )
    {
        sc::writeLogError( std::string("AttendanceManager::CompletedTask - m_pCharOwner is null.") );

        return;
    }

	GLMSG::SNET_ATTENDANCE_TASK_COMPLETED_FC msg;
	msg.idx = idx;
	
	m_pCharOwner->SendToClient(&msg);


	if (idx == 0)	//DayN attendance has been completed
	{
		//
		// msg for PopUp
		//
		GLMSG::SNET_ATTENDANCE_POPUP_COMPLETE_FC msgPopUp;	
		msgPopUp.dwGaeaID		= m_pCharOwner->GetGaeaID();

		m_pCharOwner->SendMsgViewAround(&msgPopUp);
	}
}

BOOL AttendanceManager::TakeReward(UINT idx)
{
	if(!GLUseFeatures::GetInstance().IsUsingAttendance())
	{
		sc::writeLogError(std::string("TakeReward() - Wrong use of Attendance while it is OFF!"));
		return FALSE;
	}

    if ( !m_pCharOwner )
    {
        sc::writeLogError( std::string("AttendanceManager::TakeReward - m_pCharOwner is null.") );

        return FALSE;
    }

	if( g_bAttendanceDailyMaintenance )	// 점검중에 접속한 것이라면 진행하지 말라
	{
		sc::writeLogError(sc::string::format("AttendanceManager::TakeReward() - Request from character(%1%) during maintenance !!", m_pCharOwner->GetCharID()));
		return FALSE;
	}

    if (idx >= MAX_TASKS_PER_ONE_DAY)
    {
        sc::writeLogError(sc::string::format("TakeReward index overflow. %1%", idx));
        return FALSE;
    }

	if (m_arrTasks[idx].IsTookRewards())
	{
		sc::writeLogError(sc::string::format("AttendanceManager::TakeReward() - Weird TakeReward request from character(%1%)! The reward was already taken !!", m_pCharOwner->GetCharID()));
		return FALSE;
	}

	GLMSG::SNET_ATTENDANCE_TAKEREWARD_FB msg;
	msg.idxTask = idx;

	if( !m_arrTasks[idx].IsCompleted() )
	{
		msg.bTook = FALSE;
		
		goto _takereward_response;
	}

	msg.bTook = TRUE;


	//
	//process for reward-items
	//
	ProcessRewardItemsAndMoney(idx);


	//
	//process for reward-point
	//
	ProcessRewardPoints(idx);


	//
	//process for reward-exps
	//
	ProcessRewardExps(idx);

	m_arrTasks[idx].TookRewards(TRUE);


_takereward_response:
	m_pCharOwner->SendToClient(&msg);


	return msg.bTook;
}

/*
						  calls

							|
							|
							|
							v

					Reward Procedures
*/

void AttendanceManager::ProcessRewardPoints(UINT idx)
{
    if ( !m_pCharOwner )
    {
        sc::writeLogError( std::string("AttendanceManager::ProcessRewardPoints - m_pCharOwner is null.") );

        return;
    }

	m_pCharOwner->ReceiveLivingPoint(GetTasks()[idx].GetRewardPoint());
}


void AttendanceManager::ProcessRewardItemsAndMoney(UINT idx)
{
    if ( !m_pCharOwner )
    {
        sc::writeLogError( std::string("AttendanceManager::ProcessRewardItemsAndMoney - m_pCharOwner is null.") );

        return;
    }

	DWORD	dwRewardItemID	= SNATIVEID(false).dwID;
	UINT	nRewardMoney	= GetTasks()[idx].GetRewardMoney();

	int i;
	for(i=0; i<MAX_ITEMS_PER_ONE_TASK; i++)
	{
		dwRewardItemID = GetTasks()[idx].GetRewardItem(i);

		if (dwRewardItemID == SNATIVEID(false).dwID)
		{
			break;
		}
		else
		{
			if((nRewardMoney > 0) && (i > 0))
			{
				nRewardMoney = 0;
			}

			//
			//Send RewardItem(s) & RewardMoney
			//
			m_pGaeaServer->PostSendSystemMail(
				m_pCharOwner->GetCharID()
				,std::string(m_pCharOwner->m_szName)
				,dwRewardItemID
				,(LONGLONG)nRewardMoney
				,ATTENDANCE_REWARD_POST_DESIGN
				,ATTENDANCE_REWARD_POST_SENDER
				,std::string(ID2SERVERTEXT("ATTENDANCE_REWARD_POST_SENDERNAME"))
				,std::string(ID2SERVERTEXT("ATTENDANCE_REWARD_POST_TITLE"))
				,std::string(ID2SERVERTEXT("ATTENDANCE_REWARD_POST_CONTENT"))
				);
		}
	}

	if(i == 0 && nRewardMoney > 0)
	{
		//
		//Send only RewardMoney. There's no RewardItem.
		//
		m_pGaeaServer->PostSendSystemMail(
				m_pCharOwner->GetCharID()
				,std::string(m_pCharOwner->m_szName)
				,SNATIVEID(false).dwID
				,(LONGLONG)nRewardMoney
				,ATTENDANCE_REWARD_POST_DESIGN
				,ATTENDANCE_REWARD_POST_SENDER
				,std::string(ID2SERVERTEXT("ATTENDANCE_REWARD_POST_SENDERNAME"))
				,std::string(ID2SERVERTEXT("ATTENDANCE_REWARD_POST_TITLE"))
				,std::string(ID2SERVERTEXT("ATTENDANCE_REWARD_POST_CONTENT"))
				);
	}
}


void AttendanceManager::ProcessRewardExps(UINT idx)
{
    if ( !m_pCharOwner )
    {
        sc::writeLogError( std::string("AttendanceManager::ProcessRewardExps - m_pCharOwner is null.") );

        return;
    }

	UINT nRewardExp = GetTasks()[idx].GetRewardExp();//m_pCharOwner->GET_LEVELUP_EXP());

	if ( nRewardExp  > 0 )
	{
		//
		// ItemEdit에서 입력한 경험치 배율 적용
		//
		// nRewardExp = (UINT)( (float)( nRewardExp * m_pCharOwner->GetExpMultiple() ) );
		// RM# 1874 [9/11/2015 gbgim];
		nRewardExp = static_cast<UINT>(nRewardExp);

		//
		//Exp will be updated in next FrameMove and the ExpUpdate message will be sent to the client.
		//
		m_pCharOwner->ReceiveExp(nRewardExp, false, false);
		
		//m_pCharOwner->m_sExperience.lnNow += nRewardExp;
		//m_pCharOwner->m_sExperience.LIMIT();
	}
}
