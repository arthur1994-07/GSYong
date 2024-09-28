#pragma once

#include "./GenItemTableDefine.h"

/*
 * GenItemTable.h
 * genItem ���ϵ鿡 ���� ��� ���� Ŭ����;
 * GenItemTool���� genItem�� ���� �˻��� ���� ����� ������,
 * �� ���� ��쿡�� ����ν� ������ �ʴ´�;
 * ���� genItem ���ϵ��� ���� �������� ������ ���鿡 ���� ���� ���� �ǰ� �ִ�;
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
	{ // specID �Ѱ��� ���� �ϴ� ������ ��� ������ ���� Ŭ����;
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