#include "pch.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/String/basestring.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Encrypt/CompByte.h"
#include "../SigmaCore/Encrypt/Rijndael.h"
#include "../SigmaCore/Loading/NSLoadingDirect.h"

#include "../EngineLib/DxTools/DebugSet.h"
#include "../EngineLib/G-Logic/GLOGIC.h"

#include "./GLogicData.h"
#include "./Item/GLItemDef.h"
#include "./Item/GLItemMan.h"
#include "./Skill/GLSkill.h"
#include "./Crow/GLCrowData.h"
#include "./Crow/GLCrowDataMan.h"
#include "./GLStringTable.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLStringTable::GLStringTable(void)
	: m_pLoadMap(NULL)
{
}

GLStringTable::~GLStringTable(void)
{
}

GLStringTable & GLStringTable::GetInstance()
{
	static GLStringTable Instance;
	return Instance;
}

void GLStringTable::CLEAR()
{
	if( !m_mapItemStringTable.empty() ) m_mapItemStringTable.clear();
	if( !m_mapSkillStringTable.empty() ) m_mapSkillStringTable.clear();
	if( !m_mapCrowStringTable.empty() ) m_mapCrowStringTable.clear();
}

void GLStringTable::SetCurrentMap( EMST_TYPE emType )
{
	switch ( emType )
	{
	case ITEM:	m_pLoadMap = &m_mapItemStringTable; break;
	case SKILL:	m_pLoadMap = &m_mapSkillStringTable; break;
	case CROW:	m_pLoadMap = &m_mapCrowStringTable; break;
	default:	GASSERT( FALSE ); break;
	}
}

GLStringTable::DESMAP * GLStringTable::GetCurrentMap()
{
	GASSERT( m_pLoadMap && "GLStringTable::GetCurrentMap() m_pLoadMap is NULL" );
	return m_pLoadMap;
}

bool GLStringTable::LOADFILE(const TCHAR * szFileName, EMST_TYPE emType, boost::function<void (std::string)> PercentCallbackFunc )
{
	if (!szFileName)
        return false;
	if (strlen(szFileName) == 0)
        return false;

	std::string strPath(GLOGIC::GetPath());
	strPath.append(szFileName);    

	std::auto_ptr<sc::CBaseString> pBString(
        GLOGIC::openfile_basestring(
            GLOGIC::UseLogicZipFile(), 
            GLogicData::GetInstance().GetGlogicZipFile(),
            strPath.c_str(),
            szFileName,
            true,
            GLOGIC::UseLogicPackFile()));

	if (!pBString.get())
	{
        sc::writeLogError(
            sc::string::format(
                "GLStringTable::LOADFILE(), File Open %1%", szFileName));
		return false;
	}

    DESMAP* pStringTable = NULL;
    switch (emType)
    {
    case ITEM:
        pStringTable = &m_mapItemStringTable;
        break;
    case SKILL:
        pStringTable = &m_mapSkillStringTable;
        break;
    case CROW:
        pStringTable = &m_mapCrowStringTable;
        break;
    default:
        GASSERT(FALSE);
        return false;
        break;
    }

	sc::CBaseString &cBString = *pBString;

    //std::string strKey;
    //std::string strSub;
	CString resToken, strBUFFER;
	CString strToken("\t");
    
    std::vector<std::string> vecKeyValue;
    vecKeyValue.reserve(2);

	int _iNextLine = 0;
	while ( (_iNextLine = cBString.GetNextLine(strBUFFER)) > 0)
	{
        vecKeyValue.clear();

        std::string StrLine(strBUFFER.SpanExcluding("/").GetString()); // 주석처리

        boost::split(vecKeyValue, StrLine, boost::algorithm::is_any_of("\t"), boost::algorithm::token_compress_on);
        if (vecKeyValue.size() == 0 ||  vecKeyValue[0].empty())
            continue;

        GASSERT(vecKeyValue.size() == 2);   //잘못된 string line(개행 문자를 포함한)의 경우, size가 1이 되어서 vecKeyValue[1] 연산에서 exception이 발생한다.

		// [shhan][2015.04.06] 배포는 Release 이므로 GASSERT 가 의미없어져 버린다.
		//						태국에서 Crash 옴. 타국은 이런게 없었음.
		if ( vecKeyValue.size() <= 1 )
			continue;

		const std::string strFind("\\r\\n"), strReplace("\r\n");
		CString strConvert(vecKeyValue[1].c_str());
		int nFind;
		while( ( nFind = strConvert.Find( strFind.c_str() ) ) != -1 )
		{
			strConvert.Delete( nFind, strFind.length() );
			strConvert.Insert( nFind, strReplace.c_str() );
		}

		if (!pStringTable->insert(DESMAP_VALUE(vecKeyValue[0], strConvert.GetString())).second)
        {
            sc::writeLogError(
                sc::string::format(
                    "String Table's ID %1% was duplicated.",
                    vecKeyValue[0]));
        }
		if ( PercentCallbackFunc ){
			TCHAR _szOutString[256];
			::wsprintf(_szOutString, TEXT("String table load ( Item Manager )\n( %d. )"), _iNextLine);

			PercentCallbackFunc(_szOutString);
		}

		NSLoadingDirect::FrameMoveRender(FALSE);
	}
    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender(FALSE);
    //---------------------------------------------------//
	return true;
}

