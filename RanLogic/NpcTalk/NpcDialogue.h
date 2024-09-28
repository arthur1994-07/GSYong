#pragma	once

#include <vector>
#include <boost/tr1/memory.hpp>
#include "../../SigmaCore/File/BaseStream.h"

#include "./NpcDialogueCase.h"
#include "./NpcTalkCondition.h"

///////////////////////////////////////////////////////////////////////
//	CNpcDialogue
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
///////////////////////////////////////////////////////////////////////
class CNpcDialogue
{
public:
	enum { VERSION=0x0003, };
    typedef std::vector<std::tr1::shared_ptr<CNpcDialogueCase> > DLGCASE;
	typedef DLGCASE::iterator		DLGCASE_ITER;
	typedef DLGCASE::const_iterator	DLGCASE_CITER;

public:
	CNpcDialogue();
    virtual	~CNpcDialogue();
    CNpcDialogue& operator= (const CNpcDialogue& rNpcDialog);

private:
	DWORD			m_dwNID;

public:
	DLGCASE	m_vecCase;

public:
	void Reset();

	DWORD FindConditionSatisfied(GLCHARLOGIC* pCHAR, DWORD PartyNum, int iSvrHour);
	DWORD FindNonCondition();

	DWORD AddCase(const CNpcDialogueCase& sCase);
    DWORD AddCase(std::tr1::shared_ptr<CNpcDialogueCase> spCase);
    BOOL DelCase(std::tr1::shared_ptr<CNpcDialogueCase> spCase);
	BOOL DelCase(DWORD dwIndex);

	DWORD ToUp(DWORD dwNID);
	DWORD ToDown(DWORD dwNID);

	const DLGCASE& GetDlgCaseVec() const;
	std::tr1::shared_ptr<CNpcDialogueCase> GetDlgCase(DWORD dwIndex);

	void SetDlgNID(const DWORD NativeID);
	DWORD GetDlgNID() const;

public:
    BOOL LoadFile(sc::BaseStream& SFile);
    bool LoadExcel(sc::Excel& Xls, int SheetNum, int Row);
    
protected:
    BOOL LOAD_0001(sc::BaseStream& SFile);
	BOOL LOAD_0002(sc::BaseStream& SFile);
	BOOL LOAD_0003(sc::BaseStream& SFile);	//	데이터 포맷 변경 진행

public:
	BOOL SaveFile(sc::SerialFile& SFile, CString& strTalkBuffer, CString& strAnswerBuffer);
    BOOL SaveCSVFile(std::fstream &SFile);
    bool SaveExcel(sc::Excel& Xls, const std::string& FileName);
    static void SaveExcelHead(sc::Excel& Xls);
};

