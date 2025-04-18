#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBoxEx;
class CBasicScrollBarEx;
class CBasicTextButton;

class CSentWindow : public CUIGroup
{
	enum
	{
		SENT_LIST_TEXTBOX = NO_ID + 1,
		SENT_LIST_SCROLLBAR,

		SENT_OPEN_BUTTON,
		SENT_DEL_BUTTON,
		SENT_RELOAD_BUTTON
	};

public:
	CSentWindow(EngineDeviceMan* pEngineDevice);
	virtual ~CSentWindow();

public:
	void CreateSubControl();

public:
	virtual	void TranslateUIMessage( UIGUID cID, DWORD dwMsg );

protected:
	CUIControl * CreateControl( char* szControl, const UIGUID& cID = NO_ID );
	CBasicTextButton * CreateTextButton14( char* szButton, UIGUID ControlID , char* szText );

private:
	CBasicTextBoxEx*	m_pListText;
	CBasicScrollBarEx*	m_pListScrollBar;

	CBasicTextButton * m_pOpenButton;
	CBasicTextButton * m_pDelButton;
	CBasicTextButton * m_pReloadButton;
};