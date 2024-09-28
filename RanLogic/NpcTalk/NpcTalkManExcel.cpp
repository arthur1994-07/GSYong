#include "../pch.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"

#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../GLogicData.h"
#include "../GLogicDataMan.h"

#include "./NpcDialogueSetExcelDefine.h"
#include "./NpcDialogueSet.h"
#include "./NpcDialogue.h"
#include "./NpcTalkControl.h"
#include "./NpcTalkMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool NpcTalkMan::SaveExcel(std::string& SaveFileName OUT, boost::function<void (int)> PercentCallbackFunc)
{
    //     std::string FullPath(sc::getAppPath());    
    //     SUBPATH* pPath = GLogicData::GetInstance().GetPath();
    //     if (pPath)
    //         FullPath.append(pPath->NtkFileRoot());

    // 확장자를 없애자
    std::string ExcelFileName(GLogicData::GetInstance().GetNewNpcTalkFileName());
    CString FileExt(".");
    FileExt.Append(sc::string::getFileExt(ExcelFileName.c_str()));
    sc::string::trim(ExcelFileName, FileExt.GetString());

    //ExcelFileName = FullPath+ExcelFileName;
    //AfxMessageBox(ExcelFileName.c_str(), MB_OK);
    sc::Excel Xls(ExcelFileName, SaveFileName, 1, true);
    //AfxMessageBox(SaveFileName.c_str(), MB_OK);

    // ------------------------------------------------------------------------
    // 미리 Excel Worksheet 를 추가해 둔다.
    for (int i=0; i<NPC_TALK_EXCEL::SHEET_TOTAL_NUM; ++i)
        Xls.AddWorksheet(std::string(""));

    // ------------------------------------------------------------------------
    // Save Excel Head

    // CNpcDialogueSet
    CNpcDialogueSet::SaveExcelHead(Xls);

    // CNpcDialogue
    CNpcDialogue::SaveExcelHead(Xls);

    // CNpcDialogueCase
    CNpcDialogueCase::SaveExcelHead(Xls);

    // SNpcTalkCondition
    SNpcTalkCondition::SaveExcelHead             (DLG_CASE_CONDITION_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION,                NPC_TALK_EXCEL::NpcTalkCondtionSheetName);
    SNpcTalkCondition::SaveExcelHeadCompleteQuest(DLG_CASE_CONDITION_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_QUEST_COMPALTE, NPC_TALK_EXCEL::NpcTalkConditionQuestComplateSheetName);
    SNpcTalkCondition::SaveExcelHeadHaveItem     (DLG_CASE_CONDITION_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_HAVE_ITEM,      NPC_TALK_EXCEL::NpcTalkConditionHaveItemSheetName);
    SNpcTalkCondition::SaveExcelHeadLearnSkill   (DLG_CASE_CONDITION_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_LEARN_SKILL,    NPC_TALK_EXCEL::NpcTalkConditionLearnSkillSheetName);
    SNpcTalkCondition::SaveExcelHeadDisQuest     (DLG_CASE_CONDITION_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_DIS_QUEST,      NPC_TALK_EXCEL::NpcTalkConditionDisQuestSheetName);
    SNpcTalkCondition::SaveExcelHeadDisSkill     (DLG_CASE_CONDITION_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_DIS_SKILL,      NPC_TALK_EXCEL::NpcTalkConditionDisSkillSheetName);
    SNpcTalkCondition::SaveExcelHeadSkillFact    (DLG_CASE_CONDITION_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_SKILL_FACT,     NPC_TALK_EXCEL::NpcTalkConditionSkillFactSheetName);

    // CNpcTalkControl
    CNpcTalkControl::SaveExcelHead(Xls);

    // SNpcTalk
    SNpcTalk::SaveExcelHead(Xls);
    SNpcTalk::SaveExcelHeadNeedItem(Xls);
    SNpcTalk::SaveExcelHeadTradeItem(Xls);

    // SNpcTalk / SNpcTalkCondition
    SNpcTalkCondition::SaveExcelHead             (TALK_CONTROL_NPC_TALK_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_CONDITION,      NPC_TALK_EXCEL::NpcTalkControlTalkCondition);
    SNpcTalkCondition::SaveExcelHeadCompleteQuest(TALK_CONTROL_NPC_TALK_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_QUEST_COMPALTE, NPC_TALK_EXCEL::NpcTalkControlTalkQuestComplate);
    SNpcTalkCondition::SaveExcelHeadHaveItem     (TALK_CONTROL_NPC_TALK_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_HAVE_ITEM,      NPC_TALK_EXCEL::NpcTalkControlTalkHaveItem);
    SNpcTalkCondition::SaveExcelHeadLearnSkill   (TALK_CONTROL_NPC_TALK_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_LEARN_SKILL,    NPC_TALK_EXCEL::NpcTalkControlTalkLearnSkill);
    SNpcTalkCondition::SaveExcelHeadDisQuest     (TALK_CONTROL_NPC_TALK_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_DIS_QUEST,      NPC_TALK_EXCEL::NpcTalkControlTalkDisQuest);
    SNpcTalkCondition::SaveExcelHeadDisSkill     (TALK_CONTROL_NPC_TALK_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_DIS_SKILL,      NPC_TALK_EXCEL::NpcTalkControlTalkDisSkill);
    SNpcTalkCondition::SaveExcelHeadSkillFact    (TALK_CONTROL_NPC_TALK_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_SKILL_FACT,     NPC_TALK_EXCEL::NpcTalkControlTalkSkillFact);

    // EM_CONDITIONSIGN
    SaveEmConditionSignExcel(Xls, NPC_TALK_EXCEL::EM_CONDITIONSIGN_SHEET, NPC_TALK_EXCEL::EmConditionSignSheetName);

    // EMCHARCLASS
    SaveEmCharClassExcel(Xls, NPC_TALK_EXCEL::EMCHARCLASS_SHEET, NPC_TALK_EXCEL::EmCharClassSheetName);

    // EMSCHOOLFLAG
    SaveEmSchoolFlagExcel(Xls, NPC_TALK_EXCEL::EMSCHOOLFLAG_SHEET, NPC_TALK_EXCEL::EmSchoolFlagSheetName);

    if (PercentCallbackFunc)
        PercentCallbackFunc(5); // 5%

    // Item Data
    GLogicData::GetInstance().SaveItemDataSimple(Xls, NPC_TALK_EXCEL::ITEM_DATA_SHEET, NPC_TALK_EXCEL::ItemDataSheetName);

    // GLQuestMan
    GLogicData::GetInstance().SaveQuestDataSimple(Xls, NPC_TALK_EXCEL::QUEST_DATA_SHEET, NPC_TALK_EXCEL::QuestDataSheetName);

    // GLSkillMan
    GLogicData::GetInstance().SaveSkillDataSimple(Xls, NPC_TALK_EXCEL::SKILL_DATA_SHEET, NPC_TALK_EXCEL::SkillDataSheetName);

    // Crow Data
    GLogicData::GetInstance().SaveCrowDataSimple(Xls, NPC_TALK_EXCEL::NPC_DATA_SHEET, NPC_TALK_EXCEL::NpcDataSheetName);

    if (PercentCallbackFunc)
        PercentCallbackFunc(10); // 10%

    // ------------------------------------------------------------------------
    // Name Sorting 
    std::vector<std::string> vTalkList;
    GetDlgList(vTalkList);

    if (PercentCallbackFunc)
        PercentCallbackFunc(10); // 20%

    // ------------------------------------------------------------------------
    // Save Data    
    for (size_t i=0; i<vTalkList.size(); ++i)
    {
        std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = Find(vTalkList[i]);
        if (spDlgSet)
            spDlgSet->SaveExcel(Xls);

        if (PercentCallbackFunc)
        {
            int Percentage = static_cast<int> ((i*100)/vTalkList.size());
            PercentCallbackFunc(Percentage);
        }
    }

    return true;
}

