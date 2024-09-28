#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class CBasicTextBoxEx;
class CBasicScrollBarEx;
class GLGaeaClient;

class CClubBattle : public CUIGroup
{
protected:
	enum
	{
		CLUB_LIST_TEXTBOX = NO_ID + 1,
		CLUB_LIST_SCROLLBAR
	};

public:
	CClubBattle(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CClubBattle();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;
    int m_nSelectIndex;
    CString	m_strSelectName;
    bool m_bSelAlliance;
    CBasicTextBoxEx* m_pTextBox;
    CBasicScrollBarEx*	m_pScrollBarEx;

public:
	void CreateSubControl();
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

	const DWORD GetBattleClubID();
	const CString GetBattleClubName() const	{ return m_strSelectName; }
	const bool GetIsAlliance() const { return m_bSelAlliance; }

	BOOL IsSelectedIndex();
	void LoadClubBattleList();
	void ShowClubBattleInfo( int nIndex );
};
