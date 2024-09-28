#include "../pch.h"
#include "../../SigmaCore/Encrypt/Aes.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../RanLogic/Network/SendMsgBuffer.h"
#include "../../RanLogic/Network/RcvMsgBuffer.h"

#include "../FieldServer/GLGaeaServer.h"
#include "./NetUser.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CNetUser::CNetUser()
	: m_pSendManager(NULL)
	, m_pRcvManager(NULL)
	, m_nRequestedIOCount( 0 )
	, m_dwLastRequestIOTime( 0 )
    //, m_pAes(NULL)
{
    //InitializeCriticalSectionAndSpinCount(&m_AesLock, sc::CRITICAL_SECTION_SPIN_COUNT);
	m_pSendManager = new CSendMsgBuffer();
	m_pRcvManager  = new CRcvMsgBuffer();    
	Reset();
}

CNetUser::~CNetUser()
{	
	SAFE_DELETE(m_pSendManager);
	SAFE_DELETE(m_pRcvManager);
    //SAFE_DELETE(m_pAes);
    //DeleteCriticalSection(&m_AesLock);
}

int CNetUser::SetAcceptedClient(SOCKET s)
{
	lockOn();

	if (s == INVALID_SOCKET)
	{
		lockOff();
		return NET_ERROR;
	}
	else
	{
		if (m_Socket != INVALID_SOCKET)
		{
			sc::writeLogInfo( std::string("CNetUser::SetAcceptedClient m_Socket != INVALID_SOCKET") );
			
			int nRetCode = ::closesocket(m_Socket);
			if (nRetCode == SOCKET_ERROR)
			{
				nRetCode = ::WSAGetLastError();
				sc::writeLogInfo(sc::string::format("CNetUser::SetAcceptedClient closesocket %d", nRetCode));
			}
		}

		int	nSize = 0;
		sockaddr_in	sAddrIn;

		::SecureZeroMemory(&sAddrIn, sizeof(sockaddr_in));
		m_Socket = s;
		nSize = sizeof(sockaddr_in);

		// Get client ip address and port
		::getpeername(m_Socket, (sockaddr*) &sAddrIn, &nSize);
        TCHAR szIp[MAX_IP_LENGTH+1] = {0};
		::StringCchCopy(szIp, MAX_IP_LENGTH+1, ::inet_ntoa(sAddrIn.sin_addr));
        m_IpAddress      = szIp;
		m_usPort		 = ::ntohs( sAddrIn.sin_port );
		m_dwLastMsgTime	 = timeGetTime();
		m_dwLastSaveTime = m_dwLastMsgTime;
		m_emOnline		 = net::ONLINE;
		m_bHeartBeat	 = true;
		m_dwLastRequestIOTime = m_dwLastMsgTime;

#ifndef _DEBUG		
#ifndef _NP30
		m_csa.Init(); // nProtect GameGuard 초기화
#else
		m_csa.Init(true);
		m_casSeq = 0;
		//sc::writeLogDebug(std::string("CCSAuth3 Init"));
#endif
#endif
        //SAFE_DELETE(m_pAes);
        //m_pAes = new sc::Aes;

		lockOff();
		return NET_OK;
	}
}

static int nCloseClientError = 0;

void CNetUser::CloseSocket()
{
	lockOn();

	if (m_Socket != INVALID_SOCKET)
	{
		::shutdown( m_Socket, SD_BOTH );
		int nRetCode = ::closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;

		if (nRetCode == SOCKET_ERROR)
		{
			nRetCode = ::WSAGetLastError();
			sc::writeLogInfo(sc::string::format("CNetUser::CloseClient NET_ONLINE closesocket %d", nRetCode));
		}
	}

	lockOff();
}

