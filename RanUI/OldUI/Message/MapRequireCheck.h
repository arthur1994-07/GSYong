#pragma once

#include "../../../EngineLib/GUInterface/UIWindow.h"
//#include "../../../RanLogicClient/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CBasicTextBox;
class CBasicTextButton;
class GLGaeaClient;

class CMapRequireCheck : public CUIWindow
{
private:
	static const float fDEFAULT_TIME_LEFT;
	enum
	{
		MAP_REQUIRE_CHECK_BUTTON = ET_CONTROL_NEXT
	};
public:
	CMapRequireCheck(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CMapRequireCheck(void);

	void	CreateSubControl();
	CBasicTextButton* CreateTextButton( char* szButton, UIGUID ControlID , char* szText );

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	virtual void SetVisibleSingle( BOOL bVisible );
    void SetMapCheckType( UI::EMMAPCHECK_TYPE emMapCheckType );

private:
	CBasicTextBox*		m_pTextBox;
	CBasicTextButton*	m_pButton;
	float				m_fLeftTime;
    UI::EMMAPCHECK_TYPE	m_emMapCheckType;

    bool                m_bActive;

protected:
	CInnerInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;

};

class MyMapRequireCheck : public IMapRequireCheck, private CMapRequireCheck
{
public:
    MyMapRequireCheck( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice );
    virtual ~MyMapRequireCheck()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
    virtual void SetMapCheckType( UI::EMMAPCHECK_TYPE emMapCheckType );
};