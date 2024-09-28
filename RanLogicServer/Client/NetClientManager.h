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

#define NUM_OF_RESERVED_CID_FOR_SERVER_CONNECTIONS	20	//������ ����� ���� �ּ��� Ȯ���Ǿ� �־���� CID�� ����

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
	�Ʒ��� ���� 2���� ���(A,B)���� ������ �� �ִ�.	
	
	- A -
	1.queue1�� �Һ�(pop)���ϰ�, queue2�� �Է�(push)�� �޴´�.
	2.���� queue1�� empty�Ǹ� queue2 -> queue1�� ��ü ���縦 �����ϰ� queue2�� ����.
	3.�� �������� �ߺ�Ű�� �˻��ϱ� ���� set�� filter�ν� �̿��Ѵ�. (filtering�� ���ؼ� queue1�� queue2�� ������ ���̴�.)
	4.�ٽ� 1�� ���ư��� �ݺ��Ѵ�.

	- B -
	1.concurrent_hash_map�� insert()�غ���. (�ߺ�üũ)
	2.������ ��� queue���� push�Ѵ�.
	3.concurrent_hash_map���� erase()�غ���. (����üũ)
	4.������ ��� queue������ pop�Ѵ�.
	5.�̸� ��� �ݺ�

	���ɻ����δ� A�� ����� �� ���� �Ͱ��Ƽ� �ϴ� A�� �����Ѵ�.

	typedef tbb::interface5::concurrent_hash_map<UINT, UINT>	CHSET_CID;	//mjeon.hash_set based on hash_map that the value is always 0.
	CHSET_CID	m_chmapCID;
	*/
		
	CQUEUE_CID	m_cqCID_Consumer;	//only pop
	CQUEUE_CID	m_cqCID_Producer;	//only push
	
#else
	DEQ_CLIENTS m_deqSleepCID;	//< ���� Ŭ���̾�Ʈ ID.
	MAP_CLIENTS	m_mapSleepCID;  //< ���� Ŭ���̾�Ʈ ID. ( �˻��� ����..), �ʿ���� ���δ�. ���߿� �����Ұ�. 2012-1-10 jgkim
	VEC_CLIENTS m_vecSleepCID;	//< ��ȯ�Ǵ� Ŭ���̾�Ʈ ID.
	//VEC_CONNECTIP m_vecConnectIP; // �����ϴ� IP ����Ʈ
#endif

	MAP_CHANUM m_mapChaNum;		//mjeon.AF.ChaNum - ClientSlot ��
	
protected:	
	UINT m_nMaxClient; //< ���Ӱ����� �ִ� Ŭ���̾�Ʈ��	

    std::vector<std::tr1::shared_ptr<CNetUser> > m_ClientMan; //< ����� ������ ��Ƶ� ������

	HANDLE m_hIOServer;

    sc::net::IOManager* m_pIoMan; //! ������/�ޱ� IOCP �޸� ������
	sc::CBit m_Bit; //< �������� �޸� Bit ������ ���� ���

	DWORD m_dwInPacketCount; //< ���ŵ� �� ��Ŷ ����
	DWORD m_dwInPacketSize; //< ���ŵ� �� ��Ŷ ������
	
	DWORD m_dwOutPacketCount; //< �۽��� �� ��Ŷ ����
	DWORD m_dwOutPacketSize; //< �۽��� �� ��Ŷ ������

	DWORD m_dwOutCompressCount; //< �۽��� �� ���� ��Ŷ ����
	DWORD m_dwOutCompressSize; //< �۽��� �� ���� ��Ŷ ũ��

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

	//! ���ŵ� �� ��Ŷ ����
	DWORD getInPacketCount() const { return m_dwInPacketCount; };

    //! ���ŵ� �� ��Ŷ ������
	DWORD getInPacketSize() const { return m_dwInPacketSize; };

	//! �۽��� �� ��Ŷ ����
	DWORD getOutPacketCount() const { return m_dwOutPacketCount; };
	//! �۽��� �� ��Ŷ ������
	DWORD getOutPacketSize() const { return m_dwOutPacketSize; };

	//! �۽��� �� ���� ��Ŷ ����
	DWORD getOutCompressCount() const { return m_dwOutCompressCount; };
	//! �۽��� �� ���� ��Ŷ ũ��
	DWORD getOutCompressSize() const { return m_dwOutCompressSize; };

	//! ��� ��Ŷ ī���͸� �ʱ�ȭ �Ѵ�.
	void resetPacketCount();    
	
	int CloseClient( DWORD dwClient );	//! Ư��Ŭ���̾�Ʈ�� ������ �����Ѵ�.
	void CloseAllClient(); //! ��� Ŭ���̾�Ʈ�� ������ �����Ѵ�.

	LONG GetReferenceCountMax() { return m_nReferenceCountMax; }
	void SetReferenceCountMax(LONG nReferenceCountMax ) { m_nReferenceCountMax = nReferenceCountMax; }
	
