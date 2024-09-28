#include "pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Util/DateTime.h"
#include "../DbDefineLog.h"
#include "./ADOManager.h"

#include "../../../RanLogic/GLogicData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

int AdoManager::AddAuthServerState(int nCounrty, int nServerType, const TCHAR* szIP, int nPort, int nCurUser, int nMaxUser, const __time64_t& LogDate )	
{
    sc::db::AdoExt Ado(m_GlobalAuthDBConnString);

    Ado.AppendIParamInteger("@country", nCounrty);
    Ado.AppendIParamInteger("@servertype", nServerType);
	Ado.AppendIParamVarchar("@serverip", szIP, strlen(szIP));
    Ado.AppendIParamInteger("@serverport", nPort);
	Ado.AppendIParamInteger("@curuser", nCurUser);
	//Ado.AppendIParamInteger("@maxuser", nMaxUser);
    //std::string LogTime = sc::time::DateTimeFormat(LogDate, true, true);
    //Ado.AppendIParamVarchar("@logdate", LogTime.c_str(), LogTime.length());

    if (Ado.ExecuteStoredProcedure("dbo.sp_LogAuthServerState"))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}


int AdoManager::ProcessCertificationForAuth(int nIsSessionSvr, int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::string strUniqKey, int &nCertification, std::string &strNewUniqKey, int &nSessionSvrID)	
{
    sc::db::AdoExt Ado(m_GlobalAuthDBConnString);

	Ado.AppendIParamInteger("@country", nCounrty);
    Ado.AppendIParamInteger("@servertype", nServerType);
	Ado.AppendIParamVarchar("@serverip", szIP, strlen(szIP));
    Ado.AppendIParamInteger("@serverport", nPort);
	Ado.AppendIParamVarchar("@uniqKey", strUniqKey.c_str(), strUniqKey.length());
	Ado.AppendIParamInteger("@IsSessionSvr", nIsSessionSvr);

    if (Ado.ExecuteStoredProcedure("dbo.sp_CertificationUniqKey"))
	{
		if (Ado.GetEOF())
			return sc::db::DB_OK;
		
		do
		{
			// For TEST
			//std::string uniqKey;
			//int certification;
			//int sessionSvr;

			Ado.GetCollect("SessionSvrID", nSessionSvrID);
			Ado.GetCollect("Certification", nCertification);
			Ado.GetCollect("UniqKey", strNewUniqKey);

			//nCertification = certification;
			//strNewUniqKey = uniqKey;
			//nSessionSvrID = sessionSvr;

		} while (Ado.Next());

        return sc::db::DB_OK;
	}
    else
	{
        return sc::db::DB_ERROR;
	}

}

