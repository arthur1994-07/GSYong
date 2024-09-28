//
//mjeon.attendance
//
#include "../../pch.h"
#include "./OdbcStmt.h"
#include "s_COdbcManager.h"

#include "../../Attendance/AttendanceManager.h"

#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/String/StringUtil.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int COdbcManager::AttendanceLoad(OUT AttendanceManager &AttendanceMan, UINT nChaNum)
{
    sc::writeLogError("COdbcManager::AttendanceLoad. Use ADO");
    return sc::db::DB_ERROR;
/*
	SQLRETURN sReturn	= 0;	
	UINT	nRet		= 0;
	int		nResetStatus= -1;	//출석부 RESET 상태 추적을 위해 RESET이 어느 경우에 발생했는지 Log를 남긴다.

	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();
	CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();	

	if (!spConn)
	{
		sc::writeLogError(std::string("AttendanceLoad(): GameDB->GetConnection() for DayN failure."));
		
		return sc::db::DB_ERROR;
	}
	
	
	//
    // 1.Load DayN info for the character
	//
	CString csQuery;

	csQuery.Format(_T(
			"SELECT DayN, Status, StartDate FROM dbo.AttendanceInfo WHERE ChaNum = %d "
		), nChaNum);


	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)csQuery.GetString(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(csQuery.GetString());		
		Print(GetErrorString(spConn->hStmt));
		
		nRet = sc::db::DB_ERROR;
		goto _attendance_load_end;
	}


	SQLINTEGER	nDayN			= 0;	SQLINTEGER	cbDayN		= SQL_NTS;
	SQLINTEGER	nStatus			= 0;	SQLINTEGER	cbStatus	= SQL_NTS;
	SQLCHAR		StartDate[20]	={0,};	SQLINTEGER	cbStartDate = SQL_NTS;
	

	sReturn = ::SQLFetch(spConn->hStmt);

	if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		Print(csQuery.GetString());		
		Print(GetErrorString(spConn->hStmt));

		nRet = sc::db::DB_ERROR;
		goto _attendance_load_end;
	}

	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		::SQLGetData(spConn->hStmt,  1, SQL_C_LONG,	&nDayN,		0, &cbDayN);
		::SQLGetData(spConn->hStmt,  2, SQL_C_LONG,	&nStatus,	0, &cbStatus);
		::SQLGetData(spConn->hStmt,  3, SQL_C_CHAR,	StartDate,	20,&cbStartDate);

		//
		// Verifying the Date is optional.
		//  DB will be cleaned at every 6:00am by AgentServer.
		//

		//
		// StartDate <= CurrentDate < NextStartDate ?
		//
		CTime ctStartDate;

		if ( AttendanceManager::VerifyDate(CString(StartDate), ctCurrentTime, &ctStartDate) )
		{
			//OK

			AttendanceMan.SetN(nDayN);
			AttendanceMan.SetStartDate(ctStartDate);
			
			spConn = m_pGameDB->GetConnection();

			if (!spConn)
			{
				sc::writeLogError(std::string("AttendanceLoad(): GetConnection() for TaskN failure."));
				return sc::db::DB_ERROR;
			}

			
			TaskN *pTaskN = AttendanceMan.GetTasks();

			if (pTaskN == NULL)
			{
				sc::writeLogError(std::string("AttendanceLoad(): TaskN array does not exist."));
				nRet = sc::db::DB_ERROR;
				
				goto _attendance_load_end;
			}
			
			//
			//Sync TaskN
			//
			csQuery.Format(_T(
					"SELECT TaskN, Type, Status, Value, ValueBase, RewardPoint, RewardExp, "	\
							"RewardMoney, RewardItem1, RewardItem2, RewardItem3, TookRewards "	\
					"FROM AttendanceTaskInfo "													\
					"WHERE ChaNum = %d "
				), nChaNum);

			sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)csQuery.GetString(), SQL_NTS);

			if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
			{
				Print(csQuery.GetString());		
				Print(GetErrorString(spConn->hStmt));
				nRet = sc::db::DB_ERROR;

				goto _attendance_load_end;
			}

			SQLINTEGER	nTaskN			= 0;	SQLINTEGER	cbTaskN			= SQL_NTS;
			SQLINTEGER	nType			= 0;	SQLINTEGER	cbType			= SQL_NTS;
			SQLINTEGER	nStatus			= 0;	SQLINTEGER	cbStatus		= SQL_NTS;
			SQLINTEGER	nValue			= 0;	SQLINTEGER	cbValue			= SQL_NTS;
			SQLINTEGER	nValueBase		= 0;	SQLINTEGER	cbValueBase		= SQL_NTS;
			SQLINTEGER	nRewardPoint	= 0;	SQLINTEGER	cbRewardPoint	= SQL_NTS;
			SQLINTEGER	nRewardExp		= 0;	SQLINTEGER	cbRewardExp		= SQL_NTS;
			SQLINTEGER	nRewardMoney	= 0;	SQLINTEGER	cbRewardMoney	= SQL_NTS;
			SQLINTEGER	nRewardItem1	= 0;	SQLINTEGER	cbRewardItem1	= SQL_NTS;
			SQLINTEGER	nRewardItem2	= 0;	SQLINTEGER	cbRewardItem2	= SQL_NTS;
			SQLINTEGER	nRewardItem3	= 0;	SQLINTEGER	cbRewardItem3	= SQL_NTS;
			SQLINTEGER	nTookRewards	= 0;	SQLINTEGER	cbTookRewards	= SQL_NTS;


			UINT nTasks = 0;	// number of tasks loaded
			SNATIVEID items[MAX_ITEMS_PER_ONE_TASK];


			while (TRUE)
			{
				sReturn = ::SQLFetch(spConn->hStmt);

				if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
				{
					Print(csQuery.GetString());
					Print(GetErrorString(spConn->hStmt));
					nRet = sc::db::DB_ERROR;

					goto _attendance_load_end;
				}

				if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
				{
					::SQLGetData(spConn->hStmt,  1, SQL_C_LONG,	&nTaskN,		0, &cbTaskN);

					if (nTaskN > MAX_TASKS_PER_ONE_DAY)
					{
						sc::writeLogError(sc::string::format("AttendanceLoad(): wrong TaskN(%1).", nTaskN));
					}

					::SQLGetData(spConn->hStmt,  2, SQL_C_LONG,	&nType,			0, &cbType);
					::SQLGetData(spConn->hStmt,  3, SQL_C_LONG,	&nStatus,		0, &cbStatus);
					::SQLGetData(spConn->hStmt,  4, SQL_C_LONG,	&nValue,		0, &cbValue);
					::SQLGetData(spConn->hStmt,  5, SQL_C_LONG,	&nValueBase,	0, &cbValueBase);
					::SQLGetData(spConn->hStmt,  6, SQL_C_LONG,	&nRewardPoint,	0, &cbRewardPoint);
					::SQLGetData(spConn->hStmt,  7, SQL_C_LONG,	&nRewardExp,	0, &cbRewardExp);
					::SQLGetData(spConn->hStmt,  8, SQL_C_LONG,	&nRewardMoney,	0, &cbRewardMoney);
					::SQLGetData(spConn->hStmt,  9, SQL_C_LONG,	&nRewardItem1,	0, &cbRewardItem1);
					::SQLGetData(spConn->hStmt, 10, SQL_C_LONG,	&nRewardItem2,	0, &cbRewardItem2);
					::SQLGetData(spConn->hStmt, 11, SQL_C_LONG,	&nRewardItem3,	0, &cbRewardItem3);
					::SQLGetData(spConn->hStmt, 12, SQL_C_LONG,	&nTookRewards,	0, &cbTookRewards);

					
					pTaskN[nTaskN].SetCondtion(nTaskN, nType, nValueBase);
					pTaskN[nTaskN].SetStatus((ENUM_ATTENDANCE_STATUS)nStatus);
					pTaskN[nTaskN].SetValue(nValue);
					
					pTaskN[nTaskN].SetRewardPoint(nRewardPoint);
					pTaskN[nTaskN].SetRewardExp(nRewardExp);
					pTaskN[nTaskN].SetRewardMoney(nRewardMoney);
					pTaskN[nTaskN].TookRewards(nTookRewards > 0);

					
					items[0] = SNATIVEID((DWORD)nRewardItem1);
					items[1] = SNATIVEID((DWORD)nRewardItem2);
					items[2] = SNATIVEID((DWORD)nRewardItem3);

					for (int i=0; i<MAX_ITEMS_PER_ONE_TASK; i++)
					{
						pTaskN[nTaskN].SetRewardItem(i, items[i].Mid(), items[i].Sid());
					}

					nTasks++;

					continue;
				}
				else
				{
					//There's no more data.					

					if (nTasks == 0)
					{
						//
						//This is an upgraded Attendance.
						// start next DayN
						//
						nRet = ATTENDANCE_UPDATE_UPGRADE;
						goto _attendance_load_end;
					}
					break;
				}
			}

			//
			// Sync the DayN and its Tasks to the Client
			//
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
	}
	else
	{
		//
		//This is an expired Attendance.
		// start new attendance from Day1
		//		
		if(nDayN == 0)
		{
			nResetStatus = 1;

			nRet = ATTENDANCE_UPDATE_RESET;
			goto _attendance_load_end;
		}
	}

_attendance_load_end:

	//
	// Log Attendance (RESET과 UPGRADE에 대해서만)
	//
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

		csQuery.Format(_T( "{call sp_AttendanceLog(%d, %d, '%s', %d, %d)}" )
			, nChaNum, 0, strStatus.c_str()
				, (int)AttendanceMan.GetN(), (int)AttendanceMan.GetStatus());

		m_pLogDB->ExecuteSpInt(csQuery.GetString());
	}
	
	return nRet;
*/
}


