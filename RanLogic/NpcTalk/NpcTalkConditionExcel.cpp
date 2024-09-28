#include "../pch.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"

#include "../Item/GLItemMan.h"
#include "../Quest/GLQuest.h"
#include "../Quest/GLQuestMan.h"
#include "../GLogicDataMan.h"
#include "./NpcDialogueSetExcelDefine.h"
#include "./NpcTalkCondition.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void SNpcTalkCondition::SaveExcelHead(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{    
    Xls.RenameWorksheet(SheetNum, SheetName);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");    
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        Xls.SetCell(SheetNum, 0, Col++, "Talk ID");

    Xls.SetCell(SheetNum, 0, Col++, "Version");
    Xls.SetCell(SheetNum, 0, Col++, "Use");

    Xls.SetCell(SheetNum, 0, Col++, "Sign Level");
    Xls.SetCell(SheetNum, 0, Col++, "Level");

    Xls.SetCell(SheetNum, 0, Col++, "Class");    
    Xls.SetCell(SheetNum, 0, Col++, "School");

    Xls.SetCell(SheetNum, 0, Col++, "Sign Elemental");
    Xls.SetCell(SheetNum, 0, Col++, "Elemental");

    Xls.SetCell(SheetNum, 0, Col++, "Sign Action Point");
    Xls.SetCell(SheetNum, 0, Col++, "Action Point");

    Xls.SetCell(SheetNum, 0, Col++, "Time");
    Xls.SetCell(SheetNum, 0, Col++, "Time Begin");
    Xls.SetCell(SheetNum, 0, Col++, "Time End");

    Xls.SetCell(SheetNum, 0, Col++, "Money");
    Xls.SetCell(SheetNum, 0, Col++, "Party Num");

    Xls.SetCell(SheetNum, 0, Col++, "Quest Mid");
    Xls.SetCell(SheetNum, 0, Col++, "Quest Sid");    
    Xls.SetCell(SheetNum, 0, Col++, "Quest Step");    
    Xls.SetCell(SheetNum, 0, Col++, "Quest Name");
}

void SNpcTalkCondition::SaveExcel(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID)
{    
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    int Col = 0;
    Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
    Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);    
    Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);

    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        Xls.SetCell(SheetNum, TotalRow, Col++, TalkID);

    Xls.SetCell(SheetNum, TotalRow, Col++, (int) VERSION);

    if (m_bUse)
        Xls.SetCell(SheetNum, TotalRow, Col++, 1);
    else
        Xls.SetCell(SheetNum, TotalRow, Col++, 0);

    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_signLevel);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_wLevel);

    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwClass);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_wSchool);

    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_signElemental);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_nElemental);

    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_signActionPoint);
    Xls.SetCell(SheetNum, TotalRow, Col++, m_nActionPoint);

    if (m_bTime)
        Xls.SetCell(SheetNum, TotalRow, Col++, 1);
    else
        Xls.SetCell(SheetNum, TotalRow, Col++, 0);

    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwTimeBegin);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwTimeEnd);

    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwMONEY);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwPARTYMEN_NUM);

    SNATIVEID QuestId(m_dwQUEST_NID);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) QuestId.wMainID);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) QuestId.wSubID);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwQUEST_STEP);
    if (m_dwQUEST_NID != UINT_MAX)
        Xls.SetCell(SheetNum, TotalRow, Col++, GLogicData::GetInstance().GetQuestName(QuestId));
    else
        Xls.SetCell(SheetNum, TotalRow, Col++, "");


    if (emType == DLG_CASE_CONDITION_SAVE)
    {
        SaveExcelCompleteQuest(Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_QUEST_COMPALTE, TalkName, DlgNum, CaseNum, -1);    
        SaveExcelHaveItem     (Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_HAVE_ITEM,      TalkName, DlgNum, CaseNum, -1);    
        SaveExcelLearnSkill   (Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_LEARN_SKILL,    TalkName, DlgNum, CaseNum, -1);
        SaveExcelDisQuest     (Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_DIS_QUEST,      TalkName, DlgNum, CaseNum, -1);
        SaveExcelDisSkill     (Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_DIS_SKILL,      TalkName, DlgNum, CaseNum, -1);
        SaveExcelSkillFact    (Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION_SKILL_FACT,     TalkName, DlgNum, CaseNum, -1);
    }
    else if (emType == TALK_CONTROL_NPC_TALK_SAVE)
    {
        SaveExcelCompleteQuest(Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_QUEST_COMPALTE, TalkName, DlgNum, CaseNum, TalkID);    
        SaveExcelHaveItem     (Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_HAVE_ITEM,      TalkName, DlgNum, CaseNum, TalkID);    
        SaveExcelLearnSkill   (Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_LEARN_SKILL,    TalkName, DlgNum, CaseNum, TalkID);
        SaveExcelDisQuest     (Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_DIS_QUEST,      TalkName, DlgNum, CaseNum, TalkID);
        SaveExcelDisSkill     (Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_DIS_SKILL,      TalkName, DlgNum, CaseNum, TalkID);
        SaveExcelSkillFact    (Xls, NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_SKILL_FACT,     TalkName, DlgNum, CaseNum, TalkID);
    }
    else
    {

    }
}

