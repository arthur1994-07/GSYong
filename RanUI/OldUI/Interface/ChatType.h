#pragma	once
//	ä�� �޽��� ����

namespace CHAT_CONST
{
	enum CHAT_TYPE
	{
		CHAT_NO				= 0x0000,	//	�޽��� ����
		CHAT_NORMAL			= 0x0001,	//	�Ϲ�
		CHAT_PRIVATE		= 0x0002,	//	�ӼӸ�
		CHAT_PARTY			= 0x0004,	//	��Ƽ
		CHAT_MASTER			= 0x0008,	//	������ ������
		CHAT_GUILD			= 0x0010,	//	Ŭ��
		CHAT_ALLIANCE		= 0x0020,	//	����
		CHAT_AREA				= 0x0040,	//	����
		CHAT_PARTY_RECRUIT	= 0x0080,	//	��Ƽ����
		CHAT_TOALL			= 0x0100,	//	Ȯ����
		CHAT_FACTION		= 0x0200,	//	����
		CHAT_SYSTEM			= 0x0400,	//	�ý���
		CHAT_ALL			= CHAT_NORMAL|CHAT_PRIVATE|CHAT_PARTY|CHAT_MASTER|CHAT_GUILD|CHAT_ALLIANCE|CHAT_PARTY_RECRUIT|CHAT_AREA|CHAT_TOALL|CHAT_SYSTEM|CHAT_FACTION,
	};

	enum CHAT_SIMBOL
	{
		CHAT_SIMBOL_NORMAL,			//	�Ϲ�
		CHAT_SIMBOL_PRIVATE,		//	�ӼӸ�
		CHAT_SIMBOL_PARTY,			//	��Ƽ
		CHAT_SIMBOL_GUILD,			//	Ŭ��
		CHAT_SIMBOL_ALLIANCE,		//	����
		CHAT_SIMBOL_AREA,			//	����
		CHAT_SIMBOL_PARTY_RECRUIT,	//	��Ƽ����
		CHAT_SIMBOL_TOALL,			//	Ȯ����
		CHAT_SIMBOL_SYSTEM,			//	�ý���	
		CHAT_SIMBOL_REPLAY,			//	���
		CHAT_SIMBOL_FACTION,		//	����
		
		CHAT_SIMBOL_SIZE,
	};

	static const CString CHAT_SIMBOL_BLANK = ( _T(" ") );	//	����

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
		( _T("/1") ),	//	����
		( _T("/2") ),	//	����
		( _T("/yell") ),
		( _T("/notice") ),
		( _T("/reply") ),
		( _T("/3") ),	//	����
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