#ifndef _GL_NET_LOGIC_DEFINE_H_
#define _GL_NET_LOGIC_DEFINE_H_

#pragma once

#include <WinSock2.h>
#include <strsafe.h>
#include "../../SigmaCore/String/MinGuid.h"
#include "../../EngineLib/ServiceProviderDefine.h"

namespace net 
{    
    //! server type [type]
    enum EMSERVERTYPE
    {
        SERVER_UNKNOWN  = 0,
        SERVER_LOGIN	= 1,
        SERVER_SESSION	= 2,
        SERVER_FIELD	= 3,
        SERVER_AGENT	= 4,
        SERVER_BOARD	= 5, //! 사용하지 않음
        SERVER_CACHE	= 6,
		SERVER_MATCH	= 7, //mjeon.instance.match
		SERVER_INSTANCE	= 8,
		SERVER_AUTH		= 9, // for global auth
		SERVER_INTEGRATION_CACHE	= 10,
    };
    
    //! Client Slot Type (CNetUser)
    //! 각 서버 입장에서 바라본 연결의 종류
    enum SLOT_TYPE
    {
        SLOT_UNKNOWN		= 0, //! 초기상태
        SLOT_CLIENT			= 1, //! Server : Client->Server
        SLOT_AGENT			= 2, //! Field  : Agent->Field
		SLOT_SESSION		= 4, //! Server : Server->Session
        SLOT_AGENT_FIELD	= 5, //! Agent  : Agent->Field:	최초에 Agent가 Field로 접속하는 순수한 session
        SLOT_CACHE			= 6, //! Server : Server->Cache
		SLOT_MATCH			= 7, //! Agent  : Agent->Match
		SLOT_INSTANCE		= 8, //! Match  : Instance->Match
		SLOT_AUTH			= 9, // for global auth
		SLOT_AGENT_AGENT	= 10,//! Agent	: Agent->Agent : 각 Server Group의 통신을 의미;
		SLOT_INTEGRATION_CACHE = 11, //! Server : Server->Integration Cache
    };

    enum RESERVED
    {        
        RESERVED_SLOT    = 100, ///< FIELDSERVER_MAX*MAX_CHANNEL_NUMBER, 서버간 통신을 위해 예약된 클라이언트 소켓 slot
        //RESERVED_SESSION = RESERVED_SLOT-1, ///< 세션 서버와의 통신을 위해 예약된 클라이언트 소켓 slot	
        //RESERVED_CACHE   = RESERVED_SLOT-2, ///< 캐쉬 서버와의 통신을 위해 예약
    };

    enum EM_SLOT
    {
        INVALID_SLOT = 0xFFFF,
    };

    enum EM_NET_STATE
    {
        OFFLINE    = 0, //! 연결 종료
        ONLINE     = 1, //! 연결된 상태
        //NET_CLOSE_WAIT = 2, //! 연결 종료 대기상태
    };

    enum EM_GAME_VERSION
    {
        INVALID_VERSION = -100,
		INVALID_COUNTRY = -101,
        EXCEED_VERSION = -200,
    };

	// !!!! 아래의 종료 타입과 CloseClient 전,후에 존재하는 로그메세지와 조합하여 로그아웃 타입을 확인한다
	enum CLOSE_TYPE
	{
		UNDEFINE	= 0,
		NORMAL_LOGOUT		= 1,
		UNKNOWN_SVR	= 2,
		SESSION		= 3,
		CACHE		= 4,
		FIELD		= 5,
		LOGIN		= 6,
		AUTH		= 7,
		WORLDBATTLE	= 8,

		ERROR_IO_OPERATION			= 21,
		ERROR_CREATE_IOCP			= 22,
		ERROR_UNKOWN_IO_OPERATION	= 23,

		LOGOUT_ALL				= 31,
		KICKOUT_UNDEFINE		= 32,
		KICKOUT_HEARTBEAT		= 33,
		KICKOUT_PACKET_COUNT	= 34,
		KICKOUT_ILLEGAL_MSG		= 35,
		KICKOUT_GAMEGUARD		= 36,
		KICKOUT_SHUTDONW_RULE	= 37,
		KICKOUT_FIELD_RESTART	= 38, 
	};
}

#define NET_DEFAULT_PORT			5001 

#define NET_CLIENT_LISTEN			50
#define NET_TIME_OUT				180000	// 1000 msec * 60 sec * 3 = 3 minutes
#define NET_BLOCK_TIME              1800000 // 1000 msec * 60 sec * 30 = 30분동안 block 등록을 한다.

#define NET_MAX_CLIENT				1000	///< Default max client number of per server

#define MAX_WORKER_THREAD		    6       ///< 숫자가 4개이하일때 딜레이발생
                                            ///< 의도적으로 6 개로 했다.
#ifndef MAX_CHAR_LENGTH
#define MAX_CHAR_LENGTH	            255
#endif

#define MAX_SERVER_GROUP			20		///< 최대 서버그룹
#define MAX_SERVER_NUMBER			10		///< 서버그룹당 최대 서버숫자

#define MAX_IP_LENGTH               20      ///< IP 번호 문자열 최대값
#define SVR_STATE_UPDATE_TIME		180000	///< 300000 msec = 5 min:게임서버 상태 업데이트

#define AGENTSERVER_MAX				10		///< Agent Server에 연결되는 Agent Server의 최대 개수;
#define FIELDSERVER_MAX				10      ///< 채널당 최대 필드서버 갯수
#define MAX_CHANNEL_NUMBER          10      ///< 서버당 최대 채널 갯수

#define NET_STATE_LOGIN				1		///< 로그인서버 접속상태
#define NET_STATE_AGENT				2		///< 게임서버 접속상태
#define NET_STATE_FIELD				3		///< 게임서버 접속상태
#define NET_STATE_BOARD				4		///< 게임서버 접속상태
#define NET_STATE_CTRL				5		///< 원격 컨트롤 프로그램 접속상태

#define NET_PACKET_HEAD				0
#define NET_PACKET_BODY				1

