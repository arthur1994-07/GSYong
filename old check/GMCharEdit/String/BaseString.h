#pragma once

//#include "../Encrypt/Rijndael.h"

namespace sc
{

class CRijndael;

class CBaseString  
{
public:
    enum { BS_BUFF_SIZE = 4096, }; //! 버퍼를 제한해야 하는가...?

public:
    CBaseString();
    virtual ~CBaseString();

public:
    virtual void Close() = 0;

public:
    virtual void Reset() = 0;

public:
    virtual DWORD GetNextLine(LPSTR szLine,UINT iLineSize) = 0;
    virtual DWORD GetNextLine(PSTR& szString) = 0;
    virtual DWORD GetNextLine(CString& strString) = 0;
	virtual BYTE* GetText() = 0;

protected:
    UINT		m_nBufferSize;
    DWORD		m_dwRead;
    DWORD		m_dwLine;
    DWORD		m_dwIndex;
    BYTE*		m_pBuffer;
    BOOL		m_bDecode;
    INT			m_nVersion;
    char		m_szFileName[MAX_PATH];
    sc::CRijndael* m_pRijndael;
};

} // namespace sc