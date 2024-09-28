/**
 * \date	2011/10/11
 * \author  Jun-Hyuk Choi
 * \todo	파일 관리자가 없어서 임시로 사용함
 */
#pragma once

#include <string>
#include <boost/unordered_map.hpp>

#include "../../SigmaCore/Memory/SmartPtr.h"
#include "../../SigmaCore/Util/Lock.h"

class CFileSystem : public sc::DefaultLock
{
public:
    static CFileSystem& getInstance();

private:
    struct SData
    {
        CTypeSmartPtr m_pData;
        UINT m_nSize;
        UINT m_nRefCount;

        SData();
        SData( CTypeSmartPtr pData, UINT nSize );
    };

    typedef boost::unordered_map< std::string, SData > DataMap;
    typedef DataMap::value_type DataMapValue;
    typedef DataMap::iterator DataMapIter;

private:
    DataMap m_mapData;

private:
    CTypeSmartPtr loadFile( const std::string& strFileName, UINT& nFileSize );

public:
    CTypeSmartPtr getFile( const std::string& strFileName, UINT& nFileSize );
	CTypeSmartPtr getFile( const std::string& strFileName, INT& nFileSize );
    void releaseMemory( const std::string& strFileName );
    void releaseAll();
};