int COdbcManager::AttendanceSave(IN AttendanceManager &AttendanceMan, UINT nChaNum, LONGLONG nChaCurExp)
{
    sc::writeLogError("COdbcManager::AttendanceSave. Use ADO");
    return sc::db::DB_ERROR;
/*
	UINT	nRet = 0;	

	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();
	CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");

	//
	// in case of Save that is  called by DailyMaintenance, pChar->AttendanceSetReady(FALSE) is called first.
	//
	BOOL	bOnMaintenance = !AttendanceMan.GetReady();
	
	
	//
    // 1.Save DayN info for the character
	//
	CString csQuery;


	csQuery.Format(_T( "{call sp_AttendanceSave(%d, %d, %d, '%s', ?)}" )
				,nChaNum, AttendanceMan.GetN(), AttendanceMan.GetStatus(), AttendanceMan.GetStartDate().Format(_T("%Y-%m-%d %H:%M:%S"))	);

	nRet = m_pGameDB->ExecuteSpInt(csQuery.GetString());

	if (nRet == sc::db::DB_ERROR)
	{
		sc::writeLogError(std::string("AttendanceSave(): sp_AttendanceSave returned DB_ERROR."));
		
		return nRet;
	}


	//
	// Log DayN
	//
	//if (bOnMaintenance)
	{
		csQuery.Format(_T( "{call sp_AttendanceLog(%d, %d, '%s', %d, %d)}" )
				, nChaNum, 1, "SAVE"
				, AttendanceMan.GetN(), AttendanceMan.GetStatus());

		m_pLogDB->ExecuteSpInt(csQuery.GetString());
	}


	UINT nTasks = AttendanceMan.GetNumOfTasks();

	TaskN *pTaskN = AttendanceMan.GetTasks();

	if (pTaskN == NULL)
	{
		sc::writeLogError(std::string("AttendanceSave(): TaskN array does not exist."));
		
		return sc::db::DB_ERROR;
	}


	DWORD dwItemIDs[MAX_ITEMS_PER_ONE_TASK] = { SNATIVEID(false).dwID, SNATIVEID(false).dwID, SNATIVEID(false).dwID };

	for (UINT iTask=0; iTask<nTasks; iTask++)
	{
		for (UINT iItem=0; iItem<MAX_ITEMS_PER_ONE_TASK; iItem++)
		{
			dwItemIDs[iItem] = pTaskN[iTask].GetRewardItem(iItem);
		}

		csQuery.Format(_T( "{call sp_AttendanceTaskSave(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, ?)}" )
				,nChaNum, iTask, pTaskN[iTask].GetType(), (int)pTaskN[iTask].GetStatus(), pTaskN[iTask].GetValue()
				,pTaskN[iTask].GetValueBase(), pTaskN[iTask].GetRewardPoint(), pTaskN[iTask].GetRewardExp(), pTaskN[iTask].GetRewardMoney(), pTaskN[iTask].GetNumberOfRewardItems()
				,dwItemIDs[0], dwItemIDs[1], dwItemIDs[2], pTaskN[iTask].IsTookRewards() );

		nRet = m_pGameDB->ExecuteSpInt(csQuery.GetString());

		if (nRet == sc::db::DB_ERROR)
		{
			sc::writeLogError(std::string("AttendanceSave(): sp_AttendanceTaskSave returned DB_ERROR."));
			
			return nRet;
		}

		//
		// Log Tasks
		//
		//if (bOnMaintenance)	//출석부가 안정화될 때까지는 Task로그를 남기도록 하자.
		{
			csQuery.Format(_T( "{call sp_AttendanceTaskLog(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, ?)}" )
				,nChaNum, iTask, pTaskN[iTask].GetType(), (int)pTaskN[iTask].GetStatus(), pTaskN[iTask].GetValue()
				,pTaskN[iTask].GetValueBase(), pTaskN[iTask].GetRewardPoint(), pTaskN[iTask].GetRewardExp(), pTaskN[iTask].GetRewardMoney(), pTaskN[iTask].GetNumberOfRewardItems()
				,dwItemIDs[0], dwItemIDs[1], dwItemIDs[2], pTaskN[iTask].IsTookRewards() );

			m_pLogDB->ExecuteSpInt(csQuery.GetString());
		}
	}

	return sc::db::DB_OK;
*/
}


