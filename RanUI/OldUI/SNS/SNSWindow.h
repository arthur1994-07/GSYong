
// bjju.sns

#pragma	once

#include "../Util/UIWindowObjectController.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicTextButton;
class CBasicLineBox;
class CBasicTextBox;
class CUIWindowObject;

class CSNSMenuPage;
class CSNSFacebookPage;
class CSNSTwitterPage;

class CSNSWindow : public CUIWindowObjectController
{

protected:
	enum
	{
		SNS_WINDOW_NONE   = NO_ID,
		SNS_WINDOW_OBJECT,

		SNS_WINDOW_PAGE_MENU,
		SNS_WINDOW_PAGE_FACEBOOK,
		SNS_WINDOW_PAGE_TWITTER,
	};

public:
	enum
	{
		SNS_PAGE_REQ_NONE = 0,
		SNS_PAGE_REQ_MENU_PAGE_OPEN,
		SNS_PAGE_REQ_FACEBOOK_PAGE_OPEN,
		SNS_PAGE_REQ_TWITTER_PAGE_OPEN,
	};

public:
	CSNSWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	~CSNSWindow();

public:
	virtual void		Update				( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void		UpdatePageRequest	( UIGUID ControlID, const SPAGEREQUEST& sRequest );
	virtual	void		TranslateUIMessage	( UIGUID ControlID, DWORD dwMsg );

public:
	void				CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual	void		SetVisibleSingle( BOOL bVisible );

public:
	CSNSMenuPage*		GetMenuPage     () { return m_pPage_Menu;	  }
	CSNSFacebookPage*	GetFacebookPage () { return m_pPage_Facebook; }
	CSNSTwitterPage*	GetTwitterPage  () { return m_pPage_Twitter;  }

public:
	void				 DoModalError ( const std::string& strError );
	void				 DoModalMsg   ( const std::string& strMsg   );

protected:
	CBasicTextBox*		CreateStaticControl     (const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CBasicLineBox*		CreateBackgroundControl ( char* szConatrolKeyword );

private:
	CUIWindowObject*	m_pUIObject;

private:
	CSNSMenuPage*		m_pPage_Menu;
	CSNSFacebookPage*	m_pPage_Facebook;
	CSNSTwitterPage*	m_pPage_Twitter;

protected:
    GLGaeaClient*       m_pGaeaClient;

};
