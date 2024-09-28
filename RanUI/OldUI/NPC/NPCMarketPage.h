#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../Util/NpcActionable.h"

class GLGaeaClient;
class CInnerInterface;
class CMarketPage;
class CBasicLineBoxSmart;
class CItemImage;
class CBasicTextBox;
class CBasicTextBoxEx;
class CItemSlot;
class RnButton;

const int MAX_DIALOGUE_MARKET_TAB = 3;
const int NUM_RECOMMEND_ITEM = 5;

class CNPCMarketPage : public CUIGroup, private CUIControlNpcActionable
{
private:
    struct SMARKET_CART_DATA
    {
        int nPageIndex;

        int nPosX;
        int nPosY;

        WORD wBuyNum;

        SMARKET_CART_DATA( int _nPageIndex, int _nPosX, int _nPosY, WORD _wBuyNum )
            : nPageIndex(_nPageIndex), nPosX(_nPosX), nPosY(_nPosY), wBuyNum(_wBuyNum)
        {}
    };

    typedef std::list<SMARKET_CART_DATA> DIAL_CART_LIST;
    typedef DIAL_CART_LIST::iterator DIAL_CART_ITER;

private:
    static const int nOUTOFRANGE;
    static const float fANSWER_PART_INTERVAL;
    static const int NUM_CART_LIST_ITEM;

    static const float fNOTIFY_FULL_CART_TIME;
    static const float fNOTIFY_FULL_CART_FADE_TIME;

private:
    enum
    {
        MARKET_PAGE_BACK = NO_ID + 1,
        MARKET_SELL_PAGE_0 = MARKET_PAGE_BACK + 1,
        MARKET_SELL_PAGE_1 = MARKET_PAGE_BACK + 2,
        MARKET_SELL_PAGE_2 = MARKET_PAGE_BACK + 3,

        RECOMMEND_ITEM_0 = MARKET_PAGE_BACK + 4,
        RECOMMEND_ITEM_1 = MARKET_PAGE_BACK + 5,
        RECOMMEND_ITEM_2 = MARKET_PAGE_BACK + 6,
        RECOMMEND_ITEM_3 = MARKET_PAGE_BACK + 7,
        RECOMMEND_ITEM_4 = MARKET_PAGE_BACK + 8,

        RECOMMEND_TEXT_0 = MARKET_PAGE_BACK + 9,
        RECOMMEND_TEXT_1 = MARKET_PAGE_BACK + 10,
        RECOMMEND_TEXT_2 = MARKET_PAGE_BACK + 11,
        RECOMMEND_TEXT_3 = MARKET_PAGE_BACK + 12,
        RECOMMEND_TEXT_4 = MARKET_PAGE_BACK + 13,

        RECOMMEND_CHECK_USABLE = MARKET_PAGE_BACK + 14,
        RECOMMEND_CHECK_BUYABLE = MARKET_PAGE_BACK + 15,

        CART_ICON = MARKET_PAGE_BACK + 16,
        CART_ITEM_SLOT = MARKET_PAGE_BACK + 17,

        MARKET_BUY_BUTTON = MARKET_PAGE_BACK + 18,
        DIALOGUE_CLOSE_BUTTON = MARKET_PAGE_BACK + 19,

        NOTIFY_FULL_CART_BACK = MARKET_PAGE_BACK + 20,
        NOTIFY_FULL_CART_TEXT = MARKET_PAGE_BACK + 21,

        CHECK_SELL_R_BTN = MARKET_PAGE_BACK + 22,
        CHECK_CONFIRM_SELL_ITEM = MARKET_PAGE_BACK + 23,

		RECOMMEND_CHECK_EXCHANGE_ABLE = MARKET_PAGE_BACK + 24,

        MARKET_REPAIR_BUTTON,
    };

    enum
    {
        MARKET_PAGE_0 = 0,
        MARKET_PAGE_1 = 1,
        MARKET_PAGE_2 = 2,
    };

public:
    CNPCMarketPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CNPCMarketPage();

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
    int m_nPosX;
    int m_nPosY;
    int m_nPageIndex;

    DIAL_CART_LIST m_listCart;
	DIAL_CART_LIST m_listExCart;

    __int64 m_nTotalPrice;
    __int64 m_nCurInvenMoney;

