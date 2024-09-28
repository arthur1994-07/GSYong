#ifndef S_NETCLIENT_H_
#define S_NETCLIENT_H_

#pragma once

#include <Shellapi.h>
#include "../../SigmaCore/Util/minTea.h"
#include "../../SigmaCore/Util/CBit.h"
#include "../../EngineLib/G-Logic/DxMsgProcessor.h"

#include "../../RanLogic/Util/gamecode.h"
#include "../../RanLogic/NetWork/NetLogicDefine.h"
#include "../../RanLogic/NetWork/RcvMsgBuffer.h"

#define NET_CLIENT_IP	20
#define NET_NOTIFY		WM_USER + 99

#ifndef MAX_CHAR_LENGTH
#define MAX_CHAR_LENGTH	255
#endif

extern char* AlphaHomepage;
extern char* AlphaDownload;
extern char* AlphaRegister;
extern char* AlphaLogin;

extern char* BetaHomepage;
extern char* BetaDownload;
extern char* BetaRegister;
extern char* BetaLogin;

extern char* RanHomepage;
extern char* RanDownload;
extern char* RanRegister;
extern char* RanLogin;

extern char* g_szClientVerFile;

///////////////////////////////////////////////////////////////////////////////
// Values
extern SOCKET g_sClient;

///////////////////////////////////////////////////////////////////////////////
// Function
//static unsigned int WINAPI NetThread(void* p);

//! \date 2003-05-25
//!
//! \author jgkim
//!
class CNetClient
{
public:
	CNetClient();
	CNetClient(HWND hWnd);
	~CNetClient();
	
	int	m_PatchVersion;
	int	m_LauncherVersion;
private:
	enum
	{
		eERROR,		///< 버전 파일오류
		eNORMAL,	///< 버전 일치
		eVERUP,		///< 새버전 다운해야 함
		ePATCHUP,   ///< 패치프로그램 다운해야 함
		eGAMEUP,	///< 게임	다운해야 함
		eALLUP		///< 패치, 게임 다운해야 함
	};

public:
	enum
	{
		//NET_ERR_SNDVER = NET_ERROR - 1,
		NET_ERR_CREATE_EVENT = NET_ERROR - 2,
		NET_ERR_EVENT_SELECT = NET_ERROR - 3,
		NET_ERR_CONNECT		 = NET_ERROR - 4,
		NET_ERR_EVENT_WAIT	 = NET_ERROR - 5,
		NET_ERR_CONTACT_EVENT= NET_ERROR - 6,
		NET_ERR_CREATE_THREAD= NET_ERROR - 7,
	};

protected:
	SOCKET		m_sClient;
	SOCKADDR_IN	m_Addr;
	HWND		m_hWnd;
	sc::CBit    m_Bit; ///< 비트조작 클래스
	sc::minTea		m_Tea; ///< 암호화 클래스

	BOOL		m_bThreadProc;

	BOOL		m_bGameServerInfoEnd; ///< 서버리스트를 전부 받았는지 상태체크
	BOOL		m_bKeyReceived;		  ///< 키를 받았는지 체크한다.

	INT			m_nLoginResult;		  ///< 로그인 결과 세팅
	CRYPT_KEY	m_ck;
	int         m_nRandomNumber;      ///< 랜덤 패스워드 번호

	char		m_cClientIP[MAX_IP_LENGTH+1];
	char		m_cServerIP[MAX_IP_LENGTH+1];
	
	char		m_szAppPath[1024];		

	int			m_cServerPort;
	int			m_nOnline;
	int			m_nClientNetState;
	char*		m_pSndBuffer;
	int			m_nSndBytes;

	CRcvMsgBuffer* m_pRcvManager;	

	int			m_nTotalBytes;
	int			m_nNetMode;

	DWORD		m_dwGaeaID;

	HANDLE		m_hEvent;
	HANDLE		m_hThread;
	HANDLE		m_hEventWait;
	HANDLE		m_hEventWork;
	HANDLE		m_hEventKill;

	CRITICAL_SECTION		m_CriticalSection; // criticalsection object
	G_SERVER_CUR_INFO_LOGIN m_sGame[MAX_SERVER_GROUP][MAX_CHANNEL_NUMBER];
	DxMsgProcessor*			m_pGlobalStage;

	CHAR		m_szEncryptKey[ENCRYPT_KEY+1];
	CHAR		m_szEncryptKeyHeart[ENCRYPT_KEY+1];

	CNetClient*	m_pNetField;
	CNetClient* m_pNetBoard;

	BOOL		m_bWorldBattle; // 통합전장 입장인지
	BOOL		m_bWorldBattleSvrInfoEnd; // 통합전장 서버 정보 수신 완료
	BOOL		m_bWorldBattleCharSelectEnd;

