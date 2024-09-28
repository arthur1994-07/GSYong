#ifndef S_NETUSER_H_
#define S_NETUSER_H_

#pragma once

#include <boost/circular_buffer.hpp>
#include "../../=CryptoPP/cryptlib.h"
#include "../../=CryptoPP/secblock.h"
#include "../../=CryptoPP/dh.h"
#include "../../SigmaCore/Util/Lock.h"
#include "../../SigmaCore/Util/CBit.h"

#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/WorldBattle/WorldBattleDefine.h"
#include "../Country/GLCountryManServer.h"
//#include "../../RanLogic/Util/s_CAddressChecker.h"

namespace sc {
    class Aes;
}

class CSendMsgBuffer;
class CRcvMsgBuffer;

/**
 * \ingroup NetServerLib
 *
 * \par requirements
 * win2k or later\n
 * MFC\n
 *
 * \version 1.1
 *
 * \date 2003-02-28
 *       2005-12-12 Jgkim, Add nProtect user query module
 *
 * \author jgkim
 *
 * \par license
 * Copyright(c) Mincoms. All rights reserved.
 *
 * \todo 
 *
 * \bug 
 *
 */
class CNetUser : public sc::DefaultLock
{
public:
	CNetUser();
	virtual ~CNetUser();

protected:
	USER_INFO_BASIC		m_uib; /// Basic user information

	std::set<DWORD>		m_setChaNum;		//�� ������ �����ϰ� �ִ� ĳ���� ��ȣ�� ���

	bool				m_bACCOUNT_PASS;    // �����˻縦 ����ߴ��� ����
	bool                m_bACCOUNT_PASSING; // �����˻� ��������� �˻�.
	int					m_nUse2ndPass;

	SOCKET				m_Socket;
    net::EM_NET_STATE   m_emOnline;
	
    std::string			m_IpAddress;
	USHORT				m_usPort;
	DWORD				m_dwLastMsgTime;	// ���������� �б� ����� �� �ð�
	DWORD				m_dwLastSaveTime;   // ���������� ĳ���͸� ������ �ð�
	int					m_nPing;			// Ŭ���̾�Ʈ�� ��ſ� �ɸ� �ð�	
	CTime				m_LoginTime;		// �α����� ������ �ð�
	
	DWORD				m_dwGaeaID;			// ���������� '���̾�'������ ����ID.
	int					m_ChaDbNum;         // ���� ���ӿ� �������� ĳ���� DB ��ȣ
	int					m_nUserType;        // ������ Ÿ�� (�Ϲ�����, GM, Master)
    
	__time64_t          m_tPremiumDate;     // �����̾����� ����ð�.
	__time64_t          m_tChatBlockDate;   // ä�ú�� ����ð�.

    net::SLOT_TYPE		m_SlotType;

	CRYPT_KEY			m_ck;				// �⺻��ȣȭ ��ȣŰ (���� ������ ����)
	bool				m_bHeartBeat;		// ��Ʈ��Ʈ üũ����
	WORD				m_wChaRemain;
	WORD				m_wChaTestRemain;

	int					m_nChannel;				// ����ڰ� ������ ä��
	WORD                m_wInvalidPacketCount;	// �߸��� ��Ŷ�� ���� Ƚ��
	WORD                m_wLoginAttempt;		// �ѹ� ���ӿ� �α��� ��û Ƚ��
	WORD				m_wConSendError;		// ���ӵ� ���� ���� Ƚ��, üũ �� Ŭ���̾�Ʈ ���� ���Ḧ ���� ���.
	
	sc::CBit m_Bit;
	
	CSendMsgBuffer*  m_pSendManager; ///< �޽��� ������ �޴���
	CRcvMsgBuffer*   m_pRcvManager; ///< �޽��� �ޱ� �޴���

	int	m_nRandomPass; ///< �����н����� üũ�� ���� ����

	// �ذ��� üũ
	bool				m_bExtreme;

	// ������ üũ
	bool				m_bActor;

	// �±� User Class Type
	EMIP_BONUS_CLASS	m_emIPBonus;

	// �߱� �α��� �ð� üũ
	int					m_nChinaGameTime;
	int					m_nChinaOfflineTime;
	__time64_t			m_tLastLoginTime;
	int					m_nChinaUserAge;

	// ��Ʈ�� Ž�й��� ���� �߰�
	int					m_nVTGameTime;

	// Thailand
	WORD m_wThaiFlag; /// Thailand ����� �÷���
	WORD m_wThaiTodayGamePlayTime; /// Thailand ������� �����÷��̽ð�(��)	
	