bool NpcTalkMan::LoadExcel(const std::string& LoadFileName, boost::function<void (int)> PercentCallbackFunc)
{
    if (LoadFileName.empty())
        return false;

    if (!sc::file::isExist(LoadFileName))
    {
        std::string Msg(
            sc::string::format(
            "%1% 파일이 존재하지 않습니다.",
            LoadFileName));
        AfxMessageBox(Msg.c_str(), MB_OK|MB_ICONEXCLAMATION);
        return false;
    }

    sc::Excel Xls;
    if (!Xls.Load(LoadFileName))
    {
        std::string Msg(
            sc::string::format(
            "%1% Excel 파일 읽기에 실패하였습니다. 파일이 열려 있는지 확인하세요.",
            LoadFileName));
        AfxMessageBox(Msg.c_str(), MB_OK|MB_ICONEXCLAMATION);
        return false;
    }

    // 깨끗하게 지우자
    m_DlgData.clear();
        
    if (PercentCallbackFunc)
        PercentCallbackFunc(4);
    Load_CNpcDialogueSet(Xls); 
    
    if (PercentCallbackFunc)
        PercentCallbackFunc(8);
    Load_CNpcDialogue(Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(12);
    Load_CNpcDialogueCase(Xls);

    // SNpcTalkCondition
    if (PercentCallbackFunc)
        PercentCallbackFunc(16);
    Load_TalkCondition(DLG_CASE_CONDITION_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(20);
    Load_TalkConditionCompleteQuest(DLG_CASE_CONDITION_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(24);
    Load_TalkConditionHaveItem(DLG_CASE_CONDITION_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(28);
    Load_TalkConditionLearnSkill(DLG_CASE_CONDITION_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(32);
    Load_TalkConditionDisQuest(DLG_CASE_CONDITION_SAVE, Xls);
        
    if (PercentCallbackFunc)
        PercentCallbackFunc(36);
    Load_TalkConditionDisSkill(DLG_CASE_CONDITION_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(40);
    Load_TalkConditionSkillFact(DLG_CASE_CONDITION_SAVE, Xls);

    // CNpcTalkControl
    if (PercentCallbackFunc)
        PercentCallbackFunc(44);
    Load_CNpcTalkControl(Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(48);
    Load_SNpcTalk(Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(52);
    Load_SNpcTalkNeedItem(Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(56);
    Load_SNpcTalkTradeItem(Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(60);
    Load_TalkCondition(TALK_CONTROL_NPC_TALK_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(65);
    Load_TalkConditionCompleteQuest(TALK_CONTROL_NPC_TALK_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(70);
    Load_TalkConditionHaveItem(TALK_CONTROL_NPC_TALK_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(75);
    Load_TalkConditionLearnSkill(TALK_CONTROL_NPC_TALK_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(80);
    Load_TalkConditionDisQuest(TALK_CONTROL_NPC_TALK_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(90);
    Load_TalkConditionDisSkill(TALK_CONTROL_NPC_TALK_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(95);
    Load_TalkConditionSkillFact(TALK_CONTROL_NPC_TALK_SAVE, Xls);

    if (PercentCallbackFunc)
        PercentCallbackFunc(100);
    
    for (NPC_DLG_DATA_ITER iter=m_DlgData.begin(); iter!=m_DlgData.end(); ++iter)
    {
        std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
        spDlgSet->LoadExcelFinal();
    }

    return true;
}

void NpcTalkMan::Load_CNpcDialogueSet(sc::Excel& Xls)
{
    // CNpcDialogueSet/Head 제외
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::BASE_SHEET_NUM);
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet(new CNpcDialogueSet);
        if (spDlgSet->LoadExcel(Xls, SheetNum, i))
        {
            std::string LowerNtkFile(spDlgSet->GetDlgSetName());
            sc::string::lowerString(LowerNtkFile);
            m_DlgData.insert(NPC_DLG_DATA_VALUE(LowerNtkFile, spDlgSet));
        }
    }
}

void NpcTalkMan::Load_CNpcDialogue(sc::Excel& Xls)
{
    // CNpcDialogue/Head 제외
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_DIALOGUE_SHEET);
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;        
        std::string TalkName;
        Xls.GetValue(SheetNum, i, Col++, TalkName);

        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
        spDialogue->LoadExcel(Xls, SheetNum, i);

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            spDlgSet->AddDialogue(spDialogue);
        }
    }
}

void NpcTalkMan::Load_CNpcDialogueCase(sc::Excel& Xls)
{
    // CNpcDialogueCase/Head 제외
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_DIALOGUE_CASE);
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int Version = 0;
        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, Version);

        std::tr1::shared_ptr<CNpcDialogueCase> spCase(new CNpcDialogueCase);
        spCase->LoadExcel(Xls, SheetNum, i);

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            spDlgSet->AddDialogueCase(DlgID, spCase);
        }        
    }
}

void NpcTalkMan::Load_TalkCondition(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONDITION);
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_CONDITION);

    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int CaseNum = 0;
        int Version = 0;
        int TalkId = 0;
        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, CaseNum);
        
        if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            Xls.GetValue(SheetNum, i, Col++, TalkId);

        Xls.GetValue(SheetNum, i, Col++, Version);

        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
        spCondition->LoadExcel(emType, Xls, SheetNum, i, Version);

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            if (emType == DLG_CASE_CONDITION_SAVE)
            {   
                spDlgSet->AddDialogueCaseCondition(DlgID, CaseNum, spCondition);
            }
            else if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            {
                spDlgSet->AddDialogueCaseControlCondition(DlgID, CaseNum, TalkId, spCondition);
            }
            else
            {
            }
        }
    }
}

void NpcTalkMan::Load_TalkConditionCompleteQuest(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONDITION_QUEST_COMPALTE);
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_QUEST_COMPALTE);

    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int CaseNum = 0;
        int TalkId = 0;
        
        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, CaseNum);
        if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            Xls.GetValue(SheetNum, i, Col++, TalkId);

        SNATIVEID QuestId;
        Xls.GetValue(SheetNum, i, Col++, QuestId.wMainID);
        Xls.GetValue(SheetNum, i, Col++, QuestId.wSubID);

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            if (emType == DLG_CASE_CONDITION_SAVE)
            {
                spDlgSet->AddDialogueCaseConditionCompleteQuest(DlgID, CaseNum, QuestId);
            }
            else if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            {
                spDlgSet->AddDialogueCaseControlConditionCompleteQuest(DlgID, CaseNum, TalkId, QuestId);
            }
            else
            {
            }
        }
    }
}

