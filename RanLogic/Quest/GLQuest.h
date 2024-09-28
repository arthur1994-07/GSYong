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
	EMQSCT_NPCTALK = 0,			// ��/�� ������;
	EMQSCT_MOBGEN_ITEM,			// �����Լ� ������ ������;
	EMQSCT_MOBKILL,				// �׿����� ��;
	EMQSCT_NPCQUARD,			// ��ȣ�ؾ���;
	EMQSCT_REACH_ZONE,			// ��� ��ġ ����;
	EMQSCT_GUARD_MAP_COMMENT,	// ��� ��ġ ���;
	EMQSCT_ITEM_USE,			// ����ؾ��� ������;
	EMQSCT_ITEM_GRINDING,		// �����ؾ��� ������;
	EMQSCT_LEVEL_UP_COMMENT,	// ������ �÷���;
};	  

enum EMQUESTJOIN
{
	EMQJ_NONE = 0,
	EMQJ_ATTENDANCE,
	EMQJ_JOIN,
};

enum EMQUESTFLAGS
{
	EMQF_REPEAT		= 0x0001,		//	�Ϸ�� ����Ʈ ���� ���� ����.
	EMQF_AGAIN		= 0x0002,		//	������ ����Ʈ ���� ���� ����.
	EMQF_ONLYONE	= 0x0004,		//	�� ����Ʈ�� �ٸ� ����Ʈ�� ���� ���� �Ұ���.
	EMQF_GIVEUP		= 0x0008,		//	����Ʈ ���� ���� ����.
	EMQF_DAILY		= 0x0010,		//	���� ����Ʈ �ΰ�?
	
	EMQF_PARTY		= 0x1000,		//	��Ƽ�� ���� ������ ����Ʈ.
	EMQF_PARTYQUEST	= 0x2000,		//	��Ƽ�� ���� ���� ����Ʈ.
};


enum EMQUESTPROGRESS
{
	EMQP_NON_EVENT = 0, // �̺�Ʈ ����
	EMQP_CHAR_DEAD,		// ĳ���� ���
	EMQP_START_MOVE,	// ������������ �̵�
    EMQP_SELECT_MOVE,   // ������ ��ġ�� �̵�
	EMQP_SIZE,
};

enum EMFAILTYPE
{
	EMFAIL_NONE		= 0x0000,		// Ÿ�Ծ���
	EMFAIL_TIMEOVER = 0x0001,		// Ÿ�ӿ����� ���н�
	EMFAIL_DIE      = 0x0002,		// �׾ ���н�
	EMFAIL_LEAVE    = 0x0004,		// Ư�� �ʿ��� ������ ���н�
};

enum EMGRINDINGTYPE
{
	EMGRINDING_NONE		= 0,	// Ư��������
	EMGRINDING_WEAPON	= 1,	// ����
	EMGRINDING_ARMOR	= 2,	// ��
	EMGRINDING_ALL		= 3,	// ���
};

enum EMATTENDANCE_TYPE
{
	EMATTENDANCE_TYPE_NONE =  0,
	EMATTENDANCE_TYPE_CONNECTION,	//Ư�� �ð����� ������ �����϶�
	EMATTENDANCE_TYPE_USER_KILL,	//Ư�� �������� Ư�� Ƚ����ŭ ������ óġ�϶�
	EMATTENDANCE_TYPE_MOB_KILL,	//Ư�� ���� Ư�� Ƚ���� óġ�϶�
	EMATTENDANCE_TYPE_LEVEL_UP,	//Ư�� ������ �缺�϶� 
	EMATTENDANCE_TYPE_ITEM_GET,	//Ư�� �������� �����϶�
	EMATTENDANCE_TYPE_ITEM_USE,	//Ư�� �������� Ư�� Ƚ���� ����϶�
	EMATTENDANCE_TYPE_MOVE_POS,	//Ư�� ��(Ȥ�� Ư�� ����)���� �̵��϶�
	EMATTENDANCE_TYPE_GRINDING_ITEM,	// �������� Ư�� Ƚ����ŭ �����϶�
	EMATTENDANCE_TYPE_SPARRING,	//�ٸ���Ƽ�� �п� ����� Ư�� Ƚ����ŭ �ǽ��϶�
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
	WORD wMinLevel; // ����Ʈ ����� ���� ����
	WORD wMaxLevel; // ����Ʈ ����� ���� ����
	DWORD dwDayofWeek; // ����Ʈ ���۽� ����.