	// Malaysia
	TCHAR m_szDecodedTID[TERRA_TID_DECODE+1]; /// Malaysia User information	

	// GSP
	TCHAR m_szUUID[sc::string::UUID_LENGTH]; /// UUID User length	

	// Encrypt
	TCHAR m_szEncryptKeyHeart[ENCRYPT_KEY+1];

    WORD m_wAge; //! ����, �ѱ� �˴ٿ��� üũ��

	Country::SCOUNTRY_INFO m_userCountry;	//! ��� �� ���� ����;

    wb::EM_COUNTRY m_Country; //! ����

    enum 
    {
        MAX_PACKET_COUNT = 900,
        PACKET_LOG_SIZE = 10,
        UPDATE_PACKET_TIME = 3000, // 10��
    };

    // MsgCount Check
    DWORD m_dwLastCheckTime;
    LONG m_nPacketCount;
    boost::circular_buffer<int> m_cMsgBuffer;

	// reference count
	LONG m_nRequestedIOCount;

	// ���������� io ��û�� �ð�
	DWORD m_dwLastRequestIOTime;

	bool m_bJumping;

public:	

	//! ��� �� ���� ����;
	inline Country::SCOUNTRY_INFO GetUserCountry () const { return m_userCountry; }
	void SetUserCountry ( const Country::SCOUNTRY_INFO& userCountry );
    
    //! ����
    wb::EM_COUNTRY GetCountry() const { return m_Country; }
    void SetCountry(wb::EM_COUNTRY Country) { m_Country = Country; }

    //! ����, �ѱ� �˴ٿ��� üũ��
    WORD GetAge() const { return m_wAge; }
    void SetAge(WORD Age) { m_wAge = Age; }

	//! �������� �޽����� ����Ѵ�.
	int addSendMsg(NET_MSG_GENERIC* pMsg, DWORD dwSize);

	//! �������� �޽��� ���۸� �����´�.
	void* getSendBuffer();

	//! �������� �޽��� ũ�⸦ �����´�.
	int getSendSize();

	//! Ŭ���̾�Ʈ���� ���� �޽����� �ִ´�.
	int addRcvMsg(void* pMsg, DWORD dwSize);

	//! Ŭ���̾�Ʈ���� ���� �޽����� �ϳ� �����´�.
	void* getRcvMsg();

	//! Ŭ���̾�Ʈ ���� ����
	void Reset();

	//! Ŭ���̾�Ʈ ���� ����
	void ResetRcvBuffer();

	//! Ŭ���̾�Ʈ �����Ŵ
	// DWORD dwClient ���� Ȯ�� �뵵
	int CloseClient( DWORD dwClient );

	inline bool IsOnline() { return m_Socket == INVALID_SOCKET ? false : true; }
	inline net::EM_NET_STATE GetNetState() { return m_emOnline; }
	//inline bool IsOnline()   { return m_emOnline == net::ONLINE ? true : false; }
	//inline bool IsOffline()  { return m_emOnline == net::OFFLINE ? true : false; }
	//inline void SetOnLine()  { m_emOnline = net::ONLINE; }
	//inline void SetOffLine() { m_emOnline = net::OFFLINE; }

	void	CloseSocket();
	
	int     InsertBuffer( BYTE* buff, DWORD dwSize );

	int		SetAcceptedClient(SOCKET s);
	//void	SetSocket(SOCKET s);
    void	SetIP(const std::string& IpAddress);
	void	SetLoginTime();	

	void	SetPort(USHORT usPort);
	//void	SetNetMode(int nMode);

	void	SetUserDbNum(int nUserNum);
	
	void    GspSetUUID( const TCHAR* szUUID );
	TCHAR*  GspGetUUID();

	void	KorSetUUID( const TCHAR* szUUID );
	TCHAR*	KorGetUUID();

	void	JPSetUUID( const TCHAR* szUUID );
	TCHAR*	JPGetUUID();

	void    TerraSetDecodedTID( const TCHAR* szDecodedTID );
	TCHAR*  TerraGetDecodedTID();

	void    SetEncKey( const TCHAR* szEncKey );
	TCHAR*  GetEncKey();	
	
	void	SetCryptKey(CRYPT_KEY ck);	

	void	SetGaeaID         (DWORD dwGaeaID);
	
	//mjeon.AF.AgentServer���� ���� ĳ���� ���� �ÿ� ������������ ���ȴ�. FieldServer������ �ǹ̰� ����.
	void	ResetValidChaNum() {m_setChaNum.clear();}
	void	ValidateChaNum(DWORD dwChaNum);
	void	InvalidateChaNum(DWORD dwChaNum);
	BOOL	IsValidChaNum(DWORD dwChaNum);