void NpcTalkMan::Load_TalkConditionHaveItem(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONDITION_HAVE_ITEM);
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_HAVE_ITEM);
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int CaseNum = 0;
        int TalkId = 0;

        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, CaseNum);
        if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            Xls.GetValue(SheetNum, i, Col++, TalkId);

        SNATIVEID ItemId;
        int ItemNum = 0;
        Xls.GetValue(SheetNum, i, Col++, ItemId.wMainID);
        Xls.GetValue(SheetNum, i, Col++, ItemId.wSubID);
        Xls.GetValue(SheetNum, i, Col++, ItemNum);

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            if (emType == DLG_CASE_CONDITION_SAVE)
            {
                spDlgSet->AddDialogueCaseConditionHaveItem(DlgID, CaseNum, ItemId, ItemNum);
            }
            else if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            {
                spDlgSet->AddDialogueCaseControlConditionHaveItem(DlgID, CaseNum, TalkId, ItemId, ItemNum);
            }
            else
            {
            }
        }
    }
}

void NpcTalkMan::Load_TalkConditionLearnSkill(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONDITION_LEARN_SKILL);
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_LEARN_SKILL);
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int CaseNum = 0;
        int TalkId = 0;

        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, CaseNum);
        if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            Xls.GetValue(SheetNum, i, Col++, TalkId);

        SNATIVEID SkillId;
        int SkillLevel = 0;
        Xls.GetValue(SheetNum, i, Col++, SkillId.wMainID);
        Xls.GetValue(SheetNum, i, Col++, SkillId.wSubID);
        Xls.GetValue(SheetNum, i, Col++, SkillLevel);

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            if (emType == DLG_CASE_CONDITION_SAVE)
            {
                spDlgSet->AddDialogueCaseConditionLearnSkill(DlgID, CaseNum, SkillId, SkillLevel);
            }
            else if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            {
                spDlgSet->AddDialogueCaseControlConditionLearnSkill(DlgID, CaseNum, TalkId, SkillId, SkillLevel);
            }
            else
            {
            }
        }
    }
}

