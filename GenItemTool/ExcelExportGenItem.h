#pragma once

#include "../RanLogic/GenItemTable/GenItemManager.h"
#include "../SigmaCore/File/Excel/MinExcel.h"

class ExcelExportGenItem
{
	private:
		ExcelExportGenItem(void);
	public:
		static ExcelExportGenItem& GenInstance()
		{
			static ExcelExportGenItem g_Instance;
			return g_Instance;
		}
		~ExcelExportGenItem(void);

	public:
		enum EXCELGENITEM
		{
			DATA_FIND = 0,

			DATA_LINKLOST = 1,
			DATA_LINKLOST_ITEM = 2,
			DATA_LINKLOST_SPECID = 3,
			DATA_LINKLOST_GENITEMFILE = 4,
			DATA_LINKLOST_CROW = 5,
			DATA_NONE = 6,
		};

	public: 
		void ExcelSaveGenItemTool(CWnd* _pWnd, std::vector<GenItem::GenItemData*> &_GenItem);	
		void ExcelSaveGenItemTool(CWnd* _pWnd, std::vector<GenItem::LinkLostData*> &_LinkLost, std::vector<int> &_NoUseSpecID);	

	public:
		void SaveHeadFindData(sc::Excel &_xls);
		void SaveHeadLinkLostData(sc::Excel &_xls);
		void SaveFindData(std::vector<GenItem::GenItemData*> &_Vec, sc::Excel &_xls);
		void SaveLinkLostData(std::vector<GenItem::LinkLostData*> &_Vec, sc::Excel &_xls);
		void SaveNoUseSpecID(std::vector<int> &_Vec, sc::Excel &_xls);
		


	private:
		const std::string FindDataSheetName;
		const std::string LinkLostData_Item_Sheetname;
		const std::string LinkLostData_SpecID_SheetName;
		const std::string LinkLostData_GenItemFile_SheetName;
		const std::string LinkLostData_Crow_SheetName;
};
