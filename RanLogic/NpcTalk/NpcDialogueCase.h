#pragma	once

#include <boost/tr1/memory.hpp>
#include <string>

#include "../../SigmaCore/File/BaseStream.h"

class CNpcTalkControl;
struct SNpcTalkCondition;

namespace sc {
	class SerialFile;
    class Excel;
}

///////////////////////////////////////////////////////////////////////
//	최초 작성자 : 성기엽
class CNpcDialogueCase
{
public:
	enum { VERSION = 0x0003, };

public:
    CNpcDialogueCase();
    ~CNpcDialogueCase();
    CNpcDialogueCase& operator= (const CNpcDialogueCase& rNpcDialogCase);	

private:
	std::string m_strBasicTalk; //! 기본대화
    std::tr1::shared_ptr<SNpcTalkCondition> m_spCondition; //! 행동조건
	std::tr1::shared_ptr<CNpcTalkControl> m_spTalkControl; //! 대화분기

public:
	void Reset();

	BOOL SetCondition(const SNpcTalkCondition& sCondition);
    BOOL SetCondition(std::tr1::shared_ptr<SNpcTalkCondition> spCondition);
	void ResetCondition();

	void SetBasicTalk(const std::string& strTalk);

	BOOL SetTalkControl(const CNpcTalkControl& sTalkControl);
    BOOL SetTalkControl(std::tr1::shared_ptr<CNpcTalkControl> spTalkControl);
	void ResetTalkControl();

	std::tr1::shared_ptr<SNpcTalkCondition> GetCondition() const;
	const std::string GetBasicTalk() const;
	std::tr1::shared_ptr<CNpcTalkControl> GetTalkControl() const;

	DWORD GetTalkNum () const;

public:
    BOOL LoadFile(sc::BaseStream& SFile);
    bool LoadExcel(sc::Excel& Xls, int SheetNum, int Row);
    
protected:    
	BOOL LOAD_0001(sc::BaseStream& SFile);
	BOOL LOAD_0002(sc::BaseStream& SFile);
	BOOL LOAD_0003(sc::BaseStream& SFile);	//	데이터 포맷 변경 진행

public:
	BOOL SaveFile(sc::SerialFile& SFile, CString& strAnswerBuffer, const DWORD dwDlgID, const DWORD dwCnt );
    //void SaveCSVFile(std::string& strLine);
    void SaveExcel(sc::Excel& Xls, const std::string& TalkName, int DlgNum, int CaseNum);
    static void SaveExcelHead(sc::Excel& Xls);    
	//std::string& GetBasicTalk() { return m_strBasicTalk; }
};