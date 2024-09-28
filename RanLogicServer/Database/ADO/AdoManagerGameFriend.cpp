#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "./AdoManager.h"

#include "../../SigmaCore/DebugInclude.h"

namespace db
{

int AdoManager::GetChaFriend(int nChaNum, std::vector<CHA_FRIEND> &vecFriend)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_IPARAM_INT(Ado, "@ChaNum", nChaNum);

    if (!Ado.Execute4Cmd("dbo.ChaFriendSelect", adCmdStoredProc))	
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())
        return sc::db::DB_OK;

    do
    {
        DWORD ChaS = 0;
        std::string FriendName;
        DWORD ChaFlag = 0;
        WORD ChaGroup = 0;
        DWORD ChaClass = 0;
        int ChaLevel = 0;
        int ChaMap = 0;

        Ado.GetCollect("ChaS", ChaS);
        Ado.GetCollect("ChaName", FriendName);
        Ado.GetCollect("ChaFlag", ChaFlag);
        Ado.GetCollect("ChaGroup", ChaGroup);
        Ado.GetCollect("ChaClass", ChaClass);
        Ado.GetCollect("ChaLevel", ChaLevel);
        Ado.GetCollect("ChaStartMap", ChaMap);
        
        CHA_FRIEND sFriend(ChaGroup);
        sFriend.nChaS = ChaS;
        sFriend.nChaFlag = ChaFlag;
        sFriend.m_ChaClass = ChaClass;
        sFriend.m_ChaLevel = ChaLevel;
        sFriend.m_ChaMap = MapID(ChaMap, ChaMap);
        sFriend.SetFriendName(FriendName);

        vecFriend.push_back(sFriend);

    } while (Ado.Next());

    return sc::db::DB_OK;
}

int AdoManager::GetChaFriendSMS(int nChaNum, std::vector<CHA_FRIEND_SMS> &vFriend)
{    
    sc::writeLogError("AdoManager::GetChaFriendSMS. do not call this function.");
    return sc::db::DB_ERROR;
}

int AdoManager::ChaFriendGroupSelect(int ChaDbNum, std::vector<FRIEND_GROUP>& vGroup)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    
    APPEND_IPARAM_INT(Ado, "@ChaNum", ChaDbNum);

    if (!Ado.Execute4Cmd("dbo.ChaFriendGroupSelect", adCmdStoredProc))	
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())
        return sc::db::DB_OK;

    do 
    {
        std::string GroupName;
        WORD GroupNum = 0;
        Ado.GetCollect("ChaGroup", GroupNum);
        Ado.GetCollect("GroupName", GroupName);

        FRIEND_GROUP Group(GroupNum, GroupName);
        vGroup.push_back(Group);
    } while (Ado.Next());

    return sc::db::DB_OK;
}

int AdoManager::ChaFriendGroupInsert(int ChaDbNum, WORD GroupNum, const std::string& GroupName)
{
    sc::db::AdoExt Ado(m_GameDBConnString);

    const int GroupNameSize = 33;
        
    char szGroupName[GroupNameSize] = {0};
    StringCchCopy(szGroupName, 33, GroupName.c_str());

    APPEND_IPARAM_INT    (Ado, "@ChaNum", ChaDbNum);
    APPEND_IPARAM_INT    (Ado, "@ChaGroup", GroupNum);
    APPEND_IPARAM_VARCHAR(Ado, "@GroupName", szGroupName, GroupNameSize);

    return ExecuteStoredProcedure("dbo.ChaFriendGroupInsert", Ado);
}

int AdoManager::ChaFriendGroupUpdate(int ChaDbNum, WORD GroupNum, const std::string& GroupName)
{
    sc::db::AdoExt Ado(m_GameDBConnString);

    const int GroupNameSize = 33;

    char szGroupName[GroupNameSize] = {0};
    StringCchCopy(szGroupName, 33, GroupName.c_str());

    APPEND_IPARAM_INT    (Ado, "@ChaNum", ChaDbNum);
    APPEND_IPARAM_INT    (Ado, "@ChaGroup", GroupNum);
    APPEND_IPARAM_VARCHAR(Ado, "@GroupName", szGroupName, GroupNameSize);

    return ExecuteStoredProcedure("dbo.ChaFriendGroupUpdate", Ado);
}

int AdoManager::ChaFriendGroupDelete(int ChaDbNum, WORD GroupNum)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    
    APPEND_IPARAM_INT(Ado, "@ChaNum", ChaDbNum);
    APPEND_IPARAM_INT(Ado, "@ChaGroup", GroupNum);

    return ExecuteStoredProcedure("dbo.ChaFriendGroupDelete", Ado);
}

int AdoManager::ChaFriendGroupSet(int ChaP, int ChaS, WORD GroupNum)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
     
    APPEND_IPARAM_INT(Ado, "@nChaP", ChaP);
    APPEND_IPARAM_INT(Ado, "@nChaS", ChaS);
    APPEND_IPARAM_INT(Ado, "@ChaGroup", GroupNum);

    return ExecuteStoredProcedure("dbo.ChaFriendGroupSet", Ado);
}

int AdoManager::DelChaFriend(int nChaP, int nChaS)
{   
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    AdoGame.AppendRParamInteger();

    AdoGame.AppendIParamInteger("@nChaP", nChaP);
    AdoGame.AppendIParamInteger("@nChaS", nChaS);
        
    AdoGame.AppendOParamInteger("@nReturn");

    return AdoGame.ExecuteStoredProcedureIntReturn("dbo.DeleteChaFriend", "@nReturn");
}

int AdoManager::AddChaFriend(int nChaP, int nChaS)
{
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    AdoGame.AppendRParamInteger();

    AdoGame.AppendIParamInteger("@nChaP", nChaP);
    AdoGame.AppendIParamInteger("@nChaS", nChaS);

    AdoGame.AppendOParamInteger("@nReturn");

    return AdoGame.ExecuteStoredProcedureIntReturn("dbo.InsertChaFriend", "@nReturn");
}

int AdoManager::SetChaFriend(int nChaP, int nChaS, int nFlag)
{
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    AdoGame.AppendRParamInteger();
    
    AdoGame.AppendIParamInteger("@nChaP", nChaP);
    AdoGame.AppendIParamInteger("@nChaS", nChaS);
    AdoGame.AppendIParamInteger("@nFlag", nFlag);
        
    AdoGame.AppendOParamInteger("@nReturn");

    return AdoGame.ExecuteStoredProcedureIntReturn("dbo.UpdateChaFriend", "@nReturn");
}

int AdoManager::ChaFriendFlagSelect(int ChaP, int ChaS, int& Flag OUT)
{
    // dbo.ChaFriendFlagSelect
    // @ChaP int,
    // @ChaS int,
    // @nReturn int OUTPUT

    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_OPARAM_INT(Ado, "@nReturn");

    APPEND_IPARAM_INT(Ado, "@ChaP", ChaP);
    APPEND_IPARAM_INT(Ado, "@ChaS", ChaS);    

    int nReturn = ExecuteStoredProcedureIntReturn("dbo.ChaFriendFlagSelect", Ado, "@nReturn");
    if (nReturn == sc::db::DB_ERROR)
    {
        Flag = 0; // EMFRIEND_FLAGS
        return sc::db::DB_ERROR;
    }
    else
    {
        Flag = nReturn; // EMFRIEND_FLAGS
        return sc::db::DB_OK;
    }
}

} // namespace db