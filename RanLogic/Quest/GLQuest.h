#ifndef GLQUEST_H_
#define GLQUEST_H_

#pragma once

#include <string>
#include <vector>
#include <boost/serialization/vector.hpp>

#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/SerialFile.h"

#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../Item/GLItem.h"
#include "../Inventory/GLInventoryQuest.h"

//#include "../GLogicData.h"
enum EMQUESTSTEP_COMMENT_TEXT
{
	EMQSCT_NPCTALK = 0,			// 을/를 만나라;
	EMQSCT_MOBGEN_ITEM,			// 몹에게서 습득할 아이템;
	EMQSCT_MOBKILL,				// 죽여야할 몹;
	EMQSCT_NPCQUARD,			// 보호해야함;
	EMQSCT_REACH_ZONE,			// 어느 위치 도달;
	EMQSCT_GUARD_MAP_COMMENT,	// 어느 위치 방어;
	EMQSCT_ITEM_USE,			// 사용해야할 아이템;
	EMQSCT_ITEM_GRINDING,		// 연마해야할 아이템;
	EMQSCT_LEVEL_UP_COMMENT,	// 레벨을 올려라;
};	  

enum EMQUESTJOIN
{
	EMQJ_NONE = 0,
	EMQJ_ATTENDANCE,
	EMQJ_JOIN,
};

enum EMQUESTFLAGS
{
	EMQF_REPEAT		= 0x0001,		//	완료된 퀘스트 수행 가능 여부.
	EMQF_AGAIN		= 0x0002,		//	실패한 퀘스트 수행 가능 여부.
	EMQF_ONLYONE	= 0x0004,		//	이 퀘스트는 다른 퀘스트와 동시 진행 불가능.
	EMQF_GIVEUP		= 0x0008,		//	퀘스트 포기 가능 여부.
	EMQF_DAILY		= 0x0010,		//	일일 퀘스트 인가?
	
	EMQF_PARTY		= 0x1000,		//	파티만 참여 가능한 퀘스트.
	EMQF_PARTYQUEST	= 0x2000,		//	파티원 동시 진행 퀘스트.
};


enum EMQUESTPROGRESS
{
	EMQP_NON_EVENT = 0, // 이벤트 없음
	EMQP_CHAR_DEAD,		// 캐릭터 사망
	EMQP_START_MOVE,	// 시작지점으로 이동
    EMQP_SELECT_MOVE,   // 지정된 위치로 이동
	EMQP_SIZE,
};

enum EMFAILTYPE
{
	EMFAIL_NONE		= 0x0000,		// 타입없음
	EMFAIL_TIMEOVER = 0x0001,		// 타임오버로 실패시
	EMFAIL_DIE      = 0x0002,		// 죽어서 실패시
	EMFAIL_LEAVE    = 0x0004,		// 특정 맵에서 떠나서 실패시
};

enum EMGRINDINGTYPE
{
	EMGRINDING_NONE		= 0,	// 특정아이템
	EMGRINDING_WEAPON	= 1,	// 무기
	EMGRINDING_ARMOR	= 2,	// 방어구
	EMGRINDING_ALL		= 3,	// 모두
};

enum EMATTENDANCE_TYPE
{
	EMATTENDANCE_TYPE_NONE =  0,
	EMATTENDANCE_TYPE_CONNECTION,	//특정 시간동안 접속을 유지하라
	EMATTENDANCE_TYPE_USER_KILL,	//특정 지점에서 특정 횟수만큼 유저를 처치하라
	EMATTENDANCE_TYPE_MOB_KILL,	//특정 몹을 특정 횟수로 처치하라
	EMATTENDANCE_TYPE_LEVEL_UP,	//특정 레벨을 당성하라 
	EMATTENDANCE_TYPE_ITEM_GET,	//특정 아이템을 수집하라
	EMATTENDANCE_TYPE_ITEM_USE,	//특정 아이템을 특정 횟수로 사용하라
	EMATTENDANCE_TYPE_MOVE_POS,	//특정 맵(혹은 특정 지점)으로 이동하라
	EMATTENDANCE_TYPE_GRINDING_ITEM,	// 아이템을 특정 횟수만큼 연마하라
	EMATTENDANCE_TYPE_SPARRING,	//다른파티와 학원 대련을 특정 횟수만큼 실시하라
	EMATTENDANCE_TYPE_SIZE,
};

enum EMDAYOFWEEKFLAGS 
{
	EMF_SUN	= 0x0001,
	EMF_MON = 0x0002,
	EMF_TUE = 0x0004,
	EMF_WED = 0x0008,
	EMF_THU = 0x0010,
	EMF_FRI = 0x0020,
	EMF_SAT = 0x0040,
	EMF_ALL	= EMF_SUN|EMF_MON|EMF_TUE|EMF_WED|EMF_THU|EMF_FRI|EMF_SAT,
};

struct GLQUEST_ATTENDANCE_OPTION
{
	enum { VERSION = 0x0001, };

	bool bAdditionalQuest;
	int nType; // EMATTENDANCE_TYPE
	WORD wMinLevel; // 퀘스트 수행시 제한 레밸
	WORD wMaxLevel; // 퀘스트 수행시 제한 레밸
	DWORD dwDayofWeek; // 퀘스트 시작시 요일.

	WORD wStartYear; // 퀘스트 시작시 제한 년
	WORD WStarMonth; // 퀘스트 시작시 제한 월
	WORD WstarDay; // 퀘스트 시작시 제한 일