int CNetUser::CloseClient( DWORD dwClient )
{
	lockOn();	
	int nRetCode = 0;	

	if ( INVALID_SOCKET != m_Socket )
	{
#ifndef _DEBUG
#ifndef _NP30
        m_csa.Close(); // nProtect GameGuard 종료
#else
		m_csa.Close(); // nProtect GameGuard 종료
		//sc::writeLogDebug(std::string("CCSAuth3 Close"));
#endif
#endif

		//mjeon.Initiate abortive release			
		LINGER linger;
		linger.l_onoff = TRUE;
		linger.l_linger = 0;
		setsockopt( m_Socket, SOL_SOCKET, SO_LINGER, ( const char* )&linger, sizeof( linger ) );

		::shutdown( m_Socket, SD_BOTH );

		nRetCode = ::closesocket( m_Socket );
		if ( nRetCode == SOCKET_ERROR )
		{
			nRetCode = ::WSAGetLastError();
			if ( nRetCode == WSANOTINITIALISED ) // A successful WSAStartup call must occur before using this function. 
			{
				sc::writeLogError( std::string( "CNetUser::CloseClient closesocket WSANOTINITIALISED ERROR" ) );
			}
			else if ( nRetCode == WSAENETDOWN ) // The network subsystem has failed. 
			{
				sc::writeLogError( std::string( "CNetUser::CloseClient closesocket WSAENETDOWN ERROR" ) );
			}
			else if ( nRetCode == WSAENOTSOCK ) // The descriptor is not a socket. 
			{
				sc::writeLogError( std::string( "CNetUser::CloseClient closesocket WSAENOTSOCK ERROR" ) );
			}
			else if ( nRetCode == WSAEINPROGRESS ) // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
			{
				sc::writeLogError( std::string( "CNetUser::CloseClient closesocket WSAEINPROGRESS ERROR" ) );
			}
			else if ( nRetCode == WSAEINTR ) // The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall. 
			{
				sc::writeLogError( std::string( "CNetUser::CloseClient closesocket WSAEINTR ERROR" ) );
			}
			else if ( nRetCode == WSAEWOULDBLOCK ) // The socket is marked as nonblocking and SO_LINGER is set to a nonzero time-out value. 
			{
				sc::writeLogError( std::string( "CNetUser::CloseClient closesocket WSAEWOULDBLOCK ERROR" ) );
			}
			else
			{
				sc::writeLogError( std::string( "CNetUser::CloseClient closesocket unknown ERROR %d", nRetCode ) );
			}
		}

		m_Socket = INVALID_SOCKET;
	}

	if ( GetReferenceCount() )
	{
		// 아직 요청된 io가 남아 있는 상태이다.
		// 이 클라이언트 번호의 CNetUser 는 아직 사용할 수 없다.
//#if defined(_RELEASED) || defined(_DEBUG)
//		sc::writeLogWarn( 
//			sc::string::format( 
//			"CloseClient ReferenceCount remain. current count: %1%, slottype: %2%, slot: %3%, chanum: %4%, ip: %5%, port: %6%", 
//			GetReferenceCount(), m_SlotType, dwClient, m_ChaDbNum, m_IpAddress, m_usPort ) );
//#endif
		lockOff();
		return NET_ERROR;
	}

//#if defined(_RELEASED) || defined(_DEBUG)
//	sc::writeLogInfo( 
//		sc::string::format( 
//		"CloseClient this slot can reuse. current count: %1%, slottype: %2%, slot: %3%, chanum: %4%, ip: %5%, port: %6%", 
//		GetReferenceCount(), m_SlotType, dwClient, m_ChaDbNum, m_IpAddress, m_usPort ) );
//#endif

	lockOff();

	Reset();	// Reset 내에서도 lock을 하므로, 반드시 lockOff() 외부에서 호출하라. (재귀락이 아니다.)

	return NET_OK;
}

