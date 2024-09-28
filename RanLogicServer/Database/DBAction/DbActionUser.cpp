#include "../../pch.h"
#include "../../RanLogic/Msg/LoginMsg.h"
#include "DbActionUser.h"

#include "../../Server/AgentServer.h"

#include <boost/tuple/tuple.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Agent ���� : ĳ���� �������� 1 ����
CAgentUserChaNumDecrease::CAgentUserChaNumDecrease(int nUserNum)
{
    m_nUserNum = nUserNum;
}

int CAgentUserChaNumDecrease::Execute(NetServer* pServer)
{
    return m_pDbManager->UserChaNumDecrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// TEST Session ���� : ĳ���� �������� 1 ����
CUserTestChaNumDecrease::CUserTestChaNumDecrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CUserTestChaNumDecrease::Execute(NetServer* pServer)
{
	return m_pDbManager->UserTestChaNumDecrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// TEST Agent ���� : ĳ���� �������� 1 ����
CAgentUserTestChaNumDecrease::CAgentUserTestChaNumDecrease(int nUserNum)
{
    m_nUserNum = nUserNum;
}

int CAgentUserTestChaNumDecrease::Execute(NetServer* pServer)
{
    return m_pDbManager->UserTestChaNumDecrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// Session : ������ �� �ִ� ĳ���� �� 1 ����
CUserChaNumIncrease::CUserChaNumIncrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CUserChaNumIncrease::Execute(NetServer* pServer)
{
    return m_pDbManager->UserChaNumIncrease(m_nUserNum);
}


///////////////////////////////////////////////////////////////////////////////
// Session ���� : ĳ���� �������� 1 ����
CUserChaNumDecrease::CUserChaNumDecrease(int nUserNum) 
{ 
	m_nUserNum = nUserNum;
}

int CUserChaNumDecrease::Execute(NetServer* pServer)
{
	return m_pDbManager->UserChaNumDecrease(m_nUserNum);
}


///////////////////////////////////////////////////////////////////////////////
// Agent : ������ �� �ִ� ĳ���� �� 1 ����
CAgentUserChaNumIncrease::CAgentUserChaNumIncrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CAgentUserChaNumIncrease::Execute(NetServer* pServer)
{
    return m_pDbManager->UserChaNumIncrease(m_nUserNum);
}


///////////////////////////////////////////////////////////////////////////////
// TEST Session : ������ �� �ִ� ĳ���� �� 1 ����
CUserTestChaNumIncrease::CUserTestChaNumIncrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CUserTestChaNumIncrease::Execute(NetServer* pServer)
{
	return m_pDbManager->UserTestChaNumIncrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// TEST Agent : ������ �� �ִ� ĳ���� �� 1 ����
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
// �Ϻ� WhiteRock���� �α��� ���
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
		// WhiteRock ������ ��� ����
		pTemp->JapanWRFailed( m_dwClientNum, nResult );
	}
	else
	{
		const CString& strUUID   = CWhiteRock::GetInstance()->GetUUID();
		int nUserNum = CWhiteRock::GetInstance()->GetUserNum();
		// WhiteRock ������ ��� ����
		pTemp->JapanWRSuccess( m_dwClientNum, m_strUserID,
									m_nServerGroup, m_nServerNum, strUUID, nUserNum );
	}

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// �Ϻ� WhiteRock���� �α׾ƿ� ���
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
 * ����� üũ
 * Thailand (�±�)
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

	// ������ ����ü�� �����Ѵ�.
	NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.m_ClientSlot   = m_dwClientNum;

	StringCchCopy (NetMsgFB.szIp, MAX_IP_LENGTH+1, m_strUserIP.c_str()); // ip
	StringCchCopy (NetMsgFB.szUserid, USR_ID_LENGTH+1, m_strUserID.GetString()); // id

	NetMsgFB.emIPBonus = (EMIP_BONUS_CLASS) nResult1;
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
		break;
	case 0 : // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)	
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;
		break;
	case 1 :
	case 2 : // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��	
	case 3 : // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)
		// ����� ������ �����´�.
		nRetCode = m_pDbManager->ThaiGetUserInfo( m_strUserID.GetString(), &NetMsgFB );
		if (nRetCode == sc::db::DB_ERROR) // �������� ����
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// �α׾ƿ� ó��
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
			// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

			sc::writeLogError(std::string("CAgentUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
		else
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_OK;

			// ������ ��ȯ ������ Ȯ��
			int Result = 0;
			m_pDbManager->GetItemTransferStatus( NetMsgFB.nUserNum, Result );
			NetMsgFB.ItemTransferResult = Result;
		}
		break;
	case 4 : // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;
		break;
	case 5 : // �ߺ�����
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_DUP;
		break;
	case 6 : // Block �� ����
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_BLOCK;
		break;	
	case 7 : // 18 ���̸� 3 �ð� ���
		NetMsgFB.m_emResult = EM_LOGIN_FB_THAI_UNDER18_3HOUR;
		break;
	case 8 : // 18 ���̸� ���ӺҰ��� �ð�
		NetMsgFB.m_emResult = EM_LOGIN_FB_THAI_UNDER18_TIME;
		break;
	case 9 : // 18 ���̻� ���ӺҰ��� �ð�
		NetMsgFB.m_emResult = EM_LOGIN_FB_THAI_OVER18_TIME;
		break;
	default : // �ٸ� ����� �α��� ����
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;
		break;
	}

	pTemp->ThaiMsgLogInBack( &NetMsgFB );

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// �Ϲ� : ����� �α���
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

	// ������ ����ü�� �����Ѵ�.
	NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.m_ClientSlot	= m_dwClientNum;

	StringCchCopy(NetMsgFB.szIp,	 MAX_IP_LENGTH+1, m_strUserIP.c_str()); // ip
	StringCchCopy(NetMsgFB.szUserid, USR_ID_LENGTH+1, m_strUserID.GetString()); // id

	// ���� �������� �±� User Class Type üũ�� ���ؼ� �߰�
	//	IP���ʽ�
/*
#if defined(_RELEASED) 
	int nResult1 = m_pDbManager->PhUserClassCheck( m_strUserIP.GetString() );	
	NetMsgFB.emIPBonus = (EMIP_BONUS_CLASS)nResult1;
#endif 
*/	
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError(sc::string::format("CAgentUserCheck result %1% %2%", nResult, m_strUserID.GetString()));
		break;
	case 0 : // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)	
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError(sc::string::format("CAgentUserCheck result %1% %2% %3%", nResult, m_strUserID.GetString(),m_strUserIP.c_str()));
		break;
	case 1 :
	case 2 : // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��	
	case 3 : // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)
	case 7 : // ID/PWD/������й�ȣ ��ġ

		//
		//mjeon.ReturnOfHero
		//
	case 11 : // case 1�� ���� + ReturnOfHero event
	case 12 : // case 2�� ���� + ReturnOfHero event
	case 13 : // case 3�� ���� + ReturnOfHero event
	case 17 : // case 7�� ���� + ReturnOfHero event

	case 30 : // �α����� ���� + 2����� �ű� ���� �ʿ�
	case 31 : // case 1, 2, 3, 7�� ���� + 2����� �����

		// ����� ������ �����´�.
		nRetCode = m_pDbManager->GetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // �������� ����
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// �α׾ƿ� ó��
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

			// ������ ��ȯ ������ Ȯ��
			int Result = 0;
			m_pDbManager->GetItemTransferStatus( NetMsgFB.nUserNum, Result );
			NetMsgFB.ItemTransferResult = Result;
		}	

		nRetCode = m_pDbManager->GetUserCountryInfo( NetMsgFB.nUserNum, NetMsgFB.sCountryInfo );
		if ( nRetCode == sc::db::DB_ERROR )
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;

			// �α׾ƿ� ó��
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
			// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

			sc::writeLogError(std::string("CAgentUserCheck GetUserCountryInfo sc::db::DB_ERROR") );
		}
		break;
	case 4 : // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError(sc::string::format("CAgentUserCheck result %1% %2%", nResult, m_strUserID.GetString()));
		break;
	case 5 : // �ߺ�����
		// ����� ������ �����´�.
		nRetCode = m_pDbManager->GetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // �������� ����
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// �α׾ƿ� ó��
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
	case 6 : // Block �� ����
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
	default : // �ٸ� ����� �α��� ����
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
	�ε��׽þ� - ������ Login ��ŷ ���� ������ ���� �ӽù������� Ban �ڵ� ����

	�ٺ����� ������ �ذ��ϱ� ���ؼ��� VTC�� eBank�ܿ��� ó�����־�� �Ѵ�.
	*/
#define IDN_LOGIN_BAN_FAILURE_LIMIT		(3)				//3�� ���н� Ban
#define IDN_LOGIN_BAN_TIME_LIMIT		(180000)		//3��
	
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
	//HTTP�� �̿��ؼ� VTC ����
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

	case 0: // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)
		msgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;
		sc::writeLogError(sc::string::format("DIdnUserCheck result %d", nResult));
		break;

	case 1:
	case 2: // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��
	case 3: // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)
		nRetCode = m_pDbManager->IdnGetUserInfo(m_strId, OUT &msgFB);
		
		if (nRetCode == sc::db::DB_ERROR)
		{
			msgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			
			// �α׾ƿ� ó��
			m_pDbManager->UserLogoutSimple(m_strId.c_str());

			sc::writeLogError(std::string("DIdnUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
		else
		{
			msgFB.m_emResult = EM_LOGIN_FB_SUB_OK;			
		}	
		break;

	case 4: // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
		msgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;
		sc::writeLogError( sc::string::format("DIdnUserCheck result %d", nResult) );
		break;

	case 5: // �ߺ�����
		nRetCode = m_pDbManager->IdnGetUserInfo(m_strId, OUT &msgFB);
		
		if (nRetCode == sc::db::DB_ERROR)
		{
			msgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			
			// �α׾ƿ� ó��
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

	case 6: // Block �� ����
		msgFB.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError( sc::string::format("DIdnUserCheck result %d", nResult));
		break;

	default: // �ٸ� ����� �α��� ����
		msgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("DIdnUserCheck result %d", nResult) );
		break;
	}
	
	pAgentServer->IdnMsgLogInBack(&msgFB);

	return NET_OK;
}

/*
///////////////////////////////////////////////////////////////////////////////
// DAUM : ����� �α���
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
	m_dwClientNum		= dwClientNum; // Agent Ŭ���̾�Ʈ ��ȣ
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

	// ������ ����ü�� �����Ѵ�.
	DAUM_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nmg.nType = DAUM_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	::StringCchCopy(nlfd2.szDaumGID, DAUM_MAX_GID_LENGTH+1, m_strDaumGID.GetString());
	::StringCchCopy(nlfd2.szDaumUID, DAUM_MAX_UID_LENGTH+1, m_strDaumUID.GetString());
	::StringCchCopy(nlfd2.szIp,	     MAX_IP_LENGTH+1,	    m_strUserIP.GetString()); // ip	
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	case 0 : // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)	
		nlfd2.nResult = EM_LOGIN_FB_SUB_INCORRECT;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	case 1 :
	case 2 : // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��	
	case 3 : // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)
		// ����� ������ �����Ѵ�.
		nlfd2.nResult      = EM_LOGIN_FB_SUB_OK;
        m_pDbManager->DaumGetUserInfo(m_strDaumUID.GetString(), &nlfd2);
		break;
	case 4 : // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
		nlfd2.nResult = EM_LOGIN_FB_SUB_IP_BAN;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	case 5 : // �ߺ�����
		nlfd2.nResult = EM_LOGIN_FB_SUB_DUP;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	case 6 : // Block �� ����
		nlfd2.nResult = EM_LOGIN_FB_SUB_BLOCK;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	case 7 : // ������ �����մϴ�. ���� ���Խ���
	default : // �ٸ� ����� �α��� ����
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;
		sc::writeLogError("CAgentDaumUserCheck result %d", nResult);
		break;
	}

	pTemp->DaumMsgLoginBack(&nlfd2);
	return NET_OK;
}
*/
///////////////////////////////////////////////////////////////////////////////
// DAUM : ����� �α���
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
	case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentDaumUserCheck result %d", nResult) );
		break;
	case 0 : // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)	
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentDaumUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��	
	case 3 : // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)

	case 11 : // case 1: + Return Of Hero
	case 12 : // case 2: + Return Of Hero
	case 13 : // case 3: + Return Of Hero

	case 30 : // �α����� ���� + 2����� �ű� ���� �ʿ�
	case 31 : // case 1, 2, 3, 7�� ���� + 2����� �����

		// ����� ������ �����Ѵ�.
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_OK;

		if( m_pDbManager->DaumGetUserInfo(szKorUserID, &nlfd2) == sc::db::DB_ERROR ) // �������� ����
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

			// ������ ��ȯ ������ Ȯ��
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
	case 4 : // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	case 5 : // �ߺ�����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_DUP;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	case 6 : // Block �� ����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	case 20 : // Web ���� �ð� �ʰ� 
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_EXPIRED;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	case 21 : // UUID�� web�ߺ� �α������� ���� �����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_UID_ERR;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	case 7 : // ������ �����մϴ�. ���� ���Խ���
	default : // �ٸ� ����� �α��� ����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError(sc::string::format("CAgentDaumUserCheck result %d", nResult));
		break;
	}

	pTemp->DaumMsgLoginBack(&nlfd2);
	return NET_OK;
}

