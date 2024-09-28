#pragma	once

#include "../Util/UIWindowEx.h"

class	CInnerInterface;
class	CItemBankPage;
class	CBasicTextBox;
class	CBasicTextButton;
class	GLGaeaClient;

class CItemBankWindow : public CUIWindowEx	
{
protected:
	enum
	{
		ITEMBANK_REFRESH_BUTTON = ET_CONTROL_NEXT,
		ITEMBANK_PAGE,
	};

	CItemBankPage*	m_pPage;

public:
	CItemBankWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CItemBankWindow ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	InitItemBank ();
	void	ClearItemBank();

	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );

};