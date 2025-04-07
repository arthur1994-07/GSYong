// SerialFile.cpp: implementation of the SerialFile class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>
#include "../DebugSet.h"
#include "../Encrypt/CompByte.h"
#include "./SerialFile.h"

#include "../DebugInclude.h"

namespace sc
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SerialFile::SerialFile()
    : BaseStream()
    , m_FileStream(NULL)
{
}

SerialFile::SerialFile(FOPENTYPE _Type, const char *_szFileName, bool bencode)
    : BaseStream()
    , m_FileStream(NULL)
{
	MIN_ASSERT(_szFileName);
	OpenFile(_Type, _szFileName, bencode);
}

SerialFile::~SerialFile()
{
	CloseFile ();
}

void SerialFile::SetFileType( const char *szFileType, DWORD FileID )
{
	StringCchCopy( m_szFileType, FILETYPESIZE, szFileType );
	m_FileID = FileID;
}

void SerialFile::GetFileType( char *szFileType, int nStrLen, DWORD &FileID )
{
	StringCchCopy( szFileType, nStrLen, m_szFileType );
	FileID = m_FileID;
}

void SerialFile::ReadFileType ()
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)fread( &m_szFileType, sizeof( char ), FILETYPESIZE, m_FileStream );
	Num = (int)fread( &m_FileID, sizeof( DWORD ), 1, m_FileStream );

	m_DefaultOffSet = ftell ( m_FileStream );
}

void SerialFile::WriteFileType ()
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)fwrite( &m_szFileType, sizeof( char ), FILETYPESIZE, m_FileStream );
	Num = (int)fwrite( &m_FileID, sizeof( DWORD ), 1, m_FileStream );

	m_DefaultOffSet = ftell ( m_FileStream );
}

BOOL SerialFile::NewFile(const char* pName)
{
    /*
	int fh = _creat(pName, _S_IREAD | _S_IWRITE);
	if (fh == -1)
    {
        //perror( "Couldn't create data file" );
        return FALSE;
    }
	else
	{
		//printf( "Created data file.\n" );
		_close(fh);
		return TRUE;
	}
    */
    int  fh = 0;

    // Quit if can't open file or system doesn't support sharing.
    errno_t err = _sopen_s(&fh, pName, _O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);    
    if (err != 0)
        return FALSE;    
    _close(fh);
    return TRUE;
}

BOOL SerialFile::OpenFile ( const FOPENTYPE _Type, const char *_szFileName, bool bencode )
{
	MIN_ASSERT(_szFileName);

	m_OpenType = _Type;
	m_bencode = bencode;
	StringCchCopy( m_szFileName, _MAX_FNAME, _szFileName );    
    
	// Close file when it is already open.
	CloseFile ();

	switch ( m_OpenType )
	{
	case FOT_READ:
		m_FileStream = _fsopen ( m_szFileName, "rb", _SH_DENYNO );
		if ( !m_FileStream ) break;
		
		//if( m_bencode )
		//{
		//	int nVersion;
		//	fread ( &nVersion, sizeof(int), 1, m_FileStream );
        //	if( nVersion > 0 && sc::CRijndael::VERSION >= nVersion )
		//	{
		//		m_nVersion = nVersion;
        //		m_oRijndael.Initialize( sc::CRijndael::sm_Version[nVersion-1], sc::CRijndael::DEFAULT_KEY_LENGTH );
		//	}
		//	else
		//	{
		//		fseek ( m_FileStream, 0, SEEK_SET );
		//	}
		//}

		ReadFileType ();
		break;

	case FOT_WRITE:
		m_FileStream = _fsopen(m_szFileName, "wb", _SH_DENYNO);
		if (!m_FileStream)
		{
			if (NewFile(_szFileName)) // New File
			{
				m_FileStream = _fsopen(m_szFileName, "wb", _SH_DENYNO);
				if (!m_FileStream)
                    break;
			}
			else
                break; // New File Not Create
		}

        //int nVersion = sc::CRijndael::VERSION;
        //m_oRijndael.Initialize( sc::CRijndael::sm_Version[nVersion-1], sc::CRijndael::DEFAULT_KEY_LENGTH );

		//size_t wtcount = fwrite ( &nVersion, sizeof(int), 1, m_FileStream );
		//MIN_ASSERT(wtcount==1&&"SerialFile::OpenFile()");

		WriteFileType();
		break;
	};

	if (m_FileStream)
	{
		m_IsFileOpened = TRUE;
		return TRUE;
	}

	int nErrNo = errno;
	return FALSE;
}

