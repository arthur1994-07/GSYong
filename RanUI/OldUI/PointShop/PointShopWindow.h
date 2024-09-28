#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogicClient/PointShop/PointShopClient.h"
#include "../../../EngineLib/G-Logic/TypeDefine.h"

class GLGaeaClient;
class CInnerInterface;

class CBasicLineBoxSmart;
class CBasicTextBox;
class CPointShopNode;
class CPointShopCategory;
class CBasicButton;
class CUIEditBoxMan;
class CBasicTextButton;
class CBasicComboBoxRollOver;
class CItemSlot;
class CBasicVarTextBox;
class CBasicScrollBarEx;
class RnButton;
class CPointShopTooltip;

const int NUM_POINT_SHOP_TAB = 2;
const int NUM_POINT_SHOP_CATEGORY = 14;
const int NUM_POINT_SHOP_ITEM_LIST_ONE_PAGE = 20;
const int NUM_POINT_SHOP_ORDER_BUTTON = 2;
const int MAX_ROLLOVER_TEXT = 10;
const int NUM_CART_LIST_ITEM = 10;
const float	fANSWER_LINE_INTERVAL = 5.0f;
const float fNOTIFY_FULL_CART_TIME = 4.0f;
const float fNOTIFY_FULL_CART_FADE_TIME = 0.2f;

class CPointShopWindow : public CUIWindowEx
{
    typedef std::list<CString>	SRCLIST;
    typedef SRCLIST::iterator	SRCLIST_ITER;

    typedef std::list<pointshop::SellItem> CART_LIST;
    typedef CART_LIST::iterator CART_ITER;

private:
    enum
    {
        POINT_SHOP_TITLE = ET_CONTROL_NEXT + 1,

        POINT_SHOP_ACTIVE_TAB_0 = ET_CONTROL_NEXT + 2,
        POINT_SHOP_ACTIVE_TAB_1 = ET_CONTROL_NEXT + 3,

        POINT_SHOP_INACTIVE_TAB_0 = ET_CONTROL_NEXT + 4,
        POINT_SHOP_INACTIVE_TAB_1 = ET_CONTROL_NEXT + 5,

        POINT_SHOP_CATEGORY_ALL = ET_CONTROL_NEXT + 6,
        POINT_SHOP_CATEGORY_ETC = ET_CONTROL_NEXT + 7,
        POINT_SHOP_CATEGORY_0 = ET_CONTROL_NEXT + 8,
        POINT_SHOP_CATEGORY_END = POINT_SHOP_CATEGORY_0 + NUM_POINT_SHOP_CATEGORY - 1,

        POINT_SHOP_ITEL_LIST_NODE_0 = POINT_SHOP_CATEGORY_END + 1,
        POINT_SHOP_ITEL_LIST_NODE_END = POINT_SHOP_ITEL_LIST_NODE_0 + NUM_POINT_SHOP_ITEM_LIST_ONE_PAGE - 1,

        POINT_SHOP_ORDER_BUTTON = POINT_SHOP_ITEL_LIST_NODE_END + 1,

        POINT_SHOP_PRE_BUTTON = POINT_SHOP_ORDER_BUTTON + 1,
        POINT_SHOP_NEXT_BUTTON = POINT_SHOP_ORDER_BUTTON + 2,
        POINT_SHOP_ITEM_SEARCH_MAN = POINT_SHOP_ORDER_BUTTON + 3,
        POINT_SHOP_ITEM_SEARCH_EDIT = POINT_SHOP_ORDER_BUTTON + 4,
        POINT_SHOP_ITEM_SEARCH_BUTTON = POINT_SHOP_ORDER_BUTTON + 5,
        POINT_SHOP_INCLUDE_TEXT_CHECK = POINT_SHOP_ORDER_BUTTON + 6,
        POINT_SHOP_SEARCH_COMBO_ROLLOVER = POINT_SHOP_ORDER_BUTTON + 7,
        POINT_SHOP_CART_ITEM_SLOT = POINT_SHOP_ORDER_BUTTON + 8,
        POINT_SHOP_BUY_BUTTON = POINT_SHOP_ORDER_BUTTON + 9,

