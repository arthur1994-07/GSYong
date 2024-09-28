#pragma once

#include <string.h>
#include "./GenItemTableDefine.h"
#include "./GenItemToolUtill.h"
#include "../GenItemTool/ExcelExportGenItem.h"

namespace GenItem
{
	class GenItemManager
	{
	public:
		static GenItemManager& GetInstance() //싱글톤 묶을 필요가 있음
		{		
			static GenItemManager g_GenItemManager;
			return g_GenItemManager;
		}
		~GenItemManager(void);	

	private:
		GenItemManager(void);	
		
	public:
		void LoadData(LoadCondition _Condition);
		void SaveData(GenItem::GenItemEditData &_data, std::string &_FileName);
		/********로드******************/
		bool FindData(const SearchData& _SearchData);
		bool FindDataCondition(const SearchData &_SearchData);
		bool FindCrow(const SearchData& _SearchData);
		bool FindGenItemFile(const SearchData& _SearchData);
		bool FindSpecID(const SearchData& _SearchData);
		bool FindItem(const SearchData& _SearchData);

		void FindLinkLostData(std::vector<LinkLostData*> &_LinkLostData);
		void FindDatafromWord(std::vector<std::string> &_Vec, const std::string &_Source, GenItem::FindFromWordCase _Case);

		void SeperateStringID(std::vector<SNATIVEID*> &_Vec, const std::string &_StrMID, const std::string &_StrSID, const std::string _StrSource);
		/********검색******************/		
		HRESULT SaveExcelFile(CWnd* _pWnd);
		HRESULT SaveExcelFile(CWnd* _pWnd, std::vector<LinkLostData*> &_LinkLostData, std::vector<int> &_NoUseSpecID);
		/****엑셀 파일로 저장*************/
		void LoadSelectFile();
		/********선택 파일만 데이터 갱신*********/
	public:
		void DeleteVecGenItemData();		
		void DeleverSearchData(std::vector<GenItem::GenItemData*> &_vecGenItemData);
		void DeleverNoUseSpecID(std::vector<int> &_vecNoUseSpecID);
		void FindGenItemFileData(std::string &_FileName, GenItemEditData &_GenItemEdit);
		void GetItemInfo(ItemInfo &_Info, std::string _Name, int _Mid, int _Sid);
		void GetItemInfo(ItemInfo &_Info, int _Index);
		void GetSpecIDInfo(SpecIDInfo* &_Info, int _SpecID);
		void DataChageGenInfo(SGENINFO& _Output, std::vector<std::string> &_annotation, GenItemEditData &_Tooldata);

	private:
		GenItemTool* m_InstanceGenItemTool;
		std::vector<GenItemData*> m_vecGenItemData;	
	};
}