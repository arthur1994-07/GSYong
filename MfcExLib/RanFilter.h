// By 경대
#pragma once
#include <string>
#include <vector>
#include "Slangfilter.h"

namespace sc {
    class CBaseString;
}

struct ChatIgnorUser
{
	_SYSTEMTIME m_chatTime;
	BOOL m_bIgnor;

	ChatIgnorUser()
	{
		GetSystemTime(&m_chatTime);
		m_bIgnor = FALSE;
	}
};

class CRanFilter
{
	enum
	{
		EM_ERROR_CODE = 0,
		EM_UNICODE,
		EM_HEX,
		EM_CHAR
	};

public:
	typedef std::map<CString, ChatIgnorUser>	CHAT_IGNOR_MAP;
    typedef CHAT_IGNOR_MAP::iterator			CHAT_IGNOR_MAP_ITER;
    typedef CHAT_IGNOR_MAP::const_iterator		CHAT_IGNOR_MAP_CITER;
    typedef CHAT_IGNOR_MAP::value_type			CHAT_IGNOR_MAP_VALUE;

protected:
	SlangFilter m_ChatFilter;
	SlangFilter m_NameFilter;
	CHAT_IGNOR_MAP m_ChatIgnorFilter;

	CString m_strFilted;

	SlangFilter* m_pCurrentFilter;

	BOOL m_bZipFile;
	BOOL m_bPackFile;

protected:
	UINT m_nCodePage; // 유니코드 코드페이지

public:
	BOOL Init( CONST BOOL bZipFile, CONST BOOL bPackFile, CONST TCHAR* szZipFile, CONST TCHAR* szPath );
	BOOL ChatFilter( CONST CString & );
	BOOL ChatIgnorFilter( CONST CString & );
	BOOL NameFilter( CONST CString & );
	const CString& GetProverb() const { return m_strFilted; }

	VOID SetCodePage( UINT nCodePage ) { m_nCodePage = nCodePage; }
	UINT GetCodePage() { return m_nCodePage; }

private:
	BOOL ReadToFile( CONST TCHAR* szZipFile, CONST TCHAR* szPath );
	BOOL Filter( CONST CString &, CONST SlangFilter & );
	BOOL IgnorFilter( CONST CString & );
	BOOL CompareChatTime( _SYSTEMTIME &currentTime, _SYSTEMTIME &prevTime );
	//INT Nrand(); // 난수 발생

private:
    BOOL LOAD(const std::string& ZipFileName, CONST TCHAR* szPath, CONST TCHAR* szFile);
	BOOL LOADCHAT( CONST TCHAR* szZipFile, CONST TCHAR* szPath );
	BOOL LOADNAME( CONST TCHAR* szZipFile, CONST TCHAR* szPath );
	//bool LOADPROVERB( const BOOL bZipFile, const TCHAR* szZipFile, const TCHAR* szPath );

	UINT GetSectionCode( CONST CString & strSection );
	UINT GetSectionCode_XML( const CString& strSection );

	VOID InputStringHEX( CONST CString & strBUFFER );
	VOID InputStringCHAR( CONST CString & strBUFFER );
	VOID InputStringUNICODE( CONST CString & strBUFFER );

private:
	CRanFilter();
	~CRanFilter();

public:
	static CRanFilter& GetInstance();
};