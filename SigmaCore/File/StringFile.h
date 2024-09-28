//
//	[class CStringFile], (2002.11.26), JDH.
//
//	UPDATE : (2002.12.07), JDH, 메소드 호출시에 파일이 열리지 않았을때 예외 처리를
//			하여 오류 발생을 방지.
//
//
#pragma once

#include "../String/basestring.h"

namespace sc
{

class CStringFile : public sc::CBaseString
{
public:
	CStringFile(UINT nBufSize = BS_BUFF_SIZE);
	virtual ~CStringFile();
	
public:
	BOOL Open(LPCSTR szFile, BOOL bDecode=FALSE);
	virtual void Close() override;

public:
	virtual void Reset() override;

public:
	virtual DWORD GetNextLine(LPSTR szLine,UINT iLineSize) override;
	virtual DWORD GetNextLine(PSTR& szString) override;
	virtual DWORD GetNextLine(CString& strString) override;
	virtual BYTE* GetText() override;

public:
	void ChangeBufferSize ( UINT _nBufferSize );

protected:
	DWORD	m_nMaxSize;
	DWORD	m_dwMasterIndex;
	int		m_nSectionCount;
	FILE*	m_fFile;

	bool	m_bChangeBufferSize;
};

} // namespace sc