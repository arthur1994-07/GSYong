#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CNotifyCTFSpurt : public CUIGroup
{

private:
    enum
    {
        BACK_IMAGE = NO_ID + 1,
    };

public:
    CNotifyCTFSpurt( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    ~CNotifyCTFSpurt() {};

public:
    void CreateSubControl ();

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle(BOOL visible);

public:
    void CheckCTFSpurt();

    void SetSpurtSchool( ENUM_SCHOOL emSchool );
    void VisibleSpurt( ENUM_SCHOOL emSchool );

    void UnShowNotify();

protected:
    GLGaeaClient*    m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
    CUIControl* m_pNotice;
    CUIControl* m_pWinSM;
    CUIControl* m_pWinHA;
    CUIControl* m_pWinBH;

private:
    ENUM_SCHOOL m_emSchool;

    bool m_bEnableVisible;
    bool m_bVisible;
    bool m_bBlinkView;

    float m_fStartTime;
    float m_fEndTime;
    float m_fBlinkTime;
};