// string data size
#define USR_ID_LENGTH				20
#define USR_PASS_LENGTH				USR_ID_LENGTH
#define SHA256_LENGTH				65 //! 마지막 공백문자 포함
#define USR_RAND_PASS_LENGTH        6
#define MAX_TRACING_NUM				19
#define USR_EU_ID_LENGTH				200
#define USR_EU_PASS_LENGTH				USR_EU_ID_LENGTH

#define GLOBAL_CHAT_MSG_SIZE		981
#define CHAT_MSG_SIZE				100
#define TRACING_LOG_SIZE			256

#define MAX_FRIEND					150

// #define MAX_SERVERCHAR_NUM			2	// 최대 생성할 수 있는 캐릭터 수	
// 극강부 추가
#define MAX_ONESERVERCHAR_NUM		30 // 한 서버에서 만들수 있는 캐릭터 갯수

#define SERVER_NAME_LENGTH			50
//#define MAX_APEX_PACKET_LENGTH		640
#define ENCRYPT_KEY					12
#define AUTH_DATA_LENGTH			500
#define AUTH_EVENT_INFO_TYPE		10

//////////////////////////////////////////////////////////////////////////////
// "GID": GameID   (사용자가 직접 입력했던 ID) - e.g. test1     16 자리
// "UID": UniqueID (시스템이 부여한 사용자 ID) - e.g. 15ftgr     6 자리
// "SSNHEAD": 생년월일 - e.g. 731201                             6 자리
// "SEX": 성별(M:남자, F:여자)                                   1 자리
// "NAMECHECK": 실명확인 여부(1=했음, 0=안했음)                  1 자리
// "IP": 접속한 IP
// "STATUS": 사용자 상태(A: 정회원, W: 준회원(기획예정), D: 탈퇴신청회원, B: 불량유저)
#define DAUM_MAX_PARAM_LENGTH       500
#define DAUM_MAX_GID_LENGTH         20
#define DAUM_MAX_UID_LENGTH         20
#define DAUM_MAX_SSNHEAD_LENGTH     7
#define DAUM_MAX_SEX_LENGTH         2
#define DAUM_USERPASS				20
#define KOR_MAX_GID_LENGTH			20
#define KOR_USERIP					20
#define KOR_ALLOW_TIME_ERROR        20
#define KOR_ROWCOUNT_ERROR	        21
#define KOR_ALLOW_TIME_DIFF_PLUS   21600 // 6 Hour
#define KOR_ALLOW_TIME_DIFF_MINUS -3600 // 1 Hour
#define KOR_SSN_HEAD_LENGTH          8  // 주민등록 번호 앞자리 SSN(Social Security Number)

//////////////////////////////////////////////////////////////////////////////
// TLoginName (varchar 15):	The most important field which identifies a unique gamer. 
//                          Use this as key to tables in your RAN Game Database 
//                          to maintain a record of each gamer. 
// TGender (char):		    Gender of the gamer 
// TIcNo   (char 15): 		Identity Number (User_No) of the gamer 
// TDOB    (char 10):       Date of Birth of the gamer. 
#define TERRA_TID_ENCODE            96
#define TERRA_TID_DECODE            36
#define TERRA_TLOGIN_NAME           15
#define TERRA_TGENDER               1
#define TERRA_TICNO                 15
#define TERRA_TDOB                  10
#define TERRA_USERPASS				20

// GSP Global Server Platform
#define GSP_USERID	51
#define GSP_USERIP  16
#define GSP_ALLOW_TIME_DIFF_PLUS   21600 // 6 Hour
#define GSP_ALLOW_TIME_DIFF_MINUS -3600 // 1 Hour
#define GSP_ALLOW_TIME_ERROR       20

//////////////////////////////////////////////////////////////////////////////
// Excite Japan
#define EXCITE_USER_ID				20
#define EXCITE_TDATE				14
#define EXCITE_MD5					32
#define EXCITE_SEC_PASS				16
#define EXCITE_SEC_ID				16

//////////////////////////////////////////////////////////////////////////////
// Gonzo Japan
#define JAPAN_USER_ID				16
#define JAPAN_USER_PASS				16

// Grobal Service
#define GS_USER_ID			20
#define GS_USER_PASS		20
//

// EU Service
#define GM_USER_ID			200
#define GM_USER_PASS		200
//

#define IDN_2ND_PASS_MAX_LENGTH	16
#define MD5_MAX_LENGTH			32	//128 bits = 16 Bytes = 32 hex codes (0xAB = 1 Byte)

//mjeon.Add
//We can move following definitions for post system to anywhere in valid scope.
//
#define POST_MAX_TITLE_LENGTH	36
#define POST_MAX_CONTENT_LENGTH	256	

#define POST_ON_PROCESSING		-1	//PostGet cannot be done because of former PostGet request.
#define POST_COOTIME_REMAINS	-2	//PostGet cannot be done because cooltime remains


//
//mjeon.sns
//
#define SNS_MAX_SKEY_LENGTH		128
#define SNS_MAX_TOKEN_LENGTH	64
#define SNS_MAX_UID_LENGTH		24	// max string length of 64-bit int (2^64)

#define BASE_GAEAID				100000	//서버군마다 최대 접속자 수가 다를 수 있으므로 충분히 큰 수(10만)로 한다


	/* Note for Session Key
	
	Session Key Format Change
	Sep 23, 2008 3:37pm

	We're making some changes to how we generate session keys.
	Currently, the typical user API session key consists of a 24-char string with the user id appended.
	The new session key encodes more data and will be longer.
	
	What this means to you: if your current storage for user session keys relies on the 24-char length,
	you will need to make changes to accommodate a longer session key format.
	In order to minimize the impact of future session key format changes on your backend systems,
	we're committing to a 128-char max length on session keys (not including the appended user id).
	To give you sufficient time to make any backend changes necessary to accommodate these longer session keys,
	these changes will not go live before Oct. 14.
	We will post a notice here before we start issuing session keys in the new format. 

	*/

