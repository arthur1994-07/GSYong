#include "../pch.h"
#include <algorithm>

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/Common/StlFunctions.h"

#include "../GLogicData.h"
#include "./NpcDialogueSetExcelDefine.h"
#include "./NpcDialogueSet.h"
#include "./NpcDialogue.h"
#include "./NpcTalk.h"
#include "./NpcDialogueCase.h"
#include "./NpcTalkControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool CNpcDialogueSet::SaveExcel(sc::Excel& Xls)
{    
    ReportAction();

    // Data
    int SheetNum = NPC_TALK_EXCEL::BASE_SHEET_NUM;
    int Col = 0;
    int Row = Xls.WorkSheetTotalRow(SheetNum);
    Xls.SetCell(SheetNum, Row, Col++, (int) VERSION);
    Xls.SetCell(SheetNum, Row, Col++, m_TalkName);
    
    for (int i=0; i<SNpcTalk::EM_BASIC_SIZE; ++i)
    {
        Xls.SetCell(SheetNum, Row, Col++, (int) m_bBasicTalk[i]);
    }

    // CNpcDialogue    
    for (DIALOGMAP_ITER iter=m_mapDialogue.begin(); iter!=m_mapDialogue.end(); ++iter)
    {
        std::tr1::shared_ptr<CNpcDialogue> spDialogue = iter->second;
        spDialogue->SaveExcel(Xls, m_TalkName);
    }

    // ------------------------------------------------------------------------
    ResetModified();
    return true;
}

void CNpcDialogueSet::SaveExcelHead(sc::Excel& Xls)
{
    int Col = 0;
    int SheetNum = 0;
 
    // CNpcDialogueSet --------------------------------------------------------
    SheetNum = static_cast<int> (NPC_TALK_EXCEL::BASE_SHEET_NUM);
    Xls.RenameWorksheet(SheetNum, NPC_TALK_EXCEL::BaseSheetName);
    Xls.SetCell(SheetNum, 0, Col++, "Version");
    Xls.SetCell(SheetNum, 0, Col++, "File Name");
    for (int i=0; i<SNpcTalk::EM_BASIC_SIZE; ++i)
    {
        std::string BasicTalkUse(
            sc::string::format(
                "Basic Talk %1%",
                i));
        Xls.SetCell(SheetNum, 0, Col++, BasicTalkUse);
    }    
}

bool CNpcDialogueSet::LoadExcel(sc::Excel& Xls, int SheetNum, int Row)
{
    ResetModified();
    RemoveAllDialogue();

    int Col = 0;
    int Version = 0;
    std::string TalkName;
    Xls.GetValue(SheetNum, Row, Col++, Version);
    Xls.GetValue(SheetNum, Row, Col++, TalkName);

    m_TalkName = TalkName;
    switch (Version)
    {
    case VERSION:
        return LoadExcel_0016(Xls, SheetNum, Row);
        break;

    default:
        {
            std::string ErrMsg(
                sc::string::format(
                "%1% unknown version %2%",
                m_TalkName, Version));
            sc::writeLogError(ErrMsg);
            AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONEXCLAMATION);
            return false;
        }            
        break;
    }
    return true;

    // talk 전역 ID 배정
    //AssignTalkGlobID();
    return true;
}

void CNpcDialogueSet::LoadExcelFinal()
{
    // talk 전역 ID 배정
    AssignTalkGlobID();
}

bool CNpcDialogueSet::LoadExcel_0016(sc::Excel& Xls, int Sheet, int Row)
{
    int Col = 2;
    for (int i=0; i<SNpcTalk::EM_BASIC_SIZE; ++i)
        Xls.GetValue(Sheet, Row, Col++, m_bBasicTalk[i]);    
    return true;
}