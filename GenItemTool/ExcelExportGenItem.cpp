#include "StdAfx.h"
#include "ExcelExportGenItem.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "../RanLogic/GenItemTable/GenItemTableDefine.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/File/FileUtil.h"

ExcelExportGenItem::ExcelExportGenItem(void)
:FindDataSheetName("FindData_Excel"), LinkLostData_Item_Sheetname("LinkLostData_Item"),
LinkLostData_SpecID_SheetName("LinkLostData_SpecID"), LinkLostData_GenItemFile_SheetName("LinkLostData_GenItemFile"),
LinkLostData_Crow_SheetName("LinkLostData_Crow")
{
}

ExcelExportGenItem::~ExcelExportGenItem(void)
{
}

void ExcelExportGenItem::ExcelSaveGenItemTool(CWnd* _pWnd, std::vector<GenItem::GenItemData*> &_GenItem)
{	
	CString FileName("GenItemData");		
	std::string _File(FileName);	
	std::string SaveName("");
	sc::Excel _Xls(_File, SaveName, 1, true);

	_Xls.AddWorksheet(std::string(""));

	SaveHeadFindData(_Xls);
	SaveFindData(_GenItem, _Xls);	
}

void ExcelExportGenItem::ExcelSaveGenItemTool(CWnd* _pWnd, std::vector<GenItem::LinkLostData*> &_LinkLost, std::vector<int> &_NoUseSpecID)
{
	CString FileName("LinkLost_Data");
	std::string _File(FileName);	
	std::string SaveName("");	
	sc::Excel _Xls(_File, SaveName, 4, true);

	for(int i = 0; i < 4; ++i)
		_Xls.AddWorksheet(std::string(""));

	SaveHeadLinkLostData(_Xls);
	SaveLinkLostData(_LinkLost, _Xls);
	SaveNoUseSpecID(_NoUseSpecID, _Xls);
}

void ExcelExportGenItem::SaveHeadFindData(sc::Excel &_xls)
{
	int Col(0);
	int SheetNum(0);

	_xls.RenameWorksheet(SheetNum, FindDataSheetName);
	_xls.SetCell(SheetNum, 0, Col++, "SPECID");
	
	_xls.SetCell(SheetNum, 0, Col++, "ItemID wMainID");
	_xls.SetCell(SheetNum, 0, Col++, "ItemID wSubID");
	_xls.SetCell(SheetNum, 0, Col++, "ItemName");

	_xls.SetCell(SheetNum, 0, Col++, "CrowID wMainID");
	_xls.SetCell(SheetNum, 0, Col++, "CrowID wSubID");
	_xls.SetCell(SheetNum, 0, Col++, "CrowName");

	_xls.SetCell(SheetNum, 0, Col++, "GenItemFileName");

	_xls.SetCell(SheetNum, 0, Col++, "GenNum");

	_xls.SetCell(SheetNum, 0, Col++, "RateSpec(%)");
	_xls.SetCell(SheetNum, 0, Col++, "RateGen(%)");
	_xls.SetCell(SheetNum, 0, Col++, "RateTotal(%)");

	_xls.SetCell(SheetNum, 0, Col++, "SkillID wMainID");
	_xls.SetCell(SheetNum, 0, Col++, "SkillID wSubID");
}

void ExcelExportGenItem::SaveFindData(std::vector<GenItem::GenItemData*> &_Vec, sc::Excel &_xls)
{
	unsigned int _Size = _Vec.size();

	if(_Size == 0) return;

	int Col = 0;
	int _Row = 1;
	for(unsigned int _index = 0; _index < _Size; ++_index, ++_Row)
	{
		_xls.SetCell(0, _Row, Col++, _Vec[_index]->nSpecID);

		_xls.SetCell(0, _Row, Col++, _Vec[_index]->itemID.wMainID);
		_xls.SetCell(0, _Row, Col++, _Vec[_index]->itemID.wSubID);
		_xls.SetCell(0, _Row, Col++, _Vec[_index]->stringItemName);

		_xls.SetCell(0, _Row, Col++, _Vec[_index]->crowID.wMainID);
		_xls.SetCell(0, _Row, Col++, _Vec[_index]->crowID.wSubID);
		_xls.SetCell(0, _Row, Col++, _Vec[_index]->stringCrowName);

		_xls.SetCell(0, _Row, Col++, _Vec[_index]->stringGenItemFileName);

		_xls.SetCell(0, _Row, Col++, _Vec[_index]->sizeGenNum);
		
		_xls.SetCell(0, _Row, Col++, _Vec[_index]->rateSpec*100);		
		_xls.SetCell(0, _Row, Col++, _Vec[_index]->rateGen*100);		
		_xls.SetCell(0, _Row, Col++, _Vec[_index]->rateTotal*100);

		_xls.SetCell(0, _Row, Col++, _Vec[_index]->skillID.wMainID);
		_xls.SetCell(0, _Row, Col++, _Vec[_index]->skillID.wSubID);
		
		Col = 0;
	}
}

