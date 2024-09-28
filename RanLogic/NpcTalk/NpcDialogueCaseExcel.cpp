#include "../pch.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "./NpcDialogueCase.h"
#include "./NpcTalkControl.h"
#include "./NpcTalkCondition.h"
#include "./NpcDialogueSetExcelDefine.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CNpcDialogueCase::SaveExcelHead(sc::Excel& Xls)
{
    int SheetNum = NPC_TALK_EXCEL::NPC_DIALOGUE_CASE;
    Xls.RenameWorksheet(SheetNum, NPC_TALK_EXCEL::DialogueCaseSheetName);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");
    Xls.SetCell(SheetNum, 0, Col++, "Version");
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");
    Xls.SetCell(SheetNum, 0, Col++, "Talk Condition");
    Xls.SetCell(SheetNum, 0, Col++, "Talk Control");
    Xls.SetCell(SheetNum, 0, Col++, "Basic Talk");
}

void CNpcDialogueCase::SaveExcel(sc::Excel& Xls, const std::string& TalkName, int DlgNum, int CaseNum)
{
    int SheetNum = NPC_TALK_EXCEL::NPC_DIALOGUE_CASE;
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);
    int Col = 0;
    Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
    Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) VERSION);
    Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);

    if (m_spCondition)
        Xls.SetCell(SheetNum, TotalRow, Col++, 1);
    else
        Xls.SetCell(SheetNum, TotalRow, Col++, 0);

    if (m_spTalkControl)
        Xls.SetCell(SheetNum, TotalRow, Col++, 1);
    else
        Xls.SetCell(SheetNum, TotalRow, Col++, 0);
    Xls.SetCell(SheetNum, TotalRow, Col++, m_strBasicTalk);

    if (m_spCondition)
        m_spCondition->SaveExcel(DLG_CASE_CONDITION_SAVE, Xls, NPC_TALK_EXCEL::NPC_TALK_CONDITION, TalkName, DlgNum, CaseNum, -1);

    if (m_spTalkControl)
        m_spTalkControl->SaveExcel(Xls, TalkName, DlgNum, CaseNum);
}

bool CNpcDialogueCase::LoadExcel(sc::Excel& Xls, int SheetNum, int Row)
{
    int Col = 2;
    int Version = 0;
    int CaseNum = 0;
    bool bCondition = false;
    bool bControl = false;    
    Xls.GetValue(SheetNum, Row, Col++, Version);
    Xls.GetValue(SheetNum, Row, Col++, CaseNum);
    Xls.GetValue(SheetNum, Row, Col++, bCondition);
    Xls.GetValue(SheetNum, Row, Col++, bControl);
    Xls.GetValue(SheetNum, Row, Col++, m_strBasicTalk);

    if (bCondition)
    {
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
        m_spCondition = spCondition;
    }

    if (bControl)
    {
        std::tr1::shared_ptr<CNpcTalkControl> spControl(new CNpcTalkControl);
        m_spTalkControl = spControl;
    }

    return true;
}