void SNpcTalkCondition::SaveExcelHeadCompleteQuest(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{    
    Xls.RenameWorksheet(SheetNum, SheetName);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");    
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");    
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        Xls.SetCell(SheetNum, 0, Col++, "Talk ID");

    Xls.SetCell(SheetNum, 0, Col++, "Quest Mid");        
    Xls.SetCell(SheetNum, 0, Col++, "Quest Sid");
    Xls.SetCell(SheetNum, 0, Col++, "Quest Name");
}

void SNpcTalkCondition::SaveExcelCompleteQuest(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID)
{    
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);

    for (size_t i=0; i<m_vecCompleteQuest.size(); ++i)
    {
        int Col = 0;
        Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
        Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);
        Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);
        if (TalkID != -1)
            Xls.SetCell(SheetNum, TotalRow, Col++, TalkID);

        SNATIVEID QuestId(m_vecCompleteQuest[i]);
        Xls.SetCell(SheetNum, TotalRow, Col++, QuestId.wMainID);
        Xls.SetCell(SheetNum, TotalRow, Col++, QuestId.wSubID);
        Xls.SetCell(SheetNum, TotalRow, Col++, GLogicData::GetInstance().GetQuestName(QuestId));

        TotalRow++;
    }
}

void SNpcTalkCondition::SaveExcelHeadHaveItem(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{    
    Xls.RenameWorksheet(SheetNum, SheetName);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");    
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");    
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        Xls.SetCell(SheetNum, 0, Col++, "Talk ID");

    Xls.SetCell(SheetNum, 0, Col++, "Item Mid");        
    Xls.SetCell(SheetNum, 0, Col++, "Item Sid");
    Xls.SetCell(SheetNum, 0, Col++, "Item Num");
    Xls.SetCell(SheetNum, 0, Col++, "Item Name");
}

void SNpcTalkCondition::SaveExcelHaveItem(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID)
{    
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);

    for (size_t i=0; i<m_vecHaveItem.size(); ++i)
    {
        int Col = 0;
        const SITEM_NUM& HaveItem = m_vecHaveItem[i];
        Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
        Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);        
        Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);
        if (TalkID != -1)
            Xls.SetCell(SheetNum, TotalRow, Col++, TalkID);

        Xls.SetCell(SheetNum, TotalRow, Col++, HaveItem.GetNativeId().wMainID);
        Xls.SetCell(SheetNum, TotalRow, Col++, HaveItem.GetNativeId().wSubID);
        Xls.SetCell(SheetNum, TotalRow, Col++, (int) HaveItem.GetCount());
        Xls.SetCell(SheetNum, TotalRow, Col++, GLogicData::GetInstance().GetItemName(HaveItem.GetNativeId()));

        TotalRow++;
    }
}

