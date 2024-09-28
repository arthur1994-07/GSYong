///////////////////////////////////////////////////////////////////////
//	CNpcDialogue
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//
//
///////////////////////////////////////////////////////////////////////

#pragma	once

#include <boost/tr1/memory.hpp>
#include <vector>
#include <map>

#include "../../SigmaCore/File/BaseStream.h"
#include "NpcTalk.h"

namespace sc {
    class SerialFile;
    class Excel;
}

//	CNpcDialogueSet <- CNpcDialogue <- CNpcDialogueCase <- CNpcTalkControl <- SNpcTalk
//
//
class CNpcDialogue;
class CNpcDialogueCase;
class CNpcTalkControl;
struct SNpcTalk;

class CNpcDialogueSet
{
public:
	CNpcDialogueSet();
	virtual	~CNpcDialogueSet();
    CNpcDialogueSet& operator= (CNpcDialogueSet& rvalue);

public:
	enum { VERSION = 0x0017 };	// 서버 인증 추가
	typedef std::map<DWORD, std::tr1::shared_ptr<CNpcDialogue> > DIALOGMAP;
	typedef DIALOGMAP::iterator		  DIALOGMAP_ITER;
    typedef DIALOGMAP::const_iterator DIALOGMAP_CITER;
    typedef DIALOGMAP::value_type     DIALOGMAP_VALUE;

	typedef std::vector<std::tr1::shared_ptr<SNpcTalk> > VECTALK;

    std::string m_TalkName;

protected:
	void RegistTalk(std::tr1::shared_ptr<SNpcTalk> spNpcTalk);

	void ResetAction();
	void ReportAction();
	void AssignTalkGlobID();	

	static CString m_strPath;

private:
	bool		m_bModify;
	DIALOGMAP	m_mapDialogue;

	bool		m_bBasicTalk[SNpcTalk::EM_BASIC_SIZE];

	VECTALK		m_vecTALK;

public:
	DWORD MakeDlgNID();
    std::string& GetDlgSetName() { return m_TalkName; }
    const std::string& GetDlgSetName() const { return m_TalkName; }
    void SetDlgSetName(const std::string& DlgSetName) { m_TalkName=DlgSetName; }

    BOOL AddDialogue(std::tr1::shared_ptr<CNpcDialogue> spDialogue);
	void DelDialogue(DWORD NativeID);

    bool AddDialogueCase(DWORD DlgId, std::tr1::shared_ptr<CNpcDialogueCase> spCase);
    
    bool AddDialogueCaseCondition(DWORD DlgId, int CaseNum, std::tr1::shared_ptr<SNpcTalkCondition> spCondition);
    bool AddDialogueCaseConditionCompleteQuest(DWORD DlgId, int CaseNum, SNATIVEID QuestId);
    bool AddDialogueCaseConditionHaveItem(DWORD DlgId, int CaseNum, SNATIVEID ItemId, int ItemNum);
    bool AddDialogueCaseConditionLearnSkill(DWORD DlgId, int CaseNum, SNATIVEID SkillId, int SkillLevel);
    bool AddDialogueCaseConditionDisQuest(DWORD DlgId, int CaseNum, SNATIVEID QuestId);
    bool AddDialogueCaseConditionDisSkill(DWORD DlgId, int CaseNum, SNATIVEID SkillId);
    bool AddDialogueCaseConditionSkillFact(DWORD DlgId, int CaseNum, SNATIVEID SkillId, int SkillLevel);

    bool AddDialogueCaseControl(DWORD DlgId, int CaseNum, std::tr1::shared_ptr<CNpcTalkControl> spControl);
    bool AddDialogueCaseControlTalk(DWORD DlgId, int CaseNum, std::tr1::shared_ptr<SNpcTalk> spTalk);
    bool AddDialogueCaseControlTalkNeedItem(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID ItemId, int ItemNum);
    bool AddDialogueCaseControlTalkTradeItem(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID ItemId, int ItemNum);

    bool AddDialogueCaseControlCondition(DWORD DlgId, int CaseNum, int TalkId, std::tr1::shared_ptr<SNpcTalkCondition> spCondition);
    bool AddDialogueCaseControlConditionCompleteQuest(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID QuestId);
    bool AddDialogueCaseControlConditionHaveItem(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID ItemId, int ItemNum);
    bool AddDialogueCaseControlConditionLearnSkill(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID SkillId, int SkillLevel);
    bool AddDialogueCaseControlConditionDisQuest(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID QuestId);
    bool AddDialogueCaseControlConditionDisSkill(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID SkillId);
    bool AddDialogueCaseControlConditionSkillFact(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID SkillId, int SkillLevel);

