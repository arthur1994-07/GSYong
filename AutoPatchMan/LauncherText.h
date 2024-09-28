#pragma	once

#include "../Enginelib/GUInterface/GameTextLoader.h"
#include "../enginelib/DxTools/DxFontDefine.h"

//	게임 텍스트
struct SLAUNCHERTEXT
{
	/////////////////////////////////////////////////////////////////////////////
	static const WORD progVersion = 0x0001;
	/////////////////////////////////////////////////////////////////////////////

	NS_TEXT_LOADER::STRFLAG		mapFlags[language::LANGFLAG_TOTAL_NUM];
};

const char * ID2LAUNCHERTEXT ( const char* szKeyword, int nIndex = 0 );

class CLauncherTextMan
{
private:
	CLauncherTextMan ();
	virtual ~CLauncherTextMan ();

public:
	BOOL LoadText ( const char* szFileName,BOOL bXML = FALSE );	
	void Clear ();

	const CString & GetLauncherText( std::string strKey, int Index = 0 );
	const CString &	GetText( std::string strKey, int Index);
	bool CheckText( std::string strKey, int Index = 0);
	int	GetNumber( std::string strKey, int Index);

	const char* GetPath ()				{ return m_szPath; }
	void SetPath( const char* szPath )	{ StringCchCopy(m_szPath,MAX_PATH,szPath); }
	const language::EMPROVIDE_LANGFLAG GetLangFlag()	{return m_emLang;}

	bool LoadHelpCommentString(const std::string& strID
		, std::vector<std::string>& vecOutList);

private:
	BOOL DefaultSetText();

private:
	char m_szPath[MAX_PATH];
    SLAUNCHERTEXT					m_LauncherText;
	language::EMPROVIDE_LANGFLAG	m_emLang;

public:
	static CLauncherTextMan& GetInstance();
};