void ExcelExportGenItem::SaveHeadLinkLostData(sc::Excel &_xls)
{
	int Col(0);
	int SheetNum(0);

	_xls.RenameWorksheet(SheetNum, LinkLostData_Item_Sheetname);
	_xls.SetCell(SheetNum, 0, Col++, "Type");
	_xls.SetCell(SheetNum, 0, Col++, "Message");
	
	_xls.SetCell(SheetNum, 0, Col++, "ItemName");
	_xls.SetCell(SheetNum, 0, Col++, "ItemID wMainID");
	_xls.SetCell(SheetNum, 0, Col++, "ItemID wSubID");	

	SheetNum = 1;
	Col = 0;
	_xls.RenameWorksheet(SheetNum, LinkLostData_SpecID_SheetName);
	_xls.SetCell(SheetNum, 0, Col++, "Type");
	_xls.SetCell(SheetNum, 0, Col++, "Message");

	_xls.SetCell(SheetNum, 0, Col++, "SpecID");
	_xls.SetCell(SheetNum, 0, Col++, "ItemName");
	_xls.SetCell(SheetNum, 0, Col++, "ItemID wMainID");
	_xls.SetCell(SheetNum, 0, Col++, "ItemID wSubID");
	
	SheetNum = 2;
	Col = 0;
	_xls.RenameWorksheet(SheetNum, LinkLostData_GenItemFile_SheetName);
	_xls.SetCell(SheetNum, 0, Col++, "Type");
	_xls.SetCell(SheetNum, 0, Col++, "Message");
	_xls.SetCell(SheetNum, 0, Col++, "GenItemFileName");

	SheetNum = 3;
	Col = 0;
	_xls.RenameWorksheet(SheetNum, LinkLostData_Crow_SheetName);
	_xls.SetCell(SheetNum, 0, Col++, "Type");
	_xls.SetCell(SheetNum, 0, Col++, "Message");
	_xls.SetCell(SheetNum, 0, Col++, "CrowName");
	_xls.SetCell(SheetNum, 0, Col++, "CrowID wMainID");
	_xls.SetCell(SheetNum, 0, Col++, "CrowID wSubID");

	SheetNum = 4;
	Col = 0;
	_xls.RenameWorksheet(SheetNum, _T("NoUseSpecID"));
	_xls.SetCell(SheetNum, 0, Col++, "SpecID");
}

void ExcelExportGenItem::SaveLinkLostData(std::vector<GenItem::LinkLostData*> &_Vec, sc::Excel &_xls)
{
	unsigned int _Size = _Vec.size();

	if(_Size == 0) return;

	int Col = 0;
	int _Row1 = 1, _Row2 = 1, _Row3 = 1, _Row4 = 1;
	for(unsigned int _index = 0; _index < _Size; ++_index)
	{
		switch(_Vec[_index]->m_Type)
		{
			case GenItem::LinkLostData::ITEM_DATA:
				{
					_xls.SetCell(0, _Row1, Col++, _Vec[_index]->m_Type);
					_xls.SetCell(0, _Row1, Col++, _Vec[_index]->m_StateMessage);
					_xls.SetCell(0, _Row1, Col++, _Vec[_index]->m_ItemName);
					_xls.SetCell(0, _Row1, Col++, _Vec[_index]->m_ItemID.wMainID);
					_xls.SetCell(0, _Row1, Col++, _Vec[_index]->m_ItemID.wSubID);
					_Row1++;
					Col = 0;
					break;
				}

			case GenItem::LinkLostData::SPECID_DATA:
				{
					_xls.SetCell(1, _Row2, Col++, _Vec[_index]->m_Type);
					_xls.SetCell(1, _Row2, Col++, _Vec[_index]->m_StateMessage);
					_xls.SetCell(1, _Row2, Col++, _Vec[_index]->m_SpecID);
					_xls.SetCell(1, _Row2, Col++, _Vec[_index]->m_ItemName);
					_xls.SetCell(1, _Row2, Col++, _Vec[_index]->m_ItemID.wMainID);
					_xls.SetCell(1, _Row2, Col++, _Vec[_index]->m_ItemID.wSubID);
					_Row2++;
					Col = 0;
					break;
				}

			case GenItem::LinkLostData::GENITEMFILE_DATA:
				{
					_xls.SetCell(2, _Row3, Col++, _Vec[_index]->m_Type);
					_xls.SetCell(2, _Row3, Col++, _Vec[_index]->m_StateMessage);
					_xls.SetCell(2, _Row3, Col++, _Vec[_index]->m_GenItemFileName);
					_Row3++;
					Col = 0;
					break;
				}

			case GenItem::LinkLostData::CROW_DATA:
				{
					_xls.SetCell(3, _Row4, Col++, _Vec[_index]->m_Type);
					_xls.SetCell(3, _Row4, Col++, _Vec[_index]->m_StateMessage);
					_xls.SetCell(3, _Row4, Col++, _Vec[_index]->m_CrowName);
					_xls.SetCell(3, _Row4, Col++, _Vec[_index]->m_CrowID.wMainID);
					_xls.SetCell(3, _Row4, Col++, _Vec[_index]->m_CrowID.wSubID);
					_Row4++;
					Col = 0;
					break;
				}
		}
	}
}

void ExcelExportGenItem::SaveNoUseSpecID(std::vector<int> &_Vec, sc::Excel &_xls)
{
	unsigned int _Size = _Vec.size();

	if(_Size == 0) return;

	int Col(0);
	int _Row(1);

	for(unsigned int _index = 0; _index < _Size; ++_index)
	{
		_xls.SetCell(4, _Row, Col++, _Vec[_index]);
		++_Row;
		Col = 0;
	}
}