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
        SERVER_BOARD	= 5, //! ������� ����
        SERVER_CACHE	= 6,
		SERVER_MATCH	= 7, //mjeon.instance.match
		SERVER_INSTANCE	= 8,
		SERVER_AUTH		= 9, // for global auth
		SERVER_INTEGRATION_CACHE	= 10,
    };
    
    //! Client Slot Type (CNetUser)
    //! �� ���� ���忡�� �ٶ� ������ ����
    enum SLOT_TYPE
    {
        SLOT_UNKNOWN		= 0, //! �ʱ����
        SLOT_CLIENT			= 1, //! Server : Client->Server
        SLOT_AGENT			= 2, //! Field  : Agent->Field
		SLOT_SESSION		= 4, //! Server : Server->Session
        SLOT_AGENT_FIELD	= 5, //! Agent  : Agent->Field:	���ʿ� Agent�� Field�� �����ϴ� ������ session
        SLOT_CACHE			= 6, //! Server : Server->Cache
		SLOT_MATCH			= 7, //! Agent  : Agent->Match
		SLOT_INSTANCE		= 8, //! Match  : Instance->Match
		SLOT_AUTH			= 9, // for global auth
		SLOT_AGENT_AGENT	= 10,//! Agent	: Agent->Agent : �� Server Group�� ����� �ǹ�;
		SLOT_INTEGRATION_CACHE = 11, //! Server : Server->Integration Cache
    };

    enum RESERVED
    {        
        RESERVED_SLOT    = 100, ///< FIELDSERVER_MAX*MAX_CHANNEL_NUMBER, ������ ����� ���� ����� Ŭ���̾�Ʈ ���� slot
        //RESERVED_SESSION = RESERVED_SLOT-1, ///< ���� �������� ����� ���� ����� Ŭ���̾�Ʈ ���� slot	
        //RESERVED_CACHE   = RESERVED_SLOT-2, ///< ĳ�� �������� ����� ���� ����
    };

    enum EM_SLOT
    {
        INVALID_SLOT = 0xFFFF,
    };

    enum EM_NET_STATE
    {
        OFFLINE    = 0, //! ���� ����
        ONLINE     = 1, //! ����� ����
        //NET_CLOSE_WAIT = 2, //! ���� ���� ������
    };

    enum EM_GAME_VERSION
    {
        INVALID_VERSION = -100,
		INVALID_COUNTRY = -101,
        EXCEED_VERSION = -200,
    };

	// !!!! �Ʒ��� ���� Ÿ�԰� CloseClient ��,�Ŀ� �����ϴ� �α׸޼����� �����Ͽ� �α׾ƿ� Ÿ���� Ȯ���Ѵ�
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
#define NET_BLOCK_TIME              1800000 // 1000 msec * 60 sec * 30 = 30�е��� block ����� �Ѵ�.

#define NET_MAX_CLIENT				1000	///< Default max client number of per server

#define MAX_WORKER_THREAD		    6       ///< ���ڰ� 4�������϶� �����̹߻�
                                            ///< �ǵ������� 6 ���� �ߴ�.
#ifndef MAX_CHAR_LENGTH
#define MAX_CHAR_LENGTH	            255
#endif

#define MAX_SERVER_GROUP			20		///< �ִ� �����׷�
#define MAX_SERVER_NUMBER			10		///< �����׷�� �ִ� ��������

#define MAX_IP_LENGTH               20      ///< IP ��ȣ ���ڿ� �ִ밪
#define SVR_STATE_UPDATE_TIME		180000	///< 300000 msec = 5 min:���Ӽ��� ���� ������Ʈ

#define AGENTSERVER_MAX				10		///< Agent Server�� ����Ǵ� Agent Server�� �ִ� ����;
#define FIELDSERVER_MAX				10      ///< ä�δ� �ִ� �ʵ弭�� ����
#define MAX_CHANNEL_NUMBER          10      ///< ������ �ִ� ä�� ����

#define NET_STATE_LOGIN				1		///< �α��μ��� ���ӻ���
#define NET_STATE_AGENT				2		///< ���Ӽ��� ���ӻ���
#define NET_STATE_FIELD				3		///< ���Ӽ��� ���ӻ���
#define NET_STATE_BOARD				4		///< ���Ӽ��� ���ӻ���
#define NET_STATE_CTRL				5		///< ���� ��Ʈ�� ���α׷� ���ӻ���

