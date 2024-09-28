#include "pch.h"
#include "./FileSystem.h"

#include "../../SigmaCore/String/StringUtil.h"

#include "../DxEffect/Single/DxEffSinglePropGMan.h"
#include "../DxMeshs/SkinMesh/DxSkinAnimationManager.h"
#include "../DxTools/PrivateResource.h"
#include "../G-Logic/GLogic.h"

#include "../../SigmaCore/DebugInclude.h"


CFileSystem& CFileSystem::getInstance()
{
    static CFileSystem g_cInstance;
	return g_cInstance;
}

CFileSystem::SData::SData()
    : m_nSize( 0 )
    , m_nRefCount( 1 )
{
}

CFileSystem::SData::SData( CTypeSmartPtr pData, UINT nSize )
    : m_pData( pData )
    , m_nSize( nSize )
    , m_nRefCount( 1 )
{
}

CTypeSmartPtr CFileSystem::loadFile( const std::string& strFileName, UINT& nFileSize )
{
    CTypeSmartPtr pRawData;

    std::string strFullPath = strFileName;
    CString strExt = sc::string::getFileExt( strFileName.c_str() );

    if( strExt.CompareNoCase( _T( "mtf" ) ) == 0 )
    {
        CPrivateResource cResource( CPrivateResource::ETypeTexture );
        if( cResource.LoadFileDec( strFileName.c_str() ) )
        {
            nFileSize = cResource.GetSize();
            pRawData = cResource.GetData();
            cResource.Reset( false );
        }
		else
		{
			CDebugSet::ToLogFile( sc::string::format( _T(" file open fail : %1%"), strFileName.c_str() ) );
		}
    }
    else if( strExt.CompareNoCase( _T( "mxf" ) ) == 0 )
    {
        CPrivateResource cResource( CPrivateResource::ETypeSkin );
        if( cResource.LoadFileDec( strFileName.c_str() ) )
        {
            nFileSize = cResource.GetSize();
            pRawData = cResource.GetData();
            cResource.Reset( false );
        }
		else
		{
			CDebugSet::ToLogFile( sc::string::format( _T(" file open fail : %1%"), strFileName.c_str() ) );
		}
    }
	else if( strExt.CompareNoCase( _T( "cfg" ) ) == 0 )
	{
		TCHAR szFilePath[ MAX_PATH ] = _T( "" );
		::StringCchCopy( szFilePath, MAX_PATH, DxSkinAnimationManager::GetInstance().GetPath().c_str() );
		::StringCchCat( szFilePath, MAX_PATH, strFileName.c_str() );

        strFullPath = szFilePath;

		std::auto_ptr< sc::BaseStream > pBStream( 
			GLOGIC::openfile_basestream(
				GLOGIC::bENGLIB_ZIPFILE, 
				GLOGIC::strANIMATION_ZIPFILE.c_str(),
				szFilePath, 
				strFileName.c_str() ) );

		if( pBStream.get() )
		{
			nFileSize = pBStream->GetFileSize();
			BYTE* pBuffer = (BYTE*)::malloc( nFileSize );
			pBStream->ReadBuffer( pBuffer, 0 );
			pRawData = pBuffer;
		}
		else
		{
			CDebugSet::ToLogFile( sc::string::format( _T(" file open fail : %1%"), strFileName.c_str() ) );
		}
	}
	else if( strExt.CompareNoCase( _T( "egp" ) ) == 0 )
	{
		TCHAR szFilePath[ MAX_PATH ] = _T( "" );
		::StringCchCopy( szFilePath, MAX_PATH, DxEffSinglePropGMan::GetInstance().GetPath().c_str() );
		::StringCchCat( szFilePath, MAX_PATH, strFileName.c_str() );

        strFullPath = szFilePath;

		std::auto_ptr< sc::BaseStream > pBStream( 
			GLOGIC::openfile_basestream(
				GLOGIC::bENGLIB_ZIPFILE, 
				GLOGIC::strEFFECT_ZIPFILE.c_str(),
				szFilePath, 
				strFileName.c_str() ) );

		if( pBStream.get() )
		{
			nFileSize = pBStream->GetFileSize();
			BYTE* pBuffer = (BYTE*)::malloc( nFileSize );
			pBStream->ReadBuffer( pBuffer, 0 );
			pRawData = pBuffer;
		}
		else
		{
			CDebugSet::ToLogFile( sc::string::format( _T(" file open fail : %1%"), strFileName.c_str() ) );
		}
	}
    else
    {
        FILE* pFile = _fsopen( strFileName.c_str(), "rb", _SH_DENYNO );
	    if( pFile )
        {
            fseek( pFile, 0, SEEK_END );
	        nFileSize = static_cast< UINT >( ftell( pFile ) );
            fseek( pFile, 0, SEEK_SET );

            if( nFileSize > 0 )
            {
                BYTE* pBuffer = (BYTE*)::malloc( nFileSize );
                fread( pBuffer, sizeof( BYTE ), nFileSize, pFile );
                pRawData = pBuffer;
            }

            fclose( pFile );
        }
		else
		{
			CDebugSet::ToLogFile( sc::string::format( _T(" file open fail : %1%"), strFileName.c_str() ) );
		}
    }

    return pRawData;
}

CTypeSmartPtr CFileSystem::getFile( const std::string& strFileName, UINT& nFileSize )
{
    if( strFileName.empty() )
        return CTypeSmartPtr();

    lockOn();    

    DataMapIter it = m_mapData.find( strFileName );
    if( it != m_mapData.end() )
    {
        SData& sData = it->second;

        CTypeSmartPtr pPtr = sData.m_pData;
        nFileSize = sData.m_nSize;
        ++sData.m_nRefCount;

        lockOff();

        return pPtr;
    }

	lockOff();

	// 이곳을 Lock 하면 느리다.
    CTypeSmartPtr pRawData = loadFile( strFileName, nFileSize );

	lockOn();    

    if( pRawData != NULL )
        m_mapData.insert( DataMapValue( strFileName, SData( pRawData, nFileSize ) ) );

    lockOff();

    return pRawData;
}

CTypeSmartPtr CFileSystem::getFile( const std::string& strFileName, INT& nFileSize )
{
	UINT nFileSizeTemp( nFileSize );
	CTypeSmartPtr pRawData = getFile( strFileName, nFileSizeTemp );
	nFileSize = nFileSizeTemp;

	return pRawData;
}

void CFileSystem::releaseMemory( const std::string& strFileName )
{
    if( strFileName.empty() )
        return;

    lockOn();

    DataMapIter it = m_mapData.find( strFileName );
    if( it != m_mapData.end() )
    {
        SData& sData = it->second;

        if( --sData.m_nRefCount < 1 )
            m_mapData.erase( it );
    }

    lockOff();
}

void CFileSystem::releaseAll()
{
    lockOn();

    m_mapData.clear();

    lockOff();
}