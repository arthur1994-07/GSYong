#include "../pch.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../Database/DBAction/DbActionGameAttendance.h"
#include "./AgentServerConfigXml.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CAgentServer::MsgAttendanceDailyMaintenanceReadyFA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    m_uCntFieldResponse++;

    UINT nAliveFieldNumber = GetOnlineFieldCount();

    sc::writeLogInfo(sc::string::format("MsgAttendanceDailyMaintenanceReadyFA() - received %1%/%2% responses from FieldServers.", m_uCntFieldResponse, nAliveFieldNumber));

    if (m_uCntFieldResponse == nAliveFieldNumber)
    {
        //
        // start AttendanceDailyMaintenance
        //
        AddGameAdoJob(
            db::DbActionPtr(
                new CAttendanceMaintenance()));
        m_uCntFieldResponse = 0;
    }
}

void CAgentServer::MsgAttendanceMaintenanceManualStartCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_ATTENDANCE_MAINTENANCE_MANUAL_START_CA_FB msgFB;

    if (!GLUseFeatures::GetInstance().IsUsingAttendance())
    {
        sc::writeLogError(std::string("MsgAttendanceMaintenanceManualStartCA() - Wrong use of Attendance while it is OFF!"));
        goto _doneMsgAttendanceMaintenanceManualStartCA;
    }

    GLCharAG* pChaAG = m_pGLAgentServer->GetChar(dwGaeaID);

    if (!pChaAG)
    {
        sc::writeLogError(std::string("MsgAttendanceMaintenanceManualStartCA() - The GM character does not exist!"));		
        goto _doneMsgAttendanceMaintenanceManualStartCA;
    }

    if (pChaAG->m_dwUserLvl < USER_GM3)
    {
        sc::writeLogError(sc::string::format("MsgAttendanceMaintenanceManualStartCA() - The GM-level(%1%) is low to do this job!", pChaAG->m_dwUserLvl));
        goto _doneMsgAttendanceMaintenanceManualStartCA;
    }

    SYSTEMTIME st;
    GetLocalTime(&st);
    AttendanceDailyMaintenance(st, TRUE);

    msgFB.bDone = TRUE;

_doneMsgAttendanceMaintenanceManualStartCA:
    SendClient(dwClient, &msgFB);
}
