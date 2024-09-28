#ifndef S_CCLIENTAGENT_H_
#define S_CCLIENTAGENT_H_

#pragma once

#include "./NetClientManager.h"

namespace wb
{
    enum EM_COUNTRY;
}

//! \date 2002-05-30
//! \author jgkim
//! \history
//! 2011-08-11 m_FieldMan shared_ptr


struct FIELDSERVER_INFO
{
	FIELDSERVER_INFO()
		:nChannel(-1)
		,nSvrNum(-1)
		,bInstance(FALSE)
	{
	}

	FIELDSERVER_INFO(int _nChannel, int _nSvrNum, BOOL _bInstance = FALSE)
		:nChannel(_nChannel)
		,nSvrNum(_nSvrNum)
		,bInstance(_bInstance)
	{
	}

	int		nChannel;
	int		nSvrNum;
	BOOL	bInstance;
};

// 2차비번
typedef struct _PASSWORD_BLOCK
{
	DWORD	nTryFailCount;
	CTime	tBlockTime;

	_PASSWORD_BLOCK()
		: nTryFailCount(1)
	{		
	}

private:
	void CheckBlockTime()
	{
		if( nTryFailCount >= 3 )
		{
			tBlockTime = CTime::GetCurrentTime() + CTimeSpan(0,0,5,0);
		}
	}

public:
	bool IsBlock()
	{
		if( nTryFailCount >=3 && tBlockTime > CTime::GetCurrentTime() )
			return true;
		else
			return false;
	}

	CTime GetBlockTime()
	{
		return tBlockTime;
	}

	DWORD GetTryFailCount()
	{
		return nTryFailCount;
	}

	void IncreaseCount()
	{
		nTryFailCount++;

		CheckBlockTime();
	}

	void IncreaseCount(int nCount)
	{
		nTryFailCount = nCount;
		
		CheckBlockTime();
	}

	void Reset()
	{
		nTryFailCount = 0;
	}

} PASSWORD_BLOCK, *PPASSWORD_BLOCK;
/////////////////////////////////////////////

typedef std::map<DWORD, FIELDSERVER_INFO>				MAP_FIELDSERVER_INFO;
typedef std::map<DWORD, FIELDSERVER_INFO>::iterator		MAP_FIELDSERVER_INFO_ITER;
typedef std::map<DWORD, DWORD>							MAP_CHANUM;
typedef std::map<DWORD, DWORD>::iterator				MAP_CHANUM_ITER;
// 2차비번
typedef std::map<DWORD, PASSWORD_BLOCK>					MAP_2NDPASSWORD_BLOCK_INFO;
typedef std::map<DWORD, PASSWORD_BLOCK>::iterator		MAP_2NDPASSWORD_BLOCK_INFO_ITER;
////////////////////////

class NetAgentClientMan : public NetClientManager
{
public :
	NetAgentClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax, F_SERVER_INFO (* pFieldServerInfo)[FIELDSERVER_MAX], A_SERVER_INFO* pAgentServerInfo);
	virtual ~NetAgentClientMan();

protected :
	//mjeon.AF
	F_SERVER_INFO			(* m_pFieldServerInfo)[FIELDSERVER_MAX];	//reference to m_FieldServerInfo in CAgentServer.
	A_SERVER_INFO*			m_pAgentServerInfo;

	MAP_FIELDSERVER_INFO	m_mapFieldServer;	//ClientSlot - FieldServerInfo
	CRITICAL_SECTION		m_CS_FieldInfo;

	MAP_2NDPASSWORD_BLOCK_INFO	m_map2ndPasswordBlock; // 2차비번