void CNetUser::Reset()
{
	lockOn();

	m_dwLastRequestIOTime = 0;
	m_nRequestedIOCount = 0;
	m_Socket			= INVALID_SOCKET;
	m_emOnline			= net::OFFLINE;
	m_usPort			= 0;
	m_dwLastMsgTime		= 0; // 마지막으로 읽기 통신이 된 시간
	m_dwLastSaveTime	= 0; // 마지막으로 캐릭터를 저장한 시간
	m_nPing				= 0; // 클라이언트와 통신에 걸린 시간
	m_wChaRemain		= 0;
	m_wChaTestRemain	= 0;

	m_ChaDbNum			= 0;
	m_nUserType         = 0;    
	m_dwGaeaID			= GAEAID_NULL;	
    m_SlotType			= net::SLOT_UNKNOWN; // 슬롯의 타입

	m_uib.nKey			= 0;
	m_uib.nKeyDirection = 0;
	m_uib.UserDbNum		= -1;
	m_bACCOUNT_PASS		= false;
	m_bACCOUNT_PASSING  = false;
	m_nUse2ndPass = 0;

	//m_ck.nKey			= 0;
	//m_ck.nKeyDirection  = 0;

	m_bHeartBeat		= false;
	m_nChannel          = -1;
	m_wInvalidPacketCount = 0; // 잘못된 패킷을 보낸 횟수

    CTime cTemp(1970, 2, 1, 0, 0, 0); // 프리미엄 서비스 만료일을 세팅한다. (초기값)
    m_tPremiumDate      = cTemp.GetTime();

	CTime cTemp2(1970, 2, 1, 0, 0, 0); // 채팅 블록 만료일을 세팅한다. (초기값)
	m_tChatBlockDate    = cTemp2.GetTime();

	m_nRandomPass = 0;

	memset( m_uib.szUserID,	0, sizeof(TCHAR) * GSP_USERID );
    m_IpAddress = "";
	memset( m_szDecodedTID, 0, sizeof(TCHAR) * (TERRA_TID_DECODE+1) );
    memset( m_szUUID, 0, sizeof(TCHAR) * (sc::string::UUID_LENGTH) );
	memset( m_szEncryptKeyHeart, 0, sizeof(TCHAR) * (ENCRYPT_KEY+1) );

	m_pSendManager->reset();
	m_pRcvManager->reset();

	m_wLoginAttempt = 0;       // 한번 접속에 로그인 요청 횟수


	// Thailande
	m_wThaiFlag = 0; // Thailand 사용자 플래그
	m_wThaiTodayGamePlayTime = 0; // Thailand 오늘 게임플레이 타임e
	m_emIPBonus = EMIP_BONUS_NONE; 

	// nProtect
	m_bNProtectAuth = true;
	m_bNProtectFirstCheck = true;
	m_bNProtectFirstAuthPass = false;

	// 극강부 변수 초기화
	m_bExtreme = false;

	// 변술부 변수 초기화;
	m_bActor = false;

	// 중국 로그인 시간 체크
	m_nChinaGameTime = 0;
	m_nChinaOfflineTime = 0;
	CTime cTemp3(1970, 2, 1, 0, 0, 0); // China LastLoginDate를 세팅한다. (초기값)
	m_tLastLoginTime    = cTemp3.GetTime();	

	// 베트남 탐닉방지 관련 추가
	m_nVTGameTime	= 0;
	m_nChinaUserAge = 0;
	m_wConSendError = 0;

    m_dwLastCheckTime = 0;
    m_nPacketCount = 0;
    m_cMsgBuffer.clear();
    m_cMsgBuffer.set_capacity(PACKET_LOG_SIZE);

    m_wAge = 0;    
    m_Country = wb::UNKNOWN_COUNTRY;

//    SAFE_DELETE(m_pAes);

	lockOff();
}

void CNetUser::GspSetUUID(const TCHAR* szUUID)
{
	if (szUUID)
		StringCchCopy(m_szUUID, sc::string::UUID_LENGTH, szUUID);
}

TCHAR* CNetUser::GspGetUUID()
{
	return m_szUUID;
}

