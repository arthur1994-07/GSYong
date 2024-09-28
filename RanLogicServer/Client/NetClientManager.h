#ifndef _NET_CLIENT_MANAGER_H_
#define _NET_CLIENT_MANAGER_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include <map>
#include <string>
#include <deque>
#include "../../SigmaCore/Util/Lock.h"
#include "../../SigmaCore/Util/CBit.h"

#include "../../RanLogic/Util/s_CLock.h"
#include "../../RanLogic/Util/s_CCfg.h"

#include "../../RanLogic/Country/GLCountryDefine.h"


#ifdef _USE_TBB
#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_unordered_map.h"
#include <set>

//#include "tbb/concurrent_unordered_set.h"
//#include "tbb/concurrent_hash_map.h"
#endif

#define NUM_OF_RESERVED_CID_FOR_SERVER_CONNECTIONS	20	//서버간 통신을 위해 최소한 확보되어 있어야할 CID의 갯수

namespace sc {
    namespace net {
        class IOManager;
    }
}

class CNetUser;

//! \ingroup NetServerLib
//! \date 2002-05-30 
//! \author Jgkim
//! \history
//! 2011-08-111 jgkim m_ClientMan shared_ptr
class NetClientManager
{
	struct sIPConnectCount 
	{
		int nCount;
		DWORD dwLastConnectTime;

		sIPConnectCount()
			: nCount( 1 )
			, dwLastConnectTime( ::GetTickCount() )
		{
		}
	};

	typedef std::vector<int>	        VEC_CLIENTS;
	typedef VEC_CLIENTS::iterator       VEC_CLIENTS_ITER;
    typedef VEC_CLIENTS::const_iterator VEC_CLIENTS_CITER;

	typedef std::map<int, BOOL>         MAP_CLIENTS;
    typedef MAP_CLIENTS::iterator       MAP_CLIENTS_ITER;
    typedef MAP_CLIENTS::const_iterator MAP_CLIENTS_CITER;
    typedef MAP_CLIENTS::value_type     MAP_CLIENTS_VALUE;

	typedef std::map<DWORD, DWORD>		MAP_CHANUM;
	typedef MAP_CHANUM::iterator		MAP_CHANUM_ITER;
	typedef MAP_CHANUM::value_type		MAP_CHANUM_VALUETYPE;

	typedef std::deque<int> DEQ_CLIENTS;

public:
	NetClientManager( int nMaxClient, HANDLE hIOServer, int nReferenceCountMax );
	virtual ~NetClientManager();

protected:
#ifdef _USE_TBB
	typedef tbb::concurrent_queue<UINT>				CQUEUE_CID;
	typedef tbb::concurrent_queue<UINT>::iterator	CQUEUE_CID_ITER;

	/*
	typedef tbb::interface5::concurrent_unordered_set<UINT>						CUSET_CID;
	typedef tbb::interface5::concurrent_unordered_set<UINT>::iterator			CUSET_CID_ITER;
	typedef tbb::interface5::concurrent_unordered_set<UINT>::const_iterator		CUSET_CID_CITER;
	*/

	typedef std::set<UINT>							SET_CID;
	typedef std::set<UINT>::iterator				SET_CID_ITER;
		
	//mjeon.TBB
	/*
	아래와 같이 2가지 방식(A,B)으로 구현할 수 있다.	
	
	- A -
	1.queue1는 소비(pop)만하고, queue2는 입력(push)만 받는다.
	2.만일 queue1이 empty되면 queue2 -> queue1로 전체 복사를 진행하고 queue2를 비운다.
	3.이 과정에서 중복키를 검사하기 위해 set을 filter로써 이용한다. (filtering을 위해서 queue1과 queue2로 나누는 것이다.)
	4.다시 1로 돌아가서 반복한다.

	- B -
	1.concurrent_hash_map에 insert()해본다. (중복체크)
	2.성공일 경우 queue에도 push한다.
	3.concurrent_hash_map에서 erase()해본다. (존재체크)
	4.성공일 경우 queue에서도 pop한다.
	5.이를 계속 반복

	성능상으로는 A의 방안이 더 좋을 것같아서 일단 A로 구현한다.

	typedef tbb::interface5::concurrent_hash_map<UINT, UINT>	CHSET_CID;	//mjeon.hash_set based on hash_map that the value is always 0.
	CHSET_CID	m_chmapCID;
	*/
		
	CQUEUE_CID	m_cqCID_Consumer;	//only pop
	CQUEUE_CID	m_cqCID_Producer;	//only push
	
#else
	DEQ_CLIENTS m_deqSleepCID;	//< 유휴 클라이언트 ID.
	MAP_CLIENTS	m_mapSleepCID;  //< 유휴 클라이언트 ID. ( 검색을 위해..), 필요없어 보인다. 나중에 삭제할것. 2012-1-10 jgkim
	VEC_CLIENTS m_vecSleepCID;	//< 반환되는 클라이언트 ID.
	//VEC_CONNECTIP m_vecConnectIP; // 접속하는 IP 리스트
#endif

