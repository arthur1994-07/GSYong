#include "../pch.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"
#include "../GLUseFeatures.h"
#include "../RANPARAM.h"
#include "./GLItemMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLItemMan::SaveItemDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName)
{
    Xls.RenameWorksheet(Sheet, SheetName);
    int Row = 0;
    int Col = 0;
    
    // Head
    Xls.SetCell(Sheet, Row, Col++, "Item Mid");
    Xls.SetCell(Sheet, Row, Col++, "Item Sid");
    Xls.SetCell(Sheet, Row, Col++, "Item Name");
    Xls.SetCell(Sheet, Row, Col++, "Item DWORD");

    if (!m_ppItem)
        return;

    // Data
    for (int i=0; i<MAX_MID; ++i)
    {
        for (int j=0; j<MAX_SID; ++j)
        {
            const CItemNode* pItemNode = m_ppItem[i][j];
            if (pItemNode)
            {
                ++Row;
                Col = 0;
                const SITEM& ItemData = pItemNode->m_sItem;
                const SNATIVEID& ItemId  = ItemData.BasicId();
                Xls.SetCell(Sheet, Row, Col++, ItemId.wMainID);
                Xls.SetCell(Sheet, Row, Col++, ItemId.wSubID);
                Xls.SetCell(Sheet, Row, Col++, ItemData.GetNameStr());
                Xls.SetCell(Sheet, Row, Col++, (int) ItemId.dwID);
            }
        }
    }    
}
 

HRESULT GLItemMan::LoadStringTableExcel(const char* szPath)
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

		if ( m_ppItem[nMID][nSID] )
		{
			CGameTextMan::GetInstance().InsertString(
				strId.c_str(), strValue.c_str(), CGameTextMan::EM_ITEM_TEXT);
		}
	}

	return S_OK;
}

HRESULT GLItemMan::SaveStringTableExcel(/*const char* szPath*/OUT std::string& strSavePath)
{
	int sheet = 0, row = 0, col = 0;
	sc::Excel ExcelFile("ItemStringTable", strSavePath, sheet, false);
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
	for ( int i = 0; i < MAX_MID; ++i )
	{
		for ( int j = 0; j < MAX_SID; ++j )
		{
			if ( m_ppItem[i][j] )
			{
				const SNATIVEID sNID = m_ppItem[i][j]->m_sItem.BasicId();
				CString strName, strDesc, strNameKey, strDescKey;
				strNameKey.Format("IN_%03d_%03d", sNID.Mid(), sNID.Sid());
				strDescKey.Format("ID_%03d_%03d", sNID.Mid(), sNID.Sid());

				strName = CGameTextMan::GetInstance().GetText(strNameKey.GetString(), 0, CGameTextMan::EM_ITEM_TEXT);
				strDesc = CGameTextMan::GetInstance().GetText(strDescKey.GetString(), 0, CGameTextMan::EM_ITEM_TEXT);

				// const int nResultValue = MultiByteToWideChar(CP_UTF8,0,szValue,strlen(szValue)+1,wsValue,sizeof(wsValue)/sizeof(wsValue[0]));
				// const int nResultID = MultiByteToWideChar(CP_UTF8,0,szID,strlen(szID)+1,wsID,sizeof(wsID)/sizeof(wsID[0]));

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