//!	SNpcTalk
//!	최초 작성자 : 성기엽

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
		EM_PAGE_MOVE,		//	대화 진행.
		EM_BASIC,			//	기본 기능. ( 창고, 상점, 치료, 시작위치, stats리셋, 아이템교환 )
		EM_QUEST_START,		//	퀘스트 시작.
		EM_QUEST_STEP,		//	퀘스트 진행.

		EM_TALK_SIZE
	};

	enum EM_BASIC_SUB // 수정시 WidgetDef.lua 파일 같이 수정
	{
        EM_BASIC_NULL = -1,

		EM_STORAGE = 0,		//	창고.
		EM_MARKET,			//	상점.
		EM_CURE,			//	치료.
		EM_STARTPOINT,		//	시작위치 지정.
		EM_CHAR_RESET,		//	stats, skill 리셋.
		EM_ITEM_TRADE,		//	item 교환.
		EM_BUSSTATION,		//	버스 정류장.

		EM_CLUB_NEW,		//	클럽 생성.
		EM_CLUB_UP,			//	클럽 랭크 업.

		EM_CD_CERTIFY,		//	시디 인증하기.
		EM_COMMISSION,		//	상업 수수료 설정.
		EM_CLUB_STORAGE,	//	클럽 창고.

		EM_ITEM_REBUILD,	//	아이템 개조.	// ITEMREBUILD_MARK
		EM_ODDEVEN,			//	홀짝게임
		//EM_TEXASHOLDEM,
		EM_RECOVERY_EXP,	//	경험치 회복
		EM_RANDOM_PAGE,		//  임의대화
		EM_ITEMSEARCH_PAGE,	//  아이템 검색
		EM_ATTENDANCE_BOOK,	//	출석부
		EM_ITEM_MIX,
		EM_HIRE_SUMMON,		//	소환수 고용

		EM_BASIC_SIZE		//	사이즈 바뀌면 반드시 NpcDialogueSet부분 로드부분 바꿔줘야 함
	};

public:
	SNpcTalk();
	~SNpcTalk();
    SNpcTalk& operator= (const SNpcTalk& rNpcTalk);

public:
	static	std::string	strACTIONTYPE[EM_TALK_SIZE];
	static	std::string	szBASICDESC[EM_BASIC_SIZE];

	DWORD m_dwNID;     //! 대화 ID
	DWORD m_dwGLOB_ID; //! 전체 목록에서 ID.

	std::string m_strTalk; //! 기본 대화
    std::tr1::shared_ptr<SNpcTalkCondition> m_spCondition; //! 행동조건

	int	  m_nACTION;	 //! 액션
	DWORD m_dwACTION_NO; //! 액션 행위시 선택. (다음 대화, 기본 기능 종류)
	
	DWORD m_dwACTION_PARAM1; //! 파라메타 1. 진행할 퀘스트ID	
	DWORD m_dwACTION_PARAM2; //! 파라메타 2. 진행할 퀘스트STEP

	VEC_ITEM_NUM m_vecNeedItem;
	VEC_ITEM_NUM m_vecTradeItem;

	//! 시작할 퀘스트 ID
	DWORD m_dwQuestStartID[MAX_QUEST_START];

	DWORD m_dwRandomPageID[MAX_RANDOM_PAGE];
	DWORD m_dwRandomTime;

	std::string m_strBusFile; //! 버스스테이션 파일

	SNATIVEID m_sHireSummon;	//! 고용 소환수 ID;

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

    //! 진행할 퀘스트ID
    DWORD QuestProgressID() const { return m_dwACTION_PARAM1; }
    
    //! 진행할 퀘스트STEP
    DWORD QuestProgressStepID() const { return m_dwACTION_PARAM2; }

    //! 시작할 퀘스트 ID
    DWORD QuestStartID(size_t Index) const
    {
        if (Index < MAX_QUEST_START)
            return m_dwQuestStartID[Index];
        else
            return UINT_MAX;
    }

    //! 액션
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
