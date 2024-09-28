
// bjju.sns Ãß°¡

#pragma	once

#include "../Util/WebWindowBase.h"

class GLGaeaClient;
class CBasicTextBox;
class CBasicTextButton;
class CBasicButton;
class CBasicLineBoxSmart;
class CUIEditBox;

class CTwitterWindow : public CWebWindowBase
{
protected:
	enum
	{
		TWITTER_WINDOW_NONE = ET_CONTROL_NEXT,

		TWITTER_WINDOW_EDITBOX_PIN,
		TWITTER_WINDOW_BUTTON_PIN,
	};

public:
	CTwitterWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CTwitterWindow ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	void		 CreateSubControl ();

private:
	CBasicTextButton*	m_pButton_Pin;
	CBasicLineBoxSmart* m_pLineBox_Pin;
	CUIEditBox*			m_pEditBox_Pin;

private:
    GLGaeaClient* m_pGaeaClient;
};