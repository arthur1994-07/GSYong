#include "../pch.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"
#include "./GLCrowData.h"
#include "./GLCrowDataMan.h"

#include "../GLUseFeatures.h"
#include "../RANPARAM.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../enginelib/DxTools/DxFontDefine.h"
// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCrowDataMan::SaveCrowDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName)
{
    Xls.RenameWorksheet(Sheet, SheetName);
    int Row = 0;
    int Col = 0;

    // Head
    Xls.SetCell(Sheet, Row, Col++, "Npc Mid");
    Xls.SetCell(Sheet, Row, Col++, "Npc Sid");
    Xls.SetCell(Sheet, Row, Col++, "Npc Name");
    Xls.SetCell(Sheet, Row, Col++, "Npc DWORD");
    Xls.SetCell(Sheet, Row, Col++, "Npc Talk Name");
    Xls.SetCell(Sheet, Row, Col++, "Boss");
    Xls.SetCell(Sheet, Row, Col++, "Level");
    Xls.SetCell(Sheet, Row, Col++, "HP");
    Xls.SetCell(Sheet, Row, Col++, "Bonus Exp");
    Xls.SetCell(Sheet, Row, Col++, "Bonus CP");

    std::vector<DWORD> vCrowId;
    for (int i=0; i<MAX_CROW_MID; ++i)
    {
        for (int j=0; j<MAX_CROW_SID; ++j)
        {
            SCROWDATA* pCrowData = m_ppPCROWDATA[i][j];
            if (pCrowData)
            {
                SNATIVEID CrowId = pCrowData->GetId();
                vCrowId.push_back(CrowId.dwID);                
            }
        }
    }

    std::sort(vCrowId.begin(), vCrowId.end());

    for (size_t i=0; i<vCrowId.size(); ++i)
    {
        SNATIVEID CrowId(vCrowId[i]);
        SCROWDATA* pCrowData = m_ppPCROWDATA[CrowId.wMainID][CrowId.wSubID];
        ++Row;
        Col = 0;            

        Xls.SetCell(Sheet, Row, Col++, CrowId.wMainID);
        Xls.SetCell(Sheet, Row, Col++, CrowId.wSubID);
        Xls.SetCell(Sheet, Row, Col++, pCrowData->GetName());
        Xls.SetCell(Sheet, Row, Col++, static_cast<int> (CrowId.dwID));
        Xls.SetCell(Sheet, Row, Col++, pCrowData->GetTalkFileStr());
        if (pCrowData->IsBoss())
            Xls.SetCell(Sheet, Row, Col++, 1);
        else
            Xls.SetCell(Sheet, Row, Col++, 0);
        Xls.SetCell(Sheet, Row, Col++, pCrowData->GetLevel());
        Xls.SetCell(Sheet, Row, Col++, static_cast<int> (pCrowData->GetHp()));
        Xls.SetCell(Sheet, Row, Col++, static_cast<int> (pCrowData->GetBonusExp()));
        Xls.SetCell(Sheet, Row, Col++, pCrowData->GetBonusCp());
    }
}

HRESULT GLCrowDataMan::LoadStrTblExcelFile(const char* szPath)
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

		char szID[32] = {0};
		_tcscpy(szID, strId.c_str());

		std::string strSperatorID[3];	// IN_%03d_%03d
		int nCount = 0;
		const char* const sperator = "_";
		char* rest;
		char* token;
		for ( token = strtok_s(szID, sperator, &rest);
			token != 0; token = strtok_s(NULL, sperator, &rest), ++nCount)
		{
			strSperatorID[nCount] = token;
		}

		const int nMID = atoi(strSperatorID[1].c_str());
		const int nSID = atoi(strSperatorID[2].c_str());

		if ( m_ppPCROWDATA[nMID][nSID] )
		{
			CGameTextMan::GetInstance().InsertString(
				strId.c_str(), strValue.c_str(), CGameTextMan::EM_CROW_TEXT);
		}
	}

	return S_OK;
}

HRESULT GLCrowDataMan::SaveStrTblExcelFile(std::string& strSavePath)
{
	int sheet = 0, row = 0, col = 0;
	sc::Excel ExcelFile("CrowStringTable", strSavePath, sheet, false);
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

	const int nCountryNum = static_cast<int>(language::ProvidelangToTexttoolcountrynum(RANPARAM::emProvideLangFlag));
	const std::string strTYPE("SENTENSE");

	int nCount = 0;
	for ( int i = 0; i < MAX_CROW_MID; ++i )
	{
		for ( int j = 0; j < MAX_CROW_SID; ++j )
		{
			if ( m_ppPCROWDATA[i][j] )
			{
				const SNATIVEID sNID = m_ppPCROWDATA[i][j]->GetId();
				CString strName, strNameKey;
				strNameKey.Format("CN_%03d_%03d", sNID.Mid(), sNID.Sid());

				strName = CGameTextMan::GetInstance().GetText(strNameKey.GetString(), 0, CGameTextMan::EM_CROW_TEXT);

				++row;
				col = 0;
				ExcelFile.SetCell(sheet, row, col++, nCount++);
				ExcelFile.SetCell(sheet, row, col++, strNameKey.GetString());
				ExcelFile.SetCell(sheet, row, col++, strTYPE);
				ExcelFile.SetCell(sheet, row, col++, 1);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, nCountryNum);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, strName.GetString());
			}
		}
	}
	return ExcelFile.Save() == true ? S_OK : E_FAIL;
}