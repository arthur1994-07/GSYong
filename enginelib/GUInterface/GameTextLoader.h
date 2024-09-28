///////////////////////////////////////////////////////////////////////
//	게임 텍스트 로더
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.8.1] - 작성
//
///////////////////////////////////////////////////////////////////////
#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <map>

namespace NS_TEXT_LOADER
{
	extern int	g_nTestMsgBox;	// Test Code

	// 1. 기존
	// GameWord, InText, ExText, ServerText, ItemText, CrowText, TipText, QuestText
    typedef std::tr1::unordered_map<std::string, CStringArray*> STRFLAG;
	typedef STRFLAG::iterator	                				STRFLAG_IT;
    typedef STRFLAG::value_type	                				STRFLAG_VALUE;

// 	// 2. 확장
// 	// NPCTalk, NPCAnswer
// 	// Index
// 	typedef std::tr1::unordered_map<WORD, CStringArray*>		STREX_INDEXFLAG;
// 	typedef STREX_INDEXFLAG::iterator						STREX_INDEXIT;
// 	typedef STREX_INDEXFLAG::value_type						STREX_INDEXVALUE;
// 	// Sub
// 	typedef std::tr1::unordered_map<WORD, STREX_INDEXFLAG>		STREX_SUBFLAG;
// 	typedef STREX_SUBFLAG::iterator							STREX_SUBIT;
// 	typedef STREX_SUBFLAG::value_type						STREX_SUBVALUE;
// 	// Main
// 	typedef std::tr1::unordered_map<std::string, STREX_SUBFLAG> STREXFLAG;
// 	typedef STREXFLAG::iterator									STREX_MAINIT;
// 	typedef STREXFLAG::value_type								STREX_MAINVALUE;

	enum {
		NS_TEXT_LOADOP_REFACT		= 0x0001,
		NS_TEXT_LOADOP_SERVEREXE	= 0x0002,
		NS_TEXT_LOADOP_ALLLANG		= 0x0004,
		NS_TEXT_LOADOP_CONVUTF8		= 0x0008,
	};

	extern BOOL bServer;

	BOOL LoadText( const char* szFileName, WORD progVersion, void* mapFlags, const CString& country, DWORD dwLoadOP );
	BOOL LoadTextInZip( const char* szZipFileFullPath, const char* szPath, const char* szFile, const BOOL bPack, void* mapFlags, BOOL bServerExe, const CString& country );
	BOOL ClearText( STRFLAG* mapFlags );

	const CString& GetText(const std::string& strKey, int Index, STRFLAG& mapFlags);
	int GetNumber(const std::string& strKey, int Index, STRFLAG& mapFlags );
	bool CheckText(const std::string& strKey, int Index, STRFLAG& mapFlags );
	const BOOL SaveStringTableXML( const char* const szFileName, STRFLAG* mapFlags, const BOOL bAllLang );
	// const BOOL SaveStringTableXML( const char* const szFileName, STRFLAG* mapFlags, const BOOL bAllLang );
}