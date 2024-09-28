#include "../../pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../SigmaCore/Util/DateTime.h"
#include "../../../RanLogic/Attendance/TaskN.h"
#include "../../Server/NetServer.h"
#include "../../Net/NetLogDefine.h"
#include "../../Attendance/AttendanceManager.h"
#include "./AdoManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

int AdoManager::AttendanceLoad(OUT AttendanceManager &AttendanceMan, UINT nChaNum)
{
    int sReturn = 0;
    UINT nRet = 0;
    int nResetStatus= -1; //출석부 RESET 상태 추적을 위해 RESET이 어느 경우에 발생했는지 Log를 남긴다.

    CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();
    CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");

    _variant_t StartDate;
    // StartDate <= CurrentDate < NextStartDate ?
    CTime ctStartDate;

    // 1.Load DayN info for the character
    sc::db::AdoExt Ado1(m_GameDBConnString);
    APPEND_IPARAM_INT(Ado1, "@ChaNum", nChaNum);
    
    if (!Ado1.Execute4Cmd("dbo.AttendanceInfoSelect", adCmdStoredProc))        
    {        
        nRet = sc::db::DB_ERROR;
        goto _attendance_load_end;
    }

    int nDayN = 0;

    if (Ado1.GetEOF())
    {
        // This is an expired Attendance.
        // start new attendance from Day1        
        nResetStatus = 1;

        nRet = ATTENDANCE_UPDATE_RESET;
        goto _attendance_load_end;
    }
    
    int nStatus = 0;
    
    Ado1.GetCollect("DayN", nDayN);
    Ado1.GetCollect("Status", nStatus);
    Ado1.GetCollect("StartDate", StartDate);
    
    // Verifying the Date is optional.
    // DB will be cleaned at every 6:00am by AgentServer.
    __time64_t tStartDate = sc::time::GetTime(StartDate);

    if (AttendanceManager::VerifyDate(tStartDate, ctCurrentTime, &ctStartDate) )
    {
        //OK
        AttendanceMan.SetN(nDayN);
        AttendanceMan.SetStartDate(ctStartDate);            

        TaskN *pTaskN = AttendanceMan.GetTasks();

        if (!pTaskN)
        {
            sc::writeLogError(std::string("AttendanceLoad(): TaskN array does not exist."));
            nRet = sc::db::DB_ERROR;
            goto _attendance_load_end;
        }

        // Sync TaskN
        sc::db::AdoExt Ado2(m_GameDBConnString);
        APPEND_IPARAM_INT(Ado2, "@ChaNum", nChaNum);
        if (!Ado2.Execute4Cmd("dbo.AttendanceTaskInfoSelect", adCmdStoredProc))            
        {
            nRet = sc::db::DB_ERROR;
            goto _attendance_load_end;
        }

        UINT nTasks = 0;	// number of tasks loaded

        if (Ado2.GetEOF())
        {
            // This is an upgraded Attendance.
            // start next DayN                
            nRet = ATTENDANCE_UPDATE_UPGRADE;
            goto _attendance_load_end;
        }
        
        do
        {
            /*
            TaskN, Type, Status, Value, ValueBase,
            RewardPoint, RewardExp, RewardMoney, RewardItem1, RewardItem2,
            RewardItem3, TookRewards
            */
            int nTaskN       = 0;
            int nType        = 0;
            int nStatus      = 0;
            int nValue       = 0;
            int nValueBase   = 0;
            int nRewardPoint = 0;
            int nRewardExp   = 0;
            int nRewardMoney = 0;
            int nRewardItem1 = 0;
            int nRewardItem2 = 0;
            int nRewardItem3 = 0;
            int nTookRewards = 0;
            SNATIVEID items[MAX_ITEMS_PER_ONE_TASK];

            Ado2.GetCollect("TaskN", nTaskN);
            if (nTaskN >= MAX_TASKS_PER_ONE_DAY)
            {
                sc::writeLogError(sc::string::format("AttendanceLoad(): wrong TaskN(%1).", nTaskN));
                break;
            }
            Ado2.GetCollect("Type", nType);
            Ado2.GetCollect("Status", nStatus);
            Ado2.GetCollect("Value", nValue);
            Ado2.GetCollect("ValueBase", nValueBase);
            Ado2.GetCollect("RewardPoint", nRewardPoint);
            Ado2.GetCollect("RewardExp", nRewardExp);
            Ado2.GetCollect("RewardMoney", nRewardMoney);
            Ado2.GetCollect("RewardItem1", nRewardItem1);
            Ado2.GetCollect("RewardItem2", nRewardItem2);
            Ado2.GetCollect("RewardItem3", nRewardItem3);
            Ado2.GetCollect("TookRewards", nTookRewards);

            pTaskN[nTaskN].SetCondtion(nTaskN, nType, nValueBase);
            pTaskN[nTaskN].SetStatus((ENUM_ATTENDANCE_STATUS)nStatus);
            pTaskN[nTaskN].SetValue(nValue);

            pTaskN[nTaskN].SetRewardPoint(nRewardPoint);
            pTaskN[nTaskN].SetRewardExp(nRewardExp);
            pTaskN[nTaskN].SetRewardMoney(nRewardMoney);
            pTaskN[nTaskN].TookRewards(nTookRewards > 0);

            items[0] = SNATIVEID((DWORD) nRewardItem1);
            items[1] = SNATIVEID((DWORD) nRewardItem2);
            items[2] = SNATIVEID((DWORD) nRewardItem3);

            for (int i=0; i<MAX_ITEMS_PER_ONE_TASK; ++i)
            {
                pTaskN[nTaskN].SetRewardItem(i, items[i].Mid(), items[i].Sid());
            }

            nTasks++;
        } while (Ado2.Next());

        // Sync the DayN and its Tasks to the Client                
        AttendanceMan.SetNumOfTasks(nTasks);

        nRet = ATTENDANCE_UPDATE_SYNC;
        goto _attendance_load_end;
    }
    else
    {
        //
        // if tThis is an expired Attendance.
        // start new attendance from Day1
        //
        nResetStatus = 0;

        nRet = ATTENDANCE_UPDATE_RESET;
        goto _attendance_load_end;
    }    

_attendance_load_end:

    // Log Attendance (RESET과 UPGRADE에 대해서만)
    if (nRet == ATTENDANCE_UPDATE_RESET || nRet == ATTENDANCE_UPDATE_UPGRADE)
    {
        std::string strStatus = "UPGRADE";

        if (nRet == ATTENDANCE_UPDATE_RESET)
        {
            if (nResetStatus == 0)
            {
                strStatus = "RESET-0";
            }
            else if (nResetStatus == 1)
            {
                strStatus = "RESET-1";
            }
            else
            {
                strStatus = "RESET-X";	//WRONG!!! NEVER GET TO HERE!!
            }
        }

        if (m_pServer)
        {
            NET_LOG_ATTENDANCE DataDS(
                nChaNum,
                0, // Load
                strStatus,
                AttendanceMan.GetN(),
                AttendanceMan.GetStatus());
            msgpack::sbuffer BufferDS;
            msgpack::pack(BufferDS, DataDS);
            NET_MSG_PACK MsgDS;
            MsgDS.SetData(NET_LOG_ATTENDANCE_DS, BufferDS);
            m_pServer->InsertMsg(DBACTION_CLIENT_ID, &MsgDS);
        }
    }

    return nRet;
}