void NpcTalkMan::Load_TalkConditionDisQuest(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONDITION_DIS_QUEST);
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_DIS_QUEST);
        
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int CaseNum = 0;
        int TalkId = 0;

        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, CaseNum);
        if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            Xls.GetValue(SheetNum, i, Col++, TalkId);

        SNATIVEID QuestId;        
        Xls.GetValue(SheetNum, i, Col++, QuestId.wMainID);
        Xls.GetValue(SheetNum, i, Col++, QuestId.wSubID);

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            if (emType == DLG_CASE_CONDITION_SAVE)
            {
                spDlgSet->AddDialogueCaseConditionDisQuest(DlgID, CaseNum, QuestId);
            }
            else if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            {
                spDlgSet->AddDialogueCaseControlConditionDisQuest(DlgID, CaseNum, TalkId, QuestId);
            }
            else
            {
            }
        }
    }
}

void NpcTalkMan::Load_TalkConditionDisSkill(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONDITION_DIS_SKILL);
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_DIS_SKILL);
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int CaseNum = 0;
        int TalkId = 0;

        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, CaseNum);
        if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            Xls.GetValue(SheetNum, i, Col++, TalkId);

        SNATIVEID SkillId;        
        Xls.GetValue(SheetNum, i, Col++, SkillId.wMainID);
        Xls.GetValue(SheetNum, i, Col++, SkillId.wSubID);

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            if (emType == DLG_CASE_CONDITION_SAVE)
            {
                spDlgSet->AddDialogueCaseConditionDisSkill(DlgID, CaseNum, SkillId);
            }
            else if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            {
                spDlgSet->AddDialogueCaseControlConditionDisSkill(DlgID, CaseNum, TalkId, SkillId);
            }
            else
            {
            }
        }
    }
}