	WORD wStartYear; // ����Ʈ ���۽� ���� ��
	WORD WStarMonth; // ����Ʈ ���۽� ���� ��
	WORD WstarDay; // ����Ʈ ���۽� ���� ��

	WORD wEndYear; // ����Ʈ ���۽� ���� ��
	WORD WEndMonth; // ����Ʈ ���۽� ���� ��
	WORD WEndDay; // ����Ʈ ���۽� ���� ��


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

	std::string		m_strTITLE;				//	���� �ܰ迡 ���� ����.
	std::string		m_strCOMMENT;			//	���� �ܰ迡 ���� ����.

	//	�̼� ���� #1.
	DWORD			m_dwNID_MOBKILL;		//	�׿����� Mob ID.
	DWORD			m_dwNUM_MOBKILL;		//	�׿����� Mob ����.
	std::string		m_strOBJ_MOBKILL;		//	�̼����� ����.
	//  �̼� ���� #1-1 �߰�
	bool			m_bIsPLAYERKILL;		// �÷��̾ ���̴� ����Ʈ�ΰ�


	//	�̼� ���� #2.
	GENMOBITEMARRAY	m_vecMOBGEN_QITEM;		//	MOB �߻� ����Ʈ ������.
	std::string		m_strOBJ_MOBGEN_QITEM;	//	�̼����� ����.

	//	�̼� ���� #3.
	DWORD			m_dwNID_NPCTALK;		//	��ȭ�� �ʿ��� NPC ID.
	std::string		m_strOBJ_NPCTALK;		//	�̼����� ����.

	//	�̼� ���� #4.
	SNATIVEID		m_sMAPID_REACH;			//	�����ؾ��� MAPID.
	WORD			m_wPOSX_REACH;			//	�����ؾ��� x��ǥ.	( �̴ϸ㿡�� ���� ��ǥ. )
	WORD			m_wPOSY_REACH;			//	�����ؾ��� y��ǥ.	( �̴ϸ㿡�� ���� ��ǥ. )
	WORD			m_wRADIUS_REACH;		//	���� �ݰ�.
	std::string		m_strOBJ_REACH_ZONE;	//	�̼� ���� ����.

	//	�̼� ���� #5.
	WORD			m_wLevel;				//	���� ����.


	//	�̼� ���� #6.
	DWORD			m_dwNID_NPCGUARD;		//	��ȣ�ؾ��� NPC.
	std::string		m_strOBJ_NPCGUARD;		//	�̼����� ����.

	//	�̼� ���� #7.
	SNATIVEID		m_sMAPID_DEFENSE;		//	����ؾ��� MAPID.
	WORD			m_wPOSX_DEFENSE;		//	����ؾ��� x��ǥ.	( �̴ϸ㿡�� ���� ��ǥ. )
	WORD			m_wPOSY_DEFENSE;		//	����ؾ��� y��ǥ.	( �̴ϸ㿡�� ���� ��ǥ. )
	WORD			m_wRADIUS_DEFENSE;		//	��� �ݰ�.
	float			m_fDEFENSE_TIME;		//	����ؾ��� �ð�.
	std::string		m_strOBJ_DEFENSE_ZONE;	//	�̼� ���� ����.

	//	�̼� ���� #8.
	DWORD			m_dwNID_ITEMUSE;		//	����ؾ��� ������ ID.
	DWORD			m_dwNUM_ITEMUSE;		//	����ؾ��� Ƚ��
	std::string		m_strOBJ_ITEMUSE;		//	�̼����� ����.

	// �̼� ���� # 9
	DWORD			m_dwNID_ITEMGRINDING;	//	�����ؾ��� ������ ID
	DWORD			m_dwNUM_ITEMGRINDING;	//	�����ؾ��� Ƚ��
	DWORD			m_dwItemType;			//	���� ������, ����, ��, ���
	bool			m_bConsecution;			//	���� ����
	std::string		m_strOBJ_ITEMGRINDING;  //	�̼����� ����.

	// �̼� ���� # 10
	DWORD			m_dwType_CONFTING;		//	ȥ�ڴ��,��Ƽ���
	bool			m_bSCHOOL;				//	�п����
	DWORD			m_dwNUM_CONFTING;		//	��� Ƚ��
	std::string		m_strOBJ_CONFTING;		//	�̼����� ����.