#define NET_PACKET_HEAD				0
#define NET_PACKET_BODY				1

// string data size
#define USR_ID_LENGTH				20
#define USR_PASS_LENGTH				USR_ID_LENGTH
#define SHA256_LENGTH				65 //! ������ ���鹮�� ����
#define USR_RAND_PASS_LENGTH        6
#define MAX_TRACING_NUM				19
#define USR_EU_ID_LENGTH				200
#define USR_EU_PASS_LENGTH				USR_EU_ID_LENGTH

#define GLOBAL_CHAT_MSG_SIZE		981
#define CHAT_MSG_SIZE				100
#define TRACING_LOG_SIZE			256

#define MAX_FRIEND					150

// #define MAX_SERVERCHAR_NUM			2	// �ִ� ������ �� �ִ� ĳ���� ��	
// �ذ��� �߰�
#define MAX_ONESERVERCHAR_NUM		30 // �� �������� ����� �ִ� ĳ���� ����

#define SERVER_NAME_LENGTH			50
//#define MAX_APEX_PACKET_LENGTH		640
#define ENCRYPT_KEY					12
#define AUTH_DATA_LENGTH			500
#define AUTH_EVENT_INFO_TYPE		10

//////////////////////////////////////////////////////////////////////////////
// "GID": GameID   (����ڰ� ���� �Է��ߴ� ID) - e.g. test1     16 �ڸ�
// "UID": UniqueID (�ý����� �ο��� ����� ID) - e.g. 15ftgr     6 �ڸ�
// "SSNHEAD": ������� - e.g. 731201                             6 �ڸ�
// "SEX": ����(M:����, F:����)                                   1 �ڸ�
// "NAMECHECK": �Ǹ�Ȯ�� ����(1=����, 0=������)                  1 �ڸ�
// "IP": ������ IP
// "STATUS": ����� ����(A: ��ȸ��, W: ��ȸ��(��ȹ����), D: Ż���ûȸ��, B: �ҷ�����)
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
#define KOR_SSN_HEAD_LENGTH          8  // �ֹε�� ��ȣ ���ڸ� SSN(Social Security Number)

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

#define BASE_GAEAID				100000	//���������� �ִ� ������ ���� �ٸ� �� �����Ƿ� ����� ū ��(10��)�� �Ѵ�


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
	MAXNUM_OF_AGENTSERVER_TO_MATCH		= 8,	//MatchServer�� �����ϴ� �ִ� ������(AgentServer)�� ����
	MAXNUM_OF_INSTANCESERVER_TO_MATCH	= 8,	//MatchServer�� �����ϴ� �ִ� InstanceServer�� ����
	MAXNUM_OF_INSTANCE_PER_MATCHSERVER	= 1000,	//�ִ� Instance(room) ���� ����
	INVALID_INSTANCE_KEY				= MAXDWORD,
	INVALID_MAP_ID						= MAXDWORD,
	INVALID_CHANUM						= MAXDWORD,

	INSTANCE_START_GATE					= 0,	//Instance Land�� default StartGate ��ȣ
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
	CHAT_CODE_ROOM_NOT_CONSTRUCT= 1,	//�ش� ä�ù��� ���� ����
	CHAT_CODE_ROOM_NOT_EXIST	= 2,	//�ش� ä�ù��� �������� ����
	CHAT_CODE_MAX_OPEN_CHAT		= 3,	//�ִ� ä�� ������ �ʰ�
	CHAT_CODE_MAX_MEMBERS		= 4,	//�ִ� ä�� �����ڼ� �ʰ�
	CHAT_CODE_OWNER_LOW_LEVEL	= 5,	//�氳���� �Ϸ��� ĳ���Ͱ� �䱸 �������� ����
	CHAT_CODE_MEMBER_LOW_LEVEL	= 6,	//�ʴ븦 �޴� ĳ���Ͱ� �䱸 �������� ����
	CHAT_CODE_OTHER_PRISON		= 7,
	CHAT_CODE_PRISON			= 8,
	CHAT_CODE_RECEIVE_PRISON    = 9,
};




