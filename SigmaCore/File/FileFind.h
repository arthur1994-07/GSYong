//
//	[class CFileFindTree], (2002.12.07), JDH.
//
//	
//
#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <boost/function.hpp>
#include <map>
#include <vector>
#include <string>

namespace sc
{

class CFileFindTree
{
public:
    typedef std::tr1::unordered_map<std::string, std::string> FILEMAP;
    typedef FILEMAP::iterator		                          FILEMAP_ITER;
    typedef FILEMAP::const_iterator		                      FILEMAP_CITER;
    typedef FILEMAP::value_type		                          FILEMAP_VALUE;

protected:
    std::string		m_strPath;
    FILEMAP			m_mapFile;

public:
    CFileFindTree (void);
    ~CFileFindTree (void);

public:
    void Add ( std::string &strFile, std::string &strPath );
    const std::string& FindPathName ( const std::string &str ) const;
    void CreateTree ( const std::string &strPath, BOOL bRecurse=TRUE );
    const CFileFindTree::FILEMAP* GetFileMap();
	const void FindListInExt( std::vector<std::string>& vecListStringOUT, const std::string& strExt ) const;	// 같은 확장자라면, 그 File List를 넘겨준다.
	const void FindListFullName( const std::string &strPath, std::string &strName, const std::string& strExt ) const;	// 파일이 있다면, 확장자를 strExt 로 변경한다.
	void SomeWork( boost::function<void (const TCHAR*)> func ) const;																			// m_mapFile 를 외부에서 건드는게 별로..
	void SomeWorkSameExt( boost::function<void (const TCHAR*, const TCHAR*)> func, const TCHAR* pExt ) const;									// m_mapFile 를 외부에서 건드는게 별로..
	void SomeWorkSameExt( boost::function<void (DWORD_PTR, const TCHAR*, const TCHAR*)> func, DWORD_PTR pd3dDevice, const TCHAR* pExt ) const;	// m_mapFile 를 외부에서 건드는게 별로..

protected:
    void PathRecurse ( std::string &strDir, BOOL bRecurse );
    void CleanUp ();

public:
    const std::string& GetPath() const { return m_strPath; }
};

} // namespace sc