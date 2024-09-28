#include "../pch.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../Item/GLItemMan.h"
#include "../Quest/GLQuestMan.h"
#include "../GLogicDataMan.h"
#include "./NpcDialogueSetExcelDefine.h"
#include "./NpcTalkCondition.h"
#include "./NpcTalk.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void SNpcTalk::SaveExcelHead(sc::Excel& Xls)
{
    int SheetNum = NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK;

    Xls.RenameWorksheet(SheetNum, NPC_TALK_EXCEL::NpcTalkControlTalkSheetName);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");    
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");    
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");

    Xls.SetCell(SheetNum, 0, Col++, "Version");

    Xls.SetCell(SheetNum, 0, Col++, "Talk ID");
    Xls.SetCell(SheetNum, 0, Col++, "Talk");

    Xls.SetCell(SheetNum, 0, Col++, "Action");
    Xls.SetCell(SheetNum, 0, Col++, "Action Num");

    Xls.SetCell(SheetNum, 0, Col++, "Action Quest Mid");
    Xls.SetCell(SheetNum, 0, Col++, "Action Quest Sid");
    Xls.SetCell(SheetNum, 0, Col++, "Action Quest Step");
    Xls.SetCell(SheetNum, 0, Col++, "Action Quest Name");

    Xls.SetCell(SheetNum, 0, Col++, "Random Time");
    Xls.SetCell(SheetNum, 0, Col++, "Bus File");
	Xls.SetCell(SheetNum, 0, Col++, "Hire Summon");

    Xls.SetCell(SheetNum, 0, Col++, "Condition Use");

    for (int i=0; i<MAX_QUEST_START; ++i) 
    {
        std::string QuestStart1(sc::string::format("Start Quest %1% Mid", i));
        std::string QuestStart2(sc::string::format("Start Quest %1% Sid", i));
        std::string QuestStart3(sc::string::format("Start Quest %1% Name", i));
        Xls.SetCell(SheetNum, 0, Col++, QuestStart1);
        Xls.SetCell(SheetNum, 0, Col++, QuestStart2);
        Xls.SetCell(SheetNum, 0, Col++, QuestStart3);
    }

    for (int i=0; i<MAX_RANDOM_PAGE; ++i)
    {
        std::string RandomPage(
            sc::string::format(
                "Random Page ID %1%",
                i));
        Xls.SetCell(SheetNum, 0, Col++, RandomPage);
    }
}

void SNpcTalk::SaveExcel(sc::Excel& Xls, const std::string& TalkName, int DlgNum, int CaseNum)
{
    int SheetNum = NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK;
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    int Col = 0;
    Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
    Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);
    Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);

    Xls.SetCell(SheetNum, TotalRow, Col++, VERSION);

    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwNID);
    Xls.SetCell(SheetNum, TotalRow, Col++, m_strTalk);

    Xls.SetCell(SheetNum, TotalRow, Col++, m_nACTION);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwACTION_NO);

    SNATIVEID ProgressQuest(m_dwACTION_PARAM1);
    Xls.SetCell(SheetNum, TotalRow, Col++, ProgressQuest.wMainID);
    Xls.SetCell(SheetNum, TotalRow, Col++, ProgressQuest.wSubID);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwACTION_PARAM2);
    if (m_dwACTION_PARAM1 != UINT_MAX)
        Xls.SetCell(SheetNum, TotalRow, Col++, GLogicData::GetInstance().GetQuestName(ProgressQuest));
    else
        Xls.SetCell(SheetNum, TotalRow, Col++, std::string(""));

    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwRandomTime);
    Xls.SetCell(SheetNum, TotalRow, Col++, m_strBusFile);
	Xls.SetCell(SheetNum, TotalRow, Col++, m_sHireSummon.dwID);

    if (m_spCondition)
        Xls.SetCell(SheetNum, TotalRow, Col++, 1);
    else
        Xls.SetCell(SheetNum, TotalRow, Col++, 0);

    for (int i=0; i<MAX_QUEST_START; ++i)
    {
        SNATIVEID QuestStart(m_dwQuestStartID[i]);
        Xls.SetCell(SheetNum, TotalRow, Col++, QuestStart.wMainID);
        Xls.SetCell(SheetNum, TotalRow, Col++, QuestStart.wSubID);
        if (QuestStart.dwID != UINT_MAX)
            Xls.SetCell(SheetNum, TotalRow, Col++, GLogicData::GetInstance().GetQuestName(QuestStart));
        else
            Xls.SetCell(SheetNum, TotalRow, Col++, std::string(""));
    }

    for (int i=0; i<MAX_RANDOM_PAGE; ++i)
        Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwRandomPageID[i]);

    SaveExcelNeedItem(Xls, TalkName, DlgNum, CaseNum, m_dwNID);
    SaveExcelTradeItem(Xls, TalkName, DlgNum, CaseNum, m_dwNID);
    
    if (m_spCondition)
    {
        m_spCondition->SaveExcel(
            TALK_CONTROL_NPC_TALK_SAVE,
            Xls,
            NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_CONDITION,
            TalkName,
            DlgNum,
            CaseNum,
            m_dwNID);
    }
}

