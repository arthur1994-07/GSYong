#pragma	once

#include <boost/function.hpp>
#include "../DxTools/DxFontDefine.h"
#include "./GameTextLoader.h"

#define STRUCT_STRFLAG( value )							\
	struct S##value										\
	{													\
		static const WORD progVersion = 0x0001;			\
		NS_TEXT_LOADER::STRFLAG		mapFlags[language::LANGFLAG_TOTAL_NUM];	\
	};																	\
	const char * ID2##value(const char* szKeyword, const int nIndex = 0);	\

#define STRUCT_STRFLAGEX( value )						\
	struct S##value										\
	{													\
		static const WORD progVersion = 0x0001;			\
		NS_TEXT_LOADER::STREXFLAG		mapFlags[language::LANGFLAG_TOTAL_NUM];	\
	};																		\
	const char * ID2##value(const char* szKeyword, const int nMid, const int nSid, const int nIndex); 


STRUCT_STRFLAG(GAMEWORD);
STRUCT_STRFLAG(GAMEINTEXT);
STRUCT_STRFLAG(GAMEEXTEXT);
STRUCT_STRFLAG(SERVERTEXT);
STRUCT_STRFLAG(SKILLTEXT);
STRUCT_STRFLAG(ITEMTEXT);
STRUCT_STRFLAG(CROWTEXT);
STRUCT_STRFLAG(TIPTEXT);
STRUCT_STRFLAG(QUESTTEXT);
STRUCT_STRFLAG(COMMENTTEXT);
STRUCT_STRFLAG(NPCTALKTEXT);
STRUCT_STRFLAG(AUTOLEVELSETTEXT);
/*
//	게임 텍스트
struct SGAMEWORD
{
	/////////////////////////////////////////////////////////////////////////////
	static const WORD progVersion = 0x0001;
	/////////////////////////////////////////////////////////////////////////////

	NS_TEXT_LOADER::STRFLAG		mapFlags[language::NATION_SIZE];
};

//	게임 텍스트
struct SGAMEINTEXT
{
	/////////////////////////////////////////////////////////////////////////////
	static const WORD progVersion = 0x0001;
	/////////////////////////////////////////////////////////////////////////////

	NS_TEXT_LOADER::STRFLAG		mapFlags[language::NATION_SIZE];
};

//	게임 텍스트
struct SGAMEEXTEXT
{
	/////////////////////////////////////////////////////////////////////////////
	static const WORD progVersion = 0x0001;
	/////////////////////////////////////////////////////////////////////////////

	NS_TEXT_LOADER::STRFLAG		mapFlags[language::NATION_SIZE];
};

//	서버 텍스트
struct SSERVERTEXT
{
	/////////////////////////////////////////////////////////////////////////////
	static const WORD progVersion = 0x0001;
	/////////////////////////////////////////////////////////////////////////////

	NS_TEXT_LOADER::STRFLAG		mapFlags[language::NATION_SIZE];
};

// 스킬 텍스트;
struct SSKILLTEXT
{
	static const WORD progVersion = 0x0001;
	NS_TEXT_LOADER::STRFLAG		mapFlags[language::NATION_SIZE];
};

// 아이템 텍스트;
struct SITEMTEXT
{
	static const WORD progVersion = 0x0001;
	NS_TEXT_LOADER::STRFLAG		mapFlags[language::NATION_SIZE];
};

// CROW 텍스트;
struct SCROWTEXT
{
	static const WORD progVersion = 0x0001;
	NS_TEXT_LOADER::STRFLAG		mapFlags[language::NATION_SIZE];
};

// 도움말 텍스트;
struct STIPTEXT
{
	static const WORD progVersion = 0x0001;
	NS_TEXT_LOADER::STRFLAG		mapFlags[language::NATION_SIZE];
};

// 퀘스트 텍스트;
struct SQUESTTEXT
{
	static const WORD progVersion = 0x0001;
	NS_TEXT_LOADER::STRFLAG		mapFlags[language::NATION_SIZE];
};

// NPC 대화 텍스트;
struct SNPCTALKTEXT
{
	static const WORD progVersion = 0x0001;
	NS_TEXT_LOADER::STRFLAG		mapFlags[language::NATION_SIZE];
};
*/