	void	ChaDbNum		  (int	 nChaNum);
	void	SetUserType       (int   nType);
	void    SetChaRemain	  (WORD wNum);
	void	SetChaTestRemain  (WORD wNum);
	WORD    GetChaRemain      ();
	WORD    GetChaTestRemain  ();
	void	SetChannel        (int nChannel);
	int     GetChannel        ();
	
	void	SetSlotType		  (net::SLOT_TYPE dwType);	

	void	ResetUserID();
	TCHAR*	GetUserID();	
	void	SetUserID(const TCHAR* szUserID);
    void	SetUserID(const std::string& UserID);

	TCHAR*  GspGetUserID();
	void    GspSetUserID( const TCHAR* szGspUserID );


	SOCKET	GetSOCKET();
    std::string GetIP() const;
	USHORT	GetPort();	
	//int		GetNetMode();

	int		UserDbNum();		
	
	DWORD	GetGaeaID();
	int		GetChaDbNum() const;
	int		GetUserType();
	
	net::SLOT_TYPE GetSlotType();
	
	inline bool IsClientSlot()		{ return m_SlotType == net::SLOT_CLIENT       ? true : false; }	
	inline bool IsAgentSlot()		{ return m_SlotType == net::SLOT_AGENT        ? true : false; }
    inline bool IsCacheSlot()		{ return m_SlotType == net::SLOT_CACHE        ? true : false; }
    inline bool IsSessionSlot()		{ return m_SlotType == net::SLOT_SESSION      ? true : false; }
    inline bool IsAgentFieldSlot()	{ return m_SlotType == net::SLOT_AGENT_FIELD  ? true : false; }
	inline bool IsMatchSlot()		{ return m_SlotType == net::SLOT_MATCH		  ? true : false; }
	inline bool IsInstanceSlot()	{ return m_SlotType == net::SLOT_INSTANCE	  ? true : false; }
	inline bool IsAgentAgentSlot()	{ return m_SlotType == net::SLOT_AGENT_AGENT  ? true : false; }
	inline bool IsIntegrationCacheSlot() { return m_SlotType == net::SLOT_INTEGRATION_CACHE ? true : false; }

	bool IsAccountPass() const;
	void SetAccountPass(bool bPASS);

	int IsUse2ndPass() const;
	void SetUse2ndPass(int nUse2ndPass);

	bool IsAccountPassing() const;			//! �����˻� ��������� �˻�
	void SetAccountPassing(bool bPassing);	//! �����˻� ��������� ����

	CTime	GetLoginTime();	

	inline void ResetHeartBeat() { m_bHeartBeat = false; }
	inline void SetHeartBeat()   { m_bHeartBeat = true; }
	inline bool CheckHeartBeat() { 	return m_bHeartBeat == true ? true : false; }

	CRYPT_KEY GetCryptKey();

    __time64_t GetPremiumDate();
    void       SetPremiumDate(__time64_t tTime);

	__time64_t GetChatBlockDate();
    void       SetChatBlockDate(__time64_t tTime);


	// �ذ��� üũ 	
	void	SetExtremeCreate( bool bExtreme );

	bool	GetExtreme();

	void	SetJumpingCreate( bool bJumping );
	bool	GetJumping();

	// ������ üũ;
	void		SetActorCreate ( bool bCreate );
	const bool	GetCreateActor ();

	// �±� User Class Type�� �����Ѵ�.
	void SetIPBonusClass( EMIP_BONUS_CLASS emIPBonus );

	// �±� User Class Type�� �����´�.
	EMIP_BONUS_CLASS GetIPBonusClass();


	// �߱� GameTime, OfflineTime üũ	
	void SetChinaGameTime( int nChinaGameTime );
	void SetChinaOfflineTime( int nChinaOfflineTime );
	void SetLastLoginDate( __time64_t tTime );
	void SetChinaUserAge( int nChinaUserAge );

	int GetChinaGameTime();
	int GetChinaOfflineTime();
	__time64_t GetLastLoginDate();
	int GetChinaUserAge();


	// ��Ʈ�� GameTime üũ
	void SetVTGameTime( int nVTGameTime );
	int GetVTGameTime();
        
    //  Ư���ð����� Ŭ���̾�Ʈ�� ������ ��Ŷ�� ī��Ʈ�Ѵ�.
    bool IncPacketCount( int nMsgID );
    void WritePacketLog();
	
