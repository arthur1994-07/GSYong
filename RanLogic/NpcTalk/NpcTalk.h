//!	SNpcTalk
//!	���� �ۼ��� : ���⿱

#pragma	once

#include <boost/tr1/memory.hpp>
#include <string>
#include "../../SigmaCore/File/BaseStream.h"
#include "../Item/GLItem.h"

#define MAX_NEEDITEM_COUNT 5
#define MAX_QUEST_START	5
#define MAX_RANDOM_PAGE	5

namespace sc 
{
    class SerialFile;
    class Excel;
}

struct SNpcTalkCondition;
struct GLCHARLOGIC;

struct SNpcTalk
{
public:
	enum { VERSION = 0x000A, };

	typedef std::vector<SITEM_NUM>       VEC_ITEM_NUM;
	typedef VEC_ITEM_NUM::iterator       VEC_ITEM_NUM_ITER;
    typedef VEC_ITEM_NUM::const_iterator VEC_ITEM_NUM_CITER;

	enum EM_TALK
	{
		EM_DO_NOTHING = 0,
		EM_PAGE_MOVE,		//	��ȭ ����.
		EM_BASIC,			//	�⺻ ���. ( â��, ����, ġ��, ������ġ, stats����, �����۱�ȯ )
		EM_QUEST_START,		//	����Ʈ ����.
		EM_QUEST_STEP,		//	����Ʈ ����.

		EM_TALK_SIZE
	};

	enum EM_BASIC_SUB // ������ WidgetDef.lua ���� ���� ����
	{
        EM_BASIC_NULL = -1,

		EM_STORAGE = 0,		//	â��.
		EM_MARKET,			//	����.
		EM_CURE,			//	ġ��.
		EM_STARTPOINT,		//	������ġ ����.
		EM_CHAR_RESET,		//	stats, skill ����.
		EM_ITEM_TRADE,		//	item ��ȯ.
		EM_BUSSTATION,		//	���� ������.

		EM_CLUB_NEW,		//	Ŭ�� ����.
		EM_CLUB_UP,			//	Ŭ�� ��ũ ��.

		EM_CD_CERTIFY,		//	�õ� �����ϱ�.
		EM_COMMISSION,		//	��� ������ ����.
		EM_CLUB_STORAGE,	//	Ŭ�� â��.

		EM_ITEM_REBUILD,	//	������ ����.	// ITEMREBUILD_MARK
		EM_ODDEVEN,			//	Ȧ¦����
		//EM_TEXASHOLDEM,
		EM_RECOVERY_EXP,	//	����ġ ȸ��
		EM_RANDOM_PAGE,		//  ���Ǵ�ȭ
		EM_ITEMSEARCH_PAGE,	//  ������ �˻�
		EM_ATTENDANCE_BOOK,	//	�⼮��
		EM_ITEM_MIX,
		EM_HIRE_SUMMON,		//	��ȯ�� ���

		EM_BASIC_SIZE		//	������ �ٲ�� �ݵ�� NpcDialogueSet�κ� �ε�κ� �ٲ���� ��
	};

public:
	SNpcTalk();
	~SNpcTalk();
    SNpcTalk& operator= (const SNpcTalk& rNpcTalk);

public:
	static	std::string	strACTIONTYPE[EM_TALK_SIZE];
	static	std::string	szBASICDESC[EM_BASIC_SIZE];

	DWORD m_dwNID;     //! ��ȭ ID
	DWORD m_dwGLOB_ID; //! ��ü ��Ͽ��� ID.

	std::string m_strTalk; //! �⺻ ��ȭ
    std::tr1::shared_ptr<SNpcTalkCondition> m_spCondition; //! �ൿ����

	int	  m_nACTION;	 //! �׼�
	DWORD m_dwACTION_NO; //! �׼� ������ ����. (���� ��ȭ, �⺻ ��� ����)
	
	DWORD m_dwACTION_PARAM1; //! �Ķ��Ÿ 1. ������ ����ƮID	
	DWORD m_dwACTION_PARAM2; //! �Ķ��Ÿ 2. ������ ����ƮSTEP

	VEC_ITEM_NUM m_vecNeedItem;
	VEC_ITEM_NUM m_vecTradeItem;

	//! ������ ����Ʈ ID
	DWORD m_dwQuestStartID[MAX_QUEST_START];

	DWORD m_dwRandomPageID[MAX_RANDOM_PAGE];
	DWORD m_dwRandomTime;

	std::string m_strBusFile; //! ���������̼� ����

	SNATIVEID m_sHireSummon;	//! ��� ��ȯ�� ID;

public:	
	BOOL SetCondition(const SNpcTalkCondition& sCondition);
    BOOL SetCondition(std::tr1::shared_ptr<SNpcTalkCondition> spCondition);
	void ResetCondition();

	void SetGlobID(DWORD dwID);

	BOOL DoTEST(GLCHARLOGIC* pCHARLOGIC, DWORD PartyMemberNum, int iSvrHour, DWORD dwIndex = UINT_MAX);

	const char* GetTalk() const { return m_strTalk.c_str(); }
    const std::string& GetTalkStr() const { return m_strTalk; }
	std::tr1::shared_ptr<SNpcTalkCondition> GetCondition() const;

	bool IsUscCondition();
	void SetTalk(const std::string& strTalk);
	void Init();

    //! ������ ����ƮID
    DWORD QuestProgressID() const { return m_dwACTION_PARAM1; }
    
    //! ������ ����ƮSTEP
    DWORD QuestProgressStepID() const { return m_dwACTION_PARAM2; }

    //! ������ ����Ʈ ID
    DWORD QuestStartID(size_t Index) const
    {
        if (Index < MAX_QUEST_START)
            return m_dwQuestStartID[Index];
        else
            return UINT_MAX;
    }

    //! �׼�
    int GetAction() const { return m_nACTION; }
    void AddNeedItem(SNATIVEID ItemId, int ItemNum);
    void AddTradeItem(SNATIVEID ItemId, int ItemNum);

public:
    BOOL SAVE(sc::SerialFile& SFile);
    //BOOL SAVECSV(std::fstream& SFile);
    static void SaveExcelHead(sc::Excel& Xls);
    static void SaveExcelHeadNeedItem(sc::Excel& Xls);
    static void SaveExcelHeadTradeItem(sc::Excel& Xls);
    void SaveExcel(sc::Excel& Xls, const std::string& TalkName, int DlgNum, int CaseNum);
    void SaveExcelNeedItem(sc::Excel& Xls, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID);
    void SaveExcelTradeItem(sc::Excel& Xls, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID);

protected:

public:
    BOOL LOAD(sc::BaseStream& SFile);
    bool LoadExcel(sc::Excel& Xls, int SheetNum, int Row, int Version);

protected:
	BOOL LOAD_0001(sc::BaseStream& SFile);
	BOOL LOAD_0002(sc::BaseStream& SFile);
	BOOL LOAD_0003(sc::BaseStream& SFile);
	BOOL LOAD_0004(sc::BaseStream& SFile);
	BOOL LOAD_0005(sc::BaseStream& SFile);
	BOOL LOAD_0006(sc::BaseStream& SFile);
	BOOL LOAD_0007(sc::BaseStream& SFile);
	BOOL LOAD_0008(sc::BaseStream& SFile);
	BOOL LOAD_0009(sc::BaseStream& SFile);
	BOOL LOAD_000A(sc::BaseStream& SFile);
};