//
//mjeon.instance
//
enum
{
	MAXNUM_OF_AGENTSERVER_TO_MATCH		= 8,	//MatchServer가 지원하는 최대 서버군(AgentServer)의 갯수
	MAXNUM_OF_INSTANCESERVER_TO_MATCH	= 8,	//MatchServer가 지원하는 최대 InstanceServer의 갯수
	MAXNUM_OF_INSTANCE_PER_MATCHSERVER	= 1000,	//최대 Instance(room) 생성 갯수
	INVALID_INSTANCE_KEY				= MAXDWORD,
	INVALID_MAP_ID						= MAXDWORD,
	INVALID_CHANUM						= MAXDWORD,

	INSTANCE_START_GATE					= 0,	//Instance Land의 default StartGate 번호
};


//
//mjeon.groupchat
//
enum
{
	INVALID_CHAT_KEY					= MAXDWORD,
};

enum CHAT_ERRORCODE
{
	CHAT_CODE_OK				= 0,
	CHAT_CODE_ROOM_NOT_CONSTRUCT= 1,	//해당 채팅방의 생성 실패
	CHAT_CODE_ROOM_NOT_EXIST	= 2,	//해당 채팅방이 존재하지 않음
	CHAT_CODE_MAX_OPEN_CHAT		= 3,	//최대 채팅 슬롯을 초과
	CHAT_CODE_MAX_MEMBERS		= 4,	//최대 채팅 참여자수 초과
	CHAT_CODE_OWNER_LOW_LEVEL	= 5,	//방개설을 하려는 캐릭터가 요구 레벨보다 낮다
	CHAT_CODE_MEMBER_LOW_LEVEL	= 6,	//초대를 받는 캐릭터가 요구 레벨보다 낮다
	CHAT_CODE_OTHER_PRISON		= 7,
	CHAT_CODE_PRISON			= 8,
	CHAT_CODE_RECEIVE_PRISON    = 9,
};




//! 해킹프로그램 DB 로그용 번호
enum EMHACKPROGRAM
{
	DEFAULT_HACK_NUM  =   0,
	NPROTECT_HACK_NUM = 998,
	APEX_HACK_NUM     = 999, // APEX 로그
};

//! 태국용 설정값들
#define THAI_LIMIT_MIN 180 // 태국은 18 세 이하는 하루 180 분 동안만 게임가능
#define THAI_CHECK_TIME 60000 // 1000 msec X 60 = 1 min

//! 태국 나이제한
enum EMTHAIUSERFLAG
{
	THAI_UNDER18  = 0, // 18 세 이하
	THAI_OVER18   = 1, // 18 세 이상
	THAI_FREETIME = 2, // 24 시간 사용가능
};

//! 사용자 타입설정
enum EMUSERTYPE
{
    USER_COMMON  =  0, // 일반사용자
    USER_PREMIUM =  1, // 프리미엄 유저
	USER_SPECIAL = 10, // 특별사용자 (게임방 업체 사장, 기자 등등)

	USER_USER_GM =	11, // 유저GM
	USER_USER_GM_MASTER = 12, // 유저GM

	USER_GM4     = 19, // GM 4 급, Web GM 용
	USER_GM3	 = 20, // GM 3 급
	USER_GM2	 = 21, // GM 2 급
	USER_GM1	 = 22, // GM 1 급
	USER_MASTER  = 30, // Master
};

enum EM_NEW_CHA_FB
{
	EM_CHA_NEW_FB_SUB_OK	= 1, // 캐릭터 생성 성공
	EM_CHA_NEW_FB_SUB_DUP	= 2, // 게임서버->클라이언트 : 같은 이름의 캐릭터가 이미 존재함
	EM_CHA_NEW_FB_SUB_LENGTH= 3, // 캐릭터 글자수 제한
	EM_CHA_NEW_FB_SUB_MAX	= 4, // 게임서버->클라이언트 : 더이상캐릭터 만들수 없음(최대캐릭터 숫자 초과)
	EM_CHA_NEW_FB_SUB_ERROR = 5, // 게임서버->클라이언트 : 에러발생 만들수 없음(시스템에러, 허용되지 않는 이름)
	EM_CHA_NEW_FB_SUB_THAICHAR_ERROR = 6, // 태국어문자 완성되지 않은 문자 조합 에러
	EM_CHA_NEW_FB_SUB_VNCHAR_ERROR = 7,	 // 베트남 문자 조합 에러
	EM_CHA_NEW_FB_SUB_BAD_NAME = 8,
    EM_CHA_NEW_FB_SUB_BAD_NAME_KOREA = 9,
    EM_CHA_NEW_FB_SUB_BAD_NAME_WB = 10,
	EM_CHA_NEW_FB_SUB_GM	= 11, // GM 계정은 캐릭터 생성 불가
};

enum EMNET_MSG_CHAT
{
	CHAT_TYPE_GLOBAL		= 1, // 관리용 글로벌 메시지
	CHAT_TYPE_NORMAL		= 2, // 일반적 채팅 메시지
	CHAT_TYPE_PRIVATE		= 3, // 개인적인 메시지, 귓말, 속삭임
	CHAT_TYPE_PARTY			= 4, // 파티원에게 전달되는 메시지	
	CHAT_TYPE_MASTER		= 5, // 원정대 마스터의 메시지
	CHAT_TYPE_GUILD			= 6, // 길드원에게 전달되는 메시지	
	CHAT_TYPE_ALLIANCE		= 7, // 동맹에게 전달되는 메시지
	CHAT_TYPE_AREA			= 8, // 지역 채팅
	CHAT_TYPE_PARTY_RECRUIT	= 9, // 파티모집
	CHAT_TYPE_LOUDSPEAKER	= 10, // 확성기.


	CHAT_TYPE_CTRL_GLOBAL		= 11,
	CHAT_TYPE_CTRL_GLOBAL2	= 12,

	CHAT_TYPE_FACTION		= 13,

	CHAT_TYPE_PRIVATE_PARTY_RECRUIT = 14,

