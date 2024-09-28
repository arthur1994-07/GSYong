#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CTipBox;
class CBasicTextBox;
class CInnerInterface;
class GLGaeaClient;

class CBalloonText : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

private:
	static	const	int	nTIME_MINUTE;

public:
	CBalloonText(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CBalloonText(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

	float		m_fFlowTime;

public:
	int			m_iTargetX, m_iTargetY;

private:
	CUIControl*	m_pMain;
	CUIControl*	m_pEdgeLT;
	CUIControl*	m_pEdgeRT;
	CUIControl*	m_pEdgeLB;
	CUIControl*	m_pEdgeRB;
	CUIControl* m_pLClick;
	CUIControl* m_pRClick;
	CUIControl* m_pMClick;
	CUIControl* m_pScroll;
	CUIControl* m_pNull;
	CBasicTextBox*	m_pText;
	CBasicTextBox*	m_pNormalText;
	CBasicTextBox*	m_pSkillText;

protected:
	CInnerInterface* m_pInterface;

};

class CBalloonSpaceBar : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

private:
	static	const	int	nTIME_MINUTE;

public:
	CBalloonSpaceBar(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CBalloonSpaceBar(void);

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
	void	CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float		m_fFlowTime;

private:
	CUIControl*	m_pMain;
	CBasicTextBox*	m_pText;
};

class MyBalloonText : public IBalloonText, private CBalloonText
{
public:
    MyBalloonText( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice );
    virtual ~MyBalloonText()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetUIWindowGlobalPos( const UIRECT& rcPos );
    virtual void SetUIWindowLocalPos( const UIRECT& rcPos );
};

class MyBalloonSpaceBar : public IBalloonSpaceBar, private CBalloonSpaceBar
{
public:
    MyBalloonSpaceBar( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyBalloonSpaceBar()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
};