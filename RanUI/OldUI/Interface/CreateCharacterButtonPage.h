#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "NewCreateCharEnum.h"

class COuterInterface;
class CBasicTextBox;
class CCreateCharacterMenu;
class CCreateCharacterSchool;
class CCreateCharacterSex;
class CCreateCharacterClass;

class CCreateChatacterButtonPage : public CUIGroup
{
private:
	enum
	{
		CREATE_CHARACTER_CANCEL = NO_ID + 1,
		CREATE_CHARACTER_NEW,
		CREATE_CHARACTER_CHANGE_SERVER,
		CREATE_CHARACTER_OK,
	};

public:
	CCreateChatacterButtonPage(COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CCreateChatacterButtonPage();

	void CreateSubControl ();

	CUIControl*	CreateControl( CString strControl, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
	CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTextButton* m_pNewChar;
	CBasicTextBox* m_pServerText;

protected:
	COuterInterface* m_pInterface;

};