int AdoManager::AttendanceSave(IN  AttendanceManager &AttendanceMan, UINT nChaNum, LONGLONG nChaCurExp)
{
    UINT nRet = 0;

    CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();
    CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");
    
    // in case of Save that is  called by DailyMaintenance, pChar->AttendanceSetReady(FALSE) is called first.
    BOOL bOnMaintenance = !AttendanceMan.GetReady();

	UINT nTasks = AttendanceMan.GetNumOfTasks();

    if ( nTasks != 4 )
    {
		sc::writeLogError(sc::string::format("AttendanceSave(): Task count is not match ( nTasks : %1%)", nTasks));
        return sc::db::DB_ERROR;
    }
    
    // 1.Save DayN info for the character
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);
    
    APPEND_IPARAM_INT(AdoGame, "@ChaNum", nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@DayN", AttendanceMan.GetN());
    APPEND_IPARAM_INT(AdoGame, "@Status", AttendanceMan.GetStatus());
    CString StartDate = AttendanceMan.GetStartDate().Format(_T("%Y-%m-%d %H:%M:%S"));
    APPEND_IPARAM_VARCHAR(AdoGame, "@StartDate", StartDate.GetString(), StartDate.GetLength());

    APPEND_OPARAM_INT(AdoGame, "@nReturn");
    nRet = ExecuteStoredProcedureIntReturn("dbo.sp_AttendanceSave", AdoGame, "@nReturn");
    if (nRet == sc::db::DB_ERROR)
    {
        sc::writeLogError(std::string("AttendanceSave(): sp_AttendanceSave returned DB_ERROR."));
        return nRet;
    }

    // Log DayN
    //if (bOnMaintenance)
    {
        if (m_pServer)
        {
            std::string StatusStr("SAVE");
            NET_LOG_ATTENDANCE DataDS(
                nChaNum,
                1, // Save
                StatusStr,
                AttendanceMan.GetN(),
                AttendanceMan.GetStatus());
            msgpack::sbuffer BufferDS;
            msgpack::pack(BufferDS, DataDS);
            NET_MSG_PACK MsgDS;
            MsgDS.SetData(NET_LOG_ATTENDANCE_DS, BufferDS);
            m_pServer->InsertMsg(DBACTION_CLIENT_ID, &MsgDS);
        }        
    }

   

    TaskN* pTaskN = AttendanceMan.GetTasks();

    if (!pTaskN)
    {
		sc::writeLogError(std::string("AttendanceSave(): TaskN array does not exist "));
        return sc::db::DB_ERROR;
    }

    DWORD dwItemIDs[MAX_ITEMS_PER_ONE_TASK] = { SNATIVEID(false).dwID, SNATIVEID(false).dwID, SNATIVEID(false).dwID };

    for (UINT iTask=0; iTask<nTasks; ++iTask)
    {
        for (UINT iItem=0; iItem<MAX_ITEMS_PER_ONE_TASK; ++iItem)
        {
            dwItemIDs[iItem] = pTaskN[iTask].GetRewardItem(iItem);
        }

        sc::db::AdoExt AdoGame2(m_GameDBConnString);
        APPEND_RPARAM_INT(AdoGame2);

        APPEND_IPARAM_INT(AdoGame2, "@ChaNum",       nChaNum);
        APPEND_IPARAM_INT(AdoGame2, "@TaskN",        iTask);
        APPEND_IPARAM_INT(AdoGame2, "@Type",         pTaskN[iTask].GetType());
        APPEND_IPARAM_INT(AdoGame2, "@Status", (int) pTaskN[iTask].GetStatus());
        APPEND_IPARAM_INT(AdoGame2, "@Value",        pTaskN[iTask].GetValue());
        APPEND_IPARAM_INT(AdoGame2, "@ValueBase",    pTaskN[iTask].GetValueBase());
        APPEND_IPARAM_INT(AdoGame2, "@RewardPoint",  pTaskN[iTask].GetRewardPoint());
        APPEND_IPARAM_INT(AdoGame2, "@RewardExp",    pTaskN[iTask].GetRewardExp());
        APPEND_IPARAM_INT(AdoGame2, "@RewardMoney",  pTaskN[iTask].GetRewardMoney());
        APPEND_IPARAM_INT(AdoGame2, "@nRewardItems", pTaskN[iTask].GetNumberOfRewardItems());
        APPEND_IPARAM_INT(AdoGame2, "@RewardItem1",  dwItemIDs[0]);
        APPEND_IPARAM_INT(AdoGame2, "@RewardItem2",  dwItemIDs[1]);
        APPEND_IPARAM_INT(AdoGame2, "@RewardItem3",  dwItemIDs[2]);
        APPEND_IPARAM_INT(AdoGame2, "@TookRewards",  pTaskN[iTask].IsTookRewards());
        
        APPEND_OPARAM_INT(AdoGame2, "@nReturn");

        nRet = ExecuteStoredProcedureIntReturn("dbo.sp_AttendanceTaskSave", AdoGame2, "@nReturn");
        if (nRet == sc::db::DB_ERROR)
        {
            sc::writeLogError(std::string("AttendanceSave(): sp_AttendanceTaskSave returned DB_ERROR."));
            return nRet;
        }

        // Log Tasks
        //if (bOnMaintenance) // 출석부가 안정화될 때까지는 Task로그를 남기도록 하자.
        {
            if (m_pServer)
            {
                NET_LOG_ATTENDANCE_TASK Data(
                    nChaNum,
					nChaCurExp,
                    iTask,
                    pTaskN[iTask].GetType(),
                    (int) pTaskN[iTask].GetStatus(),
                    pTaskN[iTask].GetValue(),
                    pTaskN[iTask].GetValueBase(),
                    pTaskN[iTask].GetRewardPoint(),
                    pTaskN[iTask].GetRewardExp(),
                    pTaskN[iTask].GetRewardMoney(),
                    pTaskN[iTask].GetNumberOfRewardItems(),
                    dwItemIDs[0],
                    dwItemIDs[1],
                    dwItemIDs[2],
                    pTaskN[iTask].IsTookRewards());
                msgpack::sbuffer BufferDS;
                msgpack::pack(BufferDS, Data);
                NET_MSG_PACK MsgDS;
                MsgDS.SetData(NET_LOG_ATTENDANCE_TASK_DS, BufferDS);
                m_pServer->InsertMsg(DBACTION_CLIENT_ID, &MsgDS);
            }
        }
    }

    return sc::db::DB_OK;
}

