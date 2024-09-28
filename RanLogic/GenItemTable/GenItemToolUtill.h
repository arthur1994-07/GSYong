#pragma once

#include <string.h>

#include "./GenItemTableDefine.h"
#include "../Item/GLItemMan.h"
#include "../GLStringTable.h"
#include "../Crow/GLCrowDataMan.h"
#include "../../SigmaCore/String/StringUtil.h"

namespace GenItem
{
	class GenItemTool
	{
		public:
			enum Condition
			{
				CROW_ID = 250,
				CROW_NAME,
				GENITEMFILENAME,
				SPEC_ID,
				ITEM_ID,
				ITEM_NAME,		
			};
			GenItemTool(void);
			~GenItemTool(void);
		public:
			typedef std::map<int, SpecIDInfo*> SpecIDInfoMap;
			typedef SpecIDInfoMap::iterator SpecIDInfoMapIter;
			typedef std::map<int, GenItemInfo*> GenItemInfoMap;
			typedef GenItemInfoMap::iterator GenItemInfoMapIter;

		private:
			std::vector<ItemInfo> m_VecItemInfo; 
			std::map<int, SpecIDInfo*> m_MapSpecID; // <SpecID, SpecIDInfo 구조체>
			std::map<int, GenItemInfo*> m_MapGenItemInfo; // <파일이름이 테이블에 저장된 index, GenItemInfo구조체>
			std::vector<CrowInfo> m_VecCrowInfo; 
			
			std::vector<std::string> m_VecItemName;    // 부분문자 검색용으로 사용
			std::vector<std::string> m_VecCrowName;   // 부분문자 검색용으로 사용	
		//Crow, Item에 대한 String 테이블은 여기서 만들지만 GenItem의 FileName에 대한 String 테이블은 GenItemFile로딩 시에 만든다
		private:
			/******************************************/
			struct CheckLinkGenItem
			{
				bool m_Use;
				int  m_index;

				CheckLinkGenItem()
				:m_Use(false), m_index(0)
				{}

				CheckLinkGenItem(const CheckLinkGenItem& _CheckLinkGenItem)
				:m_Use(_CheckLinkGenItem.m_Use), m_index(_CheckLinkGenItem.m_index)
				{}
			};

			std::vector<CheckLinkGenItem> m_LinkLostGenitem; 			
			/*****조금은 이질적인 데이터(GenItem파일들 중에서 Crow가 가지지 않은 파일들)***********/
			std::vector<int> m_NoUseSpecID;
			/********사용하지 않는 SpecID**********************************************/

		public:	
			/******************로딩용********************/
			bool SetItemInfo(); //GLItemMan.h에 있는 m_ppItem을 이용하는걸 고려함
			//돈일 때는 SpecID에 dwMoney의 데이터가 있음
			//
			bool SetCrowInfo(); //GLCrowDataman.h에 m_ppPCROWDATA를 이용고려
			bool SetGenItemInfo();
			bool SetSpecIDMap();
			bool SetGenItemToolInfo(); //실행용 함수(위의 네 함수를 합침)

			template <typename T>
			bool DeleteMapSecond(std::map<int, T*> &_Map);
			/***************이까지 로딩용*****************/

		public:
			void GetItemInfo(ItemInfo &_Info, std::string _Name);
			void GetItemInfo(ItemInfo &_Info, int _mid, int _sid);
			void GetItemInfo(ItemInfo &_Info, unsigned int _Index);
			void GetSpecIDInfo(SpecIDInfo* &_Info, int _SpecID);
			/*******************데이터 가져가기 용*********/
		public:
			/***************검색용***********************/
			int FindIndexFromGenFileName(std::string _FileName);

		public:
			/***************검색용***********************/			
			bool SetDataStartCrow_NativeID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_CrowID);
			bool SetDataStrartCrow_MID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_CrowID);
			bool SetDataStartCrow_Name(std::vector<GenItemData*> *_Vec, const std::string _CrowName);
			bool SetDataStartGenItem(std::vector<GenItemData*> *_Vec, std::string _GenItemName);
			bool SetDataStartSpecID(std::vector<GenItemData*> *_Vec, const int _SpecID);			
			bool SetDataStartItem_NativeID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_ItemID);
			bool SetDataStartItem_MID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_ItemID);
			bool SetDataStartItem_Name(std::vector<GenItemData*> *_Vec, const std::string _ItemName);
			//각각의 검색어들을 처음 검색으로 할 때 데이터 정렬
			void SetFindFromSearchWord(std::vector<GenItemData*> &_Vec, const SearchData* _SearchWord, Condition _Something);

			bool JudgeSame(SNATIVEID _ID, std::string _Name, SNATIVEID &_SameID, Condition _Something);
			/*************부분 문자열 검색************/
			void SetFindFromPartWord(std::vector<std::string> &_Vec, const std::string &_strSource, const GenItem::FindFromWordCase _Case);
		public:
			
			//GenItemFile을 Crow가 안가지고 있는 경우에 대한 여부를 데이터만 가지고는 판별이 어려워서 검사함수 만듬
			void CheckLinkItem(std::vector<LinkLostData*> &_LinkLostData);
			void CheckLinkSpecID(std::vector<LinkLostData*> &_LinkLostData);
			/************세트로 실행해야함*******************/
			void CheckGenItemUse(); 
			void CheckLinkGenitem(std::vector<LinkLostData*> &_LinkLostData);
			/*************안에 두 함수****************************/
			void CheckLinkCrow(std::vector<LinkLostData*> &_LinkLostData);				
			
			template<typename T>
			void DeleteVec(std::vector<T*> &_Vec);			

		public:
			/**********선택한 파일만 다시 로딩해서 갱신함*************/
			void LoadSelectFile();

		public:
			int GetNoUseSpecID(int _index) { return m_NoUseSpecID[_index]; }
			int GetMaxNumofNoUseSpecID() { return m_NoUseSpecID.size(); }
			
	};
}