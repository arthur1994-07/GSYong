#pragma	once

#include "../../Util/UIWindowEx.h"
#include "../../../InnerInterface.h"

class CQuestListRenewal;
class CBasicTextButton;
class CQuestMainRenewal;
class CQuestMainEndRenewal;
class CBasicTextBox;
class GLGaeaClient;
class CInnerInterface;

class CQuestWindowRenewal : public CUIWindowEx
{
public:
	enum
	{
		QUEST_LIST = ET_CONTROL_NEXT + 1,
		QUEST_MAIN,
		QUEST_MAIN_END,
		QUEST_ING_BUTTON,
		QUEST_DONE_BUTTON,
		QUEST_HELPER_COUNT,
	};

    enum
    {
        QUEST_WINDOW_PAGE_ING = 0,
        QUEST_WINDOW_PAGE_END = 1,
    };

public:
	CQuestWindowRenewal(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CQuestWindowRenewal();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl();

public:
	virtual	void	TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:

	bool	LOAD_QUEST_LIST();
	bool	LOAD_QUEST_MAIN();
	bool	LOAD_QUEST_STEP();

public:
	void	PageFlip( bool bFilp );
	void	REFRESH_QUEST_WINDOW();
	void	AUTOSELECT_PAGE( DWORD dwQuestID );
	void	SET_SHOW_PAGE();
	void	AUTO_LOAD_QUEST( DWORD dwQuestID );

	void	SetQuestID( DWORD dwQuestID );

	CQuestListRenewal*		GetQuestList()	{ return m_pQuestList; }
	CQuestMainRenewal*		GetQuestMain()	{ return m_pQuestMain; }
	CQuestMainEndRenewal*	GetQuestMainEnd()	{ return m_pQuestMainEnd; }

	int		GetPage()	{ return m_nPage; }

	void	ShowHelperCount();

	const UIRECT&	GetMainStepBackGP();

    bool CheckListAndReturnResult( DWORD dwID, bool bCurStatus );
    bool OpenListAndReturnResult( DWORD dwID, bool bCurStatus );
    void SetQuestListID( DWORD dwID );
    const UIRECT& GetListPageGlobalPos();
    DWORD GetQuestMainID();
    void LoadQuestMain( DWORD dwID );
    void LoadQuestMainEnd( DWORD dwID );

private:
	CQuestListRenewal*		m_pQuestList;
	CQuestMainRenewal*		m_pQuestMain;
	CQuestMainEndRenewal*	m_pQuestMainEnd;

	CBasicTextButton*	m_pIngButton;
	CBasicTextButton*	m_pDoneButton;

	CBasicTextBox*		m_pHelperCount;

	int m_nPage; // 0 = 진행 중 페이지, 1 = 종료 페이지. by luxes.
};

class MyQuestWindowRenewal : public IQuestWindowRenewal, private CQuestWindowRenewal
{
public:
    MyQuestWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyQuestWindowRenewal() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void AutoLoadQuest( DWORD dwQuestID );
    virtual UIGUID GetWndID();
    virtual void RefreshQuestWindow();
    virtual void SetShowPage();
    virtual void AutoSelectPage( DWORD dwQuestID );
    virtual const UIRECT& GetUIWindowGlobalPos();
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos );
    virtual const UIRECT& GetUIWindowLocalPos();
    virtual void SetUIWindowLocalPos( const D3DXVECTOR2& vPos );
    virtual CQuestListRenewal* GetQuestList();
    virtual CQuestMainRenewal* GetQuestMain();
    virtual CQuestMainEndRenewal* GetQuestMainEnd();
    virtual BOOL IsVisible();
    virtual const UIRECT& GetMainStepBackPos();
    virtual int GetPage();
    virtual void ShowHelperCount();
    virtual bool CheckListAndReturnResult( DWORD dwID, bool bCurStatus );
    virtual void SetQuestListID( DWORD dwID );
    virtual bool OpenListAndReturnResult( DWORD dwID, bool bCurStatus );
    virtual const UIRECT& GetListPageGlobalPos();
    virtual DWORD GetQuestMainID();
    virtual void LoadQuestMain( DWORD dwID );
    virtual void LoadQuestMainEnd( DWORD dwID );
};
