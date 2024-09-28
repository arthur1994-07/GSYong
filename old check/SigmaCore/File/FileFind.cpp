#include "stdafx.h"
#include <boost/foreach.hpp>


#include "../String/StringUtils.h"
#include "../String/StringUtil.h"
#include "./FileFind.h"


#include "../DebugInclude.h"


namespace sc
{

CFileFindTree::CFileFindTree (void)
{
}

CFileFindTree::~CFileFindTree (void)
{
}

void CFileFindTree::Add ( std::string &strFile, std::string &strPath )
{
    m_mapFile[strFile] = strPath;
}

const CFileFindTree::FILEMAP* CFileFindTree::GetFileMap()
{
    return &m_mapFile;
}

const std::string& CFileFindTree::FindPathName ( const std::string &str ) const
{
    FILEMAP_CITER iter = m_mapFile.find( str );
	if ( iter != m_mapFile.end() ) {
		return iter->second;
	} else {
		static std::string FindPathNameNull( _T( "" ) );
		return FindPathNameNull;
	}
}

const void CFileFindTree::FindListInExt( std::vector<std::string>& vecListStringOUT, const std::string& strExt ) const
{
	vecListStringOUT.clear();

	FILEMAP_CITER iter = m_mapFile.begin();
	for ( ; iter!=m_mapFile.end(); ++iter )
	{
		if ( sc::string::getFileExt( (*iter).first.c_str() ) == strExt.c_str() )
		{
			vecListStringOUT.push_back( (*iter).first );
		}
	}
}

// 파일이 있다면, 확장자를 strExt 로 변경한다.
const void CFileFindTree::FindListFullName( const std::string &strPath, std::string &strName, const std::string& strExt ) const
{
	FILEMAP_CITER iter = m_mapFile.begin();
	for ( ; iter!=m_mapFile.end(); ++iter )
	{
		if ( (*iter).second == std::string(strPath+strName).c_str() )
		{
			CString strDest;
			STRUTIL::ChangeExt( strName.c_str(), strDest, strExt.c_str() );

			strName = strDest.GetString();
		}
	}
}

void CFileFindTree::PathRecurse ( std::string &strDir, BOOL bRecurse )
{
    CFileFind finder;

    //	build a string with wildcards
    std::string strWildcard ( strDir );
    strWildcard += _T("\\*.*");

    //	start working for files
    BOOL bWorking = finder.FindFile ( strWildcard.c_str() );

    while ( bWorking )
    {
        bWorking = finder.FindNextFile ();

        //	skip . and .. files; otherwise, we'd
        //	recur infinitely!
        if ( finder.IsDots() ) continue;

        //	if it's a directory, recursively search it
        if ( finder.IsDirectory() && bRecurse )
        {
            std::string strPath = finder.GetFilePath();
            PathRecurse ( strPath, TRUE );
        }
        else
        {
            std::string strName(finder.GetFileName().GetString());
            sc::string::lowerString(strName);

            std::string strPath(finder.GetFilePath().GetString());

            m_mapFile.insert(FILEMAP_VALUE(strName,strPath));
        }
    }

    finder.Close();
}

void CFileFindTree::CreateTree ( const std::string &strPath, BOOL bRecurse )
{
    m_strPath = strPath;

    CleanUp ();

    //	Note : 해당 폴더를 모두 탐색하며 파일을 검색하여 트리에 삽입.
    //
    PathRecurse ( m_strPath, bRecurse );
}

void CFileFindTree::CleanUp ()
{
    m_mapFile.clear ();
}

// FindInverseScale 에서 사용.
void CFileFindTree::SomeWork( boost::function<void (const TCHAR*)> func ) const
{
	BOOST_FOREACH( const FILEMAP::value_type& iter, m_mapFile )
	{
		func( iter.first.c_str() );
	}
}

// FindDontHaveTexture 에서 사용.
void CFileFindTree::SomeWorkSameExt( boost::function<void (const TCHAR*, const TCHAR*)> func, const TCHAR* pExt ) const
{
	BOOST_FOREACH( const FILEMAP::value_type& iter, m_mapFile )
	{
		func( iter.first.c_str(), pExt );
	}
}

// FindDontHaveTexture 에서 사용.
void CFileFindTree::SomeWorkSameExt( boost::function<void (DWORD_PTR, const TCHAR*, const TCHAR*)> func, DWORD_PTR pd3dDevice, const TCHAR* pExt ) const
{
	BOOST_FOREACH( const FILEMAP::value_type& iter, m_mapFile )
	{
		func( pd3dDevice, iter.first.c_str(), pExt );
	}
}

} // namespace sc