///////////////////////////////////////////////////////////////////////
//	SNpcTalkCondition
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//
//
///////////////////////////////////////////////////////////////////////

#pragma	once

#include <vector>
#include <string>

#include "../Character/GLCharDefine.h"
#include "../Item/GLItem.h"
#include "./NpcTalkConditionDefine.h"

namespace sc {
    class BaseStream;
    class SerialFile;
    class Excel;
}

struct GLCHARLOGIC;

//	�������� x��. 20 < x
enum EM_CONDITIONSIGN
{
	CONDITIONSIGN_NONE	=	0,	//	EMPTY
	CONDITIONSIGN_SAME,			//	=
	CONDITIONSIGN_HIGHER,		//	<
	CONDITIONSIGN_LOWER,		//	>
	CONDITIONSIGN_SAMEHIGHER,	//	<=
	CONDITIONSIGN_SAMELOWER,	//	>=

    CONDITIONSIGN_SIZE
};
extern std::string strCONDITIONSIGN[CONDITIONSIGN_SIZE];
void SaveEmConditionSignExcel(sc::Excel& Xls, int SheetNum, const std::string& SheetName);


template<class TYPE>
BOOL NumericCompare ( TYPE A, TYPE B, EM_CONDITIONSIGN sign )
{
	switch ( sign )
	{
	case CONDITIONSIGN_NONE:		return TRUE;

	case CONDITIONSIGN_SAME:		return A == B;
	case CONDITIONSIGN_HIGHER:		return A < B;
	case CONDITIONSIGN_LOWER:		return A > B;
	case CONDITIONSIGN_SAMEHIGHER:	return A <= B;
	case CONDITIONSIGN_SAMELOWER:	return A >= B;
	};

	return FALSE;
}

struct SSkillCondition
{
	DWORD dwNID;
	WORD  wLevel;

	SSkillCondition()
        : dwNID(UINT_MAX)
        , wLevel(0xFFFF)
	{
	}

    SSkillCondition(SNATIVEID SkillId, WORD SkillLevel)
        : dwNID(SkillId.Id())
        , wLevel(SkillLevel)
    {
    }

	bool operator < ( const SSkillCondition& rvalue ) const
	{
		return dwNID < rvalue.dwNID;
	}

	bool operator == ( const SSkillCondition& rvalue ) const
	{
		return dwNID == rvalue.dwNID;
	}

    DWORD SkillId() const { return dwNID; }
    WORD SkillLevel() const { return wLevel; }
};

struct SNpcTalkCondition
{
	enum { VERSION = 0x0010, };

	typedef std::vector<SITEM_NUM>			HAVEITEM;
	typedef HAVEITEM::iterator				HAVEITEM_ITER;

	typedef std::vector<DWORD>				PROCQUEST;
	typedef PROCQUEST::iterator				PROCQUEST_ITER;

	typedef std::vector<DWORD>				PROCSKILL;
	typedef PROCSKILL::iterator				PROCSKILL_ITER;

	typedef std::vector<SSkillCondition>	LEARNSKILL;
	typedef LEARNSKILL::iterator			LEARNSKILL_ITER;

    typedef std::vector<SSkillCondition>	SKILLFACT;
    typedef SKILLFACT::iterator			    SKILLFACT_ITER;

public:
    SNpcTalkCondition();
    SNpcTalkCondition& operator= (const SNpcTalkCondition& rNpcTalkCondition);

public:
	bool				m_bUse;				//	��� ����

	EM_CONDITIONSIGN	m_signLevel;		//	���� ����.
	WORD				m_wLevel;

	DWORD				m_dwClass;			//	Ŭ���� ����.
	WORD				m_wSchool;			//	�Ҽ��б�.

	EM_CONDITIONSIGN	m_signElemental;	//	ELEMENTAL
	int					m_nElemental;

	EM_CONDITIONSIGN	m_signActionPoint;	//	��Ȱ����.
	int					m_nActionPoint;

	BOOL				m_bTime;			//	�ð� ����.
	DWORD				m_dwTimeBegin;
	DWORD				m_dwTimeEnd;

	DWORD				m_dwMONEY;			//	�ʿ� ���� �ݾ�.
	DWORD				m_dwPARTYMEN_NUM;	//	�ʿ� ��Ƽ �ο�.

	DWORD				m_dwQUEST_NID;		//	�������� ����Ʈ ID.
	DWORD				m_dwQUEST_STEP;		//	�������� ����Ʈ STEP.

	PROCQUEST			m_vecCompleteQuest;	//	�Ϸ��ؾ߸� �ϴ� ����Ʈ.
	HAVEITEM			m_vecHaveItem;		//	���� ������.
	LEARNSKILL			m_vecLearnSkill;	//	���� ��ų.
    SKILLFACT           m_vecSkillFact;     //  �ʿ��� ����

    PROCQUEST			m_vecDisQuest;		//	�� ����Ʈ�� �ޱ� ���ؼ��� �������� �ʾƾ��� ����Ʈ.
	PROCSKILL			m_vecDisSkill;		//	�� ����Ʈ�� �ޱ� ���ؼ��� �������� �ʾƾ��� ��ų.

public:
	void Reset ();	
    //! �Ϸ��ؾ߸� �ϴ� ����Ʈ
    const PROCQUEST& GetCompleteQuest() const { return m_vecCompleteQuest; }
    void AddCompleteQuest(SNATIVEID QuestID);

    //! �� ����Ʈ�� �ޱ� ���ؼ��� �������� �ʾƾ��� ����Ʈ.
    const PROCQUEST& GetDisQuest() const { return m_vecDisQuest; }
    void AddDisQuest(SNATIVEID QuestID);