void SNpcTalkCondition::SaveExcelHeadLearnSkill(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{    
    Xls.RenameWorksheet(SheetNum, SheetName);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");    
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");    
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        Xls.SetCell(SheetNum, 0, Col++, "Talk ID");

    Xls.SetCell(SheetNum, 0, Col++, "Skill Mid");        
    Xls.SetCell(SheetNum, 0, Col++, "Skill Sid");
    Xls.SetCell(SheetNum, 0, Col++, "Skill Level");
    Xls.SetCell(SheetNum, 0, Col++, "Skill Name");
}

void SNpcTalkCondition::SaveExcelLearnSkill(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID)
{    
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);

    for (size_t i=0; i<m_vecLearnSkill.size(); ++i)
    {
        int Col = 0;
        const SSkillCondition& sSkillCondition = m_vecLearnSkill[i];
        
        Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
        Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);
        Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);
        if (TalkID != -1)
            Xls.SetCell(SheetNum, TotalRow, Col++, TalkID);

        SNATIVEID SkillId(sSkillCondition.SkillId());
        Xls.SetCell(SheetNum, TotalRow, Col++, SkillId.wMainID);
        Xls.SetCell(SheetNum, TotalRow, Col++, SkillId.wSubID);
        Xls.SetCell(SheetNum, TotalRow, Col++, sSkillCondition.SkillLevel());
        Xls.SetCell(SheetNum, TotalRow, Col++, GLogicData::GetInstance().GetSkillName(SkillId));

        TotalRow++;
    }
}

void SNpcTalkCondition::SaveExcelHeadDisQuest(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{    
    Xls.RenameWorksheet(SheetNum, SheetName);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");    
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");    
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        Xls.SetCell(SheetNum, 0, Col++, "Talk ID");

    Xls.SetCell(SheetNum, 0, Col++, "Quest Mid");        
    Xls.SetCell(SheetNum, 0, Col++, "Quest Sid");
    Xls.SetCell(SheetNum, 0, Col++, "Quest Name");
}

void SNpcTalkCondition::SaveExcelDisQuest(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID)
{     
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);

    for (size_t i=0; i<m_vecDisQuest.size(); ++i)
    {
        int Col = 0;
        SNATIVEID QuestId(m_vecDisQuest[i]);

        Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
        Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);
        Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);
        if (TalkID != -1)
            Xls.SetCell(SheetNum, TotalRow, Col++, TalkID);
        
        Xls.SetCell(SheetNum, TotalRow, Col++, QuestId.wMainID);
        Xls.SetCell(SheetNum, TotalRow, Col++, QuestId.wSubID);        
        Xls.SetCell(SheetNum, TotalRow, Col++, GLogicData::GetInstance().GetQuestName(QuestId));

        TotalRow++;
    }
}

void SNpcTalkCondition::SaveExcelHeadDisSkill(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{
    Xls.RenameWorksheet(SheetNum, SheetName);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");    
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");    
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        Xls.SetCell(SheetNum, 0, Col++, "Talk ID");

    Xls.SetCell(SheetNum, 0, Col++, "Skill Mid");        
    Xls.SetCell(SheetNum, 0, Col++, "Skill Sid");
    Xls.SetCell(SheetNum, 0, Col++, "Skill Name");
}

void SNpcTalkCondition::SaveExcelDisSkill(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID)
{
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);

    for (size_t i=0; i<m_vecDisSkill.size(); ++i)
    {
        int Col = 0;
        SNATIVEID SkillId(m_vecDisSkill[i]);

        Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
        Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);        
        Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);
        if (TalkID != -1)
            Xls.SetCell(SheetNum, TotalRow, Col++, TalkID);

        Xls.SetCell(SheetNum, TotalRow, Col++, SkillId.wMainID);
        Xls.SetCell(SheetNum, TotalRow, Col++, SkillId.wSubID);        
        Xls.SetCell(SheetNum, TotalRow, Col++, GLogicData::GetInstance().GetSkillName(SkillId));

        TotalRow++;
    }
}