//! ��ŷ���α׷� DB �α׿� ��ȣ
enum EMHACKPROGRAM
{
	DEFAULT_HACK_NUM  =   0,
	NPROTECT_HACK_NUM = 998,
	APEX_HACK_NUM     = 999, // APEX �α�
};

//! �±��� ��������
#define THAI_LIMIT_MIN 180 // �±��� 18 �� ���ϴ� �Ϸ� 180 �� ���ȸ� ���Ӱ���
#define THAI_CHECK_TIME 60000 // 1000 msec X 60 = 1 min

//! �±� ��������
enum EMTHAIUSERFLAG
{
	THAI_UNDER18  = 0, // 18 �� ����
	THAI_OVER18   = 1, // 18 �� �̻�
	THAI_FREETIME = 2, // 24 �ð� ��밡��
};

//! ����� Ÿ�Լ���
enum EMUSERTYPE
{
    USER_COMMON  =  0, // �Ϲݻ����
    USER_PREMIUM =  1, // �����̾� ����
	USER_SPECIAL = 10, // Ư������� (���ӹ� ��ü ����, ���� ���)

	USER_USER_GM =	11, // ����GM
	USER_USER_GM_MASTER = 12, // ����GM

	USER_GM4     = 19, // GM 4 ��, Web GM ��
	USER_GM3	 = 20, // GM 3 ��
	USER_GM2	 = 21, // GM 2 ��
	USER_GM1	 = 22, // GM 1 ��
	USER_MASTER  = 30, // Master
};

enum EM_NEW_CHA_FB
{
	EM_CHA_NEW_FB_SUB_OK	= 1, // ĳ���� ���� ����
	EM_CHA_NEW_FB_SUB_DUP	= 2, // ���Ӽ���->Ŭ���̾�Ʈ : ���� �̸��� ĳ���Ͱ� �̹� ������
	EM_CHA_NEW_FB_SUB_LENGTH= 3, // ĳ���� ���ڼ� ����
	EM_CHA_NEW_FB_SUB_MAX	= 4, // ���Ӽ���->Ŭ���̾�Ʈ : ���̻�ĳ���� ����� ����(�ִ�ĳ���� ���� �ʰ�)
	EM_CHA_NEW_FB_SUB_ERROR = 5, // ���Ӽ���->Ŭ���̾�Ʈ : �����߻� ����� ����(�ý��ۿ���, ������ �ʴ� �̸�)
	EM_CHA_NEW_FB_SUB_THAICHAR_ERROR = 6, // �±���� �ϼ����� ���� ���� ���� ����
	EM_CHA_NEW_FB_SUB_VNCHAR_ERROR = 7,	 // ��Ʈ�� ���� ���� ����
	EM_CHA_NEW_FB_SUB_BAD_NAME = 8,
    EM_CHA_NEW_FB_SUB_BAD_NAME_KOREA = 9,
    EM_CHA_NEW_FB_SUB_BAD_NAME_WB = 10,
	EM_CHA_NEW_FB_SUB_GM	= 11, // GM ������ ĳ���� ���� �Ұ�
};

enum EMNET_MSG_CHAT
{
	CHAT_TYPE_GLOBAL		= 1, // ������ �۷ι� �޽���
	CHAT_TYPE_NORMAL		= 2, // �Ϲ��� ä�� �޽���
	CHAT_TYPE_PRIVATE		= 3, // �������� �޽���, �Ӹ�, �ӻ���
	CHAT_TYPE_PARTY			= 4, // ��Ƽ������ ���޵Ǵ� �޽���	
	CHAT_TYPE_MASTER		= 5, // ������ �������� �޽���
	CHAT_TYPE_GUILD			= 6, // �������� ���޵Ǵ� �޽���	
	CHAT_TYPE_ALLIANCE		= 7, // ���Ϳ��� ���޵Ǵ� �޽���
	CHAT_TYPE_AREA			= 8, // ���� ä��
	CHAT_TYPE_PARTY_RECRUIT	= 9, // ��Ƽ����
	CHAT_TYPE_LOUDSPEAKER	= 10, // Ȯ����.


	CHAT_TYPE_CTRL_GLOBAL		= 11,
	CHAT_TYPE_CTRL_GLOBAL2	= 12,

	CHAT_TYPE_FACTION		= 13,

