#pragma once
#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

#include "../Util/TapSelectorEx.h"

#include "RnSkillSlot.h"

typedef std::vector< SKILLINFO > SkillInfoVec;


class CBasicScrollBarEx;
class CBasicTextBox;
class CRnSkillSlot;
class CBasicLineBoxSmart;
class RnSkillNomalToolTip;
class RnSkillHelpToolTip;
class CMessageBox;


class RnSkillWindow : public CUIWindowEx, private CTapSelectorEx<DEFAULT_TAPBUTTON>
{
private:
    enum
    {
        SKILLSLOT_X_SIZE = 2,
        SKILLSLOT_Y_SIZE = 9,
        SKILLSLOT_COUNT = SKILLSLOT_X_SIZE * SKILLSLOT_Y_SIZE,

        TAB_COUNT = 4,
        TAB_HIGHLIGHT = 9,
    };

    enum
    {
        ID_RNSKILL_TAB = ET_CONTROL_NEXT + 1,
        ID_RNSKILL_TAB_0,
        ID_RNSKILL_TAB_1,
        ID_RNSKILL_TAB_2,
        ID_RNSKILL_TAB_3,
        ID_RNSKILL_HELP_BUTTON,
        ID_RNSKILL_SCROOLBAR,

        ID_RNSKILL_RESET_BUTTON,
        ID_RNSKILL_CONFIRM_BUTTON,

        ID_RNSKILL_MESSAGEBOX,

        ID_SKILL_SLOT_RECT_CONTROL = 100,
        ID_SKILL_SLOT_RECT_CONTROL_START = ID_SKILL_SLOT_RECT_CONTROL,
        ID_SKILL_SLOT_RECT_CONTROL_END = ID_SKILL_SLOT_RECT_CONTROL_START + SKILLSLOT_COUNT,

    };

public:
    typedef CTapSelectorEx::BUTTON STAPBUTTON;

    RnSkillWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    void                CreateSubControl();
    virtual void        Refresh();
    virtual	void        SetVisibleSingle ( BOOL bVisible );
    std::vector<SNATIVEID> GetAllSkillID();
    
protected:
    virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    void                TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg );
    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;

private:
    CBasicLineBoxSmart* GetLineBox( unsigned int nIndex );
    void                EventSelectedTap( UIGUID controlID );
    void                SetResetToolTip();
    void                SetConfirmToolTip();
    void                SetCheckToolTip();
    void                ResetTabButtonText();
    void                Reset();
    CBasicScrollBarEx*  m_pScrollBar;
    CBasicTextBox*	    m_pSkillPageText[ TAB_COUNT ];
    CRnSkillSlot*       m_SkillSlot[ SKILLSLOT_COUNT ];
    CBasicLineBoxSmart* m_pYLine;
    CBasicLineBoxSmart* m_pGLine;
    CBasicLineBoxSmart* m_pBLine;
    CBasicLineBoxSmart* m_pCurrentLine;
    CRnSkillSlot*       m_pRollOverSlot;
    RnButton*           m_pButtonReset;
    RnButton*           m_pButtonConfirm;
    

    RnSkillNomalToolTip* m_pResetToolTip;
    RnSkillNomalToolTip* m_pConfirmToolTip;
    RnSkillNomalToolTip* m_pCheckToolTip;
    RnSkillHelpToolTip* m_HelpTooltip;

    CMessageBox*        m_pMessageBox;

private:
    SkillInfoVec&       GetCurrentSkillInfoVec();                   // 현재 TAB 스킬정보
	SkillInfoVec&       GetCurrentSkillInfoVec(int iPage);                   // 현재 TAB 스킬정보

    SKILLINFO*          GetLearnSkill( SNATIVEID skill_id );        // 습득한 스킬인지? (임시 포함)
	SKILLINFO*			GetLearnSkillAll( SNATIVEID skill_id );

    SKILLINFO*          GetSkillInfo( unsigned int nIndex );        // 해당 스킬정보
    DWORD               GetUsingSkillPoint( unsigned int nIndex );  // 스킬의 임시로 사용된 포인트
    DWORD               GetUsingSkillPoint( const SKILLINFO& sSkillInfo );  // 스킬의 임시로 사용된 포인트
    DWORD               GetUsingSkillPointPage( unsigned int nPage ); // 임시로 해당 페이지에 사용된 포인트
    DWORD               GetUsingSkillPointAll();                    // 모든 임시로 사용된 포인트
    DWORD               GetUsingSkillMoneyAll();
    

    BOOL                IsMaster( unsigned int nIndex );
    BOOL                IsLevelUP( unsigned int nIndex );
    BOOL                IsLevelDown( unsigned int nIndex );
    SKILL_NEED          CheckSkillPrecedent( unsigned int nIndex );

    void                InitSkillPage();
    void                GetSkillList( UINT nPage, SkillInfoVec& vecSkill );
    DWORD				GetSkillPoint( SkillInfoVec& vecSkill );
    void                GetSkillIndexRange( EMCHARCLASS eClass, int& nBegin, int& nEnd );
    const char*         GetClassText( int nPage );
    unsigned int        m_nCurrentPage;
    CBasicTextBox*	    m_pSkillPoint;
    DWORD               m_dwSkillPointBACK;
    int                 m_nViewPos;
    BOOL                m_bInit;
    SkillInfoVec        m_vecSkillTab[TAB_COUNT];

};


class MyRnSkillWindow : public ISkillWindow, private RnSkillWindow
{
public:
    MyRnSkillWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
        : RnSkillWindow( pGaeaClient, pInterface, pEngineDevice )
    {
    }

public:
    virtual void CreateUIWindowAndRegisterOwnership()
    {
        RnSkillWindow::Create( SKILL_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        RnSkillWindow::CreateSubControl();

        RnSkillWindow::m_pInterface->UiRegisterControl( this, true );
        RnSkillWindow::m_pInterface->UiShowGroupFocus( SKILL_WINDOW );
    }

    virtual void ShowPageForTutorial()
    {
        // 미구현
    }

    virtual const UIRECT& GetUIWindowGlobalPos()
    {
        return RnSkillWindow::GetGlobalPos();
    }

    virtual void Refresh()
    {
        RnSkillWindow::Refresh();
    }
};

