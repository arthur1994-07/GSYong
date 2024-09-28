#pragma once

#include "./GenItemTableDefine.h"

/*
 * GenItemTable.h
 * genItem 파일들에 대한 목록 관리 클래스;
 * GenItemTool에서 genItem에 대한 검색을 위해 만들어 졌으며,
 * 그 외의 경우에는 현재로썬 사용되지 않는다;
 * 현재 genItem 파일들은 실제 아이템을 떨구는 몹들에 의해 직접 관리 되고 있다;
 */

namespace GenItem
{
	extern HRESULT LoadFile(const std::string&, const bool _bPastLoad);	
	extern HRESULT SaveFile(SGENINFO &_Geninfo, std::string &_FileName, std::vector<std::string> &_Anotation);
	
	typedef std::map<std::string, SGENINFO> GenItemMap;
	typedef GenItemMap::const_iterator GenItemMapConstIterator;
	typedef GenItemMap::iterator GenItemMapIterator;
	typedef GenItemMap::value_type GenItemMapValue;


	typedef std::vector<SNATIVEID> ItemIDVector;
	typedef std::vector<std::string> GenItemFileNameVector;
	class ElementSpecID
	{ // specID 한개에 대응 하는 아이템 목록 정보를 가진 클래스;
	public:
		ElementSpecID(void);
		ElementSpecID(const DWORD _dwSpecID);

	private:
		DWORD m_dwSpecID;
		GenItem::ItemIDVector m_vectorItemID;
		GenItem::GenItemFileNameVector m_vectorGenItemFileName;
	};
	typedef std::vector<GenItem::ElementSpecID> SpecIDVector;
	typedef SpecIDVector::const_iterator SpecIDVectorConstIterator;
	typedef SpecIDVector::iterator SpecIDVectorIterator;

	class Table
	{
	public:
		void insert(const std::string& _stringFileName, const SGENINFO& _genItemInfo);
		GenItem::GenItemMapIterator GetGenItemMapIterBegin();
		GenItem::GenItemMapIterator GetGenItemMapIterEnd();
		std::string GetVecGenItemName(int _index);
		unsigned int GetVecGenItemNameSize();
		GenItem::GenItemMapIterator GetGenItemMapIter(std::string _FileName);

	public:
		static GenItem::Table& getInstance(void);
		~Table(void);

	private:
		Table(void);

	private:
		GenItem::GenItemFileNameVector m_vecFileNameGenItem;
		GenItem::GenItemMap m_mapGenItem;
		GenItem::SpecIDVector m_vectorSpecID;
	};
}

#include "./GenItemTable.hpp"