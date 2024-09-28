#include "stdafx.h"
#include "../Encrypt/CompByte.h"
#include "../Compress/Unzipper.h"
#include "../Compress/ZipMan.h"
#include "./SFileSystem.h"
#include "./SerialMemory.h"


#include "../DebugInclude.h"

namespace sc
{

SerialMemory::SerialMemory(void) 
	: BaseStream()
    , m_pBuffer(NULL)
	, m_nBufSize(0)
	, m_dwOffSet(0)
{
}

SerialMemory::SerialMemory(const char *_szZipFileName, const char *_szFileName, bool bencode) 
	: BaseStream()
    , m_pBuffer(NULL)
	, m_nBufSize(0)
	, m_dwOffSet(0)
{
	MIN_ASSERT(_szFileName);
	MIN_ASSERT(_szZipFileName);

    OpenFile(std::string(_szZipFileName), std::string(_szFileName), bencode);
}

SerialMemory::~SerialMemory(void)
{
	CloseFile ();
}

void SerialMemory::GetFileType ( char *szFileType, int nStrLen, DWORD &FileID )
{
	StringCchCopy( szFileType, nStrLen, m_szFileType );
	FileID = m_FileID;
}

void SerialMemory::CloseFile ( BOOL bRemind )
{
	if ( m_pBuffer )
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	} // if ( m_pBuffer )

	m_DefaultOffSet = 0;
	m_nBufSize = 0;
	m_dwOffSet = 0;
	m_IsFileOpened = FALSE;
	m_FileID = 0;
}

BOOL SerialMemory::OpenFile(
    const std::string& ZipFileName, 
    const std::string& FileName, 
    bool bencode, 
    BOOL bPack)
{
	if (FileName.empty() || ZipFileName.empty())
        return FALSE;

	// 이미 열려 있을때 파일 닫음.
	if (IsOpen())
        CloseFile();

	if (bPack)
	{
        std::string strPath(ZipFileName);
		strPath += FileName;

		SFileHandle * pFileHandle = sc::file::g_sFileSystem.OpenFile(strPath.c_str());
		if (pFileHandle)
		{
			FILECONTEXT * pFileContext = pFileHandle->GetFileContext();
			if( pFileContext )
			{
				BYTE buffer[4096]={0};
				DWORD dwRead;

				m_nBufSize = 0;
				m_pBuffer = new BYTE[pFileContext->lUncompressSize];

				do
				{
					dwRead = pFileHandle->Read( buffer, 4096 );
					
					memcpy( m_pBuffer+m_nBufSize, buffer, dwRead );
					m_nBufSize += dwRead;
				}
				while(dwRead > 0);
			}

			sc::file::g_sFileSystem.CloseFile( pFileHandle );
		}
	}
	else
	{
		//CUnzipper cUnzip;
		//m_nBufSize = cUnzip.UnzipToMemory(ZipFileName, FileName, m_pBuffer);
        m_nBufSize = ZipMan::GetInstance().UnzipToMemory(ZipFileName, FileName, m_pBuffer);
	}

	// note :	파일 사이즈가 너무 커도 UINT_MAX가 넘어 옴. 약 17메가.
	//			파일을 1이라도 읽었다는걸 확신할수 있나?
	if( m_nBufSize == UINT_MAX || m_nBufSize == 0)
		return FALSE;

	m_OpenType = FOT_READ;
	m_bencode = bencode;
	StringCchCopy(m_szFileName, _MAX_FNAME, FileName.c_str());
	StringCchCopy(m_szZipFileName, _MAX_PATH, ZipFileName.c_str());

	//if( m_bencode )
	//{
	//	int nVersion;
	//	memcpy( &nVersion, m_pBuffer, sizeof(int) );
    //	if( nVersion > 0 && sc::CRijndael::VERSION >= nVersion )
	//	{
	//		m_nVersion = nVersion;
    //		m_oRijndael.Initialize( sc::CRijndael::sm_Version[nVersion-1], sc::CRijndael::DEFAULT_KEY_LENGTH );

	//		m_nBufSize -= sizeof(int);
	//		memcpy( m_pBuffer, m_pBuffer+sizeof(int), m_nBufSize );
	//		
	//		m_oRijndael.DecryptEx( (char*)m_pBuffer, m_nBufSize );
	//	}
	//}

	ReadFileType ();

	m_IsFileOpened = TRUE;
	return TRUE;
}