	WORD wEndYear; // 퀘스트 시작시 제한 년
	WORD WEndMonth; // 퀘스트 시작시 제한 월
	WORD WEndDay; // 퀘스트 시작시 제한 일


	GLQUEST_ATTENDANCE_OPTION () :
	bAdditionalQuest(false),
	nType(EMATTENDANCE_TYPE_NONE),
	wMinLevel(0),
	wMaxLevel(USHRT_MAX),
	dwDayofWeek(0),
	wStartYear(0),
	WStarMonth(0),
	WstarDay(0),
	wEndYear(0),
	WEndMonth(0),
	WEndDay(0)
	{
	}

	bool LOAD_0001 ( sc::BaseStream &SFile );
	bool LOAD ( sc::BaseStream &SFile );
	bool SAVE ( sc::SerialFile &SFile );
};

typedef std::vector<DWORD>			DWQARRAY;
typedef DWQARRAY::iterator			DWQARRAY_ITER;

struct GLQUEST_PROGRESS_MAP
{
	SNATIVEID	nidMAP;
	WORD		wPosX;
	WORD		wPosY;
	DWORD		dwGateID;

	GLQUEST_PROGRESS_MAP() :
	nidMAP(false),
		wPosX(USHRT_MAX),
		wPosY(USHRT_MAX),
		dwGateID(0)
	{
	}

	void Init()
	{
		nidMAP   = false;
		wPosX    = USHRT_MAX;
		wPosY    = USHRT_MAX;
		dwGateID = 0;
	}
};


struct SGENQUESTITEM
{
	enum { VERSION = 0x0001, 
		GENMOB_MAX = 10, 
	};

	SNATIVEID	sNID;
	WORD		wNUM;
	float		fGEN_RATE;
	DWQARRAY	vecGEN_MOB;

	SGENQUESTITEM () :
		sNID(false),
		wNUM(1),
		fGEN_RATE(10.0f)
	{
	}

	SGENQUESTITEM& operator= ( const SGENQUESTITEM& rvalue );

	bool Find ( DWORD dwMOB );
	BOOL GENMOB_PUSH ( DWORD dwID );
	void GENMOB_ERASE ( DWORD dwIndex );

	bool LOAD ( sc::BaseStream &SFile );
	bool SAVE ( sc::SerialFile &SFile );
};

typedef std::vector<SGENQUESTITEM>	GENMOBITEMARRAY;
typedef GENMOBITEMARRAY::iterator	GENMOBITEMARRAY_ITER;

typedef std::vector<SITEMCUSTOM>	INVENQARRAY;
typedef INVENQARRAY::iterator		INVENQARRAY_ITER;

namespace QUEST
{
    const std::string TextExportBegin         = "<BEGIN>";
    const std::string TextExportEnd           = "<END>";
    const std::string TextExportSubject       = "<SUBJECT>";
    const std::string TextExportSubjectEnd    = "</SUBJECT>";
    const std::string TextExportDesc          = "<DESCRIPTION>";
    const std::string TextExportDescEnd       = "</DESCRIPTION>";
    const std::string TextExportQuestId       = "<QUEST_ID>";
    const std::string TextExportProgress      = "<PROGRESS>";
    const std::string TextExportProgressEnd   = "</PROGRESS>";
    const std::string TextExportNpcTalk       = "<NPCTALK>";
    const std::string TextExportNpcTalkEnd    = "</NPCTALK>";
    const std::string TextExportKillMob       = "<KILLMOB>";
    const std::string TextExportKillMobEnd    = "</KILLMOB>";
	const std::string TextExportItemUse       = "<ITEMUSE>";
	const std::string TextExportItemUseEnd    = "</ITEMUSE>";
    const std::string TextExportProtectNpc    = "<PROTECTNPC>";
    const std::string TextExportProtectNpcEnd = "</PROTECTNPC>";
    const std::string TextExportMovePos       = "<MOVE2POS>";
    const std::string TextExportMovePosEnd    = "</MOVE2POS>";
    const std::string TextExportGetItem       = "<GETITEM>";
    const std::string TextExportGetItemEnd    = "</GETITEM>";
    const std::string TextExportProtectPos    = "<PROTECTPOS>";
    const std::string TextExportProtectPosEnd = "</PROTECTPOS>";

	const std::string TextExportItemGrinding       = "<GRINDING>";
	const std::string TextExportItemGrindingEnd   = "</GRINDING>";

	const std::string TextExportConfting       = "<CONFTING>";
	const std::string TextExportConftingEnd    = "</CONFTING>";

	const std::string TextExportLevelUp      = "<LEVELUP>";
	const std::string TextExportLevelUpEnd    = "</LEVELUP>";
} // namespace QUEST

struct GLQUEST_STEP
{
	enum { 
		VERSION = 0x0026,
		NEEDQITEM_MAX = 5,
	};

	std::string		m_strTITLE;				//	현재 단계에 대한 제목.
	std::string		m_strCOMMENT;			//	현재 단계에 대한 설명.

	//	이수 조건 #1.
	DWORD			m_dwNID_MOBKILL;		//	죽여야할 Mob ID.
	DWORD			m_dwNUM_MOBKILL;		//	죽여야할 Mob 숫자.
	std::string		m_strOBJ_MOBKILL;		//	이수조건 설명.
	//  이수 조건 #1-1 추가
	bool			m_bIsPLAYERKILL;		// 플레이어를 죽이는 퀘스트인가


	//	이수 조건 #2.
	GENMOBITEMARRAY	m_vecMOBGEN_QITEM;		//	MOB 발생 퀘스트 아이템.
	std::string		m_strOBJ_MOBGEN_QITEM;	//	이수조건 설명.

