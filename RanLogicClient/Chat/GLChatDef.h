#pragma once

namespace GLCHAT_DEF
{
	// 최대 채팅 보관 갯수
	static const WORD g_wMaxChatCount = 200;

	// 채팅 타입 ( 수정시 WidgetDef.lua 파일 같이 수정 )
	enum EMCHAT_TYPE
	{
		EMCHAT_NO				= 0x0000,		// 메시지 없음
		EMCHAT_NORMAL			= 0x0001,		// 일반
		EMCHAT_PRIVATE			= 0x0002,		// 귓속말
		EMCHAT_PARTY			= 0x0004,		// 파티
		EMCHAT_MASTER			= 0x0008,		// 원정대 마스터
		EMCHAT_GUILD			= 0x0010,		// 클럽
		EMCHAT_ALLIANCE			= 0x0020,		// 동맹
		EMCHAT_AREA				= 0x0040,		// 지역
		EMCHAT_PARTY_RECRUIT	= 0x0080,		// 파티모집
		EMCHAT_TOALL			= 0x0100,		// 확성기
		EMCHAT_FACTION			= 0x0200,		// 진영
		EMCHAT_SYSTEM			= 0x0400,		// 시스템
		EMCHAT_COMMAND			= 0X0800,		// 커맨드

		EMCHAT_PRIVATE_PARTY_RECRUIT = 0x1000, // 귓말로 보내는 파티 초대관련 메세지 //

		EMCHAT_ALL				= EMCHAT_NORMAL | EMCHAT_PRIVATE | EMCHAT_PARTY | EMCHAT_MASTER | 
								  EMCHAT_GUILD | EMCHAT_ALLIANCE | EMCHAT_PARTY_RECRUIT | EMCHAT_AREA |
								  EMCHAT_TOALL | EMCHAT_FACTION | EMCHAT_PRIVATE_PARTY_RECRUIT | EMCHAT_SYSTEM,
	};

	// 채팅 심볼 ( 수정시 WidgetDef.lua 파일 같이 수정 )
	enum EMCHAT_SIMBOL
	{
		EMCHAT_SIMBOL_NORMAL,			// 일반
		EMCHAT_SIMBOL_PRIVATE,			// 귓속말
		EMCHAT_SIMBOL_PARTY,			// 파티
		EMCHAT_SIMBOL_GUILD,			// 클럽
		EMCHAT_SIMBOL_ALLIANCE,			// 동맹
		EMCHAT_SIMBOL_AREA,				// 지역
		EMCHAT_SIMBOL_PARTY_RECRUIT,	// 파티모집
		EMCHAT_SIMBOL_TOALL,			// 확성기
		EMCHAT_SIMBOL_SYSTEM,			// 시스템	
		EMCHAT_SIMBOL_REPLAY,			// 답신

		EMCHAT_SIMBOL_SIZE,
	};

	// 채팅 컬러 덧씌우기 위한 데이터 
	struct SCHAT_CUSTOM_COLOR
	{
		DWORD	dwColor;
		int		nBegin;
		int		nEnd;

		SCHAT_CUSTOM_COLOR()
			: dwColor( 0xFFFFFFFF )
			, nBegin( 0 )
			, nEnd( 0 )
		{
		}
	};

	typedef std::vector< SCHAT_CUSTOM_COLOR >		VEC_CHAT_CUSTOM_COLOR;
	typedef VEC_CHAT_CUSTOM_COLOR::const_iterator	VEC_CHAT_CUSTOM_COLOR_CITER;

	// 채팅 이름 정보보기 데이터
	struct SCHAT_REFERCHAR_NAME
	{
		std::string strName;
		int nBegin;
		int nEnd;

		SCHAT_REFERCHAR_NAME()
			: nBegin( 0 )
			, nEnd( 0 )
		{
		}
	};
}