// const char * ID2GAMEWORD(const char* szKeyword, int nIndex = 0);
// const char * ID2GAMEINTEXT(const char* szKeyword, int nIndex = 0);
// const char * ID2GAMEEXTEXT(const char* szKeyword, int nIndex = 0);
// const char * ID2SERVERTEXT(const char* szKeyword, int nIndex = 0);
// const char * ID2SKILLTEXT(const char* szKeyword, int nIndex = 0);
// const char * ID2ITEMTEXT(const char* szKeyword, int nIndex = 0);
// const char * ID2CROWTEXT(const char* szKeyword, int nIndex = 0);
// const char * ID2TIPTEXT(const char* szKeyword, int nIndex = 0);
// const char * ID2QUESTTEXT(const char* szKeyword, int nIndex = 0);
// const char * ID2NPCTALKTEXT(const char* szKeyword, int nIndex = 0);


class CGameTextMan
{
public:
	enum
	{		
		EM_GAME_WORD,
		EM_GAME_IN_TEXT,
		EM_GAME_EX_TEXT,
		EM_SERVER_TEXT,
		EM_SKILL_TEXT,
		EM_ITEM_TEXT,
		EM_CROW_TEXT,
		EM_TIP_TEXT,
		EM_COMMENT_TEXT,
		EM_QUEST_TEXT,
		EM_NPCTALK_TEXT,
		EM_AUTOLEVELSET_TEXT,

		EM_TEXT_SIZE,
	};

	enum
	{
		GAMETEXTMAN_LOADOP_REFACT		= 0x0001,
		GAMETEXTMAN_LOADOP_SERVEREXE	= 0x0002,
		GAMETEXTMAN_LOADOP_ALLLANG		= 0x0004,
		GAMETEXTMAN_LOADOP_CONVUTF8	= 0x0008,
	};

private:
	CGameTextMan();
	virtual ~CGameTextMan();

private:
    char m_szPath[MAX_PATH];

    SGAMEWORD	m_GameWord;
    SGAMEINTEXT	m_GameInText;
    SGAMEEXTEXT	m_GameExText;
    SSERVERTEXT m_ServerText;
	SSKILLTEXT	m_SkillText;
	SITEMTEXT   m_ItemText;
	SCROWTEXT	m_CrowText;
	STIPTEXT	m_TipText;
	SCOMMENTTEXT	m_CommentText;
	SQUESTTEXT		m_QuestText;
	SNPCTALKTEXT	m_NPCTalkText;
	SAUTOLEVELSETTEXT m_AutoLevelText;

public:
    static CGameTextMan& GetInstance();

public:
	const char* GetPath() const { return m_szPath; }
	void SetPath(const char* szPath) { StringCchCopy(m_szPath, MAX_PATH, szPath); }

	const CString& GetGameWord(const std::string& strKey, const int Index = 0);
	const CString& GetGameInText(const std::string& strKey, const int Index = 0);
	const CString& GetGameExText(const std::string& strKey, const int Index = 0);
	const CString& GetServerText(const std::string& strKey, const int nIndex = 0);
	const CString& GetSkillText(const std::string& strKey, const int nIndex = 0);
	const CString& GetItemText(const std::string& strKey, const int nIndex = 0);
	const CString& GetCrowText(const std::string& strKey, const int nIndex = 0);
	const CString& GetTipText(const std::string& strKey, const int nIndex = 0);
	const CString GetTipRandomText();
	const CString& GetQuestText(const std::string& strKey, const int nIndex = 0);
	const CString& GetNPCTalkText(const std::string& strKey, const int nIndex = 0);
	const CString& GetCommentText(const std::string& strKey, const int nIndex = 0);
	const CString& GetAutoLevelSetText(const std::string& strKey, const int nIndex = 0);

	const CString& GetText(const std::string& strKey, const int Index, const int nType);
	int	GetNumber(const std::string& strKey, int Index, int nType);

	void Clear(int nType);
	BOOL LoadText(const char* szFileName, int nType,const CString& country, DWORD dwLoadOP );
	const BOOL SaveXML(const char* const szFileName, const int nType, const BOOL bAllLang);
	const BOOL InsertString( const char* const szKeyName, const char* const szDesc, const int nType, const int nIndex = -1 );
	const BOOL DeleteString( const char* const szKeyName, const int nType );
	bool LoadHelpCommentString(const std::string& strID, std::string* pOutList, INT nType, const int nCount);
	bool LoadHelpUnknownCommentString(const std::string& strID, std::vector<std::string>& pOutList, INT nType);
	bool ParseComment();

	const BOOL FindData(const std::string& strKey, const INT nType);
private:
	// CommentLua는 xml Load 이후 스트링 테이블에 담아두지않고 COMMENT::에 담아둔다;
	void LoadHelpCommentString(const std::string& strID, std::string* pOutList, NS_TEXT_LOADER::STRFLAG& mapFlags, const int nCount);\
};
