#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

#include "../Util/UIWindowEx.h"

#include <queue>

class CUIGroup;
class CBasicLineBox;
class CBasicTextBox;
class CPostBoxPage;
class CPostBoxPageFrame;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_POSTBOX_WINDOW_OBJECT_CLOSE = UIMSG_USER1;
const DWORD UIMSG_POSTBOX_WINDOW_OBJECT_NEXT  = UIMSG_USER2;
////////////////////////////////////////////////////////////////////

class CPostBoxWindowObject : public CUIWindowEx
{
protected:
	enum
	{
		POSTBOX_WINDOW_OBJECT_NONE = ET_CONTROL_NEXT,
		POSTBOX_WINDOW_OBJECT_FRAME,

		POSTBOX_WINDOW_OBJECT_NEXT,
	};

public:
	CPostBoxWindowObject ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CPostBoxWindowObject ();


public:
	void				CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	virtual void		CreateSubControlEx () = 0;
	virtual void		CreatePageFrame    ( const char* szPageFrameKeyword );

public:
	virtual void		Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void		SetVisibleSingle ( BOOL bVisible );
	virtual void		SetGlobalPos ( const D3DXVECTOR2& vPos );

public:
	virtual BOOL		IsParentExclusiveControl();
	virtual void		ResetParentExclusiveControl();
	virtual void		SetParentExclusiveControl();

public:
	virtual void		WindowOpen  ();
	virtual void		WindowClose ();

public:
	virtual void		PageOpen	 ( CPostBoxPage* pPage );
	virtual void		PageClose	 ();

public:
	const BOOL			IsWindowOpen ()						 { return IsVisible(); }
	const BOOL			IsPageOpen   ( CPostBoxPage* pPage );

public:
	CUIControl*			GetTopControl () { if ( GetParent() ) { return GetParent(); } return this; }
	CPostBoxPage*		GetPage ();

public:
	void				RequestPush ( const DWORD dwRequest );
	const DWORD			RequestPop  ();

public:
	void				SetResize ( const char* szControlKeyword );

public:
	const DWORD			GetSlot ()						{ return m_dwSlot;		}
	void				SetSlot ( const DWORD dwSlot )	{ m_dwSlot = dwSlot;	}

protected:
	void				SetParentResize ( char* szControlKeyword );

protected:
	CBasicTextBox*		CreateStaticControl     (const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CBasicLineBox*		CreateBackgroundControl ( char* szConatrolKeyword );

protected:
	CPostBoxPageFrame*	m_pFrame;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	std::queue<DWORD>	m_Request;

protected:
	DWORD				m_dwSlot;

protected:
	CInnerInterface*	m_pInterface;
};
