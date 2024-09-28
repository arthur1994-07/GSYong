#include "StdAfx.h"
#include "DShowMemReader.h"

DShowMemStream::DShowMemStream(void)
:	m_pbData( NULL )
,	m_llLength( 0 )
,	m_llPosition( 0 )
,	m_dwKBPerSec( 0 )
{
}

DShowMemStream::DShowMemStream( LPBYTE pbData, LONGLONG llLength, DWORD dwKBPerSec )
:	m_pbData( pbData )
,	m_llLength( llLength )
,	m_llPosition( 0 )
,	m_dwKBPerSec( dwKBPerSec )
{
	m_dwTimeStart = timeGetTime();
}

DShowMemStream::~DShowMemStream(void)
{
}

void DShowMemStream::operator = ( const DShowMemStream& value )
{
	this->m_pbData = value.m_pbData;
	this->m_llLength = value.m_llLength;
	this->m_llPosition = value.m_llPosition;
	this->m_dwKBPerSec = value.m_dwKBPerSec;
	this->m_dwTimeStart = value.m_dwTimeStart;
}

HRESULT DShowMemStream::SetPointer( LONGLONG llPos )
{
	if( llPos < 0 || llPos > m_llLength )
	{
		return S_FALSE;
	}
	else
	{
		m_llPosition = llPos;
		return S_OK;
	}
}

HRESULT DShowMemStream::Read( PBYTE pbBuffer, DWORD dwBytesToRead, BOOL bAlign, LPDWORD pdwBytesRead )
{
	CAutoLock lck( &m_csLock );
	DWORD dwReadLength;

	/*  Wait until the bytes are here! */
	DWORD dwTime = timeGetTime();
	if( m_llPosition + dwBytesToRead > m_llLength )
		dwReadLength = (DWORD) ( m_llLength - m_llPosition );
	else
		dwReadLength = dwBytesToRead;

	DWORD dwTimeToArrive = ( (DWORD) m_llPosition + dwReadLength ) / m_dwKBPerSec;
	if( dwTime - m_dwTimeStart < dwTimeToArrive )
		Sleep( dwTimeToArrive - dwTime + m_dwTimeStart );

	CopyMemory( (PVOID) pbBuffer, (PVOID) ( m_pbData + m_llPosition ),
		dwReadLength);

	m_llPosition += dwReadLength;
	*pdwBytesRead = dwReadLength;

	return S_OK;
}

LONGLONG DShowMemStream::Size( LONGLONG *pSizeAvailable )
{
	LONGLONG llCurrentAvailable = Int32x32To64( ( timeGetTime() - m_dwTimeStart ), m_dwKBPerSec );

	*pSizeAvailable = min( m_llLength, llCurrentAvailable );
	return m_llLength;
}

//---------------------------------------------------------------------------------------------------
DShowMemReader::DShowMemReader( DShowMemStream* pStream, CMediaType* pmt, HRESULT* phr )
: CAsyncReader( NAME( "Mem Reader\0" ), NULL, pStream, phr )
{
	m_mt = *pmt;
}

DShowMemReader::~DShowMemReader(void)
{
}