#pragma	once
//	채팅 메시지 정의

namespace CHAT_CONST
{
	enum CHAT_TYPE
	{
		CHAT_NO				= 0x0000,	//	메시지 없음
		CHAT_NORMAL			= 0x0001,	//	일반
		CHAT_PRIVATE		= 0x0002,	//	귓속말
		CHAT_PARTY			= 0x0004,	//	파티
		CHAT_MASTER			= 0x0008,	//	원정대 마스터
		CHAT_GUILD			= 0x0010,	//	클럽
		CHAT_ALLIANCE		= 0x0020,	//	동맹
		CHAT_AREA				= 0x0040,	//	지역
		CHAT_PARTY_RECRUIT	= 0x0080,	//	파티모집
		CHAT_TOALL			= 0x0100,	//	확성기
		CHAT_FACTION		= 0x0200,	//	진영
		CHAT_SYSTEM			= 0x0400,	//	시스템
		CHAT_ALL			= CHAT_NORMAL|CHAT_PRIVATE|CHAT_PARTY|CHAT_MASTER|CHAT_GUILD|CHAT_ALLIANCE|CHAT_PARTY_RECRUIT|CHAT_AREA|CHAT_TOALL|CHAT_SYSTEM|CHAT_FACTION,
	};

	enum CHAT_SIMBOL
	{
		CHAT_SIMBOL_NORMAL,			//	일반
		CHAT_SIMBOL_PRIVATE,		//	귓속말
		CHAT_SIMBOL_PARTY,			//	파티
		CHAT_SIMBOL_GUILD,			//	클럽
		CHAT_SIMBOL_ALLIANCE,		//	동맹
		CHAT_SIMBOL_AREA,			//	지역
		CHAT_SIMBOL_PARTY_RECRUIT,	//	파티모집
		CHAT_SIMBOL_TOALL,			//	확성기
		CHAT_SIMBOL_SYSTEM,			//	시스템	
		CHAT_SIMBOL_REPLAY,			//	답신
		CHAT_SIMBOL_FACTION,		//	진영
		
		CHAT_SIMBOL_SIZE,
	};

	static const CString CHAT_SIMBOL_BLANK = ( _T(" ") );	//	공백

	static const CString CHAT_TYPE_SYMBOL[CHAT_SIMBOL_SIZE] = 
	{
		( _T("/s") ),
		( _T("/w") ),
		( _T("/p") ),
		( _T("/c") ),
		( _T("/u") ),
		( _T("/1") ),
		( _T("/2") ),
		( _T("/y") ),
		( _T("/n") ),
		( _T("/r") ),
		( _T("/3") ),
	};

	static const CString CHAT_TYPE_SYMBOL_EX[CHAT_SIMBOL_SIZE] = 
	{
		( _T("/say") ),
		( _T("/whisper") ),
		( _T("/party") ),
		( _T("/club") ),
		( _T("/union") ),
		( _T("/1") ),	//	동일
		( _T("/2") ),	//	동일
		( _T("/yell") ),
		( _T("/notice") ),
		( _T("/reply") ),
		( _T("/3") ),	//	동일
	};

	static const CString CHAT_TYPE_SYMBOL_OLD[CHAT_SIMBOL_SIZE] = 
	{
		( _T("") ),
		( _T("@") ),
		( _T("#") ),
		( _T("%") ),
		( _T("!") ),
		( _T("") ),
		( _T("") ),
		( _T("$") ),
		( _T("&") ),
		( _T("") ),
	};


};