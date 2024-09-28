//
//mjeon.attendance
//
#include "../../pch.h"
#include "DbActionGameAttendance.h"

#include <boost/smart_ptr/weak_ptr.hpp>

#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"
#include "../../FieldServer/GLGaeaServer.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


class TaskN;


CAttendanceLoad::CAttendanceLoad(UINT nChaNum, OUT std::tr1::shared_ptr<AttendanceManager> &pAttendanceMan)
    : m_nChaNum(nChaNum)
{
	m_pAttendanceMan = pAttendanceMan;
}

CAttendanceLoad::~CAttendanceLoad()
{
}

int CAttendanceLoad::Execute(NetServer* pServer)
{
	int nReturn = NET_ERROR;
	
	if (!pServer)
		return NET_ERROR;

	CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);

	if( !pFieldServer )
		return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CAttendanceLoad::Execute. Use ADO");
        return NET_ERROR;
    }

	nReturn = m_pDbManager->AttendanceLoad(*m_pAttendanceMan, m_nChaNum);

	if (nReturn == ATTENDANCE_UPDATE_SYNC)
	{
		//
		//insert the result msg into FieldS
		//
		GLMSG::SNET_ATTENDANCE_SYNC_FF msg;
		msg.dwChaNum = m_nChaNum;

		pFieldServer->InsertMsg(DBACTION_CLIENT_ID, &msg);
	}
	else if (nReturn == ATTENDANCE_UPDATE_RESET)
	{
		//
		//insert the result msg into FieldS
		//

        // Already been reseted before call this function. you can find information of reset on log db.

		/*CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);

		if (pFieldServer)
		{
			GLMSG::SNET_ATTENDANCE_RESET_FF msg;
			msg.dwChaNum = m_nChaNum;

			pFieldServer->InsertMsg(DBACTION_CLIENT_ID, &msg);
		}*/
	}
	else if (nReturn == ATTENDANCE_UPDATE_UPGRADE)
	{
		//
		//insert the result msg into FieldS
		//
		GLMSG::SNET_ATTENDANCE_UPGRADE_FF msg;
		msg.dwChaNum = m_nChaNum;

		pFieldServer->InsertMsg(DBACTION_CLIENT_ID, &msg);
	}
	else
	{
		//
		//MUST NOT come into here.
		//		
		sc::writeLogError(std::string("DbAction(CAttendanceLoad) failure. Check it out."));
	}

	GLMSG::SNET_ATTENDANCE_LOAD_END msg;
	msg.dwChaNum = m_nChaNum;

	pFieldServer->InsertMsg(DBACTION_CLIENT_ID, &msg);

	return nReturn;
}


CAttendanceSave::CAttendanceSave(
    DWORD dwClientID,
    UINT nChaNum,
	LONGLONG nChaCurExp,
    IN std::tr1::shared_ptr<AttendanceManager> &pAttendanceMan,
    BOOL bLastOne4Maintenance)
    : m_nChaNum(nChaNum)
	, m_nChaCurExp(nChaCurExp)
    , m_bLastOne4Maintenance(bLastOne4Maintenance)
{
	m_pAttendanceMan = pAttendanceMan;
	m_ClientSlot = dwClientID;
}

CAttendanceSave::~CAttendanceSave()
{
}

int CAttendanceSave::Execute(NetServer* pServer)
{
    if (!pServer)
        return NET_ERROR;

    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return NET_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer)
        return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CAttendanceSave::Execute. Use ADO");
        return NET_ERROR;
    }

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->AttendanceSave(*m_pAttendanceMan, m_nChaNum, m_nChaCurExp);
	if (nReturn == sc::db::DB_OK)
	{
		;
	}
	else
	{
		sc::writeLogError(std::string("DbAction(CAttendanceSave) failed to call AttendanceSave()."));
	}

	//
	// in case of the last one for AttendanceMaintenance, send a message to AgentS to notify all characters finished save().
	//
	if (m_bLastOne4Maintenance)
	{
		GLMSG::SNET_ATTENDANCE_DAILYMAINTENANCE_READY_FA msgReady;
		pGaeaServer->SENDTOAGENT(&msgReady);
	}
	return nReturn;
}