void CNetUser::SetEncKey(const TCHAR* szEncKey)
{
	if (szEncKey)
		StringCchCopy(m_szEncryptKeyHeart, ENCRYPT_KEY+1, szEncKey);
}

TCHAR* CNetUser::GetEncKey()
{
	return m_szEncryptKeyHeart;
}

// 한국 로그인 방식 변경에 따른 추가
void CNetUser::KorSetUUID( const TCHAR* szUUID )
{
    if (szUUID)
		StringCchCopy(m_szUUID, sc::string::UUID_LENGTH, szUUID);
}

TCHAR* CNetUser::KorGetUUID()
{
	return m_szUUID;
}

// 일본 WhiteRock System과의 통신을 위해 추가
void CNetUser::JPSetUUID( const TCHAR* szUUID )
{
    if (szUUID)
		StringCchCopy(m_szUUID, sc::string::UUID_LENGTH, szUUID);
}

TCHAR* CNetUser::JPGetUUID()
{
	return m_szUUID;
}

void CNetUser::TerraSetDecodedTID( const TCHAR* szDecodedTID )
{
	if (szDecodedTID)
		StringCchCopy(m_szDecodedTID, TERRA_TID_DECODE+1, szDecodedTID);
}

TCHAR* CNetUser::TerraGetDecodedTID()
{
    return m_szDecodedTID;
}

void CNetUser::ResetUserID()
{
	memset(m_uib.szUserID, 0, sizeof(TCHAR) * GSP_USERID);	
}

//! 클라이언트 버퍼 리셋
void CNetUser::ResetRcvBuffer()
{
    m_pRcvManager->reset();	
}

//! 클라이언트에서 받은 메시지를 넣는다.
int CNetUser::addRcvMsg(void* pMsg, DWORD dwSize)
{
	ASSERT(pMsg);

	if (!pMsg || dwSize > NET_DATA_BUFSIZE)
        return NET_ERROR;	
	
	if (!IsOnline())
	{
		sc::writeLogError(std::string("CNetUser::addRcvMsg IsOnline false"));
		Reset();
		return NET_ERROR;
	}

	// 현재 까지 받은 데이터 사이즈를 체크한다.
	int nRcvSize = (int) dwSize;
	int nTotalRcvSize = m_pRcvManager->getRcvSize()+nRcvSize;
	
	// 총 받은 데이터 사이즈가 최대 버퍼사이즈보다 작아야 한다.	
	if (nTotalRcvSize > NET_DATA_CLIENT_MSG_BUFSIZE)
	{
		sc::writeLogError(sc::string::format("addRcvMsg %1% %2% Buffer Over", nTotalRcvSize, m_IpAddress));
        ResetRcvBuffer();
		return NET_ERROR;
	}
	else
	{
		m_pRcvManager->addRcvMsg(pMsg, nRcvSize);
		return NET_OK;
	}
}

//! 클라이언트에서 받은 메시지를 하나 가져온다.
void* CNetUser::getRcvMsg()
{	
	if (IsOnline())
	{
        return m_pRcvManager->getMsg();		
	}
	else
	{
        sc::writeLogError(std::string("getRcvMsg IsOnline false"));
        Reset();		
        return NULL;        
	}
}

/*
void CNetUser::SetSocket(SOCKET s)
{
	m_Socket = s;
}
*/

void CNetUser::SetCryptKey(CRYPT_KEY ck)
{
	lockOn();
	m_ck = ck;
	lockOff();
}

CRYPT_KEY CNetUser::GetCryptKey()
{	
	return m_ck;	
}

SOCKET CNetUser::GetSOCKET()
{
	return m_Socket;
}

void CNetUser::SetIP(const std::string& IpAddress)
{	
	m_IpAddress = IpAddress;
}

std::string CNetUser::GetIP() const
{
	return m_IpAddress;
}

void CNetUser::SetPort(USHORT usPort)
{
	m_usPort = usPort;
}

