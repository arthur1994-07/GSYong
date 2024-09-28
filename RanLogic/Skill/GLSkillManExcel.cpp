#include "../pch.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"
#include "../GLUseFeatures.h"
#include "../RANPARAM.h"
#include "./GLSkill.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLSkillMan::SaveSkillDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName)
{
    Xls.RenameWorksheet(Sheet, SheetName);
    int Row = 0;
    int Col = 0;

    // Head
    Xls.SetCell(Sheet, Row, Col++, "SKill Mid");
    Xls.SetCell(Sheet, Row, Col++, "Skill Sid");
    Xls.SetCell(Sheet, Row, Col++, "Skill Name");
    Xls.SetCell(Sheet, Row, Col++, "Skill DWORD");

    // Data    
    for (size_t i=0; i<EMSKILLCLASS_NSIZE; ++i)
    {
        for (size_t j=0; j<MAX_CLASSSKILL; ++j)
        {
            GLSKILL* pSkillData = m_pSkills[i][j];            
            if (pSkillData)
            {
                ++Row;
                Col = 0;            
                Xls.SetCell(Sheet, Row, Col++, pSkillData->GetId().wMainID);
                Xls.SetCell(Sheet, Row, Col++, pSkillData->GetId().wSubID);
                Xls.SetCell(Sheet, Row, Col++, pSkillData->GetNameStr());
                Xls.SetCell(Sheet, Row, Col++, (int) pSkillData->GetId().dwID);
            }
        }
    }
}

HRESULT GLSkillMan::LoadStrTblExcelFile(const char* szPath)
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

		GLSKILL* pSKILL;
		if ( (pSKILL = GetData(nMID, nSID)) != NULL )
		{
			CGameTextMan::GetInstance().InsertString(
				strId.c_str(), strValue.c_str(), CGameTextMan::EM_SKILL_TEXT);
		}
	}

	return S_OK;
}

HRESULT GLSkillMan::SaveStrTblExcelFile(std::string& strSavePath)
{
	int sheet = 0, row = 0, col = 0;
	sc::Excel ExcelFile("SkillStringTable", strSavePath, sheet, false);
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
	for ( int i = 0; i < EMSKILLCLASS_NSIZE; ++i )
	{
		for ( int j = 0; j < MAX_CLASSSKILL; ++j )
		{
			GLSKILL* pSKILL;
			if ( (pSKILL = GetData(i, j)) != NULL )
			{
				const SNATIVEID sNID = pSKILL->GetId();
				CString strName, strDesc, strNameKey, strDescKey;
				strNameKey.Format("SN_%03d_%03d", sNID.Mid(), sNID.Sid());
				strDescKey.Format("SD_%03d_%03d", sNID.Mid(), sNID.Sid());

				strName = CGameTextMan::GetInstance().GetText(strNameKey.GetString(), 0, CGameTextMan::EM_SKILL_TEXT);
				strDesc = CGameTextMan::GetInstance().GetText(strDescKey.GetString(), 0, CGameTextMan::EM_SKILL_TEXT);

				// Name
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

				// Desc
				++row;
				col = 0;
				ExcelFile.SetCell(sheet, row, col++, nCount++);
				ExcelFile.SetCell(sheet, row, col++, strDescKey.GetString());
				ExcelFile.SetCell(sheet, row, col++, strTYPE);
				ExcelFile.SetCell(sheet, row, col++, 1);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, nCountryNum);
				ExcelFile.SetCell(sheet, row, col++, 0);
				ExcelFile.SetCell(sheet, row, col++, strDesc.GetString());
			}
		}
	}
	return ExcelFile.Save() == true ? S_OK : E_FAIL;
}