	// 모든 채팅 메세지가 GM이 사용하는 것이 아니므로 필요한것만 추가
// 	CHAT_ADD_TYPE_FOPR_GM = 100,
// 	CHAT_TYPE_NORMAL_GM	= 102, // GM - 일반적 채팅 메시지
// 	CHAT_TYPE_PRIVATE_GM	= 103, // GM - 개인적인 메시지, 귓말, 속삭임
// 	CHAT_TYPE_PARTY_GM		= 104, // GM - 파티원에게 전달되는 메시지	
// 	CHAT_TYPE_GUILD_GM		= 106, // GM -  길드원에게 전달되는 메시지
// 	CHAT_TYPE_AREA_GM		= 108, // GM - 지역 채팅
// 	CHAT_TYPE_LOUDSPEAKER_GM	= 110, // GM - 확성기.
// 	CHAT_TYPE_FACTION_GM		= 113,
	CHAT_ADD_TYPE_FOPR_GM = 0x100,
	CHAT_TYPE_GLOBAL_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_GLOBAL		,
	CHAT_TYPE_NORMAL_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_NORMAL		, // GM - 일반적 채팅 메시지
	CHAT_TYPE_PRIVATE_GM		= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_PRIVATE		, // GM - 개인적인 메시지, 귓말, 속삭임
	CHAT_TYPE_PARTY_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_PARTY			, // GM - 파티원에게 전달되는 메시지	
	CHAT_TYPE_MASTER_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_MASTER		,
	CHAT_TYPE_GUILD_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_GUILD			, // GM -  길드원에게 전달되는 메시지
	CHAT_TYPE_ALLIANCE_GM		= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_ALLIANCE		,
	CHAT_TYPE_AREA_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_AREA			, // GM - 지역 채팅
	CHAT_TYPE_PARTY_RECRUIT_GM	= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_PARTY_RECRUIT	,
	CHAT_TYPE_LOUDSPEAKER_GM	= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_LOUDSPEAKER	, // GM - 확성기.
	CHAT_TYPE_CTRL_GLOBAL_GM	= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_CTRL_GLOBAL,
	CHAT_TYPE_CTRL_GLOBAL2_GM	= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_CTRL_GLOBAL2,
	CHAT_TYPE_FACTION_GM		= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_FACTION,
};

enum EM_LOGIN_FB_SUB
{	
    EM_LOGIN_FB_SUB_OK             =  0, //! 로그인 성공
	EM_LOGIN_FB_SUB_FAIL	       =  1, //! 로그인 실패
	EM_LOGIN_FB_SUB_SYSTEM	       =  2, //! 시스템에러로 로그인 실패
	EM_LOGIN_FB_SUB_USAGE	       =  3, //! 허가된 사용시간 에러
	EM_LOGIN_FB_SUB_DUP		       =  4, //! 중복된 접속
	EM_LOGIN_FB_SUB_INCORRECT      =  5, //! ID/PWD 불일치 = (새로운 계정을 생성 또는 재입력요구),
	EM_LOGIN_FB_SUB_IP_BAN	       =  6, //! ID/PWD 가 일치하지만 IP 가 사용불가능 상태= (접근 차단된 IP),	
	EM_LOGIN_FB_SUB_BLOCK          =  7, //! ID 가 블록된 상태
	EM_LOGIN_FB_SUB_UNCON          =  8, //! Daum : 비정상적 로그인데이타 (새롭게 웹에서 로그인후 게임실행)
	EM_LOGIN_FB_SUB_EXPIRED        =  9, //! Daum : 인증시간이 초과된 사용자이기 때문에, 웹에 재접속을 하라는 메시지를 보낸다
	EM_LOGIN_FB_SUB_GID_ERR        = 10, //! Daum : GID가 없음 웹에서 새롭게 로그인
	EM_LOGIN_FB_SUB_UID_ERR        = 11, //! Daum : UID가 없음 웹에서 새롭게 로그인
	EM_LOGIN_FB_SUB_UNKNOWN        = 12, //! Daum : 유저 삽입실패
	EM_LOGIN_FB_SUB_SSNHEAD        = 13, //! Daum : 주민번호오류
	EM_LOGIN_FB_SUB_ADULT          = 14, //! Daum : 성인인증실패
	EM_LOGIN_FB_CH_FULL            = 15, //! 채널이 Full 인 상태 접속불가
	EM_LOGIN_FB_THAI_UNDER18_3HOUR = 16, //! 18 세미만 3 시간 경과 
	EM_LOGIN_FB_THAI_UNDER18_TIME  = 17, //! 18 세미만 접속불가능 시간
	EM_LOGIN_FB_THAI_OVER18_TIME   = 18, //! 18 세이상 접속불가능 시간
	EM_LOGIN_FB_SUB_RANDOM_PASS    = 19, //! 랜덤패스워드 에러
	EM_LOGIN_FB_SUB_PASS_OK		   = 20, //! 초기 패스워드 업데이트 성공
	EM_LOGIN_FB_SUB_ALREADYOFFLINE = 21, //! 로그인성공이지만 이미 오프라인일때
	EM_LOGIN_FB_SUB_SECID_ALREADY  = 22, //! 아뒤가 중복됩니다.
	EM_LOGIN_FB_SUB_BETAKEY		   = 23, //! 베타키 인증 실패
	EM_LOGIN_FB_SUB_IMPOSSIBLEIP   = 24, //! 접속 불가능한 IP
	EM_LOGIN_FB_WRONG_SP		   = 25, //! 잘못된 ServiceProvider
    EM_LOGIN_FB_KOR_SHUTDOWN	   = 26, //! 한국, Shutdown 16세이하 접속불가 시간
	EM_LOGIN_FB_KR_OK_USE_PASS		= 27, // 로그인 성공 + 2차비번 사용 , 2차비번 사용안하는 경우는 0 으로 처리
	EM_LOGIN_FB_KR_OK_NEW_PASS		= 28, // 로그인 성공 + 2차비번 신규 설정
	EM_LOGIN_FB_ITEM_TRANSFER		= 29, // 아이템 변환 중이다. 잠시후에 접속해라.
};

// 란모바일
enum EM_RAN_MOBILE
{	
   EM_RAN_MOBILE_START_GM = 0,

   EM_RAN_MOBILE_CREATE_GM = 1,
   EM_RAN_MOBILE_DELETE_GM = 2,
   EM_RAN_MOBILE_INFO_GM = 3,
   EM_RAN_MOBILE_INFO_ALL_GM = 4,
   EM_RAN_MOBILE_GET_POINT_GM = 5,
   EM_RAN_MOBILE_GET_JEWEL_GM = 6,
   EM_RAN_MOBILE_UPDATE_DATE_GM = 7,
   EM_RAN_MOBILE_UPDATE_POINT_GM = 8, 
   EM_RAN_MOBILE_UPDATE_JEWEL_GM = 9,

