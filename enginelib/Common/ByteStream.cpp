#include "pch.h"
#include "./ByteStream.h"

//#pragma warning(disable:4996)

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace se {

ByteStream::ByteStream( const DWORD dwReserve )
    : m_dwIter(0)
{
	m_Buffer.reserve( dwReserve );
}

ByteStream::ByteStream(const LPBYTE pBuff, const DWORD dwSize)
    : m_dwIter(0)
{
    WriteBuffer(pBuff, dwSize);
}

ByteStream::ByteStream(const std::vector<BYTE>& vBuffer)
	: m_dwIter(0)
{
    WriteBufferVector(vBuffer);
}

ByteStream::~ByteStream()
{
}

BOOL ByteStream::operator << (short Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (int Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (WORD Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (DWORD Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (UINT Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (float Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (double Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (const D3DVECTOR& Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (const D3DCOLORVALUE& Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (const D3DBLEND& Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (BYTE Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (char Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (wchar_t Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (bool Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (LONGLONG Value)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(&Value), LPBYTE(&Value) + sizeof(Value));
    return TRUE;
}

BOOL ByteStream::operator << (const std::string& str)
{
    *this << DWORD(str.length()+1);
    m_Buffer.insert(m_Buffer.end(), LPBYTE(str.c_str()), LPBYTE(str.c_str()) + sizeof(char)*(str.length()+1));
    return TRUE;
}

BOOL ByteStream::operator << (const std::wstring& str)
{
    *this << DWORD(str.length()+1);
    m_Buffer.insert(m_Buffer.end(), LPBYTE(str.c_str()), LPBYTE(str.c_str()) + sizeof(wchar_t)*(str.length()+1));
    return TRUE;
}

BOOL ByteStream::WriteBuffer(const void* pBuffer, DWORD Size)
{
    m_Buffer.insert(m_Buffer.end(), LPBYTE(pBuffer), LPBYTE(pBuffer)+Size);
    return TRUE;
}

//! 주의:이미 존재하는 버퍼를 덮어쓴다. 뒷쪽에 더해지는 것이 아니다.
BOOL ByteStream::WriteBufferVector(const std::vector<BYTE>& vBuffer)
{
    m_Buffer = vBuffer;
    return TRUE;
}

BOOL ByteStream::operator >> (short& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);    
    //std::copy(&m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value));
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(short);
    return TRUE;
}

BOOL ByteStream::operator >> (int& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(int)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(int);
    return TRUE;
}

BOOL ByteStream::operator >> (WORD& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (DWORD& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy(&m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value));
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (UINT& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (float& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (double& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (D3DVECTOR& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (D3DCOLORVALUE& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (D3DBLEND& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (BYTE& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (char& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (wchar_t& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (bool& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value) );
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (LONGLONG& Value)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(Value) <= sizeBuff);

    //std::copy(&m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(Value)], LPBYTE(&Value));
    memcpy(&Value, &m_Buffer[m_dwIter], sizeof(Value));
    m_dwIter += sizeof(Value);
    return TRUE;
}

BOOL ByteStream::operator >> (std::string& str)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    DWORD dwSize;
    *this >> dwSize;

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT(sizeof(char)*dwSize <= DWORD(sizeBuff));

    char* szBuffer = new char[dwSize];
    //std::copy(&m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(char)*dwSize], LPBYTE(szBuffer));
    memcpy(szBuffer, &m_Buffer[m_dwIter], sizeof(char)*dwSize);
    str = szBuffer;
    delete[] szBuffer;
    m_dwIter += sizeof(char)*dwSize;
    return TRUE;
}

BOOL ByteStream::operator >> (std::wstring& str)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    DWORD dwSize;
    *this >> dwSize;

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    GASSERT( sizeof(wchar_t) * dwSize <= DWORD(sizeBuff));

    wchar_t* szBuffer = new wchar_t[dwSize];
    memcpy(szBuffer, &m_Buffer[m_dwIter], sizeof(wchar_t)*dwSize);
    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+sizeof(wchar_t)*dwSize], LPBYTE(szBuffer) );
    str = szBuffer;
    delete[] szBuffer;
    m_dwIter += sizeof(wchar_t)*dwSize;
    return TRUE;
}

//template<typename TYPE>
//BOOL ByteStream::operator << (const std::vector<TYPE>& vecVALUE)
//{
//    BOOL bOK(FALSE);
//    bOK = operator << ( (DWORD)vecVALUE.size() );
//    if ( !bOK )					return FALSE;
//
//    if ( vecVALUE.empty() )		return TRUE;
//    return WriteBuffer ( &(vecVALUE[0]), DWORD(sizeof(TYPE)*vecVALUE.size()) );
//}
//
//template<typename TYPE>
//BOOL ByteStream::operator >> ( std::vector<TYPE> &vecVALUE )
//{
//    BOOL bOK(FALSE);
//    DWORD dwSize(0);
//
//    vecVALUE.clear();
//
//    bOK = operator >> ( dwSize );
//    if ( !bOK )					return FALSE;
//
//    if ( dwSize==0 )			return TRUE;
//
//    vecVALUE.reserve(dwSize);
//    for ( DWORD i=0; i<dwSize; ++i )
//    {
//        TYPE tVALUE;
//        bOK = ReadBuffer ( &tVALUE, DWORD(sizeof(TYPE)) );
//        if ( !bOK )				return FALSE;
//
//        vecVALUE.push_back ( tVALUE );
//    }
//
//    return TRUE;
//}

BOOL ByteStream::ReadBuffer ( void* pBuffer, DWORD Size )
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));

    if ( (int)Size > sizeBuff )
		return FALSE;	//ERROR

    GASSERT ( int(Size)<=sizeBuff );

	if (Size <= 0)	// mjeon.migration
		return FALSE;	//occured during getting skill data to SCHARDATA.

	memcpy_s( pBuffer,  sizeof(BYTE)*Size, &m_Buffer[m_dwIter], sizeof(BYTE)*Size );
    //std::copy ( &m_Buffer[m_dwIter], &m_Buffer[m_dwIter+Size], (LPBYTE)pBuffer );

    m_dwIter += Size;

    return TRUE;
}

void ByteStream::GetBuffer ( LPBYTE &pBuff, DWORD &dwSize )
{
    pBuff = &m_Buffer[0];
    dwSize = (DWORD) m_Buffer.size();
}

BOOL ByteStream::SetOffSet(long _OffSet)
{
	if ( m_Buffer.size() <= m_dwIter )
	{
		return FALSE;
	}

    int sizeBuff = (int)(m_Buffer.end() - (m_Buffer.begin()+m_dwIter));
    if (_OffSet > sizeBuff)
        return FALSE;
    m_dwIter = _OffSet;
    return TRUE;
}

long ByteStream::GetfTell()
{
    return m_dwIter;
}

void ByteStream::ClearBuffer()
{
    m_Buffer.clear(); 
    m_dwIter=0;
}

void ByteStream::ReadFileType()
{
    ::memcpy( m_szFileType, &m_Buffer[m_dwIter], sizeof(TCHAR)*FILETYPESIZE );
    m_dwIter += sizeof(TCHAR)*FILETYPESIZE;

    ::memcpy( &m_FileID, &m_Buffer[m_dwIter], sizeof(DWORD) );
    m_dwIter += sizeof(DWORD);

    m_DefaultOffSet = m_dwIter;
}

void ByteStream::GetFileType(TCHAR *szFileType, int nStrLen, DWORD& FileID)
{
    ::StringCchCopy( szFileType, nStrLen, m_szFileType );
    FileID = m_FileID;
}

} // namespace se