    //! ���� ������
    const HAVEITEM& GetHaveItem() const { return m_vecHaveItem; }
    void AddHaveItem(SNATIVEID ItemID, int ItemNum);

    //! ���� ��ų
    const LEARNSKILL& GetLearnSkill() const { return m_vecLearnSkill; }
    void AddLearnSkill(SNATIVEID SkillID, int SkillLevel);

    //! �ʿ��� ����
    const SKILLFACT& GetSkillFact() const { return m_vecSkillFact; }
    void AddSkillFact(SNATIVEID SkillID, int SkillLevel);

    //! �� ����Ʈ�� �ޱ� ���ؼ��� �������� �ʾƾ��� ��ų
    const PROCSKILL& GetDisSkill() const { return m_vecDisSkill; }
    void AddDisSkill(SNATIVEID SkillID);

public:
	bool IsUse()				{ return m_bUse; }
	BOOL IsTERM_LEVEL ()		{ return m_signLevel!=CONDITIONSIGN_NONE; }
	BOOL IsTERM_CLASS ()		{ return m_dwClass!=GLCC_NONE; }
	BOOL IsTERM_ELEMENTAL ()	{ return m_signElemental!=CONDITIONSIGN_NONE; }
	BOOL IsTERM_ACTIONP ()		{ return m_signActionPoint!=CONDITIONSIGN_NONE; }
	BOOL IsTERM_TIME ()			{ return m_bTime; }

	BOOL IsTERM_QUEST ()		{ return m_dwQUEST_NID!=UINT_MAX; }

	BOOL IsTERM_ITEM ()			{ return !m_vecHaveItem.empty(); }
	BOOL IsTERM_SKILL ()		{ return !m_vecLearnSkill.empty(); }
    BOOL IsTERM_SKILLFACT ()    { return !m_vecSkillFact.empty(); }

public:
	BOOL DoTEST(GLCHARLOGIC* pCHARLOGIC, DWORD PartyMemberNum, int iSvrHour);
	BOOL EmptyCondition();

	std::string GetBoolString(BOOL bVALUE);
	void GetConditionText(std::string& strName);

public:
    BOOL LOAD(sc::BaseStream& SFile);
    bool LoadExcel(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, int Row, int Version);
    //BOOL LOADCSVFILE( CStringArray &StrArray, int iReadStart, int iLineNumber );

protected:
	BOOL LOAD_0001(sc::BaseStream& SFile);
	BOOL LOAD_0002(sc::BaseStream& SFile);
	BOOL LOAD_0003(sc::BaseStream& SFile);
	BOOL LOAD_0004(sc::BaseStream& SFile);
	BOOL LOAD_0005(sc::BaseStream& SFile);
	BOOL LOAD_0006(sc::BaseStream& SFile);
	BOOL LOAD_0007(sc::BaseStream& SFile);	//	������ ���� ���� ����
	BOOL LOAD_0008(sc::BaseStream& SFile);	//	��뿩�� �ɼ� �߰�
    BOOL LOAD_0009(sc::BaseStream& SFile);	//	�ʿ� ������ ��� ����
    BOOL LOAD_0010(sc::BaseStream& SFile);	//	�ʿ��� ���� �߰�
	
public:
    BOOL SAVE(sc::SerialFile& SFile);
	//BOOL SAVECSVFILE(std::string& strLine);
    
    void SaveExcel(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID);
    static void SaveExcelHead(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName);

    static void SaveExcelHeadCompleteQuest(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName);
    void SaveExcelCompleteQuest(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID);

    static void SaveExcelHeadHaveItem(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName);
    void SaveExcelHaveItem(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID);

    static void SaveExcelHeadLearnSkill(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName);
    void SaveExcelLearnSkill(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID);

    static void SaveExcelHeadDisQuest(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName);
    void SaveExcelDisQuest(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID);

    static void SaveExcelHeadDisSkill(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName);
    void SaveExcelDisSkill(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID);

    static void SaveExcelHeadSkillFact(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName);
    void SaveExcelSkillFact(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID);

public:
	BOOL ADDHAVEITEM (DWORD dwNID, DWORD dwNum);
	BOOL DELHAVEITEM(DWORD dwNID);

	BOOL ADDLEARNSKILL(const SSkillCondition& sSkillCondition);
	BOOL DELLEARNSKILL(DWORD NativeID);

	BOOL ADDDISSKILL(DWORD dwNID);
	BOOL DELDISSKILL(DWORD dwNID);

	BOOL ADDDISQUEST(DWORD dwNID);
	BOOL DELDISQUEST(DWORD dwNID);

	BOOL ADDCOMQUEST(DWORD dwNID);
	BOOL DELCOMQUEST(DWORD dwNID);

    BOOL ADDSKILLFACT(const SSkillCondition& sSkillCondition);
    BOOL DELSKILLFACT(DWORD dwNID);

    //! �������� ����Ʈ ID
    DWORD ProgressQuestId() const { return m_dwQUEST_NID; }
    
    //! �������� ����Ʈ STEP
    DWORD ProgressQuestStep() const { return m_dwQUEST_STEP; }

public:
	void RESET_QUEST ()
	{
		m_dwQUEST_NID = UINT_MAX;
		m_dwQUEST_STEP = 0;
	}
};


class HAVE_ITEM_CMP
{
public:
	bool operator() ( const SITEM_NUM &lvalue, const SITEM_NUM &rvalue )
	{
		if ( lvalue.sNativeID.wMainID < rvalue.sNativeID.wMainID )
			return true;
		else if ( lvalue.sNativeID.wMainID == rvalue.sNativeID.wMainID &&
				  lvalue.sNativeID.wSubID < rvalue.sNativeID.wSubID ) 
			return true;
		
		return false;
	}
};