	MAP_CHANUM m_mapChaNum;		//mjeon.AF.ChaNum - ClientSlot 맵
	
protected:	
	UINT m_nMaxClient; //< 접속가능한 최대 클라이언트수	

    std::vector<std::tr1::shared_ptr<CNetUser> > m_ClientMan; //< 사용자 정보를 담아둘 포인터

	HANDLE m_hIOServer;

    sc::net::IOManager* m_pIoMan; //! 보내기/받기 IOCP 메모리 관리자
	sc::CBit m_Bit; //< 기초적인 메모리 Bit 연산을 위한 멤버

	DWORD m_dwInPacketCount; //< 수신된 총 패킷 갯수
	DWORD m_dwInPacketSize; //< 수신된 총 패킷 사이즈
	
	DWORD m_dwOutPacketCount; //< 송신한 총 패킷 갯수
	DWORD m_dwOutPacketSize; //< 송신한 총 패킷 사이즈

	DWORD m_dwOutCompressCount; //< 송신한 총 압축 패킷 갯수
	DWORD m_dwOutCompressSize; //< 송신한 총 압축 패킷 크기

    CRITICAL_SECTION	m_CS_ClientMan;
	CRITICAL_SECTION	m_CS_CID;
	CRITICAL_SECTION	m_CS_ChaNum;

	LONG m_nReferenceCountMax;

#ifdef _USE_TBB
	typedef tbb::concurrent_unordered_map< std::string, sIPConnectCount >	MAP_CONNECTED_IP;
	typedef tbb::concurrent_unordered_map< DWORD, DWORD >					MAP_CONNECTED_USER_NUM;
#else
	typedef std::map< std::string, sIPConnectCount >	MAP_CONNECTED_IP;
	typedef std::map< DWORD, DWORD >					MAP_CONNECTED_USER_NUM;
#endif

	MAP_CONNECTED_IP			m_mapConnectedIP;
	MAP_CONNECTED_USER_NUM		m_mapConnectedUserNum;

public:
	bool VerifyIP( const std::string& IpAddress );
	bool VerifyUserNum( DWORD dwUserDbNum, EMSERVICE_PROVIDER emSP );
	void UpdateUserLogoutTime( DWORD dwUserDbNum );

	//! 수신된 총 패킷 갯수
	DWORD getInPacketCount() const { return m_dwInPacketCount; };

    //! 수신된 총 패킷 사이즈
	DWORD getInPacketSize() const { return m_dwInPacketSize; };

	//! 송신한 총 패킷 갯수
	DWORD getOutPacketCount() const { return m_dwOutPacketCount; };
	//! 송신한 총 패킷 사이즈
	DWORD getOutPacketSize() const { return m_dwOutPacketSize; };

	//! 송신한 총 압축 패킷 갯수
	DWORD getOutCompressCount() const { return m_dwOutCompressCount; };
	//! 송신한 총 압축 패킷 크기
	DWORD getOutCompressSize() const { return m_dwOutCompressSize; };

	//! 모든 패킷 카운터를 초기화 한다.
	void resetPacketCount();    
	
	int CloseClient( DWORD dwClient );	//! 특정클라이언트의 연결을 종료한다.
	void CloseAllClient(); //! 모든 클라이언트의 연결을 종료한다.

	LONG GetReferenceCountMax() { return m_nReferenceCountMax; }
	void SetReferenceCountMax(LONG nReferenceCountMax ) { m_nReferenceCountMax = nReferenceCountMax; }
	
#ifdef _USE_TBB
	void ResetPreSleepCID() {};
#else
	void ResetPreSleepCID();
#endif

	int GetFreeClientID(bool bForce=false);	//! 서버간 연결을 할때는 bForce 값을 true 로 해서 요청한다.
	void ReleaseClientID(DWORD dwClient);	