	//	�̼� ���� #11.
	DWORD			m_dwNUM_LEVELUP;		//	�������Ѿ��� ����
	std::string		m_strOBJ_LEVELUP;		//	�̼����� ����.


	//	�Ϸ�� ó��.
	INVENQARRAY			 m_vecGIFT_QITEM;		//	���� STEP �Ϸ�� ���� Quest Item.
	DWQARRAY			 m_vecRESET_QITEM;		//	���� �������� �Ѿ�� ���µ� Quest Item.
	
	GLQUEST_PROGRESS_MAP m_stepMoveMap;

	GLQUEST_STEP()
        : m_strTITLE("���� �ܰ迡 ���� ����")
        , m_strCOMMENT("���� �ܰ迡 ���� ����")
        , m_strOBJ_NPCTALK("��ȭ�ؾ� ��")
        , m_dwNID_NPCTALK(UINT_MAX)
        , m_strOBJ_MOBKILL("��ġ�ؾ� �� ����")
        , m_dwNID_MOBKILL(UINT_MAX)
        , m_dwNUM_MOBKILL(0)
		, m_bIsPLAYERKILL(false)
        , m_strOBJ_MOBGEN_QITEM("�����Լ� ������ ������")
        , m_strOBJ_NPCGUARD("��ȣ�ؾ� ��")
        , m_dwNID_NPCGUARD(UINT_MAX)
        , m_strOBJ_REACH_ZONE("��� ��ġ����")
        , m_sMAPID_REACH(false)
        , m_wPOSX_REACH(0)
        , m_wPOSY_REACH(0)
        , m_wRADIUS_REACH(60)
        , m_strOBJ_DEFENSE_ZONE("��� ��ġ���")
        , m_sMAPID_DEFENSE(false)
        , m_wPOSX_DEFENSE(0)
        , m_wPOSY_DEFENSE(0)
        , m_wRADIUS_DEFENSE(0)
        , m_fDEFENSE_TIME(0.0f)
        , m_wLevel(USHRT_MAX)
		, m_dwNID_ITEMUSE(UINT_MAX)
		, m_dwNUM_ITEMUSE(0)
		, m_strOBJ_ITEMUSE("����ؾ��� ������")
		, m_dwNID_ITEMGRINDING(UINT_MAX)
		, m_dwNUM_ITEMGRINDING(0)
		, m_dwItemType(0)
		, m_bConsecution(false)
		, m_strOBJ_ITEMGRINDING("�����ؾ��� ������")
		, m_dwType_CONFTING(EMCONFT_NONE)
		, m_bSCHOOL(false)
		, m_dwNUM_CONFTING(0)
		, m_strOBJ_CONFTING("����� �ǽ��϶�")
		, m_dwNUM_LEVELUP(0)
		, m_strOBJ_LEVELUP("������ �÷���")
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
	bool LOAD_0012 ( sc::BaseStream &SFile );	//	������ ���� ���� ����
	bool LOAD_0013 ( sc::BaseStream &SFile );	//	������ ���� ���� ����
    bool LOAD_0014 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���.
	bool LOAD_0015 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���.
	bool LOAD_0016 ( sc::BaseStream &SFile );	// �⼮�� ���� ����Ʈ �߰�
    bool LOAD_0017 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���.
    bool LOAD_0018 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���.
	bool LOAD_0019 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���.
	bool LOAD_0020 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���.
	bool LOAD_0021 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���.
	bool LOAD_0022 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���.
	bool LOAD_0023 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���;
	bool LOAD_0024 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���;
	bool LOAD_0025 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���;
	bool LOAD_0026 ( sc::BaseStream &SFile );	// ������ �����Ͱ� ����Ǿ���;
	// ���� SITEMCUSTOM�� ���� �ϳ��� �и��°� �ƴѰ�?;

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
		wSCHOOL(10),	//MAX_SCHOOL 10 - MAX_SCHOOL�� �� ��쿡 ��� ������ ���� include ������ ������ ������ �߻�

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
	bool LOAD_003( sc::BaseStream &SFile );	//	������ ���� ���� ����
	bool LOAD_004( sc::BaseStream &SFile );	//	�⼮�� ���� ����Ʈ �߰�

	bool LOAD ( sc::BaseStream &SFile );
	bool SAVE ( sc::SerialFile &SFile );
};