int AdoManager::AddAuthEventInfo( G_AUTH_EVENT_INFO *eventInfo )	
{
    sc::db::AdoExt Ado(m_GlobalAuthDBConnString);

	Ado.AppendIParamInteger("@country", eventInfo->counrtyType);
	Ado.AppendIParamInteger("@servertype", eventInfo->serverType);
	Ado.AppendIParamVarchar("@serverip", eventInfo->serverIP, strlen(eventInfo->serverIP));
	Ado.AppendIParamInteger("@serverport", eventInfo->servicePort);
	Ado.AppendIParamFloat("@eventExp", eventInfo->eventExp);
	Ado.AppendIParamFloat("@eventItem", eventInfo->eventItem);
	Ado.AppendIParamFloat("@eventMoney", eventInfo->eventMoney);
	Ado.AppendIParamFloat("@eventGrade", eventInfo->eventGrade);
	Ado.AppendIParamSmall("@eventSpeedEx", (unsigned short)eventInfo->eventSpeedEx);
	Ado.AppendIParamSmall("@eventAtkSpeedEx", (unsigned short)eventInfo->eventAtkSpeedEx);
	Ado.AppendIParamSmall("@eventAtkRateEx", (unsigned short)eventInfo->eventAtkRateEx);
	Ado.AppendIParamInteger("@limitStartLv", eventInfo->limitStartLv);
	Ado.AppendIParamInteger("@limitEndLv", eventInfo->limitEndLv);
	Ado.AppendIParamInteger("@limitPlayTime", eventInfo->limitPlayTime);
	Ado.AppendIParamInteger("@limitBusterTime", eventInfo->limitBusterTime);
	Ado.AppendIParamFloat("@limitItemGainRate", eventInfo->limitItemGainRate);
	Ado.AppendIParamFloat("@limitExpGainRate", eventInfo->limitExpGainRate);
	Ado.AppendIParamInteger("@limitEventMinute", (int)eventInfo->limitEventMinute);
	Ado.AppendIParamInteger("@eventClassExp", (int)eventInfo->eventClassExp);
	Ado.AppendIParamFloat("@eventClassExpRate", eventInfo->eventClassExpRate);
	Ado.AppendIParamInteger("@eventClassExpStartLv", eventInfo->eventClassExpStartLv);
	Ado.AppendIParamInteger("@eventClassExpEndLv", eventInfo->eventClassExpEndLv);
	Ado.AppendIParamInteger("@eventClassItem", (int)eventInfo->eventClassItem);
	Ado.AppendIParamFloat("@eventClassItemRate", eventInfo->eventClassItemRate);
	Ado.AppendIParamInteger("@eventClassItemStartLv", eventInfo->eventClassItemStartLv);
	Ado.AppendIParamInteger("@eventClassItemEndLv", eventInfo->eventClassItemEndLv);
	Ado.AppendIParamInteger("@eventClassMoney", (int)eventInfo->eventClassMoney);
	Ado.AppendIParamFloat("@eventClassMoneyRate", eventInfo->eventClassMoneyRate);
	Ado.AppendIParamInteger("@eventClassMoneyStartLv", eventInfo->eventClassMoneyStartLv);
	Ado.AppendIParamInteger("@eventClassMoneyEndLv", eventInfo->eventClassMoneyEndLv);
	Ado.AppendIParamInteger("@ipEventAttackValue", (int)eventInfo->ipEventAttackValue);
	Ado.AppendIParamInteger("@ipEventAttackSpeed", (int)eventInfo->ipEventAttackSpeed);
	Ado.AppendIParamInteger("@ipEventMoveSpeed", (int)eventInfo->ipEventMoveSpeed);
	Ado.AppendIParamInteger("@ipEventItemDrop", (int)eventInfo->ipEventItemDrop);
	Ado.AppendIParamInteger("@ipEventMoneyDrop", (int)eventInfo->ipEventMoneyDrop);
	Ado.AppendIParamInteger("@ipEventExp", (int)eventInfo->ipEventExp);
	Ado.AppendIParamInteger("@ipEventHPRecovery", (int)eventInfo->ipEventHPRecovery);
	Ado.AppendIParamInteger("@ipEventMPRecovery", (int)eventInfo->ipEventMPRecovery);
	Ado.AppendIParamInteger("@ipEventSPRecovery", (int)eventInfo->ipEventSPRecovery);
	Ado.AppendIParamInteger("@ipEventAllRecovery", (int)eventInfo->ipEventAllRecovery);
	Ado.AppendIParamInteger("@ipEventCriticalRate", (int)eventInfo->ipEventCriticalRate);

	Ado.AppendIParamFloat("@itemGainRate", eventInfo->itemGainRate);
	Ado.AppendIParamFloat("@expGainRate", eventInfo->expGainRate);
	Ado.AppendIParamInteger("@minEventLevel", eventInfo->minEventLevel);
	Ado.AppendIParamInteger("@maxEventLevel", eventInfo->maxEventLevel);
	Ado.AppendIParamInteger("@eventStart", (int)eventInfo->eventStart);
	Ado.AppendIParamInteger("@eventPlayTime", eventInfo->eventPlayTime);
	Ado.AppendIParamInteger("@eventBusterTime", eventInfo->eventBusterTime);
	std::string startTime = eventInfo->eventStartTime.Format("%Y-%m-%d %H:%M"); 
    Ado.AppendIParamVarchar("@eventStartTime", startTime.c_str(), startTime.length());
	Ado.AppendIParamInteger("@eventEndMinute", (int)eventInfo->eventEndMinute);

    if (Ado.ExecuteStoredProcedure("dbo.sp_LogAuthEventInfo"))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}

int AdoManager::AddAuthServerConnectionStateInit()	
{
    sc::db::AdoExt Ado(m_GlobalAuthDBConnString);

    if (Ado.ExecuteStoredProcedure("dbo.sp_LogAuthServerConnectionStateInit"))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}

int AdoManager::AddAuthServerConnectionState( int nSessionSvrID, int nCounrty, int nServerType, const TCHAR* szIP, int nPort )	
{
    sc::db::AdoExt Ado(m_GlobalAuthDBConnString);

	Ado.AppendIParamInteger("@svrID", nSessionSvrID);
    Ado.AppendIParamInteger("@country", nCounrty);
    Ado.AppendIParamInteger("@servertype", nServerType);
	Ado.AppendIParamVarchar("@serverip", szIP, strlen(szIP));
    Ado.AppendIParamInteger("@serverport", nPort);

    if (Ado.ExecuteStoredProcedure("dbo.sp_LogAuthServerConnectionState"))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}


int AdoManager::AuthCountryAccessApproveSelect( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::vector<int>& vecIndex OUT )
{
	/*
	@ServiceCountry = 0,
	@ServerType = 2,
	@ServerIP = N'172.16.2.93',
	@ServerPort = 15003
	*/

	sc::db::AdoExt Ado(m_GlobalAuthDBConnString);
	Ado.AppendIParamInteger("@ServiceCountry", nCounrty);
	Ado.AppendIParamInteger("@ServerType", nServerType);
	Ado.AppendIParamVarchar("@ServerIP", szIP, strlen(szIP));
	Ado.AppendIParamInteger("@ServerPort", nPort);

	if (!Ado.Execute4Cmd("dbo.CountryAccessApproveSelect", adCmdStoredProc))	
	{
		sc::writeLogInfo(std::string("CountryAccessApproveSelect ERROR"));
		return sc::db::DB_ERROR;
	}

	if (Ado.GetEOF())
	{
		sc::writeLogInfo(std::string("CountryAccessApproveSelect EOF"));
		return sc::db::DB_OK;
	}

	do
	{ 
		int nRawId=0;
		int nCountry=0;
		Ado.GetCollect("AuthID",		nRawId );
		Ado.GetCollect("CountryCode",   nCountry );

		if( nCountry != 0 )
			vecIndex.push_back( nCountry );

	} while (Ado.Next());

	return sc::db::DB_OK;
}

} // namespace db
