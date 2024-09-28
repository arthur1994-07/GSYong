#pragma once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class CTipBox;
class CBasicTextBox;
class GLGaeaClient;

class CMouseTip : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

private:
	static	const	int	nTIME_MINUTE;

public:
	CMouseTip(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CMouseTip(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	int			m_nSize_X;
	int			m_nSize_Y;
	float		m_fFlowTime;

private:
	CTipBox*	m_pMain;
	CUIControl*		m_pLeftDir;
	CUIControl*		m_pRightDir;
	CUIControl*		m_pUpDir;
	CUIControl*		m_pDownDir;
	CUIControl*		m_pMouse;
	CUIControl*		m_pMouseButton;
	CUIControl*		m_pMouseScroll;
	CBasicTextBox*	m_pTipText;
	CBasicTextBox*	m_pTitleText;

protected:
	CInnerInterface* m_pInterface;

};