USHORT CNetUser::GetPort()
{
	return m_usPort;
}

TCHAR* CNetUser::GspGetUserID()
{
	return m_uib.szUserID;
}

void CNetUser::GspSetUserID(const TCHAR* szGspUserID)
{
	if (szGspUserID)
	    ::StringCchCopy(m_uib.szUserID, GSP_USERID, szGspUserID);
}

void CNetUser::SetUserID(const TCHAR* szUserID)
{
	if (szUserID)
	    ::StringCchCopy(m_uib.szUserID, GSP_USERID, szUserID);	
}

void CNetUser::SetUserID(const std::string& UserID)
{
    ::StringCchCopy(m_uib.szUserID, GSP_USERID, UserID.c_str());
}

TCHAR* CNetUser::GetUserID()
{
	return m_uib.szUserID;
}

void CNetUser::SetUserDbNum(int nUserNum)
{
	m_uib.UserDbNum = nUserNum;
}

int CNetUser::UserDbNum()
{
	return m_uib.UserDbNum;
}
    
void CNetUser::SetLoginTime()
{
	 m_LoginTime = CTime::GetCurrentTime();
}

void CNetUser::SetGaeaID(DWORD dwGaeaID)
{
	m_dwGaeaID = dwGaeaID;
}

DWORD CNetUser::GetGaeaID()
{
	return m_dwGaeaID;
}

//mjeon.AF.AgentServer에서 최초 캐릭터 접속 시에 인증목적으로 사용된다. FieldServer에서는 의미가 없다.
void CNetUser::ValidateChaNum(DWORD dwChaNum)
{
	m_setChaNum.insert(dwChaNum);
}

void CNetUser::InvalidateChaNum(DWORD dwChaNum)
{
	m_setChaNum.erase(dwChaNum);
}

BOOL CNetUser::IsValidChaNum(DWORD dwChaNum)
{
	return (m_setChaNum.count(dwChaNum) > 0);
}

void CNetUser::ChaDbNum(int nChaNum)
{
	m_ChaDbNum = nChaNum;
}

int CNetUser::GetChaDbNum() const
{
	return m_ChaDbNum;
}

CTime CNetUser::GetLoginTime()
{
	return m_LoginTime;
}

void CNetUser::SetSlotType(net::SLOT_TYPE dwType)
{
	m_SlotType = dwType;
}

net::SLOT_TYPE CNetUser::GetSlotType()
{
	return m_SlotType;
}

void CNetUser::SetUserType(int nType)
{
	m_nUserType = nType;
}

int	CNetUser::GetUserType()
{
	return m_nUserType;
}

void CNetUser::SetChaRemain( WORD wNum )
{
    m_wChaRemain = wNum;
}

void CNetUser::SetChaTestRemain(WORD wNum)
{
	m_wChaTestRemain = wNum;
}

WORD CNetUser::GetChaRemain()
{
	return m_wChaRemain;
}

WORD CNetUser::GetChaTestRemain()
{
	return m_wChaTestRemain;
}

__time64_t CNetUser::GetPremiumDate()
{
    return m_tPremiumDate;
}

void CNetUser::SetPremiumDate(__time64_t tTime)
{
    m_tPremiumDate = tTime;
}

void CNetUser::SetChinaGameTime( int nChinaGameTime )
{
	m_nChinaGameTime = nChinaGameTime;
}

void CNetUser::SetChinaOfflineTime( int nChinaOfflineTime )
{
	m_nChinaOfflineTime = nChinaOfflineTime;
}

void CNetUser::SetLastLoginDate( __time64_t tTime )
{
	m_tLastLoginTime = tTime;
}

void CNetUser::SetVTGameTime( int nVTGameTime )
{
	m_nVTGameTime = nVTGameTime;
}

// 베트남 탐닉방지 추가
int CNetUser::GetVTGameTime()
{
	return m_nVTGameTime;
}