	CHAT_TYPE_PRIVATE_PARTY_RECRUIT = 14,

	// ��� ä�� �޼����� GM�� ����ϴ� ���� �ƴϹǷ� �ʿ��Ѱ͸� �߰�
// 	CHAT_ADD_TYPE_FOPR_GM = 100,
// 	CHAT_TYPE_NORMAL_GM	= 102, // GM - �Ϲ��� ä�� �޽���
// 	CHAT_TYPE_PRIVATE_GM	= 103, // GM - �������� �޽���, �Ӹ�, �ӻ���
// 	CHAT_TYPE_PARTY_GM		= 104, // GM - ��Ƽ������ ���޵Ǵ� �޽���	
// 	CHAT_TYPE_GUILD_GM		= 106, // GM -  �������� ���޵Ǵ� �޽���
// 	CHAT_TYPE_AREA_GM		= 108, // GM - ���� ä��
// 	CHAT_TYPE_LOUDSPEAKER_GM	= 110, // GM - Ȯ����.
// 	CHAT_TYPE_FACTION_GM		= 113,
	CHAT_ADD_TYPE_FOPR_GM = 0x100,
	CHAT_TYPE_GLOBAL_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_GLOBAL		,
	CHAT_TYPE_NORMAL_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_NORMAL		, // GM - �Ϲ��� ä�� �޽���
	CHAT_TYPE_PRIVATE_GM		= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_PRIVATE		, // GM - �������� �޽���, �Ӹ�, �ӻ���
	CHAT_TYPE_PARTY_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_PARTY			, // GM - ��Ƽ������ ���޵Ǵ� �޽���	
	CHAT_TYPE_MASTER_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_MASTER		,
	CHAT_TYPE_GUILD_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_GUILD			, // GM -  �������� ���޵Ǵ� �޽���
	CHAT_TYPE_ALLIANCE_GM		= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_ALLIANCE		,
	CHAT_TYPE_AREA_GM			= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_AREA			, // GM - ���� ä��
	CHAT_TYPE_PARTY_RECRUIT_GM	= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_PARTY_RECRUIT	,
	CHAT_TYPE_LOUDSPEAKER_GM	= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_LOUDSPEAKER	, // GM - Ȯ����.
	CHAT_TYPE_CTRL_GLOBAL_GM	= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_CTRL_GLOBAL,
	CHAT_TYPE_CTRL_GLOBAL2_GM	= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_CTRL_GLOBAL2,
	CHAT_TYPE_FACTION_GM		= CHAT_ADD_TYPE_FOPR_GM+CHAT_TYPE_FACTION,
};