int COdbcManager::AttendanceMaintenance()
{
    sc::writeLogError("COdbcManager::AttendanceMaintenance. Use ADO");
    return sc::db::DB_ERROR;
/*
	UINT	nRet = 0;	

	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();
	CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");	//manual 점검이 아니라면, 이 부분은 항상 새벽 6:00g혹은 그보다 조금 더 지난 시간일 것이다.
		
	CString csQuery;

	csQuery.Format(_T( "{call sp_AttendanceMaintenance('%s', ?)}" ), csCurrentTime.GetString());


	nRet = m_pGameDB->ExecuteSpInt(csQuery.GetString());

	if (nRet == -1) //Not a DB_ERROR. This is a just returned status-code.
	{
		sc::writeLogInfo(std::string("AttendanceMaintenance(): No Upgrade of DayN."));
		
		return nRet;
	}
	else if (nRet == 1)
	{
		sc::writeLogInfo(std::string("AttendanceMaintenance(): The maintenance was already done today. It can be done once per a day."));
		
		return nRet;
	}

	return nRet;
*/
}

int COdbcManager::AttendanceLogInsert(int ChaNum, int Type, const std::string& LoadType, int DayN, int Status)
{
    sc::writeLogError("COdbcManager::AttendanceLogInsert. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::AttendanceTaskInsert(
    int ChaNum, LONGLONG ChaCurExp, int TaskN, int Type, int Status, int Value,
    int ValueBase, int RewardPoint, int RewardExp, int RewardMoney, int RewardItems,
    int RewardItem1, int RewardItem2, int RewardItem3, int TookRewards)
{
    sc::writeLogError("COdbcManager::AttendanceTaskInsert. Use ADO");
    return sc::db::DB_ERROR;
}