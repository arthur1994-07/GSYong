#pragma once

#include "../Util/UIWindowEx.h"
#include "../Util/NpcActionable.h"

class GLGaeaClient;
class CInnerInterface;

class CBasicLineBoxSmart;
class CBasicTextBox;

class CNPCDialoguePage;
class CNPCMarketPage;
class CNPCPromotionPage;
class CNPCRebuyPage;

const int MAX_DIALOGUE_WIN_TAB = 5;

class CDialogueWindowRenewal : public CUIWindowEx
{
private:
    enum
    {
        DIALOGUE_TITLE = ET_CONTROL_NEXT + 1,

        DIALOGUE_TAB_0 =  DIALOGUE_TITLE + 1,
        DIALOGUE_TAB_1 =  DIALOGUE_TITLE + 2,
        DIALOGUE_TAB_2 =  DIALOGUE_TITLE + 3,
        DIALOGUE_TAB_3 =  DIALOGUE_TITLE + 4,
        DIALOGUE_TAB_4 =  DIALOGUE_TITLE + 5,

        DIALOGUE_IN_TAB_0 =  DIALOGUE_TITLE + 6,
        DIALOGUE_IN_TAB_1 =  DIALOGUE_TITLE + 7,
        DIALOGUE_IN_TAB_2 =  DIALOGUE_TITLE + 8,
        DIALOGUE_IN_TAB_3 =  DIALOGUE_TITLE + 9,
        DIALOGUE_IN_TAB_4 =  DIALOGUE_TITLE + 10,

        DIALOGUE_PAGE = DIALOGUE_TITLE + 11,
        MARKET_PAGE = DIALOGUE_TITLE + 12,
        PROMOTION_PAGE = DIALOGUE_TITLE + 13,
        REBUY_PAGE = DIALOGUE_TITLE + 14,
    };

    enum
    {
        TAB_DIALOGUE_PAGE = 0,
        TAB_MARKET_0_PAGE = 1,
        TAB_MARKET_1_PAGE = 2,
        TAB_MARKET_2_PAGE = 3,
        TAB_REBUY_PAGE = 4,
    };

public:
    CDialogueWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CDialogueWindowRenewal();

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL bVisible );

protected:
    GLGaeaClient* m_pGaeaClient;

private:
    CBasicLineBoxSmart* m_pWindowBack;
    CBasicLineBoxSmart* m_pBack;

    CBasicLineBoxSmart* m_pActiveTab[ MAX_DIALOGUE_WIN_TAB ];
    CBasicLineBoxSmart* m_pInActiveTab[ MAX_DIALOGUE_WIN_TAB ];
    CBasicTextBox* m_pTabText[ MAX_DIALOGUE_WIN_TAB ];

    CNPCDialoguePage* m_pDialoguePage;
    CNPCMarketPage* m_pMarketPage;
    CNPCPromotionPage* m_pPromotionPage;
    CNPCRebuyPage* m_pRebuyPage;

    bool m_bEnableMarket;
    int m_nCurTabPage;
    bool m_bEnableTab[ MAX_DIALOGUE_WIN_TAB ];
    int m_nRebuyTabIndex;

public:
    void CreateSubControl();

    bool SetDialogueData( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID, GLCHARLOGIC* pCharLogic );

    void SelectTab( int nIndex );

    void CheckMarketTab( const DWORD dwNPCGlobalID );
    void UnShowAllMarketTab();

    SNATIVEID GetNPCID();
    DWORD GetNPCGlobalID();

    int GetPageIndex();

	bool IsCurPageMarketType(GLInventorySale::CROW_SALE_TYPE_VERSION type);

    void GetItemIndex( int* pnPosX, int* pnPosY );

    const SINVENITEM* GetItem( int nPosX, int nPosY ) const;

    void AddCartList( int nPageIndex, int nPosX, int nPosY, WORD wBuyNum = 1 );
    void ResetCartListInDialogueWindowMarketPage( int nPageIndex, int nPosX, int nPosY );
	void UpdateExChangeTab();

    void LoadRebuyList();

    void SetRebuyTabCountText( int nCount );
    int GetRebuyItemCount();

    bool IsOpenMarketPage();

    void SetEnableMarketPage(bool bEnable);

    void SetCheckRebuySupplies(bool bCheck );

	void Refresh();

private:
    void AllUnShowPage();

    void SetVisibleTab( int nIndex );
    void SetVisibleMarketTab();
};