	//	이수 조건 #3.
	DWORD			m_dwNID_NPCTALK;		//	대화가 필요한 NPC ID.
	std::string		m_strOBJ_NPCTALK;		//	이수조건 설명.

	//	이수 조건 #4.
	SNATIVEID		m_sMAPID_REACH;			//	도달해야할 MAPID.
	WORD			m_wPOSX_REACH;			//	도달해야할 x좌표.	( 미니멥에서 쓰는 좌표. )
	WORD			m_wPOSY_REACH;			//	도달해야할 y좌표.	( 미니멥에서 쓰는 좌표. )
	WORD			m_wRADIUS_REACH;		//	도달 반경.
	std::string		m_strOBJ_REACH_ZONE;	//	이수 조건 설명.

	//	이수 조건 #5.
	WORD			m_wLevel;				//	레벨 도달.


	//	이수 조건 #6.
	DWORD			m_dwNID_NPCGUARD;		//	보호해야할 NPC.
	std::string		m_strOBJ_NPCGUARD;		//	이수조건 설명.

	//	이수 조건 #7.
	SNATIVEID		m_sMAPID_DEFENSE;		//	방어해야할 MAPID.
	WORD			m_wPOSX_DEFENSE;		//	방어해야할 x좌표.	( 미니멥에서 쓰는 좌표. )
	WORD			m_wPOSY_DEFENSE;		//	방어해야할 y좌표.	( 미니멥에서 쓰는 좌표. )
	WORD			m_wRADIUS_DEFENSE;		//	방어 반경.
	float			m_fDEFENSE_TIME;		//	방어해야할 시간.
	std::string		m_strOBJ_DEFENSE_ZONE;	//	이수 조건 설명.

	//	이수 조건 #8.
	DWORD			m_dwNID_ITEMUSE;		//	사용해야할 아이템 ID.
	DWORD			m_dwNUM_ITEMUSE;		//	사용해야할 횟수
	std::string		m_strOBJ_ITEMUSE;		//	이수조건 설명.

	// 이수 조건 # 9
	DWORD			m_dwNID_ITEMGRINDING;	//	연마해야할 아이템 ID
	DWORD			m_dwNUM_ITEMGRINDING;	//	연마해야할 횟수
	DWORD			m_dwItemType;			//	지정 아이템, 무기, 방어구, 모두
	bool			m_bConsecution;			//	연속 연마
	std::string		m_strOBJ_ITEMGRINDING;  //	이수조건 설명.

	// 이수 조건 # 10
	DWORD			m_dwType_CONFTING;		//	혼자대련,파티대련
	bool			m_bSCHOOL;				//	학원대련
	DWORD			m_dwNUM_CONFTING;		//	대련 횟수
	std::string		m_strOBJ_CONFTING;		//	이수조건 설명.

	//	이수 조건 #11.
	DWORD			m_dwNUM_LEVELUP;		//	증가시켜야할 레벨
	std::string		m_strOBJ_LEVELUP;		//	이수조건 설명.


	//	완료시 처리.
	INVENQARRAY			 m_vecGIFT_QITEM;		//	현제 STEP 완료시 보상 Quest Item.
	DWQARRAY			 m_vecRESET_QITEM;		//	다음 스텟으로 넘어갈때 리셋될 Quest Item.
	
	GLQUEST_PROGRESS_MAP m_stepMoveMap;

	GLQUEST_STEP()
        : m_strTITLE("현재 단계에 대한 제목")
        , m_strCOMMENT("현재 단계에 대한 설명")
        , m_strOBJ_NPCTALK("대화해야 함")
        , m_dwNID_NPCTALK(UINT_MAX)
        , m_strOBJ_MOBKILL("퇴치해야 할 몬스터")
        , m_dwNID_MOBKILL(UINT_MAX)
        , m_dwNUM_MOBKILL(0)
		, m_bIsPLAYERKILL(false)
        , m_strOBJ_MOBGEN_QITEM("몹에게서 습득할 아이템")
        , m_strOBJ_NPCGUARD("보호해야 함")
        , m_dwNID_NPCGUARD(UINT_MAX)
        , m_strOBJ_REACH_ZONE("어느 위치도달")
        , m_sMAPID_REACH(false)
        , m_wPOSX_REACH(0)
        , m_wPOSY_REACH(0)
        , m_wRADIUS_REACH(60)
        , m_strOBJ_DEFENSE_ZONE("어느 위치방어")
        , m_sMAPID_DEFENSE(false)
        , m_wPOSX_DEFENSE(0)
        , m_wPOSY_DEFENSE(0)
        , m_wRADIUS_DEFENSE(0)
        , m_fDEFENSE_TIME(0.0f)
        , m_wLevel(USHRT_MAX)
		, m_dwNID_ITEMUSE(UINT_MAX)
		, m_dwNUM_ITEMUSE(0)
		, m_strOBJ_ITEMUSE("사용해야할 아이템")
		, m_dwNID_ITEMGRINDING(UINT_MAX)
		, m_dwNUM_ITEMGRINDING(0)
		, m_dwItemType(0)
		, m_bConsecution(false)
		, m_strOBJ_ITEMGRINDING("연마해야할 아이템")
		, m_dwType_CONFTING(EMCONFT_NONE)
		, m_bSCHOOL(false)
		, m_dwNUM_CONFTING(0)
		, m_strOBJ_CONFTING("대련을 실시하라")
		, m_dwNUM_LEVELUP(0)
		, m_strOBJ_LEVELUP("레벨을 올려라")
	{
	}

