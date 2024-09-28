#include "../pch.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"
#include "./GLQuestMan.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLQuestMan::SaveQuestDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName)
{
    Xls.RenameWorksheet(Sheet, SheetName);
    int Row = 0;
    int Col = 0;

    // Head
    Xls.SetCell(Sheet, Row, Col++, "Quest Mid");
    Xls.SetCell(Sheet, Row, Col++, "Quest Sid");
    Xls.SetCell(Sheet, Row, Col++, "Quest Name");
    Xls.SetCell(Sheet, Row, Col++, "Quest DWORD");

    std::vector<DWORD> vQuestId;
    for (MAPQUEST_ITER iter=m_mapQuestMap.begin(); iter!=m_mapQuestMap.end(); ++iter)
        vQuestId.push_back(iter->first);

    std::sort(vQuestId.begin(), vQuestId.end());

    // Data
    for (size_t i=0; i<vQuestId.size(); ++i)
    {        
        SNATIVEID QuestId((DWORD) i);
        GLQUEST* pQuestData = Find((DWORD) i);
        if (pQuestData)
        {
			++Row;
			Col = 0;            
			Xls.SetCell(Sheet, Row, Col++, QuestId.wMainID);
			Xls.SetCell(Sheet, Row, Col++, QuestId.wSubID);
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", QuestId.wMainID, QuestId.wSubID);
				Xls.SetCell(Sheet, Row, Col++, CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString());
			}
			else
			{				
				Xls.SetCell(Sheet, Row, Col++, pQuestData->GetTitleStr());
			}
			Xls.SetCell(Sheet, Row, Col++, (int) QuestId.dwID);
        }
    }
}

HRESULT GLQuestMan::LoadStringTableExcel(const char* szPath)
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

		// QN_%03d_%03d, QD_%03d_%03d, QN_%03d_%03d_P_%d, QD_%03d_%03d_D_%d
		std::string strSperatorID[5];
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
		const SNATIVEID sNID(nMID, nSID);
		GLQUEST* pQuest = NULL;
		if ( (pQuest = this->Find(sNID.dwID)) == NULL )
		{
			std::string ErrMsg(
				sc::string::format(
				"QUEST Quest(%1%,%2%) not exist!"
				, nMID
				, nSID));
			AfxMessageBox(ErrMsg.c_str(), MB_OK); 
			continue;
		}

		// Step
		if ( nCount == 5 )
		{
			int nStep = atoi(strSperatorID[4].c_str());
			if ( pQuest->GetSTEPNUM() < (unsigned int)nStep )
			{
				std::string ErrMsg(
					sc::string::format(
					"QUEST %1% step %2% not exist!"
					, strId.c_str()
					, nStep));
				AfxMessageBox(ErrMsg.c_str(), MB_OK); 
				continue;
			}
		}

		CGameTextMan::GetInstance().InsertString(strId.c_str(), strValue.c_str(), CGameTextMan::EM_QUEST_TEXT);
	}
	return S_OK;
}

HRESULT GLQuestMan::SaveStringTableExcel(std::string& strSavePath)
{
	int sheet = 0, row = 0, col = 0;
	sc::Excel ExcelFile("QuestStringTable", strSavePath, sheet, false);
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

	MAPQUEST_ITER iter_pos;
	SNODE sNode;

	int nCount = 0;
	for ( iter_pos = m_mapQuestMap.begin(); iter_pos != m_mapQuestMap.end(); iter_pos++ )
	{
		sNode = (SNODE) iter_pos->second;
		const SNATIVEID sNID = sNode.pQUEST->m_sNID;

		CString strNameKey, strDescKey, strName, strDesc;
		strNameKey.Format("QN_%03d_%03d", sNID.Mid(), sNID.Sid());
		strDescKey.Format("QD_%03d_%03d", sNID.Mid(), sNID.Sid());

		strName = CGameTextMan::GetInstance().GetText(strNameKey.GetString(), 0, CGameTextMan::EM_QUEST_TEXT);
		strDesc = CGameTextMan::GetInstance().GetText(strDescKey.GetString(), 0, CGameTextMan::EM_QUEST_TEXT);

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

		// Quest Step
		for ( size_t i = 0; i < sNode.pQUEST->m_vecProgStep.size(); ++i )
		{
			CString strStepNameKey, strStepDescKey, strStepName, strStepDesc;
			strStepNameKey.Format("%s_P_%d", strNameKey, i);
			strStepDescKey.Format("%s_D_%d", strDescKey, i);

			strStepName = CGameTextMan::GetInstance().GetText(strStepNameKey.GetString(), 0, CGameTextMan::EM_QUEST_TEXT);
			strStepDesc = CGameTextMan::GetInstance().GetText(strStepDescKey.GetString(), 0, CGameTextMan::EM_QUEST_TEXT);

			// Step Name
			++row;
			col = 0;
			ExcelFile.SetCell(sheet, row, col++, nCount++);
			ExcelFile.SetCell(sheet, row, col++, strStepNameKey.GetString());
			ExcelFile.SetCell(sheet, row, col++, strTYPE);
			ExcelFile.SetCell(sheet, row, col++, 1);
			ExcelFile.SetCell(sheet, row, col++, 0);
			ExcelFile.SetCell(sheet, row, col++, 0);
			ExcelFile.SetCell(sheet, row, col++, 0);
			ExcelFile.SetCell(sheet, row, col++, nCountryNum);
			ExcelFile.SetCell(sheet, row, col++, 0);
			ExcelFile.SetCell(sheet, row, col++, strStepName.GetString());

			// Step Desc
			++row;
			col = 0;
			ExcelFile.SetCell(sheet, row, col++, nCount++);
			ExcelFile.SetCell(sheet, row, col++, strStepDescKey.GetString());
			ExcelFile.SetCell(sheet, row, col++, strTYPE);
			ExcelFile.SetCell(sheet, row, col++, 1);
			ExcelFile.SetCell(sheet, row, col++, 0);
			ExcelFile.SetCell(sheet, row, col++, 0);
			ExcelFile.SetCell(sheet, row, col++, 0);
			ExcelFile.SetCell(sheet, row, col++, nCountryNum);
			ExcelFile.SetCell(sheet, row, col++, 0);
			ExcelFile.SetCell(sheet, row, col++, strStepDesc.GetString());
		}
	}
	return ExcelFile.Save() == true ? S_OK : E_FAIL;
}