#pragma	once

#include "../Util/UIWindowEx.h"
//#include "../../../RanLogic/Skill/GLSkill.h"
#include "../../InnerInterface.h"

class	CInnerInterface;
class	CBasicTextButton;
class	CSkillPage;
class	CBasicTextBox;
struct	SCHARSKILL;
class GLGaeaClient;

class CSkillWindow : public CUIWindowEx
{
protected:
	enum
	{
		SKILL_WINDOW_PAGE_BUTTON0 = ET_CONTROL_NEXT,
		SKILL_WINDOW_PAGE_BUTTON1,
		SKILL_WINDOW_PAGE_BUTTON2,
		SKILL_WINDOW_PAGE_BUTTON3,
		SKILL_WINDOW_PAGE0,
		SKILL_WINDOW_PAGE1,
		SKILL_WINDOW_PAGE2,
		SKILL_WINDOW_PAGE3,
	};

	enum
	{
		nMAXPAGE = 4,
	};

public:
	CSkillWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSkillWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl();

private:
	void	SetVisiblePage( int nPage );

public:
	void	SetVisiblePageForTutorial();

public:
	virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTextButton*	CreateTextButton( const char* szButton, UIGUID ControlID, const char* szText );	
	CSkillPage*			CreateSkillPage( UIGUID ControlID, SNATIVEID sNativeIDArray[GLSkillMan::MAX_CLASSSKILL], int nSkillCount );

private:
	CUIControl*		CreateControl( char* szControl );
	CBasicTextBox*	CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign );

private:
	void	GetSkillIndexRange( EMCHARCLASS emCharClass, int* pnBeginIndex, int* pnEndIndex );
	void	SortSkill( EMCHARCLASS emCharClass, SNATIVEID sNativeIDArrayAll[EMSKILL_PC_CLASSNUM][GLSkillMan::MAX_CLASSSKILL], int nSkillCountAll[EMSKILL_PC_CLASSNUM] );

private:
	CSkillPage*			m_pPage[nMAXPAGE];
	CBasicTextButton*	m_pPageButton[nMAXPAGE];

private:
	DWORD				m_dwSkillPointBACK;
	CBasicTextBox*		m_pRestPoint;

};

class MySkillWindow : public ISkillWindow, private CSkillWindow
{
public:
    MySkillWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MySkillWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void ShowPageForTutorial();
    virtual const UIRECT& GetUIWindowGlobalPos();

    virtual void Refresh()
    {
    }
};