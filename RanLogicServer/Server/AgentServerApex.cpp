#include "../pch.h"
#include "../../RanLogic/Msg/ApexMsg.h"

#include "../Client/NetAgentClientMan.h"
#include "../Database/DBAction/DbActionUser.h"
#include "../Database/DBAction/DbActionGame.h"
#include "../Database/DBAction/DbActionLog.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//_FUNC_S_REC pfRec = NULL;

#define APEX_HACK_BLOCK_DAY 3

//! Apex 가 경고메시지를 클라이언트에게 보낸다.
void CAgentServer::ApexSendMessageToClient( DWORD dwClient, WORD wAction, WORD wHour )
{
	if( ( wAction == 2 ) && ( wHour > 0x2238/*365일*/ ) )
	{
		wHour = 0x2238;
	}

    // 클라이언트에 경고 메시지를 보낸다.
	NET_APEX_ANSWER msg;
	msg.wAction = wAction;
	msg.wHour = wHour;
	//m_pClientManager->SendClient2( dwClient, &msg );
	SendClient( dwClient, &msg );
}

//! Apex 가 사용자를 블럭 시킬때 호출됨 Block
void CAgentServer::ApexUserBlock( DWORD dwClient, WORD dwHour )
{
	//	블럭시간 지정.
	CTime cBLOCK = CTime::GetCurrentTime();

	DWORD dwBlockMin  = 0;

	if ( dwHour == 0xFFFF )
		dwBlockMin = 525600; // 60 * 24 * 365; // 1년
	else
		dwBlockMin = dwHour * 60;

	CTimeSpan cSPAN( 0, 0, dwBlockMin, 0 );
	cBLOCK += cSPAN;

	m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CUserBlockDate(m_pClientManager->UserDbNum(dwClient), cBLOCK.GetTime())));
}

//! Apex 가 사용자 채팅을 금지 시킬때 호출됨 
void CAgentServer::ApexUserChatBlock( DWORD dwClient, WORD dwHour )
{
	if( dwHour == 0 ) return;
	if( dwHour > 0x2238/*365일*/ ) dwHour = 0x2238;

	DWORD dwMin = dwHour * 60;

	GLMSG::SNET_GM_CHAT_BLOCK_FB NetMsgFB;

    PGLCHARAG pCHAR =  m_pGLAgentServer->GetCharByUserAccount(std::string(m_pClientManager->GetUserID(dwClient)));
	
	if ( !pCHAR ) // 접속중이 아닐경우
	{
		NetMsgFB.bBLOCK = false;
		StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, m_pClientManager->GetUserID( dwClient ) );
		SendClient( dwClient, &NetMsgFB );
		return;
	}
	
	//	블럭시간 지정.
	CTime cBLOCK = CTime::GetCurrentTime();
	CTimeSpan cSPAN(0,0,dwMin,0);
	cBLOCK += cSPAN;

	pCHAR->m_tCHATBLOCK = cBLOCK.GetTime();

	//	DB에 저장.
    AddGameDbJob(
        db::DbActionPtr(
            new db::UserChatBlockTimeSet(pCHAR->m_UserDbNum,pCHAR->m_tCHATBLOCK)));	

	// FLD에 알림.
	GLMSG::SNET_GM_CHAT_BLOCK_FLD NetMsgFLD;
	NetMsgFLD.dwCHARID = pCHAR->m_CharDbNum;
	NetMsgFLD.dwBLOCK_MINUTE = dwMin;
	SendAllChannel(&NetMsgFLD);

	//	FB.
	NetMsgFB.bBLOCK = true;
	NetMsgFB.dwBLOCK_MINUTE = dwMin;
	StringCchCopy( NetMsgFB.szCHARNAME, CHAR_SZNAME, pCHAR->m_szName );
	StringCchCopy( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, pCHAR->m_szUserName );
	SendClient( dwClient, &NetMsgFB );
}

