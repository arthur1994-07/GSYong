#pragma once
#include "../ReferChar/ReferSkillSlot.h"
#include "../../InnerInterface.h"

class RnButton;
class CBasicTextBoxEx;

struct SKILL_NEED
{
    // 해당 수치가 0 이라면 이미 만족한 경우다.
    BOOL bPrecedentSkill;       // (FALSE == 이미 배웠거나 선행스킬 없음, TRUE == 선행스킬이 존재 )
    DWORD dwLevel;              // 요구 레벨 
    DWORD dwSkillPoint;         // 요구 스킬포인트
    DWORD dwPow;                // 힘
    DWORD dwDex;                // 민첩 
    DWORD dwSpi;                // 정신
    DWORD dwStr;                // 체력
    DWORD dwSta;                // 근력
    DWORD dwGold;               // 사용 골드
    BOOL  bUseScrool;           // 스크롤 사용 여부
};

struct SKILLINFO
{
    PGLSKILL            pSkill;
    DWORD               dwLevel;

    DWORD               dwTempSkillLevel;       // 임시로 올라간 레벨이 저장된다.
    BOOL                bMaster;                // 현재 스킬이 마스터인지 아닌지(임시 상승 포함)
    int                 nState;                 // 현재 상태
    DWORD               dwUsingSkillPoint;      // 사용된 스킬 포인트
    SKILL_NEED          sNeed;                  // STATE_IMPOSSIBLE 상태일때 필요한 정보
};
////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_RNSKILL_UP_CLICK = UIMSG_USER1;
const DWORD UIMSG_RNSKILL_DOWN_CLICK = UIMSG_USER2;

class CRnSkillSlot :  public CUIGroup 
{
private:
    enum
    {
        ID_SLOT =  NO_ID + 1,
        ID_SLOT_IMAGE,
        ID_SLOT_MOUSEROVER_IMAGE,
        ID_SLOT_BUTTON_PLUS,
        ID_SLOT_BUTTON_MINUS,
    };

public:
    enum
    {
        STATE_IMPOSSIBLE,               // 활성 불가능 - 스킬 조건을 만족시키지 못함
        STATE_DEACTIVATE,               // 비활성화 - 활성화 가능 상태
        STATE_ACTIVATE,                 // 활성 스킬 - 스킬 레벨 변화 없음
        
        STATE_TEMP = 100,               // 임시 상승 값
        STATE_DEACTIVATE_TEMP = STATE_DEACTIVATE + STATE_TEMP,  // 비활성 -> 임시 상승 
        STATE_ACTIVATE_TEMP = STATE_ACTIVATE + STATE_TEMP,      // 활성   -> 임시 상승
        
    };

    CRnSkillSlot( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    void	            CreateSubControl();
    virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    void		        SetSkillInfo( SKILLINFO* pSkillInfo, BOOL bRollOver );
    const SKILLINFO&    GetSkillInfo();
private:
    void                SetSlotState();
    void                RollOver( BOOL bRollover );
    
    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;

    CSkillImage*	    m_pSkillImage;
    CUIControl*         m_pShadow;
    CBasicTextBoxEx*    m_pTextBoxLine0;
    CBasicTextBoxEx*    m_pTextBoxLine1;
    RnButton*           m_pRnButtonPlus;
    RnButton*           m_pRnButtonMinus;

    SKILLINFO*          m_pSkillInfo;
};