void NpcTalkMan::Load_TalkConditionSkillFact(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONDITION_SKILL_FACT);
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_SKILL_FACT);

    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int CaseNum = 0;
        int TalkId = 0;

        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, CaseNum);
        if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            Xls.GetValue(SheetNum, i, Col++, TalkId);

        SNATIVEID SkillId;        
        int SkillLevel = 0;
        Xls.GetValue(SheetNum, i, Col++, SkillId.wMainID);
        Xls.GetValue(SheetNum, i, Col++, SkillId.wSubID);
        Xls.GetValue(SheetNum, i, Col++, SkillLevel);

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            if (emType == DLG_CASE_CONDITION_SAVE)
            {
                spDlgSet->AddDialogueCaseConditionSkillFact(DlgID, CaseNum, SkillId, SkillLevel);
            }
            else if (emType == TALK_CONTROL_NPC_TALK_SAVE)
            {
                spDlgSet->AddDialogueCaseControlConditionSkillFact(DlgID, CaseNum, TalkId, SkillId, SkillLevel);
            }
            else
            {

            }
        }
    }
}

void NpcTalkMan::Load_CNpcTalkControl(sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL);
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int CaseNum = 0;
        int Version = 0;
        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, CaseNum);
        Xls.GetValue(SheetNum, i, Col++, Version);

        std::tr1::shared_ptr<CNpcTalkControl> spControl(new CNpcTalkControl);
        
        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            spDlgSet->AddDialogueCaseControl(DlgID, CaseNum, spControl);
        }
    }
}

void NpcTalkMan::Load_SNpcTalk(sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK);
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int CaseNum = 0;
        int Version = 0;
        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, CaseNum);
        Xls.GetValue(SheetNum, i, Col++, Version);

        std::tr1::shared_ptr<SNpcTalk> spTalk(new SNpcTalk);
        spTalk->LoadExcel(Xls, SheetNum, i, Version);

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            spDlgSet->AddDialogueCaseControlTalk(DlgID, CaseNum, spTalk);
        }
    }
}