	//! ���������� ��û���� 1 ȸ ���ӽ� 5 ȸ �̻� �α��� ��û�� 
	//! ������� ������ ����...
	//! �α��� ��û Ƚ�� ������Ŵ
	void IncLoginAttempt();

	//! �α��� ��û Ƚ�� ������
	WORD GetLoginAttempt();	

	void SetRandomPassNumber(int nNumber);
	int GetRandomPassNumber();
	void ResetRandomPassNumber();


	// Thailand
	WORD ThaiGetFlag ();
	void ThaiSetFlag (WORD wFlag);
	void ThaiSetTodayPlayTime (WORD wMinutes);
	bool ThaiIs3HourOver (CTime& currentTime);

	// reference count
	void AddReference( DWORD dwClient );
	void ReleaseReference( DWORD dwClient );
	LONG GetReferenceCount();
	DWORD GetLastRequestIOTime();
	void ResetReference();


	// nProtect
protected:

#ifndef _DEBUG
#ifndef _NP30
	CCSAuth2            m_csa; ///< nProtect GameGuard ������ Ŭ����
#else
	CCSAuth3            m_csa;
	int					m_casSeq;
#endif
#endif

	bool                m_bNProtectAuth;
	bool                m_bNProtectFirstCheck; ///< ������ nProtect �� ó�� �����޴°�?	
	bool                m_bNProtectFirstAuthPass; ///< ���ӿ� �����ϱ� ���� ��� ������ �Ϸ��ߴ°�?
	
public:
	//! nProtect ó�� �����ΰ�?
	bool nProtectIsFirstCheck();

	//! nProtect ó�� �������� ����
	void nProtectSetFirstCheck(bool bState);

	//! nProtect GameGuard ������ ������ �������� ���� ȣ��
	bool nProtectSetAuthQuery();
	int nProtect30SetAuthQuery();
	
    	
	//! nProtect GameGuard ������ ���� ��������
	GG_AUTH_DATA nProtectGetAuthQuery();
	BYTE* nProtect30GetAuthQuery();

	

	//! nProtect GameGuard ���� ���� ��������
	GG_AUTH_DATA nProtectGetAuthAnswer();
	BYTE* nProtect30GetAuthAnswer();


	//! nProtect GameGuard ���� ���� ����
	void nProtectSetAuthAnswer(GG_AUTH_DATA& ggad);
	void nProtect30SetAuthAnswer(BYTE* ggad, int nPacketSeq);

	//! nProtect GameGuard ���� ��� ��������
	DWORD nProtectCheckAuthAnswer();
	DWORD nProtect30CheckAuthAnswer(UINT32 nReceivedSize);

	//! nProtect GameGuard ���� ���¸� ���½�Ų��.
	void nProtectResetAuth();

	//! nProtect GameGuard ó�� ������ ����ߴ°�?
	//! �� ������ ����ؾ� �����÷��̸� �� �� �ִ�.
	bool nProtectFirstAuthPass();

	//! nProtect GameGuard ó������ ������� ����
	void nProtectSetFirstAuthPass(bool bState);

    //! -----------------------------------------------------------------------
    //! AES ��ȣȭ
    //! -----------------------------------------------------------------------
    /*
protected:
    CRITICAL_SECTION m_AesLock;
    sc::Aes* m_pAes;

public:
    //! server �� public key �� �����´�.
    //! �� key �� client �� �����ؾ� �Ѵ�.
    //! \param pPubKey key �� ���� ���� 32 byte �̻�
    //! \param nKeyLength ����Ű�� ũ�⸦ �����ش�.
    void GetServerPubKey(BYTE* pPubKey, int& nKeyLength OUT);

    void SetServerPubKey(BYTE* pPubKey);

    //! Client->Server Client PubKey
    bool SetClientPubKey(BYTE* pPubKey, int nKeyLength);

    void GetClientPubKey(BYTE* pPubKey, int& nKeyLength OUT);

    //! \param pBuffer ��ȣȭ�� ����
    //! \nLength ��ȣȭ�� ������ ����.
    bool EncryptBuffer(BYTE* pBuffer, int nLength);

    //! \param pBuffer ��ȣȭ�� ����
    //! \nLength ��ȣȭ�� ������ ����.
    bool DecryptBuffer(BYTE* pBuffer, int nLength);

    // ----------------------------------------------------------------------------
#ifdef _DEBUG
    CryptoPP::SecByteBlock* AES_getSecretKey();
    CryptoPP::DH* AES_getDH();
    byte* AES_getIV();
#endif
    // ----------------------------------------------------------------------------
    */
};

#endif // S_NETUSER_H_
