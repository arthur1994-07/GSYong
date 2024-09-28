#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CBasicLineBoxSmart;

class CForceRebirth : public CUIGroup
{
public:
    CForceRebirth( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~CForceRebirth() {};

private:
protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
    CBasicTextBox* m_pRebirthText;
	CBasicTextBox* m_pWatchIconText;

    CUIControl* m_pWatchIcon;
    CBasicLineBoxSmart* m_pBlank;
    CBasicLineBoxSmart* m_pGauge;

private:
    bool m_bForceRebirth;

    float m_fTime;
    float m_fMaxTime;

public:
    void CreateSubControl();
    bool IsForceRebirth();

    void SetForceRebirth( const float _fTime );

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
};