void NpcTalkMan::Load_SNpcTalkNeedItem(sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_NEED_ITEM);
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int CaseNum = 0;
        int TalkId = 0;
        SNATIVEID NeedItem;
        int NeedItemNum = 0;
        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, CaseNum);
        Xls.GetValue(SheetNum, i, Col++, TalkId);

        Xls.GetValue(SheetNum, i, Col++, NeedItem.wMainID);
        Xls.GetValue(SheetNum, i, Col++, NeedItem.wSubID);
        Xls.GetValue(SheetNum, i, Col++, NeedItemNum);        

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            spDlgSet->AddDialogueCaseControlTalkNeedItem(DlgID, CaseNum, TalkId, NeedItem, NeedItemNum);
        }
    }
}

void NpcTalkMan::Load_SNpcTalkTradeItem(sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_TRADE_ITEM);
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    for (int i=1; i<TotalRow; ++i)
    {
        int Col = 0;
        std::string TalkName;
        int DlgID = 0;
        int CaseNum = 0;
        int TalkId = 0;
        SNATIVEID TradeItem;
        int TradeItemNum = 0;
        Xls.GetValue(SheetNum, i, Col++, TalkName);
        Xls.GetValue(SheetNum, i, Col++, DlgID);
        Xls.GetValue(SheetNum, i, Col++, CaseNum);
        Xls.GetValue(SheetNum, i, Col++, TalkId);

        Xls.GetValue(SheetNum, i, Col++, TradeItem.wMainID);
        Xls.GetValue(SheetNum, i, Col++, TradeItem.wSubID);
        Xls.GetValue(SheetNum, i, Col++, TradeItemNum);        

        sc::string::lowerString(TalkName);
        NPC_DLG_DATA_ITER iter = m_DlgData.find(TalkName);
        if (iter != m_DlgData.end())
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
            spDlgSet->AddDialogueCaseControlTalkTradeItem(DlgID, CaseNum, TalkId, TradeItem, TradeItemNum);
        }
    }
}