	const char* GetTITLE() const			 { return m_strTITLE.c_str(); }
    const std::string& GetTitleStr() const	 { return m_strTITLE; }
	const char* GetCOMMENT() const			 { return m_strCOMMENT.c_str(); }
    const std::string& GetCommentStr() const { return m_strCOMMENT; }
	
	const char* GetOBJ_NPCTALK() const		{ return m_strOBJ_NPCTALK.c_str(); }
	const char* GetOBJ_MOBGEN_QITEM() const	{ return m_strOBJ_MOBGEN_QITEM.c_str(); }
	const char* GetOBJ_MOBKILL() const		{ return m_strOBJ_MOBKILL.c_str(); }
	const char* GetOBJ_ITEMUSE() const		{ return m_strOBJ_ITEMUSE.c_str(); }
	const char* GetOBJ_CONFTING() const		{ return m_strOBJ_CONFTING.c_str(); }
	const char* GetOBJ_ITEMGRINDING() const	{ return m_strOBJ_ITEMGRINDING.c_str(); }
	const char* GetOBJ_NPCGUARD() const		{ return m_strOBJ_NPCGUARD.c_str(); }
	const char* GetOBJ_REACH_ZONE() const	{ return m_strOBJ_REACH_ZONE.c_str(); }
	const char* GetOBJ_DEFENSE_ZONE() const	{ return m_strOBJ_DEFENSE_ZONE.c_str(); }
	const char* GetOBJ_LEVEL_UP() const	{ return m_strOBJ_LEVELUP.c_str(); }

	bool IsNEED_NPCTALK() const		{ return (m_dwNID_NPCTALK!=UINT_MAX); }
	bool IsNEED_QITEM() const		{ return (!m_vecMOBGEN_QITEM.empty()); }
	bool IsNEED_MOBKILL() const		{ return (m_dwNID_MOBKILL!=UINT_MAX); }
	bool IsNEED_NPCGUARD() const	{ return (m_dwNID_NPCGUARD!=UINT_MAX); }
	bool IsNEED_REACHZONE() const	{ return (m_sMAPID_REACH!=SNATIVEID(false)); }
	bool IsNEED_DEFENSEZONE() const	{ return (m_sMAPID_DEFENSE!=SNATIVEID(false)); }
	bool IsNEED_LEVEL() const		{ return (m_wLevel!=USHRT_MAX); }
	bool IsNEED_PLAYERKILL() const	{ return (m_bIsPLAYERKILL);}
	bool IsNEED_ITEMUSE() const		{ return (m_dwNID_ITEMUSE!=UINT_MAX); }
	bool IsNEED_ITEMGRINDING() const		{ return (m_dwNID_ITEMGRINDING!=UINT_MAX)||(m_dwItemType!=0); }
	bool IsNEED_CONFTING() const	{ return (m_dwType_CONFTING!=EMCONFT_NONE); }
	bool IsNEED_LEVELUP() const	
	{ 
		return (m_dwNUM_LEVELUP>0);
	}

	void RESET_NPCTALK ()
	{
		m_strOBJ_NPCTALK = "";
		m_dwNID_NPCTALK = UINT_MAX;
	}

	void RESET_MOBGEN_QITEM ()
	{
		m_strOBJ_MOBGEN_QITEM = "";
		m_vecMOBGEN_QITEM.clear();
	}

	void RESET_MOBKILL ()
	{
		m_strOBJ_MOBKILL = "";
		m_dwNID_MOBKILL = UINT_MAX;
		m_dwNUM_MOBKILL = 0;
	}

	void RESET_ITEMUSE ()
	{
		m_dwNID_ITEMUSE = UINT_MAX;
		m_dwNUM_ITEMUSE = 0;
		m_strOBJ_ITEMUSE = "";
	}

	void RESET_NPCGUARD ()
	{
		m_strOBJ_NPCGUARD = "";
		m_dwNID_NPCGUARD = UINT_MAX;
	}

	void RESET_REACHZONE ()
	{
		m_strOBJ_REACH_ZONE = "";
		m_sMAPID_REACH = SNATIVEID(false);
		m_wPOSX_REACH = (0);
		m_wPOSY_REACH = (0);
		m_wRADIUS_REACH = (60);
	}

	void RESET_DEFENSE ()
	{
		m_strOBJ_DEFENSE_ZONE = "";
		m_sMAPID_DEFENSE = SNATIVEID(false);
		m_wPOSX_DEFENSE = (0);
		m_wPOSY_DEFENSE = (0);
		m_wRADIUS_DEFENSE = (0);
		m_fDEFENSE_TIME = (0.0f);
	}

	void RESET_LEVEL ()
	{
		m_wLevel = USHRT_MAX;
	}

	void RESET_ITEMGRINDING()
	{
		m_dwNID_ITEMGRINDING = UINT_MAX;	
		m_dwNUM_ITEMGRINDING = 0;	
		m_dwItemType = 0;
		m_bConsecution = false;
		m_strOBJ_ITEMGRINDING = "";  
	}

	void RESET_CONFTING()
	{
		m_dwType_CONFTING = EMCONFT_NONE;
		m_bSCHOOL =  false;
		m_dwNUM_CONFTING = 0;
		m_strOBJ_CONFTING = "";
	}

	void RESET_LEVELUP()
	{
		m_dwNUM_LEVELUP = 0;
		m_strOBJ_LEVELUP = "";
	}