void SerialFile::CloseFile ( BOOL bRemind )
{
	if ( m_FileStream )		fclose( m_FileStream );

	m_FileStream	= NULL;
	m_IsFileOpened	= FALSE;
	m_DefaultOffSet = 0;
}

// Note : SerialFile의 기본 파일 HEADER영역을 제외한 곳 부터.
//		0 - Position.
//
BOOL SerialFile::SetOffSet ( long _OffSet )
{
	if ( fseek ( m_FileStream, _OffSet + m_DefaultOffSet, SEEK_SET ) ) return FALSE;

	return TRUE;
}

//	Note : 파일의 m_DefaultOffSet를 제외한 현제 위치를 가져온다.
//
long SerialFile::GetfTell ()
{
	return ftell ( m_FileStream ) - m_DefaultOffSet;
}

DWORD SerialFile::GetFileSize()
{
	DWORD dwBufferOffset = GetfTell();

	fseek( m_FileStream, 0, SEEK_END );
	DWORD nFileSize = static_cast< UINT >( ftell( m_FileStream ) );
	SetOffSet ( dwBufferOffset );

	return nFileSize;
}

BOOL SerialFile::IsSameRemindLoadSize( int nSize )
{
	// nSize 가 4096 이상이면 사이즈 비교를 해도 소용이 없다.
	// 이런식으로 비교를 하면 안된다.
	// 현재는 이 파일이 유효한 파일인지 알기위해서 사용하도록 한다.
	return nSize >= 4096 ? TRUE : (m_FileStream->_cnt == nSize ? TRUE : FALSE);
}

BOOL SerialFile::BeginBlock ()
{
	MIN_ASSERT(m_dwBlockMarkPOS < EMBLOCK_SIZE && "SerialFile::BeginBlock");
	MIN_ASSERT(m_nMarkPos[m_dwBlockMarkPOS]==0 && "SerialFile::BeginBlock");

	m_nMarkPos[m_dwBlockMarkPOS] = GetfTell();	//	현재 위치 저장.
	*this << DWORD(-1);					//	MARK 저장.

	++m_dwBlockMarkPOS;

	return TRUE;
}

BOOL SerialFile::EndBlock ()
{
	--m_dwBlockMarkPOS;

	MIN_ASSERT(m_dwBlockMarkPOS < EMBLOCK_SIZE && "SerialFile::EndBlock");
	MIN_ASSERT(m_nMarkPos[m_dwBlockMarkPOS]!=0 && "SerialFile::EndBlock");

	long nEndPos = GetfTell();
	DWORD dwTotalSize = DWORD ( ( nEndPos - m_nMarkPos[m_dwBlockMarkPOS] ) - sizeof(DWORD) );

	SetOffSet ( m_nMarkPos[m_dwBlockMarkPOS] );
	*this << dwTotalSize;

	SetOffSet ( nEndPos );

	m_nMarkPos[m_dwBlockMarkPOS] = 0;

	return TRUE;
}

DWORD SerialFile::ReadBlockSize ()
{
	DWORD dwSize;
	*this >> dwSize;
	return dwSize;
}

