#pragma	once

#include "../Util/UIOuterWindow.h"

class COuterInterface;
class CSelectCharacterPageListItem;
class CBasicScrollBarEx;
struct SCHARINFO_LOBBY;
class GLGaeaClient;
class CLockSelectCharacterPageListItem;

class CSelectCharacterPageList : public CUIGroup
{
	enum
	{
		
		SELECT_CHAR_FOCUS_0 = NO_ID + 1,
		SELECT_CHAR_FOCUS_1,
		SELECT_CHAR_FOCUS_2,
		SELECT_CHAR_FOCUS_3,
		SELECT_CHAR_FOCUS_4,
		SELECT_CHAR_FOCUS_5,
		SELECT_CHARACTER_SCROLLBAR,

		MAX_CHAR_LIST_NUM = 6,
	};

public:
	CSelectCharacterPageList(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSelectCharacterPageList ();

public:
	void CreateSubControl ();
	CUIControl*	CreateControl( char* szControl );
	void SetSelectCharIndex( DWORD dwCharSelect, INT nCharStart );
	void ResetAll();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
private:
	INT m_nSelectPage;
	INT m_nSelectNum;
	SCHARINFO_LOBBY* m_pSelectCharInfo;

	CSelectCharacterPageListItem* m_pListItem[ MAX_CHAR_LIST_NUM ];
	CUIControl*	m_pItemFocus[ MAX_CHAR_LIST_NUM ];
	CBasicScrollBarEx*		m_pScrollBar;
    CLockSelectCharacterPageListItem* m_pLockItem[ MAX_CHAR_LIST_NUM ];

protected:
	COuterInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;
};