	bool LOAD_0001 ( sc::BaseStream &SFile );
	bool LOAD_0002 ( sc::BaseStream &SFile );
	bool LOAD_0003 ( sc::BaseStream &SFile );
	bool LOAD_0004 ( sc::BaseStream &SFile );
	bool LOAD_0005 ( sc::BaseStream &SFile );
	bool LOAD_0006 ( sc::BaseStream &SFile );
	bool LOAD_0007 ( sc::BaseStream &SFile );
	bool LOAD_0008 ( sc::BaseStream &SFile );
	bool LOAD_0009 ( sc::BaseStream &SFile );
	bool LOAD_0010 ( sc::BaseStream &SFile );
	bool LOAD_0011 ( sc::BaseStream &SFile );
	bool LOAD_0012 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
	bool LOAD_0013 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
    bool LOAD_0014 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다.
	bool LOAD_0015 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다.
	bool LOAD_0016 ( sc::BaseStream &SFile );	// 출석부 관련 퀘스트 추가
    bool LOAD_0017 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다.
    bool LOAD_0018 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다.
	bool LOAD_0019 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다.
	bool LOAD_0020 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다.
	bool LOAD_0021 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다.
	bool LOAD_0022 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다.
	bool LOAD_0023 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다;
	bool LOAD_0024 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다;
	bool LOAD_0025 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다;
	bool LOAD_0026 ( sc::BaseStream &SFile );	// 아이템 데이터가 변경되었다;
	// 위에 SITEMCUSTOM들 버전 하나씩 밀리는거 아닌가?;

	bool LOAD ( sc::BaseStream &SFile );
	bool SAVE ( sc::SerialFile &SFile );

	GLQUEST_STEP& operator= ( const GLQUEST_STEP &rVALUE );

	void RESETITEM_ERASE ( DWORD dwIndex );
	void GIFTITEM_ERASE ( DWORD dwIndex );

	BOOL MOBGENITEM_PUSH( SGENQUESTITEM& sITEM );
	void MOBGENITEM_ERASE( DWORD dwIndex );
};

struct GLQUEST_START
{
	enum { VERSION = 0x0004, };

	DWORD		dwCLASS;
	WORD		wSCHOOL;
	
	WORD		wLEVEL;

	SNATIVEID	nidSKILL;
	SNATIVEID	nidITEM;

	SNATIVEID	nidMAP;
	WORD		wSIZE;
	WORD		wPosX;
	WORD		wPosY;

	int			nJOIN;

	GLQUEST_START () :
		dwCLASS(GLCC_ALL_TRICKER),
		wSCHOOL(10),	//MAX_SCHOOL 10 - MAX_SCHOOL을 쓸 경우에 헤더 파일의 교차 include 때문에 재정의 문제가 발생

		nidITEM(false),
		nidSKILL(false),

		nidMAP(false),
		wPosX(USHRT_MAX),
		wPosY(USHRT_MAX),
		wSIZE(0),

		wLEVEL(USHRT_MAX),
		nJOIN(EMQJ_NONE)
	{
	}

	bool LOAD_001( sc::BaseStream &SFile );
	bool LOAD_002( sc::BaseStream &SFile );
	bool LOAD_003( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
	bool LOAD_004( sc::BaseStream &SFile );	//	출석부 관력 퀘스트 추가

	bool LOAD ( sc::BaseStream &SFile );
	bool SAVE ( sc::SerialFile &SFile );
};


struct GLQUEST_PROGRESS 
{
	DWORD dwStartProgress;  // 시작시 이벤트
	DWORD dwEndProgress;	  // 종료시 이벤트
	DWORD dwFailProgress;	  // 실패시 이벤트
		
	DWORD			dwfailType;		  // 실패의 조건

	GLQUEST_PROGRESS_MAP startMap;
	GLQUEST_PROGRESS_MAP endMap;
	GLQUEST_PROGRESS_MAP failMap;

	bool				 bUseProgressEvent;


	GLQUEST_PROGRESS()
	{
		Init();
	}

	VOID Init()
	{
		dwStartProgress = EMQP_NON_EVENT;
		dwEndProgress   = EMQP_NON_EVENT;
		dwFailProgress  = EMQP_NON_EVENT;

		dwfailType    = 0;

		startMap.Init();	
		endMap.Init();
		failMap.Init();

		bUseProgressEvent = FALSE;
	};

	bool IsTIMEOVER ()		{ return (dwfailType&EMFAIL_TIMEOVER)!=NULL; }
	bool IsDIE ()			{ return (dwfailType&EMFAIL_DIE)!=NULL; }	
	bool IsLEAVE ()			{ return (dwfailType&EMFAIL_LEAVE)!=NULL; }	
    
};

class GLQUEST
{
public:
	enum { VERSION = 0x0029, };

	typedef std::vector<GLQUEST_STEP>	VECQSTEP;
	typedef VECQSTEP::iterator			VECQSTEP_ITER;

public:
    GLQUEST();
    ~GLQUEST();
    GLQUEST& operator= (const GLQUEST& rVALUE);

public:
	SNATIVEID		m_sNID;					//	고유 ID.
	DWORD			m_dwEDITVER;			//	편집 버전.
											//	( 종전 퀘스트 설정과 완전히 구도가 틀려저서 종전 설정으로 
											//	플레이한 사람이 진행하기 불가능할 경우. )

	std::string		m_strTITLE;				//	퀘스트 타이틀.
	std::string		m_strCOMMENT;			//	퀘스트 설명.
	
	DWORD			m_dwFlags;				//	속성.

    DWORD           m_dwQuestArea;          // 퀘스트 지역.

	GLQUEST_START	m_sSTARTOPT;			//	자동 시작 조건.