   EM_RAN_MOBILE_CREATE_MONEY = 11,
   EM_RAN_MOBILE_CREATE_CACHE = 12,
   EM_RAN_MOBILE_EXTEND_TIME = 13,
   EM_RAN_MOBILE_GET_DAILY_ITEM = 14,
   EM_RAN_MOBILE_SEND_JEWEL = 15,
   EM_RAN_MOBILE_GET_POINT = 16,
   EM_RAN_MOBILE_CHANGE_ITEM = 17,
   EM_RAN_MOBILE_INFO = 18,
   EM_RAN_MOBILE_INFO_ALL = 19,
   EM_RAN_MOBILE_INFO_ALL_END = 20,

   EM_RAN_MOBILE_END,
};

// 란 모바일 DB 결과
enum ERanMobileResult
{
	ERanMobileResult_OK = 1,

	// 캐릭터 생성
	EResult_CreateChar_DuplicateName = -2,	// 닉네임 중복
	EResult_CreateChar_TimeError = -3,		// 연장 시간 삽입 에러
	EResult_CreateChar_NoUserInfo = -4,		// 계정 정보 없음
	EResult_CreateChar_NoMoney = -5,		// 소지금 부족
	EResult_CreateChar_NoItem = -6,			// 존재하지 않는 아이템		

	// 연장
	EResult_Extend_TimeError = -2,			// 연장 시간값 잘못 들어옴
	EResult_Extend_InsertError = -3,		// 삽입 실패
	EResult_Extend_CharInfo = -4,			// 캐릭터 정보 없음
	EResult_Extend_CharInfo2 = -5,			// 캐릭터 정보 없음
	EResult_Extend_NoMoney = -6,			// 소지금 부족

	// 보석 보내기
	EResult_SendJewel_Count = -2,			// 보석 개수값 잘못 들어옴
	EResult_SendJewel_InsertError = -3,		// 삽입 실패
	EResult_SendJewel_NoItem = -4,			// 존재하지 않는 아이템

	// MPoint
	EResult_MPoint_Count = -2,				// 소진될 MPoint 값 잘못 들어옴
	EResult_MPoint_OverCount = -3,			// 총 MPoint 보다 큰 값 들어옴
	EResult_MPoint_InsertError = -4,		// 삽입 실패

	// 교환
	EResult_Change_NoData = -2,				// 교환 데이터 없음
	EResult_Change_MPoint = -3,				// MPoint 부족
	EResult_Change_FullInventory = -4,		// 인벤토리 가득참

	// 일일 보상
	EResult_DailyReward_UpdateError = -2,		// DB 업데이트 에러
	EResult_DailyReward_FullInventory = -3,		// 인벤토리 가득참
	EResult_DailyReward_Already = -4,			// 이미 받음
};


// BASIC User Information
// 28 Bytes
struct USER_INFO_BASIC
{
	INT		UserDbNum;	
	USHORT	nKeyDirection;
	USHORT	nKey;
	TCHAR	szUserID[GSP_USERID];
	
	USER_INFO_BASIC()
	{
		UserDbNum = 0;
		nKeyDirection = 0;
		nKey = 0;
		memset(szUserID, 0, sizeof(TCHAR) * (GSP_USERID));
	};
};
typedef USER_INFO_BASIC* LPUSER_INFO_BASIC;

struct CRYPT_KEY
{
	sc::MGUID guid;

    CRYPT_KEY()
    {        
    }
};
typedef CRYPT_KEY* LPCRYPT_KEY;

struct G_SERVER_CUR_INFO_LOGIN
{
    char szServerIP[MAX_IP_LENGTH+1]; ///< Server IP Address
    int nServicePort;            ///< Server Port
    int nServerGroup;                ///< Server Group Number
    int nServerNumber;               ///< Channel Number or Server Number
    int nServerCurrentClient;	     ///< Channel Current Client
    int nServerMaxClient;		     ///< Channel Max Client
    bool bPK;                         ///< Channel PK information
    bool bNewServer;                   /// NewServer flag

    G_SERVER_CUR_INFO_LOGIN()
        : nServicePort(0)
        , nServerGroup(0)
        , nServerNumber(0)
        , nServerCurrentClient(0)
        , nServerMaxClient(0)
        , bPK(true)
        , bNewServer(false)
    {
        memset(szServerIP, 0, sizeof(char) * (MAX_IP_LENGTH+1));
    }

    G_SERVER_CUR_INFO_LOGIN& operator=(const G_SERVER_CUR_INFO_LOGIN& rhs)
    {        
        nServicePort         = rhs.nServicePort;
        nServerGroup         = rhs.nServerGroup;
        nServerNumber        = rhs.nServerNumber;
        nServerCurrentClient = rhs.nServerCurrentClient;
        nServerMaxClient     = rhs.nServerMaxClient;
        bPK                  = rhs.bPK;
        bNewServer           = rhs.bNewServer;

        StringCchCopy(szServerIP, MAX_IP_LENGTH+1, rhs.szServerIP);

        return *this;
    }
};
typedef G_SERVER_CUR_INFO_LOGIN* LPG_SERVER_CUR_INFO_LOGIN;
typedef G_SERVER_CUR_INFO_LOGIN** LPPG_SERVER_CUR_INFO_LOGIN;

///////////////////////////////////////////////////////////////////////////////
// Base Msg
// If you change base message, all message number will be change
// If user find packet order, change NET_MSG_BASE number
// All Message Number must less than 2 byte(65535)

