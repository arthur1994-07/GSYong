#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "NewCreateCharEnum.h"

class COuterInterface;
class CBasicTextBox;
class CCreateCharacterMenu;
class CCreateCharacterSchool;
class CCreateCharacterSex;
class CCreateCharacterClass;
class CBasicButton;
class GLGaeaClient;

class CCreateCharacterWindow : public CUIGroup
{
private:
	enum
	{
		CREATE_MENU_PAGE = NO_ID + 1,
		CREATE_SCHOOL_PAGE,
		CREATE_SEX_PAGE,
		CREATE_CLASS_PAGE,

		ROTATE_LEFT_BUTTON,
		ROTATE_RIGHT_BUTTON,
	};

private:
	CCreateCharacterMenu*	m_pCharMenu;
	CCreateCharacterSchool* m_pCharSchool;
	CCreateCharacterSex*	m_pCharSex;
	CCreateCharacterClass*	m_pCharClass;

	CUIControl* m_pLeft;
	CUIControl* m_pRight;

public:
	CCreateCharacterWindow(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CCreateCharacterWindow();

	CCreateCharacterMenu* GetMenu() { return m_pCharMenu;}

	void CreateSubControl ();

	void SetData( INT nSchool, INT nSex, INT nClass );
	void ResetData();

	BOOL IsCancelButton(); 
	void UseCancelButton(); 

	CUIControl*	CreateControl( CString strControl, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
	CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void SetVisibleSingle( BOOL bVisible );

protected:
	COuterInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;
};