void SerialMemory::ReadFileType ()
{
	int Num = 0;

	Num = (int)read( &m_szFileType, sizeof( char )*FILETYPESIZE );
	Num = (int)read( &m_FileID, sizeof( DWORD ) );

	m_DefaultOffSet = m_dwOffSet;
}

DWORD SerialMemory::ReadBlockSize ()
{
	DWORD dwSize;
	*this >> dwSize;
	return dwSize;
}

inline size_t SerialMemory::read ( void* pbuffer, DWORD dwSize )
{
	MIN_ASSERT(m_nBufSize&&"SerialMemory::read()");
	MIN_ASSERT(pbuffer&&"SerialMemory::read()");

	// dwSize가 0일 경우 전체를 넘겨준다.
	if ( dwSize == 0 )
	{
		memcpy( pbuffer, m_pBuffer, m_nBufSize );
	}
	else
	{
		if ( (m_dwOffSet+dwSize) > m_nBufSize )		return 0;

		memcpy( pbuffer, m_pBuffer + m_dwOffSet, dwSize );
		m_dwOffSet += dwSize;
	}

	if ( m_bencode )
	{
		//if( m_nVersion == -1 )
		//{
			compbyte::decode ( (BYTE*)pbuffer, dwSize );
		//}
	}

	// 별 의미없는 값
	return 1;
}

BOOL SerialMemory::ReadBuffer ( void* pBuffer, DWORD Size )
{
	MIN_ASSERT(pBuffer);

	int Num = 0;

	Num = (int)read ( pBuffer, Size );

	return TRUE;
}

// Note : SerialFile의 기본 파일 HEADER영역을 제외한 곳 부터.
//		0 - Position.
//
BOOL SerialMemory::SetOffSet ( long _OffSet )
{
	m_dwOffSet = _OffSet + m_DefaultOffSet;
	// always false
    //if( m_dwOffSet < 0 )
	//	return FALSE;	
	return TRUE;
}

//	Note : 파일의 m_DefaultOffSet를 제외한 현제 위치를 가져온다.
//
long SerialMemory::GetfTell ()
{
	return m_dwOffSet - m_DefaultOffSet;
}

BOOL SerialMemory::BeginBlock ()
{
	//if ( emBLOCK > EMBLOCK_SIZE )	return FALSE;

	MIN_ASSERT(m_nMarkPos[m_dwBlockMarkPOS]==0&&"SerialMemory::BeginBlock");

	m_nMarkPos[m_dwBlockMarkPOS] = GetfTell();	//	현재 위치 저장.
	*this << DWORD(-1);					//	MARK 저장.

	++m_dwBlockMarkPOS;

	return TRUE;
}

BOOL SerialMemory::EndBlock ()
{
	//if ( emBLOCK > EMBLOCK_SIZE )	return FALSE;

	--m_dwBlockMarkPOS;

	MIN_ASSERT(m_nMarkPos[m_dwBlockMarkPOS]!=0&&"SerialMemory::EndBlock");

	long nEndPos = GetfTell();
	DWORD dwTotalSize = DWORD ( ( nEndPos - m_nMarkPos[m_dwBlockMarkPOS] ) - sizeof(DWORD) );

	SetOffSet ( m_nMarkPos[m_dwBlockMarkPOS] );
	*this << dwTotalSize;

	SetOffSet ( nEndPos );

	m_nMarkPos[m_dwBlockMarkPOS] = 0;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// 수정 안해도 되는 함수

BOOL SerialMemory::operator >> ( short &Value )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemory::operator >> ( int &Value )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemory::operator >> ( WORD &Value )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemory::operator >> ( DWORD &Value )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemory::operator >> ( UINT &Value )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemory::operator >> ( float &Value )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemory::operator >> ( double &Value )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemory::operator >> ( BYTE &Value )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemory::operator >> ( char &Value )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemory::operator >> ( bool &Value )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemory::operator >> ( std::string &str )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	DWORD dwSize;
	*this >> dwSize;
	
	char *szBuffer = new char[dwSize];
	Num = (int)read ( szBuffer, sizeof(char)*(dwSize) );

	str = szBuffer;
	delete[] szBuffer;

	return TRUE;
}

BOOL SerialMemory::operator >> ( LONGLONG &Value )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

} // namespace sc