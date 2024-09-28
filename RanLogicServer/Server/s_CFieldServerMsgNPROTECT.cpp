#include "../pch.h"
#include "./s_CFieldServer.h"
#include "../Database/DBAction/DbActionLog.h"
#include "../Database/DbManager.h"
#include "../Client/NetFieldClientMan.h"

#include "../../SigmaCore/DebugInclude.h"

void CFieldServer::WriteNprotectLog( DWORD dwClient, DWORD dwResult )
{
#ifndef _NP30
	GG_AUTH_DATA Query = m_pClientManager->nProtectGetAuthQuery( dwClient );
	GG_AUTH_DATA Answer = m_pClientManager->nProtectGetAuthAnswer( dwClient );
    

    std::string strLog(
        sc::string::format(
            "ID(%1%) Result(%2%) Query(%3% %4% %5% %6%) Answer(%7% %8% %9% %10%)",
            m_pClientManager->GetUserID( dwClient ),
            dwResult,
            Query.dwIndex,
            Query.dwValue1,
            Query.dwValue2,
            Query.dwValue3,
            Answer.dwIndex,
            Answer.dwValue1,
            Answer.dwValue2,
            Answer.dwValue3));
	sc::writeLogError(strLog);        
	// 2006-08-16 대만요청
	// DB 에도 로그를 남긴다.	
    if (m_pAdoManager)
    {
        m_pAdoManager->AddLogJob(
            db::DbActionPtr(
                new db::LogHackProgram(
		            m_pClientManager->UserDbNum(dwClient),
		            m_pClientManager->ChaDbNum(dwClient),
		            NPROTECT_HACK_NUM,
		            (char*) strLog.c_str())));
    }
#else
	sc::writeLogError(sc::string::format("WriteNprotectLog ERROR ( nProtect have to work at Agent Server ) , ClientID %1%", dwClient));
#endif
}