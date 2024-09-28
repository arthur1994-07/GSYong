#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class CBasicButton;
class CBasicVarTextBox;
class GLGaeaClient;

class CBikeBoostWindow : public CUIGroup
{
	enum
	{
		BIKE_GAGE_BACK = NO_ID + 1,
		BIKE_BUTTON_BACK,
		BIKE_GAGE,
		BIKE_BUTTON
	};

public:
	CBikeBoostWindow(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CBikeBoostWindow();

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
	CUIControl* m_pGageBack;
	CUIControl* m_pGage;
	CUIControl* m_pButtonBack;
	CUIControl* m_pButton;

};