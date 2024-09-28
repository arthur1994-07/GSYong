#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../RanLogic/Msg/FriendMsg.h"

#include "../../Server/AgentServer.h"
#include "../DbManager.h"
#include "DbActionGameFriend.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace db
{

FriendDelete::FriendDelete(int nChaP, int nChaS)
{
    m_nChaP = nChaP;
    m_nChaS = nChaS;
}
int FriendDelete::Execute(NetServer* pServer)
{
    m_pDbManager->DelChaFriend(m_nChaP, m_nChaS);
    return NET_OK;
}

FriendAdd::FriendAdd(int nChaP, int nChaS)
{
    m_nChaP = nChaP;
    m_nChaS = nChaS;
}

int FriendAdd::Execute(NetServer* pServer)
{
    m_pDbManager->AddChaFriend(m_nChaP, m_nChaS);
    return NET_OK;
}

FriendFlagUpdate::FriendFlagUpdate(int nChaP, int nChaS, int nFlag)
{
    m_nChaP = nChaP;
    m_nChaS = nChaS;
    m_nFlag = nFlag;
}

int FriendFlagUpdate::Execute(NetServer* pServer)
{
    m_pDbManager->SetChaFriend(m_nChaP, m_nChaS, m_nFlag);
    return NET_OK;
}

ChaFriendGroupInsert::ChaFriendGroupInsert(int ChaDbNum, WORD GroupNum, const std::string& GroupName)
    : m_ChaDbNum(ChaDbNum)
    , m_GroupNum(GroupNum)
    , m_GroupName(GroupName)
{
}

ChaFriendGroupInsert::~ChaFriendGroupInsert()
{
}

int ChaFriendGroupInsert::Execute(NetServer* pServer)
{
    if (m_pSubDbManager)
    {
        return m_pSubDbManager->ChaFriendGroupInsert(m_ChaDbNum, m_GroupNum, m_GroupName);
    }
    else
    {
        sc::writeLogError("ChaFriendGroupInsert::Execute. must be use ADO");
        return sc::db::DB_ERROR;
    }
}

ChaFriendGroupUpdate::ChaFriendGroupUpdate(int ChaDbNum, WORD GroupNum, const std::string& GroupName)
    : m_ChaDbNum(ChaDbNum)
    , m_GroupNum(GroupNum)
    , m_GroupName(GroupName)
{
}

ChaFriendGroupUpdate::~ChaFriendGroupUpdate()
{
}

int ChaFriendGroupUpdate::Execute(NetServer* pServer)
{
    if (m_pSubDbManager)
    {
        return m_pSubDbManager->ChaFriendGroupUpdate(m_ChaDbNum, m_GroupNum, m_GroupName);
    }
    else
    {
        sc::writeLogError("ChaFriendGroupUpdate::Execute. must be use ADO");
        return sc::db::DB_ERROR;
    }
}

ChaFriendGroupDelete::ChaFriendGroupDelete(int ChaDbNum, WORD GroupNum)
    : m_ChaDbNum(ChaDbNum)
    , m_GroupNum(GroupNum)
{
}

ChaFriendGroupDelete::~ChaFriendGroupDelete()
{
}

int ChaFriendGroupDelete::Execute(NetServer* pServer)
{
    if (m_pSubDbManager)
    {
        return m_pSubDbManager->ChaFriendGroupDelete(m_ChaDbNum, m_GroupNum);
    }
    else
    {
        sc::writeLogError("ChaFriendGroupDelete::Execute. must be use ADO");
        return sc::db::DB_ERROR;
    }
}

ChaFriendGroupSet::ChaFriendGroupSet(DWORD ChaP, DWORD ChaS, WORD GroupNum)
    : m_ChaP(ChaP)
    , m_ChaS(ChaS)
    , m_GroupNum(GroupNum)
{
}

ChaFriendGroupSet::~ChaFriendGroupSet()
{
}

int ChaFriendGroupSet::Execute(NetServer* pServer)
{
    if (m_pSubDbManager)
    {
        return m_pSubDbManager->ChaFriendGroupSet(m_ChaP, m_ChaS, m_GroupNum);
    }
    else
    {
        sc::writeLogError("ChaFriendGroupSet::Execute. must be use ADO");
        return sc::db::DB_ERROR;
    }
}

FriendStateSelect::FriendStateSelect(DWORD MainChar, DWORD SubChar, DWORD ReqChaDbNum)
    : m_MainChar(MainChar)
    , m_SubChar(SubChar)
    , m_ReqCharDbNum(ReqChaDbNum)
{   
}

FriendStateSelect::~FriendStateSelect()
{
}

int FriendStateSelect::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("FriendStateSelect::Execute. must be use ADO");
        return sc::db::DB_ERROR;
    }

    int FriendFlag = 0;
    m_pDbManager->ChaFriendFlagSelect(m_MainChar, m_SubChar, FriendFlag);

    GLMSG::NET_FRIEND_STATE_REQ_DA MsgDA(m_MainChar, m_SubChar, m_ReqCharDbNum, FriendFlag);
    CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
    if (pAgentServer)
    {
        pAgentServer->InsertMsg(DBACTION_CLIENT_ID, &MsgDA);
        return sc::db::DB_OK;
    }
    else
    {
        return sc::db::DB_ERROR;
    }
}       

} // namespace db