// APEX에 이상 사용자 로그아웃 통지
void CAgentServer::ApexUserLogout(DWORD dwClient)
{
    //#if defined( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM ) //|| defined(_RELEASED) // Apex 적용
	// APEX 버젼 변경
    if (m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_CHINA || m_ServiceProvider == SP_THAILAND)					
    {
        APEX_SERVER::NoticeApexProxy_UserLogout( dwClient, m_pClientManager->GetUserID( dwClient ) );
    }
    //#endif
}

//! Apex 가 사용자 로그를 남길때 호출됨
void CAgentServer::ApexUserHackLog(DWORD dwClient, TCHAR* szComment)
{
    if (m_pAdoManager)
    {
	    m_pAdoManager->AddLogJob(
            db::DbActionPtr(
                new db::LogHackProgram(
                    m_pClientManager->UserDbNum( dwClient ),
                    m_pClientManager->ChaDbNum( dwClient ),
                    APEX_HACK_NUM,
                    szComment)));
    }

	sc::writeLogInfo(
        sc::string::format("APEX:%s ClientNum %d UserNum %d UserID %s", 
            szComment,
            dwClient,
            m_pClientManager->UserDbNum( dwClient ),
            m_pClientManager->GetUserID( dwClient )));
}

void CAgentServer::ApexCloseClient( DWORD dwClientID)
{
    if (!m_pClientManager->IsClientSlot(dwClientID))
    {
        // APEX 의 서버간 통신연결 중단을 막는다.
        sc::writeLogError(sc::string::format("APEX Wrong call ApexCloseClient slot %1%", dwClientID));
        return;
    }
    else
    {		
        DWORD dwSetTime = timeGetTime();
        dwSetTime += 30000; // 서버에서 클라이언트 종료까지 30초의 시간을 준다.
        ApexCloseList::APEX_ID_TIME apexPair( dwClientID, dwSetTime );
        m_ApexClose.push_back( apexPair );
    }
}

void CAgentServer::ApexCloseClientFinal()
{
	DWORD dwClient = m_ApexClose.ApexCloseClientFinal();
	if (dwClient != 0)
	{
		CloseClient( dwClient, net::KICKOUT_GAMEGUARD );
	}
}

// Apex 적용
namespace APEX_SERVER
{
	int TickDataFromApex() {
		int nRet = 0;
		nRet = s_getData_FromApex_cb(1000, GetDataFromApex, NULL);
		if( nRet )
		{
			return 1;
		}
		return nRet;
	}

	int GetDataFromApex(void *pMyParaData, char* pData, char cDataType, unsigned int nSendID, int nRetLen)
	{
		int nRet = 0;
		switch (cDataType) {
			case 'D':
				nRet = NetSendToGameClient(nSendID, pData, nRetLen);
				break;
			case 'K':
				GameServerKillUser(nSendID, pData);
				break;
			default:
				break;
		}
		return nRet;	
	}

	long NetSendToGameClient(signed int nSendId,const char * pBuffer,int nLen)
	{
		if ( !g_pAgentServer || 
			 nSendId <= 0 || 
			 pBuffer==NULL || 
			 nLen<=0 )
		{
			sc::writeLogError(std::string("APEX:NetSendToGameClient"));

			return -1;
		}

        if (nLen >= NET_APEX_DATA::MAX_APEX_PACKET_LENGTH)
		{
			sc::writeLogError(sc::string::format("APEX:NetSendToGameClient nLen>MAX_APEX_PACKET_LENGTH nLen : %d", nLen ));
			return -1;
		}

		NET_APEX_DATA msg;
		bool bSet = msg.SetData(pBuffer, nLen);
        if (bSet)
		    g_pAgentServer->SendClient(nSendId, &msg);
		return 0;
	}