	//int SetAcceptedClient(DWORD dwClient, SOCKET sSocket);
	//void	SetNetMode			(DWORD dwClient, int nMode);
	//int		GetNetMode			(DWORD dwClient);	
	//
	//mjeon.SetAcceptedClient를 호출하는 시점에 SlotType이 명확하다면, 반드시 매개변수로 넘겨주도록 하자.
	//
	int SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType = net::SLOT_CLIENT);
    std::string GetClientIP(DWORD dwClient) const;
	USHORT  GetClientPort(DWORD dwClient);
	SOCKET	GetSocket(DWORD dwClient);
	void	CloseSocket(DWORD dwClient);

	int		GetMaxClient(); // Return max client number	
	int		GetCurrentClientNumber(); // 현재 접속중인 클라이언트 숫자	

	int	addRcvMsg(DWORD dwClient, void* pMsg, DWORD dwSize);

	void* getRcvMsg(DWORD dwClient);

	void ResetRcvBuffer(DWORD dwClient);	

	//! Buffer 에 남아있는 모든 data 를 전송한다.
	void SendClientFinal();

    //! Data 를 모았다가 합쳐서 압축해서 전송한다.
	int SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer);

    //! 버퍼링 하지 않고 즉시 전송한다.
	int SendClient2(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
	int SendClient2(DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);

	bool IsAccountPass(DWORD dwClient) const; //! 계정 인증(login) 통과하였는지 점검.
	void SetAccountPass(DWORD dwClient, bool bPASS); // 계정 인증(login) 통과 설정.

	void SetUse2ndPass(DWORD dwClient, int nUse2ndPass); 
	int IsUse2ndPass(DWORD dwClient) const; 
	
	CRYPT_KEY GetCryptKey		(DWORD dwClient);
	CRYPT_KEY GetNewCryptKey	(void);
	void	  SetCryptKey		(DWORD dwClient, CRYPT_KEY ck);
	
	net::SLOT_TYPE GetSlotType(DWORD dwClient);
	void	SetSlotType			(DWORD dwClient, net::SLOT_TYPE emType);

	void	ResetUserID(DWORD dwClient);

	void	ResetHeartBeat(DWORD dwClient);
	void	SetHeartBeat(DWORD dwClient);
	bool	CheckHeartBeat(DWORD dwClient);

	bool	IsClientSlot		(DWORD dwClient) const;
	bool	IsAgentSlot			(DWORD dwClient);
	//bool	IsFieldSlot			(DWORD dwClient);
	bool	IsOnline			(DWORD dwClient);
	bool	IsOffline			(DWORD dwClient);

	void	GspSetUserID(DWORD dwClient, const TCHAR* szGspUserID);
	TCHAR*	GspGetUserID(DWORD dwClient);

	void	SetUserID(DWORD dwClient, const TCHAR* szUserID);
    void	SetUserID(DWORD dwClient, const std::string& UserID);
	TCHAR*	GetUserID(DWORD dwClient);

	void	SetUserDbNum(DWORD dwClient, INT nUserNum);
    int		UserDbNum(DWORD dwClient) const;

	void	SetLoginTime        (DWORD dwClient);
	void	SetGaeaID           (DWORD dwClient, DWORD dwGaeaID);
	void	ResetGaeaID			(DWORD dwClient);	
	void	SetChaNum           (DWORD dwClient, int nChaNum);
	void    SetUserType         (DWORD dwClient, int nType=0);

	
	
	DWORD	GetUserCharObjID    (DWORD dwClient);
	INT		GetLoginTime	    (DWORD dwClient);
	CTime	GetLoginTimeEx		(DWORD dwClient);
	DWORD	GetGaeaID		    (DWORD dwClient);	
	int		ChaDbNum		    (DWORD dwClient) const;
	DWORD	GetClientByUserNum	(DWORD dwUserNum);
	DWORD	GetClientID			(DWORD dwChaNum);	//mjeon.AF.ChaNum으로 해당 ClientSlot 획득
	int		GetUserType         (DWORD dwClient);

	void    SetChaRemain        (DWORD dwClient, WORD wChaNum);
	void	SetChaTestRemain    (DWORD dwClient, WORD wChaNum);

	WORD    GetChaRemain	    (DWORD dwClient);
	WORD    GetChaTestRemain    (DWORD dwClient);

	void     SetChannel         (DWORD dwClient, int nChannel=0);
	int      GetChannel         (DWORD dwClient);

	bool	IsGaming            (DWORD dwClient);

    __time64_t GetPremiumDate   (DWORD dwClient);
    void       SetPremiumDate   (DWORD dwClient, __time64_t tTime);

	__time64_t GetChatBlockDate (DWORD dwClient);
    void       SetChatBlockDate (DWORD dwClient, __time64_t tTime);	

	Country::SCOUNTRY_INFO	GetCountryInfo ( DWORD dwClient );
	void					SetCountryInfo ( DWORD dwClient, const Country::SCOUNTRY_INFO& sCountryInfo );

	//! nProtect GameGuard 인증용 쿼리를 가져오기 전에 호출
	virtual bool nProtectSetAuthQuery(DWORD dwClient);
	virtual int nProtect30SetAuthQuery(DWORD dwClient);
    	
	//! nProtect GameGuard 인증용 쿼리 가져오기
	virtual GG_AUTH_DATA nProtectGetAuthQuery(DWORD dwClient);
	virtual BYTE* nProtect30GetAuthQuery(DWORD dwClient);

	//! nProtect GameGuard 인증 응답 가져오기
	virtual GG_AUTH_DATA nProtectGetAuthAnswer( DWORD dwClient );
	virtual BYTE* nProtect30GetAuthAnswer( DWORD dwClient );

	//! nProtect GameGuard 인증 응답 설정
	virtual void nProtectSetAuthAnswer(DWORD dwClient, GG_AUTH_DATA& ggad);
	virtual void nProtect30SetAuthAnswer(DWORD dwClient, BYTE* ggad, int nPacketSeq);

	//! nProtect GameGuard 인증 결과 가져오기
	virtual DWORD nProtectCheckAuthAnswer(DWORD dwClient);

	//! nProtect GameGuard 인증 상태를 리셋시킨다.
	virtual void nProtectResetAuth(DWORD dwClient);

	PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE nType=NET_SEND_POSTED);
	void ReleaseOperationData(PER_IO_OPERATION_DATA* pData);

	//! 랜덤패스워드 번호를 설정한다.
	void SetRandomPassNumber(DWORD dwClient, int nNumber);

	//! 랜덤패스워드 번호를 가져온다.
	int GetRandomPassNumber(DWORD dwClient);

	void ResetRandomPassNumber(DWORD dwCient);

	// 극강부 생성 여부를 설정한다;
	void SetExtremeCreate( DWORD dwClient, bool bExtreme );

	// 극강부 생성 여부를 가져온다;
	bool GetExtreme( DWORD dwClient );

	void SetJumpingCreate( DWORD dwClient, bool bJumping );

	bool GetJumping( DWORD dwClient );

	// 변술부 생성 제한;
	void SetActorCreate ( DWORD dwClient, bool bCreate );
	const bool GetCreateActor ( DWORD dwClient );

	// 태국 User Class Type을 설정한다.
	void SetIPBonusClass( DWORD dwClient, EMIP_BONUS_CLASS emIPBonus );

	// 태국 User Class Type을 가져온다.
	EMIP_BONUS_CLASS GetIPBonusClass( DWORD dwClient );

	// 중국 GameTime을 세팅해 둔다.
	void SetChinaGameTime( DWORD dwClient, int nChinaGameTime );

	// 중국 GameTime을 가지고 온다.
	int GetChinaGameTime( DWORD dwClient );

	// 중국 OfflineTime을 세팅해 둔다.
	void SetChinaOfflineTime( DWORD dwClient, int nChinaOfflineTime );

	// 중국 OfflineTime을 가지고 온다.
	int GetChinaOfflineTime( DWORD dwClient );

	// 중국 LastLoginDate를 세팅하고 가지고 온다.
	void       SetLastLoginDate   ( DWORD dwClient, __time64_t tTime );
	__time64_t GetLastLoginDate   ( DWORD dwClient );

	// 중국 UserAge를 설정해 둔다.
	void SetChinaUserAge( DWORD dwClient, int nChinaUserAge );

	// 중국 UserAge를 가지고 온다.
	int GetChinaUserAge( DWORD dwClient );

	// 베트남 GameTime을 세팅해 둔다.
	void SetVTGameTime( DWORD dwClient, int nChinaGameTime );

	// 베트남 GameTime을 가지고 온다.
	int GetVTGameTime( DWORD dwClient );

	// 메세지를 저장해 뒀다가 가져온다.
	//NET_MSG_GENERIC * GetNetMsg( DWORD dwClient );

    //  특정시간내에 클라이언트가 보내온 패킷을 카운트한다.
    bool IncPacketCount( DWORD dwClient, int nMsgID );
    void WritePacketLog( DWORD dwClient );

	// reference count
	void AddReference( DWORD dwClient, bool bIOCheck = true );
	void ReleaseReference( DWORD dwClient );
	LONG GetReferenceCount( DWORD dwClient );
	DWORD GetLastRequestIOTime( DWORD dwClient );
	void CleanDummyClientSlot();

#ifdef _USE_TBB
	//mjeon.concurrent_hash_map's size() takes constant time, but is slower than for most STL containers.

	//mjeon.ToDo
    inline size_t GetDeqSleepSize() const { return 0; }	//TBB를 사용할 경우에는 호출부를 찾아서 모두 다른 방식으로 출력해주자.
    inline size_t GetVecSleepSize() const { return 0; }
#else
    //! 유휴 클라이언트 ID.    
    inline size_t GetDeqSleepSize() const { return m_deqSleepCID.size(); }
    //! 반환되는 클라이언트 ID.
    inline size_t GetVecSleepSize() const { return m_vecSleepCID.size(); }
#endif
};

#endif // _NET_CLIENT_MANAGER_H_