/**
 * GSP : ����� �α���
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
	m_dwClientNum = dwClientNum; // Agent ������ Client ��ȣ
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

	// ������ ����ü�� �����Ѵ�.
	GSP_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nType = GSP_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	StringCchCopy( nlfd2.szGspUserID, GSP_USERID, szGspUserID ); // ID	
	StringCchCopy( nlfd2.szIp, GSP_USERIP, m_strUserIP.GetString() ); // ip
	StringCchCopy( nlfd2.szUUID, sc::string::UUID_LENGTH, m_strUUID.GetString() ); // UUID
	
	switch (nResult)
	{
	/*
	case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		sc::writeLogError(
			_T("CAgentGspUserCheck result %d"), 
			nResult);
		break;
	*/
	case 0 : // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)	
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentGspUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��	
	case 3 : // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)
		// ����� ������ �����Ѵ�.
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_OK;
        m_pDbManager->GspGetUserInfo( szGspUserID, &nlfd2 );
		break;
	case 4 : // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;					
		sc::writeLogError( sc::string::format("CAgentGspUserCheck result %d", nResult) );
		break;
	case 5 : // �ߺ�����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_DUP;
		sc::writeLogError( sc::string::format("CAgentGspUserCheck result %d", nResult) );
		break;
	case 6 : // Block �� ����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_BLOCK;
		sc::writeLogError( sc::string::format("CAgentGspUserCheck result %d", nResult) );
		break;
	case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
	case GSP_ALLOW_TIME_ERROR:
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_EXPIRED;		
		sc::writeLogError( sc::string::format("CAgentGspUserCheck result %d", nResult) );
		break;
	case 7 : // ������ �����մϴ�. ���� ���Խ���
	default : // �ٸ� ����� �α��� ����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	}

	pTemp->GspMsgLoginBack(&nlfd2);

	return NET_OK;
}