        POINT_SHOP_HELP_BUTTON = POINT_SHOP_ORDER_BUTTON + 10,

        POINT_SHOP_VIEW_SEARCH_WORD_BUTTON = POINT_SHOP_ORDER_BUTTON + 11,
        POINT_SHOP_SEARCH_LIST_COMBO_ROLLOVER = POINT_SHOP_ORDER_BUTTON + 12,

        POINT_SHOP_NOTIFY_FULL_CART_BACK = POINT_SHOP_ORDER_BUTTON + 13,
        POINT_SHOP_NOTIFY_FULL_CART_TEXT = POINT_SHOP_ORDER_BUTTON + 14,

        POINT_SHOP_CATEGORY_RELOAD_BUTTON = POINT_SHOP_ORDER_BUTTON + 15,

        POINT_SHOP_EMPTY_CART_ICON = POINT_SHOP_ORDER_BUTTON + 16,
    };

    enum
    {
        ORDER_BUTTON_NONE_STATE = 0,
        ORDER_BUTTON_DESC_STATE = 1,
        ORDER_BUTTON_ASC_STATE = 2,
    };

private:
    CBasicLineBoxSmart* m_pMainBack;

    CBasicLineBoxSmart* m_pActiveTab[ NUM_POINT_SHOP_TAB ];
    CBasicLineBoxSmart* m_pInActiveTab[ NUM_POINT_SHOP_TAB ];
    CBasicTextBox* m_pTabText[ NUM_POINT_SHOP_TAB ];
    CBasicLineBoxSmart* m_pMain;

    CBasicLineBoxSmart* m_pCategoryBack;
    CPointShopCategory* m_pCategoryAll;
    CPointShopCategory* m_pCategory[ NUM_POINT_SHOP_CATEGORY ];
    CPointShopCategory* m_pCategoryETC;
    CPointShopCategory* m_pCategoryStart;

    CPointShopNode* m_pItemListLeftTemp;
    CPointShopNode* m_pItemListRightTemp;
    CPointShopNode* m_pItemList[ NUM_POINT_SHOP_ITEM_LIST_ONE_PAGE ];

    CBasicLineBoxSmart* m_pSelectItem;

    CUIControl* m_pASC_Image[ NUM_POINT_SHOP_ORDER_BUTTON ];
    CUIControl* m_pDESC_Image[ NUM_POINT_SHOP_ORDER_BUTTON ];
    CUIControl* m_pNone_Image[ NUM_POINT_SHOP_ORDER_BUTTON ];
    CBasicTextBox* m_pOrderText[ NUM_POINT_SHOP_ORDER_BUTTON ];

    CBasicButton* m_pPrePageButton;
    CUIControl* m_pPrePageButtonOver;
    CBasicButton* m_pNextPageButton;
    CUIControl* m_pNextPageButtonOver;
    CBasicTextBox* m_pCurPageText;

    CBasicLineBoxSmart* m_pItemSearchBack;
    CUIEditBoxMan* m_pItemNameEdit;
    CBasicTextButton* m_pItemSearchButton;

    CBasicTextBox* m_pPointText;
    CBasicTextBox* m_pCurPointText;

    CUIControl* m_pCheckResearch;
    CUIControl* m_pUnCheckResearch;
    CBasicTextBox* m_pResearchText;
    CBasicComboBoxRollOver*	m_pComboRollSearchOver;

    CBasicLineBoxSmart* m_pCartListBack;
    CUIControl* m_pCartIconButton;
    CItemSlot* m_pCartItemSlot;
    CBasicLineBoxSmart* m_pEmptyCartHelpBack;
    CBasicTextBox* m_pEmptyCartHelpText;

    RnButton* m_pBuyButton;
    CBasicTextBox* m_pBuyButtonPointText;
    CBasicTextBox* m_pBuyButtonBuyText;
    CPointShopTooltip* m_pBuyButtonTooltip;

