#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class	COuterInterface;
class	CBasicButton;
class	CBasicTextBox;
class	CSelectCharacterInfoPage;
class	CBasicTextButton;
class	CSelectCharacterPageList;
class	GLGaeaClient;
struct	SCHARINFO_LOBBY;

const DWORD UIMSG_MOUSEIN_SELECT_CHARACTERPAGE = UIMSG_USER1;

class CSelectCharacterPage : public CUIGroup
{
private:
	enum
	{
		SELECT_CHARACTER_START = NO_ID + 1,
		SELECT_CHARACTER_CANCEL,
		SELECT_CHARACTER_LEFT_BUTTON,
		SELECT_CHARACTER_LEFT_BUTTON_FLIP,
		SELECT_CHARACTER_RIGHT_BUTTON,
		SELECT_CHARACTER_RIGHT_BUTTON_FLIP,
		SELECT_CHARACTER_LIST,
	};

public:
	CSelectCharacterPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSelectCharacterPage ();

public:
	void CreateSubControl ();
	CSelectCharacterPageList* GetSelectCharPageList() { return m_pSelectCharacterPageList; }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void SetVisibleSingle( BOOL bVisible );
public:
	BOOL SndGameJoin();
    void SndGameJoinMsg();

public:
	void	ResetAll ();

private:
    bool CheckSelectClubCondition( const SCHARINFO_LOBBY* pCharInfo );

private:
	CSelectCharacterInfoPage* m_pSelectCharacterPage;
	CSelectCharacterPageList* m_pSelectCharacterPageList;

protected:
	CBasicTextBox* m_pPageNumTextBox;

	CBasicTextButton* m_pStartButton;

protected:
	COuterInterface* m_pInterface;
	GLGaeaClient*	 m_pGaeaClient;

};