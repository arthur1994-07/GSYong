#include "pch.h"

#include "../../SigmaCore/Compress/ZipMan.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../Item/GLItemMan.h"

#include "./GenItemTable.h"

namespace GenItem
{
	HRESULT LoadFile(const std::string&, const bool _bPastLoad)
	{ // tool에서만 사용할 data이므로, tool이 아닌 경우에는 처리 하지 않는다;
		if ( _bPastLoad == false )
			return S_OK;

		const ZipMan::VECTOR_FILE_NAME* const _pListFileVector(ZipMan::GetInstance().getFileListByExtension("genitem"));
		if ( _pListFileVector == NULL )
			return E_FAIL;

		const ZipMan::VECTOR_FILE_NAME& _vectorListFile(*_pListFileVector);		

		CGENITEM _genItem;
		GenItem::Table& _genItemTable(GenItem::Table::getInstance());

		const std::string _stringPath(GLOGIC::GetPath());
		const DWORD _sizeGenItemFile(_vectorListFile.size());
		for ( DWORD _index(_sizeGenItemFile); _index--; )
		{
			const std::string _stringFileName(_vectorListFile[_index]);

			SGENINFO _genItemInfo;
			
			_genItem.GetGenInfo("", _stringFileName, _genItemInfo);
			_genItemTable.insert(_stringFileName, _genItemInfo);			
		}		

		return S_OK;
	}

	HRESULT SaveFile(SGENINFO &_Geninfo, std::string &_FileName, std::vector<std::string> &_Anotation)
	{
		CGENITEM _genItem;
		if(_genItem.SaveFile(_Geninfo, _FileName, _Anotation))
			return S_OK;
		else
			return S_FALSE;
	}

	ElementSpecID::ElementSpecID(void)
		: m_dwSpecID(0)
	{
	}
	ElementSpecID::ElementSpecID(const DWORD _dwSpecID)
		: m_dwSpecID(_dwSpecID)
	{
	}


	GenItem::Table& Table::getInstance(void)
	{
		static GenItem::Table _instance;
		return _instance;
	}
	Table::Table(void)
	{
		for ( DWORD _i(0); _i < GLItemMan::SPECID_NUM; ++_i )
			m_vectorSpecID.push_back(GenItem::ElementSpecID(_i));
	}
	Table::~Table(void)
	{
	}
	void Table::insert(const std::string& _stringFileName, const SGENINFO& _genItemInfo)
	{
		m_mapGenItem.insert(GenItemMapValue(_stringFileName, _genItemInfo));
		m_vecFileNameGenItem.push_back(_stringFileName); //GenItemFileName에 대한 String 테이블
	}

	GenItem::GenItemMapIterator Table::GetGenItemMapIterBegin()
	{
		return m_mapGenItem.begin();
	}

	GenItem::GenItemMapIterator Table::GetGenItemMapIterEnd()
	{
		return m_mapGenItem.end();
	}

	GenItem::GenItemMapIterator Table::GetGenItemMapIter(std::string _FileName)
	{			
		return m_mapGenItem.find(_FileName);
	}

	std::string Table::GetVecGenItemName(int _index)
	{
		return m_vecFileNameGenItem[_index];
	}

	unsigned int Table::GetVecGenItemNameSize()
	{
		return m_vecFileNameGenItem.size();
	}
	
}