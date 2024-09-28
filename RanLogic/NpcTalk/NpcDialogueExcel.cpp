#include "../pch.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "./NpcDialogueCase.h"
#include "./NpcTalkCondition.h"
#include "./NpcDialogueSetExcelDefine.h"
#include "./NpcDialogue.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CNpcDialogue::SaveExcelHead(sc::Excel& Xls)
{
    int SheetNum = NPC_TALK_EXCEL::NPC_DIALOGUE_SHEET;
    Xls.RenameWorksheet(SheetNum, NPC_TALK_EXCEL::DialogueSheetName);

    int Col = 0;
    Xls.SetCell(SheetNum, 0, Col++, "FileName");
    Xls.SetCell(SheetNum, 0, Col++, "Version");    
    Xls.SetCell(SheetNum, 0, Col++, "Dlg ID");
}

bool CNpcDialogue::SaveExcel(sc::Excel& Xls, const std::string& FileName)
{
    int SheetNum = NPC_TALK_EXCEL::NPC_DIALOGUE_SHEET;
    int TotalRow = Xls.WorkSheetTotalRow(SheetNum);

    int Col = 0;
    Xls.SetCell(SheetNum, TotalRow, Col++, FileName);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) VERSION);
    Xls.SetCell(SheetNum, TotalRow, Col++, (int) m_dwNID);

    for (size_t i=0; i<m_vecCase.size(); ++i)
    {        
        std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = m_vecCase[i];
        spDlgCase->SaveExcel( Xls, FileName, m_dwNID, static_cast< int >( i ) );
    }
    return true;
}

bool CNpcDialogue::LoadExcel(sc::Excel& Xls, int SheetNum, int Row)
{
    int Version = 0;
    int Col = 1;
    Xls.GetValue(SheetNum, Row, Col++, Version);
    Xls.GetValue(SheetNum, Row, Col++, m_dwNID);

    return true;
}