//	도움말 윈도우
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2004.02.24]
//			@ 작성

#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CQuestList;
class CQuestMain;
class CQuestStep;
class CBasicTextButton;
class GLGaeaClient;

class CQuestWindow : public CUIWindowEx
{
public:
	enum
	{
		QUEST_LIST = ET_CONTROL_NEXT,
		QUEST_MAIN,
		QUEST_STEP,
		QUEST_LIST_BUTTON_MORE
	};

public:
	CQuestWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CQuestWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl();

public:
	virtual	void	TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void	AUTOSELECT_PAGE( DWORD dwQuestID );
	void	SET_SHOW_PAGE( int nPage, bool bMakeMsg = false );
	int		GET_SHOW_PAGE()						{ return m_nPage; }
	void	REFRESH_QUEST_WINDOW();

private:
	bool	LOAD_QUEST_LIST();
	bool	LOAD_QUEST_MAIN();
	bool	LOAD_QUEST_STEP();

private:
	void	SetQuestID( DWORD dwQuestID )		{ m_dwQuestID = dwQuestID; }
	void	SET_ALL_INVISIBLE_MAKEMSG();

public:
	DWORD	GetQuestID()						{ return m_dwQuestID; }

private:
	CBasicTextButton*	CreateTextButton14( char* szButton, UIGUID ControlID , char* szText );

private:
	DWORD	m_dwQuestID;
	int		m_nPage;

	CQuestList*		m_pQuestList;
	CQuestMain*		m_pQuestMain;
	CQuestStep*		m_pQuestStep;

	CBasicTextButton*	m_pMoreButton;

public: // for tutorial. by luxes.
	CQuestList*	GetQuestList() { return m_pQuestList; }
	CQuestMain*	GetQuestMain() { return m_pQuestMain; }
	CQuestStep*	GetQuestStep() { return m_pQuestStep; }

    const UIRECT& GetProgListTextBoxGlobalPos();
    const UIRECT& GetProgListTextBoxLocalPos();
    const UIRECT& GetProgMainTextBoxGlobalPos();
    const UIRECT& GetProgMainTextBoxLocalPos();
    const UIRECT& GetProgMainMoreBtnGlobalPos();

};

class MyQuestWindow : public IQuestWindow, private CQuestWindow
{
public:
    MyQuestWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyQuestWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual UIGUID GetWndID();
    virtual void RefreshQuestWindow();
    virtual int	 GetShowPage();
    virtual DWORD GetQuestID();
    virtual void SetShowPage( int nPage, bool bMakeMsg = false );
    virtual void AutoSelectPage( DWORD dwQuestID );
    virtual const UIRECT& GetUIWindowGlobalPos();
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos );
    virtual const UIRECT& GetProgListTextBoxGlobalPos();
    virtual const UIRECT& GetProgListTextBoxLocalPos();
    virtual const UIRECT& GetProgMainTextBoxGlobalPos();
    virtual const UIRECT& GetProgMainTextBoxLocalPos();
    virtual const UIRECT& GetProgMainMoreBtnGlobalPos();
};