enum EM_LOGIN_FB_SUB
{	
    EM_LOGIN_FB_SUB_OK             =  0, //! �α��� ����
	EM_LOGIN_FB_SUB_FAIL	       =  1, //! �α��� ����
	EM_LOGIN_FB_SUB_SYSTEM	       =  2, //! �ý��ۿ����� �α��� ����
	EM_LOGIN_FB_SUB_USAGE	       =  3, //! �㰡�� ���ð� ����
	EM_LOGIN_FB_SUB_DUP		       =  4, //! �ߺ��� ����
	EM_LOGIN_FB_SUB_INCORRECT      =  5, //! ID/PWD ����ġ = (���ο� ������ ���� �Ǵ� ���Է¿䱸),
	EM_LOGIN_FB_SUB_IP_BAN	       =  6, //! ID/PWD �� ��ġ������ IP �� ���Ұ��� ����= (���� ���ܵ� IP),	
	EM_LOGIN_FB_SUB_BLOCK          =  7, //! ID �� ��ϵ� ����
	EM_LOGIN_FB_SUB_UNCON          =  8, //! Daum : �������� �α��ε���Ÿ (���Ӱ� ������ �α����� ���ӽ���)
	EM_LOGIN_FB_SUB_EXPIRED        =  9, //! Daum : �����ð��� �ʰ��� ������̱� ������, ���� �������� �϶�� �޽����� ������
	EM_LOGIN_FB_SUB_GID_ERR        = 10, //! Daum : GID�� ���� ������ ���Ӱ� �α���
	EM_LOGIN_FB_SUB_UID_ERR        = 11, //! Daum : UID�� ���� ������ ���Ӱ� �α���
	EM_LOGIN_FB_SUB_UNKNOWN        = 12, //! Daum : ���� ���Խ���
	EM_LOGIN_FB_SUB_SSNHEAD        = 13, //! Daum : �ֹι�ȣ����
	EM_LOGIN_FB_SUB_ADULT          = 14, //! Daum : ������������
	EM_LOGIN_FB_CH_FULL            = 15, //! ä���� Full �� ���� ���ӺҰ�
	EM_LOGIN_FB_THAI_UNDER18_3HOUR = 16, //! 18 ���̸� 3 �ð� ��� 
	EM_LOGIN_FB_THAI_UNDER18_TIME  = 17, //! 18 ���̸� ���ӺҰ��� �ð�
	EM_LOGIN_FB_THAI_OVER18_TIME   = 18, //! 18 ���̻� ���ӺҰ��� �ð�
	EM_LOGIN_FB_SUB_RANDOM_PASS    = 19, //! �����н����� ����
	EM_LOGIN_FB_SUB_PASS_OK		   = 20, //! �ʱ� �н����� ������Ʈ ����
	EM_LOGIN_FB_SUB_ALREADYOFFLINE = 21, //! �α��μ��������� �̹� ���������϶�
	EM_LOGIN_FB_SUB_SECID_ALREADY  = 22, //! �Ƶڰ� �ߺ��˴ϴ�.
	EM_LOGIN_FB_SUB_BETAKEY		   = 23, //! ��ŸŰ ���� ����
	EM_LOGIN_FB_SUB_IMPOSSIBLEIP   = 24, //! ���� �Ұ����� IP
	EM_LOGIN_FB_WRONG_SP		   = 25, //! �߸��� ServiceProvider
    EM_LOGIN_FB_KOR_SHUTDOWN	   = 26, //! �ѱ�, Shutdown 16������ ���ӺҰ� �ð�
	EM_LOGIN_FB_KR_OK_USE_PASS		= 27, // �α��� ���� + 2����� ��� , 2����� �����ϴ� ���� 0 ���� ó��
	EM_LOGIN_FB_KR_OK_NEW_PASS		= 28, // �α��� ���� + 2����� �ű� ����
	EM_LOGIN_FB_ITEM_TRANSFER		= 29, // ������ ��ȯ ���̴�. ����Ŀ� �����ض�.
};

// �������
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

// �� ����� DB ���
enum ERanMobileResult
{
	ERanMobileResult_OK = 1,

	// ĳ���� ����
	EResult_CreateChar_DuplicateName = -2,	// �г��� �ߺ�
	EResult_CreateChar_TimeError = -3,		// ���� �ð� ���� ����
	EResult_CreateChar_NoUserInfo = -4,		// ���� ���� ����
	EResult_CreateChar_NoMoney = -5,		// ������ ����
	EResult_CreateChar_NoItem = -6,			// �������� �ʴ� ������		

	// ����
	EResult_Extend_TimeError = -2,			// ���� �ð��� �߸� ����
	EResult_Extend_InsertError = -3,		// ���� ����
	EResult_Extend_CharInfo = -4,			// ĳ���� ���� ����
	EResult_Extend_CharInfo2 = -5,			// ĳ���� ���� ����
	EResult_Extend_NoMoney = -6,			// ������ ����

	// ���� ������
	EResult_SendJewel_Count = -2,			// ���� ������ �߸� ����
	EResult_SendJewel_InsertError = -3,		// ���� ����
	EResult_SendJewel_NoItem = -4,			// �������� �ʴ� ������

	// MPoint
	EResult_MPoint_Count = -2,				// ������ MPoint �� �߸� ����
	EResult_MPoint_OverCount = -3,			// �� MPoint ���� ū �� ����
	EResult_MPoint_InsertError = -4,		// ���� ����

	// ��ȯ
	EResult_Change_NoData = -2,				// ��ȯ ������ ����
	EResult_Change_MPoint = -3,				// MPoint ����
	EResult_Change_FullInventory = -4,		// �κ��丮 ������

	// ���� ����
	EResult_DailyReward_UpdateError = -2,		// DB ������Ʈ ����
	EResult_DailyReward_FullInventory = -3,		// �κ��丮 ������
	EResult_DailyReward_Already = -4,			// �̹� ����
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

// �Ʒ� �⺻ �޼����� �ߺ��� ���� �ʵ��� �����Ѵ�.
//#if defined(CH_PARAM)
//#define NET_MSG_BASE				1019				  // �⺻ �޽���
//#elif defined(HK_PARAM)
//#define NET_MSG_BASE				1007				  // �⺻ �޽���
//#elif defined(ID_PARAM)
//#define NET_MSG_BASE				977					  // �⺻ �޽���
//#elif defined(JP_PARAM)
//#define NET_MSG_BASE				1031				  // �⺻ �޽���
//#elif defined(KR_PARAM)
//#define NET_MSG_BASE				987					  // �⺻ �޽���
//#elif defined(KRT_PARAM)
//#define NET_MSG_BASE				971					  // �⺻ �޽���
//#elif defined(MY_PARAM)
//#define NET_MSG_BASE				969					  // �⺻ �޽���
//#elif defined(MYE_PARAM)
//#define NET_MSG_BASE				1005				  // �⺻ �޽���
//#elif defined(PH_PARAM)
//#define NET_MSG_BASE				1017				  // �⺻ �޽���
//#elif defined(VN_PARAM)
//#define NET_MSG_BASE				1021				  // �⺻ �޽���
//#elif defined(TW_PARAM)
//#define NET_MSG_BASE				997					  // �⺻ �޽���
//#elif defined(TH_PARAM)
//#define NET_MSG_BASE				989					  // �⺻ �޽���
//#elif defined(GS_PARAM)
//#define NET_MSG_BASE				1004				  // �⺻ �޽���
//#else
//#define NET_MSG_BASE				995					  // �⺻ �޽���
//#endif

/*
#define NET_MSG_LGIN				(NET_MSG_BASE +  507) // �α��μ��� �޽���
#define NET_MSG_LOBBY				(NET_MSG_BASE + 1013) // ���Ӽ��� �κ�
#define NET_MSG_LOBBY_MAX			(NET_MSG_BASE + 1509) // ���Ӽ��� �κ� �ִ밪

#define NET_MSG_GCTRL				(NET_MSG_BASE + 2011) // ���Ӽ��� �����÷���
#define NET_MSG_GCTRL_MAX			(NET_MSG_BASE + 4010) // ���Ӽ��� �����÷��� �ִ밪
*/

// 2005-11-04
//#define NET_MSG_LGIN				(NET_MSG_BASE +  413) // �α��μ��� �޽���
//#define NET_MSG_LOBBY				(NET_MSG_BASE +  913) // ���Ӽ��� �κ�
//#define NET_MSG_LOBBY_MAX			(NET_MSG_BASE + 1413) // ���Ӽ��� �κ� �ִ밪
//
//#define NET_MSG_GCTRL				(NET_MSG_BASE + 1823) // ���Ӽ��� �����÷���
//#define NET_MSG_GCTRL_MAX			(NET_MSG_BASE + 3823) // ���Ӽ��� �����÷��� �ִ밪

// 2007-04-11

//#define NET_MSG_LGIN				(NET_MSG_BASE +  450) // �α��μ��� �޽���
//#define NET_MSG_LOBBY				(NET_MSG_BASE +  950) // ���Ӽ��� �κ�
//#define NET_MSG_LOBBY_MAX			(NET_MSG_BASE + 1450) // ���Ӽ��� �κ� �ִ밪

//#define NET_MSG_GCTRL				(NET_MSG_BASE + 1900) // ���Ӽ��� �����÷���
//#define NET_MSG_GCTRL_MAX			(NET_MSG_BASE + 4000) // ���Ӽ��� �����÷��� �ִ밪

struct G_SERVER_INFO
{
    net::EMSERVERTYPE ServerType;  ///< ���� ����
    int	nServerGroup;			  ///< ���� �׷� ��ȣ
    int	nServerNumber;			  ///< ���� ��ȣ
    int	nServerField;			  ///< �ʵ��ȣ	
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
    net::EMSERVERTYPE ServerType;  ///< ���� ����
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
    net::EMSERVERTYPE ServerType;  ///< ���� ����
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
	net::EMSERVERTYPE serverType;  ///< ���� ����
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