int AdoManager::AttendanceMaintenance()
{   
    UINT nRet = 0;
    CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();
    CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");	//manual 점검이 아니라면, 이 부분은 항상 새벽 6:00g혹은 그보다 조금 더 지난 시간일 것이다.

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_VARCHAR(AdoGame, "@CurrentDate", csCurrentTime.GetString(), csCurrentTime.GetLength());
    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    nRet = ExecuteStoredProcedureIntReturn("dbo.sp_AttendanceMaintenance", AdoGame, "@nReturn");
    if (nRet == -1) //Not a DB_ERROR. This is a just returned status-code.
    {
        sc::writeLogInfo(std::string("AttendanceMaintenance(): No Upgrade of DayN."));
        return nRet;
    }
    else if (nRet == 1)
    {
        sc::writeLogError(std::string("AttendanceMaintenance(): The maintenance was already done today. It can be done once per a day."));
        return nRet;
    }
    return nRet;
}

int AdoManager::AttendanceLogInsert(
    int ChaNum,
    int Type,
    const std::string& LoadType,
    int DayN,
    int Status)
{
    sc::db::AdoExt AdoLog(m_LogDBConnString);
    APPEND_RPARAM_INT(AdoLog);

    APPEND_IPARAM_INT    (AdoLog, "@ChaNum",   ChaNum);
    APPEND_IPARAM_INT    (AdoLog, "@Type",     Type);
    APPEND_IPARAM_VARCHAR(AdoLog, "@LoadType", LoadType.c_str(), LoadType.length());
    APPEND_IPARAM_INT    (AdoLog, "@DayN",     DayN);
    APPEND_IPARAM_INT    (AdoLog, "@Status",   Status);

    return ExecuteStoredProcedure("dbo.sp_AttendanceLog", AdoLog);
}