	//char		m_szSendGarbageMsg[2][12];
	//BYTE		m_dwGarbageNum;

	// USHORT		m_nKeyDirection;	  ///< 암호키 방향
	// USHORT		m_nKey;			      ///< 암호키 shift 량
	// char*		m_pRcvBuffer;
	// char*		m_pMsgBuffer;	
	// int			m_nRcvBytes;

public:
	ApexChina::CGameClient m_ApexClient; ///< APEX RSA 암호화

	void EncryptLoginDataWithApexRSA(
        unsigned char * szUserID, 
        unsigned char * szUserPassword, 
        unsigned char * szRandomPassword);
	
public:
	G_SERVER_CUR_INFO_LOGIN* GetSeverCurInfoLogin ()	{ return (G_SERVER_CUR_INFO_LOGIN*)m_sGame; }
	//void	LockOn();
	//void	LockOff();

	int		ConnectServer     (unsigned long uServerIP,  u_short nPort, int nState);
	int		ConnectServer     (const char*   szServerIP, int nPort,     int nState);
	int		ConnectServer	  (const char*   szServerIP, int nPort,     int nState, CRYPT_KEY ck);
	int		ConnectLoginServer(const char *szAddress, int nPort/* Default=5001 */);
	int		ConnectFieldServer(char *szAddress, int nPort);
	//int	ConnectBoardServer(char *szAddress, int nPort);
	//int	ConnectAgentServer(int nServerGroup, int nServerNumber);
	int		ConnectGameServer (int nServerGroup, int nServerNumber);
	int		CloseConnect      (void);
	
	BOOL	IsThreadProc ()	{ return m_bThreadProc; }

	void	SetWndHandle ( HWND hWnd, DxMsgProcessor* pGlobalStage=NULL );
	
	int		StartNetThread();
	int		StopNetThread(bool wait);
	int		NetThreadProc();

	//BOOL	CheckClientVer(); // 클라이언트의 버전을 체크한다.
	
	int		GetClientVer(int& LauncherVer, int& PatchVer); // 파일로부터 클라이언트 버전을 가져온다.
    int		GetClientVer();                                // 자기 자신의 맴버를 채운다.
	int		GetGameVer();
	int		GetPatchVer();
	char*	GetSreverIP(void);

	int     GetRandomPassNumber(void)		{ return m_nRandomNumber; }
	void    ResetRandomPassNumber()			{ m_nRandomNumber = 0; }

	void	SetAppPath(); // 프로그램의 실행경로를 설정한다.
	void	SetDefault();
	void	SetGaeaID(DWORD dwGaeaID)		{ m_dwGaeaID = dwGaeaID; }
	DWORD	GetGaeaID()						{ return m_dwGaeaID; }

	bool	IsOnline();
	bool	IsOnlineField();
	bool	IsOnlineBoard();
	BOOL	IsGameSvrInfoEnd();
	BOOL	IsKeyReceived();
	int		IsLogin();

	int		Recv(void);
	int		Send	   (CHAR* buff);
	int		Send       (CHAR* buff, INT nSize);
	int		SendNormal (CHAR* buff, INT nSize);

    //int SendAesEncrypt(NET_MSG_GENERIC* pMsg);
    //int SendAesEncrypt(BYTE* buff, int nSize);

	//int		GetGarbageMsg();
	//char*   SendMsgAddGarbageValue(CHAR* buff, INT& nSize);
	int		SendToAgent(CHAR* buff, INT nSize);
	int		SendToBoard(CHAR* buff, INT nSize);
	int		SendToField(CHAR* buff, INT nSize);
	int		SendToFieldNormal(CHAR* buff, INT nSize);
	int	    SendBuffer();
	int		SendBuffer2();
	void*	IsValid(void);

	void	RegisterNewAccount(void);

	//////////////////////////////////////////////////////////////////////////////
	void	ClientProcess(WPARAM wParam, LPARAM lParam);
	
	int		SndVersion(int PatchVersion, int LauncherVersion);
	int		SndVersion(void);

	const TCHAR* DoEncryptKey();	// 하트비트용 키를 암호화한다. ( Nprotect 전용 ) 
	
	// W.B.
	BOOL IsWorldBattle();
	void SetWorldBattle(BOOL bWorldBattle);
	
	BOOL IsWorldBattleSvrInfoEnd();
	void SetWorldBattleSvrInfoEnd(BOOL bWorldBattleSvrInfoEnd);

	BOOL IsWorldBattleCharSelectEnd();
	void SetWorldBattleCharSelectEnd(BOOL bWorldBattleCharSelectEnd);
	
