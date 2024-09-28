#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class CBasicButton;
class CBasicVarTextBox;
class GLGaeaClient;

class CBikeGetOffWindow : public CUIGroup
{
	enum
	{
		BIKE_GET_OFF_BUTTON = NO_ID + 1,
	};

public:
	CBikeGetOffWindow(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CBikeGetOffWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl ();

	CUIControl*	CreateControl( CString strControl, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, char* szButtonOver, UIGUID ControlID );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID cID, DWORD dwMsg );

private:
	CBasicButton*	m_pGetOffButton;
	CBasicVarTextBox*	m_pInfo;
};