int CNetUser::GetChinaGameTime()
{
	return m_nChinaGameTime;
}

int CNetUser::GetChinaOfflineTime()
{
	return m_nChinaOfflineTime;
}

__time64_t CNetUser::GetLastLoginDate()
{
	return m_tLastLoginTime;
}

void CNetUser::SetChinaUserAge( int nChinaUserAge )
{
	m_nChinaUserAge = nChinaUserAge;
}

int CNetUser::GetChinaUserAge()
{
	return m_nChinaUserAge;
}

__time64_t CNetUser::GetChatBlockDate()
{
	return m_tChatBlockDate;
}

void CNetUser::SetChatBlockDate(__time64_t tTime)
{
	m_tChatBlockDate = tTime;
}

void CNetUser::SetChannel(int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		sc::writeLogError(sc::string::format("SetChannel CH %1%", nChannel));
		return;
	}
	else
	{
		m_nChannel = nChannel;
	}
}

int CNetUser::GetChannel()
{
	return m_nChannel;
}

bool CNetUser::IsAccountPass() const
{
	return m_bACCOUNT_PASS;
}

void CNetUser::SetAccountPass(bool bPASS)
{
	m_bACCOUNT_PASS = bPASS;
}

int CNetUser::IsUse2ndPass() const
{
	return m_nUse2ndPass;
}

void CNetUser::SetUse2ndPass(int nUse2ndPass)
{
	m_nUse2ndPass = nUse2ndPass;
}

//! 계정검사 통과중인지 검사
bool CNetUser::IsAccountPassing() const
{
	return m_bACCOUNT_PASSING;
}

//! 계정검사 통과중인지 세팅
void CNetUser::SetAccountPassing(bool bPassing)
{
	m_bACCOUNT_PASSING = bPassing;
}

WORD CNetUser::ThaiGetFlag ()
{
	return m_wThaiFlag;
}

void CNetUser::ThaiSetFlag (WORD wFlag)
{
	m_wThaiFlag = wFlag;
}

void CNetUser::ThaiSetTodayPlayTime (WORD wMinutes)
{
	m_wThaiTodayGamePlayTime = wMinutes;
}

