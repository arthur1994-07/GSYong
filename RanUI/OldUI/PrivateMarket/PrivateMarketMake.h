#pragma	once

#include "../../../EngineLib/GUInterface/UIWindow.h"

class CInnerInterface;
class CBasicTextBox;
class CBasicButton;
class CUIEditBox;
class CBasicTextButton;
class GLGaeaClient;

class CPrivateMarketMake : public CUIWindow
{
private:
static	const int nLIMIT_CHAR;
static	const char BLANK_SYMBOL;

protected:
	enum
	{
		PRIVATE_MARKET_MAKE_OK = ET_CONTROL_NEXT,
		PRIVATE_MARKET_MAKE_CANCEL,
		PRIVATE_MARKET_MAKE_EDIT
	};

public:
	CPrivateMarketMake(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPrivateMarketMake ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	Init ();

public:	
	void	CreateSubControl ();

public:
	const CString& GetTITLE ();

private:
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	SetVisibleSingle ( BOOL bVisible );

private:
	CUIEditBox*	m_pEditBox;

protected:
	CInnerInterface* m_pInterface;

};
