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
			std::map<int, SpecIDInfo*> m_MapSpecID; // <SpecID, SpecIDInfo ����ü>
			std::map<int, GenItemInfo*> m_MapGenItemInfo; // <�����̸��� ���̺� ����� index, GenItemInfo����ü>
			std::vector<CrowInfo> m_VecCrowInfo; 
			
			std::vector<std::string> m_VecItemName;    // �κй��� �˻������� ���
			std::vector<std::string> m_VecCrowName;   // �κй��� �˻������� ���	
		//Crow, Item�� ���� String ���̺��� ���⼭ �������� GenItem�� FileName�� ���� String ���̺��� GenItemFile�ε� �ÿ� �����
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
			/*****������ �������� ������(GenItem���ϵ� �߿��� Crow�� ������ ���� ���ϵ�)***********/
			std::vector<int> m_NoUseSpecID;
			/********������� �ʴ� SpecID**********************************************/

		public:	
			/******************�ε���********************/
			bool SetItemInfo(); //GLItemMan.h�� �ִ� m_ppItem�� �̿��ϴ°� �����
			//���� ���� SpecID�� dwMoney�� �����Ͱ� ����
			//
			bool SetCrowInfo(); //GLCrowDataman.h�� m_ppPCROWDATA�� �̿���
			bool SetGenItemInfo();
			bool SetSpecIDMap();
			bool SetGenItemToolInfo(); //����� �Լ�(���� �� �Լ��� ��ħ)

			template <typename T>
			bool DeleteMapSecond(std::map<int, T*> &_Map);
			/***************�̱��� �ε���*****************/

		public:
			void GetItemInfo(ItemInfo &_Info, std::string _Name);
			void GetItemInfo(ItemInfo &_Info, int _mid, int _sid);
			void GetItemInfo(ItemInfo &_Info, unsigned int _Index);
			void GetSpecIDInfo(SpecIDInfo* &_Info, int _SpecID);
			/*******************������ �������� ��*********/
		public:
			/***************�˻���***********************/
			int FindIndexFromGenFileName(std::string _FileName);

		public:
			/***************�˻���***********************/			
			bool SetDataStartCrow_NativeID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_CrowID);
			bool SetDataStrartCrow_MID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_CrowID);
			bool SetDataStartCrow_Name(std::vector<GenItemData*> *_Vec, const std::string _CrowName);
			bool SetDataStartGenItem(std::vector<GenItemData*> *_Vec, std::string _GenItemName);
			bool SetDataStartSpecID(std::vector<GenItemData*> *_Vec, const int _SpecID);			
			bool SetDataStartItem_NativeID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_ItemID);
			bool SetDataStartItem_MID(std::vector<GenItemData*> *_Vec, const SNATIVEID &_ItemID);
			bool SetDataStartItem_Name(std::vector<GenItemData*> *_Vec, const std::string _ItemName);
			//������ �˻������ ó�� �˻����� �� �� ������ ����
			void SetFindFromSearchWord(std::vector<GenItemData*> &_Vec, const SearchData* _SearchWord, Condition _Something);

			bool JudgeSame(SNATIVEID _ID, std::string _Name, SNATIVEID &_SameID, Condition _Something);
			/*************�κ� ���ڿ� �˻�************/
			void SetFindFromPartWord(std::vector<std::string> &_Vec, const std::string &_strSource, const GenItem::FindFromWordCase _Case);
		public:
			
			//GenItemFile�� Crow�� �Ȱ����� �ִ� ��쿡 ���� ���θ� �����͸� ������� �Ǻ��� ������� �˻��Լ� ����
			void CheckLinkItem(std::vector<LinkLostData*> &_LinkLostData);
			void CheckLinkSpecID(std::vector<LinkLostData*> &_LinkLostData);
			/************��Ʈ�� �����ؾ���*******************/
			void CheckGenItemUse(); 
			void CheckLinkGenitem(std::vector<LinkLostData*> &_LinkLostData);
			/*************�ȿ� �� �Լ�****************************/
			void CheckLinkCrow(std::vector<LinkLostData*> &_LinkLostData);				
			
			template<typename T>
			void DeleteVec(std::vector<T*> &_Vec);			

		public:
			/**********������ ���ϸ� �ٽ� �ε��ؼ� ������*************/
			void LoadSelectFile();

		public:
			int GetNoUseSpecID(int _index) { return m_NoUseSpecID[_index]; }
			int GetMaxNumofNoUseSpecID() { return m_NoUseSpecID.size(); }
			
	};
}