#include "pch.h"
#include "./PrivateResource.h"

#include "../../SigmaCore/String/StringUtils.h"

#include "EncryptDataDef.h"
#include "DebugSet.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPrivateResource::CPrivateResource( BYTE eType )
    : m_eType( eType )
    , m_pBuffer( NULL )
    , m_nSize( 0 )
{
    switch( m_eType )
    {
    case ETypeTexture:
        {
            m_nHeaderSize = TEX_HEADER_SIZE;
	        m_nVersion = TEX_VERSION;
	        m_nXorValue = TEX_XOR_DATA;
	        m_nDiffValue = TEX_DIFF_DATA;
			m_nDiffValue2 = 0;
        }
        break;

    case ETypeSkin:
        {
            m_nHeaderSize = SKINMESH_HEADER_SIZE;
	        m_nVersion = SKINMESH_VERSION;
	        m_nXorValue = SKINMESH_XOR_DATA;
	        m_nDiffValue = SKINMESH_DIFF_DATA;
			m_nDiffValue2 = 0;
        }
        break;

	case ETypeWidgetScript:
		{
			m_nHeaderSize = WIDGET_SCRIPT_HEADER_SIZE;
			m_nVersion = WIDGET_SCRIPT_VERSION;
			m_nXorValue = WIDGET_SCRIPT_XOR_DATA;
			m_nDiffValue = WIDGET_SCRIPT_DIFF_DATA;
			m_nDiffValue2 = WIDGET_SCRIPT_DIFF2_DATA;
		}
		break;
    }
}

CPrivateResource::~CPrivateResource()
{
    Reset();
}

void CPrivateResource::Reset( bool bDelete )
{
    if( !bDelete )
        m_pBuffer = NULL;

	if( m_pBuffer )
	{
		::free( m_pBuffer );
		m_pBuffer = NULL;
	}

	m_nSize = 0;
}

FILE* CPrivateResource::LoadFile_Decrypt_NewFile( const char* szFileName, const char* szNewFileName )
{
	Reset();

	bool bReturn = false;

	FILE* pFile = _fsopen( szFileName, "rb", _SH_DENYNO );
	if( pFile )
	{
		bReturn = Decrypt( pFile );
		fclose( pFile );
	}
	else
	{
		return NULL;
	}

	// D : 파일을 임시 파일로 지정합니다. 마지막 파일 포인터를 닫을 때 삭제됩니다. 
	// 허용되는 공유 유형입니다. : _SH_DENYRW : 파일에 대한 읽기 및 쓰기 권한을 거부합니다. 
	FILE* pTempFile = _fsopen( szNewFileName, "w+bD", _SH_DENYRW );
	if( !pTempFile )
	{
		return NULL;
	}

	fwrite( m_pBuffer, sizeof( BYTE ), m_nSize + 4, pTempFile );

	//fseek( pTempFile, 0, SEEK_SET );

	return pTempFile;
}

bool CPrivateResource::LoadFileDec( const char* szFileName )
{
	Reset();

    bool bReturn = false;

	FILE* pFile = _fsopen( szFileName, "rb", _SH_DENYNO );
	if( pFile )
    {
        bReturn = Decrypt( pFile );
        fclose( pFile );
    }

	return bReturn;
}

bool CPrivateResource::SaveFileDec( const char* szFileNameIn, const char* szFileNameOut, bool bDelete )
{
	Reset();

    bool bReturn = false;

	FILE* pFile = _fsopen( szFileNameIn, "rb", _SH_DENYNO );
	if( pFile )
    {
    	bReturn = DecryptToFile( pFile, szFileNameIn, szFileNameOut );
        fclose( pFile );
    }

    if( bDelete )
        DeleteFile( szFileNameIn );

	return bReturn;
}

bool CPrivateResource::SaveFileEnc( const char* szFileNameIn, const char* szFileNameOut, bool bDelete )
{
	Reset();

    bool bReturn = false;

	FILE* pFile = _fsopen( szFileNameIn, "rb", _SH_DENYNO );
	if( pFile )
    {
        bReturn = EncryptToFile( pFile, szFileNameIn, szFileNameOut );
        fclose( pFile );
    }

    if( bDelete )
        DeleteFile( szFileNameIn );

	return bReturn;
}