public:
    void Reset(DWORD dwClient);

	//
	//mjeon.SetAcceptedClient를 호출하는 시점에 SlotType이 명확하다면, 반드시 매개변수로 넘겨주도록 하자.
	//
	int SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType = net::SLOT_UNKNOWN);

	SOCKET	GetSocket	  (DWORD dwClient);
	SOCKET	GetFieldSocket(DWORD dwClient);	
	DWORD	GetFieldClientSlot( DWORD dwClient );
	DWORD	AdjestSlotNum( DWORD dwClient );

	DWORD	GetServerGroup ( DWORD dwClient );
	DWORD	GetClientID_ServerGroup ( DWORD dwServerGroup );

	//mjeon.AF
	int		SetUpFieldInfo(DWORD dwClient, int nFieldServer, DWORD dwGaeaID, int nServerChannel);	
	int		SetUpInstanceInfo(DWORD dwClient, int nInstanceServer, int nServerChannel /*Instance로 넘어가기 전에 캐릭터가 속했던 채널*/);

	BOOL	IsBoundByInstance(DWORD dwClient, OUT DWORD &dwInstanceServerNum);

	int		CloseField(DWORD dwClient);
	void	ResetFieldInfo(DWORD dwClient);	//mjeon.AF

	// 각 Agent 및 Field 정보에서 Client ID를 얻어서 Client ID를 이용해 보냄;
	int		SendAgent ( DWORD dwAgentNum, NET_MSG_GENERIC* pBuffer );
	int		SendField ( DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pBuffer );	//Send to Field using Channel and FieldSvrNum

	// Client Manager에서 관리하는 Client ID를 이용해 보냄;
	int		SendField (DWORD dwClient, NET_MSG_GENERIC* pBuffer);						//Send to Field using client's slot (not including AgentFieldSlot)
	int		SendField (DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);

	void	SendClientFinal();

	int		addRcvMsg(DWORD dwClient, void* pMsg, DWORD dwSize);

	void	ResetRcvBuffer(DWORD dwClient);

	void*	getRcvMsg(DWORD dwClient);	
    
	bool	IsOnline      (DWORD dwClient);

	bool	IsAgentFieldSlot(DWORD dwClient);
	bool	IsMatchSlot(DWORD dwClient);
	bool	IsAgentAgentSlot(DWORD dwClient);

	void	ResetValidChaNumList(DWORD dwClient);
	void	ValidateChaNum(DWORD dwClient, DWORD dwChaNum);
	void	InvalidateChaNum(DWORD dwClient, DWORD dwChaNum);
	BOOL	IsValidChaNum(DWORD dwClient, DWORD dwChaNum);

	void	SetHeartBeat   (DWORD dwClient);
	void    ResetHeartBeat (DWORD dwClient);
	bool	CheckHeartBeat (DWORD dwClient);	

	net::SLOT_TYPE GetSlotType(DWORD dwClient);
    std::string GetClientIP(DWORD dwClient) const;
	USHORT  GetClientPort  (DWORD dwClient);

	// 한국 로그인 방식 변경에 따른 추가
	void    KorSetUUID( DWORD dwClient, const TCHAR* szUUID );
	TCHAR*  KorGetUUID( DWORD dwClient );

	// 일본 WhiteRock과 통신을 하고 Return된 UUID 세팅을 위해서 추가.
	void	JPSetUUID( DWORD dwClient, const TCHAR* szUUID );
	TCHAR*	JPGetUUID( DWORD dwClient );

	void    GspSetUUID( DWORD dwClient, const TCHAR* szUUID );
	TCHAR*  GspGetUUID( DWORD dwClient );

	void    TerraSetDecodedTID( DWORD dwClient, const TCHAR* szDecodedTID );
	TCHAR*  TerraGetDecodedTID( DWORD dwClient );

	void    SetEncKey( DWORD dwClient, const TCHAR* szEncKey );
	TCHAR*  GetEncKey( DWORD dwClient );

	bool    IsAccountPassing(DWORD dwClient); // 계정 인증중인지 검사
	void    SetAccountPassing(DWORD dwClient, bool bPassing); // 계정 인증중 세팅

	void    SaveFreeSlot   ();
    
    //! 나이를 세팅한다.
    void SetAge(DWORD ClientSlot, WORD Age);
    WORD GetAge(DWORD ClientSlot) const;

    //! 국가
    wb::EM_COUNTRY GetCountry(DWORD ClientSlot) const;
    void SetCountry(DWORD ClientSlot, wb::EM_COUNTRY Country);

	//! Thailand 의 사용자 플래그를 세팅한다.
	//! \param dwClient
	//! \param wFlag 
	void ThaiSetFlag (DWORD dwClient, WORD wFlag);
	WORD ThaiGetFlag (DWORD dwClient);
	void ThaiSetTodayPlayTime (DWORD dwClient, WORD wMinutes);
	bool ThaiIs3HourOver (DWORD dwClient, CTime& currentTime);	

	//! 말레이지아 요청으로 1 회 접속시 5 회 이상 로그인 요청시 
	//! 사용자의 접속을 끊음...
	//! 로그인 요청 횟수 증가시킴
	void IncLoginAttempt(DWORD dwClient);
	//! 로그인 요청 횟수 가져옴
	WORD GetLoginAttempt(DWORD dwClient);	
	
	// nProtect	
	//! nProtect GameGuard 인증용 쿼리를 가져오기 전에 호출
	virtual bool nProtectSetAuthQuery(DWORD dwClient) override;
	virtual int nProtect30SetAuthQuery(DWORD dwClient) override;
    	
	//! nProtect GameGuard 인증용 쿼리 가져오기
	virtual GG_AUTH_DATA nProtectGetAuthQuery(DWORD dwClient) override;
	virtual BYTE* nProtect30GetAuthQuery(DWORD dwClient) override;

	//! nProtect GameGuard 인증 응답 가져오기
	virtual GG_AUTH_DATA nProtectGetAuthAnswer(DWORD dwClient) override;
	virtual BYTE* nProtect30GetAuthAnswer(DWORD dwClient) override;

	//! nProtect GameGuard 인증 응답 설정
	virtual void nProtectSetAuthAnswer(DWORD dwClient, GG_AUTH_DATA& ggad) override;
	virtual void nProtect30SetAuthAnswer(DWORD dwClient, BYTE* ggad, int nPacketSeq) override;

	//! nProtect GameGuard 인증 결과 가져오기
	DWORD nProtectCheckAuthAnswer(DWORD dwClient);
	DWORD nProtect30CheckAuthAnswer(DWORD dwClient, UINT32 nReceivedSize);

	//! nProtect GameGuard 인증 상태를 리셋시킨다.
	virtual void nProtectResetAuth(DWORD dwClient);

	//! nProtect 처음 인증여부 세팅
	void nProtectFirstCheck(DWORD dwClient, bool bState);

	//! nProtect 처음 인증인가?
	bool nProtectIsFirstCheck(DWORD dwClient);
	
	//! nProtect GameGuard 처음 인증을 통과했는가?
	//! 이 인증을 통과해야 게임플레이를 할 수 있다.
	bool nProtectFirstAuthPass( DWORD dwClient );

	//! nProtect GameGuard 처음인증 통과상태 세팅
	void nProtectSetFirstAuthPass( DWORD dwClient, bool bState );

	// 2차비번
	bool IsBlockBy2ndPassword(DWORD dwUserDBNum); // 2차비번 오류로 인해 블럭된 사용자인지
	CTime GetBlockExpiredTime(DWORD dwUserDBNum);	
	void ResetCertification2ndPassword(DWORD dwUserDBNum);	
	void Increase2ndPasswordError(DWORD dwUserDBNum);	
	///////////////////////////////////
};

#endif // S_CCLIENTAGENT_H_
