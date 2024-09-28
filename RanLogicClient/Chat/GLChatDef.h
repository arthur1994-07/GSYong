#pragma once

namespace GLCHAT_DEF
{
	// �ִ� ä�� ���� ����
	static const WORD g_wMaxChatCount = 200;

	// ä�� Ÿ�� ( ������ WidgetDef.lua ���� ���� ���� )
	enum EMCHAT_TYPE
	{
		EMCHAT_NO				= 0x0000,		// �޽��� ����
		EMCHAT_NORMAL			= 0x0001,		// �Ϲ�
		EMCHAT_PRIVATE			= 0x0002,		// �ӼӸ�
		EMCHAT_PARTY			= 0x0004,		// ��Ƽ
		EMCHAT_MASTER			= 0x0008,		// ������ ������
		EMCHAT_GUILD			= 0x0010,		// Ŭ��
		EMCHAT_ALLIANCE			= 0x0020,		// ����
		EMCHAT_AREA				= 0x0040,		// ����
		EMCHAT_PARTY_RECRUIT	= 0x0080,		// ��Ƽ����
		EMCHAT_TOALL			= 0x0100,		// Ȯ����
		EMCHAT_FACTION			= 0x0200,		// ����
		EMCHAT_SYSTEM			= 0x0400,		// �ý���
		EMCHAT_COMMAND			= 0X0800,		// Ŀ�ǵ�

		EMCHAT_PRIVATE_PARTY_RECRUIT = 0x1000, // �Ӹ��� ������ ��Ƽ �ʴ���� �޼��� //

		EMCHAT_ALL				= EMCHAT_NORMAL | EMCHAT_PRIVATE | EMCHAT_PARTY | EMCHAT_MASTER | 
								  EMCHAT_GUILD | EMCHAT_ALLIANCE | EMCHAT_PARTY_RECRUIT | EMCHAT_AREA |
								  EMCHAT_TOALL | EMCHAT_FACTION | EMCHAT_PRIVATE_PARTY_RECRUIT | EMCHAT_SYSTEM,
	};

	// ä�� �ɺ� ( ������ WidgetDef.lua ���� ���� ���� )
	enum EMCHAT_SIMBOL
	{
		EMCHAT_SIMBOL_NORMAL,			// �Ϲ�
		EMCHAT_SIMBOL_PRIVATE,			// �ӼӸ�
		EMCHAT_SIMBOL_PARTY,			// ��Ƽ
		EMCHAT_SIMBOL_GUILD,			// Ŭ��
		EMCHAT_SIMBOL_ALLIANCE,			// ����
		EMCHAT_SIMBOL_AREA,				// ����
		EMCHAT_SIMBOL_PARTY_RECRUIT,	// ��Ƽ����
		EMCHAT_SIMBOL_TOALL,			// Ȯ����
		EMCHAT_SIMBOL_SYSTEM,			// �ý���	
		EMCHAT_SIMBOL_REPLAY,			// ���

		EMCHAT_SIMBOL_SIZE,
	};

	// ä�� �÷� ������� ���� ������ 
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

	// ä�� �̸� �������� ������
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