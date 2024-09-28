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

	std::set<DWORD>		m_setChaNum;		//이 유저가 보유하고 있는 캐릭터 번호의 목록

	bool				m_bACCOUNT_PASS;    // 계정검사를 통과했는지 여부
	bool                m_bACCOUNT_PASSING; // 계정검사 통과중인지 검사.
	int					m_nUse2ndPass;

	SOCKET				m_Socket;
    net::EM_NET_STATE   m_emOnline;
	
    std::string			m_IpAddress;
	USHORT				m_usPort;
	DWORD				m_dwLastMsgTime;	// 마지막으로 읽기 통신이 된 시간
	DWORD				m_dwLastSaveTime;   // 마지막으로 캐릭터를 저장한 시간
	int					m_nPing;			// 클라이언트와 통신에 걸린 시간	
	CTime				m_LoginTime;		// 로그인을 성공한 시각
	
	DWORD				m_dwGaeaID;			// 게임참여시 '가이아'에서의 고유ID.
	int					m_ChaDbNum;         // 현재 게임에 참여중인 캐릭터 DB 번호
	int					m_nUserType;        // 유저의 타입 (일반유저, GM, Master)
    
	__time64_t          m_tPremiumDate;     // 프리미엄서비스 만료시간.
	__time64_t          m_tChatBlockDate;   // 채팅블록 만료시간.

    net::SLOT_TYPE		m_SlotType;

	CRYPT_KEY			m_ck;				// 기본암호화 암호키 (현재 사용되지 않음)
	bool				m_bHeartBeat;		// 하트비트 체크여부
	WORD				m_wChaRemain;
	WORD				m_wChaTestRemain;

	int					m_nChannel;				// 사용자가 참여한 채널
	WORD                m_wInvalidPacketCount;	// 잘못된 패킷을 보낸 횟수
	WORD                m_wLoginAttempt;		// 한번 접속에 로그인 요청 횟수
	WORD				m_wConSendError;		// 연속된 전송 에러 횟수, 체크 후 클라이언트 강제 종료를 위해 사용.
	
	sc::CBit m_Bit;
	
	CSendMsgBuffer*  m_pSendManager; ///< 메시지 보내기 메니저
	CRcvMsgBuffer*   m_pRcvManager; ///< 메시지 받기 메니저

	int	m_nRandomPass; ///< 랜덤패스워드 체크를 위한 변수

	// 극강부 체크
	bool				m_bExtreme;

	// 변술부 체크
	bool				m_bActor;

	// 태국 User Class Type
	EMIP_BONUS_CLASS	m_emIPBonus;

	// 중국 로그인 시간 체크
	int					m_nChinaGameTime;
	int					m_nChinaOfflineTime;
	__time64_t			m_tLastLoginTime;
	int					m_nChinaUserAge;

	// 베트남 탐닉방지 관련 추가
	int					m_nVTGameTime;

	// Thailand
	WORD m_wThaiFlag; /// Thailand 사용자 플래그
	WORD m_wThaiTodayGamePlayTime; /// Thailand 사용자의 게임플레이시간(분)	
	
	// Malaysia
	TCHAR m_szDecodedTID[TERRA_TID_DECODE+1]; /// Malaysia User information	

	// GSP
	TCHAR m_szUUID[sc::string::UUID_LENGTH]; /// UUID User length	

	// Encrypt
	TCHAR m_szEncryptKeyHeart[ENCRYPT_KEY+1];

    WORD m_wAge; //! 나이, 한국 셧다운제 체크용

	Country::SCOUNTRY_INFO m_userCountry;	//! 대륙 및 국가 정보;

    wb::EM_COUNTRY m_Country; //! 국가

    enum 
    {
        MAX_PACKET_COUNT = 900,
        PACKET_LOG_SIZE = 10,
        UPDATE_PACKET_TIME = 3000, // 10초
    };

    // MsgCount Check
    DWORD m_dwLastCheckTime;
    LONG m_nPacketCount;
    boost::circular_buffer<int> m_cMsgBuffer;

	// reference count
	LONG m_nRequestedIOCount;

	// 마지막으로 io 요청한 시간
	DWORD m_dwLastRequestIOTime;

	bool m_bJumping;

