
// bjju.sns Ãß°¡

#pragma	once

#include "../Util/UIPage.h"

#include "../EngineLib/G-Logic/GLDefine.h"

class   CInnerInterface;
class   GLGaeaClient;
class   CSNSMenuIcon;

class CSNSMenuPage : public CUIPage
{

protected:
	enum
	{
		PAGE_MENU = PAGE_NEXT,

		PAGE_MENU_BUTTON_FACEBOOK_AUTH,
		PAGE_MENU_BUTTON_TWITTER_AUTH,
		PAGE_MENU_BUTTON_FACEBOOK_USE,
		PAGE_MENU_BUTTON_TWITTER_USE,
	};


public:
	static const D3DCOLOR cCOLOR_TEXT_DEFAULT	= D3DCOLOR_ARGB(255, 255, 255, 255);
	static const D3DCOLOR cCOLOR_TEXT_SELECT	= D3DCOLOR_ARGB(255, 253, 167, 23);

public:
	CSNSMenuPage (GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSNSMenuPage ();

protected:
	virtual void		CreateSubControl();

public:
	virtual void		Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	virtual void		OpenSubControl ();

protected:
	void				UpdateControl ();

private:
	CSNSMenuIcon*		m_pIcon_FaceBookAuth;
	CSNSMenuIcon*		m_pIcon_TwitterAuth;
	CSNSMenuIcon*		m_pIcon_FaceBookUse;
	CSNSMenuIcon*		m_pIcon_TwitterUse;

private:
	BOOL				m_bFBAuth;
	BOOL				m_bTTAuth;
	BOOL				m_bEnable;

protected:
    GLGaeaClient*           m_pGaeaClient;
    CInnerInterface*        m_pInterface;

};
