#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Activity/ActivityBase.h"

class ActivityListButton;
class ActivityListSlot;
class CBasicTextBox;
class CBasicScrollBarEx;
class GLGaeaClient;

class ActivityMainPage : public CUIGroup
{
private:
    enum 
    {
        SPEC_ACT_LIST_PAGE_ALL      = 0,
        SPEC_ACT_LIST_PAGE_PROG     = 1,
        SPEC_ACT_LIST_PAGE_COMP     = 2,
        SPEC_ACT_LIST_PAGE_NOTCOMP  = 3,
        SPEC_ACT_LIST_PAGE_NUM      = 4,
        
        SPEC_ACT_LIST_SLOT_NUM = 6,
    };

	enum
	{
		SPEC_ACT_LIST_ALL_BUTTON = NO_ID + 1,
        SPEC_ACT_LIST_PROG_BUTTON,
        SPEC_ACT_LIST_COMP_BUTTON,
        SPEC_ACT_LIST_NOTCOMP_BUTTON, 
	};

public:
	ActivityMainPage(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~ActivityMainPage();
    
    void	CreateSubControl ();
    CBasicTextBox*	CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign, UIGUID nWndID = NO_ID );

    void    SetPage( const VEC_ACTIVITIES& vecSpecAct );
    void    UpdatePage( const VEC_ACTIVITIES& vecSpecAct );
    
    void    SetVisiblePage();

    void    UpdatePageData();
    void    UpdateSlot();

protected:
    GLGaeaClient* m_pGaeaClient;
    

public:
    ActivityListButton* m_pListButton[SPEC_ACT_LIST_PAGE_NUM];
    ActivityListSlot*   m_pListSlot[SPEC_ACT_LIST_SLOT_NUM];
    CBasicScrollBarEx* m_pSlotListScroll;

    int            m_nListScrollPos;
    int            m_nPage;
    VEC_ACTIVITIES m_vecSpecAct;
    VEC_ACTIVITIES m_vecSpecActPage;

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
};