CAttendanceMaintenance::CAttendanceMaintenance()
{
}


int CAttendanceMaintenance::Execute(NetServer* pServer)
{
	int nReturn = NET_ERROR;
	
	if (!pServer)
		return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CAttendanceMaintenance::Execute. Use ADO");
        return NET_ERROR;
    }

	nReturn = m_pDbManager->AttendanceMaintenance();

	//if(nReturn != 1)	//already done today
	{
		sc::writeLogInfo(std::string("Attendance Daily Maintenance DONE."));

		CAgentServer* pAgentS = reinterpret_cast<CAgentServer*> (pServer);		
		if (!pAgentS)
		{
			sc::writeLogError(std::string("DbAction(CAttendanceMaintenance): pAgentServer is NULL. So could not send DAILYMAINTENANCE_END_AF to all FieldServers."));
			return sc::db::DB_ERROR;
		}
	
		GLMSG::SNET_ATTENDANCE_DAILYMAINTENANCE_END_AF msg;
		pAgentS->SendAllChannel(&msg);

		pAgentS->Ready4AttendanceMaintenance(TRUE);	//now AgentS will handle manual-AttendanceMaintenance again.
	}

	return nReturn;
}

AttendanceLogInsert::AttendanceLogInsert(
    int ChaNum,
    int Type,
    const std::string& LoadType,
    int DayN,
    int Status)
    : m_ChaNum(ChaNum)
    , m_Type(Type)
    , m_LoadType(LoadType)
    , m_DayN(DayN)
    , m_Status(Status)
{
}
    
AttendanceLogInsert::~AttendanceLogInsert()
{
}
    
int AttendanceLogInsert::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("AttendanceLogInsert::Execute. Use ADO");
        return NET_ERROR;
    }

    return m_pDbManager->AttendanceLogInsert(
        m_ChaNum,
        m_Type,
        m_LoadType,
        m_DayN,
        m_Status);
}

AttendanceTaskLogInsert::AttendanceTaskLogInsert(
    int ChaNum, LONGLONG ChaCurExp, int TaskN, int Type, int Status, int Value,
    int ValueBase, int RewardPoint, int RewardExp, int RewardMoney, int RewardItems, 
    int RewardItem1, int RewardItem2, int RewardItem3, int TookRewards)
    : m_ChaNum(ChaNum)
	, m_ChaCurExp(ChaCurExp)
    , m_TaskN(TaskN)
    , m_Type(Type)
    , m_Status(Status)
    , m_Value(Value)
    , m_ValueBase(ValueBase)
    , m_RewardPoint(RewardPoint)
    , m_RewardExp(RewardExp)
    , m_RewardMoney(RewardMoney)
    , m_RewardItems(RewardItems)
    , m_RewardItem1(RewardItem1)
    , m_RewardItem2(RewardItem2)
    , m_RewardItem3(RewardItem3)
    , m_TookRewards(TookRewards)
{
}

AttendanceTaskLogInsert::~AttendanceTaskLogInsert()
{
}

int AttendanceTaskLogInsert::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("AttendanceTaskLogInsert::Execute. Use ADO");
        return NET_ERROR;
    }

    m_pDbManager->AttendanceTaskInsert(
        m_ChaNum,
		m_ChaCurExp,
        m_TaskN,
        m_Type,
        m_Status,
        m_Value,
        m_ValueBase,
        m_RewardPoint,
        m_RewardExp,
        m_RewardMoney,
        m_RewardItems,
        m_RewardItem1,
        m_RewardItem2,
        m_RewardItem3,
        m_TookRewards);
    return NET_OK;
}
