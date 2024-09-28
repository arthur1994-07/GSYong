
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class  CPostBoxReceiveItem;
struct SAPOSTCLIENT;
class GLGaeaClient;
class CInnerInterface;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_POSTBOX_RECEIVE_LIST_READ  = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CPostBoxReceiveList : public CUIGroup
{

protected:
	enum
	{
		POSTBOX_PAGE_RECEIVE_LIST_NONE = NO_ID,

		POSTBOX_PAGE_RECEIVE_ITEM0,
		POSTBOX_PAGE_RECEIVE_ITEM1,
		POSTBOX_PAGE_RECEIVE_ITEM2,
		POSTBOX_PAGE_RECEIVE_ITEM3,
		POSTBOX_PAGE_RECEIVE_ITEM4,

		POSTBOX_PAGE_RECEIVE_ITEM_BEGIN_INDEX = POSTBOX_PAGE_RECEIVE_ITEM0,
	};

public:
	enum
	{
		MAX_ITEM	= 5,
		SELECT_NULL = -1,
	};

public:
	CPostBoxReceiveList(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPostBoxReceiveList ();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
	virtual void		 Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		 TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void				 CreateSubControl ();
	void				 OpenSubControl   ();
	void				 CloseSubControl  ();

public:
	void				 UpdateItem ( const BOOL bForcing=FALSE );
	void				 UpdateItem ( SAPOSTCLIENT* pPostData );

public:
	CPostBoxReceiveItem* CreateItem ( const char* szControl, UIGUID ControlID );

public:
	void				 SelectPost ( const DWORD dwItem );

public:
	const BOOL			 ReturnItem  ( const DWORD dwItem  );
	const BOOL			 ReturnIndex ( const DWORD dwIndex );
	const BOOL			 DeleteItem  ( const DWORD dwItem  );
	const BOOL			 DeleteIndex ( const DWORD dwIndex );

public:
	void				 PageNext ();
	void				 PagePrev ();

public:
	SAPOSTCLIENT*		 GetSelectedPost  ();
	const DWORD			 GetSelectedItem  ();
	const DWORD			 GetSelectedIndex ()  { return m_dwSelectIndex;	}
	const DWORD			 GetCurrentPage   ()  { return m_dwPage;		}
	const DWORD			 GetMaxPage		  ()  { return m_dwPageMax;		}

public:
	CPostBoxReceiveItem* GetItem ( const DWORD dwItem ) { return m_pItem[dwItem]; }
	void				 GetCheckedItems ( DWORD* pInOutItemArray, DWORD& dwOutCount );

public:
	const DWORD			 ItemToIndex ( const DWORD dwItem  );
	const DWORD			 IndexToPage ( const DWORD dwIndex );

private:
	CPostBoxReceiveItem* m_pItem [MAX_ITEM];

private:
	DWORD				 m_dwPage;
	DWORD				 m_dwPageMax;
	DWORD				 m_dwPostMax;
	DWORD				 m_dwSelectIndex;
};