    float m_fNotifyFullCartTime;
    float m_fNotifyFullCartFadeTime;

private:
    CBasicLineBoxSmart* m_pSellListBack;
    CMarketPage* m_pSellPage[ MAX_DIALOGUE_MARKET_TAB ];

    CBasicLineBoxSmart* m_pRecommendListBack;
    CBasicTextBox* m_pRecommendTitle;
    CItemSlot* m_pRecommendItem[ NUM_RECOMMEND_ITEM ];
    CBasicTextBoxEx* m_pRecommendItemText[ NUM_RECOMMEND_ITEM ];
    CBasicLineBoxSmart* m_pCheckItemTypeBack;

    CUIControl* m_pCheckUsableItem;
    CUIControl* m_pUncheckUsableItem;
    CBasicTextBox* m_pUsableItemText;

    CUIControl* m_pCheckBuyableItem;
    CUIControl* m_pUncheckBuyableItem;
    CBasicTextBox* m_pBuyableItemText;

	CUIControl* m_pCheckExchangeableItem;
	CUIControl* m_pUncheckExchangeableItem;
	CBasicTextBox* m_pExchangeableItemText;

    CBasicLineBoxSmart* m_pMoneyBack;
    CBasicTextBox* m_pMoneyText;

    CBasicLineBoxSmart* m_pCartListBack;
    CUIControl* m_pCartIconButton;
    CItemSlot* m_pCartItemSlot;
    CBasicLineBoxSmart* m_pEmptyCartHelpBack;
    CBasicTextBox* m_pEmptyCartHelpText;

    CBasicLineBoxSmart* m_pNotifyFullCartBack;
    CBasicTextBox* m_pNotifyFullCartText;

    RnButton* m_pBuyButton;
    CBasicTextBox* m_pBuyButtonPointText;
    CBasicTextBox* m_pBuyButtonBuyText;

    RnButton* m_pCloseButton;

    CUIControl* m_pCheckSellRBtn;
    CUIControl* m_pUncheckSellRBtn;
    CBasicTextBox* m_pSellRBtnText;

    CUIControl* m_pCheckConfirmSell;
    CUIControl* m_pUncheckConfirmSell;
    CBasicTextBox* m_pConfirmSellText;

private:
    CMarketPage* CreateMarketPage( UIGUID ControlID );
    CItemSlot* CreateItemSlot( const char* szControl, UIGUID ControlID );

    bool CheckRecommendItem(const SITEM* pItemData, const SITEMCUSTOM& sItemCustom, bool bTypeCheck = true, int iCol = -1, int iRow = -1);
	bool CheckExRecommendItem(const SITEM* pItemData, const SITEMCUSTOM& sItemCustom, bool bTypeCheck = true, int iCol = -1, int iRow = -1);

    void SetDiffuseAlphaItemList();
	void SetExDiffuseAlphaItemList();
	
    void SetCheckUsableItem( bool bCheck );
    void SetCheckBuyableItem( bool bCheck );
	void SetCheckExchangeableItem( bool bCheck );

    void SetItemIndex( int nPosX, int nPosY );

    void LoadCartList();
    void ResetCartList();

    void ViewNotifyFullCart();
    void UnViewNotifyFullCart();

    void SetCurInvenMoney();

    void SetCheckSellRBtnItem( bool bCheck );
    void SetCheckConfirmSellItem( bool bCheck );

public:
    void CreateSubControl();

    void SetMarketData( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID );

    void SetMarketPage( int nPageIndex );
    void SetRecommendItem( );

	void SetExRecommandItem();

    SNATIVEID GetNPCID();
    DWORD GetNPCGlobalID();

    int GetPageIndex();

    void GetItemIndex( int* pnPosX, int* pnPosY );
    const SINVENITEM* GetItem( int nPosX, int nPosY ) const;

    void AddCartList( int nPageIndex, int nPosX, int nPosY, WORD wBuyNum = 1 );

    __int64 GetTotalPriceInCart();

    void BuyCartListItem();
	void BuyCartListItemEx();

    void ResetCartListInDialogueWindowMarketPage( int nPageIndex, int nPosX, int nPosY );

	BOOL IsCurPageMarketType( GLInventorySale::CROW_SALE_TYPE_VERSION type );
	GLInventorySale::CROW_SALE_TYPE_VERSION GetCurPageMarketType();
	GLInventorySale::CROW_SALE_TYPE_VERSION GetPageMarketType(int iPage);
};