#ifdef _USE_TBB
	void ResetPreSleepCID() {};
#else
	void ResetPreSleepCID();
#endif

	int GetFreeClientID(bool bForce=false);	//! ������ ������ �Ҷ��� bForce ���� true �� �ؼ� ��û�Ѵ�.
	void ReleaseClientID(DWORD dwClient);	

	//int SetAcceptedClient(DWORD dwClient, SOCKET sSocket);
	//void	SetNetMode			(DWORD dwClient, int nMode);
	//int		GetNetMode			(DWORD dwClient);	
	//
	//mjeon.SetAcceptedClient�� ȣ���ϴ� ������ SlotType�� ��Ȯ�ϴٸ�, �ݵ�� �Ű������� �Ѱ��ֵ��� ����.
	//
	int SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType = net::SLOT_CLIENT);
    std::string GetClientIP(DWORD dwClient) const;
	USHORT  GetClientPort(DWORD dwClient);
	SOCKET	GetSocket(DWORD dwClient);
	void	CloseSocket(DWORD dwClient);

	int		GetMaxClient(); // Return max client number	
	int		GetCurrentClientNumber(); // ���� �������� Ŭ���̾�Ʈ ����	

	int	addRcvMsg(DWORD dwClient, void* pMsg, DWORD dwSize);

	void* getRcvMsg(DWORD dwClient);

	void ResetRcvBuffer(DWORD dwClient);	

	//! Buffer �� �����ִ� ��� data �� �����Ѵ�.
	void SendClientFinal();

    //! Data �� ��Ҵٰ� ���ļ� �����ؼ� �����Ѵ�.
	int SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer);

    //! ���۸� ���� �ʰ� ��� �����Ѵ�.
	int SendClient2(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
	int SendClient2(DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);

	bool IsAccountPass(DWORD dwClient) const; //! ���� ����(login) ����Ͽ����� ����.
	void SetAccountPass(DWORD dwClient, bool bPASS); // ���� ����(login) ��� ����.

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
	DWORD	GetClientID			(DWORD dwChaNum);	//mjeon.AF.ChaNum���� �ش� ClientSlot ȹ��
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

	//! nProtect GameGuard ������ ������ �������� ���� ȣ��
	virtual bool nProtectSetAuthQuery(DWORD dwClient);
	virtual int nProtect30SetAuthQuery(DWORD dwClient);
    	
	//! nProtect GameGuard ������ ���� ��������
	virtual GG_AUTH_DATA nProtectGetAuthQuery(DWORD dwClient);
	virtual BYTE* nProtect30GetAuthQuery(DWORD dwClient);

	//! nProtect GameGuard ���� ���� ��������
	virtual GG_AUTH_DATA nProtectGetAuthAnswer( DWORD dwClient );
	virtual BYTE* nProtect30GetAuthAnswer( DWORD dwClient );

	//! nProtect GameGuard ���� ���� ����
	virtual void nProtectSetAuthAnswer(DWORD dwClient, GG_AUTH_DATA& ggad);
	virtual void nProtect30SetAuthAnswer(DWORD dwClient, BYTE* ggad, int nPacketSeq);

	//! nProtect GameGuard ���� ��� ��������
	virtual DWORD nProtectCheckAuthAnswer(DWORD dwClient);

	//! nProtect GameGuard ���� ���¸� ���½�Ų��.
	virtual void nProtectResetAuth(DWORD dwClient);

	PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE nType=NET_SEND_POSTED);
	void ReleaseOperationData(PER_IO_OPERATION_DATA* pData);

	//! �����н����� ��ȣ�� �����Ѵ�.
	void SetRandomPassNumber(DWORD dwClient, int nNumber);

	//! �����н����� ��ȣ�� �����´�.
	int GetRandomPassNumber(DWORD dwClient);

	void ResetRandomPassNumber(DWORD dwCient);

	// �ذ��� ���� ���θ� �����Ѵ�;
	void SetExtremeCreate( DWORD dwClient, bool bExtreme );

	// �ذ��� ���� ���θ� �����´�;
	bool GetExtreme( DWORD dwClient );

	void SetJumpingCreate( DWORD dwClient, bool bJumping );

	bool GetJumping( DWORD dwClient );

	// ������ ���� ����;
	void SetActorCreate ( DWORD dwClient, bool bCreate );
	const bool GetCreateActor ( DWORD dwClient );

	// �±� User Class Type�� �����Ѵ�.
	void SetIPBonusClass( DWORD dwClient, EMIP_BONUS_CLASS emIPBonus );

	// �±� User Class Type�� �����´�.
	EMIP_BONUS_CLASS GetIPBonusClass( DWORD dwClient );

	// �߱� GameTime�� ������ �д�.
	void SetChinaGameTime( DWORD dwClient, int nChinaGameTime );

	// �߱� GameTime�� ������ �´�.
	int GetChinaGameTime( DWORD dwClient );

	// �߱� OfflineTime�� ������ �д�.
	void SetChinaOfflineTime( DWORD dwClient, int nChinaOfflineTime );

	// �߱� OfflineTime�� ������ �´�.
	int GetChinaOfflineTime( DWORD dwClient );

	// �߱� LastLoginDate�� �����ϰ� ������ �´�.
	void       SetLastLoginDate   ( DWORD dwClient, __time64_t tTime );
	__time64_t GetLastLoginDate   ( DWORD dwClient );

	// �߱� UserAge�� ������ �д�.
	void SetChinaUserAge( DWORD dwClient, int nChinaUserAge );

	// �߱� UserAge�� ������ �´�.
	int GetChinaUserAge( DWORD dwClient );

	// ��Ʈ�� GameTime�� ������ �д�.
	void SetVTGameTime( DWORD dwClient, int nChinaGameTime );

	// ��Ʈ�� GameTime�� ������ �´�.
	int GetVTGameTime( DWORD dwClient );

	// �޼����� ������ �״ٰ� �����´�.
	//NET_MSG_GENERIC * GetNetMsg( DWORD dwClient );

    //  Ư���ð����� Ŭ���̾�Ʈ�� ������ ��Ŷ�� ī��Ʈ�Ѵ�.
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
    inline size_t GetDeqSleepSize() const { return 0; }	//TBB�� ����� ��쿡�� ȣ��θ� ã�Ƽ� ��� �ٸ� ������� ���������.
    inline size_t GetVecSleepSize() const { return 0; }
#else
    //! ���� Ŭ���̾�Ʈ ID.    
    inline size_t GetDeqSleepSize() const { return m_deqSleepCID.size(); }
    //! ��ȯ�Ǵ� Ŭ���̾�Ʈ ID.
    inline size_t GetVecSleepSize() const { return m_vecSleepCID.size(); }
#endif
};

#endif // _NET_CLIENT_MANAGER_H_
