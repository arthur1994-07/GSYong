#include "../pch.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"

#include "./NpcTalk.h"
#include "./NpcDialogueSetExcelDefine.h"
#include "./NpcTalkControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CNpcTalkControl::SaveExcelHead(sc::Excel& Xls)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL);
    Xls.RenameWorksheet(SheetNum, NPC_TALK_EXCEL::NpcTalkControlSheetName);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");    
    Xls.SetCell(SheetNum, 0, Col++, "Dialog ID");    
    Xls.SetCell(SheetNum, 0, Col++, "Case Num");
    Xls.SetCell(SheetNum, 0, Col++, "Version");
}

void CNpcTalkControl::SaveExcel(sc::Excel& Xls, const std::string& TalkName, int DlgNum, int CaseNum)
{
    int SheetNum = static_cast<int> (NPC_TALK_EXCEL::NPC_TALK_CONTROL);
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);

    int Col = 0;
    Xls.SetCell(SheetNum, TotalRow, Col++, TalkName);
    Xls.SetCell(SheetNum, TotalRow, Col++, DlgNum);
    Xls.SetCell(SheetNum, TotalRow, Col++, CaseNum);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) VERSION);

    for (NPCMAP_IT iter=m_mapTalk.begin(); iter!=m_mapTalk.end(); ++iter)
    {
        std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;
        spNpcTalk->SaveExcel(Xls, TalkName, DlgNum, CaseNum);
    }
}