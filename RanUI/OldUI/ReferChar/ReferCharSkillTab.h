#pragma once

#include "../Util/UIGroupHelper.h"
#include "../Util/TapSelectorEx.h"
#include "../../InnerInterface.h"

class CReferSkillSlot;
class CBasicScrollBarEx;

typedef std::vector< PGLSKILL > SkillInfoVec;

class CReferCharSkillTab : public CUIGroupHelper, private CTapSelectorEx<DEFAULT_RADIO_BUTTON>
{
private:
    enum
    {
        SKILLSLOT_X_SIZE = 2,
        SKILLSLOT_Y_SIZE = 10,
        SKILLSLOT_COUNT = SKILLSLOT_X_SIZE * SKILLSLOT_Y_SIZE,

		TAB_COUNT = 4,
		TAB_HIGHLIGHT = 10,
    };

	enum
    {
        ID_SKILL_TAB_0 = NO_ID + 1,
        ID_SKILL_TAB_1,
        ID_SKILL_TAB_2,
        ID_SKILL_TAB_3,
        
        ID_SKILL_SCROOLBAR,

        ID_SKILL_SLOT_RECT_CONTROL = 100,
        ID_SKILL_SLOT_RECT_CONTROL_START = ID_SKILL_SLOT_RECT_CONTROL,
        ID_SKILL_SLOT_RECT_CONTROL_END = ID_SKILL_SLOT_RECT_CONTROL_START + SKILLSLOT_COUNT,
    };

public:
    CReferCharSkillTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CReferCharSkillTab();

	void                CreateSubControl();
    virtual void        TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    void                Refresh();

private:
    void                EventSelectedTap(UIGUID controlID);

    void                InitSkillPage( UINT nPage );
	void                GetSkillList( UINT nPage, SkillInfoVec& vecSkill );
	DWORD				GetSkillPoint( SkillInfoVec& vecSkill );

	GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;

	bool				m_bInit;
    GLCHARLOGIC*        m_pTarget; 

	CBasicScrollBarEx*  m_pScrollBar;
    CBasicTextBox*	    m_pSkillPageText[4];
    CReferSkillSlot*    m_SkillSlot[ SKILLSLOT_COUNT ];

	int					m_nMouseX;
	int					m_nMouseY;

	SkillInfoVec        m_vecCurrentSkill;
    int                 m_nViewPos;
};