	GLQUEST_ATTENDANCE_OPTION m_sAttendanceOption; // 출석부 퀘스트 옵션

	//	최초 부여 조건.
	DWORD			m_dwBeginPartyMemNum;	//	퀘스트를 받기 위한 최소 파티 인원.
	DWORD			m_dwBeginMoney;			//	퀘스트 받기 위한 비용.

	//	진행 조건.
	DWORD			m_dwLimitTime;			//	제한 시간.	( 실제 시간의 분단위. )
	bool			m_bTimeLimiorKeep;		//	제한, 유지 시간 ex) 제한시간 10분, 10분동안 접속유지
	bool			m_bPastOnlyOnOline ;	//	클라이언트 종료후에도 시간 흐름
	bool			m_bNonDie;				//	수행 도중 사망하면 안됨.
	DWORD			m_dwLimitPartyMemNum;	//	제한 파티 인원.

	VECQSTEP		m_vecProgStep;			//	전체 진행 스텝 정보.

	//	완료시 보상.
	DWORD			m_dwGiftMONEY;
	LONGLONG		m_lnGiftEXP;
	DWORD			m_dwGiftSKILLPOINT;
	DWORD			m_dwGiftSTATSPOINT;
	DWORD			m_dwGiftLIFEPOINT;
	DWORD			m_dwGiftDisPK;
	DWORD			m_dwGiftELEMENT;

	DWORD			m_dwGiftQUEST;
	INVENQARRAY		m_vecGiftITEM;
	DWQARRAY		m_vecGiftSKILL;
	
	//	퀘스트 진행 중에 적용되는 이벤트
	GLQUEST_PROGRESS m_ProgressEvent;

public:
	const char* GetTITLE() const;
    const std::string& GetTitleStr() const;
	const char* GetCOMMENT() const;
	const std::string& GetCommentStr() const;

	DWORD GetSTEPNUM() const { return (DWORD) m_vecProgStep.size(); }
	GLQUEST::VECQSTEP& GetSTEP() { return m_vecProgStep; }
    const GLQUEST::VECQSTEP& GetSTEP() const { return m_vecProgStep; }
	GLQUEST_STEP* GetSTEP(DWORD dwSTEP);

public:
	DWORD ToUpStep ( DWORD dwSTEP );
	DWORD ToDownStep ( DWORD dwSTEP );

public:
	bool IsREPEAT ()		{ return (m_dwFlags&EMQF_REPEAT)!=NULL; }		//	여러번 수행 가능.
	bool IsAGAIN ()			{ return (m_dwFlags&EMQF_AGAIN)!=NULL; }		//	(포기한것) 다시 수행 가능.
	bool IsPARTY ()			{ return (m_dwFlags&EMQF_PARTY)!=NULL; }		//	파티일때 수행 가능.
	bool IsPARTYQUEST ()	{ return (m_dwFlags&EMQF_PARTYQUEST)!=NULL; }	//	파티원 동시 진행 퀘스트.

	bool IsONLYONE ()		{ return (m_dwFlags&EMQF_ONLYONE)!=NULL; }		//	동시에 진행불가?
	bool IsGIVEUP ()		{ return (m_dwFlags&EMQF_GIVEUP)!=NULL; }		//	포기 가능
	
	bool IsLIMITTIME ()		{ return m_dwLimitTime!=0; }
	bool IsNonDIE ()		{ return m_bNonDie; }

	bool IsDaily()			{ return (m_dwFlags&EMQF_DAILY)!=NULL;	}		//	일일 퀘스트인가?

protected:
	bool LOAD_0001 ( sc::BaseStream &SFile );
	bool LOAD_0002 ( sc::BaseStream &SFile );
	bool LOAD_0003 ( sc::BaseStream &SFile );
	bool LOAD_0004 ( sc::BaseStream &SFile );
	bool LOAD_0005 ( sc::BaseStream &SFile );
	bool LOAD_0006 ( sc::BaseStream &SFile );
	bool LOAD_0007 ( sc::BaseStream &SFile );
	bool LOAD_0008 ( sc::BaseStream &SFile );
	bool LOAD_0009 ( sc::BaseStream &SFile );
	bool LOAD_0010 ( sc::BaseStream &SFile );
	bool LOAD_0011 ( sc::BaseStream &SFile );
	bool LOAD_0012 ( sc::BaseStream &SFile );
	bool LOAD_0013 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
	bool LOAD_0014 ( sc::BaseStream &SFile );	//	EXP 보상 경험치 DWORD => LONGLONG
	bool LOAD_0015 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
    bool LOAD_0016 ( sc::BaseStream &SFile );	//	int m_nQuestArea 추가.
    bool LOAD_0017 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
	bool LOAD_0018 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
	bool LOAD_0019 ( sc::BaseStream &SFile );	//	출석부 관련 변수 추가
    bool LOAD_0020 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
    bool LOAD_0021 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
	bool LOAD_0022 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
	bool LOAD_0023 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
	bool LOAD_0024 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
	bool LOAD_0025 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
	bool LOAD_0026 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
	bool LOAD_0027 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
	bool LOAD_0028 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
	bool LOAD_0029 ( sc::BaseStream &SFile );	//	SITEMCUSTOM 변경됨
	// 위에 SITEMCUSTOM들 버전 하나씩 밀리는거 아닌가?;

public:
    bool LOAD(const std::string& FileName);
	bool SAVE ( const char *szFile );