// 아래 기본 메세지는 중복이 되지 않도록 정의한다.
//#if defined(CH_PARAM)
//#define NET_MSG_BASE				1019				  // 기본 메시지
//#elif defined(HK_PARAM)
//#define NET_MSG_BASE				1007				  // 기본 메시지
//#elif defined(ID_PARAM)
//#define NET_MSG_BASE				977					  // 기본 메시지
//#elif defined(JP_PARAM)
//#define NET_MSG_BASE				1031				  // 기본 메시지
//#elif defined(KR_PARAM)
//#define NET_MSG_BASE				987					  // 기본 메시지
//#elif defined(KRT_PARAM)
//#define NET_MSG_BASE				971					  // 기본 메시지
//#elif defined(MY_PARAM)
//#define NET_MSG_BASE				969					  // 기본 메시지
//#elif defined(MYE_PARAM)
//#define NET_MSG_BASE				1005				  // 기본 메시지
//#elif defined(PH_PARAM)
//#define NET_MSG_BASE				1017				  // 기본 메시지
//#elif defined(VN_PARAM)
//#define NET_MSG_BASE				1021				  // 기본 메시지
//#elif defined(TW_PARAM)
//#define NET_MSG_BASE				997					  // 기본 메시지
//#elif defined(TH_PARAM)
//#define NET_MSG_BASE				989					  // 기본 메시지
//#elif defined(GS_PARAM)
//#define NET_MSG_BASE				1004				  // 기본 메시지
//#else
//#define NET_MSG_BASE				995					  // 기본 메시지
//#endif

/*
#define NET_MSG_LGIN				(NET_MSG_BASE +  507) // 로그인서버 메시지
#define NET_MSG_LOBBY				(NET_MSG_BASE + 1013) // 게임서버 로비
#define NET_MSG_LOBBY_MAX			(NET_MSG_BASE + 1509) // 게임서버 로비 최대값

#define NET_MSG_GCTRL				(NET_MSG_BASE + 2011) // 게임서버 게임플레이
#define NET_MSG_GCTRL_MAX			(NET_MSG_BASE + 4010) // 게임서버 게임플레이 최대값
*/

// 2005-11-04
//#define NET_MSG_LGIN				(NET_MSG_BASE +  413) // 로그인서버 메시지
//#define NET_MSG_LOBBY				(NET_MSG_BASE +  913) // 게임서버 로비
//#define NET_MSG_LOBBY_MAX			(NET_MSG_BASE + 1413) // 게임서버 로비 최대값
//
//#define NET_MSG_GCTRL				(NET_MSG_BASE + 1823) // 게임서버 게임플레이
//#define NET_MSG_GCTRL_MAX			(NET_MSG_BASE + 3823) // 게임서버 게임플레이 최대값

// 2007-04-11

//#define NET_MSG_LGIN				(NET_MSG_BASE +  450) // 로그인서버 메시지
//#define NET_MSG_LOBBY				(NET_MSG_BASE +  950) // 게임서버 로비
//#define NET_MSG_LOBBY_MAX			(NET_MSG_BASE + 1450) // 게임서버 로비 최대값

//#define NET_MSG_GCTRL				(NET_MSG_BASE + 1900) // 게임서버 게임플레이
//#define NET_MSG_GCTRL_MAX			(NET_MSG_BASE + 4000) // 게임서버 게임플레이 최대값

struct G_SERVER_INFO
{
    net::EMSERVERTYPE ServerType;  ///< 서버 유형
    int	nServerGroup;			  ///< 서버 그룹 번호
    int	nServerNumber;			  ///< 서버 번호
    int	nServerField;			  ///< 필드번호	
    int	nServerMaxClient;		  ///< Max clients
    int	nServerCurrentClient;	  ///< Current connected client number
    int nServerChannel;           ///< Channel number
    int nServerChannelNumber;     ///< Agent's channel number
    int nServerChannellMaxClient; ///< Channel max client
    int	nServicePort;
    int	nControlPort;
    unsigned long ulServerIP;
    bool bPk;                     ///< Channel PK information
    bool bNewServer;              ///< New Server flag

    char szServerName[SERVER_NAME_LENGTH+1];
    char szServerIP[MAX_IP_LENGTH+1];
    char szUserID[USR_ID_LENGTH+1];
    char szUserPass[USR_PASS_LENGTH+1];

    G_SERVER_INFO()
    {
        memset(szServerName, 0, sizeof(char) * (SERVER_NAME_LENGTH+1));
        memset(szServerIP,   0, sizeof(char) * (MAX_IP_LENGTH+1));
        memset(szUserID,     0, sizeof(char) * (USR_ID_LENGTH+1));
        memset(szUserPass,   0, sizeof(char) * (USR_PASS_LENGTH+1));

        ulServerIP               = INADDR_NONE;
        nServicePort             = 0;
        nControlPort             = 0;
        ServerType               = net::SERVER_UNKNOWN;
        nServerGroup             = 0;
        nServerNumber            = 0;
        nServerField             = 0;
        nServerMaxClient         = 0;
        nServerCurrentClient     = 0;
        nServerChannel           = 0;
        nServerChannelNumber     = 0;
        nServerChannellMaxClient = 0;
        bPk                      = true; 
        bNewServer               = false;
    }

    G_SERVER_INFO& operator=(const G_SERVER_INFO& rhs)
    {
        ServerType               = rhs.ServerType;
        nServerGroup             = rhs.nServerGroup;
        nServerNumber            = rhs.nServerNumber;
        nServerField             = rhs.nServerField;
        nServerMaxClient         = rhs.nServerMaxClient;
        nServerCurrentClient     = rhs.nServerCurrentClient;
        nServerChannel           = rhs.nServerChannel;
        nServerChannelNumber     = rhs.nServerChannelNumber;
        nServerChannellMaxClient = rhs.nServerChannellMaxClient;
        nServicePort             = rhs.nServicePort;
        nControlPort             = rhs.nControlPort;
        ulServerIP               = rhs.ulServerIP;
        bPk                      = rhs.bPk;
        bNewServer               = rhs.bNewServer;

        StringCchCopy(szServerName, SERVER_NAME_LENGTH+1, rhs.szServerName);
        StringCchCopy(szServerIP, MAX_IP_LENGTH+1, rhs.szServerIP);
        StringCchCopy(szUserID, USR_ID_LENGTH+1, rhs.szUserID);
        StringCchCopy(szUserPass, USR_PASS_LENGTH+1, rhs.szUserPass);

        return *this;
    }