    CUIControl* m_pViewSearchWordButton;
    CUIControl* m_pUnViewSearchWordButton;
    CBasicComboBoxRollOver* m_pSearchWordRollOver;

    CBasicButton* m_pHelpButton;
    CUIControl* m_pHelpButtonOver;
    CPointShopTooltip* m_pHelpButtonTooltip;

    CBasicLineBoxSmart* m_pNotifyFullCartBack;
    CBasicTextBox* m_pNotifyFullCartText;

    CBasicButton* m_pCategoryReloadButton;
    CUIControl* m_pCategoryReloadButtonOver;
    CBasicVarTextBox* m_pNotifyCategoryReloadButton;

private:
    int m_nCurPage;
    std::vector<pointshop::SellItemClient> m_SellItemList;
    int m_nOrderButtonState;
    bool m_bCheckIncludeText;
    std::vector<CString> m_vecString;
    SRCLIST m_listSrcString;
    int m_nSelectString;
    int m_nComboPageNum;
    int m_nBeforeStrLength;
    std::string m_strBeforeEdit;
    __time64_t m_UpdateDate;
    CART_LIST m_listCart;
    bool m_bHelpButton;
    bool m_bViewSearchWord;
    std::list<std::string> m_listSearchWord;
    float m_fNotifyFullCartTime;
    float m_fNotifyFullCartFadeTime;
    pointshop::UI_OPTION m_sETC_UI_Option;
    __int64 m_nTotalPrice;
    bool m_bInit;
    bool m_bForceClose;

public:
    CPointShopWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CPointShopWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual	void SetVisibleSingle( BOOL bVisible );
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
    void CreateSubControl();

    void RefreshWindow();
    void ReLoadAll();
    void ReLoadSellItemList();
    void ReLoadSellItemNodes();

    void PreItemListPage();
    void NextItemListPage();

    void SetPointText( __int64 nPoint );

    void SetAllCategory( bool bCheck );

    void CheckIncludeText();
    void UnCheckIncludeText();

    __int64 GetPriceInCart();

    void ResetCartItem( SNATIVEID sItemID );
    
    void SearchItemByName( std::string& strItemName );
	std::string GetSearchEditValue();

    void SetForceCloseWindow( bool _bForceClose );
    bool IsForceCloseWindow();

private:
    void ReLoadCategory();

    void GetCategoryCheckType( std::vector<EMITEM_TYPE>& TypeList );
    void GetOrderState( std::vector<pointshop::SEARCH_ORDER>& OrderList );

    void PrintPage();

    void ComboInit();
    void UpdateComoboItem();
    void LoadSearchList();
    void ResetAll();

    void SearchItemList();

    void AddCartList( SNATIVEID ItemID, __int64 Price );
    void ReLoadCartList();
    void ResetCartList();

    void CheckAutoCategory();
    void ResetAutoCategory();

    void BuyItemList();

    void AddSearchWordList( std::string& strSearchWord );
    void SetSearchWorldListText();

    void ViewSearchWordList();
    void UnViewSearchWordList();
    void ResetViewSearchWordList();

    void ViewNotifyFullCart();
    void UnViewNotifyFullCart();

    void ReloadCategoryTypeForUser();

    void InitCartList();
};

class MyPointShopWindow : public IPointShopWindow, private CPointShopWindow
{
public:
    MyPointShopWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyPointShopWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void RefreshWindow() ;
    virtual void ReLoadItemList();
    virtual void ReLoadAll();
    virtual void SetPointText( __int64 nPoint );
    virtual void SetAllCategory( bool bCheck );
    virtual void UnCheckIncludeTextSearch();
    virtual __int64 GetPriceInCart();
    virtual void ResetCartItem( SNATIVEID sItemID );
    virtual void SearchItem( std::string& strSearchName );
	virtual std::string GetSearchName();
    virtual void SetForceCloseWindow( bool _bForceClose );
    virtual bool IsForceCloseWindow();
};
