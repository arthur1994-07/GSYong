
// bjju.sns

#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
extern const DWORD UIMSG_PAGE_CLOSE;
extern const DWORD UIMSG_PAGE_REQ;
////////////////////////////////////////////////////////////////////

class CBasicLineBox;
class CBasicTextBox;
class CBasicLineBoxSmart;
class CUIWindowObject;
class CUIPageFrame;
class CSwapImage;

class CUIPage : public CUIGroup
{
	friend CUIPageFrame;

protected:
	enum
	{
		PAGE_NONE = NO_ID + 1,

		PAGE_NEXT,
	};

public:
	static const D3DCOLOR cCOLOR_BUTTON_ENABLE	= D3DCOLOR_ARGB(255, 255, 255, 255);
	static const D3DCOLOR cCOLOR_BUTTON_DISABLE	= D3DCOLOR_ARGB(150, 150, 150, 150);

public:
	CUIPage(EngineDeviceMan* pEngineDevice);
	virtual	~CUIPage () = 0;

public:
	virtual HRESULT			InitDeviceObjects   ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT			DeleteDeviceObjects ();

public:
	virtual void			Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void			TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
    virtual void			CreateSubControl () {}
	virtual void			BeginSubControl  () {}
	virtual void			OpenSubControl   () {}
	virtual void			CloseSubControl  () {}
	virtual void			EndSubControl	 () {}

	

public:

	virtual void			OnPageVisible(bool _isOpen) {}

	virtual CBasicTextBox*		CreateStaticControl     (const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	virtual CBasicLineBox*		CreateBackgroundControl ( char* szConatrolKeyword );
	virtual CBasicLineBoxSmart* CreateLineBox			( char* szConatrolKeyword, char* szTexInfo );
	virtual CBasicTextBox*		CreateNumberBox			( char* szConatrolKeyword );
	virtual CSwapImage*			CreateSwapImage			( char* szConatrolKeyword, UIGUID ControlID = NO_ID );

public:
    virtual void			GetOrgLocalPos  ( UIRECT& OutRect );
    virtual void            AdapterPosition ( const std::string& strAdapeterKeyword, const bool bNegative=false );

public:
	// Note : 요청사항 전달용
	void					RequestPush ( const DWORD dwID, const DWORD dwArg1=0 );

public:
	void					Create	( const char* szControlKeyword, const char* pTitleName, UIGUID WndID );
	void					Begin	();
	void					End		();
	void					Open	( CUIWindowObject* pParentWindow );
	
	void					Close   ();

public:
	CString&				GetTitleName	()	{ return m_strTitleName;  }
	CUIWindowObject*		GetParentWindow	()	{ return m_pParentWindow; }

protected:
	CUIWindowObject*		m_pParentWindow;
	CString					m_strTitleName;

protected:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

};
