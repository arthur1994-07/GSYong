#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../RanLogic/Notify/Notify.h"
#include "./AdoManager.h"

#include "../../SigmaCore/DebugInclude.h"

namespace db
{

int AdoManager::NotifySelect(int ChaDbNum, std::vector<std::tr1::shared_ptr<gamelogic::NotifyBase> >& vNotify)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_IPARAM_INT(Ado, "@ChaNum", ChaDbNum);

    if (!Ado.Execute4Cmd("dbo.NotifySelect", adCmdStoredProc))	
        return sc::db::DB_ERROR;
    
    if (Ado.GetEOF())
        return sc::db::DB_OK;

    do
    {   
        std::string strGuid;
        sc::MGUID Guid;
        WORD NotifyType = 0;
        std::string JsonData;
        _variant_t varDate;

        Ado.GetCollect("NotifyGuid", strGuid);
        Ado.GetCollect("NotifyType", NotifyType);
        Ado.GetCollect("NotifyData", JsonData);
        Ado.GetCollect("NotifyDate", varDate);
        
        char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
        StringCchCopy(szGuid, sc::string::UUID_LENGTH_BRACES, strGuid.c_str());
        sc::string::stringToUUID(szGuid, Guid);

        //sc::writeLogInfo(sc::string::format("NotifySelect %1% %2%", strGuid, szGuid));

        std::tr1::shared_ptr<gamelogic::NotifyBase> spNotify = gamelogic::MakeNotify(
            static_cast<gamelogic::notify::NOTIFY_TYPE> (NotifyType),
            Guid,
            JsonData,
            sc::time::GetTime(varDate));
        if (spNotify)
            vNotify.push_back(spNotify);

    } while (Ado.Next());

    return sc::db::DB_OK;

}

int AdoManager::NotifyInsert(int ChaDbNum, gamelogic::notify::NOTIFY_TYPE Type, sc::MGUID& Guid, const std::string& Data)
{
    //char szJsonStr[512] = {0};
    //StringCchCopy(szJsonStr, 512, Data.c_str());

    std::string strGuid = sc::string::uuidToString(Guid);
	sc::string::upperString(strGuid);

	sc::db::AdoExt Ado(m_GameDBConnString);
	APPEND_IPARAM_VARCHAR(Ado, "@NotifyGuid", strGuid.c_str(), static_cast<long> (strGuid.length()));
    APPEND_IPARAM_INT    (Ado, "@ChaNum",     ChaDbNum);    
    APPEND_IPARAM_SMALL  (Ado, "@NotifyType", static_cast<WORD> (Type));
    APPEND_IPARAM_VARCHAR(Ado, "@NotifyData", Data.c_str(), Data.length());

    return ExecuteStoredProcedure("dbo.NotifyInsert", Ado);
}

int AdoManager::NotifyDelete(sc::MGUID& Guid)
{   
    std::string strGuid = sc::string::uuidToString(Guid);
	sc::string::upperString(strGuid);

    sc::db::AdoExt Ado(m_GameDBConnString);

    APPEND_IPARAM_VARCHAR(Ado, "@NotifyGuid", strGuid.c_str(), static_cast<long> (strGuid.length()));

    return ExecuteStoredProcedure("dbo.NotifyDelete", Ado);
}

} // namespace db