	void SaveCsvHead( std::fstream &SFile, CString strFileName );
	void SaveCsv( std::fstream &SFile, CString strFileName );
	void SaveCsvInfoHead( std::fstream &SFile, CString strFileName );
	void SaveCsvBaseHead( std::fstream &SFile );
	void SaveCsvInfoData( std::fstream &SFile );
	void SaveCsvBaseData( std::fstream &SFile );
	void SaveCsvProgress( std::fstream &SFile );
	void SaveCsvProgressBaseHead( std::fstream &SFile );
	void SaveCsvProgressBaseData( std::fstream &SFile );

	void LoadCsv( CStringArray &StrArray, CStringArray &StrArrayProgress );
	void LoadCsvInfoDate( CStringArray &StrArray );
	void LoadCsvBaseData( CStringArray &StrArray, INT& iCsvCur );
	void LoadCsvProgress( CStringArray &StrArray, INT& iCsvCur );

	void ExportText( std::fstream &SFile );
	void ExportTextSubJect( std::fstream &SFile );
	void ExportTextProgress( std::fstream &SFile );
	void ImportText( sc::CStringFile& strFile );
	void ImportTextSubject( sc::CStringFile& strFile );
	void ImportTextProgress( sc::CStringFile& strFile );
};

enum EMQUESTPROG
{
	EMQP_REQREADING		= 0x0001,	// 퀘스트를 열어서 읽어보기를 요청.
};

struct GLQUESTPROG_STEP
{
	enum { VERSION = 0x0004, };

	//	이수 조건.

	bool		m_bLEVELUP_ALL;				//	모든 렙업 완료
	DWORD		m_dwNUM_LEVELUP;			//	올려야할 레벨

	bool		m_bCONFTING_ALL;			//	모든 대련 완료
	DWORD		m_dwNUM_CONFTING;			//	대련해야할 횟수

	bool		m_bITEMGRINDING_ALL;		//	연마해야할 아이템 모두 사용
	DWORD		m_dwNUM_ITEMGRINDING;		//	연마한 아이템 횟수

	bool		m_bITEMUSE_ALL;				//	사용해야할 아이템 모두 사용
	DWORD		m_dwNUM_ITEMUSE;			//	사용한 아이템 횟수

	bool		m_bMOBKILL_ALL;				//	죽여야할 Mob 모두 죽임.
	DWORD		m_dwNUM_MOBKILL;			//	죽인 Mob 숫자.
	
	bool		m_bQITEM;					//	필요 퀘스트 아이템 결과.
	bool		m_bTALK;					//	대화했는지 여부.	
	bool		m_bREACH_ZONE;				//	도달 위치에 도착 여부.

	bool		m_bLevel;

	bool		m_bNPCGUARD_DEATH;			//	보호해야할 NPC 사망 여부.
	
	bool		m_bDEFENSE_ZONE_BEGIN;		//	방어 위치 사수 시작 여부.
	bool		m_bDEFENSE_ZONE_COMPLETE;	//	방어 위치 사수 완료.
	float		m_fDEFENSE_ZONE_TIMER;		//	방어 위치 사수 경과시간.



public:
	GLQUESTPROG_STEP () :
		m_bTALK(true),
		m_bQITEM(true),
		
		m_bLEVELUP_ALL(true),
		m_dwNUM_LEVELUP(0),

		m_bCONFTING_ALL(true),
		m_dwNUM_CONFTING(EMCONFT_NONE),

		m_bITEMGRINDING_ALL(true),
		m_dwNUM_ITEMGRINDING(UINT_MAX),

		m_bITEMUSE_ALL(true),
		m_dwNUM_ITEMUSE(UINT_MAX),

		m_bMOBKILL_ALL(true),
		m_dwNUM_MOBKILL(UINT_MAX),
		m_bNPCGUARD_DEATH(false),

		m_bREACH_ZONE(true),

		m_bDEFENSE_ZONE_BEGIN(true),
		m_bDEFENSE_ZONE_COMPLETE(true),
		m_fDEFENSE_ZONE_TIMER(FLT_MAX),

		m_bLevel(true)
	{
	}

public:
	bool BEGIN_PROG ( const GLQUEST_STEP &sSTEP );
	void RESET ();

public:
	bool IsCOMPLETE ();
	std::string LogIsCOMPLETE();

public:
	GLQUESTPROG_STEP& operator= ( const GLQUESTPROG_STEP &rVALUE );

public:
	DWORD		GetDataSize();
	BOOL		GetData( se::SerializeWritableData& data );
	BOOL 		SET_BYBUFFER ( se::ByteStream &ByteStream ) const;
	BOOL 		GET_BYBUFFER ( se::ByteStream &ByteStream );

	BOOL LOAD_0001( se::ByteStream &ByteStream );
	BOOL LOAD_0002( se::ByteStream &ByteStream );
	BOOL LOAD_0003( se::ByteStream &ByteStream );	//	데이터 포맷 변경 진행
	BOOL LOAD_0004( se::ByteStream &ByteStream );	//	출석부 퀘스트 추가

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & m_bTALK;
		ar & m_bQITEM;

		ar & m_bLEVELUP_ALL;
		ar & m_dwNUM_LEVELUP;

		ar & m_bCONFTING_ALL;
		ar & m_dwNUM_CONFTING;

		ar & m_bITEMGRINDING_ALL;
		ar & m_dwNUM_ITEMGRINDING;

		ar & m_bITEMUSE_ALL;
		ar & m_dwNUM_ITEMUSE;

		ar & m_bMOBKILL_ALL;
		ar & m_dwNUM_MOBKILL;
		ar & m_bNPCGUARD_DEATH;

