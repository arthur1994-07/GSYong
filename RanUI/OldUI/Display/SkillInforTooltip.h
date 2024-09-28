#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CBasicLineBoxSmart;
class CItemImage;
class CBasicTextBox;
class CBasicVarTextBox;
class CSkillImage;
struct GLCHARLOGIC;

class CSkillInfoTooltip : public CUIGroup
{
private:
    enum
    {
        MAIN = NO_ID + 1,
        SKILL_IMAGE,
		CLOSE_BUTTON,
    };

    enum SKILL_INFO_TYPE
    {
        INFO_NONE_TYPE,

        NORMAL_DURATION_PLUS,
        NORMAL_DURATION_MINUS,

        NORMAL_NONDURATION_PLUS,
        NORMAL_NONDURATION_MINUS,

        PASSIVE_DURATION_PLUS,
        PASSIVE_DURATION_MINUS,

        PASSIVE_NONDURATION_PLUS,
        PASSIVE_NONDURATION_MINUS,
    };

    enum SKILL_INFO_STATE_BLOW
    {
        VALUE_COUNT_0,
        VALUE_COUNT_1_VAR1,
        VALUE_COUNT_1_VAR2,
        VALUE_COUNT_2,
    };

public:

    enum SKILL_TOOLTIP_TYPE
    {
        BASIC_TOOLTIP,
        MULTISKILL_MAIN_TOOLTIP,
        MULTISKILL_SUB_TOOLTIP,
		MULTISKILL_LINK_TOOLTIP,
		MULTISKILL_DELAY_ACTION_TOOLTIP
    };

public:
    CSkillInfoTooltip( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CSkillInfoTooltip();

private:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
	CBasicButton* m_pClose;
    CBasicLineBoxSmart* m_pLineBox;
    CUIControl* m_pDummyControl;
    CUIControl* m_pDummyNameControl;
    CUIControl* m_pDummyLabel8Control;
    CSkillImage* m_pSkillImage;

    CBasicTextBox* m_pSkillName;
	CBasicTextBox* m_pSkillLabel;
	CBasicTextBox* m_pSkillLabel0;
    CBasicTextBox* m_pSkillLabel1;
    CBasicTextBox* m_pSkillLabel2;
    CBasicTextBox* m_pSkillLabel3;
    CBasicTextBox* m_pSkillLabel4;
    CBasicTextBox* m_pSkillLabel5;
    CBasicTextBox* m_pSkillLabel6;
	CBasicTextBox* m_pSkillLabel7;
    CBasicTextBox* m_pSkillLabel8;

private:
    bool m_bBlockMousetracking;
    D3DXVECTOR2 m_vMousePointGap;

	int m_PosX;
	int m_PosY;
	D3DXVECTOR2 m_vDragPos;

	GLCHARLOGIC* m_pTarget;
    const GLSKILL* m_pSkillData;
    const SCHARSKILL* m_pCharSkill;
    WORD m_wLevel;          // 스킬 레벨 (배열상의 레벨);
    WORD m_wShowLevel;      // 보이는 레벨 (배열보다 1크다);
    bool m_bNextLevel;

	UIRECT m_rcpSkillImage;
	UIRECT m_rcSkillName;
	UIRECT m_rcDummuname;
	UIRECT m_rcSkillLabel0;
	UIRECT m_rcSkillLabel1;
	UIRECT m_rcSkillLabel2;

    const CSkillInfoTooltip* m_pOthertMultiSkillTooltip; //멀티스킬의 경우만 사용 (메인스킬은 서브스킬을, 서브스킬은 메인스킬 툴팁을 저장하고 있음);
	const CSkillInfoTooltip* m_pLinkSkillTooltip;
	const CSkillInfoTooltip* m_pDealyActionSkillTooltip;

    WORD m_emTooltipType;
public:
    void CreateSubControl();
	void CreateCloseButton();
    void AllControlRePos( int x, int y );

	void SetBlockMouseTracking( bool bTracking );

    void DisplaySkillInformation( GLCHARLOGIC* pTarget, const GLSKILL* pSkill, const WORD wShowLevel, const WORD _emTooltipType = CSkillInfoTooltip::BASIC_TOOLTIP, const bool bShowReq = false, const bool bShowLv = true );

    void ResetInformation();
    void SetOtherMultiSkillTooltip( const CSkillInfoTooltip* pSkillTooltip );
	void SetLinkSkillTooltip( const CSkillInfoTooltip* pSkillTooltip );
	void SetDealyActionSSkillTooltip( const CSkillInfoTooltip* pSkillTooltip );
	void SetBGAlpha(int nValue);

	SNATIVEID GetSkillID();

private:
    void SetSkillImage();  // Info : 스킬 이미지;
    void SetSkillName(const bool bShowLv = true);  // Info : 스킬 이름;
	void SetInformationLabel0_1();  // Info : 추가스킬;
	void SetInformationLabel0_2();  // Info : 링크스킬;
	void SetInformationLabel0_3();  // Info : 지연스킬;
    void SetInformationLabel1();  // Info : 등급, 속성;
    void SetInformationLabel2( BOOL bMultiSkill );  // Info : 적용 타입, 적용무기, 소모조건;
    void SetInformationLabel3();  // Info : 습득 조건;
    void SetInformationLabel4();  // Info : 스킬 영역;
    void SetInformationLabel5();  // Info : 적용 타입;
    void SetInformationLabel5_1();// Info : 적용 타입;
    void SetInformationLabel5_2();  // Info : 적용 타입;
    void SetInformationLabel6();  // Info : 상태이상 효과;
	void SetInformationLabel7();  // Info : 발동 스킬 설명;
    void SetInformationLabel8();  // Info : 스킬 설명;

    std::string Skill_ToolTip_Test_lua( float fLife, 
        float fVAR1_SCALE, 
        float fVAR2_SCALE,
        EMSPEC_ADDON emSPEC,
        const SKILL::SSPEC& sSPEC);

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	void TranslateMessage( DWORD dwMsg );
};
