
//  [5/12/2014 gbgim];
// - 임시 문자열 데이터를 기록하기 위한 클래스;
// - Quest Edit, NpcAction Tool에서 수정 완료 이전까지 변경된 문자열을;
//	 기록하기 위해 사용;
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