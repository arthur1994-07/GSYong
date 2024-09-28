
// bjju.sns

#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"
#include "../Util/UIPageRequest.h"

#include <map>
#include <vector>

class CInnerInterface;
class CBasicTextButton;
class CBasicLineBox;
class CBasicTextBox;
class CUIWindowObject;
class CUIPage;

class CUIWindowObjectController : public CUIGroup
{

public:
	enum
	{
		WINDOW_PAGE_REQ_NONE = 0,
	};

public:
	enum
	{
		SLOT_NULL = -1,
	};

public:
	typedef std::map<UIGUID, CUIWindowObject*>	MAPWINDOW;
	typedef MAPWINDOW::iterator					MAPWINDOWITER;

	typedef std::vector<CUIWindowObject*>		VECWINDOWSLOT;
	typedef VECWINDOWSLOT::iterator				VECWINDOWSLOTITER;

	typedef std::vector<CUIPage*>				VECPAGE;
	typedef VECPAGE::iterator					VECPAGEITER;

public:
	CUIWindowObjectController(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CUIWindowObjectController ();

public:
	virtual HRESULT		 InitDeviceObjects	( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void		 Update				( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void		 UpdatePageRequest	( UIGUID ControlID, const SPAGEREQUEST& sRequest );
	virtual	void		 TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void		 SetVisibleSingle	( BOOL bVisible );

public :
	void				 Release ();

public:
	void				 Close ();

public:
	void				 WindowResize		 ();

public:
	const BOOL 			 WindowRegister		 ( CUIWindowObject* pWindow, UIGUID WndControlID );
	const BOOL 			 PageRegister		 ( CUIPage*	pPage );

public:
	void				 WindowBegin		 ( UIGUID WndControlID );
	void				 WindowEnd			 ();

public:
	const BOOL			 IsWindowOpenAble    ( const BOOL bPrint=FALSE );
	const BOOL			 IsWindowCloseAble	 ( const BOOL bPrint=FALSE );

public:
	const BOOL			 IsPageOpen			 ( CUIPage* pPage );

public:
	void				 WindowPageOpen		 ( UIGUID WndControlID, CUIPage* pPage );
	void				 WindowPageOpen		 ( CUIWindowObject* pWindow, CUIPage* pPage );
	void				 WindowPageOpenSlot  ( CUIWindowObject* pWindow, CUIPage* pPage, const DWORD dwSlot );

public:
	void				 WindowPageClose	 ( UIGUID WndControlID			 );
	void				 WindowPageClose	 ( CUIWindowObject*		pWindow  );
	void				 WindowPageClose	 ( CUIPage*				pPage	 );

public:
	void				 SetOpenWindow		 ( UIGUID WndControlID )	{ m_OpenWindowID = WndControlID; }
	void				 SetOpenPage		 ( CUIPage* pPage )			{ m_pOpenPage = pPage;			 }

public:
	const DWORD			 GetEnableSlotCount  ();
	const DWORD			 GetEmptySlot		 ();

public:
    virtual void         SetFocusControl   ();
    virtual void         ResetFocusControl ();

private:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

private:
	BOOL					m_bOpen;
	UIGUID					m_OpenWindowID;
	CUIPage*				m_pOpenPage;
	MAPWINDOW				m_MapWindow;
	VECWINDOWSLOT			m_vecSlot;
	VECPAGE					m_vecPage;

protected:
    CInnerInterface*        m_pInterface;

};