/**
 * TERRA : ����� �α���
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
	m_dwClientNum   = dwClientNum; // Agent ������ Client ��ȣ
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
	
//	IP���ʽ�
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

	// ������ ����ü�� �����Ѵ�.
	TERRA_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nType = TERRA_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	StringCchCopy(nlfd2.szTerraTLoginName, TERRA_TLOGIN_NAME+1, szTLoginName); // TLoginName
	StringCchCopy(nlfd2.szTerraDecodedTID, TERRA_TID_DECODE+1, m_strDecodedTID.GetString()); // Decoded TID
	StringCchCopy(nlfd2.szIp,	           MAX_IP_LENGTH+1,	  m_strUserIP.GetString()); // ip

	nlfd2.emIPBonus = (EMIP_BONUS_CLASS)nResult1;
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d ClientNum %d LoginName %s UserIp %s", nResult, m_dwClientNum, szTLoginName, m_strUserIP.GetString()) );
		break;
	case 0 : // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��	
	case 3 : // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)
		// ����� ������ �����Ѵ�.
        m_pDbManager->TerraGetUserInfo(szTLoginName, &nlfd2);
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_OK;
/*
#if	defined( MYE_PARAM ) || defined( MY_PARAM ) 
		{
			// ������ ip���� %%%
			// �α� ������ ���� ���� ������ ���������� üũ�Ѵ�.
			CTime time = m_pDbManager->GetCreateDate(nlfd2.nUserNum);
			// 2009/10/1�� ���� �����ڴ� IP üũ�� �Ѵ�.
			if( time.GetYear() >= 2009 && time.GetMonth() >= 10 )
			{
				if( !pTemp->CheckConnectionIP(m_strUserIP.GetString()) )
				{
					// �α׾ƿ� ó��
					m_pDbManager->UserLogoutSimple (nlfd2.szTerraTLoginName);
					nlfd2.nResult = EM_LOGIN_FB_SUB_IMPOSSIBLEIP;

						sc::writeLogError(_T("CAgentTerraUserCheck Login impossible IP %s"), m_strUserIP.GetString() );
				}
			}
		}
#endif
*/
		break;
	case 4 : // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	case 5 : // �ߺ�����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_DUP;
        m_pDbManager->TerraGetUserInfo(szTLoginName, &nlfd2);
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d ClientNum %d LoginName %s UserIp %s", nResult, m_dwClientNum, szTLoginName, m_strUserIP.GetString()) );
		break;
	case 6 : // Block �� ����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	case 7 : // ������ �����մϴ�. ���� ���Խ���
	default : // �ٸ� ����� �α��� ����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	}

	pTemp->TerraMsgLoginBack(&nlfd2);

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// �Ϲ� : GS �α���
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

	// ������ ����ü�� �����Ѵ�.
	GS_NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.nClient	= m_dwClientNum;

	StringCchCopy(NetMsgFB.szIp,	 MAX_IP_LENGTH+1, m_strUserIP.c_str()); // ip
	StringCchCopy(NetMsgFB.szUserid, USR_ID_LENGTH+1, m_strUserID.GetString()); // id


	switch (nResult)
	{
	case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentGsUserCheck result %d", nResult) );
		break;
	case 0 : // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)	
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentGsUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��	
	case 3 : // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)

	case 30 : // �α����� ���� + 2����� �ű� ���� �ʿ�
	case 31 : // case 1, 2, 3, 7�� ���� + 2����� �����
		// ����� ������ �����´�.
		nRetCode = m_pDbManager->GsGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // �������� ����
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// �α׾ƿ� ó��
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

			// ������ ��ȯ ������ Ȯ��
			int Result = 0;
			m_pDbManager->GetItemTransferStatus( NetMsgFB.nUserNum, Result );
			NetMsgFB.ItemTransferResult = Result;
		}

		nRetCode = m_pDbManager->GetUserCountryInfo( NetMsgFB.nUserNum, NetMsgFB.sCountryInfo );
		if ( nRetCode == sc::db::DB_ERROR )
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;

			// �α׾ƿ� ó��
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
			// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

			sc::writeLogError(std::string("CAgentGsUserCheck GetUserCountryInfo sc::db::DB_ERROR") );
		}
		break;
	case 4 : // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError( sc::string::format("CAgentGsUserCheck result %d", nResult) );
		break;
	case 5 : // �ߺ�����
		// ����� ������ �����´�.
		nRetCode = m_pDbManager->GsGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // �������� ����
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// �α׾ƿ� ó��
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
	case 6 : // Block �� ����
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
	default : // �ٸ� ����� �α��� ����
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentGsUserCheck result %d", nResult) );
		break;
	}
	
	pTemp->GsMsgLoginBack( &NetMsgFB );

	return NET_OK;
}