void SNpcTalk::SaveExcelHeadNeedItem(sc::Excel& Xls)
{
    int SheetNum = NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_NEED_ITEM;

    Xls.RenameWorksheet(SheetNum, NPC_TALK_EXCEL::NpcTalkControlTalkNeedItem);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");    
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");    
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");
    Xls.SetCell(SheetNum, 0, Col++, "Talk ID");

    Xls.SetCell(SheetNum, 0, Col++, "Item Mid");
    Xls.SetCell(SheetNum, 0, Col++, "Item Sid");
    Xls.SetCell(SheetNum, 0, Col++, "Item Num");
    Xls.SetCell(SheetNum, 0, Col++, "Item Name");
}

void SNpcTalk::SaveExcelNeedItem(sc::Excel& Xls, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID)
{
    int SheetNum = NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_NEED_ITEM;
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);    
    for (size_t i=0; i<m_vecNeedItem.size(); ++i)
    {
        int Col = 0;
        Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
        Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);
        Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);
        Xls.SetCell(SheetNum, TotalRow, Col++, TalkID);

        const SITEM_NUM& ItemNum = m_vecNeedItem[i];
        Xls.SetCell(SheetNum, TotalRow, Col++, ItemNum.sNativeID.wMainID);
        Xls.SetCell(SheetNum, TotalRow, Col++, ItemNum.sNativeID.wSubID);
        Xls.SetCell(SheetNum, TotalRow, Col++, (int) ItemNum.dwNum);
        Xls.SetCell(SheetNum, TotalRow, Col++, GLogicData::GetInstance().GetItemName(ItemNum.sNativeID));

        TotalRow++;
    }
}

void SNpcTalk::SaveExcelHeadTradeItem(sc::Excel& Xls)
{
    int SheetNum = NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_TRADE_ITEM;

    Xls.RenameWorksheet(SheetNum, NPC_TALK_EXCEL::NpcTalkControlTalkTradeItem);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");    
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");    
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");
    Xls.SetCell(SheetNum, 0, Col++, "Talk ID");

    Xls.SetCell(SheetNum, 0, Col++, "Item Mid");
    Xls.SetCell(SheetNum, 0, Col++, "Item Sid");
    Xls.SetCell(SheetNum, 0, Col++, "Item Num");
    Xls.SetCell(SheetNum, 0, Col++, "Item Name");
}

void SNpcTalk::SaveExcelTradeItem(sc::Excel& Xls, const std::string& TalkName, int DlgNum, int CaseNum, int TalkID)
{    
    int SheetNum = NPC_TALK_EXCEL::NPC_TALK_CONTROL_TALK_TRADE_ITEM;
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);    
    for (size_t i=0; i<m_vecTradeItem.size(); ++i)
    {
        int Col = 0;
        Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
        Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);
        Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);
        Xls.SetCell(SheetNum, TotalRow, Col++, TalkID);

        const SITEM_NUM& ItemNum = m_vecTradeItem[i];
        Xls.SetCell(SheetNum, TotalRow, Col++, ItemNum.sNativeID.wMainID);
        Xls.SetCell(SheetNum, TotalRow, Col++, ItemNum.sNativeID.wSubID);
        Xls.SetCell(SheetNum, TotalRow, Col++, (int) ItemNum.dwNum);
        Xls.SetCell(SheetNum, TotalRow, Col++, GLogicData::GetInstance().GetItemName(ItemNum.sNativeID));        

        TotalRow++;
    }
}

bool SNpcTalk::LoadExcel(sc::Excel& Xls, int SheetNum, int Row, int Version)
{
    int Col = 4;    
    std::string TempStr;
    Xls.GetValue(SheetNum, Row, Col++, m_dwNID);
    Xls.GetValue(SheetNum, Row, Col++, m_strTalk);

    Xls.GetValue(SheetNum, Row, Col++, m_nACTION);
    Xls.GetValue(SheetNum, Row, Col++, m_dwACTION_NO);

    SNATIVEID ProgressQuest;
    Xls.GetValue(SheetNum, Row, Col++, ProgressQuest.wMainID);
    Xls.GetValue(SheetNum, Row, Col++, ProgressQuest.wSubID);
    m_dwACTION_PARAM1 = ProgressQuest.Id();

    Xls.GetValue(SheetNum, Row, Col++, m_dwACTION_PARAM2);

    Xls.GetValue(SheetNum, Row, Col++, TempStr); // Skip

    Xls.GetValue(SheetNum, Row, Col++, m_dwRandomTime);
    Xls.GetValue(SheetNum, Row, Col++, m_strBusFile);
	Xls.GetValue(SheetNum, Row, Col++, m_sHireSummon.dwID);

    bool bCondition = false;
    Xls.GetValue(SheetNum, Row, Col++, bCondition);
    if (bCondition)
    {
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
        m_spCondition = spCondition;
    }

    for (int i=0; i<MAX_QUEST_START; ++i)
    {
        SNATIVEID QuestStart;
        Xls.GetValue(SheetNum, Row, Col++, QuestStart.wMainID);
        Xls.GetValue(SheetNum, Row, Col++, QuestStart.wSubID);
        m_dwQuestStartID[i] = QuestStart.Id();
        Xls.GetValue(SheetNum, Row, Col++, TempStr);
    }

    for (int i=0; i<MAX_RANDOM_PAGE; ++i)
        Xls.GetValue(SheetNum, Row, Col++, m_dwRandomPageID[i]);

    return true;
}