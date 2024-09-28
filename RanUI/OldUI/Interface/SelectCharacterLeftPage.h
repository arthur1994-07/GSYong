#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class COuterInterface;
class CSelectCharacterCard;
class CBasicTextButton;
class CBasicTextBox;
struct SCHARINFO_LOBBY;
class GLGaeaClient;

const DWORD UIMSG_MOUSEIN_SELECT_CHARACTERLEFTPAGE = UIMSG_USER1;

class CSelectCharacterLeftPage : public CUIGroup
{
private:
	enum
	{
		SELECT_CHARACTER_NEW = NO_ID + 1,
		SELECT_CHARACTER_DELETE,
		SELECT_CHARACTER_CHANGE_SERVER,
		SELECT_CHARACTER_GAME_START,
	};

public:
	CSelectCharacterLeftPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSelectCharacterLeftPage ();

public:
	void CreateSubControl ();
	CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );
	BOOL SndGameJoin (const char *psz2ndPass = NULL); // 2Â÷ºñ¹ø
	void SndCharDelete();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	SCHARINFO_LOBBY * GetDelCharInfo()				{ return m_pDelCharInfo; }

private:
	SCHARINFO_LOBBY* m_pDelCharInfo;
	CSelectCharacterCard* m_pSelectCharacterCard;
	CBasicTextButton* m_pNewButton;
	CBasicTextBox* m_pServerText;

protected:
	COuterInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;
};