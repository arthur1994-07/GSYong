#ifndef _NPC_TALK_MAN_H_
#define _NPC_TALK_MAN_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/function.hpp>

#include "./NpcTalkConditionDefine.h"
#include "../../SigmaCore/Xml/2.5.2/tinyxml.h"

class CNpcDialogueSet;
class CNpcDialogue;
class CNpcDialogueCase;
class CNpcTalkControl;
struct SNpcTalk;

namespace sc
{
    class Excel;
}

//! 2011-07-29 jgkim
//! CNpcDialogSet 통합관리
class NpcTalkMan
{
public:
    typedef std::tr1::unordered_map<std::string, std::tr1::shared_ptr<CNpcDialogueSet> > NPC_DLG_DATA;
    typedef NPC_DLG_DATA::iterator                                 NPC_DLG_DATA_ITER;
    typedef NPC_DLG_DATA::const_iterator                           NPC_DLG_DATA_CITER;
    typedef NPC_DLG_DATA::value_type                               NPC_DLG_DATA_VALUE;

    enum { VERSION=1, };

public:
    NpcTalkMan();
    ~NpcTalkMan();

private:
    NPC_DLG_DATA m_DlgData;    

public:
    bool Save(std::string& SaveFileName OUT);
    bool SaveExcel(std::string& SaveFileName OUT, boost::function<void (int)> PercentCallbackFunc=NULL);
    bool LoadExcel(const std::string& LoadFileName, boost::function<void (int)> PercentCallbackFunc=NULL);

    bool Load(const std::string& LoadFileName);

    void ExportTextAll();

    const std::tr1::shared_ptr<CNpcDialogueSet> Find(const std::string& NtkFile);
    bool AddDlgSet(const std::string& DlgSetName, std::tr1::shared_ptr<CNpcDialogueSet> spDlg);
    bool DeleteDlgSet(const std::string& DlgSetName);

    //! 분리된 Ntk 파일을 하나로 통합하기 위해서 임시로 만듬
    //! 통합된 이후에는 삭제를 합시다.
    void MergeNtkFiles(const std::vector<std::string>& vFileList);    
    void GetDlgList(std::vector<std::string>& vFileList);

	void Clear(void);

private:
    void Load_CNpcDialogueSet(sc::Excel& Xls);
    void Load_CNpcDialogue(sc::Excel& Xls);
    void Load_CNpcDialogueCase(sc::Excel& Xls);

    void Load_TalkCondition(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls);
    void Load_TalkConditionCompleteQuest(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls);
    void Load_TalkConditionHaveItem(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls);
    void Load_TalkConditionLearnSkill(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls);
    void Load_TalkConditionDisQuest(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls);
    void Load_TalkConditionDisSkill(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls);
    void Load_TalkConditionSkillFact(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls);

    void Load_CNpcTalkControl(sc::Excel& Xls);
    void Load_SNpcTalk(sc::Excel& Xls);
    void Load_SNpcTalkNeedItem(sc::Excel& Xls);
    void Load_SNpcTalkTradeItem(sc::Excel& Xls);

public:
	// 데이터통합;
	const char* GetNpcTalkString( const std::string& strKey, const WORD wNID, const WORD wSubID );
	const bool ConvertXml(const std::string& strPath);

	HRESULT LoadStringTableExcel(const char* szPath);
	HRESULT SaveStringTableExcel(std::string& strSavePath);

private:
	const bool NpcTalkMan::GetChildElement( TIXMLOLD::TiXmlElement** outXmlElem, 
		TIXMLOLD::TiXmlElement* rootElem,
		const char* atrbName,
		const char* atrbData);
};

#endif // _NPC_TALK_MAN_H_