///////////////////////////////////////////////////////////////////////////////
// �Ϲ� : EU �α���
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

	// ������ ����ü�� �����Ѵ�.
	GM_NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.nClient	= m_dwClientNum;

	//
	//HTTP BASIC AUTH�� ���� ����
	//
	std::string strUserNum;
	int nRetCode = 0;	

	int nBlock = 1;

	if ( !pAgentServer->GmHTTPAuth(m_strUserID, m_strPasswd, strUserNum, nBlock) )
	{
		// EU�� ��� GM���̵� �Է��ϸ� ������ ����ϴ� ���̵� �޾ƿ��Ƿ� ��ȯ���־�� �Ѵ�
		// ClientManager�� ���̵��� ��� �Ʒ� GmMsgLoginBack() �Լ����� ��ȯ�Ѵ�
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
		// EU�� ��� GM���̵� �Է��ϸ� ������ ����ϴ� ���̵� �޾ƿ��Ƿ� ��ȯ���־�� �Ѵ�
		// ClientManager�� ���̵��� ��� �Ʒ� GmMsgLoginBack() �Լ����� ��ȯ�Ѵ�
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
		case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
			sc::writeLogError( sc::string::format("CAgentGmUserCheck result %d", nResult) );
			break;
		case 0 : // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)	
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
			sc::writeLogError( sc::string::format("CAgentGmUserCheck result %d", nResult) );
			break;
		case 1 :
		case 2 : // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��	
		case 3 : // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)
			// ����� ������ �����´�.
			nRetCode = m_pDbManager->GmGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
			if (nRetCode == sc::db::DB_ERROR) // �������� ����
			{
				NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
				// �α׾ƿ� ó��
				m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
				// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

				sc::writeLogError(std::string("CAgentGmUserCheck GetUserInfo sc::db::DB_ERROR") );
			}
			else
			{
				NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_OK;
			}
			break;
		case 4 : // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
			sc::writeLogError( sc::string::format("CAgentGmUserCheck result %d", nResult) );
			break;
		case 5 : // �ߺ�����
			// ����� ������ �����´�.
			nRetCode = m_pDbManager->GmGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
			if (nRetCode == sc::db::DB_ERROR) // �������� ����
			{
				NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
				// �α׾ƿ� ó��
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
		case 6 : // Block �� ����
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
		default : // �ٸ� ����� �α��� ����
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
			sc::writeLogError( sc::string::format("CAgentGmUserCheck result %d", nResult) );
			break;
		}
	}

	pAgentServer->GmMsgLoginBack( &NetMsgFB );

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// �Ϲ� : US �α���
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

	// ������ ����ü�� �����Ѵ�.
	US_NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.nClient	= m_dwClientNum;

	StringCchCopy(NetMsgFB.szIp,	 MAX_IP_LENGTH+1, m_strUserIP.c_str()); // ip
	StringCchCopy(NetMsgFB.szUserid, USR_ID_LENGTH+1, m_strUserID.GetString()); // id


	switch (nResult)
	{
	case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentUsUserCheck result %d", nResult) );
		break;
	case 0 : // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)	
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentUsUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��	
	case 3 : // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)
		// ����� ������ �����´�.
		nRetCode = m_pDbManager->UsGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // �������� ����
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// �α׾ƿ� ó��
			m_pDbManager->UserLogoutSimple (m_strUserID.GetString());
			// m_pDbManager->UserLogoutSimple2(NetMsgFB.nUserNum);

			sc::writeLogError(std::string("CAgentUsUserCheck GetUserInfo sc::db::DB_ERROR") );
		}
		else
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_OK;

			// ������ ��ȯ ������ Ȯ��
			int Result = 0;
			m_pDbManager->GetItemTransferStatus( NetMsgFB.nUserNum, Result );
			NetMsgFB.ItemTransferResult = Result;
		}
		break;
	case 4 : // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError( sc::string::format("CAgentUsUserCheck result %d", nResult) );
		break;
	case 5 : // �ߺ�����
		// ����� ������ �����´�.
		nRetCode = m_pDbManager->UsGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == sc::db::DB_ERROR) // �������� ����
		{
			NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;
			// �α׾ƿ� ó��
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
	case 6 : // Block �� ����
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
	default : // �ٸ� ����� �α��� ����
		NetMsgFB.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentUsUserCheck result %d", nResult) );
		break;
	}

	pTemp->UsMsgLoginBack( &NetMsgFB );

	return NET_OK;
}

