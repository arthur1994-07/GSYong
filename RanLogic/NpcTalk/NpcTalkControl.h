///////////////////////////////////////////////////////////////////////
//	CNpcTalkControl
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//
//
///////////////////////////////////////////////////////////////////////

#pragma	once

#include <boost/tr1/memory.hpp>
#include "../../SigmaCore/File/BaseStream.h"
#include <map>
struct SNpcTalk;

namespace sc
{
	class SerialFile;
    class Excel;
}

class CNpcTalkControl
{
public:
	enum { VERSION = 0x0002, };
    typedef std::map<DWORD, std::tr1::shared_ptr<SNpcTalk> > NPCMAP;
	typedef NPCMAP::iterator		   NPCMAP_IT;
	typedef NPCMAP::const_iterator	   NPCMAP_CIT;
    typedef NPCMAP::value_type         NPCMAP_VALUE;

public:
	CNpcTalkControl();
	virtual	~CNpcTalkControl();
    CNpcTalkControl& operator= (const CNpcTalkControl& rNpcTalkControl);

private:
    NPCMAP m_mapTalk;

public:
	DWORD MakeTalkNID();

	BOOL AddTalk(std::tr1::shared_ptr<SNpcTalk> spTalk);
	void DelTalk(DWORD NativeID);

	std::tr1::shared_ptr<SNpcTalk> GetTalk(DWORD NativeID);
	std::tr1::shared_ptr<SNpcTalk> GetTalk2(DWORD NativeNum);

	DWORD ToUp(DWORD dwNID);
	DWORD ToDown(DWORD dwNID);

	NPCMAP*	GetTalkMap();
    const NPCMAP& GetTalkMapConst() const { return m_mapTalk; }
	DWORD GetTalkNum();

	void RemoveTalk();

    BOOL LoadFile(sc::BaseStream& SFile);
private:
	BOOL LOAD_0001(sc::BaseStream& SFile);
	BOOL LOAD_0002(sc::BaseStream& SFile);	//	데이터 포맷 변경 진행

public:
	BOOL SaveFile(sc::SerialFile& SFile, CString& strAnswerBuffer, const DWORD dwDlgID, const DWORD dwCnt );
	//BOOL SaveCSVFile(std::fstream& SFile);

    static void SaveExcelHead(sc::Excel& Xls);
    void SaveExcel(sc::Excel& Xls, const std::string& TalkName, int DlgNum, int CaseNum);
};