	//
	//Taiwan, Hongkong, 내부
	//
	int SndLogin(const char* szUserID, const char* szUserPassword, const char* szRandomPassword, int nChannel=0);	
	int	SndDelCharacter(int nChaNum, const TCHAR* szPasswd);


	//	
	//Indonesia
	//  mjeon.indonesia
	int IdnSndLogin(const char* userid, const char* userpass, int nChannel);
	int	IdnSndDelCharacter(int nChaNum, std::string str2ndPass);



	//
	//Daum Game(한국)
	//
	int	DaumSndLogin(LPCSTR szParameter, int nChannel);
	int DaumSndPassCheck(const TCHAR* szDaumGID, const TCHAR* szUserPassword, int nCheckFlag);
	int	DaumSndDelCharacter(int nChaNum, const TCHAR* szPasswd);
	

	//
	//GS
	//
	int GsSndWBLogin(const char* userid, const char* userpass, int nChannel);	
	int GsSndLogin(const char* userid, const char* userpass, int nChannel);	
	int	GsSndDelCharacter(int nChaNum, const TCHAR* szPasswd);	


	//
	// EU (Games-Masters)
	// 로그인 구조체 패킷등은 아직 생성하지 않았다. 로그인 방식이 정해지면 추가해야 한다.
	// enum 값도 추가 되어야 한다. 아이디 길이 등등
	// 지금은 GS와 같은 것을 우선 사용한다.
	int GmSndLogin( const char* userid, const char* userpass, int nChannel );
	int	GmSndDelCharacter( int nChaNum );

	//
	// US (GameSamba)
	// 로그인 구조체 패킷등은 아직 생성하지 않았다. 로그인 방식이 정해지면 추가해야 한다.
	// enum 값도 추가 되어야 한다. 아이디 길이 등등
	// 지금은 GS와 같은 것을 우선 사용한다.
	int UsSndLogin( const char* userid, const char* userpass, int nChannel );
	int	UsSndDelCharacter( int nChaNum );

	//
	//Thailand (태국)
	//
	int ThaiSndLogin(const char* userid, const char* userpass, int nChannel);


	//
	//Terra (Malaysia, 필리핀, 베트남)
	//
	int TerraSndLogin(const TCHAR* szTID, int nChannel);
	int TerraSndPassCheck(const TCHAR* szUserid, const TCHAR* szUserPassword, int nCheckFlag);
	int TerraSndDelCharacter(int nChaNum);


	//
	//Japan
	//	
	int JapanSndLogin(const char* szUserID, const char* szUserPassword, int nChannel=0);	
	int JapanSndDelCharacter(int nChaNum);


	//
	//Excite Japan 로그인 데이터 전송
	//
	/*
		szU			UserID		char 20
		szT			유효기간	char 14
		szMD5		MD5 char	32
		nChannel	채널번호
	*/
	int ExciteSndLogin(const char* szU, const char* szT, const char* szMD5, int nChannel);
	int ExciteSndPassCheck(const TCHAR* szUserid, const TCHAR* szUserPassword, int nCheckFlag);
	int ExciteSndDelCharacter(int nChaNum);


	//
	//China
	//
	int ChinaSndLogin(const TCHAR* szUserID, const TCHAR* szUserPassword, const TCHAR* szRandomPassword, int nChannel=0);
	int	ChinaSndDelCharacter(int nChaNum, const TCHAR* szPasswd);


	//
	//GSP
	//	
	int GspSndLogin(const TCHAR* szUUID, int nChannel);
	int GspSndDelCharacter( int nChaNum );

    //! -----------------------------------------------------------------------
    //! World Battle
    int WorldBattleLogin(const char* szUserId, const char* szPassword, int Channel);

    //! -----------------------------------------------------------------------

	int SndAccountInfo(const char* uID, const char* uPass, const char* uName);
	int	SndRequestUserInfo(void);
	int	SndCreateChaInfo(int nIndex, WORD wSchool, WORD wFace, WORD wHair, WORD wHairColor, WORD wSex, const char* strChaName);
	int	SndChaAllBasicInfo(); // 사용자 캐릭터의 기본정보를 요청한다.
	//int	SndChaBasicInfo(int nChaNum); 
    // 사용자 캐릭터 기본정보를 요청한다. 캐릭터 선택화면 디스플레이용
    int	SndChaBasicInfoPack(const std::vector<DWORD>& vecChaNum); // 중복 메세지 제거 메세지팩
	
	
	/*
	// Ping to Server (Agent or Field).
	void PingToAgent();
	void PingToField();
	*/

	/**
	* 서버에 랜덤키를 요청한다.
	* client->agent
	*/
	int SndRequestRandomKey(void);