    friend bool operator==( const G_SERVER_INFO& lhs, const G_SERVER_INFO& rhs )
    {
        if ( (::strcmp(lhs.szServerIP, rhs.szServerIP) == 0) &&
            (lhs.nServicePort == rhs.nServicePort) )
        {
            return true;
        }
        else 
        {
            return false;
        }
    }

    void SetServerIp(const char* szIp)
    {
        StringCchCopy(szServerIP, MAX_IP_LENGTH+1, szIp);
    }
};

struct G_SERVER_CUR_INFO
{
    int	nServerCurrentClient;	// Corrent connected client number
    int	nServerMaxClient;		// Max clients

    G_SERVER_CUR_INFO()
        : nServerCurrentClient(0)
        , nServerMaxClient(0)
    {
    }
};

struct G_AUTH_INFO
{
    net::EMSERVERTYPE ServerType;  ///< 서버 유형
	int nCounrtyType;
    char szServerIP[MAX_IP_LENGTH+1];
	int	nServicePort;
	int nSessionSvrID;
    char szServerName[SERVER_NAME_LENGTH+1];
	char szAuthData[AUTH_DATA_LENGTH+1];

    G_AUTH_INFO()
    {
        memset(szServerName, 0, sizeof(char) * (SERVER_NAME_LENGTH+1));
        memset(szServerIP, 0, sizeof(char) * (MAX_IP_LENGTH+1));
		memset(szAuthData, 0, sizeof(char) * (AUTH_DATA_LENGTH+1));

		nCounrtyType = 0;
        nServicePort = 0;
        ServerType = net::SERVER_UNKNOWN;
		nSessionSvrID = 0;
    }

    G_AUTH_INFO& operator=(const G_AUTH_INFO& rhs)
    {
        ServerType = rhs.ServerType;
        nServicePort = rhs.nServicePort;
		nCounrtyType = rhs.nCounrtyType;
		nSessionSvrID = rhs.nSessionSvrID;

		StringCchCopy(szServerName, SERVER_NAME_LENGTH+1, rhs.szServerName);
        StringCchCopy(szServerIP, MAX_IP_LENGTH+1, rhs.szServerIP);
		StringCchCopy(szAuthData, AUTH_DATA_LENGTH+1, rhs.szAuthData);
        
        return *this;
    }

    friend bool operator==( const G_AUTH_INFO& lhs, const G_AUTH_INFO& rhs )
    {
        if ( (::strcmp(lhs.szServerIP, rhs.szServerIP) == 0) &&
            (lhs.nServicePort == rhs.nServicePort) &&
			(lhs.ServerType == rhs.ServerType) &&
			(lhs.nCounrtyType == rhs.nCounrtyType) )
        {
            return true;
        }
        else 
        {
            return false;
        }
    }

	friend bool operator==( const G_AUTH_INFO& lhs, const G_SERVER_INFO& rhs )
    {
        if ( (::strcmp(lhs.szServerIP, rhs.szServerIP) == 0) &&
            (lhs.nServicePort == rhs.nServicePort) &&
			(lhs.ServerType == rhs.ServerType) )
        {
            return true;
        }
        else 
        {
            return false;
        }
    }

    void SetServerIp(const char* szIp)
    {
        StringCchCopy(szServerIP, MAX_IP_LENGTH+1, szIp);
    }
	
	void SetServerName(const char* szSvrName)
    {
        StringCchCopy(szServerName, SERVER_NAME_LENGTH+1, szSvrName);
    }

	void SetAuthData(const char* szData)
    {
        StringCchCopy(szAuthData, AUTH_DATA_LENGTH+1, szData);
    }
};

struct G_AUTH_CUR_INFO
{
    net::EMSERVERTYPE ServerType;  ///< 서버 유형
	int nCounrtyType;
    char szServerIP[MAX_IP_LENGTH+1];
	int	nServicePort;
    char szServerName[SERVER_NAME_LENGTH+1];
	int	nServerCurrentUser;	
    int	nServerMaxLoginedUser;

    G_AUTH_CUR_INFO()
    {
        memset(szServerName, 0, sizeof(char) * (SERVER_NAME_LENGTH+1));
        memset(szServerIP, 0, sizeof(char) * (MAX_IP_LENGTH+1));

		nCounrtyType = 0;
        nServicePort = 0;
        ServerType = net::SERVER_UNKNOWN;
		nServerCurrentUser = 0;
		nServerMaxLoginedUser = 0;
    }

    G_AUTH_CUR_INFO& operator=(const G_AUTH_CUR_INFO& rhs)
    {
        ServerType = rhs.ServerType;
        nServicePort = rhs.nServicePort;
		nCounrtyType = rhs.nCounrtyType;
		nServerCurrentUser = rhs.nServerCurrentUser;
		nServerMaxLoginedUser = rhs.nServerMaxLoginedUser;

		StringCchCopy(szServerName, SERVER_NAME_LENGTH+1, rhs.szServerName);
        StringCchCopy(szServerIP, MAX_IP_LENGTH+1, rhs.szServerIP);
        
        return *this;
    }

    void SetServerIp(const char* szIp)
    {
        StringCchCopy(szServerIP, MAX_IP_LENGTH+1, szIp);
    }
	
	void SetServerName(const char* szSvrName)
    {
        StringCchCopy(szServerName, SERVER_NAME_LENGTH+1, szSvrName);
    }

};

struct G_AUTH_EVENT_INFO
{
	net::EMSERVERTYPE serverType;  ///< 서버 유형
	int counrtyType;
    char serverIP[MAX_IP_LENGTH+1];
	int	servicePort;

	float eventExp;
	float eventItem;
	float eventMoney;
	float eventGrade;
	WORD eventSpeedEx;
	WORD eventAtkSpeedEx;
	WORD eventAtkRateEx;
	int limitStartLv;
	int limitEndLv;
	int limitPlayTime;
	int limitBusterTime;
	float limitItemGainRate;
	float limitExpGainRate;
	DWORD limitEventMinute;

	DWORD eventClassExp;
	float eventClassExpRate;
	int eventClassExpStartLv;
	int eventClassExpEndLv;

	DWORD eventClassItem;
	float eventClassItemRate;
	int eventClassItemStartLv;
	int eventClassItemEndLv;

