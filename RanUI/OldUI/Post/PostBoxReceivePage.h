
#pragma	once

#include "PostBoxPage.h"

class  CInnerInterface;
class  CPostBoxReceiveList;
class  CBasicButton;
class  CBasicTextButton;
class  CBasicTextBox; 
class  CSwapImage;
struct SAPOSTCLIENT;

class CPostBoxReceivePage : public CPostBoxPage
{

protected:
	enum
	{
		POSTBOX_PAGE_RECEIVE_NONE = POSTBOX_PAGE_NEXT,

		POSTBOX_PAGE_RECEIVE_LIST,
		POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_LEFT,
		POSTBOX_PAGE_RECEIVE_IMAGE_ARROW_RIGHT,
		POSTBOX_PAGE_RECEIVE_BUTTON_DELETE,
		POSTBOX_PAGE_RECEIVE_BUTTON_REFRESH,
	};

public:
	CPostBoxReceivePage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPostBoxReceivePage();

protected:
	static const D3DCOLOR cCOLOR_REFRESH_BUTTON_ENABLE	= D3DCOLOR_ARGB(255, 255, 255, 255);
	static const D3DCOLOR cCOLOR_REFRESH_BUTTON_DISABLE	= D3DCOLOR_ARGB(255, 150, 150, 150);

protected:
	virtual void	CreateSubControl ();

protected:
	virtual void	BeginSubControl ();
	virtual void	OpenSubControl  ();
	virtual void	CloseSubControl ();

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void			UpdateVariableText	  ();
	void			UpdateVariableControl ();

public:
	void			UpdateItem ();
	void			UpdateItem ( SAPOSTCLIENT* pPostData );

public:
	void			SelectPost   ( const DWORD dwItem );
	void			SelectReset  () { SelectPost ( -1 ); }

public:
	const BOOL		ReturnItem  ( const DWORD dwItem  );
	const BOOL		ReturnIndex ( const DWORD dwIndex );
	const BOOL		DeleteItem  ( const DWORD dwItem  );
	const BOOL		DeleteIndex ( const DWORD dwIndex );

public:
	SAPOSTCLIENT*	GetSelectedPost  ();
	const DWORD		GetSelectedIndex ();
	const DWORD		GetSelectedItem  ();

public:
	void			ReqMultiDelete_Check  ();
	void			ReqMultiDelete_Commit ( const BOOL bAttachDelete=TRUE );

private:
	CPostBoxReceiveList* m_pList;

	CBasicTextBox*		 m_pText_New;
	CBasicTextBox*		 m_pText_Page;

	CSwapImage*			 m_pImage_Arrow_Left;
	CSwapImage*			 m_pImage_Arrow_Right;

	CBasicTextButton*	 m_pButton_Delete;
	CBasicButton*		 m_pButton_Refresh;
};