void SNpcTalkCondition::SaveExcelHeadSkillFact(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{    
    Xls.RenameWorksheet(SheetNum, SheetName);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");    
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");    
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        Xls.SetCell(SheetNum, 0, Col++, "Talk ID");

    Xls.SetCell(SheetNum, 0, Col++, "Skill Mid");
    Xls.SetCell(SheetNum, 0, Col++, "Skill Sid");
    Xls.SetCell(SheetNum, 0, Col++, "Skill Level");
    Xls.SetCell(SheetNum, 0, Col++, "Skill Name");
}

void SNpcTalkCondition::SaveExcelSkillFact(sc::Excel& Xls, int SheetNum, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID)
{     
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);

    for (size_t i=0; i<m_vecSkillFact.size(); ++i)
    {
        int Col = 0;
        const SSkillCondition& SkillFact = m_vecSkillFact[i];
        SNATIVEID SkillId(SkillFact.SkillId());

        Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
        Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);
        Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);
        if (TalkID != -1)
            Xls.SetCell(SheetNum, TotalRow, Col++, TalkID);

        Xls.SetCell(SheetNum, TotalRow, Col++, SkillId.wMainID);
        Xls.SetCell(SheetNum, TotalRow, Col++, SkillId.wSubID);
        Xls.SetCell(SheetNum, TotalRow, Col++, SkillFact.SkillLevel());
        Xls.SetCell(SheetNum, TotalRow, Col++, GLogicData::GetInstance().GetSkillName(SkillId));

        TotalRow++;
    }
}

bool SNpcTalkCondition::LoadExcel(TALK_CONDITION_SAVE_TYPE emType, sc::Excel& Xls, int SheetNum, int Row, int Version)
{    
    int Col = 4;
    if (emType == TALK_CONTROL_NPC_TALK_SAVE)
        Col = 5;

    int SignLevel = 0;
    int SignElemental = 0;
    int SignActionPoint = 0;

    Xls.GetValue(SheetNum, Row, Col++, m_bUse);
    Xls.GetValue(SheetNum, Row, Col++, SignLevel); m_signLevel = static_cast<EM_CONDITIONSIGN> (SignLevel);
    Xls.GetValue(SheetNum, Row, Col++, m_wLevel);

    Xls.GetValue(SheetNum, Row, Col++, m_dwClass);
    Xls.GetValue(SheetNum, Row, Col++, m_wSchool);

    Xls.GetValue(SheetNum, Row, Col++, SignElemental); m_signElemental = static_cast<EM_CONDITIONSIGN> (SignElemental);
    Xls.GetValue(SheetNum, Row, Col++, m_nElemental);

    Xls.GetValue(SheetNum, Row, Col++, SignActionPoint); m_signActionPoint = static_cast<EM_CONDITIONSIGN> (SignActionPoint);
    Xls.GetValue(SheetNum, Row, Col++, m_nActionPoint);

    Xls.GetValue(SheetNum, Row, Col++, m_bTime);

    Xls.GetValue(SheetNum, Row, Col++, m_dwTimeBegin);
    Xls.GetValue(SheetNum, Row, Col++, m_dwTimeEnd);

    Xls.GetValue(SheetNum, Row, Col++, m_dwMONEY);
    Xls.GetValue(SheetNum, Row, Col++, m_dwPARTYMEN_NUM);

    SNATIVEID QuestId;
    Xls.GetValue(SheetNum, Row, Col++, QuestId.wMainID);
    Xls.GetValue(SheetNum, Row, Col++, QuestId.wSubID);
    m_dwQUEST_NID = QuestId.dwID;

    Xls.GetValue(SheetNum, Row, Col++, m_dwQUEST_STEP);    

    return true;
}