inline size_t SerialFile::read ( void* pbuffer, DWORD dwSize )
{
	MIN_ASSERT(m_FileStream&&"SerialFile::read()");
	MIN_ASSERT(pbuffer&&"SerialFile::read()");
	
	size_t rdcount;

	// If dwSize is 0, the entire data is passed..
	if ( dwSize == 0 )
	{
		DWORD dwBufferOffset = GetfTell();
		fseek( m_FileStream, 0, SEEK_END );
		DWORD nFileSize = static_cast< UINT >( ftell( m_FileStream ) );
		fseek( m_FileStream, 0, SEEK_SET );
		rdcount = fread ( pbuffer, nFileSize, 1, m_FileStream );
		SetOffSet ( dwBufferOffset );
	}
	else
	{
		rdcount = fread ( pbuffer, dwSize, 1, m_FileStream );
	}

	if ( m_bencode )
	{
		compbyte::decode ( (BYTE*)pbuffer, dwSize );
	}

	//if ( m_bencode )
	//{
	//	//if( m_nVersion != -1 )
	//	//{
	//	//	int nLen = m_oRijndael.GetEncryptLength( dwSize );
	//	//	char * pBuffer = new char[nLen];
	//	//	memset( pBuffer, 0, nLen );

	//	//	rdcount = fread ( pBuffer, sizeof(char)*nLen, 1, m_FileStream );
	//	//	m_oRijndael.DecryptEx( pBuffer, nLen );

	//	//	memcpy( pbuffer, pBuffer, dwSize );
	//	//	delete [] pBuffer;
	//	//}
	//	//else
	//	{
	//		rdcount = fread ( pbuffer, dwSize, 1, m_FileStream );
	//		compbyte::decode ( (BYTE*)pbuffer, dwSize );
	//	}
	//}
	//else
	//{
	//	rdcount = fread ( pbuffer, dwSize, 1, m_FileStream );
	//}
    
    if ( rdcount!=1 )
	{
        std::string str = sc::string::format("file read error!\n file name: %1%\n dwSize: %2%", m_szFileName, dwSize);
        DebugSet::GetInstance()->LogToFile(str);
        ::MessageBoxA( NULL, str.c_str(), "File Read Error!", MB_ICONEXCLAMATION | MB_OK );
	}

	return rdcount;
}

inline size_t SerialFile::write ( const void* pbuffer, DWORD dwSize )
{
	MIN_ASSERT(m_FileStream&&"SerialFile::write()");
	MIN_ASSERT(pbuffer&&"SerialFile::write()");

	size_t wtcount(0);
	if ( m_bencode )
	{
		//int nLen = m_oRijndael.GetEncryptLength( dwSize );
		//char * szBuffer = new char[nLen];
		//memset( szBuffer, 0, nLen );
		//m_oRijndael.EncryptEx( (char*)pbuffer, szBuffer, dwSize );

		//wtcount = fwrite ( szBuffer, nLen, 1, m_FileStream );
		//MIN_ASSERT(wtcount==1&&"SerialFile::write()");

		//if ( wtcount!=1 )
		//{
		//	CDebugSet::ToLogFile ( "file write error : %s", m_szFileName );
		//}

		//delete [] szBuffer;

		BYTE *ptemp = new BYTE[dwSize];
		memcpy ( ptemp, pbuffer, sizeof(BYTE)*dwSize );
		compbyte::encode ( (BYTE*)ptemp, dwSize );


		wtcount = fwrite ( ptemp, dwSize, 1, m_FileStream );
		if ( wtcount!=1 )
		{
            std::string str = sc::string::format("file write error!\n file name: %1%\n dwSize: %2%", m_szFileName, dwSize);
            DebugSet::GetInstance()->LogToFile(str);
            ::MessageBoxA( NULL, str.c_str(), "File Write Error!", MB_ICONEXCLAMATION | MB_OK );
		}

		delete[] ptemp;
	}
	else
	{
		wtcount = fwrite ( pbuffer, dwSize, 1, m_FileStream );
        if ( wtcount!=1 )
        {
            std::string str = sc::string::format("file write error!\n file name: %1%\n dwSize: %2%", m_szFileName, dwSize);
            DebugSet::GetInstance()->LogToFile(str);
            ::MessageBoxA( NULL, str.c_str(), "File Write Error!", MB_ICONEXCLAMATION | MB_OK );
        }
	}

	return wtcount;
}

BOOL SerialFile::WriteBuffer ( const void *pBuffer, DWORD Size )
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	MIN_ASSERT(pBuffer);

	int Num = 0;

	Num = (int)write( pBuffer, Size );

	return TRUE;
}