		ar & m_bREACH_ZONE;

		ar & m_bDEFENSE_ZONE_BEGIN;
		ar & m_bDEFENSE_ZONE_COMPLETE;
		ar & m_fDEFENSE_ZONE_TIMER;

		ar & m_bLevel;
	}
};	

class GLQUESTPROG
{
public:
	enum { VERSION = 0x0005 };

	typedef std::vector<DWORD>	DWARRAY;
	typedef DWARRAY::iterator	DWARRAY_ITER;

	typedef std::vector<LONGLONG>	TQUESTSTART;
	typedef TQUESTSTART::iterator	TQUESTSTART_ITER;

public:
	SNATIVEID	m_sNID;					//	고유 ID.
	DWORD		m_dwEDITVER;
	
	DWORD		m_dwFlags;				//	속성.
	
	bool		m_bCOMPLETE;			//	완료 여부.
	float		m_fLAPSTIME;			//	시작후 경과 시간.
	
	DWORD		m_dwCOUNT;				//	시도 횟수.

	__time64_t			m_tStartTime;		//	퀘스트 시작 시간
	__time64_t			m_tEndTime;		//	퀘스트 종료 시간

	// 기숙사 및 반복 퀘스트 문제로 인해서 접속 불가 문제 처리
	// 퀘스트 반복할때마다 시간정보 누적되지 않도록 처리. 차후에 리뉴얼 출석부 사용할 경우 고려 필요
	//
	// [shhan][2015.12.14] 출석부 리뉴얼이 새로 된다면 m_vectQuestStart 걸 사용할 시 메모리 단편화가 생길 수 있으므로 
	//						다른 방법으로 작업하길 바람.
	//
	TQUESTSTART			m_vectQuestStart;

	GLInventoryQuest	m_sINVENTORY;	//	퀘스트 전용 인벤.
	
	DWORD				m_dwSTEP;		//	현재 진행 퀘스트 STEP.
	GLQUESTPROG_STEP	m_sSTEP_PROG;	//	현재 진행 정보.

public:
	bool START ( GLQUEST* pQUEST, GLQUESTPROG *pOLD_PROG );
	bool DoPROGRESS( bool &_bModifyInven, GLQUEST_PROGRESS_MAP &stepMoveMap, std::vector< SINVENITEM >& vecDeleted, bool& bInserted );
	void DoGIVEUP ();
	bool DoCOMPLETE( std::vector< SINVENITEM >& vecDeleted, bool bTraceQuest = false );

	bool CheckCOMPLETE (bool bChangeSchool = false);
	std::string LogCheckCOMPLETE ();
	bool CheckTIMEOVER ();
	int ConvertQuestGroup(WORD wBeforeSchool, DWORD dwClass, WORD wAfterSchool, DWORD &dwID);
	void ChangeCompletedQuestID(DWORD dwQuestID);
	void ResetAndConvertProgressQuest(DWORD dwQuestID);


	bool IsLastStep();
public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );

public:
	void ResetReqREADING ()	{ m_dwFlags &= ~EMQP_REQREADING; }

public:
	void ReqREADING ()		{ m_dwFlags |= EMQP_REQREADING; }
	bool IsReqREADING ()	{ return (m_dwFlags&EMQP_REQREADING)!=NULL; }
	bool IsNonDIE ();

public:
	GLQUESTPROG& operator= ( const GLQUESTPROG &rVALUE );

public:
	DWORD		GetDataSize();
	BOOL		GetData( se::SerializeWritableData& data );
	BOOL		SET_BYBUFFER ( se::ByteStream &ByteStream ) const;
	BOOL		GET_BYBUFFER ( se::ByteStream &ByteStream );

public:
	BOOL LOAD_001 ( se::ByteStream &ByteStream );
	BOOL LOAD_002 ( se::ByteStream &ByteStream );
	BOOL LOAD_003 ( se::ByteStream &ByteStream );	//	데이터 포맷 변경 진행
	BOOL LOAD_004 ( se::ByteStream &ByteStream );	//	퀘스트 종료시간 추가
	BOOL LOAD_005 ( se::ByteStream &ByteStream );	//	데이터 변경은 없으나, 퀘스트 아이템이 아이템 테이블에 저장 및 로드하도록 변경됨

public:
	GLQUESTPROG () :
		m_sNID(false),
		m_dwEDITVER(1),
		m_dwFlags(NULL),
		m_bCOMPLETE(false),
		m_dwCOUNT(0),
		m_fLAPSTIME(0.0f),
		m_tStartTime(0),
		m_tEndTime(0),
		m_dwSTEP(0)
	{
	}

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & m_sNID;
		ar & m_dwEDITVER;

		ar & m_dwFlags;

		ar & m_bCOMPLETE;
		ar & m_fLAPSTIME;

		ar & m_dwCOUNT;

		ar & m_tStartTime;

		ar & m_sINVENTORY;

		ar & m_dwSTEP;
		ar & m_sSTEP_PROG;

		ar & m_tEndTime;

		ar & m_vectQuestStart;
	}
};

typedef std::vector<std::string> VEC_QUEST_AREA;
typedef VEC_QUEST_AREA::iterator VEC_QUEST_AREA_ITER;

namespace COMMENT
{
	extern std::string QUEST_PROGRESS[EMQP_SIZE];
    extern int QUEST_AREA_NUM;
    extern VEC_QUEST_AREA QUEST_AREA_NAME;
	extern std::string QUEST_ATTENDANCE_TYPE[EMATTENDANCE_TYPE_SIZE];
}

#endif // GLQUEST_H_
