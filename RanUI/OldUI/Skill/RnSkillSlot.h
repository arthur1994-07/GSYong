#pragma once
#include "../ReferChar/ReferSkillSlot.h"
#include "../../InnerInterface.h"

class RnButton;
class CBasicTextBoxEx;

struct SKILL_NEED
{
    // �ش� ��ġ�� 0 �̶�� �̹� ������ ����.
    BOOL bPrecedentSkill;       // (FALSE == �̹� ����ų� ���ེų ����, TRUE == ���ེų�� ���� )
    DWORD dwLevel;              // �䱸 ���� 
    DWORD dwSkillPoint;         // �䱸 ��ų����Ʈ
    DWORD dwPow;                // ��
    DWORD dwDex;                // ��ø 
    DWORD dwSpi;                // ����
    DWORD dwStr;                // ü��
    DWORD dwSta;                // �ٷ�
    DWORD dwGold;               // ��� ���
    BOOL  bUseScrool;           // ��ũ�� ��� ����
};

struct SKILLINFO
{
    PGLSKILL            pSkill;
    DWORD               dwLevel;

    DWORD               dwTempSkillLevel;       // �ӽ÷� �ö� ������ ����ȴ�.
    BOOL                bMaster;                // ���� ��ų�� ���������� �ƴ���(�ӽ� ��� ����)
    int                 nState;                 // ���� ����
    DWORD               dwUsingSkillPoint;      // ���� ��ų ����Ʈ
    SKILL_NEED          sNeed;                  // STATE_IMPOSSIBLE �����϶� �ʿ��� ����
};
////////////////////////////////////////////////////////////////////
//	����� �޽��� ����
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
        STATE_IMPOSSIBLE,               // Ȱ�� �Ұ��� - ��ų ������ ������Ű�� ����
        STATE_DEACTIVATE,               // ��Ȱ��ȭ - Ȱ��ȭ ���� ����
        STATE_ACTIVATE,                 // Ȱ�� ��ų - ��ų ���� ��ȭ ����
        
        STATE_TEMP = 100,               // �ӽ� ��� ��
        STATE_DEACTIVATE_TEMP = STATE_DEACTIVATE + STATE_TEMP,  // ��Ȱ�� -> �ӽ� ��� 
        STATE_ACTIVATE_TEMP = STATE_ACTIVATE + STATE_TEMP,      // Ȱ��   -> �ӽ� ���
        
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