	std::tr1::shared_ptr<CNpcDialogue> GetDialogue(DWORD NativeID);
    std::tr1::shared_ptr<CNpcDialogue> GetDialogue2(int nDialogue);
	void GetDlgText(DWORD dwIndex, std::string& strName);

    std::tr1::shared_ptr<CNpcDialogueCase> GetCase(DWORD DlgId, int CaseNum);
    std::tr1::shared_ptr<SNpcTalkCondition> GetCondition(DWORD DlgId, int CaseNum);
    std::tr1::shared_ptr<SNpcTalkCondition> GetCondition(DWORD DlgId, int CaseNum, int TalkId);
    std::tr1::shared_ptr<CNpcTalkControl> GetControl(DWORD DlgId, int CaseNum);

	void RemoveAllDialogue();

	std::tr1::shared_ptr<SNpcTalk> GetTalk(DWORD dwTalkID) const;
    std::tr1::shared_ptr<SNpcTalk> GetTalk(DWORD DlgId, int CaseNum, int TalkId);
	DWORD GetTalkSize() const { return (DWORD) m_vecTALK.size(); }

	DIALOGMAP* GetDialogueMap();

	BOOL Save(const char* szFileName);
    BOOL Save(sc::SerialFile& SFile, CString& strTalkBuffer, CString& strAnswerBuffer);
    
    bool SaveExcel(sc::Excel& Xls);
    static void SaveExcelHead(sc::Excel& Xls);
    
    BOOL Load(const std::string& FileName);
    BOOL Load(sc::BaseStream& SFile);
    bool LoadExcel(sc::Excel& Xls, int SheetNum, int Row);
    void LoadExcelFinal();

    std::string ExportText( const std::string& strFileName );
    std::string ImportText( const std::string& strFileName );

private:    
    bool LoadExcel_0016(sc::Excel& Xls, int Sheet, int Row);

protected:
    BOOL LOAD_0001(sc::BaseStream& SFile);
    BOOL LOAD_0002(sc::BaseStream& SFile);
    BOOL LOAD_0003(sc::BaseStream& SFile);
    BOOL LOAD_0004(sc::BaseStream& SFile);
    BOOL LOAD_0005(sc::BaseStream& SFile);
    BOOL LOAD_0006(sc::BaseStream& SFile);
    BOOL LOAD_0007(sc::BaseStream& SFile);	// ITEMREBUILD_MARK
    BOOL LOAD_0008(sc::BaseStream& SFile);	// 홀짝게임
    BOOL LOAD_0009(sc::BaseStream& SFile);	// 경험치 회복
    BOOL LOAD_0010(sc::BaseStream& SFile);	// 임의대화
    BOOL LOAD_0011(sc::BaseStream& SFile);	// 아이템 검색 추가
    BOOL LOAD_0012(sc::BaseStream& SFile);	// 출석부 기능 추가
    BOOL LOAD_0013(sc::BaseStream& SFile);	// 아이템 조합 추가
    BOOL LOAD_0014(sc::BaseStream& SFile);	//	데이터 포맷 변경 진행
    BOOL LOAD_0015(sc::BaseStream& SFile);	//	bool형 변경
    BOOL LOAD_0016(sc::BaseStream& SFile);	// m_strFileName 저장
	BOOL LOAD_0017(sc::BaseStream& SFile);	// m_strFileName 저장

public:
	BOOL SimpleEdit ( CWnd* pWnd );

	void DoModified ()		{ m_bModify = true; }
	void ResetModified ()	{ m_bModify = false; }
	bool IsModify ()		{ return m_bModify; }
	bool IsBasicTalk(int nAction) const;

	static const char* GetPath ()			{ return m_strPath.GetString(); }
	static void SetPath ( char* szPath )	{ m_strPath = szPath; }

    const DIALOGMAP& GetDialogueSet() const { return m_mapDialogue; }
    const VECTALK& GetTalkVec() const { return m_vecTALK; }
};

typedef std::tr1::shared_ptr<CNpcDialogueSet> spNpcDialogSet;