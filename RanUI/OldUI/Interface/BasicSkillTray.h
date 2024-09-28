#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Skill/GLSkill.h"
#include "SkillTrayTab.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_SKILLSLOT = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CInnerInterface;
class CBasicQuickSkillSlotEx;
class CBasicVarTextBox;
class GLGaeaClient;
struct SCHARSKILL;

class CBasicSkillTray : public CUIGroup
{
private:
    static	const	int	nLBUPSKIP;

protected:
    enum
    {
        QUICK_SKILL_SLOT1 = NO_ID + 1,
        QUICK_SKILL_SLOT2,
        QUICK_SKILL_SLOT3,
        QUICK_SKILL_SLOT4,
        QUICK_SKILL_SLOT5,

        QUICK_SKILL_SLOT6,
        QUICK_SKILL_SLOT7,
        QUICK_SKILL_SLOT8,
        QUICK_SKILL_SLOT9,
        QUICK_SKILL_SLOT0,

        QUICK_SKILL_TRAY_TAB_INFO
    };

    enum SKILL_INFOR_TYPE
    {
        INFOR_NONE_TYPE,

        NORMAL_DURATION_PLUS,
        NORMAL_DURATION_MINUS,

        NORMAL_NONDURATION_PLUS,
        NORMAL_NONDURATION_MINUS,

        PASSIVE_DURATION_PLUS,
        PASSIVE_DURATION_MINUS,

        PASSIVE_NONDURATION_PLUS,
        PASSIVE_NONDURATION_MINUS,
    };

public:
    CBasicSkillTray(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CBasicSkillTray ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
    void	CreateSubControl( INT nTabIndex );

private:
    void	UpdateSkillInfo ( SNATIVEID sNativeID );
    void	LoadSkillInfo ( PGLSKILL const pSkill, SCHARSKILL* const pCharSkill );
    void    SetInformationSkillInfo( PGLSKILL const pSkill, SCHARSKILL* const pCharSkill );

public:
    virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
    void 	SetShotcutText ( DWORD nID, CString& strTemp );
	void	SetVisibleReservedSkill( const bool bEnable );
	const UIRECT& GetSlotGlobalPos( const WORD wSlot );

public:
    virtual void TurnHorizontal ();
    virtual void TurnVertical   ();

public:
    void         SetSkillInfoVisibleOff ();
	void		 ResetSlotText();
	void		 ResetLinkSkillText();

	// 백터에 중복 아이디가 있는지 체크;
	// 중복되는 발동스킬을 알 수 없어서 임시로 저장후 확인;
	bool		 IsHaveSkill( const SNATIVEID& sNativeID );

public:
    CBasicQuickSkillSlotEx*		m_pSlotEx[QUICK_SKILL_SLOT_MAX];
    CUIControl*                 m_pSlotEnd;

private:	
    CBasicVarTextBox*	m_pSkillInfo;
	CBasicVarTextBox*	m_pSkillInfoMulti;
	CBasicVarTextBox*	m_pSkillInfoLink[SKILL::MAX_ADDON];
	CBasicVarTextBox*	m_pSkillInfoDealyAction;
	std::vector<SNATIVEID> m_vecSkillInfoLinkID;

private:
    int	m_nLBUPSKIP;

protected:
    CInnerInterface* m_pInterface;

};