const TCHAR * GLStringTable::GetString(const std::string& strKey, EMST_TYPE emType)
{
	//GASSERT( szKey && "GLStringTable::GetString() szKey is NULL" );
	static TCHAR szEmpty[] = {""};
	SetCurrentMap(emType);
	DESMAP* pMap = GetCurrentMap();
	if (!pMap)
        return &szEmpty[0];

	DESMAP_ITER Iter = pMap->find(strKey);
	if (Iter != pMap->end())
		return Iter->second.c_str();
    else
	    return	&szEmpty[0];
}

const std::string& GLStringTable::GetItemString(const std::string& strKey) const
{
    DESMAP_CITER Iter = m_mapItemStringTable.find(strKey);
    if (Iter != m_mapItemStringTable.end())
    {
        return Iter->second;
    }
    else
    {
        static std::string TempStr("");
        return TempStr;
    }
}

const std::string& GLStringTable::GetSkillString(const std::string& strKey) const
{
    DESMAP_CITER Iter = m_mapSkillStringTable.find(strKey);
    if (Iter != m_mapSkillStringTable.end())
    {
        return Iter->second;
    }
    else
    {
        static std::string TempStr("");
        return TempStr;
    }
}

const std::string& GLStringTable::GetCrowString(const std::string& strKey) const
{    
    DESMAP_CITER Iter = m_mapCrowStringTable.find(strKey);
    if (Iter != m_mapCrowStringTable.end())
    {
        return Iter->second;
    }
    else
    {
        static std::string TempStr("");
        return TempStr;
    }
}

void GLStringTable::InsertString( std::string & strKey, std::string & strSub, EMST_TYPE emType )
{
	SetCurrentMap( emType );

	//if( !m_mapStringTable.insert( std::make_pair( strKey, strSub ) ).second )
	//{
	//		return false;
	//}

    if( strKey.size() < 2 )
    {
        std::string ErrorMsg(
            sc::string::format(
            "Wrong Key. ( %1% , %2% ) was not inserted.",
            strKey,
            strSub));
        return ;
    }

	DESMAP* pMap = GetCurrentMap();
    (*pMap)[strKey] = strSub;
	//pMap->insert(DESMAP_VALUE(strKey, strSub));
}

void GLStringTable::DeleteString( std::string & strKeyName, std::string & strKeyDesc, EMST_TYPE emType )
{
	SetCurrentMap( emType );

	DESMAP * pMap = GetCurrentMap();
	pMap->erase( strKeyName );
	pMap->erase( strKeyDesc );
}

void GLStringTable::DeleteString( WORD wMID, WORD wSID, EMST_TYPE emType )
{
	TCHAR szName[ITEM_SZNAME] = {0};
	TCHAR szDesc[ITEM_SZCOMMENT] = {0};
	
	switch ( emType )
	{
		case ITEM:
			StringCchPrintf ( szName, ITEM_SZNAME, "IN_%03d_%03d", wMID, wSID );
			StringCchPrintf ( szDesc, ITEM_SZCOMMENT, "ID_%03d_%03d", wMID, wSID );
			break;

		case SKILL:
			StringCchPrintf ( szName, ITEM_SZNAME, "SN_%03d_%03d", wMID, wSID );
			StringCchPrintf ( szDesc, ITEM_SZCOMMENT, "SD_%03d_%03d", wMID, wSID );
			break;

		case CROW: 
			StringCchPrintf ( szName, ITEM_SZNAME, "CN_%03d_%03d", wMID, wSID );
			//StringCchPrintf ( szDesc, ITEM_SZCOMMENT, "CD_%03d_%03d", wMID, wSID );
			break;

		default:
			GASSERT( FALSE );
			break;
	}

	return DeleteString( std::string(szName), std::string(szDesc), emType );
}

