#include "../../pch.h"
#include "./s_COdbcManager.h"
#include "./OdbcStmt.h"
#include "../DBAction/DbAction.h"

#include "../../../SigmaCore/DebugInclude.h"

// Get All Friend list
int COdbcManager::GetChaFriend(int nChaNum, std::vector<CHA_FRIEND> &vecFriend)
{
    sc::writeLogError("COdbcManager::GetChaFriend. Use ADO");
    return sc::db::DB_ERROR;
}

// Delete friend from friend list
int COdbcManager::DelChaFriend(int nChaP, int nChaS)
{
    sc::writeLogError("COdbcManager::DelChaFriend. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string Query(
        sc::string::format(
            "{call DeleteChaFriend(%1%, %2%, ?)}",
            nChaP,
            nChaS));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::AddChaFriend(int nChaP, int nChaS)
{
    sc::writeLogError("COdbcManager::AddChaFriend. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string Query(
        sc::string::format(
            "{call InsertChaFriend(%1%, %2%, ?)}",
            nChaP,
            nChaS));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::SetChaFriend(int nChaP, int nChaS, int nFlag)
{
    sc::writeLogError("COdbcManager::SetChaFriend. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string Query(
        sc::string::format(
            "{call UpdateChaFriend(%1%, %2%, %3%, ?)}",
            nChaP,
            nChaS,
            nFlag));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::ChaFriendFlagSelect(int ChaP, int ChaS, int& Flag)
{
    sc::writeLogError("COdbcManager::ChaFriendFlagSelect. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ChaFriendGroupSelect(int ChaDbNum, std::vector<FRIEND_GROUP>& vGroup)
{
    sc::writeLogError("COdbcManager::ChaFriendGroupSelect. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ChaFriendGroupInsert(int ChaDbNum, WORD GroupNum, const std::string& GroupName)
{
    sc::writeLogError("COdbcManager::ChaFriendGroupInsert. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ChaFriendGroupUpdate(int ChaDbNum, WORD GroupNum, const std::string& GroupName)
{
    sc::writeLogError("COdbcManager::ChaFriendGroupUpdate. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ChaFriendGroupDelete(int ChaDbNum, WORD GroupNum)
{
    sc::writeLogError("COdbcManager::ChaFriendGroupDelete. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ChaFriendGroupSet(int ChaP, int ChaS, WORD GroupNum)
{
    sc::writeLogError("COdbcManager::ChaFriendGroupSet. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::NotifySelect(int ChaDbNum, std::vector<std::tr1::shared_ptr<gamelogic::NotifyBase> >& vNotify)
{
    sc::writeLogError("COdbcManager::NotifySelect. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::NotifyInsert(int ChaDbNum, gamelogic::notify::NOTIFY_TYPE Type, sc::MGUID& Guid, const std::string& Data)
{
    sc::writeLogError("COdbcManager::NotifyInsert. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::NotifyDelete(sc::MGUID& Guid)
{
    sc::writeLogError("COdbcManager::NotifyDelete. Use ADO");
    return sc::db::DB_ERROR;
}
