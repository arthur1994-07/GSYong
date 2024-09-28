// StringFile.cpp: implementation of the CStringFile class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "../Encrypt/Rijndael.h"
#include "../gassert.h"
#include "../Encrypt/CompByte.h"
#include "./StringFile.h"


#include "../DebugInclude.h"


namespace sc
{

CStringFile::CStringFile(UINT nBufSize)
    : m_dwMasterIndex(0)
    , m_fFile(NULL)
	, m_bChangeBufferSize(false)
{
	m_dwRead = nBufSize;
	m_nBufferSize = nBufSize;
	m_pBuffer = new BYTE[nBufSize];
}

CStringFile::~CStringFile ()
{
	delete[] m_pBuffer;
	Close ();
}

//BOOL CStringFile::OpenAddPastVer ( LPCSTR szFile, BOOL bDecode )
//{
//	GASSERT(m_fFile==NULL&&"CStringFile::Open ()");
//
//	m_fFile = fopen ( szFile, "rb" );
//	if ( m_fFile==NULL )	return FALSE;
//
//	fseek ( m_fFile, 0, SEEK_END );
//	m_nMaxSize = ftell ( m_fFile );
//
//	fseek ( m_fFile, 0, SEEK_SET );
//
//	m_bDecode = bDecode;
//	StringCchCopy( m_szFileName, MAX_PATH, szFile );
//
//	if( m_bDecode )
//	{
//		int nVersion;
//		fread ( &nVersion, sizeof(int), 1, m_fFile );
//		if( nVersion > 0 && sc::CRijndael::VERSION >= nVersion )
//		{
//			m_nVersion = nVersion;
//			m_oRijndael.Initialize( sc::CRijndael::sm_Version[nVersion-1], sc::CRijndael::DEFAULT_KEY_LENGTH );
//		}
//		else
//		{
//			fseek ( m_fFile, 0, SEEK_SET );
//		}
//	}
//	return TRUE;
//}

BOOL CStringFile::Open ( LPCSTR szFile, BOOL bDecode )
{
	MIN_ASSERT(m_fFile==NULL&&"CStringFile::Open ()");

	m_fFile = _fsopen ( szFile, "rb", _SH_DENYNO );
	if ( m_fFile==NULL )	return FALSE;

	fseek ( m_fFile, 0, SEEK_END );
	m_nMaxSize = ftell ( m_fFile );

	fseek ( m_fFile, 0, SEEK_SET );

	m_bDecode = bDecode;
	StringCchCopy( m_szFileName, MAX_PATH, szFile );

	char fname[_MAX_FNAME]={0,};
	char ext[_MAX_EXT]={0,};
	_splitpath_s( szFile, NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT );

	bool bPastLoad = FALSE;
	if( _stricmp( fname, "param" ) == 0 )
	{
		bPastLoad = TRUE;
	}

	if( m_bDecode )
	{
		int nVersion;
		fread ( &nVersion, sizeof(int), 1, m_fFile );
        if( nVersion > 0 && sc::CRijndael::VERSION >= nVersion )
		{
			m_nVersion = nVersion;
            m_pRijndael->Initialize(
                sc::CRijndael::sm_Version[m_nVersion-1].c_str(),
                sc::CRijndael::sm_KeyLength[m_nVersion-1], 
                sc::CRijndael::sm_chain0,
                sc::CRijndael::sm_KeyLength[m_nVersion-1]);
		}
		else
		{
			fseek ( m_fFile, 0, SEEK_SET );
			return FALSE;
		}
	}

	return TRUE;
}

void CStringFile::Close ()
{
	if( m_fFile )
	{
		fclose ( m_fFile );
		m_fFile = NULL;
	} // if( m_fFile )

	//m_dwIndex = 0;
	//m_dwLine = 0;

	m_dwMasterIndex = 0;
	m_dwIndex = 0;
	m_dwLine = 0;
}

DWORD CStringFile::GetNextLine(CString &strString)
{
	if (!m_fFile)
        return 0;

	char* szString = strString.GetBuffer(m_nBufferSize);
	DWORD dwReturn = this->GetNextLine(szString, m_nBufferSize);
	if (dwReturn == 0)
        strString.Empty();
	strString.ReleaseBuffer();
	return dwReturn;
}

DWORD CStringFile::GetNextLine ( PSTR &szString )
{
	if ( m_fFile==NULL )	return 0;

	SAFE_DELETE(szString);

	szString = new CHAR[m_nBufferSize];
	DWORD dwReturn = this->GetNextLine ( szString, m_nBufferSize );

	SAFE_DELETE(szString);
	
	return dwReturn;
}

DWORD CStringFile::GetNextLine ( LPSTR szLine, UINT iLineSize )
{
	if ( m_fFile==NULL )	return 0;

	BYTE	*chTemp;
	BOOL	bStop=FALSE;
	int		nOut;

	chTemp = (LPBYTE) szLine;
	*chTemp = 0;
	nOut = 0;

	while ( !bStop )
	{
		if ( !m_dwLine || m_dwIndex==m_dwRead )
		{
			m_dwMasterIndex = ftell ( m_fFile );
			
			if( m_bDecode )
			{
				if( m_nVersion != -1 )
				{
					int nLen = m_pRijndael->GetEncryptLength( m_nBufferSize );
                    if( static_cast< DWORD >( nLen ) > m_dwRead )
                    {
                        delete [] m_pBuffer;
                        m_pBuffer = new BYTE[nLen];
                        m_nBufferSize = nLen;
                    }

					m_dwRead = (DWORD)fread ( m_pBuffer, sizeof(BYTE), nLen, m_fFile );

					if( m_dwRead > 0 )
						m_pRijndael->DecryptEx( (char*)m_pBuffer, nLen );
				}
				else
				{
					if ( m_bChangeBufferSize )
					{
						SAFE_DELETE_ARRAY( m_pBuffer );
						m_pBuffer = new BYTE[ m_nBufferSize ];

						m_bChangeBufferSize = false;
					}

					m_dwRead = (DWORD)fread ( m_pBuffer, sizeof(BYTE), m_nBufferSize, m_fFile );

					if( m_dwRead > 0 )
						compbyte::decode ( m_pBuffer, sizeof(BYTE)*m_dwRead );
				}
			}
			else
			{
				if ( m_bChangeBufferSize )
				{
					SAFE_DELETE_ARRAY( m_pBuffer );
					m_pBuffer = new BYTE[ m_nBufferSize ];

					m_bChangeBufferSize = false;
				}

				m_dwRead = (DWORD)fread ( m_pBuffer, sizeof(BYTE), m_nBufferSize, m_fFile );
			}

			m_dwIndex = 0;
			if ( m_dwRead == 0 )
			{
				bStop = TRUE; // Error during readfile or END-OF-FILE encountered

				if ( nOut>0 )
				{
					chTemp[nOut++] = (char) 0;
					return m_dwLine;	
				}
				else return m_dwLine = 0; //nix gelezen
			}
			else
			{
				if ( m_dwRead != (DWORD)m_nBufferSize )
					bStop = TRUE;	//END-OF-FILE
			}
		}

		for ( ; m_dwIndex<m_dwRead; m_dwIndex++ )
		{
			if ( (nOut+1) == iLineSize )
			{
				TCHAR szError[260] = {0};
				StringCchPrintf( szError, 260, "CStringFile::m_pBuffer Not enough. File:%s, Line:%u(Over Char Number:%d)",
								m_szFileName, ++m_dwLine, iLineSize );
				::MessageBox ( NULL, szError, "Warning !", MB_ICONEXCLAMATION|MB_OK );
				chTemp[nOut] = '\0';
				return m_dwLine;
			}

			switch ( m_pBuffer[m_dwIndex] )
			{
			case 0x0d://End of Line encountered
			case 0x0a:
				if ( (m_dwIndex+1) < m_dwRead ) // Check we're not on end of m_pBuffer ???
				{
					if ( m_pBuffer[m_dwIndex+1] == '\n' || m_pBuffer[m_dwIndex+1] == '\r' )
					{
						if ( !*chTemp )	m_dwLine++;
						m_dwIndex++;
					}
				}

				if ( *chTemp )
				{
					chTemp[nOut++] = '\0';
					m_dwLine++;

					return m_dwLine;
				}

				break;

			default:
				chTemp[nOut++] = m_pBuffer[m_dwIndex];
				break;
			};
		}
	}

	if ( nOut > 0 )
	{
		chTemp[nOut++] = '\0';
		
		if ( m_dwLine==0 )	m_dwLine++;

		return m_dwLine;
	}

	return m_dwLine = 0; //nix gelezen
}

BYTE* CStringFile::GetText()
{
	return m_pBuffer;
}

void CStringFile::ChangeBufferSize ( UINT _nBufferSize )
{
	m_nBufferSize = _nBufferSize;

	m_bChangeBufferSize = true;
}

void CStringFile::Reset ()
{
	if ( m_fFile==NULL )	return;

	m_dwIndex = 0;
	m_dwLine = 0;

	fseek ( m_fFile, SEEK_SET, 0 );
}

} // namespace sc