bool GLStringTable::SAVEFILE( EMST_TYPE emType )
{
	TCHAR szFullPathFileName[MAX_PATH] = {0};
	StringCchCopy( szFullPathFileName, MAX_PATH, GLOGIC::GetPath() );

	switch ( emType )
	{
	case ITEM:	StringCchCat ( szFullPathFileName, MAX_PATH, GLItemMan::GetInstance()._STRINGTABLE ); break;
	case SKILL:	StringCchCat ( szFullPathFileName, MAX_PATH, GLSkillMan::GetInstance()._STRINGTABLE ); break;
	case CROW:	StringCchCat ( szFullPathFileName, MAX_PATH, GLCrowDataMan::_STRINGTABLE ); break;
	default:	GASSERT( FALSE ); break;
	}

	SetCurrentMap( emType );

    DESMAP_FOR_SAVE	m_mapNameForSave;
    DESMAP_FOR_SAVE	m_mapDescForSave;
	DESMAP_ITER iter, iter2;
	DESMAP_ITER iter_end, iter_end2;
    DESMAPSAVE_ITER iter_save, iter_save_end;
    DESMAPSAVE_ITER iter_save2, iter_save_end2;

	iter = GetCurrentMap()->begin();
	iter_end = GetCurrentMap()->end();


	for( ; iter != iter_end; ++iter )
	{
        if( iter->first.size() > 1 )
        {
            if( iter->first[1] == 'N')
                m_mapNameForSave.insert( std::make_pair( iter->first, iter->second ) );
            else
                m_mapDescForSave.insert( std::make_pair( iter->first, iter->second ) );
        }
        else
        {
            std::string ErrorMsg(
                sc::string::format(
                "Wrong Key. (%1%, %2%) was not saved.",
                iter->first,
                iter->second));
        }
	}

	CString strBuffer, strBuffer1;
	strBuffer = "// Note : 키(ID)와 내용은 반드시 탭으로 구분되어야 합니다.\r\n//\r\n";

    iter_save = m_mapNameForSave.begin();
    iter_save_end = m_mapNameForSave.end();
    iter_save2 = m_mapDescForSave.begin();
    iter_save_end2 = m_mapDescForSave.end();

	const std::string strFind("\r\n"), strReplace("\\r\\n");

    for( ; iter_save != iter_save_end; ++iter_save )
    {
        strBuffer1.Format( _T("%s\t%s\r\n"), iter_save->first.c_str(), iter_save->second.c_str() );
        strBuffer += strBuffer1;

        if( iter_save2 != iter_save_end2 )
        {
			CString strConvert(iter_save2->second.c_str());
			int nFind;
			while( ( nFind = strConvert.Find( strFind.c_str() ) ) != -1 )
			{
				strConvert.Delete( nFind, strFind.length() );
				strConvert.Insert( nFind, strReplace.c_str() );
			}


            strBuffer1.Format( _T("%s\t%s\r\n"), iter_save2->first.c_str(), strConvert.GetString() );
            strBuffer += strBuffer1;
            ++iter_save2;
        }
    }

    sc::CRijndael oRijndael;
    oRijndael.Initialize( sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::DEFAULT_KEY_LENGTH );

	int nLen = oRijndael.GetEncryptStringLength( strBuffer );
	char * szBuffer = new char[nLen];

	oRijndael.EncryptEx( strBuffer.GetString(), szBuffer, strBuffer.GetLength() );
    int nVersion = sc::CRijndael::VERSION;

	CFile file;
	file.Open( _T(szFullPathFileName), CFile::modeCreate|CFile::modeWrite );	// 파일 열기
	file.Write( &nVersion, sizeof(int) );							// 버젼 쓰기
	file.Write( szBuffer, nLen );									// 파일 쓰기
	file.Close();

	delete [] szBuffer;

	return true;
}