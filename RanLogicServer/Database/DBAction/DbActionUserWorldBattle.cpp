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

    case 0: // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)
        msgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;
        sc::writeLogError(sc::string::format("WorldBattleUserCheck result %1%", nResult));
        break;

    case 1:
    case 2: // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��
    case 3: // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)
        nRetCode = m_pDbManager->WorldBattleGetUserInfo(m_strId, OUT msgFB);
        if (nRetCode == sc::db::DB_ERROR)
        {
            msgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
            // �α׾ƿ� ó��
            m_pDbManager->UserLogoutSimple(m_strId.c_str());
            sc::writeLogError(std::string("WorldBattleGetUserInfo"));
        }
        else
        {
            msgFB.m_emResult = EM_LOGIN_FB_SUB_OK;
        }	
        break;

    case 4: // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
        msgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;
        sc::writeLogError(std::string("WorldBattleUserCheck EM_LOGIN_FB_SUB_IP_BAN"));
        break;

    case 5: // �ߺ�����
        nRetCode = m_pDbManager->WorldBattleGetUserInfo(m_strId, OUT msgFB);
        if (nRetCode == sc::db::DB_ERROR)
        {
            // �α׾ƿ� ó��
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

    case 6: // Block �� ����
        msgFB.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
        sc::writeLogError( sc::string::format("WorldBattleUserCheck result %1%", nResult));
        break;

    default: // �ٸ� ����� �α��� ����
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