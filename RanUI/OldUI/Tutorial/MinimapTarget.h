#pragma once

#include "../Util/UIWindowEx.h"

class GLGaeaClient;

class CMinimapTarget : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

public:
	CMinimapTarget(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual ~CMinimapTarget(void);

public:
    static	const int nTIME_MINUTE;

private:
    GLGaeaClient* m_pGaeaClient;
    float		m_fFlowTime;
    int			m_iTargetX, m_iTargetY;
    bool		m_bEnable;
    CUIControl* m_pMiniMapTargetMain;
    CUIControl* m_pMiniMapTargetBody;

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );	

    void CreateSubControl();
    bool IsEnable() const { return m_bEnable; }
    void SetEnable(bool State) { m_bEnable=State; }
};