bool CNetUser::ThaiIs3HourOver (CTime& currentTime)
{
	CTimeSpan ts = currentTime - m_LoginTime; // 로그인 시간부터 현재시간까지 계산

	// 오늘 총 플레이한 시간 계산
	LONGLONG llTotalTime = (LONGLONG) m_wThaiTodayGamePlayTime + ts.GetTotalMinutes();

	// 180 분 (3시간) 지났는지 체크
	if (llTotalTime >= THAI_LIMIT_MIN)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//! 보내야할 메시지를 등록한다.
int CNetUser::addSendMsg(NET_MSG_GENERIC* pMsg, DWORD dwSize )
{
	return m_pSendManager->addMsg(pMsg, dwSize);
}

//! 보내야할 메시지 버퍼를 가져온다.
void* CNetUser::getSendBuffer()
{
	return m_pSendManager->getSendBuffer();
}

//! 보내야할 메시지 크기를 가져온다.
int CNetUser::getSendSize()
{
	return m_pSendManager->getSendSize();
}

//! 말레이지아 요청으로 1 회 접속시 5 회 이상 로그인 요청시 
//! 사용자의 접속을 끊음...
//! 로그인 요청 횟수 증가시킴
void CNetUser::IncLoginAttempt()
{
	++m_wLoginAttempt;
}

//! 로그인 요청 횟수 가져옴
WORD CNetUser::GetLoginAttempt()
{
	return m_wLoginAttempt;
}

void CNetUser::SetRandomPassNumber(int nNumber)
{
	if (nNumber > 0)
	{
		m_nRandomPass = nNumber;
	}
	else
	{
		m_nRandomPass = 1;
	}
}

int CNetUser::GetRandomPassNumber()
{
	return m_nRandomPass;
}

void CNetUser::ResetRandomPassNumber()
{
	m_nRandomPass = 0;
}

// 극강부 생성 가능 여부를 등록한다.
void CNetUser::SetExtremeCreate( bool bExtreme )
{
	m_bExtreme = bExtreme;
}

// 극강부 생성 가능 여부를 불러온다.
bool CNetUser::GetExtreme()
{
	return m_bExtreme;
}

void CNetUser::SetJumpingCreate( bool bJumping )
{
	m_bJumping = bJumping;
}

bool CNetUser::GetJumping()
{
	return m_bJumping;
}

void CNetUser::SetActorCreate ( bool bCreate )
{
	m_bActor = bCreate;
}

const bool CNetUser::GetCreateActor ()
{
	return m_bActor;
}

bool CNetUser::IncPacketCount( int nMsgID )
{
	const DWORD		dwCurTick = GetTickCount();
    if ( dwCurTick - m_dwLastCheckTime > UPDATE_PACKET_TIME ) 
    {
        m_nPacketCount = 0;
        m_dwLastCheckTime = dwCurTick;
    }

    m_cMsgBuffer.push_back( nMsgID );
    InterlockedIncrement(&m_nPacketCount);

    if ( m_nPacketCount > MAX_PACKET_COUNT )
        return true;

	return false;
}

void CNetUser::WritePacketLog()
{
    sc::writeLogError(
        sc::string::format(
            "Packet Count Error IP %1%, UserID %2%, UserNum %3%, CharNum %4%",
            GetIP(),
            GetUserID(),
            UserDbNum(),
            GetChaDbNum()));

    std::string strLog(
        sc::string::format(
            "Packet List NET_MSG_GCTRL(%1%)",
            NET_MSG_GCTRL));
    for (size_t i=0; i<m_cMsgBuffer.size(); ++i)
    {
        strLog.append(
            sc::string::format(
                "%1% ",
                m_cMsgBuffer[i]));
    }
    sc::writeLogError(strLog);
}

// 태국 User Class Type을 설정한다.
void CNetUser::SetIPBonusClass( EMIP_BONUS_CLASS emIPBonus )
{
	m_emIPBonus = emIPBonus;
}

// 태국 User Class Type을 가져온다.
EMIP_BONUS_CLASS CNetUser::GetIPBonusClass()
{
	return m_emIPBonus;
}

void CNetUser::AddReference( DWORD dwClient )
{
	m_dwLastRequestIOTime = ::timeGetTime();
	InterlockedIncrement( ( volatile LONG* )&m_nRequestedIOCount );

//#ifndef _RELEASE
	//sc::writeLogError( sc::string::format( "user AddReference client: %1% count: %2%", dwClient, m_nRequestedIOCount ) );
//#endif
}

void CNetUser::ReleaseReference( DWORD dwClient )
{
	InterlockedDecrement( ( volatile LONG* )&m_nRequestedIOCount );

//#ifndef _RELEASE
	//sc::writeLogError( sc::string::format( "user ReleaseReference client: %1% count: %2%", dwClient, m_nRequestedIOCount ) );
//#endif
}

LONG CNetUser::GetReferenceCount()
{
	return m_nRequestedIOCount;
}

DWORD CNetUser::GetLastRequestIOTime()
{
	return m_dwLastRequestIOTime;
}

void CNetUser::ResetReference()
{
	InterlockedExchange( ( volatile LONG* )&m_nRequestedIOCount, 0 );
}

void CNetUser::SetUserCountry ( const Country::SCOUNTRY_INFO& sCountryInfo )
{
	// 존재하지 않는 국가라면 세팅하지 않는다;
	if ( !Country::GLCountryManServer::Instance()->ExistCountry(
		sCountryInfo.wContinentID,
		sCountryInfo.wCountryID ) )
	{
		m_userCountry.RESET();

		return;
	}

	// 세팅;
	m_userCountry = sCountryInfo;
}