int AdoManager::AttendanceTaskInsert(
    int ChaNum,
	LONGLONG ChaCurExp,
    int TaskN,
    int Type,
    int Status,
    int Value,
    int ValueBase,
    int RewardPoint,
    int RewardExp,
    int RewardMoney,
    int RewardItems,
    int RewardItem1,
    int RewardItem2,
    int RewardItem3,
    int TookRewards)
{
    sc::db::AdoExt AdoLog(m_LogDBConnString);
    APPEND_RPARAM_INT(AdoLog);

    APPEND_IPARAM_INT(AdoLog, "@ChaNum",       ChaNum);
	APPEND_IPARAM_MONEY(AdoLog, "@ChaCurExp",    ChaCurExp);
    APPEND_IPARAM_INT(AdoLog, "@TaskN",        TaskN);
    APPEND_IPARAM_INT(AdoLog, "@Type",         Type);
    APPEND_IPARAM_INT(AdoLog, "@Status",       Status);
    APPEND_IPARAM_INT(AdoLog, "@Value",        Value);
    APPEND_IPARAM_INT(AdoLog, "@ValueBase",    ValueBase);
    APPEND_IPARAM_INT(AdoLog, "@RewardPoint",  RewardPoint);
    APPEND_IPARAM_INT(AdoLog, "@RewardExp",    RewardExp);
    APPEND_IPARAM_INT(AdoLog, "@RewardMoney",  RewardMoney);
    APPEND_IPARAM_INT(AdoLog, "@nRewardItems", RewardItems);
    APPEND_IPARAM_INT(AdoLog, "@RewardItem1",  RewardItem1);
    APPEND_IPARAM_INT(AdoLog, "@RewardItem2",  RewardItem2);
    APPEND_IPARAM_INT(AdoLog, "@RewardItem3",  RewardItem3);
    APPEND_IPARAM_INT(AdoLog, "@TookRewards",  TookRewards);

    APPEND_OPARAM_INT(AdoLog, "@nReturn");            

    return ExecuteStoredProcedureIntReturn("dbo.sp_AttendanceTaskLog_20150506", AdoLog, "@nReturn");
}

// ----------------------------------------------------------------------------
} // namespace db
// ----------------------------------------------------------------------------