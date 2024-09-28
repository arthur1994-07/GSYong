
#include "pch.h"
#include "../../RanLogic/RANPARAM.h"
#include "./GameTextControl.h"

#include "SingleobjectTextControl.h"

CSingleobjectTextControl::CSingleobjectTextControl()
{

}

CSingleobjectTextControl::~CSingleobjectTextControl()
{
	Clear();
}

CSingleobjectTextControl& CSingleobjectTextControl::GetInstance()
{
	static CSingleobjectTextControl s_NtkControl;
	return s_NtkControl;
}

void CSingleobjectTextControl::InsertString( const std::string& strKey, const std::string& strDesc )
{
	NS_TEXT_LOADER::STRFLAG* hmMapFlag = &m_mapFlags[RANPARAM::emProvideLangFlag];
	CStringArray* pStrArr;

	if ( (*hmMapFlag)[strKey] == NULL )
	{
		pStrArr = new CStringArray();
		(*hmMapFlag)[strKey] = pStrArr;
	}
	else
	{
		pStrArr = (*hmMapFlag)[strKey];
	}

	pStrArr->RemoveAll();
	pStrArr->Add(strDesc.c_str());
}

void CSingleobjectTextControl::DeleteString( const std::string& strKey )
{
	NS_TEXT_LOADER::STRFLAG* hmMapFlag = &m_mapFlags[RANPARAM::emProvideLangFlag];
	CStringArray* pStrArr;

	if ( hmMapFlag == NULL )
		return ;
	else
	{
		CStringArray* pStrArr = (*hmMapFlag)[strKey];
		pStrArr->RemoveAll();
	}
}

const CString& CSingleobjectTextControl::GetText( const std::string& strKey )
{
	return NS_TEXT_LOADER::GetText(strKey, 0, m_mapFlags[RANPARAM::emProvideLangFlag]);
}

void CSingleobjectTextControl::Clear()
{
	NS_TEXT_LOADER::ClearText(m_mapFlags);
}