HRESULT NpcTalkMan::LoadStringTableExcel(const char* szPath)
{
	int row = 0, col = 0, max_row = 0, sheet = 0;
	sc::Excel ExcelFile;
	if (ExcelFile.Load(szPath) == false )
	{
		std::string Msg(
			sc::string::format(
			"%1% Excel 파일 읽기에 실패하였습니다. 파일이 열려 있는지 확인하세요.",
			szPath));
		AfxMessageBox(Msg.c_str(), MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	if ( ExcelFile.WorkSheetTotalCols(sheet) != 10 )
	{
		std::string Msg(
			sc::string::format(
			"%1% Excel 파일 읽기에 실패하였습니다. 엑셀 열을 확인하세요.",
			szPath));
		AfxMessageBox(Msg.c_str(), MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	max_row = ExcelFile.WorkSheetTotalRow(sheet);
	for ( row = 1; row < max_row; ++row )	// 0 is Header cell;
	{
		std::string strId, strValue;
		ExcelFile.GetValue(sheet, row, 1, strId);		// Id;		
		ExcelFile.GetValue(sheet, row, 9, strValue);	// Data;

		char szID[256] = {0};
		_tcscpy(szID, strId.c_str());

		std::vector<std::string> vecString;
		int nCount = 0;
		const char* const sperator = "_";
		char* rest;
		char* token;
		for ( token = strtok_s(szID, sperator, &rest);
			token != 0; token = strtok_s(NULL, sperator, &rest), ++nCount)
		{
			vecString.push_back(token);
		}

		// Talk/Answer 판별;
		bool bTalkData = true;
		char szNtkName[256] = {0};
		const int nStrSize = static_cast<int>(vecString.size())-1;
		for ( int i = nStrSize; i >= 0; --i )
		{
			const int len = strlen(vecString[i].c_str());
			for ( int j = 0; j < len; ++j )
			{
				// if ( '9'<vecString[i].c_str()[j] || '0'> vecString[i].c_str()[j] )
				if ( !isdigit(vecString[i].c_str()[j]) )
				{
					if ( nStrSize - i == 3 )
						bTalkData = false;

					CString strFindName(strId.c_str());
					int nFind = 0, nAccum = 0;
					while ( (nFind = strFindName.Find("_", nFind+1)) != -1 )
					{
						if ( nAccum == i )
						{
							strncpy(szNtkName, strId.c_str(), nFind);
							i = -1; 
							break;
						}
						++nAccum;
					}
					break;
				}
			}
		}

		const int nSize = bTalkData ? static_cast<int>(vecString.size()) - 2:static_cast<int>(vecString.size())-3;

		std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = this->Find(szNtkName);
		if ( !spDlgSet )
		{
			std::string OkMsg(
				sc::string::format("\"%1%\" not exist!", strId.c_str() ) );
			MessageBox(NULL, OkMsg.c_str(), _T("fail"), MB_OK );
			continue;
		}

		const int nDlgNum = atoi(vecString[nSize].c_str());
		std::tr1::shared_ptr<CNpcDialogue> spDialogue = spDlgSet->GetDialogue(nDlgNum);
		if (!spDialogue)
		{
			std::string OkMsg(
				sc::string::format("DialogueNum \"%1%\" of \"%2%\" not exist!", nDlgNum, strId.c_str() ) );
			MessageBox(NULL, OkMsg.c_str(), _T("fail"), MB_OK );
			continue;
		}

		const int nDlgCaseNum = atoi(vecString[nSize+1].c_str());
		std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = spDialogue->GetDlgCase(nDlgCaseNum);
		if ( !spDlgCase )
		{
			std::string OkMsg(
				sc::string::format("DialogueNum \"%1%\", DialogueCase \"%2%\" of \"%3%\" not exist!", nDlgNum, nDlgCaseNum, strId.c_str() ) );
			MessageBox(NULL, OkMsg.c_str(), _T("fail"), MB_OK );
			continue;
		}

		if ( bTalkData == true )
		{
			CGameTextMan::GetInstance().InsertString(strId.c_str(), strValue.c_str(), CGameTextMan::EM_NPCTALK_TEXT);
		}
		else
		{
			const int nAnswerNum = atoi(vecString[nSize+2].c_str());
			if ( spDlgCase->GetTalkControl() == NULL )
			{
				std::string OkMsg(
					sc::string::format("DialogueNum \"%1\", DialogueCase \"%2%\", Answer \"%3%\" of \"%4%\" not exist!", nDlgNum, nDlgCaseNum, nAnswerNum, strId.c_str() ) );
				MessageBox(NULL, OkMsg.c_str(), _T("fail"), MB_OK );
				continue;
			}

			// std::tr1::shared_ptr<SNpcTalk> spNpcTalk = spDlgCase->GetTalkControl()->GetTalk2(nAnswerNum);
			//if ( spNpcTalk == NULL )
			//{
			//	std::string OkMsg(
			//		sc::string::format("DialogueNum \"%1\", DialogueCase \"%2\", Answer \"%3\" of \"%4%\" not exist!", nDlgNum, nDlgCaseNum, nAnswerNum, strId.c_str() ) );
			//	MessageBox(NULL, OkMsg.c_str(), _T("fail"), MB_OK );
			//	continue;
			//}
			bool bFindAnswer = false;
			CNpcTalkControl::NPCMAP talkMapConst = spDlgCase->GetTalkControl()->GetTalkMapConst();
			for (CNpcTalkControl::NPCMAP_IT npciter=talkMapConst.begin(); npciter!=talkMapConst.end(); ++npciter)
			{
				std::tr1::shared_ptr<SNpcTalk> spNpcTalk = npciter->second;
				if (spNpcTalk->m_dwNID == nAnswerNum)
				{
					bFindAnswer = true;
					break;
				}
			}

			if ( bFindAnswer )
				CGameTextMan::GetInstance().InsertString(strId.c_str(), strValue.c_str(), CGameTextMan::EM_NPCTALK_TEXT);
		}
		
	}
	return S_OK;
}

HRESULT NpcTalkMan::SaveStringTableExcel(std::string& strSavePath)
{
	int sheet = 0, row = 0, col = 0;
	sc::Excel ExcelFile("NpcTalkStringTable", strSavePath, sheet, false);
	ExcelFile.SetCell(sheet, row, col++, L"NUM"		);
	ExcelFile.SetCell(sheet, row, col++, L"ID"		);
	ExcelFile.SetCell(sheet, row, col++, L"TYPE"	);
	ExcelFile.SetCell(sheet, row, col++, L"VERSION"	);
	ExcelFile.SetCell(sheet, row, col++, L"INDEX"	);
	ExcelFile.SetCell(sheet, row, col++, L"STATE"	);
	ExcelFile.SetCell(sheet, row, col++, L"DATANUM"	);
	ExcelFile.SetCell(sheet, row, col++, L"COUNTRY"	);
	ExcelFile.SetCell(sheet, row, col++, L"C_NAME"	);
	ExcelFile.SetCell(sheet, row, col++, L"DATA"	);

	int nCount = 0;
	const int nCountryNum = static_cast<int>(language::ProvidelangToTexttoolcountrynum(RANPARAM::emProvideLangFlag));
	const std::string strTYPE("SENTENSE");

	NPC_DLG_DATA_ITER iter, end = m_DlgData.end();

	for (iter=m_DlgData.begin(); iter!=end; ++iter)
	{
		std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;

		std::string strTalk(spDlgSet->GetDlgSetName().c_str());
		CNpcDialogueSet::DIALOGMAP_ITER dlgSetIter, dlgSetEnd = spDlgSet->GetDialogueMap()->end();
		for ( dlgSetIter = spDlgSet->GetDialogueMap()->begin(); dlgSetIter!=dlgSetEnd; ++dlgSetIter )
		{
			INT iCount;
			std::tr1::shared_ptr<CNpcDialogue> spDialogue = dlgSetIter->second;
			CNpcDialogue::DLGCASE_ITER dlgCaseIter, dlgCaseEnd = spDialogue->m_vecCase.end();
			for ( iCount = 0, dlgCaseIter = spDialogue->m_vecCase.begin(); dlgCaseIter!=dlgCaseEnd; ++dlgCaseIter, ++iCount )
			{
				std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = *dlgCaseIter;

				CString strTalkNameKey, strTalkName;
				strTalkNameKey.Format("%s_%d_%d", spDlgSet->GetDlgSetName().c_str(), spDialogue->GetDlgNID(), iCount);
				strTalkName = CGameTextMan::GetInstance().GetText(strTalkNameKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT);
				// talk;
				++row;
				col = 0;
				ExcelFile.SetCell(sheet, row, col++, nCount++);
				ExcelFile.SetCell(sheet, row, col++, strTalkNameKey.GetString());
				ExcelFile.SetCell(sheet, row, col++, strTYPE);
				ExcelFile.SetCell(sheet, row, col++, 1);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, nCountryNum);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, strTalkName.GetString());

				// answer
				if ( spDlgCase->GetTalkControl() == NULL )
				{
					++iCount;
					continue;
				}
				CNpcTalkControl::NPCMAP talkMapConst = spDlgCase->GetTalkControl()->GetTalkMapConst();
				for (CNpcTalkControl::NPCMAP_IT npciter=talkMapConst.begin(); npciter!=talkMapConst.end(); ++npciter)
				{
					std::tr1::shared_ptr<SNpcTalk> spNpcTalk = npciter->second;

					CString strAnswerNameKey, strAnswerName;
					strAnswerNameKey.Format("%s_%d_%d_%d", spDlgSet->GetDlgSetName().c_str(), spDialogue->GetDlgNID(), iCount, spNpcTalk->m_dwNID );
					strAnswerName = CGameTextMan::GetInstance().GetText(strAnswerNameKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT);
					//answer
					++row;
					col = 0;
					ExcelFile.SetCell(sheet, row, col++, nCount++);
					ExcelFile.SetCell(sheet, row, col++, strAnswerNameKey.GetString());
					ExcelFile.SetCell(sheet, row, col++, strTYPE);
					ExcelFile.SetCell(sheet, row, col++, 1);
					ExcelFile.SetCell(sheet, row, col++, 0);
					ExcelFile.SetCell(sheet, row, col++, 0);
					ExcelFile.SetCell(sheet, row, col++, 0);
					ExcelFile.SetCell(sheet, row, col++, nCountryNum);
					ExcelFile.SetCell(sheet, row, col++, 0);
					ExcelFile.SetCell(sheet, row, col++, strAnswerName.GetString());
				}
				++iCount;
			}
		}
	}
	return ExcelFile.Save() == true ? S_OK : E_FAIL;
}