#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "./ActivityWindowPage.h"

class CBasicTextBox;
class CBasicLineBoxSmart;
class GLGaeaClient;


class ActivityListSlot : public CUIGroup
{
public:    
    enum
    {
        SPEC_ACT_SLOT_PRESENT = NO_ID + 1,
    };

	enum SPEC_ACT_SLOT_TYPE
    {
        SPEC_ACT_SLOT_TYPE_IDLE,         //  일반
        SPEC_ACT_SLOT_TYPE_CONTINUE,     //  진행중
        SPEC_ACT_SLOT_TYPE_NOTCOMP,      //  진행불가
        SPEC_ACT_SLOT_TYPE_COMP,         //  완료
        SPEC_ACT_SLOT_TYPE_SIZE, 
	};

public:
    static const float fDESC_ANIMATION_BEGIN_DALAY;
    static const float fDESC_ANIMATION_SPEED_DALAY;

public:
	ActivityListSlot(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~ActivityListSlot();
    
    void	CreateSubControl ();

public:

    CUIControl*     m_pArrivalImage; 
    CUIControl*     m_pAllImage;


    CBasicLineBoxSmart*     m_pPresentBack[SPEC_ACT_SLOT_TYPE_SIZE];
    CBasicLineBoxSmart*     m_pRewardBack[SPEC_ACT_SLOT_TYPE_SIZE];
    
    CUIControl*         m_pComplateImage;
    CBasicTextBox*      m_pCompText;       

    CBasicTextBox*      m_pArrivalText;

    CBasicTextBox*      m_pTitleText;
    CBasicTextBox*      m_pDescText;
    CBasicTextBox*      m_pProgressText;

    CBasicTextBox*      m_pRewardTitleText;
    CBasicTextBox*      m_pRewardPointText;

    CBasicLineBoxSmart* m_pRewardTitleLineBox;
    CBasicLineBoxSmart* m_pRewardPointLineBox;

    CUIControl*         m_pRewardTitleImage;
    CUIControl*         m_pRewardPointImage;

    Activity            m_Activity;
    SPEC_ACT_SLOT_TYPE  m_emSlotType;

public:
    CString             m_strDesc;
    bool                m_bAniDesc;
    UINT                m_nAniDescCur;
    UINT                m_nAniDescMax;
    float               m_fAniTime;

    bool                m_bMouseOver;
    float               m_fMouseOverTime;

protected:
    GLGaeaClient* m_pGaeaClient;

public:


    CBasicTextBox*	CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign, UIGUID nWndID = NO_ID );



    void    SetSlotType( Activity& cActivity );
    void    SetArrival ( int nCount );
    void    SetComplate( SPEC_ACT_SLOT_TYPE emSlotType );
    
    //  임시 셋팅함수
    void    SetTitle( const CString& strTitle );
    void    SetDesc( const CString& strText );
    void    SetProgress( Activity& cActivity );
    void    SetReward( const CString& strTitle, int nPoint );
    void    SetSpecAct( const Activity& cActivity );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
};