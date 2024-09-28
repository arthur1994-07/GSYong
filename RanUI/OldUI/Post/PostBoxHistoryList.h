
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class  CPostBoxHistoryItem;
struct SAPOSTCLIENT;
class GLGaeaClient;

class CPostBoxHistoryList : public CUIGroup
{

protected:
	enum
	{
		POSTBOX_PAGE_RECEIVE_LIST_NONE = NO_ID,

		POSTBOX_PAGE_HISTORY_ITEM0,
		POSTBOX_PAGE_HISTORY_ITEM1,
		POSTBOX_PAGE_HISTORY_ITEM2,
		POSTBOX_PAGE_HISTORY_ITEM3,
		POSTBOX_PAGE_HISTORY_ITEM4,

		POSTBOX_PAGE_HISTORY_ITEM_BEGIN_INDEX = POSTBOX_PAGE_HISTORY_ITEM0,
	};

public:
	enum
	{
		MAX_ITEM = 5,
	};

public:
    enum CPOSTBOX_HISTORY_TYPE
    {
        POSTBOX_HISTORY_ALL,
        POSTBOX_HISTORY_SEND,
        POSTBOX_HISTORY_RECV,
    };

public:
	CPostBoxHistoryList(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, CPOSTBOX_HISTORY_TYPE historyType);
	virtual	~CPostBoxHistoryList();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	virtual void		 Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		 TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void		 CreateSubControl ();

public:
	virtual void		 UpdateItem ( const BOOL bForcing=FALSE );

protected:
    void                 UpdateItemWithHistoryInfoSet ( GLPostClient::SPOSTHISTORYINFOSET& historyInfoSet, const BOOL bForcing=FALSE );

public:
	CPostBoxHistoryItem* CreateItem ( const char* szControl, UIGUID ControlID );

public:
	void				 SelectPost ( const DWORD dwIndex );

public:
	void				 PageNext ();
	void				 PagePrev ();

public:
    void                 ResetCurrentPage ()     { m_dwPage = 0; }
	const DWORD			 GetCurrentPage   ()	 { return m_dwPage;		}
	const DWORD			 GetMaxPage		  ()	 { return m_dwPageMax;	}

protected:
	CPostBoxHistoryItem* m_pItem [MAX_ITEM];
    CPOSTBOX_HISTORY_TYPE m_historyType;

protected:
	DWORD				 m_dwPage;
	DWORD				 m_dwPageMax;

};
