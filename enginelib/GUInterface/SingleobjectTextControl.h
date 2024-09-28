
//  [5/12/2014 gbgim];
// - �ӽ� ���ڿ� �����͸� ����ϱ� ���� Ŭ����;
// - Quest Edit, NpcAction Tool���� ���� �Ϸ� �������� ����� ���ڿ���;
//	 ����ϱ� ���� ���;
#pragma once

#include <boost/function.hpp>
#include "../DxTools/DxFontDefine.h"
#include "./GameTextLoader.h"

class CSingleobjectTextControl
{
private:
	NS_TEXT_LOADER::STRFLAG		m_mapFlags[language::NATION_SIZE];

private:
	CSingleobjectTextControl();
	virtual ~CSingleobjectTextControl();

public:
	static CSingleobjectTextControl& GetInstance();

public:
	void InsertString( const std::string& strKey, const std::string& strDesc );
	void DeleteString( const std::string& strKey );
	const CString& GetText( const std::string& strKey );
	void Clear();	
};