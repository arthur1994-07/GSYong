#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

class CAuthenticatorCTFDisplay : public CUIGroup
{
public:
    CAuthenticatorCTFDisplay(EngineDeviceMan* pEngineDevice);
    virtual	~CAuthenticatorCTFDisplay ();

public:
    void	SetPercentSM ( float fPercent );
    void	SetPercentHA ( float fPercent );
    void	SetPercentBH ( float fPercent );

public:
    void	SetAuthSchool ( ENUM_SCHOOL emSchool ) { m_emAuthSchool = emSchool; }

public:
    void    Update();

public:
    void	CreateSubControl ();

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
    void         SetImageCulling ( CUIControl* pBase, CUIControl* pImage, float fPercent );

private:
    CUIControl*  m_pImage[SCHOOL_NUM];
    CUIControl*  m_pImage_Over[SCHOOL_NUM];
    CUIControl*  m_pImage_Over_Base[SCHOOL_NUM];
    CUIControl*  m_pImage_Back[SCHOOL_NUM];
    CUIControl*  m_pImage_FightSign[SCHOOL_NUM];
    CUIControl*  m_pImage_Gage[SCHOOL_NUM];

    CUIControl*  m_pImageAuth;

private:
    float        m_fPercent[SCHOOL_NUM];
    float        m_fPercentBlinkTime[SCHOOL_NUM];
    ENUM_SCHOOL  m_emAuthSchool;

private:
    static const float s_fDefaultBlinkTime;

};