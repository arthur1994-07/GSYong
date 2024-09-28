#pragma once

#include "./Async/asyncrdr.h"
#include "./Async/asyncflt.h"

class DShowMemStream : public CAsyncStream
{
public:
	DShowMemStream(void);
	DShowMemStream( LPBYTE pbData, LONGLONG llLength, DWORD dwKBPerSec = INFINITE );
	~DShowMemStream(void);

private :
	CCritSec	m_csLock;
	PBYTE		m_pbData;
	LONGLONG	m_llLength;
	LONGLONG	m_llPosition;
	DWORD		m_dwKBPerSec;
	DWORD		m_dwTimeStart;

public :
	HRESULT SetPointer( LONGLONG llPos );
	HRESULT Read( PBYTE pbBuffer, DWORD dwBytesToRead, BOOL bAlign, LPDWORD pdwBytesRead );
	LONGLONG Size( LONGLONG* pSizeAvailable );

	DWORD Alignment() { return 1; }
	void Lock() { m_csLock.Lock(); }
	void Unlock() { m_csLock.Unlock(); }

public :
	void operator = ( const DShowMemStream& value );
};

//---------------------------------------------------------------------------------------------------
class DShowMemReader : public CAsyncReader
{
public :
	DShowMemReader( DShowMemStream* pStream, CMediaType* pmt, HRESULT* phr );
	~DShowMemReader(void);

public :
	STDMETHODIMP Register() { return S_OK; }
	STDMETHODIMP Unregister() { return S_OK; }
};