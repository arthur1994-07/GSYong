#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "NewCreateCharEnum.h"

class COuterInterface;
class CBasicButton;
class GLGaeaClient;

class CRotateCharacter : public CUIGroup
{
private:
	enum
	{
		ROTATE_LEFT_BUTTON = NO_ID + 1,
		ROTATE_RIGHT_BUTTON,
	};
public:
	CRotateCharacter(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CRotateCharacter();

	void CreateSubControl ();

	CUIControl*	CreateControl( CString strControl, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
	CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void SetVisibleSingle( BOOL bVisible );

private:
	CBasicButton* m_pButtonLeft;
	CBasicButton* m_pButtonRight;

protected:
	COuterInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;
};