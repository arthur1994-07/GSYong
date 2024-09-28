#include "../../pch.h"
#include "../../RanLogic/Msg/LoginMsg.h"
#include "DbActionUser.h"

#include "../../Server/AgentServer.h"

#include <boost/tuple/tuple.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Agent 서버 : 캐릭터 생성숫자 1 감소
CAgentUserChaNumDecrease::CAgentUserChaNumDecrease(int nUserNum)
{
    m_nUserNum = nUserNum;
}

int CAgentUserChaNumDecrease::Execute(NetServer* pServer)
{
    return m_pDbManager->UserChaNumDecrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// TEST Session 서버 : 캐릭터 생성숫자 1 감소
CUserTestChaNumDecrease::CUserTestChaNumDecrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CUserTestChaNumDecrease::Execute(NetServer* pServer)
{
	return m_pDbManager->UserTestChaNumDecrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// TEST Agent 서버 : 캐릭터 생성숫자 1 감소
CAgentUserTestChaNumDecrease::CAgentUserTestChaNumDecrease(int nUserNum)
{
    m_nUserNum = nUserNum;
}

int CAgentUserTestChaNumDecrease::Execute(NetServer* pServer)
{
    return m_pDbManager->UserTestChaNumDecrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// Session : 생성할 수 있는 캐릭터 수 1 증가
CUserChaNumIncrease::CUserChaNumIncrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CUserChaNumIncrease::Execute(NetServer* pServer)
{
    return m_pDbManager->UserChaNumIncrease(m_nUserNum);
}


///////////////////////////////////////////////////////////////////////////////
// Session 서버 : 캐릭터 생성숫자 1 감소
CUserChaNumDecrease::CUserChaNumDecrease(int nUserNum) 
{ 
	m_nUserNum = nUserNum;
}

int CUserChaNumDecrease::Execute(NetServer* pServer)
{
	return m_pDbManager->UserChaNumDecrease(m_nUserNum);
}


///////////////////////////////////////////////////////////////////////////////
// Agent : 생성할 수 있는 캐릭터 수 1 증가
CAgentUserChaNumIncrease::CAgentUserChaNumIncrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CAgentUserChaNumIncrease::Execute(NetServer* pServer)
{
    return m_pDbManager->UserChaNumIncrease(m_nUserNum);
}


///////////////////////////////////////////////////////////////////////////////
// TEST Session : 생성할 수 있는 캐릭터 수 1 증가
CUserTestChaNumIncrease::CUserTestChaNumIncrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CUserTestChaNumIncrease::Execute(NetServer* pServer)
{
	return m_pDbManager->UserTestChaNumIncrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// TEST Agent : 생성할 수 있는 캐릭터 수 1 증가
CAgentUserTestChaNumIncrease::CAgentUserTestChaNumIncrease(int nUserNum)
{
    m_nUserNum = nUserNum;
}

int CAgentUserTestChaNumIncrease::Execute(NetServer* pServer)
{
    return m_pDbManager->UserTestChaNumIncrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
//
CUserUpdateCha::CUserUpdateCha(int nUserNum, const char* szCharName)
{
	m_nUserNum = nUserNum;
    if (szCharName)
    {
	    m_strCharName = szCharName;
        sc::string::trim(m_strCharName, " ");        
        sc::string::replaceForDb(m_strCharName);
    }
}

CUserUpdateCha::~CUserUpdateCha()
{
}

int CUserUpdateCha::Execute(NetServer* pServer)
{
	return m_pDbManager->UserUpdateCha(m_nUserNum, m_strCharName.c_str());
}

///////////////////////////////////////////////////////////////////////////////
// 일본 WhiteRock과의 로그인 통신
CAgentJapnaWRLogin::CAgentJapnaWRLogin(
    const char* szUserID,
    const char* szPassword,
    const std::string& strIP,
    int	nServerGroup,
    int nServerNum,
    DWORD dwClientNum )
{
	m_strUserID		= szUserID;
	m_strPassword	= szPassword;
    m_strIP			= strIP.c_str();
	m_nServerGroup	= nServerGroup;
	m_nServerNum	= nServerNum;
	m_dwClientNum	= dwClientNum;
}

CAgentJapnaWRLogin::~CAgentJapnaWRLogin()
{
}

int CAgentJapnaWRLogin::Execute(NetServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;

	int nResult = CWhiteRock::GetInstance()->SendLoginXML( m_strUserID, m_strPassword, m_strIP );

	if ( HTTP_STATUS_OK != nResult )
	{
		// WhiteRock 서버와 통신 실패
		pTemp->JapanWRFailed( m_dwClientNum, nResult );
	}
	else
	{
		const CString& strUUID   = CWhiteRock::GetInstance()->GetUUID();
		int nUserNum = CWhiteRock::GetInstance()->GetUserNum();
		// WhiteRock 서버와 통신 성공
		pTemp->JapanWRSuccess( m_dwClientNum, m_strUserID,
									m_nServerGroup, m_nServerNum, strUUID, nUserNum );
	}

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// 일본 WhiteRock과의 로그아웃 통신
CAgentJapnaWRLogout::CAgentJapnaWRLogout( const char* szUserUUID )
{
	m_strUserUUID	= szUserUUID;
}

int CAgentJapnaWRLogout::Execute(NetServer* pServer)
{	
	CWhiteRock::GetInstance()->SendLogoutXML( m_strUserUUID );
	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
CUserLogoutSimple::CUserLogoutSimple(const CString& strUserID)
{
	m_strUserID = strUserID;
}

int CUserLogoutSimple::Execute(NetServer* pServer)
{
	return m_pDbManager->UserLogoutSimple(m_strUserID.GetString());
}

///////////////////////////////////////////////////////////////////////////////
//
CUserLogoutSimple2::CUserLogoutSimple2(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CUserLogoutSimple2::Execute(NetServer* pServer)
{
	return m_pDbManager->UserLogoutSimple2(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
//
CUserLogout::CUserLogout(const char* szUserID, 
						int nUserNum, 
						int nGameTime,
						int nChaNum,
						int nSvrGrp,
						int nSvrNum, 
						int nTotalGameTime,
						int nOfflineTime, 
						const char* szUserIP )
{
	m_strUserID = szUserID;
	m_nUserNum = nUserNum;
	m_nGameTime = nGameTime;
	m_nChaNum = nChaNum;
	m_nSvrGrp = nSvrGrp;
	m_nSvrNum = nSvrNum;
	m_nTotalGameTime = nTotalGameTime;
	m_nOfflineTime = nOfflineTime;
	m_strUserIP = szUserIP;
	if ( m_strUserIP.IsEmpty() )
	{
		m_strUserIP = "ip null";
	}
}

int CUserLogout::Execute(NetServer* pServer)
{
	return m_pDbManager->UserLogout(m_strUserID.GetString(),
													m_strUserIP.GetString(),
													m_nUserNum,
													m_nGameTime,
													m_nChaNum,
													m_nSvrGrp,
													m_nSvrNum,
													m_nTotalGameTime,
													m_nOfflineTime );
}

/**
 * \ingroup NetServerLib
 * 사용자 체크
 * Thailand (태국)
 */
CAgentThaiUserCheck::CAgentThaiUserCheck(
    const char* szUserID,
    const char* szPasswd, 
    const std::string& UserIP, 
    int nSvrGrp, 
    int nSvrNum,
    DWORD dwClient,
    DWORD dwClientNum)
{
	m_ClientSlot  = dwClient;
	m_strUserID = szUserID;
	m_strPasswd = szPasswd;
	m_strUserIP = UserIP;
	m_nSvrGrp   = nSvrGrp;
	m_nSvrNum   = nSvrNum;
	m_dwClientNum = dwClientNum;
}

int CAgentThaiUserCheck::Execute (NetServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);	

	int nRetCode = 0;	
	int nResult = m_pDbManager->ThaiUserCheck(
        m_strUserID.GetString(),
        m_strPasswd.GetString(),
        m_strUserIP.c_str(),
        m_nSvrGrp,
        m_nSvrNum );
	int nResult1 = m_pDbManager->ThaiUserClassCheck( m_strUserIP.c_str() );

	// 전송할 구조체를 세팅한다.
	NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.m_ClientSlot   = m_dwClientNum;

	StringCchCopy (NetMsgFB.szIp, MAX_IP_LENGTH+1, m_strUserIP.c_str()); // ip
	StringCchCopy (NetMsgFB.szUserid, USR_ID_LENGTH+1, m_strUserID.GetString()); // id

	NetMsgFB.emIPBonus = (EMIP_BONUS_CLASS) nResult1;
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 가져온다.
		nRetCode = m_pDbManager->ThaiGetUserInfo( m_strUserID.GetString(), &NetMsgFB );
		if (nRetCode == sc::db::DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
			// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

			sc::writeLogError(std::string("CAgentUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
		else
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_OK;

			// 아이템 변환 중인지 확인
			int Result = 0;
			m_pDbManager->GetItemTransferStatus( NetMsgFB.nUserNum, Result );
			NetMsgFB.ItemTransferResult = Result;
		}
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;
		break;
	case 5 : // 중복접속
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_DUP;
		break;
	case 6 : // Block 된 상태
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_BLOCK;
		break;	
	case 7 : // 18 세미만 3 시간 경과
		NetMsgFB.m_emResult = EM_LOGIN_FB_THAI_UNDER18_3HOUR;
		break;
	case 8 : // 18 세미만 접속불가능 시간
		NetMsgFB.m_emResult = EM_LOGIN_FB_THAI_UNDER18_TIME;
		break;
	case 9 : // 18 세이상 접속불가능 시간
		NetMsgFB.m_emResult = EM_LOGIN_FB_THAI_OVER18_TIME;
		break;
	default : // 다른 모든경우 로그인 실패
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;
		break;
	}

	pTemp->ThaiMsgLogInBack( &NetMsgFB );

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// 일반 : 사용자 로그인
CAgentUserCheck::CAgentUserCheck(
	const TCHAR* szUserID,
	const TCHAR* szPasswd, 
    const std::string& UserIP, 
	const TCHAR* szRandomPasswd, // Random password
	int nRandomNum,
	int nSvrGrp, 
	int nSvrNum,
	DWORD dwClient,
	DWORD dwClientNum,
	DWORD dwServiceProvider )
	: m_strUserID( szUserID )
	, m_strPasswd( szPasswd )	
	, m_strRandomPasswd( szRandomPasswd )
	, m_nSvrGrp( nSvrGrp )
	, m_nSvrNum( nSvrNum )

	, m_dwClientNum( dwClientNum )
	, m_nRandomNum( nRandomNum )
	, m_dwServiceProvider( dwServiceProvider )
{	
	m_ClientSlot = dwClient;
	m_strUserIP = UserIP;
}

//
//mjeon
//CAgentUserCheck has same code-flow as CAddLogServerState that was drawed in mjeon's note.
//
int CAgentUserCheck::Execute(NetServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;

	int nRetCode = 0;	
	int nResult = m_pDbManager->UserCheck(m_strUserID.GetString(),
        m_strPasswd.GetString(),
        m_strUserIP.c_str(),
        m_strRandomPasswd.GetString(),
        m_nRandomNum,
        m_nSvrGrp,
        m_nSvrNum);

	// 전송할 구조체를 세팅한다.
	NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.m_ClientSlot	= m_dwClientNum;

	StringCchCopy(NetMsgFB.szIp,	 MAX_IP_LENGTH+1, m_strUserIP.c_str()); // ip
	StringCchCopy(NetMsgFB.szUserid, USR_ID_LENGTH+1, m_strUserID.GetString()); // id

	// 내부 서버에서 태국 User Class Type 체크를 위해서 추가
	//	IP보너스
/*
#if defined(_RELEASED) 
	int nResult1 = m_pDbManager->PhUserClassCheck( m_strUserIP.GetString() );	
	NetMsgFB.emIPBonus = (EMIP_BONUS_CLASS)nResult1;
#endif 
*/	
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError(sc::string::format("CAgentUserCheck result %1% %2%", nResult, m_strUserID.GetString()));
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError(sc::string::format("CAgentUserCheck result %1% %2% %3%", nResult, m_strUserID.GetString(),m_strUserIP.c_str()));
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
	case 7 : // ID/PWD/랜덤비밀번호 일치

		//
		//mjeon.ReturnOfHero
		//
	case 11 : // case 1과 동일 + ReturnOfHero event
	case 12 : // case 2과 동일 + ReturnOfHero event
	case 13 : // case 3과 동일 + ReturnOfHero event
	case 17 : // case 7과 동일 + ReturnOfHero event

	case 30 : // 로그인은 성공 + 2차비번 신규 설정 필요
	case 31 : // case 1, 2, 3, 7과 동일 + 2차비번 사용중

		// 사용자 정보를 가져온다.
		nRetCode = m_pDbManager->GetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
			// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

			sc::writeLogError(std::string("CAgentUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
		else
		{
			if( nResult == 30 )
				NetMsgFB.m_emResult = EM_LOGIN_FB_KR_OK_NEW_PASS;
			else if( nResult == 31 )
				NetMsgFB.m_emResult = EM_LOGIN_FB_KR_OK_USE_PASS;
			else
				NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_OK;

			// 아이템 변환 중인지 확인
			int Result = 0;
			m_pDbManager->GetItemTransferStatus( NetMsgFB.nUserNum, Result );
			NetMsgFB.ItemTransferResult = Result;
		}	

		nRetCode = m_pDbManager->GetUserCountryInfo( NetMsgFB.nUserNum, NetMsgFB.sCountryInfo );
		if ( nRetCode == sc::db::DB_ERROR )
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;

			// 로그아웃 처리
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
			// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

			sc::writeLogError(std::string("CAgentUserCheck GetUserCountryInfo sc::db::DB_ERROR") );
		}
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError(sc::string::format("CAgentUserCheck result %1% %2%", nResult, m_strUserID.GetString()));
		break;
	case 5 : // 중복접속
		// 사용자 정보를 가져온다.
		nRetCode = m_pDbManager->GetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());

			sc::writeLogError(std::string("CAgentUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
        else
        {
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_DUP;			
			sc::writeLogError(
                sc::string::format(
                    "CAgentUserCheck result %1% ClientNum %2% LoginID %3% UserIp %4%",
                    nResult,
                    m_dwClientNum,
                    m_strUserID.GetString(),
                    m_strUserIP.c_str()));
		}
		break;
	case 6 : // Block 된 상태
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError(sc::string::format("CAgentUserCheck result %1% %2%", nResult, m_strUserID.GetString()));
		break;
	case 8 : // EM_LOGIN_FB_SUB_RANDOM_PASS
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_RANDOM_PASS;		
		sc::writeLogError( sc::string::format("CAgentUserCheck result %1% %2%", nResult, m_strUserID.GetString()));
		break;
#ifdef GS_PARAM
	case 23 : // EM_LOGIN_FB_SUB_BETAKEY
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_BETAKEY;
		sc::writeLogError(sc::string::format("CAgentUserCheck result %1% %2%", nResult, m_strUserID.GetString()));
		break;
#endif
	default : // 다른 모든경우 로그인 실패
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError(sc::string::format("CAgentUserCheck result %1% %2%", nResult, m_strUserID.GetString()));
		break;
	}
	
	if (m_dwServiceProvider == SP_CHINA)
		pTemp->ChinaMsgLoginBack( &NetMsgFB );
	else
        pTemp->MsgLogInBack( &NetMsgFB );

	return NET_OK;
}



//
//mjeon.indonesia
//
DIdnUserCheck::DIdnUserCheck(
    const std::string& strId,
    const std::string& strPass,
    const std::string& strIP,
    int nSvrGrp,
    int nSvrNum,
    DWORD dwClient,
    DWORD dwClientNum,
    DWORD dwServiceProvider)
    : m_strId(strId)
    , m_strPass(strPass)
    , m_nSvrGrp(nSvrGrp)
    , m_nSvrNum(nSvrNum)
    , m_dwClientNum(dwClientNum)
    , m_dwServiceProvider(dwServiceProvider)
{
	m_ClientSlot	= dwClient;
	m_strUserIP	= strIP;
}


int DIdnUserCheck::Execute(NetServer* pServer)
{
	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>(pServer);

	if (pAgentServer == NULL)
		return NET_ERROR;

	/*
	mjeon
	인도네시아 - 무작위 Login 해킹 문제 방지를 위해 임시방편으로 Ban 코드 삽입

	근본적인 문제를 해결하기 위해서는 VTC쪽 eBank단에서 처리해주어야 한다.
	*/
#define IDN_LOGIN_BAN_FAILURE_LIMIT		(3)				//3번 실패시 Ban
#define IDN_LOGIN_BAN_TIME_LIMIT		(180000)		//3분
	
	static std::map<std::string, boost::tuple<DWORD/*failure count*/, DWORD/*last timestamp*/>> mapBan;	
	
	DWORD dwFailure	= 0;
	DWORD dwLastTick= 0;
	DWORD dwTickNow	= GetTickCount();
	
	if(mapBan.count(m_strId) > 0)
	{
		dwFailure	= mapBan[m_strId].get<0>();	//failure count
		dwLastTick	= mapBan[m_strId].get<1>();	//last timestamp
		
		if (dwFailure >= IDN_LOGIN_BAN_FAILURE_LIMIT)
		{
			if((dwTickNow - dwLastTick) > IDN_LOGIN_BAN_TIME_LIMIT)
			{
				mapBan.erase(m_strId);

				dwFailure	= 0;
				dwLastTick	= 0;
			}
			else
			{
				sc::writeLogError(sc::string::format("DIdnUserCheck::Execute() - %1% is still Banned.", m_strId));
				return NET_ERROR;
			}
		}
	}


	//
	//HTTP를 이용해서 VTC 인증
	//
	int nUserNum = pAgentServer->IdnAuthVTC(m_strId, m_strPass);

	if(nUserNum > 0)
	{
		//VTC auth succeed.

		if (mapBan.count(m_strId) > 0)
		{
			mapBan.erase(m_strId);

			dwFailure	= 0;
			dwLastTick	= 0;
		}
	}
	else
	{
		sc::writeLogError(sc::string::format("DIdnUserCheck::Execute() - VTC auth failed(%1%). ID: %2%, PW: %3%", nUserNum, m_strId, m_strPass ));
		switch (nUserNum)
		{
		case -1:
			sc::writeLogError(std::string("DIdnUserCheck::Execute() - System error."));
			break;

		case -5:
			sc::writeLogError(std::string("DIdnUserCheck::Execute() - Electronic signature is invalid."));
			break;

		case -48:
			sc::writeLogError(std::string("DIdnUserCheck::Execute() - The account is blocked."));
			break;

		case -50:
			sc::writeLogError(std::string("DIdnUserCheck::Execute() - The account does not exist."));
			break;

		case -53:
			mapBan[m_strId] = boost::make_tuple(++dwFailure, dwTickNow);

			sc::writeLogError(std::string("DIdnUserCheck::Execute() - Password is invalid."));
			break;
		
		case -100:
			sc::writeLogError(std::string("DIdnUserCheck::Execute() - Service code doest not exis."));
			break;

		case -101:
			sc::writeLogError(std::string("DIdnUserCheck::Execute() - Service code is invalid."));
			break;

		case -999:
			sc::writeLogError(std::string("DIdnUserCheck::Execute() - Webservice system error."));			
			break;

		default:
			sc::writeLogError(std::string("DIdnUserCheck::Execute() - Unkown Error."));			
			break;
		}

		return NET_ERROR;
	}

	int nRetCode = 0;
	
	int nResult = m_pDbManager->IdnUserCheck(nUserNum, m_strId, m_strUserIP, m_nSvrGrp, m_nSvrNum);

	
	NET_LOGIN_FEEDBACK_DATA2 msgFB;
	msgFB.nType		= NET_MSG_LOGIN_FB;
	msgFB.m_ClientSlot	= m_dwClientNum;

	StringCchCopy(msgFB.szIp,	 MAX_IP_LENGTH+1, m_strUserIP.c_str()); // ip
	StringCchCopy(msgFB.szUserid, USR_ID_LENGTH+1, m_strId.c_str()); // id

		
	switch (nResult)
	{
	case sc::db::DB_ERROR:
		msgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
		sc::writeLogError(sc::string::format("DIdnUserCheck result %d", nResult));
		break;

	case 0: // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)
		msgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;
		sc::writeLogError(sc::string::format("DIdnUserCheck result %d", nResult));
		break;

	case 1:
	case 2: // ID/PWD 가 일치하고, IP 가 사용가능
	case 3: // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		nRetCode = m_pDbManager->IdnGetUserInfo(m_strId, OUT &msgFB);
		
		if (nRetCode == sc::db::DB_ERROR)
		{
			msgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			
			// 로그아웃 처리
			m_pDbManager->UserLogoutSimple(m_strId.c_str());

			sc::writeLogError(std::string("DIdnUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
		else
		{
			msgFB.m_emResult = EM_LOGIN_FB_SUB_OK;			
		}	
		break;

	case 4: // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		msgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;
		sc::writeLogError( sc::string::format("DIdnUserCheck result %d", nResult) );
		break;

	case 5: // 중복접속
		nRetCode = m_pDbManager->IdnGetUserInfo(m_strId, OUT &msgFB);
		
		if (nRetCode == sc::db::DB_ERROR)
		{
			msgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			
			// 로그아웃 처리
			m_pDbManager->UserLogoutSimple (m_strId.c_str());

			sc::writeLogError(std::string("DIdnUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
        else
        {
			msgFB.m_emResult = EM_LOGIN_FB_SUB_DUP;			
			sc::writeLogError(
                sc::string::format(
                    "DIdnUserCheck result %1% ClientNum %2% LoginID %3% UserIp %4%", nResult, m_dwClientNum, m_strId, m_strUserIP));
		}
		break;

	case 6: // Block 된 상태
		msgFB.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError( sc::string::format("DIdnUserCheck result %d", nResult));
		break;

	default: // 다른 모든경우 로그인 실패
		msgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("DIdnUserCheck result %d", nResult) );
		break;
	}
	
	pAgentServer->IdnMsgLogInBack(&msgFB);

	return NET_OK;
}

/*
///////////////////////////////////////////////////////////////////////////////
// DAUM : 사용자 로그인
CAgentDaumUserCheck::CAgentDaumUserCheck(const char* szDaumGID, 
		                                 const char* szDaumUID,
				                         const char* szDaumSSNHEAD,
				                         const char* szDaumSEX,
				                         const char* szUserIP, 
				                         int nSvrGrp, 
				                         int nSvrNum,				   
				                         DWORD dwClient,
				                         DWORD dwClientNum)
{
	m_strDaumGID		= szDaumGID;
	m_strDaumUID		= szDaumUID;
	m_strDaumSSNHEAD	= szDaumSSNHEAD;
	m_strDaumSEX		= szDaumSEX;
	m_strUserIP			= szUserIP;
	m_nSvrGrp			= nSvrGrp;
	m_nSvrNum			= nSvrNum;		
	m_dwClient			= dwClient; 
	m_dwClientNum		= dwClientNum; // Agent 클라이언트 번호
}
	
int CAgentDaumUserCheck::Execute(CServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) {
		sc::writeLogError("CAgentDaumUserCheck::Execute pTemp==NULL");
		return NET_ERROR;
	}

	int nResult = m_pDbManager->DaumUserCheck(m_strDaumGID.GetString(),
															 m_strDaumUID.GetString(),
															 m_strDaumSSNHEAD.GetString(),
															 m_strDaumSEX.GetString(),
															 m_strUserIP.GetString(),
															 m_nSvrGrp,
															 m_nSvrNum);

	// 전송할 구조체를 세팅한다.
	DAUM_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nmg.nType = DAUM_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	::StringCchCopy(nlfd2.szDaumGID, DAUM_MAX_GID_LENGTH+1, m_strDaumGID.GetString());
	::StringCchCopy(nlfd2.szDaumUID, DAUM_MAX_UID_LENGTH+1, m_strDaumUID.GetString());
	::StringCchCopy(nlfd2.szIp,	     MAX_IP_LENGTH+1,	    m_strUserIP.GetString()); // ip	
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		nlfd2.nResult = EM_LOGIN_FB_SUB_INCORRECT;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 세팅한다.
		nlfd2.nResult      = EM_LOGIN_FB_SUB_OK;
        m_pDbManager->DaumGetUserInfo(m_strDaumUID.GetString(), &nlfd2);
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.nResult = EM_LOGIN_FB_SUB_IP_BAN;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	case 5 : // 중복접속
		nlfd2.nResult = EM_LOGIN_FB_SUB_DUP;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	case 6 : // Block 된 상태
		nlfd2.nResult = EM_LOGIN_FB_SUB_BLOCK;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	}

	pTemp->DaumMsgLoginBack(&nlfd2);
	return NET_OK;
}
*/
///////////////////////////////////////////////////////////////////////////////
// DAUM : 사용자 로그인
CAgentDaumUserCheck::CAgentDaumUserCheck(
		const TCHAR* szUUID,
        const std::string& UserIP,
		int nSvrGrp,
		int nSvrNum,
		DWORD dwClient,
		DWORD dwClientNum )
{
		m_strUUID = szUUID;
		m_strUserIP = UserIP.c_str();
		m_nSvrGrp = nSvrGrp;
		m_nSvrNum = nSvrNum;
		m_dwClientNum = dwClientNum;	
}

int CAgentDaumUserCheck::Execute( NetServer *pServer )
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	if (!pTemp)
        return NET_ERROR;

	TCHAR szKorUserID[DAUM_MAX_GID_LENGTH+1] = {0};

	int nRetCode = 0;
	int UserAge = 0;
    int nResult = m_pDbManager->DaumUserCheck(
		szKorUserID,
		m_strUUID.GetString(),
		m_strUserIP.GetString(),
		m_nSvrGrp,
		m_nSvrNum,
        UserAge);

	DAUM_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nType = DAUM_NET_MSG_LOGIN_FB;
	nlfd2.nClient = m_dwClientNum;
	nlfd2.SetDaumGid(szKorUserID);// gid
	nlfd2.SetIp(m_strUserIP.GetString()); // ip	
	nlfd2.SetUUID(m_strUUID.GetString()); // UUID
    nlfd2.SetAge(UserAge);

	switch (nResult)
	{
	case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentDaumUserCheck result %d", nResult) );
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentDaumUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)

	case 11 : // case 1: + Return Of Hero
	case 12 : // case 2: + Return Of Hero
	case 13 : // case 3: + Return Of Hero

	case 30 : // 로그인은 성공 + 2차비번 신규 설정 필요
	case 31 : // case 1, 2, 3, 7과 동일 + 2차비번 사용중

		// 사용자 정보를 세팅한다.
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_OK;

		if( m_pDbManager->DaumGetUserInfo(szKorUserID, &nlfd2) == sc::db::DB_ERROR ) // 가져오기 실패
		{
			nlfd2.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
		}
		else
		{
			if( nResult == 30 )
				nlfd2.m_emResult = EM_LOGIN_FB_KR_OK_NEW_PASS;
			else if( nResult == 31 )
				nlfd2.m_emResult = EM_LOGIN_FB_KR_OK_USE_PASS;
			else
				nlfd2.m_emResult = EM_LOGIN_FB_SUB_OK;	

			// 아이템 변환 중인지 확인
			int Result = 0;
			m_pDbManager->GetItemTransferStatus( nlfd2.nUserNum, Result );
			nlfd2.ItemTransferResult = Result;
		}

		nRetCode = m_pDbManager->GetUserCountryInfo( nlfd2.nUserNum, nlfd2.sCountryInfo );
		if ( nRetCode == sc::db::DB_ERROR )
		{
			nlfd2.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;

			sc::writeLogError(std::string("CAgentDaumUserCheck GetUserCountryInfo sc::db::DB_ERROR") );
		}
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	case 5 : // 중복접속
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_DUP;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	case 6 : // Block 된 상태
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	case 20 : // Web 인증 시간 초과 
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_EXPIRED;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	case 21 : // UUID가 web중복 로그인으로 인해 사라짐
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_UID_ERR;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	}

	pTemp->DaumMsgLoginBack(&nlfd2);
	return NET_OK;
}

/**
 * GSP : 사용자 로그인
 * \param szDecodedTID 
 * \param szUserIP 
 * \param nSvrGrp 
 * \param nSvrNum 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
CAgentGspUserCheck::CAgentGspUserCheck(
    const TCHAR* szUUID,
    const std::string& UserIP,
    int nSvrGrp,
    int nSvrNum,
    DWORD dwClient,
    DWORD dwClientNum)
{
	m_strUUID = szUUID;
	m_strUserIP = UserIP.c_str();
	m_nSvrGrp = nSvrGrp;
	m_nSvrNum = nSvrNum;
	m_dwClientNum = dwClientNum; // Agent 에서의 Client 번호
}

int CAgentGspUserCheck::Execute(NetServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;

	TCHAR szGspUserID[GSP_USERID] = {0};

	int nResult = m_pDbManager->GspUserCheck(
		szGspUserID,
		m_strUUID.GetString(),
		m_strUserIP.GetString(),
		m_nSvrGrp,
		m_nSvrNum );

	// 전송할 구조체를 세팅한다.
	GSP_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nType = GSP_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	StringCchCopy( nlfd2.szGspUserID, GSP_USERID, szGspUserID ); // ID	
	StringCchCopy( nlfd2.szIp, GSP_USERIP, m_strUserIP.GetString() ); // ip
	StringCchCopy( nlfd2.szUUID, sc::string::UUID_LENGTH, m_strUUID.GetString() ); // UUID
	
	switch (nResult)
	{
	/*
	case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		sc::writeLogError(
			_T("CAgentGspUserCheck result %d"), 
			nResult);
		break;
	*/
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentGspUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 세팅한다.
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_OK;
        m_pDbManager->GspGetUserInfo( szGspUserID, &nlfd2 );
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;					
		sc::writeLogError( sc::string::format("CAgentGspUserCheck result %d", nResult) );
		break;
	case 5 : // 중복접속
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_DUP;
		sc::writeLogError( sc::string::format("CAgentGspUserCheck result %d", nResult) );
		break;
	case 6 : // Block 된 상태
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_BLOCK;
		sc::writeLogError( sc::string::format("CAgentGspUserCheck result %d", nResult) );
		break;
	case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
	case GSP_ALLOW_TIME_ERROR:
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_EXPIRED;		
		sc::writeLogError( sc::string::format("CAgentGspUserCheck result %d", nResult) );
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	}

	pTemp->GspMsgLoginBack(&nlfd2);

	return NET_OK;
}

/**
 * TERRA : 사용자 로그인
 * \param szDecodedTID 
 * \param szUserIP 
 * \param nSvrGrp 
 * \param nSvrNum 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
CAgentTerraUserCheck::CAgentTerraUserCheck(
	const TCHAR* szDecodedTID,
    const std::string& UserIP, 
	int nSvrGrp,
	int nSvrNum,
	DWORD dwClient,
	DWORD dwClientNum )
{
	m_strDecodedTID = szDecodedTID;
	m_strUserIP     = UserIP.c_str();
	m_nSvrGrp       = nSvrGrp;
	m_nSvrNum       = nSvrNum;
	m_dwClientNum   = dwClientNum; // Agent 에서의 Client 번호
}

int CAgentTerraUserCheck::Execute(NetServer* pServer)
{
    CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL)
		return NET_ERROR;

	char szTLoginName[TERRA_TLOGIN_NAME+1] = {0};

	int nResult = m_pDbManager->TerraUserCheck(szTLoginName,
		                                                      m_strDecodedTID.GetString(),
															  m_strUserIP.GetString(),
															  m_nSvrGrp,
															  m_nSvrNum);

	int nResult1 = 0;

	EMSERVICE_PROVIDER sp = pTemp->GetServiceProvider();
	
//	IP보너스
//#if	defined( MYE_PARAM ) || defined( MY_PARAM )
	if (sp == SP_MALAYSIA || sp == SP_MALAYSIA_EN)
	{
		nResult1 = m_pDbManager->MyUserClassCheck( m_strUserIP.GetString() );
	}
	else if (sp == SP_PHILIPPINES)
	{
//#elif defined ( PH_PARAM ) 
		nResult1 = m_pDbManager->PhUserClassCheck( m_strUserIP.GetString() );	
	}
//#endif 

	// 전송할 구조체를 세팅한다.
	TERRA_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nType = TERRA_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	StringCchCopy(nlfd2.szTerraTLoginName, TERRA_TLOGIN_NAME+1, szTLoginName); // TLoginName
	StringCchCopy(nlfd2.szTerraDecodedTID, TERRA_TID_DECODE+1, m_strDecodedTID.GetString()); // Decoded TID
	StringCchCopy(nlfd2.szIp,	           MAX_IP_LENGTH+1,	  m_strUserIP.GetString()); // ip

	nlfd2.emIPBonus = (EMIP_BONUS_CLASS)nResult1;
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d ClientNum %d LoginName %s UserIp %s", nResult, m_dwClientNum, szTLoginName, m_strUserIP.GetString()) );
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 세팅한다.
        m_pDbManager->TerraGetUserInfo(szTLoginName, &nlfd2);
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_OK;
/*
#if	defined( MYE_PARAM ) || defined( MY_PARAM ) 
		{
			// 말레샤 ip차단 %%%
			// 로긴 성공후 현재 접속 가능한 아이피인지 체크한다.
			CTime time = m_pDbManager->GetCreateDate(nlfd2.nUserNum);
			// 2009/10/1일 이후 가입자는 IP 체크를 한다.
			if( time.GetYear() >= 2009 && time.GetMonth() >= 10 )
			{
				if( !pTemp->CheckConnectionIP(m_strUserIP.GetString()) )
				{
					// 로그아웃 처리
					m_pDbManager->UserLogoutSimple (nlfd2.szTerraTLoginName);
					nlfd2.nResult = EM_LOGIN_FB_SUB_IMPOSSIBLEIP;

						sc::writeLogError(_T("CAgentTerraUserCheck Login impossible IP %s"), m_strUserIP.GetString() );
				}
			}
		}
#endif
*/
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	case 5 : // 중복접속
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_DUP;
        m_pDbManager->TerraGetUserInfo(szTLoginName, &nlfd2);
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d ClientNum %d LoginName %s UserIp %s", nResult, m_dwClientNum, szTLoginName, m_strUserIP.GetString()) );
		break;
	case 6 : // Block 된 상태
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	}

	pTemp->TerraMsgLoginBack(&nlfd2);

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// 일반 : GS 로그인
CAgentGsUserCheck::CAgentGsUserCheck(
	const TCHAR* szUserID,
	const TCHAR* szPasswd, 
    const std::string& UserIP, 
	int nSvrGrp, 
	int nSvrNum,
	DWORD dwClient,
	DWORD dwClientNum,
	DWORD dwServiceProvider )
	: m_strUserID( szUserID )
	, m_strPasswd( szPasswd )	
	, m_nSvrGrp( nSvrGrp )
	, m_nSvrNum( nSvrNum )
	, m_dwClientNum( dwClientNum )
	, m_dwServiceProvider( dwServiceProvider )
{	
	m_ClientSlot = dwClient;
	m_strUserIP = UserIP;
}

int CAgentGsUserCheck::Execute(NetServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;

	int nRetCode = 0;	
	int nResult = m_pDbManager->GsUserCheck(m_strUserID.GetString(),
        m_strPasswd.GetString(),
        m_strUserIP.c_str(),
        m_nSvrGrp,
        m_nSvrNum);

	// 전송할 구조체를 세팅한다.
	GS_NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.nClient	= m_dwClientNum;

	StringCchCopy(NetMsgFB.szIp,	 MAX_IP_LENGTH+1, m_strUserIP.c_str()); // ip
	StringCchCopy(NetMsgFB.szUserid, USR_ID_LENGTH+1, m_strUserID.GetString()); // id


	switch (nResult)
	{
	case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentGsUserCheck result %d", nResult) );
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentGsUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)

	case 30 : // 로그인은 성공 + 2차비번 신규 설정 필요
	case 31 : // case 1, 2, 3, 7과 동일 + 2차비번 사용중
		// 사용자 정보를 가져온다.
		nRetCode = m_pDbManager->GsGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
			// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

			sc::writeLogError(std::string("CAgentGsUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
		else
		{
			if( nResult == 30 )
				NetMsgFB.m_emResult = EM_LOGIN_FB_KR_OK_NEW_PASS;
			else if( nResult == 31 )
				NetMsgFB.m_emResult = EM_LOGIN_FB_KR_OK_USE_PASS;
			else	
				NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_OK;

			// 아이템 변환 중인지 확인
			int Result = 0;
			m_pDbManager->GetItemTransferStatus( NetMsgFB.nUserNum, Result );
			NetMsgFB.ItemTransferResult = Result;
		}

		nRetCode = m_pDbManager->GetUserCountryInfo( NetMsgFB.nUserNum, NetMsgFB.sCountryInfo );
		if ( nRetCode == sc::db::DB_ERROR )
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;

			// 로그아웃 처리
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
			// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

			sc::writeLogError(std::string("CAgentGsUserCheck GetUserCountryInfo sc::db::DB_ERROR") );
		}
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError( sc::string::format("CAgentGsUserCheck result %d", nResult) );
		break;
	case 5 : // 중복접속
		// 사용자 정보를 가져온다.
		nRetCode = m_pDbManager->GsGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());

			sc::writeLogError(std::string("CAgentGsUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
        else
        {
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_DUP;			
			sc::writeLogError(
                sc::string::format(
                    "CAgentGsUserCheck result %d ClientNum %d LoginID %s UserIp %s",
                    nResult,
                    m_dwClientNum,
                    m_strUserID.GetString(),
                    m_strUserIP.c_str()));
		}
		break;
	case 6 : // Block 된 상태
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError( sc::string::format("CAgentGsUserCheck result %d", nResult) );
		break;
	case 7 : // EM_LOGIN_FB_SUB_RANDOM_PASS
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_RANDOM_PASS;		
		sc::writeLogError( sc::string::format("CAgentGsUserCheck result %d", nResult) );
		break;
#ifdef GS_PARAM
	case 23 : // EM_LOGIN_FB_SUB_BETAKEY
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_BETAKEY;		
		sc::writeLogError( sc::string::format("CAgentUserCheck result %d", nResult) );
		break;
#endif
	default : // 다른 모든경우 로그인 실패
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentGsUserCheck result %d", nResult) );
		break;
	}
	
	pTemp->GsMsgLoginBack( &NetMsgFB );

	return NET_OK;
}


///////////////////////////////////////////////////////////////////////////////
// 일반 : EU 로그인
CAgentGmUserCheck::CAgentGmUserCheck(
									 const TCHAR* szUserID,
									 const TCHAR* szPasswd, 
									 const std::string& UserIP, 
									 int nSvrGrp, 
									 int nSvrNum,
									 DWORD dwClient,
									 DWORD dwClientNum,
									 DWORD dwServiceProvider )
									 : m_strUserID( szUserID )
									 , m_strPasswd( szPasswd )	
									 , m_nSvrGrp( nSvrGrp )
									 , m_nSvrNum( nSvrNum )
									 , m_dwClientNum( dwClientNum )
									 , m_dwServiceProvider( dwServiceProvider )
{	
	m_ClientSlot = dwClient;
	m_strUserIP = UserIP;
}

int CAgentGmUserCheck::Execute( NetServer* pServer )
{
	CAgentServer* pAgentServer = reinterpret_cast< CAgentServer* >( pServer );

	if ( pAgentServer == NULL )
		return NET_ERROR;

	// 전송할 구조체를 세팅한다.
	GM_NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.nClient	= m_dwClientNum;

	//
	//HTTP BASIC AUTH를 통한 인증
	//
	std::string strUserNum;
	int nRetCode = 0;	

	int nBlock = 1;

	if ( !pAgentServer->GmHTTPAuth(m_strUserID, m_strPasswd, strUserNum, nBlock) )
	{
		// EU의 경우 GM아이디를 입력하면 란에서 사용하는 아이디를 받아오므로 변환해주어야 한다
		// ClientManager의 아이디의 경우 아래 GmMsgLoginBack() 함수에서 변환한다
		m_strUserID = strUserNum.c_str();
		StringCchCopy(NetMsgFB.szIp,	 MAX_IP_LENGTH+1, m_strUserIP.c_str()); // ip
		StringCchCopy(NetMsgFB.szUserid, USR_ID_LENGTH+1, m_strUserID.GetString()); // id

		if( nBlock != 0 )
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
			sc::writeLogError( sc::string::format("GmHTTPAuth Fail : CAgentGmUserCheck UserID %d", m_strUserID) );
		}
		else
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
			sc::writeLogError( sc::string::format("GmHTTPAuth Fail : CAgentGmUserCheck UserID %d", m_strUserID) );
		}
	}
	else
	{
		// EU의 경우 GM아이디를 입력하면 란에서 사용하는 아이디를 받아오므로 변환해주어야 한다
		// ClientManager의 아이디의 경우 아래 GmMsgLoginBack() 함수에서 변환한다
		m_strUserID = strUserNum.c_str();
		StringCchCopy(NetMsgFB.szIp,	 MAX_IP_LENGTH+1, m_strUserIP.c_str()); // ip
		StringCchCopy(NetMsgFB.szUserid, USR_ID_LENGTH+1, m_strUserID.GetString()); // id

		int nResult = m_pDbManager->GmUserCheck( m_strUserID.GetString(),
			m_strPasswd.GetString(),
			m_strUserIP.c_str(),
			m_nSvrGrp,
			m_nSvrNum );

		switch (nResult)
		{
		case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
			sc::writeLogError( sc::string::format("CAgentGmUserCheck result %d", nResult) );
			break;
		case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
			sc::writeLogError( sc::string::format("CAgentGmUserCheck result %d", nResult) );
			break;
		case 1 :
		case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
		case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
			// 사용자 정보를 가져온다.
			nRetCode = m_pDbManager->GmGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
			if (nRetCode == sc::db::DB_ERROR) // 가져오기 실패
			{
				NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
				// 로그아웃 처리
				m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
				// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

				sc::writeLogError(std::string("CAgentGmUserCheck GetUserInfo sc::db::DB_ERROR") );
			}
			else
			{
				NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_OK;
			}
			break;
		case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
			sc::writeLogError( sc::string::format("CAgentGmUserCheck result %d", nResult) );
			break;
		case 5 : // 중복접속
			// 사용자 정보를 가져온다.
			nRetCode = m_pDbManager->GmGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
			if (nRetCode == sc::db::DB_ERROR) // 가져오기 실패
			{
				NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
				// 로그아웃 처리
				m_pDbManager->UserLogoutSimple (m_strUserID.GetString());

				sc::writeLogError(std::string("CAgentGmUserCheck GetUserInfo sc::db::DB_ERROR") );
			}
			else
			{
				NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_DUP;			
				sc::writeLogError(
					sc::string::format(
					"CAgentGmUserCheck result %d ClientNum %d LoginID %s UserIp %s",
					nResult,
					m_dwClientNum,
					m_strUserID.GetString(),
					m_strUserIP.c_str()));
			}
			break;
		case 6 : // Block 된 상태
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
			sc::writeLogError( sc::string::format("CAgentGmUserCheck result %d", nResult) );
			break;
		case 7 : // EM_LOGIN_FB_SUB_RANDOM_PASS
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_RANDOM_PASS;		
			sc::writeLogError( sc::string::format("CAgentGmUserCheck result %d", nResult) );
			break;
#ifdef GS_PARAM
		case 23 : // EM_LOGIN_FB_SUB_BETAKEY
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_BETAKEY;		
			sc::writeLogError( sc::string::format("CAgentUserCheck result %d", nResult) );
			break;
#endif
		default : // 다른 모든경우 로그인 실패
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
			sc::writeLogError( sc::string::format("CAgentGmUserCheck result %d", nResult) );
			break;
		}
	}

	pAgentServer->GmMsgLoginBack( &NetMsgFB );

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// 일반 : US 로그인
CAgentUsUserCheck::CAgentUsUserCheck(
									 const TCHAR* szUserID,
									 const TCHAR* szPasswd, 
									 const std::string& UserIP, 
									 int nSvrGrp, 
									 int nSvrNum,
									 DWORD dwClient,
									 DWORD dwClientNum,
									 DWORD dwServiceProvider )
									 : m_strUserID( szUserID )
									 , m_strPasswd( szPasswd )	
									 , m_nSvrGrp( nSvrGrp )
									 , m_nSvrNum( nSvrNum )
									 , m_dwClientNum( dwClientNum )
									 , m_dwServiceProvider( dwServiceProvider )
{	
	m_ClientSlot = dwClient;
	m_strUserIP = UserIP;
}

int CAgentUsUserCheck::Execute( NetServer* pServer )
{
	CAgentServer* pTemp = reinterpret_cast< CAgentServer* >( pServer );

	if ( pTemp == NULL )
		return NET_ERROR;

	int nRetCode = 0;	
	int nResult = m_pDbManager->UsUserCheck( m_strUserID.GetString(),
		m_strPasswd.GetString(),
		m_strUserIP.c_str(),
		m_nSvrGrp,
		m_nSvrNum );

	// 전송할 구조체를 세팅한다.
	US_NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.nClient	= m_dwClientNum;

	StringCchCopy(NetMsgFB.szIp,	 MAX_IP_LENGTH+1, m_strUserIP.c_str()); // ip
	StringCchCopy(NetMsgFB.szUserid, USR_ID_LENGTH+1, m_strUserID.GetString()); // id


	switch (nResult)
	{
	case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentUsUserCheck result %d", nResult) );
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentUsUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 가져온다.
		nRetCode = m_pDbManager->UsGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
			// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

			sc::writeLogError(std::string("CAgentUsUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
		else
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_OK;

			// 아이템 변환 중인지 확인
			int Result = 0;
			m_pDbManager->GetItemTransferStatus( NetMsgFB.nUserNum, Result );
			NetMsgFB.ItemTransferResult = Result;
		}
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError( sc::string::format("CAgentUsUserCheck result %d", nResult) );
		break;
	case 5 : // 중복접속
		// 사용자 정보를 가져온다.
		nRetCode = m_pDbManager->UsGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());

			sc::writeLogError(std::string("CAgentUsUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
		else
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_DUP;			
			sc::writeLogError(
				sc::string::format(
				"CAgentUsUserCheck result %d ClientNum %d LoginID %s UserIp %s",
				nResult,
				m_dwClientNum,
				m_strUserID.GetString(),
				m_strUserIP.c_str()));
		}
		break;
	case 6 : // Block 된 상태
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError( sc::string::format("CAgentUsUserCheck result %d", nResult) );
		break;
	case 7 : // EM_LOGIN_FB_SUB_RANDOM_PASS
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_RANDOM_PASS;		
		sc::writeLogError( sc::string::format("CAgentUsUserCheck result %d", nResult) );
		break;
#ifdef GS_PARAM
	case 23 : // EM_LOGIN_FB_SUB_BETAKEY
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_BETAKEY;		
		sc::writeLogError( sc::string::format("CAgentUserCheck result %d", nResult) );
		break;
#endif
	default : // 다른 모든경우 로그인 실패
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentUsUserCheck result %d", nResult) );
		break;
	}

	pTemp->UsMsgLoginBack( &NetMsgFB );

	return NET_OK;
}

/**
 * Daum : 사용자 Password Check
 * \param szUserGID
 * \param szUserPass 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
CAgentDaumPassCheck::CAgentDaumPassCheck(
			const TCHAR* szDaumGID,
			const TCHAR* szUserPass,
			int nCheckFlag,
			DWORD dwClient,
			DWORD dwClientNum )

{
	m_strDaumGID	= szDaumGID;
	m_strUserPass	= szUserPass;
	m_nCheckFlag	= nCheckFlag;
	m_dwClientNum	= dwClientNum;
}

int CAgentDaumPassCheck::Execute(NetServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if( pTemp == NULL ) return NET_ERROR;

	int nResult = m_pDbManager->DaumUserPassCheck(	m_strDaumGID.GetString(), 
																	m_strUserPass.GetString(),
																	m_nCheckFlag);
		

	// 전송할 구조체를 세팅한다.
	DAUM_NET_PASSCHECK_FEEDBACK_DATA2 nlfd2;
	
	nlfd2.nClient = m_dwClientNum;
	StringCchCopy( nlfd2.szDaumGID, DAUM_MAX_GID_LENGTH+1, m_strDaumGID.GetString() );

	switch(nResult)
	{
	case sc::db::DB_ERROR : // DB에러로 로그인 실패 (-1)
		nlfd2.nResult = sc::db::DB_ERROR;		
		sc::writeLogError( sc::string::format("CAgentDaumPassCheck result %d", nResult) );
		break;	
	case 0 :
		// 사용자 정보를 세팅한다. (0)
		nlfd2.nResult = EM_LOGIN_FB_SUB_OK;
		break;
	case 1 : // ID / PWD 불일치 (로그인 실패) (1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentDaumUserCheck result %d", nResult) );
		break;
	case 2 : // PW 입력 실패 (2) 
		nlfd2.nResult = EM_LOGIN_FB_SUB_PASS_OK;
		break;			
	default : // 다른 모든경우 로그인 실패(1)
		nlfd2.nResult = sc::db::DB_ERROR;		
		sc::writeLogError( sc::string::format("CAgentDaumPassCheck result %d", nResult) );
		break;
	}

	pTemp->DaumMsgPassCheckBack(&nlfd2);

	return NET_OK;
}


/**
 * TERRA : 사용자 Password Check
 * \param szTLoginName
 * \param szUserPass 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
CAgentTerraPassCheck::CAgentTerraPassCheck(
			const TCHAR* szTLoginName,
			const TCHAR* szUserPass,
			int nCheckFlag,
			DWORD dwClient,
			DWORD dwClientNum )

{
	m_strTLoginName		= szTLoginName;
	m_strUserPass		= szUserPass;
	m_nCheckFlag		= nCheckFlag;
	m_dwClientNum		= dwClientNum;
}

int CAgentTerraPassCheck::Execute(NetServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if( pTemp == NULL ) return NET_ERROR;

	int nResult = m_pDbManager->TerraUserPassCheck( m_strTLoginName.GetString(),
																   m_strUserPass.GetString(),
																   m_nCheckFlag );

	// 전송할 구조체를 세팅한다.
	TERRA_NET_PASSCHECK_FEEDBACK_DATA2 nlfd2;
	nlfd2.nClient	= m_dwClientNum;	

	StringCchCopy(nlfd2.szTerraTLoginName, TERRA_TLOGIN_NAME+1, m_strTLoginName.GetString()); // TLoginName
	
	switch(nResult)
	{
	case sc::db::DB_ERROR : // DB에러로 패스워드 업데이트 실패 (-1)
		nlfd2.nResult = sc::db::DB_ERROR;		
		sc::writeLogError( sc::string::format("CAgentTerraPassCheck result %d", nResult) );
		break;	
	case 0 :
		// 사용자 정보를 세팅한다. (0)
		nlfd2.nResult = EM_LOGIN_FB_SUB_OK;
		break;
	case 1 : // ID / PWD 불일치 (로그인 실패) (1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	case 2 : // 초기 패스워드 업데이트 성공
		nlfd2.nResult = EM_LOGIN_FB_SUB_PASS_OK;
		break;			
	default : // 다른 모든경우 로그인 실패(1)
		nlfd2.nResult = sc::db::DB_ERROR;		
		sc::writeLogError( sc::string::format("CAgentTerraPassCheck result %d", nResult) );
		break;
	}

	pTemp->TerraMsgPassCheckBack(&nlfd2);

	return NET_OK;
}


/**
* Excite 사용자 체크
*/
CAgentExciteUserCheck::CAgentExciteUserCheck(
    const char* szUserID,
    const std::string& UserIP,
    int nSvrGrp,
    int nSvrNum,
    DWORD dwClient,
    DWORD dwClientNum)
{
	m_strUserID = szUserID;
	m_strUserIP = UserIP.c_str();
	m_nSvrGrp = nSvrGrp;
	m_nSvrNum = nSvrNum;

	m_dwClientNum = dwClientNum; // Agent 에서의 Client 번호
}

int CAgentExciteUserCheck::Execute(NetServer* pServer)
{	
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;	

	int nResult = m_pDbManager->ExciteUserCheck(m_strUserID.GetString(),
															 m_strUserIP.GetString(),
															 m_nSvrGrp,
															 m_nSvrNum);
	// 전송할 구조체를 세팅한다.
	EXCITE_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nType = EXCITE_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	::StringCchCopy(nlfd2.szExciteUserID, EXCITE_USER_ID+1, m_strUserID.GetString()); // id
	::StringCchCopy(nlfd2.szIp,	          MAX_IP_LENGTH+1,	m_strUserIP.GetString()); // ip
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 세팅한다.
		nlfd2.m_emResult      = EM_LOGIN_FB_SUB_OK;
        m_pDbManager->ExciteGetUserInfo(m_strUserID.GetString(), &nlfd2);
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 5 : // 중복접속
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_DUP;
		// 중복된 접속이라도 유저 정보를 가져와서 셋팅한다.
		m_pDbManager->ExciteGetUserInfo(m_strUserID.GetString(), &nlfd2);		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 6 : // Block 된 상태
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	}

	pTemp->ExciteMsgLoginBack(&nlfd2);
	
	return NET_OK;
}


/**
* 일본 Gonzo 사용자 체크
*/
CAgentJapanUserCheck::CAgentJapanUserCheck(
    const char* szUserID,
    const std::string& UserIP,
    int nSvrGrp,
    int nSvrNum,
    DWORD dwClient,
    DWORD dwClientNum,
    int nUserNum)
{
	m_strUserID = szUserID;
	m_strUserIP = UserIP.c_str();
	m_nSvrGrp = nSvrGrp;
	m_nSvrNum = nSvrNum;

	m_dwClientNum = dwClientNum; // Agent 에서의 Client 번호
	m_nUserNum = nUserNum;		// WhiteRock System에서의 UserNum
}

int CAgentJapanUserCheck::Execute(NetServer* pServer)
{	
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	if (!pTemp)
		return NET_ERROR;

	int nResult = m_pDbManager->JapanUserCheck(
		m_strUserID.GetString(),
		m_strUserIP.GetString(),
		m_nSvrGrp,
		m_nSvrNum,
		m_nUserNum);
	// 전송할 구조체를 세팅한다.
	JAPAN_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nType = JAPAN_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	::StringCchCopy(nlfd2.JapanUserID,	  JAPAN_USER_ID+1, m_strUserID.GetString()); // id
	::StringCchCopy(nlfd2.szIp,	          MAX_IP_LENGTH+1,	m_strUserIP.GetString()); // ip
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentJapanUserCheck result %d", nResult) );
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentJapaneUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 세팅한다.
		nlfd2.m_emResult      = EM_LOGIN_FB_SUB_OK;
        m_pDbManager->JapanGetUserInfo(m_strUserID.GetString(), &nlfd2);
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError( sc::string::format("CAgentJapanUserCheck result %d", nResult) );
		break;
	case 5 : // 중복접속
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_DUP;
		// 중복된 접속이라도 유저 정보를 가져와서 셋팅한다.
		m_pDbManager->JapanGetUserInfo(m_strUserID.GetString(), &nlfd2);		
		sc::writeLogError( sc::string::format("CAgentJapanUserCheck result %d", nResult) );
		break;
	case 6 : // Block 된 상태
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError( sc::string::format("CAgentJapanUserCheck result %d", nResult) );
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentJapanUserCheck result %d", nResult) );
		break;
	}

	pTemp->JapanMsgLoginBack(&nlfd2);
	
	return NET_OK;
}

/**
 * EXCITE : 사용자 Password Check
 * \param szExciteUserID
 * \param szUserPass 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
CAgentExcitePassCheck::CAgentExcitePassCheck(
			const TCHAR* szExciteUserID,
			const TCHAR* szExciteUserID2,
			const TCHAR* szUserPass,
			int nCheckFlag,
			DWORD dwClient,
			DWORD dwClientNum )

{
	m_strExciteUserID	= szExciteUserID;
	m_strExciteUserID2	= szExciteUserID2;
	m_strUserPass		= szUserPass;
	m_nCheckFlag		= nCheckFlag;
	m_dwClientNum		= dwClientNum;
}

int CAgentExcitePassCheck::Execute(NetServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (!pTemp)
        return NET_ERROR;

	int len		= m_strExciteUserID2.GetLength();
	int len1	= m_strUserPass.GetLength();

	for (int i = 0; i < len; i++)
	{
		m_strExciteUserID2.SetAt(i, m_strExciteUserID2.GetAt(i) + 1);
	}

	for (int i = 0; i < len1; i++)
	{
		m_strUserPass.SetAt(i, m_strUserPass.GetAt(i) + 1);
	}

	int nResult = m_pDbManager->ExciteUserPassCheck( m_strExciteUserID.GetString(),
									  m_strExciteUserID2.GetString(),
									  m_strUserPass.GetString(),
									  m_nCheckFlag );

	// 전송할 구조체를 세팅한다.
	EXCITE_NET_PASSCHECK_FEEDBACK_DATA2 nlfd2;
	nlfd2.nClient	= m_dwClientNum;	

	StringCchCopy(nlfd2.szExciteUserID, EXCITE_USER_ID+1, m_strExciteUserID.GetString()); // UserID
	
	switch(nResult)
	{
	case sc::db::DB_ERROR : // DB에러로 패스워드 업데이트 실패 (-1)
		nlfd2.nResult = sc::db::DB_ERROR;		
		sc::writeLogError( sc::string::format("CAgentExcitePassCheck result %d", nResult) );
		break;	
	case 0 :
		// 사용자 정보를 세팅한다. (0)
		nlfd2.nResult = EM_LOGIN_FB_SUB_OK;
		break;
	case 1 : // ID / PWD 불일치 (로그인 실패) (1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 2 : // 초기 패스워드 업데이트 성공
		nlfd2.nResult = EM_LOGIN_FB_SUB_PASS_OK;
		break;			
	case 3 : // 초기 패스워드 업데이트 성공
		nlfd2.nResult = EM_LOGIN_FB_SUB_SECID_ALREADY;
		break;			
	default : // 다른 모든경우 로그인 실패(1)
		nlfd2.nResult = sc::db::DB_ERROR;		
		sc::writeLogError( sc::string::format("CAgentExcitePassCheck result %d", nResult) );
		break;
	}

	pTemp->ExciteMsgPassCheckBack(&nlfd2);

	return NET_OK;
}



CUserLogoutSvr::CUserLogoutSvr(
	int nSvrGrp,
	int nSvrNum )
	: m_nSvrGrp( nSvrGrp )
	, m_nSvrNum( nSvrNum )
{	
}

int CUserLogoutSvr::Execute(NetServer* pServer)
{
	return m_pDbManager->UserLogoutSvr(m_nSvrGrp, m_nSvrNum);
}

CUserBlock::CUserBlock(
	int nUserNum,
	int nDay )
	: m_nUserNum( nUserNum )
	, m_nDay( nDay )
{
}

int CUserBlock::Execute(NetServer* pServer)
{
	return m_pDbManager->UserBlock( m_nUserNum, m_nDay );
}

CUserBlockDate::CUserBlockDate(
	int nUserNum,
	__time64_t tBlockTime )
	: m_nUserNum( nUserNum)
{
	m_Date = tBlockTime;
}

int CUserBlockDate::Execute(NetServer* pServer)
{
    return m_pDbManager->UserBlockDate( m_nUserNum, m_Date );
}

UserDbLogItemDataInsertUpdate::UserDbLogItemDataInsertUpdate(
    int ItemMid,
    int ItemSid,
    const std::string& ItemName)
    : m_ItemMid(ItemMid)
    , m_ItemSid(ItemSid)
    , m_ItemName(ItemName)
{
}

UserDbLogItemDataInsertUpdate::~UserDbLogItemDataInsertUpdate()
{
}

int UserDbLogItemDataInsertUpdate::Execute(NetServer* pServer)
{
    return m_pDbManager->UserDbLogItemDataInsertUpdate(m_ItemMid, m_ItemSid, m_ItemName);
}

UserSystemInfoInsert::UserSystemInfoInsert( 
	DWORD dwUserNum, 
	DWORD dwCpuCoreNumber, 
	DWORD dwCpuClock, 
	DWORDLONG dwMemorySize,
	DWORD dwResolutionX,
	DWORD dwResolutionY,
	DWORD dwGpuVendor,
	DWORD dwGpuModel,
	DWORD dwShaderModel,
	int languageCode )
{
	m_dwUserNum = dwUserNum;
	m_dwCpuCoreNumber = dwCpuCoreNumber;
	m_dwCpuClock = dwCpuClock;
	m_dwMemorySize = dwMemorySize;
	m_dwResolutionX = dwResolutionX;
	m_dwResolutionY = dwResolutionY;
	m_dwGpuVendor = dwGpuVendor;		// 0 - Etc, 1 - Intel, 2 - Nvidia, 3 - AMD
	m_dwGpuModel = dwGpuModel;
	m_dwShaderModel = dwShaderModel;
	m_languageCode = languageCode;
}

int UserSystemInfoInsert::Execute( NetServer* pServer )
{
	if ( db::ADO != m_pDbManager->GetConnectionType() )
	{
		sc::writeLogError( "UserSystemInfoInsert::Execute. Use ADO" );
		return NET_ERROR;
	}

	return m_pDbManager->UserSystemInfoInsert( m_dwUserNum, m_dwCpuCoreNumber, m_dwCpuClock, m_dwMemorySize, m_dwResolutionX, m_dwResolutionY, m_dwGpuVendor, m_dwGpuModel, m_dwShaderModel, m_languageCode );
}

int UserWriteGMLog::Execute( NetServer* pServer )
{
    return m_pDbManager->WriteGMLog(nGmNum, strCmd, strUserIP);
    
}
