#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CBasicTextBox;
class CSwapImage;
class GLGaeaClient;

class CPostNotifyButton : public CUIGroup
{
public:
	CPostNotifyButton(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CPostNotifyButton(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	enum
	{
		POSTNOTIFY_BUTTON_NONE = NO_ID + 1,

		POSTNOTIFY_BUTTON_IMAGE,
	};

public:
	enum EMNOTIFY_TYPE
	{
		EMNOTIFY_TYPE_INIT			= 0,
		EMNOTIFY_TYPE_NONE			= 1,
		EMNOTIFY_TYPE_UNREAD		= 2,
	};

public:
	static const D3DCOLOR cCOLOR_TEXT_DEFAULT = D3DCOLOR_ARGB(255, 0, 0, 0);
	static const D3DCOLOR cCOLOR_TEXT_OVER	  = D3DCOLOR_ARGB(255, 255, 50, 50);

public:
	void			CreateSubControl();

public:
	virtual	void	TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CSwapImage*		m_pImage;
	CSwapImage*		m_pImageTextBack;
	CBasicTextBox*	m_pText;

private:
	EMNOTIFY_TYPE	m_emType;
	DWORD			m_dwNew;
	DWORD			m_dwTotal;

protected:
	CInnerInterface* m_pInterface;
};

class MyPostNotifyButton : public IPostNotifyButton, private CPostNotifyButton
{
public:
    MyPostNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyPostNotifyButton()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
};