	DWORD eventClassMoney;
	float eventClassMoneyRate;
	int eventClassMoneyStartLv;
	int eventClassMoneyEndLv;

    DWORD ipEventAttackValue;
    DWORD ipEventAttackSpeed;
    DWORD ipEventMoveSpeed;
    DWORD ipEventItemDrop;
    DWORD ipEventMoneyDrop; 
    DWORD ipEventExp;
    DWORD ipEventHPRecovery;
    DWORD ipEventMPRecovery;
    DWORD ipEventSPRecovery;
    DWORD ipEventAllRecovery; 
    DWORD ipEventCriticalRate;

    float itemGainRate;		
    float expGainRate;		
    int	minEventLevel;		
    int	maxEventLevel;		
    bool eventStart;		
    int	eventPlayTime;		
    int	eventBusterTime;	
    CTime eventStartTime;		
    DWORD eventEndMinute;	


    G_AUTH_EVENT_INFO()
    {
		memset(serverIP, 0, sizeof(char) * (MAX_IP_LENGTH+1));
		counrtyType = 0;
        servicePort = 0;
        serverType = net::SERVER_UNKNOWN;

		eventExp = 0;
		eventItem = 0;
		eventMoney = 0;
		eventGrade = 0;
		eventSpeedEx = 0;
		eventAtkSpeedEx = 0;
		eventAtkRateEx = 0;
		limitStartLv = 0;
		limitEndLv = 0;
		limitPlayTime = 0;
		limitBusterTime = 0;
		limitItemGainRate = 0;
		limitExpGainRate = 0;
		limitEventMinute = 0;
		eventClassExp = 0;
		eventClassExpRate = 0;
		eventClassExpStartLv = 0;
		eventClassExpEndLv = 0;

		eventClassItem = 0;
		eventClassItemRate = 0;
		eventClassItemStartLv = 0;
		eventClassItemEndLv = 0;

		eventClassMoney = 0;
		eventClassMoneyRate = 0;
		eventClassMoneyStartLv = 0;
		eventClassMoneyEndLv = 0;

		ipEventAttackValue = 0;
		ipEventAttackSpeed = 0;
		ipEventMoveSpeed = 0;
		ipEventItemDrop = 0;
		ipEventMoneyDrop = 0; 
		ipEventExp = 0;
		ipEventHPRecovery = 0;
		ipEventMPRecovery = 0;
		ipEventSPRecovery = 0;
		ipEventAllRecovery = 0; 
		ipEventCriticalRate = 0;

		itemGainRate = 0;		
		expGainRate = 0;		
		minEventLevel = 0;		
		maxEventLevel = 0;		
		eventStart = FALSE;		
		eventPlayTime = 0;		
		eventBusterTime = 0;	
		eventStartTime = 0;		
		eventEndMinute = 0;	
    }

    G_AUTH_EVENT_INFO& operator=(const G_AUTH_EVENT_INFO& rhs)
    {
		StringCchCopy(serverIP, MAX_IP_LENGTH+1, rhs.serverIP);
		counrtyType = rhs.counrtyType;
        servicePort = rhs.servicePort;
        serverType = rhs.serverType;

		eventExp = rhs.eventExp;
		eventItem = rhs.eventItem;
		eventMoney = rhs.eventMoney;
		eventGrade = rhs.eventGrade;
		eventSpeedEx = rhs.eventSpeedEx;
		eventAtkSpeedEx = rhs.eventAtkSpeedEx;
		eventAtkRateEx = rhs.eventAtkRateEx;
		limitStartLv = rhs.limitStartLv;
		limitEndLv = rhs.limitEndLv;
		limitPlayTime = rhs.limitPlayTime;
		limitBusterTime = rhs.limitBusterTime;
		limitItemGainRate = rhs.limitItemGainRate;
		limitExpGainRate = rhs.limitExpGainRate;
		limitEventMinute = rhs.limitEventMinute;
		eventClassExp = rhs.eventClassExp;
		eventClassExpRate = rhs.eventClassExpRate;
		eventClassExpStartLv = rhs.eventClassExpStartLv;
		eventClassExpEndLv = rhs.eventClassExpEndLv;

		eventClassItem = rhs.eventClassItem;
		eventClassItemRate = rhs.eventClassItemRate;
		eventClassItemStartLv = rhs.eventClassItemStartLv;
		eventClassItemEndLv = rhs.eventClassItemEndLv;

		eventClassMoney = rhs.eventClassMoney;
		eventClassMoneyRate = rhs.eventClassMoneyRate;
		eventClassMoneyStartLv = rhs.eventClassMoneyStartLv;
		eventClassMoneyEndLv = rhs.eventClassMoneyEndLv;

		ipEventAttackValue = rhs.ipEventAttackValue;
		ipEventAttackSpeed = rhs.ipEventAttackSpeed;
		ipEventMoveSpeed = rhs.ipEventMoveSpeed;
		ipEventItemDrop = rhs.ipEventItemDrop;
		ipEventMoneyDrop = rhs.ipEventMoneyDrop; 
		ipEventExp = rhs.ipEventExp;
		ipEventHPRecovery = rhs.ipEventHPRecovery;
		ipEventMPRecovery = rhs.ipEventMPRecovery;
		ipEventSPRecovery = rhs.ipEventSPRecovery;
		ipEventAllRecovery = rhs.ipEventAllRecovery; 
		ipEventCriticalRate = rhs.ipEventCriticalRate;

		itemGainRate = rhs.itemGainRate;		
		expGainRate = rhs.expGainRate;		
		minEventLevel = rhs.minEventLevel;		
		maxEventLevel = rhs.maxEventLevel;		
		eventStart = rhs.eventStart;		
		eventPlayTime = rhs.eventPlayTime;		
		eventBusterTime = rhs.eventBusterTime;	
		eventStartTime = rhs.eventStartTime;		
		eventEndMinute = rhs.eventEndMinute;	     

        return *this;
    }

	void SetServerIp(const char* szIp)
    {
        StringCchCopy(serverIP, MAX_IP_LENGTH+1, szIp);
    }
};

#endif // _GL_NET_LOGIC_DEFINE_H_
