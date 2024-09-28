#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../enginelib/DxEffect/Single/DxEffSingleMan.h"

class GLGaeaClient;
class CInnerInterface;

class CaptureTheFlagEffectMan : public CUIGroup
{
    enum
    {
        EMFLAG_BLUE = 0,
        EMFLAG_RED,
    };

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;
    EngineDeviceMan* m_pEngineDevice;
    SPDXEFFSINGLEGROUP m_spRedFlagArrowEffect;
    SPDXEFFSINGLEGROUP m_spBlueFlagArrowEffect;

    float m_fTime;
    STARGETID m_vRedFlagGuideArrowTarget;
    STARGETID m_vBlueFlagGuideArrowTarget;

    bool m_bBlinkRedFlag;
    bool m_bBlinkBlueFlag;
    float m_fBlinkTimer;
    bool m_bBlinkState;

    bool bVisibleRedFlag;
    bool bVisibleBlueFlag;

public:
    CaptureTheFlagEffectMan(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CaptureTheFlagEffectMan();

    void CreateSubControl();

    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;
    virtual HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice ) override;

    HRESULT CreateFlagArrowEffect( LPDIRECT3DDEVICEQ pd3dDevice );
    
    void SetFlagGuideArrow( const int _flagNum, const STARGETID& sTargetID, const bool bBlink );
};

class MyCaptureTheFlagEffectMan : public ICaptureTheFlagEffectMan, public CaptureTheFlagEffectMan
{
public:
    MyCaptureTheFlagEffectMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyCaptureTheFlagEffectMan()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
    virtual void SetFlagGuideArrow( const int _flagNum, const STARGETID& sTargetID, const bool bBlink );
};