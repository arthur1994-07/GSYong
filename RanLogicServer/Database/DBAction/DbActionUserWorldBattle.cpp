#include "pch.h"

#include "../../RanLogic/Msg/LoginMsg.h"
#include "../../Server/AgentServer.h"

#include "DbActionUser.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

WorldBattleUserCheck::WorldBattleUserCheck(
    const std::string& strId,
    const std::string& strPass,
    const std::string& strIP,
    int nSvrGrp,
    int nSvrNum,
    //DWORD dwClient,
    DWORD ClientSlot,
    DWORD dwServiceProvider)
    : m_strId(strId)
    , m_strPass(strPass)
    , m_nSvrGrp(nSvrGrp)
    , m_nSvrNum(nSvrNum)
    //, m_dwClientNum(dwClientNum)
    , m_dwServiceProvider(dwServiceProvider)    
{
    m_strUserIP = strIP;
    m_ClientSlot = ClientSlot;
}

WorldBattleUserCheck::~WorldBattleUserCheck()
{
}

int WorldBattleUserCheck::Execute(NetServer* pServer)
{
    CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
    if (!pAgentServer)
        return NET_ERROR;

	int nResult = m_pDbManager->WorldBattleUserCheck(m_strId, m_strPass, m_strUserIP, m_nSvrGrp, m_nSvrNum);

    NET_LOGIN_FEEDBACK_DATA2 msgFB;
    //msgFB.nType = NET_MSG_LOGIN_FB;
    msgFB.m_ClientSlot = m_ClientSlot;
    msgFB.SetIp(m_strUserIP.c_str());
    msgFB.SetUserId(m_strId.c_str());

    int nRetCode = 0;

    switch (nResult)
    {
    case sc::db::DB_ERROR:
        msgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
        sc::writeLogError(sc::string::format("WorldBattleUserCheck result %1%", nResult));
        break;

    case 0: // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)
        msgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;
        sc::writeLogError(sc::string::format("WorldBattleUserCheck result %1%", nResult));
        break;

    case 1:
    case 2: // ID/PWD 가 일치하고, IP 가 사용가능
    case 3: // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
        nRetCode = m_pDbManager->WorldBattleGetUserInfo(m_strId, OUT msgFB);
        if (nRetCode == sc::db::DB_ERROR)
        {
            msgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
            // 로그아웃 처리
            m_pDbManager->UserLogoutSimple(m_strId.c_str());
            sc::writeLogError(std::string("WorldBattleGetUserInfo"));
        }
        else
        {
            msgFB.m_emResult = EM_LOGIN_FB_SUB_OK;
        }	
        break;

    case 4: // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
        msgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;
        sc::writeLogError(std::string("WorldBattleUserCheck EM_LOGIN_FB_SUB_IP_BAN"));
        break;

    case 5: // 중복접속
        nRetCode = m_pDbManager->WorldBattleGetUserInfo(m_strId, OUT msgFB);
        if (nRetCode == sc::db::DB_ERROR)
        {
            // 로그아웃 처리
            msgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;            
            m_pDbManager->UserLogoutSimple(m_strId.c_str());
            sc::writeLogError(std::string("WorldBattleUserCheck GetUserInfo sc::db::DB_ERROR"));
        }
        else
        {
            msgFB.m_emResult = EM_LOGIN_FB_SUB_DUP;
            sc::writeLogError(
                sc::string::format(
                    "WorldBattleUserCheck result %1% ClientSlot %2% LoginID %3% UserIp %4%", nResult, m_ClientSlot, m_strId, m_strUserIP));
        }
        break;

    case 6: // Block 된 상태
        msgFB.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
        sc::writeLogError( sc::string::format("WorldBattleUserCheck result %1%", nResult));
        break;

    default: // 다른 모든경우 로그인 실패
        msgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
        sc::writeLogError(sc::string::format("WorldBattleUserCheck result %1%", nResult));
        break;
    }

    pAgentServer->InsertMsg(0, &msgFB);
    return NET_OK;
}

UserCountryUpdate::UserCountryUpdate(int UserDbNum, wb::EM_COUNTRY Country)
    : m_UserDbNum(UserDbNum)
    , m_Country(Country)
{
}

UserCountryUpdate::~UserCountryUpdate()
{
}

int UserCountryUpdate::Execute(NetServer* m_pServer)
{
    return m_pDbManager->UserCountryUpdate(m_UserDbNum, m_Country);
}