public:	

	//! 대륙 및 국가 정보;
	inline Country::SCOUNTRY_INFO GetUserCountry () const { return m_userCountry; }
	void SetUserCountry ( const Country::SCOUNTRY_INFO& userCountry );
    
    //! 국가
    wb::EM_COUNTRY GetCountry() const { return m_Country; }
    void SetCountry(wb::EM_COUNTRY Country) { m_Country = Country; }

    //! 나이, 한국 셧다운제 체크용
    WORD GetAge() const { return m_wAge; }
    void SetAge(WORD Age) { m_wAge = Age; }

	//! 보내야할 메시지를 등록한다.
	int addSendMsg(NET_MSG_GENERIC* pMsg, DWORD dwSize);

	//! 보내야할 메시지 버퍼를 가져온다.
	void* getSendBuffer();

	//! 보내야할 메시지 크기를 가져온다.
	int getSendSize();

	//! 클라이언트에서 받은 메시지를 넣는다.
	int addRcvMsg(void* pMsg, DWORD dwSize);

	//! 클라이언트에서 받은 메시지를 하나 가져온다.
	void* getRcvMsg();

	//! 클라이언트 상태 리셋
	void Reset();

	//! 클라이언트 버퍼 리셋
	void ResetRcvBuffer();

	//! 클라이언트 종료시킴
	// DWORD dwClient 에러 확인 용도
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
	
	//mjeon.AF.AgentServer에서 최초 캐릭터 접속 시에 인증목적으로 사용된다. FieldServer에서는 의미가 없다.
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

	bool IsAccountPassing() const;			//! 계정검사 통과중인지 검사
	void SetAccountPassing(bool bPassing);	//! 계정검사 통과중인지 세팅

	CTime	GetLoginTime();	

	inline void ResetHeartBeat() { m_bHeartBeat = false; }
	inline void SetHeartBeat()   { m_bHeartBeat = true; }
	inline bool CheckHeartBeat() { 	return m_bHeartBeat == true ? true : false; }

	CRYPT_KEY GetCryptKey();

    __time64_t GetPremiumDate();
    void       SetPremiumDate(__time64_t tTime);

	__time64_t GetChatBlockDate();
    void       SetChatBlockDate(__time64_t tTime);


	// 극강부 체크 	
	void	SetExtremeCreate( bool bExtreme );

	bool	GetExtreme();

	void	SetJumpingCreate( bool bJumping );
	bool	GetJumping();

	// 변술부 체크;
	void		SetActorCreate ( bool bCreate );
	const bool	GetCreateActor ();

	// 태국 User Class Type을 설정한다.
	void SetIPBonusClass( EMIP_BONUS_CLASS emIPBonus );

	// 태국 User Class Type을 가져온다.
	EMIP_BONUS_CLASS GetIPBonusClass();


	// 중국 GameTime, OfflineTime 체크	
	void SetChinaGameTime( int nChinaGameTime );
	void SetChinaOfflineTime( int nChinaOfflineTime );
	void SetLastLoginDate( __time64_t tTime );
	void SetChinaUserAge( int nChinaUserAge );

	int GetChinaGameTime();
	int GetChinaOfflineTime();
	__time64_t GetLastLoginDate();
	int GetChinaUserAge();


	// 베트남 GameTime 체크
	void SetVTGameTime( int nVTGameTime );
	int GetVTGameTime();
        
    //  특정시간내에 클라이언트가 보내온 패킷을 카운트한다.
    bool IncPacketCount( int nMsgID );
    void WritePacketLog();
	
	//! 말레이지아 요청으로 1 회 접속시 5 회 이상 로그인 요청시 
	//! 사용자의 접속을 끊음...
	//! 로그인 요청 횟수 증가시킴
	void IncLoginAttempt();

	//! 로그인 요청 횟수 가져옴
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
	CCSAuth2            m_csa; ///< nProtect GameGuard 인증용 클래스
#else
	CCSAuth3            m_csa;
	int					m_casSeq;
#endif
#endif

	bool                m_bNProtectAuth;
	bool                m_bNProtectFirstCheck; ///< 접속후 nProtect 를 처음 인증받는가?	
	bool                m_bNProtectFirstAuthPass; ///< 게임에 진입하기 전에 모든 인증을 완료했는가?
	
public:
	//! nProtect 처음 인증인가?
	bool nProtectIsFirstCheck();

	//! nProtect 처음 인증여부 세팅
	void nProtectSetFirstCheck(bool bState);

	//! nProtect GameGuard 인증용 쿼리를 가져오기 전에 호출
	bool nProtectSetAuthQuery();
	int nProtect30SetAuthQuery();
	
    	
	//! nProtect GameGuard 인증용 쿼리 가져오기
	GG_AUTH_DATA nProtectGetAuthQuery();
	BYTE* nProtect30GetAuthQuery();

	

	//! nProtect GameGuard 인증 응답 가져오기
	GG_AUTH_DATA nProtectGetAuthAnswer();
	BYTE* nProtect30GetAuthAnswer();


	//! nProtect GameGuard 인증 응답 설정
	void nProtectSetAuthAnswer(GG_AUTH_DATA& ggad);
	void nProtect30SetAuthAnswer(BYTE* ggad, int nPacketSeq);

	//! nProtect GameGuard 인증 결과 가져오기
	DWORD nProtectCheckAuthAnswer();
	DWORD nProtect30CheckAuthAnswer(UINT32 nReceivedSize);

	//! nProtect GameGuard 인증 상태를 리셋시킨다.
	void nProtectResetAuth();

	//! nProtect GameGuard 처음 인증을 통과했는가?
	//! 이 인증을 통과해야 게임플레이를 할 수 있다.
	bool nProtectFirstAuthPass();

	//! nProtect GameGuard 처음인증 통과상태 세팅
	void nProtectSetFirstAuthPass(bool bState);

    //! -----------------------------------------------------------------------
    //! AES 암호화
    //! -----------------------------------------------------------------------
    /*
protected:
    CRITICAL_SECTION m_AesLock;
    sc::Aes* m_pAes;

public:
    //! server 의 public key 를 가져온다.
    //! 이 key 는 client 에 전송해야 한다.
    //! \param pPubKey key 를 받을 버퍼 32 byte 이상
    //! \param nKeyLength 공개키의 크기를 돌려준다.
    void GetServerPubKey(BYTE* pPubKey, int& nKeyLength OUT);

    void SetServerPubKey(BYTE* pPubKey);

    //! Client->Server Client PubKey
    bool SetClientPubKey(BYTE* pPubKey, int nKeyLength);

    void GetClientPubKey(BYTE* pPubKey, int& nKeyLength OUT);

    //! \param pBuffer 암호화할 버퍼
    //! \nLength 암호화할 버퍼의 길이.
    bool EncryptBuffer(BYTE* pBuffer, int nLength);

    //! \param pBuffer 복호화할 버퍼
    //! \nLength 복호화할 버퍼의 길이.
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
