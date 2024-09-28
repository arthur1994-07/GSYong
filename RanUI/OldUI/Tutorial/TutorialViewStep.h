#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CTipBox;
class CUIWindowEx;

class CTutorialViewStep : public CUIGroup
{
public:
	CTutorialViewStep(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CTutorialViewStep();

protected:
	enum
	{
		TUTORIAL_VIEW_STEP_MAIN = NO_ID + 1,
	};

public:
	void	CreateSubControl();
	void	SetOnStep( int nIndex );

public:
	virtual	void	TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	int	m_iIndex;

private:
	CUIWindowEx*	m_pMain;
	CBasicTextBox*	m_pTitle;
	CBasicTextBox*	m_pStepBox[ 9 ];
	CString			m_pStepText[ 9 ];

protected:
	CInnerInterface* m_pInterface;

};

class MyTutorialViewStep : public ITutorialViewStep, private CTutorialViewStep
{
public:
    MyTutorialViewStep( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyTutorialViewStep()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual const UIRECT& GetUIWindowGlobalPos();
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos );
    virtual void SetUIWindowLocalPos( const D3DXVECTOR2& vPos );
    virtual void SetOnStep( int nIndex );
};