struct GLQUEST_PROGRESS 
{
	DWORD dwStartProgress;  // ���۽� �̺�Ʈ
	DWORD dwEndProgress;	  // ����� �̺�Ʈ
	DWORD dwFailProgress;	  // ���н� �̺�Ʈ
		
	DWORD			dwfailType;		  // ������ ����

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
	SNATIVEID		m_sNID;					//	���� ID.
	DWORD			m_dwEDITVER;			//	���� ����.
											//	( ���� ����Ʈ ������ ������ ������ Ʋ������ ���� �������� 
											//	�÷����� ����� �����ϱ� �Ұ����� ���. )

	std::string		m_strTITLE;				//	����Ʈ Ÿ��Ʋ.
	std::string		m_strCOMMENT;			//	����Ʈ ����.
	
	DWORD			m_dwFlags;				//	�Ӽ�.

    DWORD           m_dwQuestArea;          // ����Ʈ ����.

	GLQUEST_START	m_sSTARTOPT;			//	�ڵ� ���� ����.

	GLQUEST_ATTENDANCE_OPTION m_sAttendanceOption; // �⼮�� ����Ʈ �ɼ�

	//	���� �ο� ����.
	DWORD			m_dwBeginPartyMemNum;	//	����Ʈ�� �ޱ� ���� �ּ� ��Ƽ �ο�.
	DWORD			m_dwBeginMoney;			//	����Ʈ �ޱ� ���� ���.

	//	���� ����.
	DWORD			m_dwLimitTime;			//	���� �ð�.	( ���� �ð��� �д���. )
	bool			m_bTimeLimiorKeep;		//	����, ���� �ð� ex) ���ѽð� 10��, 10�е��� ��������
	bool			m_bPastOnlyOnOline ;	//	Ŭ���̾�Ʈ �����Ŀ��� �ð� �帧
	bool			m_bNonDie;				//	���� ���� ����ϸ� �ȵ�.
	DWORD			m_dwLimitPartyMemNum;	//	���� ��Ƽ �ο�.

	VECQSTEP		m_vecProgStep;			//	��ü ���� ���� ����.

	//	�Ϸ�� ����.
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
	
	//	����Ʈ ���� �߿� ����Ǵ� �̺�Ʈ
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
	bool IsREPEAT ()		{ return (m_dwFlags&EMQF_REPEAT)!=NULL; }		//	������ ���� ����.
	bool IsAGAIN ()			{ return (m_dwFlags&EMQF_AGAIN)!=NULL; }		//	(�����Ѱ�) �ٽ� ���� ����.
	bool IsPARTY ()			{ return (m_dwFlags&EMQF_PARTY)!=NULL; }		//	��Ƽ�϶� ���� ����.
	bool IsPARTYQUEST ()	{ return (m_dwFlags&EMQF_PARTYQUEST)!=NULL; }	//	��Ƽ�� ���� ���� ����Ʈ.

	bool IsONLYONE ()		{ return (m_dwFlags&EMQF_ONLYONE)!=NULL; }		//	���ÿ� ����Ұ�?
	bool IsGIVEUP ()		{ return (m_dwFlags&EMQF_GIVEUP)!=NULL; }		//	���� ����
	
	bool IsLIMITTIME ()		{ return m_dwLimitTime!=0; }
	bool IsNonDIE ()		{ return m_bNonDie; }

	bool IsDaily()			{ return (m_dwFlags&EMQF_DAILY)!=NULL;	}		//	���� ����Ʈ�ΰ�?

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
	bool LOAD_0013 ( sc::BaseStream &SFile );	//	������ ���� ���� ����
	bool LOAD_0014 ( sc::BaseStream &SFile );	//	EXP ���� ����ġ DWORD => LONGLONG
	bool LOAD_0015 ( sc::BaseStream &SFile );	//	������ ���� ���� ����
    bool LOAD_0016 ( sc::BaseStream &SFile );	//	int m_nQuestArea �߰�.
    bool LOAD_0017 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
	bool LOAD_0018 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
	bool LOAD_0019 ( sc::BaseStream &SFile );	//	�⼮�� ���� ���� �߰�
    bool LOAD_0020 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
    bool LOAD_0021 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
	bool LOAD_0022 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
	bool LOAD_0023 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
	bool LOAD_0024 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
	bool LOAD_0025 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
	bool LOAD_0026 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
	bool LOAD_0027 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
	bool LOAD_0028 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
	bool LOAD_0029 ( sc::BaseStream &SFile );	//	SITEMCUSTOM �����
	// ���� SITEMCUSTOM�� ���� �ϳ��� �и��°� �ƴѰ�?;

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
	EMQP_REQREADING		= 0x0001,	// ����Ʈ�� ��� �о�⸦ ��û.
};

