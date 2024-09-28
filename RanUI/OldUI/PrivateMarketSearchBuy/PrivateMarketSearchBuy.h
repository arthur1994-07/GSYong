#pragma	once

#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"
#include "../Util/TapSelectorEx.h"

class CInnerInterface;
class CBasicTextBox;
class CBasicTextButton;
class GLGaeaClient;
class MarketSearchPage;
class MarketSearchWhishPage;
class MarketHistoryPage;
class MarketRPPage;
class CBasicLineBoxSmart;

struct SINVENITEM;

class PrivateMarketSearchBuy : public CUIWindowEx, private CTapSelectorEx<DEFAULT_TAPBUTTON>
{
private:
	typedef CTapSelectorEx::BUTTON STAPBUTTON;
	enum
	{
		ID_SEARCH_TAB = ET_CONTROL_NEXT + 1,
		ID_WISH_LIST_TAB,
		ID_SHOPPING_HISTORY_TAB,
        ID_RAN_POINT_TAB,

		ID_SEARCH_PAGE,
		ID_WHISH_LIST_PAGE,
		ID_HISTORY_PAGE,
        ID_RAN_POINT_PAGE,

		ID_HELP_BUTTON,
	};

public:
	PrivateMarketSearchBuy(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~PrivateMarketSearchBuy();

protected:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface*    m_pInterface;
	CUIGroup*           m_pCurrentPage;

public:
	MarketSearchPage*       m_pSerchPage;
	MarketSearchWhishPage*  m_pWhishPage;
	MarketHistoryPage*      m_pHistoryPage;
    MarketRPPage*           m_pRanPointPage;
	CBasicLineBoxSmart*     m_pWhishPageEvent;

	float m_fFlickeringTime;
	bool bIsWhishPageEvent;

public:
	void CreateSubControl ();

    void RanPointRefresh();

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
	virtual void PrivatemarketRefreshSearchResult();
	virtual void CloseAllRollOver();
    virtual void SetVisibleSingle( BOOL bVisible );
	virtual void SetPageWaiting( bool bIsWaiting );
	virtual bool IsPageWaiting();
	void WhishPageEvent(float fElapsedTime );
	virtual void SetWhishPageEvent(bool bBOOL);

private:
	CUIGroup* GetTapWindow(UIGUID controlID);
	void EventSelectedTap( UIGUID controlID );


};

class MyPrivateMarketSearchBuy: public IPrivateMarketSearchBuy, private PrivateMarketSearchBuy
{
public:
	MyPrivateMarketSearchBuy( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
		:PrivateMarketSearchBuy( pGaeaClient, pInterface, pEngineDevice )
	{

	}
	virtual ~MyPrivateMarketSearchBuy()   {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();
	virtual void PrivatemarketRefreshSearchResult();
	virtual void CloseAllRollOver();
	virtual void SetPageWaiting( bool bIsWaiting );
	virtual bool IsPageWaiting();
	virtual void SetWhishPageEvent(bool bBOOL);
    virtual void Refresh();
};