bool CPrivateResource::Decrypt( FILE* pFile )
{
	fseek( pFile, 0, SEEK_END );
	int nSize = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );

    int nVersion = 0;
    fread( &nVersion, sizeof( int ), 1, pFile );
    if( nVersion != m_nVersion )
        return false;

    // 최대 4바이트의 길이가 더 필요하다
    int nReadSize = 0;
    fread( &nReadSize, sizeof( int ), 1, pFile );
    if( nSize != nReadSize + m_nHeaderSize + 4 )
        return false;

    int nFileType = 0;
    fread( &nFileType, sizeof( int ), 1, pFile );

	m_nSize = nReadSize;

    return DecryptBuffer( pFile );
}

bool CPrivateResource::DecryptToFile( FILE* pFile, const char* szFileNameIn, const char* szFileNameOut )
{
    if( !Decrypt( pFile ) )
    {
        Reset();
        return false;
    }

    bool bReturn = false;

	FILE* pFileOutput = _fsopen( szFileNameOut, "wb", _SH_DENYNO );
	if( pFileOutput )
	{
        bReturn = true;
		fwrite( m_pBuffer, sizeof( BYTE ), m_nSize, pFileOutput );
        fclose( pFileOutput );
	}

	Reset();

	return bReturn;
}

bool CPrivateResource::EncryptToFile( FILE* pFile, const char* szFileNameIn, const char* szFileNameOut )
{
	fseek( pFile, 0, SEEK_END );
	m_nSize = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );

    // 최대 4바이트의 길이가 더 필요하다
    m_pBuffer = (BYTE*)::malloc( m_nHeaderSize + m_nSize + 4 );
	memset( m_pBuffer, 0, m_nHeaderSize + m_nSize + 4 );

	memcpy( m_pBuffer, &m_nVersion, sizeof( int ) );
	memcpy( m_pBuffer + 4 , &m_nSize, sizeof( int ) );

    // 이건 쓰지도 않으면서 왜 기록하는거야
    int nFileType = GetFileType( szFileNameIn );
    memcpy( m_pBuffer + 8, &nFileType, sizeof( int ) );	

	fread( m_pBuffer + m_nHeaderSize, sizeof( BYTE ), m_nSize, pFile );

	bool bReturn = EncryptBuffer();
	if( !bReturn )
	{
		Reset();
		return false;
	}

    bReturn = false;

	FILE* pFileOutput = _fsopen( szFileNameOut, "wb", _SH_DENYNO );
	if( pFileOutput )
	{
		bReturn = true;
        fwrite( m_pBuffer, sizeof( BYTE ), m_nHeaderSize + m_nSize + 4, pFileOutput );
        fclose( pFileOutput );
	}

	Reset();

    return bReturn;
}

bool CPrivateResource::DecryptBuffer( FILE* pFile )
{
	m_pBuffer = (BYTE*)::malloc( m_nSize + 4 );
    memset( m_pBuffer, 0, m_nSize + 4 );
	fread( m_pBuffer, sizeof( BYTE ), m_nSize + 4, pFile );

	if( !ENCRYPT_DATA::Decrypt( (char*)m_pBuffer, m_nSize + 4 ) )
	{
		Reset();
		return false;
	}

	// XOR변환
	for( int i=0; i<m_nSize; ++i )
	{
		m_pBuffer[ i ] += m_nDiffValue2;
		m_pBuffer[ i ] = m_pBuffer[ i ] ^ m_nXorValue;
		m_pBuffer[ i ] += m_nDiffValue;
	}

	return true;
}

bool CPrivateResource::EncryptBuffer()
{
	// XOR변환
	for( int i=0; i<m_nSize; ++i )
	{
		m_pBuffer[ m_nHeaderSize + i ] -= m_nDiffValue;
		m_pBuffer[ m_nHeaderSize + i ] = m_pBuffer[ m_nHeaderSize + i ] ^ m_nXorValue;
		m_pBuffer[ m_nHeaderSize + i ] -= m_nDiffValue2;
	}

	return ENCRYPT_DATA::Encrypt( (char*)m_pBuffer + m_nHeaderSize, m_nSize + 4 );
}

int CPrivateResource::GetFileType( const char* szFileName )
{
    CString strFileExt = STRUTIL::GetExtName( szFileName );

    switch( m_eType )
    {
    case ETypeTexture:
        {
            for( int i=0; i<TEXTURE_TYPE; ++i )
            {
	            if( strFileExt.CollateNoCase( ENCRYPT_DATA::g_szTexFileExt[ i ] ) == 0 )
                    return i;
            }
        }
        break;

    case ETypeSkin:
        {
            for( int i=0; i<SKINMESH_TYPE; ++i )
            {
	            if( strFileExt.CollateNoCase( ENCRYPT_DATA::g_szSkinFileExt[ i ] ) == 0 )
                    return i;
            }
        }
        break;

	default:
		return 0;
    }

    return 0;
}