struct GLQUESTPROG_STEP
{
	enum { VERSION = 0x0004, };

	//	�̼� ����.

	bool		m_bLEVELUP_ALL;				//	��� ���� �Ϸ�
	DWORD		m_dwNUM_LEVELUP;			//	�÷����� ����

	bool		m_bCONFTING_ALL;			//	��� ��� �Ϸ�
	DWORD		m_dwNUM_CONFTING;			//	����ؾ��� Ƚ��

	bool		m_bITEMGRINDING_ALL;		//	�����ؾ��� ������ ��� ���
	DWORD		m_dwNUM_ITEMGRINDING;		//	������ ������ Ƚ��

	bool		m_bITEMUSE_ALL;				//	����ؾ��� ������ ��� ���
	DWORD		m_dwNUM_ITEMUSE;			//	����� ������ Ƚ��

	bool		m_bMOBKILL_ALL;				//	�׿����� Mob ��� ����.
	DWORD		m_dwNUM_MOBKILL;			//	���� Mob ����.
	
	bool		m_bQITEM;					//	�ʿ� ����Ʈ ������ ���.
	bool		m_bTALK;					//	��ȭ�ߴ��� ����.	
	bool		m_bREACH_ZONE;				//	���� ��ġ�� ���� ����.

	bool		m_bLevel;

	bool		m_bNPCGUARD_DEATH;			//	��ȣ�ؾ��� NPC ��� ����.
	
	bool		m_bDEFENSE_ZONE_BEGIN;		//	��� ��ġ ��� ���� ����.
	bool		m_bDEFENSE_ZONE_COMPLETE;	//	��� ��ġ ��� �Ϸ�.
	float		m_fDEFENSE_ZONE_TIMER;		//	��� ��ġ ��� ����ð�.



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
	BOOL LOAD_0003( se::ByteStream &ByteStream );	//	������ ���� ���� ����
	BOOL LOAD_0004( se::ByteStream &ByteStream );	//	�⼮�� ����Ʈ �߰�

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
	SNATIVEID	m_sNID;					//	���� ID.
	DWORD		m_dwEDITVER;
	
	DWORD		m_dwFlags;				//	�Ӽ�.
	
	bool		m_bCOMPLETE;			//	�Ϸ� ����.
	float		m_fLAPSTIME;			//	������ ��� �ð�.
	
	DWORD		m_dwCOUNT;				//	�õ� Ƚ��.

	__time64_t			m_tStartTime;		//	����Ʈ ���� �ð�
	__time64_t			m_tEndTime;		//	����Ʈ ���� �ð�

	// ����� �� �ݺ� ����Ʈ ������ ���ؼ� ���� �Ұ� ���� ó��
	// ����Ʈ �ݺ��Ҷ����� �ð����� �������� �ʵ��� ó��. ���Ŀ� ������ �⼮�� ����� ��� ��� �ʿ�
	//
	// [shhan][2015.12.14] �⼮�� �������� ���� �ȴٸ� m_vectQuestStart �� ����� �� �޸� ����ȭ�� ���� �� �����Ƿ� 
	//						�ٸ� ������� �۾��ϱ� �ٶ�.
	//
	TQUESTSTART			m_vectQuestStart;

	GLInventoryQuest	m_sINVENTORY;	//	����Ʈ ���� �κ�.
	
	DWORD				m_dwSTEP;		//	���� ���� ����Ʈ STEP.
	GLQUESTPROG_STEP	m_sSTEP_PROG;	//	���� ���� ����.

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
	BOOL LOAD_003 ( se::ByteStream &ByteStream );	//	������ ���� ���� ����
	BOOL LOAD_004 ( se::ByteStream &ByteStream );	//	����Ʈ ����ð� �߰�
	BOOL LOAD_005 ( se::ByteStream &ByteStream );	//	������ ������ ������, ����Ʈ �������� ������ ���̺� ���� �� �ε��ϵ��� �����

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
