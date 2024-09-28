#pragma once

#include <string>

namespace sc
{
	class CRijndael;

	class CBaseString  
	{
	public:
		enum { BS_BUFF_SIZE = 4096, };

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

	protected:
		DWORD	m_nMaxSize;
		DWORD	m_dwMasterIndex;
		int		m_nSectionCount;
		FILE*	m_fFile;
	};

	sc::CBaseString* openfile_basestring(
		const std::string& ZipFileName, const char* szFullPath, const char* szFile, bool bENC = false, BOOL bPack = FALSE );

} // namespace sc


namespace CHARSET
{
	void SetCodePage(int nCodePage);
	bool IsLeadChar(char cC);
};

namespace STRUTIL
{
	//	Note : 등록된 "구분자" 제거 메소드.
	void ClearSeparator();

	//	Note : "구분자" 등록 메소드.
	void RegisterSeparator( const CString& strSeparator );

	//	Note : 스트링을 "구분자"로 분리하여 스트링 어래이에 넣어서 반환.
	//		"실패"시 "FALSE"를 리턴.
	//		"성공"시 "TRUE"를 리턴.
	//
	BOOL StringSeparate ( CString &Str, CStringArray &StrArray );
}