	//int SndRequestAllChaInfo(void); // 해당 사용자의 모든 캐릭터 정보를 요청한다.
	//int SndPing(void);	
	int	SndReqServerInfo(void); // 게임서버의 정보를 서버에 요청한다.
	int	SndGameJoin(int ChaDbNum, const char *psz2ndPass = NULL, BOOL bNewPass = FALSE); // 2차비번
	int	SndHeartbeat();

	//int SndEventLottery(const char* szLotteryName);
	
	//////////////////////////////////////////////////////////////////////////////
	void MessageProcess();
	void MessageProcessLogin(NET_MSG_GENERIC* nmg);
	void MessageProcessGame (NET_MSG_GENERIC* nmg);
	void MessageProcessCommon(NET_MSG_GENERIC* nmg);

	//////////////////////////////////////////////////////////////////////////////
	// Game Server Message	
	void	MsgCryptKey			(NET_MSG_GENERIC* nmg);
	void    MsgRandomNum       (NET_MSG_GENERIC* nmg);
	void	MsgLoginFeedBack   (NET_MSG_GENERIC* nmg);	
	//void	MsgChat				(NET_MSG_GENERIC* nmg);	// 채팅메시지
	//void	MsgChaBAInfo		(NET_MSG_GENERIC* nmg);
	//void	MsgChaBInfo			(NET_MSG_GENERIC* nmg);
	int		MsgConnectClientToField(NET_MSG_GENERIC* nmg);
	int		MsgConnectClientToInstance(NET_MSG_GENERIC* nmg);	
	
	//////////////////////////////////////////////////////////////////////////////
	// Login Server Message
	void	MsgGameSvrInfo		(NET_MSG_GENERIC* nmg);
	void	MsgGameSvrInfoEnd	(NET_MSG_GENERIC* nmg);
		
	//////////////////////////////////////////////////////////////////////////////
	// Common Message	
	int	MsgVersionInfo(NET_MSG_GENERIC* nmg);
    int MsgVersionCheck(NET_MSG_GENERIC* nmg);
	int	MsgEncryptKey(NET_MSG_GENERIC* nmg);
	void MsgNewVersionDown();

    //! --------------------------------------------------------------------------
    //! AES encrypt Start
    //! --------------------------------------------------------------------------
protected:
    //sc::Aes* m_pAes;
    //bool m_bAesKeyReceived;

public:
    //! Client->Server AES 공개키를 요청한다.
    void msg_CS_AES_REQ_SERVER_PUB_KEY();

    //! Server->Client AES Server public key
    void msg_SC_AES_SERVER_PUB_KEY(NET_MSG_GENERIC* pMsg);

    void msg_SC_AES_TEST(NET_MSG_GENERIC* pMsg);
    void msg_SC_AES_TEST2(NET_MSG_GENERIC* pMsg);

    //bool isKeyReceived() { return m_bAesKeyReceived; }

    //! --------------------------------------------------------------------------
    //! AES encrypt End
    //! --------------------------------------------------------------------------

};
#endif // S_NETCLIENT_H_


//      ,
//    ,x?n=4??$$n.
//  -Lb$;zPs$Lb;d)%.
// ;d$$$$$$$$$$$$b$;-
// $$$$$$$$$$$$$$$$$;-
// ?$$$$$$$$$$$$$$$$"
//.d$$$$$$$$$$$$$$$$.
// `$$c,,$$$bc,,d$$>"
//  ?$$$$$eed$$$$$F
//   "?$$$L,J$$$F)ee,
//       """";zd$$$$$$$ec,
//        .dF$$$$$$$$$$$$$$$eeu,,.;;!!!!!!!!!i;
//        <Fd$$$$$$$$$$$$$$$$$$$P,!!!!!!!!!!!!!!i
//         '$$$$$$$'<$$$$$$$$$$":!!!'!!!''(!!!!!!!>
//          $$$$$$'d$$$$$$$$$P.<!'./` -'`((((!!!!!!
//          $$$?$'$$$$$$$$$$$ !' `,cucucuc, `'!!!!'
//          $$Jz".,"?$$$$$$$F   u$$$$$$$$$$$r `!'
//         d$$$$ d$$$   """""  d$$$$$$$$$$$$" ueP
//        -$$$$"ci"$"         $$$$$$$$$$$P".,d$$        __    e$ec.
//   e=7?Rbi3P ??$$P         d$$$$$$$$P" <PF??? ..,,ccd$$$$c,`""??$$eec,_
//    -?F""""                ?$$$$$$$$$b$$$$$$$$$$$$$$$$$$$$$$$$$r."""""""
//                             `""""""""""""""""""""```       """``
// jgkim