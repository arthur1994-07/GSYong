#pragma once

#include "../String/basestring.h"

namespace sc
{

class CStringMemory : public sc::CBaseString
{
public:
	CStringMemory(UINT nBufSize=BS_BUFF_SIZE);
	virtual ~CStringMemory();
	
public:
    BOOL Open(
        const std::string& ZipFileName, 
		LPCSTR _szFileName, 
		BOOL bDecode = FALSE, 
		BOOL bPack = FALSE);

	virtual void Close() override;

public:
	virtual void Reset() override;

public:
	virtual DWORD GetNextLine(LPSTR szLine,UINT iLineSize) override;
	virtual DWORD GetNextLine(PSTR& szString) override;
	virtual DWORD GetNextLine(CString& strString) override;
	virtual BYTE* GetText() override { return m_pBufferMem; }

protected:
	DWORD		m_dwOffSet; // ¸Þ¸ð¸® Offset;
	UINT		m_nBufSize;
	BYTE*		m_pBufferMem;
	UINT		m_nBufSizeTemp;
};

} // namespace sc