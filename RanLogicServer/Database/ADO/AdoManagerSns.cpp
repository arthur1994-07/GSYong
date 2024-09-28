#include "../../pch.h"
#include "../../../SigmaCore/gassert.h"
#include "./ADOManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

int AdoManager::SNSAuthedFB(SFACEBOOK &AuthFB)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger    ("@ChaNum", AuthFB.ChaNum);
    Ado.AppendIParamVarchar("@SKey", AuthFB.SKey, strlen(AuthFB.SKey));
    Ado.AppendIParamVarchar("@UID",  AuthFB.UID,  strlen(AuthFB.UID));

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_SNSAuthedFB", "@nReturn");
}

int AdoManager::SNSUnAuthedFB(int nChaNum)
{   
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger("@ChaNum", nChaNum);
    
    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_SNSUnAuthedFB", "@nReturn");
}

int AdoManager::SNSGetInfoFB(SFACEBOOK &AuthFB)
{
    sc::writeLogError("Do not call this function. SNSGetInfoFB");
    return sc::db::DB_ERROR;
}

int AdoManager::SNSGetUidFB(int nTargetChaNum, std::string &m_UID)
{
    sc::writeLogError("Do not call this function. SNSGetUidFB");
    return sc::db::DB_ERROR;
}

int AdoManager::SNSAuthedTW(STWITTER &AuthTW)
{  
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger    ("@ChaNum",  AuthTW.ChaNum);
    Ado.AppendIParamVarchar("@AToken",  AuthTW.AToken,  strlen(AuthTW.AToken));
    Ado.AppendIParamVarchar("@ATokenS", AuthTW.ATokenS, strlen(AuthTW.ATokenS));
    Ado.AppendIParamVarchar("@UID",     AuthTW.UID,     strlen(AuthTW.UID));

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_SNSAuthedTW", "@nReturn");        
}

int AdoManager::SNSUnAuthedTW(int nChaNum)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger("@ChaNum", nChaNum);

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_SNSUnAuthedTW", "@nReturn");
}

int AdoManager::SNSGetInfoTW(STWITTER &AuthTW)
{
    sc::writeLogError("Do not call this function. SNSGetInfoTW");
    return sc::db::DB_ERROR;
}

int AdoManager::SNSGetUidTW(int nTargetChaNum, std::string &m_UID)
{
    sc::writeLogError("Do not call this function. SNSGetUidTW");
    return sc::db::DB_ERROR;
}

int AdoManager::SNSGetInfo(SFACEBOOK &AuthFB, STWITTER &AuthTW)
{
    sc::writeLogError("Do not call this function. SNSGetInfo");
    return sc::db::DB_ERROR;
}

} // namespace db