/**
 * Daum : ����� Password Check
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
		

	// ������ ����ü�� �����Ѵ�.
	DAUM_NET_PASSCHECK_FEEDBACK_DATA2 nlfd2;
	
	nlfd2.nClient = m_dwClientNum;
	StringCchCopy( nlfd2.szDaumGID, DAUM_MAX_GID_LENGTH+1, m_strDaumGID.GetString() );

	switch(nResult)
	{
	case sc::db::DB_ERROR : // DB������ �α��� ���� (-1)
		nlfd2.nResult = sc::db::DB_ERROR;		
		sc::writeLogError( sc::string::format("CAgentDaumPassCheck result %d", nResult) );
		break;	
	case 0 :
		// ����� ������ �����Ѵ�. (0)
		nlfd2.nResult = EM_LOGIN_FB_SUB_OK;
		break;
	case 1 : // ID / PWD ����ġ (�α��� ����) (1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentDaumUserCheck result %d", nResult) );
		break;
	case 2 : // PW �Է� ���� (2) 
		nlfd2.nResult = EM_LOGIN_FB_SUB_PASS_OK;
		break;			
	default : // �ٸ� ����� �α��� ����(1)
		nlfd2.nResult = sc::db::DB_ERROR;		
		sc::writeLogError( sc::string::format("CAgentDaumPassCheck result %d", nResult) );
		break;
	}

	pTemp->DaumMsgPassCheckBack(&nlfd2);

	return NET_OK;
}


/**
 * TERRA : ����� Password Check
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

	// ������ ����ü�� �����Ѵ�.
	TERRA_NET_PASSCHECK_FEEDBACK_DATA2 nlfd2;
	nlfd2.nClient	= m_dwClientNum;	

	StringCchCopy(nlfd2.szTerraTLoginName, TERRA_TLOGIN_NAME+1, m_strTLoginName.GetString()); // TLoginName
	
	switch(nResult)
	{
	case sc::db::DB_ERROR : // DB������ �н����� ������Ʈ ���� (-1)
		nlfd2.nResult = sc::db::DB_ERROR;		
		sc::writeLogError( sc::string::format("CAgentTerraPassCheck result %d", nResult) );
		break;	
	case 0 :
		// ����� ������ �����Ѵ�. (0)
		nlfd2.nResult = EM_LOGIN_FB_SUB_OK;
		break;
	case 1 : // ID / PWD ����ġ (�α��� ����) (1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentTerraUserCheck result %d", nResult) );
		break;
	case 2 : // �ʱ� �н����� ������Ʈ ����
		nlfd2.nResult = EM_LOGIN_FB_SUB_PASS_OK;
		break;			
	default : // �ٸ� ����� �α��� ����(1)
		nlfd2.nResult = sc::db::DB_ERROR;		
		sc::writeLogError( sc::string::format("CAgentTerraPassCheck result %d", nResult) );
		break;
	}

	pTemp->TerraMsgPassCheckBack(&nlfd2);

	return NET_OK;
}


/**
* Excite ����� üũ
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

	m_dwClientNum = dwClientNum; // Agent ������ Client ��ȣ
}

int CAgentExciteUserCheck::Execute(NetServer* pServer)
{	
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;	

	int nResult = m_pDbManager->ExciteUserCheck(m_strUserID.GetString(),
															 m_strUserIP.GetString(),
															 m_nSvrGrp,
															 m_nSvrNum);
	// ������ ����ü�� �����Ѵ�.
	EXCITE_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nType = EXCITE_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	::StringCchCopy(nlfd2.szExciteUserID, EXCITE_USER_ID+1, m_strUserID.GetString()); // id
	::StringCchCopy(nlfd2.szIp,	          MAX_IP_LENGTH+1,	m_strUserIP.GetString()); // ip
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 0 : // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��	
	case 3 : // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)
		// ����� ������ �����Ѵ�.
		nlfd2.m_emResult      = EM_LOGIN_FB_SUB_OK;
        m_pDbManager->ExciteGetUserInfo(m_strUserID.GetString(), &nlfd2);
		break;
	case 4 : // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 5 : // �ߺ�����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_DUP;
		// �ߺ��� �����̶� ���� ������ �����ͼ� �����Ѵ�.
		m_pDbManager->ExciteGetUserInfo(m_strUserID.GetString(), &nlfd2);		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 6 : // Block �� ����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 7 : // ������ �����մϴ�. ���� ���Խ���
	default : // �ٸ� ����� �α��� ����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	}

	pTemp->ExciteMsgLoginBack(&nlfd2);
	
	return NET_OK;
}


/**
* �Ϻ� Gonzo ����� üũ
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

	m_dwClientNum = dwClientNum; // Agent ������ Client ��ȣ
	m_nUserNum = nUserNum;		// WhiteRock System������ UserNum
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
	// ������ ����ü�� �����Ѵ�.
	JAPAN_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nType = JAPAN_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	::StringCchCopy(nlfd2.JapanUserID,	  JAPAN_USER_ID+1, m_strUserID.GetString()); // id
	::StringCchCopy(nlfd2.szIp,	          MAX_IP_LENGTH+1,	m_strUserIP.GetString()); // ip
	
	switch (nResult)
	{
	case sc::db::DB_ERROR : // �ý��ۿ����� �α��� ���� (-1)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_SYSTEM;		
		sc::writeLogError( sc::string::format("CAgentJapanUserCheck result %d", nResult) );
		break;
	case 0 : // ID / PWD ����ġ (���ο� ������ ���� �Ǵ� ���Է¿䱸)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_INCORRECT;		
		sc::writeLogError( sc::string::format("CAgentJapaneUserCheck result %d", nResult) );
		break;
	case 1 :
	case 2 : // ID/PWD �� ��ġ�ϰ�, IP �� ��밡��	
	case 3 : // ID/PWD �� ��ġ�ϰ�, IP ������ ���� (�Ϲ������� �α��� ����)
		// ����� ������ �����Ѵ�.
		nlfd2.m_emResult      = EM_LOGIN_FB_SUB_OK;
        m_pDbManager->JapanGetUserInfo(m_strUserID.GetString(), &nlfd2);
		break;
	case 4 : // ID/PWD �� ��ġ������ IP �� ���Ұ��� ����(���� ���ܵ� IP)
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_IP_BAN;		
		sc::writeLogError( sc::string::format("CAgentJapanUserCheck result %d", nResult) );
		break;
	case 5 : // �ߺ�����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_DUP;
		// �ߺ��� �����̶� ���� ������ �����ͼ� �����Ѵ�.
		m_pDbManager->JapanGetUserInfo(m_strUserID.GetString(), &nlfd2);		
		sc::writeLogError( sc::string::format("CAgentJapanUserCheck result %d", nResult) );
		break;
	case 6 : // Block �� ����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_BLOCK;		
		sc::writeLogError( sc::string::format("CAgentJapanUserCheck result %d", nResult) );
		break;
	case 7 : // ������ �����մϴ�. ���� ���Խ���
	default : // �ٸ� ����� �α��� ����
		nlfd2.m_emResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentJapanUserCheck result %d", nResult) );
		break;
	}

	pTemp->JapanMsgLoginBack(&nlfd2);
	
	return NET_OK;
}

/**
 * EXCITE : ����� Password Check
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

	// ������ ����ü�� �����Ѵ�.
	EXCITE_NET_PASSCHECK_FEEDBACK_DATA2 nlfd2;
	nlfd2.nClient	= m_dwClientNum;	

	StringCchCopy(nlfd2.szExciteUserID, EXCITE_USER_ID+1, m_strExciteUserID.GetString()); // UserID
	
	switch(nResult)
	{
	case sc::db::DB_ERROR : // DB������ �н����� ������Ʈ ���� (-1)
		nlfd2.nResult = sc::db::DB_ERROR;		
		sc::writeLogError( sc::string::format("CAgentExcitePassCheck result %d", nResult) );
		break;	
	case 0 :
		// ����� ������ �����Ѵ�. (0)
		nlfd2.nResult = EM_LOGIN_FB_SUB_OK;
		break;
	case 1 : // ID / PWD ����ġ (�α��� ����) (1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;		
		sc::writeLogError( sc::string::format("CAgentExciteUserCheck result %d", nResult) );
		break;
	case 2 : // �ʱ� �н����� ������Ʈ ����
		nlfd2.nResult = EM_LOGIN_FB_SUB_PASS_OK;
		break;			
	case 3 : // �ʱ� �н����� ������Ʈ ����
		nlfd2.nResult = EM_LOGIN_FB_SUB_SECID_ALREADY;
		break;			
	default : // �ٸ� ����� �α��� ����(1)
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
