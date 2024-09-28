
// bjju.sns

#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"

#include "../Util/UIWindowEx.h"
#include "../Util/UIPageRequest.h"

#include <queue>

class CUIGroup;
class CBasicLineBox;
class CBasicTextBox;
class CUIWindowObjectController;
class CUIPage;
class CUIPageFrame;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_WINDOW_OBJECT_CLOSE = UIMSG_USER1;
const DWORD UIMSG_WINDOW_OBJECT_NEXT  = UIMSG_USER2;
////////////////////////////////////////////////////////////////////

class CUIWindowObject : public CUIWindowEx
{
protected:
	enum
	{
		WINDOW_OBJECT_NONE = ET_CONTROL_NEXT,
		WINDOW_OBJECT_FRAME,

		WINDOW_OBJECT_NEXT,
	};

public:
	enum
	{
		FLAG_PAGE_CHANGE_RESIZE_WINDOW_ANIMATION = 0x0001,
        FLAG_PAGE_CHANGE_ANIMATION				 = 0x0002,
        FLAG_TITLE_NAME_NONCHANGE	             = 0x0004,
	};

	enum
	{
		WINDOW_ANIMATION_NONE	= 0,
		WINDOW_ANIMATION_LINEAR	= 1,
	};

public:
	CUIWindowObject (CInnerInterface* pInteface, EngineDeviceMan* pEngineDevice);
	virtual	~CUIWindowObject ();

public:
	void				CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	virtual void		CreateSubControlEx () {}
	virtual void		CreatePageFrame    ( const char* szPageFrameKeyword, const bool bDefaultPos=false );

public:
	virtual void		Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		TranslateUIMessage	( UIGUID ControlID, DWORD dwMsg );
	virtual void		SetVisibleSingle	( BOOL bVisible );
	virtual void		SetGlobalPos		( const D3DXVECTOR2& vPos );

public:
	virtual BOOL		IsParentExclusiveControl();
	virtual void		ResetParentExclusiveControl();
	virtual void		SetParentExclusiveControl();

public:
	virtual void		WindowOpen  ();
	virtual void		WindowClose ();

public:
	virtual void		PageOpen	 ( CUIPage* pPage );
	virtual void		PageClose	 ();

public:
	const BOOL			IsWindowOpen ()						 { return IsVisible(); }
	const BOOL			IsPageOpen   ( CUIPage* pPage );

public:
	CUIControl*			GetTopControl () { if ( GetParent() ) { return GetParent(); } return this; }
	CUIPage*			GetPage ();

public:
	void				RequestPush ( const DWORD dwID, const DWORD dwArg1=0 );
	const SPAGEREQUEST	RequestPop  ();

public:
	void				SetResize ( const char* szControlKeyword );
	void				SetResize ( CUIPage* pPage  );
	void				SetResize ( UIRECT&  rcSize );

public:
	const DWORD			GetSlot ()						{ return m_dwSlot;		}
	void				SetSlot ( const DWORD dwSlot )	{ m_dwSlot = dwSlot;	}

public:
	void				SetFlag   ( const DWORD dwFlag )  { m_dwFlag |= dwFlag;  }
	void				ResetFlag ( const DWORD dwFlag )  { m_dwFlag &= ~dwFlag; }

public:
	void				SetParentController ( CUIWindowObjectController* pParent )	{ m_pParentController = pParent;	}
	void				SetWindowAniType  ( const DWORD dwType )					{ m_dwAniType = dwType;				}
	void				SetWindowAniSpeed ( const FLOAT fSpeedPerSec )				{ m_fAniSpeedPerSec = fSpeedPerSec; }
	void				SetPageAniType  ( const DWORD dwType );
	void				SetPageAniSpeed ( const FLOAT fSpeedPerSec );

protected:
	void				SetParentResize  ( char* szControlKeyword );
	void				SetFramePos		 ( CUIPageFrame* pFrame );

protected:
	void				BeginOpenAnimation ( CUIPage* pDest );
	void				UpdateAnimation	   ( float fElapsedTime );

protected:
	CUIPageFrame*				m_pFrame;
	CUIWindowObjectController*	m_pParentController;

protected:
	LPDIRECT3DDEVICEQ	        m_pd3dDevice;
	std::queue<SPAGEREQUEST>    m_Request;

protected:
	DWORD				        m_dwSlot;
	DWORD				        m_dwFlag;

protected:
	DWORD				        m_dwAniType;
	FLOAT				        m_fAniTime;
	FLOAT				        m_fAniSpeedPerSec;
	CUIPage*			        m_pAniDest;
	UIRECT				        m_rcAniRectSrc;
	UIRECT				        m_rcAniRectDest;
	BOOL				        m_bAniPlay;

};