BOOL SerialFile::ReadBuffer ( void* pBuffer, DWORD Size )
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	MIN_ASSERT(pBuffer);

	int Num = 0;

	Num = (int)read ( pBuffer, Size );

	return TRUE;
}

BOOL SerialFile::operator << (short Value)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	Num = (int) write(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator << (int Value)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	Num = (int) write(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator << (WORD Value)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	Num = (int) write(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator << (DWORD Value)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	Num = (int) write(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator << (UINT Value)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	Num = (int) write(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator << (float Value)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	Num = (int) write(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator << (double Value)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	Num = (int) write(&Value, sizeof(Value));
	return TRUE;
}


BOOL SerialFile::operator << (BYTE Value)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	Num = (int) write(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator << (char Value)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	Num = (int) write(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator << (bool Value)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	Num = (int) write(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator << (const std::string& str)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	*this << DWORD(str.length()+1);	
	Num = (int) write(str.c_str(), sizeof(char)*(DWORD(str.length())+1));
	return TRUE;
}

BOOL SerialFile::operator << (LONGLONG Value)
{
	MIN_ASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;
	Num = (int) write(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator << (const D3DVECTOR& Value)
{
    MIN_ASSERT(m_OpenType==FOT_WRITE);
    int Num = 0;
    Num = (int) write(&Value, sizeof(Value));
    return TRUE;
}

BOOL SerialFile::operator << (const D3DCOLORVALUE& Value)
{
    MIN_ASSERT(m_OpenType==FOT_WRITE);
    int Num = 0;
    Num = (int) write(&Value, sizeof(Value));
    return TRUE;
}

BOOL SerialFile::operator << (const D3DBLEND& Value)
{
    MIN_ASSERT(m_OpenType==FOT_WRITE);
    int Num = 0;
    Num = (int) write(&Value, sizeof(Value));
    return TRUE;
}

BOOL SerialFile::operator >> (short& Value)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;
	Value = FALSE;
	Num = (int) read(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator >> (int& Value)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;
	Value = FALSE;
	Num = (int) read(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator >> (WORD& Value)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;
	Value = FALSE;
	Num = (int) read(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator >> (DWORD& Value)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;
	Value = FALSE;
	Num = (int) read(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator >> (UINT &Value)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;
	Num = (int) read(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator >> (float& Value)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;
	Num = (int) read(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator >> (double& Value)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int) read(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator >> (BYTE& Value)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;
	Value = FALSE;
	Num = (int) read(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator >> (char& Value)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;
	Value = FALSE;
	Num = (int) read(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator >> (bool &Value)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;
	Value = FALSE;
	Num = (int) read(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator >> (std::string& str)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;
	DWORD dwSize;
	*this >> dwSize;	
	char *szBuffer = new char[dwSize];
	Num = (int) read(szBuffer, sizeof(char)*(dwSize));
	str = szBuffer;
	delete[] szBuffer;
	return TRUE;
}

BOOL SerialFile::operator >> (LONGLONG& Value)
{
	MIN_ASSERT(m_OpenType==FOT_READ);
	int Num = 0;
	Value = FALSE;
	Num = (int) read(&Value, sizeof(Value));
	return TRUE;
}

BOOL SerialFile::operator >> (D3DVECTOR& Value)
{
    MIN_ASSERT(m_OpenType==FOT_READ);
    int Num = 0;
    Num = (int) read(&Value, sizeof(Value));
    return TRUE;
}

BOOL SerialFile::operator >> (D3DCOLORVALUE& Value)
{
    MIN_ASSERT(m_OpenType==FOT_READ);
    int Num = 0;
    Num = (int) read(&Value, sizeof(Value));
    return TRUE;
}

BOOL SerialFile::operator >> (D3DBLEND &Value)
{
    MIN_ASSERT(m_OpenType==FOT_READ);
    int Num = 0;
    Num = (int) read(&Value, sizeof(Value));
    return TRUE;
}

} // namespace sc