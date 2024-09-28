
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
extern const DWORD UIMSG_POSTBOX_PAGE_CLOSE;
extern const DWORD UIMSG_POSTBOX_PAGE_REQ;
////////////////////////////////////////////////////////////////////

class CBasicLineBox;
class CBasicTextBox; 
class CPostBoxWindowObject;
class CPostBoxPageFrame;
class CItemImageEx;
class CSwapImage;
class GLGaeaClient;
class CInnerInterface;

class CPostBoxPage : public CUIGroup
{
	friend CPostBoxPageFrame;

protected:
	enum
	{
		POSTBOX_PAGE_NONE = NO_ID + 1,

		POSTBOX_PAGE_NEXT,
	};

public:
	static const D3DCOLOR cCOLOR_BUTTON_ENABLE	= D3DCOLOR_ARGB(255, 255, 255, 255);
	static const D3DCOLOR cCOLOR_BUTTON_DISABLE	= D3DCOLOR_ARGB(150, 150, 150, 150);

public:
	CPostBoxPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPostBoxPage();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
	virtual HRESULT			InitDeviceObjects   ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT			DeleteDeviceObjects ();

public:
	virtual void			Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void			TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	virtual void			CreateSubControl () = 0;
	virtual void			BeginSubControl  () {}
	virtual void			OpenSubControl   () {}
	virtual void			CloseSubControl  () {}
	virtual void			EndSubControl	 () {}

public:
	virtual CBasicTextBox*  CreateStaticControl     (const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	virtual CBasicLineBox*  CreateBackgroundControl ( char* szConatrolKeyword );
	virtual CBasicTextBox*	CreateNumberBox			( char* szConatrolKeyword );
	virtual CItemImageEx*	CreateItemImage			( char* szConatrolKeyword, UIGUID ControlID = NO_ID );
	virtual CSwapImage*		CreateSwapImage			( char* szConatrolKeyword, UIGUID ControlID = NO_ID );

public:
	// Note : 요청사항 전달용
	void					RequestPush ( const DWORD dwRequest );

public:
	void					Create	( const char* szControlKeyword, const char* pTitleName, UIGUID WndID );
	void					Begin	();
	void					End		();
	void					Open	( CPostBoxWindowObject* pParentWindow );
	void					Close   ();

public:
	CString&				GetTitleName	()	{ return m_strTitleName;  }
	CPostBoxWindowObject*	GetParentWindow	()	{ return m_pParentWindow; }

protected:
	CPostBoxWindowObject*	m_pParentWindow;
	CString					m_strTitleName;

protected:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

};