	long GameServerKillUser( signed int nId, char* pData )
	{
		if( !g_pAgentServer )
			return -1;		

		KillDataSt *pSt = NULL;
		pSt = (KillDataSt *)pData;

		int nErrNo = pSt->nOriData >> 16;
		nErrNo = nErrNo & 0xffff;
		int Action = pSt->nOriData & 0xffff;

		sc::writeLogError(sc::string::format("APEX:GameServerKillUser Action %d %d", nId, Action ));

		switch (Action)
		{
        // 대만 요청으로 경고단계는 삭제
		//case 1: // 경고
		//	{
		//		g_pAgentServer->ApexUserHackLog( (DWORD) nId, _T("Apex Warning") );
		//		g_pAgentServer->ApexSendMessageToClient( nId, Action, 0 );
		//	}
		//	break;
		case 2: // 대화금지			
			{
				CString strTemp( _T("Apex Chat Block ") );
				strTemp += "1";				
				g_pAgentServer->ApexUserHackLog( (DWORD) nId, (TCHAR*) strTemp.GetString() );
				g_pAgentServer->ApexSendMessageToClient( nId, Action, 1 );
				g_pAgentServer->ApexUserChatBlock( nId, 1 );
			}
			break;

        case 1: // 경고
		case 3: // 경고 & 종료			
		case 4: // 블락 & 종료		
			{
				g_pAgentServer->ApexUserHackLog( (DWORD) nId, _T("Apex User Kick") );
				g_pAgentServer->ApexSendMessageToClient( nId, Action, 0 );
				g_pAgentServer->ApexCloseClient( (DWORD) nId );
				g_pAgentServer->ApexUserLogout( (DWORD) nId );
			}
			break;
// 중국 요청으로 블락은 없앤다.
/*
			{
				CString strTemp( _T("Apex User Block ") );
				strTemp += "24";		
				g_pAgentServer->ApexUserHackLog( (DWORD) nId, (TCHAR*) strTemp.GetString() );
				g_pAgentServer->ApexSendMessageToClient( nId, Action, 24 );
				g_pAgentServer->ApexUserBlock( nId, 24 );
				g_pAgentServer->ApexCloseClient( (DWORD) nId );
			}
			break;
*/
		default:			
			break;
		};
		
		return 0;
	}

	int StartApexProxy()
	{
		// APEX 버젼 변경에 따라 시작 부분 변경
		int nRet = s_start();
		//CHSStart( NetSendToGameClient, pfRec );				
		//CHSSetFunc( (void*)(GameServerKillUser), FLAG_KILLUSER );		

		return 0;
	}

	int StopApexProxy()
	{	
		// APEX 버젼 변경에 따라 종료 부분 변경
		//CHSEnd();
		s_end();
		return 0;
	}

	int NoticeApexProxy_UserLogin(int nSendId, const char * szUserID )
	{
		//if(pfRec)
		//{
		//	pfRec( 'L', nSendId, szUserID, (int)strlen(szUserID) );			
		//}
		s_sendData_ToApex('L', nSendId, szUserID, (int)strlen(szUserID));
		return 0;
	}

	int NoticeApexProxy_UserLogout(int nSendId, const char * szUserID )
	{
		//if(pfRec)
		//{
		//	pfRec( 'G', nSendId, szUserID, (int)strlen(szUserID) );
		//}
		s_sendData_ToApex('G', nSendId, szUserID, (int)strlen(szUserID));
		return 0;
	}

	int NoticeApexProxy_UserData(int nSendId,const char * pBuf,int nBufLen)
	{
		//if(pfRec)
		//{
		//	pfRec( 'T', nSendId, pBuf, nBufLen );			
		//}
		s_sendData_ToApex('T', nSendId, pBuf, nBufLen);
		return 0;
	}

// Apex 적용( 홍콩 )

	int NoticeApexProxy_UserIP( int nSendId, int nIP )
	{
		//if(pfRec)
		//{
		//	IpCmd_st IpCmd;
		//	IpCmd.cFlag = 1;
		//	IpCmd.nIp = nIP;

		//	pfRec('S',nSendId, (char*)&IpCmd, sizeof(IpCmd_st) );
		//}
		IpCmd_st ipInfo;
		ipInfo.cFlag = 0x01;
		ipInfo.nIp = nIP;
		s_sendData_ToApex('S', nSendId, (const char*)&ipInfo, sizeof(ipInfo));

		return 0;
	}

	int NoticeApexProxy_UserReturn ( int nSendId, int nReturn )
	{
		//if( pfRec )
		//{
		//	pfRec('R',nSendId,(char*)&nReturn,sizeof(nReturn));
		//}

		return 0;
	}

};
