#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/G-Logic/GLogic.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "../Item/ItemSlot.h"
#include "../Util/RnButton.h"
#include "./PointShopWindow.h"
#include "./PointShopNode.h"
#include "./PointShopCategory.h"
#include "./PointShopTooltip.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPointShopWindow::CPointShopWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pMainBack(NULL)
    , m_pMain(NULL)
    , m_pCategoryBack(NULL)
    , m_pCategoryAll(NULL)
    , m_pCategoryETC(NULL)
    , m_pCategoryStart(NULL)
    , m_pItemListLeftTemp(NULL)
    , m_pItemListRightTemp(NULL)
    , m_pSelectItem(NULL)
    , m_pPrePageButton(NULL)
    , m_pPrePageButtonOver(NULL)
    , m_pNextPageButton(NULL)
    , m_pNextPageButtonOver(NULL)
    , m_pCurPageText(NULL)
    , m_pItemSearchBack(NULL)
    , m_pItemNameEdit(NULL)
    , m_pItemSearchButton(NULL)
    , m_pPointText(NULL)
    , m_pCurPointText(NULL)
    , m_pCheckResearch(NULL)
    , m_pUnCheckResearch(NULL)
    , m_pResearchText(NULL)
    , m_pComboRollSearchOver(NULL)
    , m_pCartListBack(NULL)
    , m_pCartIconButton(NULL)
    , m_pCartItemSlot(NULL)
    , m_pEmptyCartHelpBack(NULL)
    , m_pEmptyCartHelpText(NULL)
    , m_pBuyButton(NULL)
    , m_pBuyButtonPointText(NULL)
    , m_pBuyButtonBuyText(NULL)
    , m_pBuyButtonTooltip(NULL)
    , m_pHelpButton(NULL)
    , m_pHelpButtonOver(NULL)
    , m_pHelpButtonTooltip(NULL)
    , m_pViewSearchWordButton(NULL)
    , m_pUnViewSearchWordButton(NULL)
    , m_pSearchWordRollOver(NULL)
    , m_pNotifyFullCartBack(NULL)
    , m_pNotifyFullCartText(NULL)
    , m_pCategoryReloadButton(NULL)
    , m_pCategoryReloadButtonOver(NULL)
    , m_nCurPage(0)
    , m_nOrderButtonState(ORDER_BUTTON_NONE_STATE)
    , m_bCheckIncludeText(false)
    , m_nSelectString(0)
    , m_nComboPageNum(0)
    , m_nBeforeStrLength(0)
    , m_UpdateDate(0)
    , m_bHelpButton(false)
    , m_bViewSearchWord(false)
    , m_fNotifyFullCartTime(fNOTIFY_FULL_CART_TIME)
    , m_fNotifyFullCartFadeTime(fNOTIFY_FULL_CART_FADE_TIME)
    , m_nTotalPrice(0)
    , m_bInit(false)
    , m_bForceClose(false)
{
    for ( int i = 0; i < NUM_POINT_SHOP_TAB; i++ )
    {
        m_pActiveTab[ i ] = NULL;
        m_pInActiveTab[ i ] = NULL;
        m_pTabText[ i ] = NULL;
    }

    for ( int i = 0; i < NUM_POINT_SHOP_CATEGORY; i++ )
        m_pCategory[ i ] = NULL;

    for ( int i = 0; i < NUM_POINT_SHOP_ITEM_LIST_ONE_PAGE; i++ )
        m_pItemList[ i ] = NULL;

    for ( int i = 0; i < NUM_POINT_SHOP_ORDER_BUTTON; i++ )
    {
        m_pASC_Image[ i ] = NULL;
        m_pDESC_Image[ i ] = NULL;
        m_pNone_Image[ i ] = NULL;
        m_pOrderText[ i ] = NULL;
    }

    m_SellItemList.clear();
    m_vecString.clear();
    m_listSrcString.clear();
    m_listCart.clear();
    m_listSearchWord.clear();

    m_sETC_UI_Option.m_OptionName = ID2GAMEWORD("POINT_SHOP_ETC_CATEGORY");
    m_sETC_UI_Option.m_TypeVec.clear();
}

CPointShopWindow::~CPointShopWindow()
{
}

void CPointShopWindow::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "BASIC_WINDOW_BODY_MAIN_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "BASIC_WINDOW_BODY_MAIN_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pMainBack = pLinBoxSmart;
    }

    {    
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        char* strActiveTab[ NUM_POINT_SHOP_TAB ] =
        {
            "POINT_SHOP_ACTIVE_TAB0"
            ,"POINT_SHOP_ACTIVE_TAB1"
        };

        char* strInActiveTab[ NUM_POINT_SHOP_TAB ] =
        {
            "POINT_SHOP_INACTIVE_TAB0"
            ,"POINT_SHOP_INACTIVE_TAB1"
        };

        char* strActiveTabLine[ NUM_POINT_SHOP_TAB ] =
        {
            "POINT_SHOP_ACTIVE_TAB0_LINE"
            , "POINT_SHOP_ACTIVE_TAB1_LINE"
        };

        char* strInActiveTabLine[ NUM_POINT_SHOP_TAB ] =
        {
            "POINT_SHOP_INACTIVE_TAB0_LINE"
            , "POINT_SHOP_INACTIVE_TAB1_LINE"
        };

        char* strUITabText[NUM_POINT_SHOP_TAB ] =
        {
            "POINT_SHOP_TAB_TEXT_0"
            , "POINT_SHOP_TAB_TEXT_1"
        };

        for ( int i = 0; i < NUM_POINT_SHOP_TAB; i++ )
        {
            CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
            pLinBoxSmart->CreateSub( this, strActiveTab[ i ], UI_FLAG_XSIZE | UI_FLAG_YSIZE, (POINT_SHOP_ACTIVE_TAB_0 + i) );
            pLinBoxSmart->CreateSubControl( strActiveTabLine[ i ] );
            RegisterControl( pLinBoxSmart );
            m_pActiveTab[ i ] = pLinBoxSmart;

            pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
            pLinBoxSmart->CreateSub( this, strInActiveTab[ i ], UI_FLAG_XSIZE | UI_FLAG_YSIZE, (POINT_SHOP_INACTIVE_TAB_0 + i) );
            pLinBoxSmart->CreateSubControl( strInActiveTabLine[ i ] );
            RegisterControl ( pLinBoxSmart );
            m_pInActiveTab[ i ] = pLinBoxSmart;
            m_pInActiveTab[ i ]->SetVisibleSingle( FALSE );

            CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
            pTextBox->CreateSub( this, strUITabText[ i ], UI_FLAG_DEFAULT );
            pTextBox->SetFont( pFont9 );
            pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
            RegisterControl( pTextBox );
            m_pTabText[ i ] = pTextBox;
            m_pTabText[ i ]->SetText( ID2GAMEWORD("POINT_SHOP_TAB_TEXT", i ), NS_UITEXTCOLOR::BLACK );
        }

        // Note : 아래 탭은 현재 비활성화.
        m_pActiveTab[ 1 ]->SetVisibleSingle( FALSE );
        m_pInActiveTab[ 1 ]->SetVisibleSingle( FALSE );
        m_pTabText[ 1 ]->SetVisibleSingle( FALSE );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "POINT_SHOP_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_MAIN_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pMain = pLinBoxSmart;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "POINT_SHOP_CATEGORY_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_CATEGORY_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pCategoryBack = pLinBoxSmart;
    }

    {
        CPointShopCategory* pCategory = new CPointShopCategory( m_pInterface, m_pEngineDevice );
        pCategory->CreateSub( this, "POINT_SHOP_CATEGORY_ALL", UI_FLAG_DEFAULT, POINT_SHOP_CATEGORY_ALL );
        pCategory->CreateSubControl();
        RegisterControl( pCategory );
        m_pCategoryAll = pCategory;
        m_pCategoryAll->SetEnableCategoryAll( true );

        pCategory = new CPointShopCategory( m_pInterface, m_pEngineDevice );
        pCategory->CreateSub( this, "POINT_SHOP_CATEGORY", UI_FLAG_DEFAULT, POINT_SHOP_CATEGORY_ETC );
        pCategory->CreateSubControl();
        RegisterControl( pCategory );
        m_pCategoryETC = pCategory;
        m_pCategoryETC->SetVisibleSingle( FALSE );
    }

    {
        for ( int i = 0; i < NUM_POINT_SHOP_CATEGORY; i++ )
        {
            CPointShopCategory* pCategory = new CPointShopCategory( m_pInterface, m_pEngineDevice );
            pCategory->CreateSub( this, "POINT_SHOP_CATEGORY", UI_FLAG_DEFAULT, (POINT_SHOP_CATEGORY_0 + i) );
            pCategory->CreateSubControl();
            RegisterControl( pCategory );
            m_pCategory[ i ] = pCategory;
            m_pCategory[ i ]->SetVisibleSingle( FALSE );
        }
    }

    {
        CPointShopCategory* pCategory = new CPointShopCategory( m_pInterface, m_pEngineDevice );
        pCategory->CreateSub( this, "POINT_SHOP_CATEGORY_START", UI_FLAG_DEFAULT );
        pCategory->CreateSubControl();
        RegisterControl( pCategory );
        m_pCategoryStart = pCategory;
        m_pCategoryStart->SetVisibleSingle( FALSE );
    }

    {
        for ( int i = 0; i < NUM_POINT_SHOP_ITEM_LIST_ONE_PAGE; i++ )
        {
            CPointShopNode* pItemNode = new CPointShopNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
            pItemNode->CreateSub( this, "POINT_SHOP_ITEM_LIST_NODE", UI_FLAG_DEFAULT, (POINT_SHOP_ITEL_LIST_NODE_0 + i) );
            pItemNode->CreateSubControl();
            RegisterControl( pItemNode );
            m_pItemList[ i ] = pItemNode;
            m_pItemList[ i ]->SetVisibleSingle( FALSE );
        }
    }

    {
        CPointShopNode* pItemNode = new CPointShopNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pItemNode->CreateSub( this, "POINT_SHOP_ITEM_LIST_NODE_LEFT", UI_FLAG_DEFAULT );
        pItemNode->CreateSubControl();
        RegisterControl( pItemNode );
        m_pItemListLeftTemp = pItemNode;
        m_pItemListLeftTemp->SetVisibleSingle( FALSE );

        pItemNode = new CPointShopNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pItemNode->CreateSub( this, "POINT_SHOP_ITEM_LIST_NODE_RIGHT", UI_FLAG_DEFAULT );
        pItemNode->CreateSubControl();
        RegisterControl( pItemNode );
        m_pItemListRightTemp = pItemNode;
        m_pItemListRightTemp->SetVisibleSingle( FALSE );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "POINT_SHOP_ITEM_LIST_SELECT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_LIST_SELECT_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pSelectItem = pLinBoxSmart;
        m_pSelectItem->SetVisibleSingle( FALSE );
    }

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_CATEGORY_LINE" );
        RegisterControl( pControl );
    }

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_PRICE_ORDER_NONE_0" );
        RegisterControl( pControl );
        m_pNone_Image[ 0 ] = pControl;
        m_pNone_Image[ 0 ]->SetVisibleSingle( TRUE );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_PRICE_ORDER_ASC_0" );
        RegisterControl( pControl );
        m_pASC_Image[ 0 ] = pControl;
        m_pASC_Image[ 0 ]->SetVisibleSingle( FALSE );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_PRICE_ORDER_DESC_0" );
        RegisterControl( pControl );
        m_pDESC_Image[ 0 ] = pControl;
        m_pDESC_Image[ 0 ]->SetVisibleSingle( FALSE );

        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_PRICE_ORDER_TEXT_0", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pOrderText[ 0 ] = pTextBox;
        m_pOrderText[ 0 ]->SetText( ID2GAMEWORD("POINT_SHOP_PRICE_ORDER_TEXT"), NS_UITEXTCOLOR::WHITE );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_PRICE_ORDER_BACK_0", UI_FLAG_DEFAULT, POINT_SHOP_ORDER_BUTTON );
        RegisterControl( pControl );
    }

    {
        CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "POINT_SHOP_PRE_PAGE_BUTTON", UI_FLAG_DEFAULT, POINT_SHOP_PRE_BUTTON );
        pButton->CreateFlip( "POINT_SHOP_PRE_PAGE_BUTTON_F", CBasicButton::CLICK_FLIP );
        RegisterControl( pButton );
        m_pPrePageButton = pButton;
        m_pPrePageButton->SetFlip( FALSE );

        pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "POINT_SHOP_NEXT_PAGE_BUTTON", UI_FLAG_DEFAULT, POINT_SHOP_NEXT_BUTTON );
        pButton->CreateFlip( "POINT_SHOP_NEXT_PAGE_BUTTON_F", CBasicButton::CLICK_FLIP );
        RegisterControl( pButton );
        m_pNextPageButton = pButton;
        m_pNextPageButton->SetFlip( FALSE );

        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_PRE_PAGE_BUTTON_OVER" );
        RegisterControl( pControl );
        m_pPrePageButtonOver = pControl;
        m_pPrePageButtonOver->SetVisibleSingle( FALSE );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_NEXT_PAGE_BUTTON_OVER" );
        RegisterControl( pControl );
        m_pNextPageButtonOver = pControl;
        m_pNextPageButtonOver->SetVisibleSingle( FALSE );

        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_CUR_PAGE_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pCurPageText = pTextBox;

        std::string strCurPage = sc::string::format( "%d", m_nCurPage + 1 );
        m_pCurPageText->SetText( strCurPage.c_str(), NS_UITEXTCOLOR::WHITE );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "POINT_SHOP_ITEM_SEARCH_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_SEARCH_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pItemSearchBack = pLinBoxSmart;
    }

    {
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
        pEditBoxMan->CreateSub( this, "POINT_SHOP_SEARCH_EDIT_MAN", UI_FLAG_DEFAULT, POINT_SHOP_ITEM_SEARCH_MAN );
        pEditBoxMan->CreateEditBox( POINT_SHOP_ITEM_SEARCH_EDIT, "POINT_SHOP_SEARCH_EDIT", "POINT_SHOP_SEARCH_CARRAT", TRUE, UINT_MAX, pFont9, 30 );
        RegisterControl( pEditBoxMan );
        m_pItemNameEdit = pEditBoxMan;
    }

    {
        const int nBUTTONSIZE = CBasicTextButton::SIZE19;

        CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
        pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POINT_SHOP_ITEM_SEARCH_BUTTON );
        pTextButton->CreateBaseButton( "POINT_SHOP_SEARCH_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, ID2GAMEWORD("POINT_SHOP_SEARCH_BUTTON") );
        RegisterControl( pTextButton );
        m_pItemSearchButton = pTextButton;
        m_pItemSearchButton->SetFlip( FALSE );
    }

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_RESEARCH_CHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pCheckResearch = pControl;
        m_pCheckResearch->SetVisibleSingle( FALSE );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_RESEARCH_UNCHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pUnCheckResearch = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_RESEARCH_CHECK_BACK", UI_FLAG_DEFAULT, POINT_SHOP_INCLUDE_TEXT_CHECK );
        RegisterControl( pControl );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_RESEARCH_LINK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
    }

    {
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_RESEARCH_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pResearchText = pTextBox;
        m_pResearchText->SetText( ID2GAMEINTEXT("POINT_SHOP_RESEARCH_TEXT") );
    }


    {
        CBasicComboBoxRollOver *pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POINT_SHOP_SEARCH_COMBO_ROLLOVER );
        pComboBoxRollOver->CreateBaseComboBoxRollOver( "POINT_SHOP_COMBO_ROLLOVER_SEARCH", FALSE );			
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        RegisterControl( pComboBoxRollOver );
        m_pComboRollSearchOver = pComboBoxRollOver;
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_POINT_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pPointText = pTextBox;
        m_pPointText->ClearText();
        m_pPointText->SetText( ID2GAMEWORD("POINT_SHOP_POINT_TEXT"), NS_UITEXTCOLOR::WHITE );

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_CUR_POINT_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pCurPointText = pTextBox;
    }

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_UNDER_LINE" );
        RegisterControl( pControl );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "POINT_SHOP_CART_LIST_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_CART_LIST_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pCartListBack = pLinBoxSmart;

        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_CART_ICON", UI_FLAG_DEFAULT, POINT_SHOP_EMPTY_CART_ICON );
        RegisterControl( pControl );
        m_pCartIconButton = pControl;
    }

    {
        CItemSlot* pItemSlot = new CItemSlot( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pItemSlot->CreateSub( this, "POINT_SHOP_CART_ITEM_SLOT", UI_FLAG_DEFAULT, POINT_SHOP_CART_ITEM_SLOT );
        pItemSlot->CreateSubControl( NUM_CART_LIST_ITEM, TRUE );
        RegisterControl( pItemSlot );
        m_pCartItemSlot = pItemSlot;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "POINT_SHOP_CART_ICON_HELP_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_CART_ICON_HELP_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pEmptyCartHelpBack = pLinBoxSmart;
        m_pEmptyCartHelpBack->SetVisibleSingle( FALSE );

        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_CART_ICON_HELP_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pEmptyCartHelpText = pTextBox;
        m_pEmptyCartHelpText->SetText( ID2GAMEINTEXT("POINT_SHOP_CART_ICON_HELP"), NS_UITEXTCOLOR::WHITE );
        m_pEmptyCartHelpText->SetVisibleSingle( FALSE );
    }

    {
        RnButton* pRnButton = NULL;

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
        //////////////////////////////////////////////////////////////////////////

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"POINT_SHOP_BUY_BUTTON",UI_FLAG_DEFAULT,POINT_SHOP_BUY_BUTTON);
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pBuyButton = pRnButton;

        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_BUY_BUTTON_TEXT_LEFT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pBuyButtonPointText = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_BUY_BUTTON_TEXT_RIGHT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pBuyButtonBuyText = pTextBox;
        m_pBuyButtonBuyText->SetText( ID2GAMEWORD( "POINT_SHOP_BUY_BUTTON_TEXT" ), NS_UITEXTCOLOR::ORANGE );
    }

    {
        CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "POINT_SHOP_HELP_BUTTON", UI_FLAG_DEFAULT, POINT_SHOP_HELP_BUTTON );
        pButton->CreateFlip( "POINT_SHOP_HELP_BUTTON_OVER", CBasicButton::MOUSEIN_FLIP );
        RegisterControl( pButton );
        m_pHelpButton = pButton;
        m_pHelpButton->SetFlip( FALSE );

        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_HELP_BUTTON_F" );
        RegisterControl( pControl );
        m_pHelpButtonOver = pControl;
        m_pHelpButtonOver->SetVisibleSingle( FALSE );
    }

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_VIEW_SEARCH_WORD_BUTTON" );
        RegisterControl( pControl );
        m_pViewSearchWordButton = pControl;
        m_pViewSearchWordButton->SetVisibleSingle( FALSE );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_UNVIEW_SEARCH_WORD_BUTTON" );
        RegisterControl( pControl );
        m_pUnViewSearchWordButton = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_VIEW_SEARCH_WORD_BUTTON_BACK", UI_FLAG_DEFAULT, POINT_SHOP_VIEW_SEARCH_WORD_BUTTON );
        RegisterControl( pControl );
    }

    {
        CBasicComboBoxRollOver *pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POINT_SHOP_SEARCH_LIST_COMBO_ROLLOVER );
        pComboBoxRollOver->CreateWhiteComboBoxRollOver( "POINT_SHOP_SEARCH_WORD_LIST_ROLLOVER", FALSE );
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        RegisterControl( pComboBoxRollOver );
        m_pSearchWordRollOver = pComboBoxRollOver;
    }

    {
        for ( int i = 0; i < RANPARAM::MAX_SEARCH_WORD; i++ )
        {
            std::string& strSearchWord = RANPARAM::strPointShopSearchWord[ i ];
            if ( strSearchWord.length() != 0 )
            {
                m_listSearchWord.push_back( strSearchWord );
            }
        }

        SetSearchWorldListText();
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "POINT_SHOP_NOTIFY_FULL_CART_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POINT_SHOP_NOTIFY_FULL_CART_BACK );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_NOTIFY_FULL_CART_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pNotifyFullCartBack = pLinBoxSmart;

        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "POINT_SHOP_NOTIFY_FULL_CART_TEXT", UI_FLAG_DEFAULT, POINT_SHOP_NOTIFY_FULL_CART_TEXT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pNotifyFullCartText = pTextBox;
        m_pNotifyFullCartText->AddText( ID2GAMEINTEXT("POINT_SHOP_NOTIFY_FULL_CART_TEXT0"), NS_UITEXTCOLOR::YELLOW );
        m_pNotifyFullCartText->AddText( ID2GAMEINTEXT("POINT_SHOP_NOTIFY_FULL_CART_TEXT1"), NS_UITEXTCOLOR::WHITE );

        UnViewNotifyFullCart();
    }

    {
        CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "POINT_SHOP_CATEGORY_RELOAD_BUTTON", UI_FLAG_DEFAULT, POINT_SHOP_CATEGORY_RELOAD_BUTTON );
        pButton->CreateFlip( "POINT_SHOP_CATEGORY_RELOAD_BUTTON_F", CBasicButton::CLICK_FLIP );
        RegisterControl( pButton );
        m_pCategoryReloadButton = pButton;
        m_pCategoryReloadButton->SetFlip( FALSE );

        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_CATEGORY_RELOAD_BUTTON_OVER" );
        RegisterControl( pControl );
        m_pCategoryReloadButtonOver = pControl;
        m_pCategoryReloadButtonOver->SetVisibleSingle( FALSE );
    }

    {
        m_pNotifyCategoryReloadButton = new CBasicVarTextBox(m_pEngineDevice);
        m_pNotifyCategoryReloadButton->CreateSub( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pNotifyCategoryReloadButton->CreateSubControl();
        m_pNotifyCategoryReloadButton->SetVisibleSingle( FALSE );
        RegisterControl( m_pNotifyCategoryReloadButton );
        m_pNotifyCategoryReloadButton->AddTextNoSplit( ID2GAMEINTEXT("POINT_SHOP_NOTIFY_CATEGORY_RELOAD"), NS_UITEXTCOLOR::YELLOW );
    }

    {
        CPointShopTooltip* pTooltip = new CPointShopTooltip( m_pInterface, m_pEngineDevice );
        pTooltip->CreateSub( this, "POINT_SHOP_TOOLTIP_BOX", UI_FLAG_DEFAULT );
        pTooltip->CreateSubControl();
        RegisterControl( pTooltip );
        m_pHelpButtonTooltip = pTooltip;
        pTooltip->SetVisibleSingle( FALSE );

        std::string strText;

        strText = sc::string::format( "%s", ID2GAMEINTEXT("POINT_SHOP_HELP_TOOLTIP_TITLE_0") );
        m_pHelpButtonTooltip->SetTextUpBoxTitle( strText, NS_UITEXTCOLOR::LIGHTSKYBLUE );
        strText = sc::string::format( "%s", ID2GAMEINTEXT("POINT_SHOP_HELP_TOOLTIP_TITLE_1") );
        m_pHelpButtonTooltip->SetTextDownBoxTitle( strText, NS_UITEXTCOLOR::LIGHTSKYBLUE );

        strText = sc::string::format( "%s", ID2GAMEINTEXT("POINT_SHOP_HELP_TOOLTIP_TEXT_0") );
        m_pHelpButtonTooltip->SetTextUpBoxText( strText, NS_UITEXTCOLOR::WHITE );
        strText = sc::string::format( "%s", ID2GAMEINTEXT("POINT_SHOP_HELP_TOOLTIP_TEXT_1") );
        m_pHelpButtonTooltip->SetTextDownBoxText( strText, NS_UITEXTCOLOR::WHITE );

        // Help Button Tooltip 위치 설정.
        const UIRECT& rcWindowLP = m_pHelpButton->GetLocalPos();
        const UIRECT& rcWindowGP = m_pHelpButton->GetGlobalPos();
        const UIRECT& rcTooltipLP = m_pHelpButtonTooltip->GetLocalPos();
        D3DXVECTOR2 vPos;

        const float fHelpButtonSizeX = rcWindowLP.sizeX;
        const float fHelpButtonSizeY = rcWindowLP.sizeY;
        const float fTooltipSizeX = rcTooltipLP.sizeX;
        const float fTooltipSizeY = rcTooltipLP.sizeY;

        vPos.x = rcWindowLP.left + fHelpButtonSizeX * 0.5f - fTooltipSizeX;
        vPos.y = rcWindowLP.top + fHelpButtonSizeY * 0.5f;
        m_pHelpButtonTooltip->SetLocalPos( vPos );

        vPos.x = rcWindowGP.left + fHelpButtonSizeX * 0.5f - fTooltipSizeX;
        vPos.y = rcWindowGP.top + fHelpButtonSizeY * 0.5f;
        m_pHelpButtonTooltip->SetGlobalPos( vPos );
    }

    {
        CPointShopTooltip* pTooltip = new CPointShopTooltip( m_pInterface, m_pEngineDevice );
        pTooltip->CreateSub( this, "POINT_SHOP_TOOLTIP_BOX", UI_FLAG_DEFAULT );
        pTooltip->CreateSubControl();
        RegisterControl( pTooltip );
        m_pBuyButtonTooltip = pTooltip;
        pTooltip->SetVisibleSingle( FALSE );

        std::string strText;

        strText = sc::string::format( "%s", ID2GAMEINTEXT("POINT_SHOP_BUY_BUTTON_TOOLTIP_TITLE_0") );
        m_pBuyButtonTooltip->SetTextUpBoxTitle( strText, NS_UITEXTCOLOR::LIGHTSKYBLUE );
        strText = sc::string::format( "%s", ID2GAMEINTEXT("POINT_SHOP_BUY_BUTTON_TOOLTIP_TITLE_1") );
        m_pBuyButtonTooltip->SetTextDownBoxTitle( strText, NS_UITEXTCOLOR::LIGHTSKYBLUE );

        strText = sc::string::format( "%s", ID2GAMEINTEXT("POINT_SHOP_BUY_BUTTON_TOOLTIP_TEXT_0") );
        m_pBuyButtonTooltip->SetTextUpBoxText( strText, NS_UITEXTCOLOR::WHITE );
        strText = sc::string::format( "%s", ID2GAMEINTEXT("POINT_SHOP_BUY_BUTTON_TOOLTIP_TEXT_1") );
        m_pBuyButtonTooltip->SetTextUpBoxText( strText, NS_UITEXTCOLOR::ORNAGERED );
        strText = sc::string::format( "%s", ID2GAMEINTEXT("POINT_SHOP_BUY_BUTTON_TOOLTIP_TEXT_2") );
        m_pBuyButtonTooltip->SetTextDownBoxText( strText, NS_UITEXTCOLOR::WHITE );
        strText = sc::string::format( "%s", ID2GAMEINTEXT("POINT_SHOP_BUY_BUTTON_TOOLTIP_TEXT_3") );
        m_pBuyButtonTooltip->SetTextDownBoxText( strText, NS_UITEXTCOLOR::WHITE );

        // Buy Button Tooltip 위치 설정.
        const UIRECT& rcWindowLP = m_pBuyButton->GetLocalPos();
        const UIRECT& rcWindowGP = m_pBuyButton->GetGlobalPos();
        const UIRECT& rcTooltipLP = m_pBuyButtonTooltip->GetLocalPos();
        D3DXVECTOR2 vPos;

        const float fTooltipSizeX = rcTooltipLP.sizeX;
        const float fTooltipSizeY = rcTooltipLP.sizeY;
        const float fOffset = 2.0f;

        vPos.x = rcWindowLP.right - fTooltipSizeX;
        vPos.y = rcWindowLP.top - fTooltipSizeY - fOffset;
        m_pBuyButtonTooltip->SetLocalPos( vPos );

        vPos.x = rcWindowGP.right - fTooltipSizeX;
        vPos.y = rcWindowGP.top - fTooltipSizeY - fOffset;
        m_pBuyButtonTooltip->SetGlobalPos( vPos );
    }
}

void CPointShopWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    if ( ControlID >= POINT_SHOP_ITEL_LIST_NODE_0 &&
        ControlID <= POINT_SHOP_ITEL_LIST_NODE_END )
    {
        if ( CHECK_MOUSE_IN ( dwMsg ) )
        {
            int nIndex = ControlID - POINT_SHOP_ITEL_LIST_NODE_0;

            SNATIVEID sItemID = m_pItemList[ nIndex ]->GetItemID();
            if ( sItemID != NATIVEID_NULL() )
            {
                SITEMCUSTOM sItemCustom( sItemID );

				const SITEM* pItemData = GLogicData::GetInstance().GetItem(sItemID);
                if (pItemData)
                {
                    sItemCustom.TurnNumSet(pItemData->GETAPPLYNUM());
                    sItemCustom.GenTypeSet(EMGEN_POINTSHOP);
                }

				m_pInterface->SHOW_ITEM_INFO(
					sItemCustom,
					FALSE, FALSE, FALSE, TRUE, FALSE,
					USHRT_MAX, USHRT_MAX );
            }

			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if( sItemID != NATIVEID_NULL() )
				{
					if( DxInputDevice::GetInstance().GetKeyState( DIK_LSHIFT ) & DXKEY_PRESSED )
					{
						WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
						if( pWishList )
						{
							pWishList->NewPointStoreItem(
								SITEMCUSTOM( sItemID ),
								m_pItemList[ nIndex ]->GetItemPrice() );
						}
						return;
					}
				}

				// 선택 활성화.
				const UIRECT& rcWindowLP = m_pItemList[ nIndex ]->GetLocalPos();
				D3DXVECTOR2 vPos;

				vPos.x = rcWindowLP.left;
				vPos.y = rcWindowLP.top;
				m_pSelectItem->SetLocalPos( vPos );

				const UIRECT& rcWindowGP = m_pItemList[ nIndex ]->GetGlobalPos();
				vPos.x = rcWindowGP.left;
				vPos.y = rcWindowGP.top;
				m_pSelectItem->SetGlobalPos( vPos );

				m_pSelectItem->SetVisibleSingle( TRUE );
			}
			else if ( CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
			{
				AddCartList( m_pItemList[ nIndex ]->GetItemID(),
					m_pItemList[ nIndex ]->GetItemPrice() );

				// 선택 활성화.
				const UIRECT& rcWindowLP = m_pItemList[ nIndex ]->GetLocalPos();
				D3DXVECTOR2 vPos;

				vPos.x = rcWindowLP.left;
				vPos.y = rcWindowLP.top;
				m_pSelectItem->SetLocalPos( vPos );

				const UIRECT& rcWindowGP = m_pItemList[ nIndex ]->GetGlobalPos();
				vPos.x = rcWindowGP.left;
				vPos.y = rcWindowGP.top;
				m_pSelectItem->SetGlobalPos( vPos );

				m_pSelectItem->SetVisibleSingle( TRUE );
			}
		}
    }

    switch ( ControlID )
    {
    case POINT_SHOP_CART_ITEM_SLOT:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                int nPosX = m_pCartItemSlot->GetItemIndex();
                if (nPosX < 0)
                    return;
                const SINVENITEM* pInvenItem = m_pCartItemSlot->GetItemImage( nPosX );
                {				
                    if (pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()))
                    {
                        SITEMCUSTOM sItemCustom = pInvenItem->sItemCustom;
                        {
                            const SITEM* pItemData = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
                            if (pItemData)
                            {
                                sItemCustom.TurnNumSet(pItemData->GETAPPLYNUM());
                                sItemCustom.GenTypeSet(EMGEN_POINTSHOP);
                            }
                        }

						m_pInterface->SHOW_ITEM_INFO(
							sItemCustom,
							FALSE, FALSE, FALSE, FALSE, FALSE,
							USHRT_MAX, USHRT_MAX );
                    }
                }
            }

            if ( CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
            {
                int nPosX = m_pCartItemSlot->GetItemIndex();
                if ( nPosX < 0 ) return;

                int nCarListSize = static_cast<int>( m_listCart.size() );
                if ( nPosX < nCarListSize )
                {
                    CART_ITER itr =  m_listCart.begin();
                    for ( int i = 0; i < nPosX && i < nCarListSize; i++ ) itr++;

                    m_listCart.erase( itr );

                    ReLoadCartList();
                }
            }
        }
        break;

    case POINT_SHOP_ITEM_SEARCH_MAN:
    case POINT_SHOP_ITEM_SEARCH_EDIT:
        {
            if( m_pItemNameEdit->GetEditLength( POINT_SHOP_ITEM_SEARCH_EDIT ) != 0 )
            {
                if( DxInputDevice::GetInstance().IsUpdatKeyState() )
                {
                    if( UIKeyCheck::GetInstance()->Check( DIK_PGUP, DXKEY_DOWN )  )
                    {
                        if( m_nComboPageNum > 0 ) 
                        {
                            // 이전 페이지로 이동
                            m_nComboPageNum--;
                            m_nSelectString = 0;

                            UpdateComoboItem();

                            m_pItemNameEdit->SetEditString( POINT_SHOP_ITEM_SEARCH_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                            m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                        }
                    }else if( UIKeyCheck::GetInstance()->Check( DIK_PGDN, DXKEY_DOWN )  )
                    {
                        if(((m_nComboPageNum+1)*MAX_ROLLOVER_TEXT) < (int)m_vecString.size()) 
                        {
                            m_nComboPageNum++;
                            m_nSelectString = 0;

                            UpdateComoboItem();

                            m_pItemNameEdit->SetEditString( POINT_SHOP_ITEM_SEARCH_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                            m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                        }

                    }else if( UIKeyCheck::GetInstance()->Check( DIK_UP, DXKEY_DOWN )  )
                    {
                        if(m_nSelectString > 0) 
                        {
                            m_nSelectString--;
                            m_pItemNameEdit->SetEditString( POINT_SHOP_ITEM_SEARCH_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                            m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                        }else if( m_nComboPageNum > 0 ) 
                        {
                            // 이전 페이지로 이동
                            m_nComboPageNum--;
                            m_nSelectString = MAX_ROLLOVER_TEXT-1;

                            UpdateComoboItem();

                            m_pItemNameEdit->SetEditString( POINT_SHOP_ITEM_SEARCH_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                            m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                        }
                    }else if( UIKeyCheck::GetInstance()->Check( DIK_DOWN, DXKEY_DOWN )  )
                    {
                        if(m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT) < (int)m_vecString.size()-1) 
                        {
                            if( m_nSelectString < MAX_ROLLOVER_TEXT-1 )
                            {
                                m_nSelectString++;
                                m_pItemNameEdit->SetEditString( POINT_SHOP_ITEM_SEARCH_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                                m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                            }else{
                                // 이후 페이지로 이동
                                m_nComboPageNum++;
                                m_nSelectString = 0;

                                UpdateComoboItem();

                                m_pItemNameEdit->SetEditString( POINT_SHOP_ITEM_SEARCH_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                                m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                            }
                        }
                    }else{
                        CString strItemName = m_pItemNameEdit->GetEditString( POINT_SHOP_ITEM_SEARCH_EDIT );

                        int nStrLength = strItemName.GetLength();

                        std::string TempItemName = sc::string::format( "%s", strItemName );
                        if ( strcmp( m_strBeforeEdit.c_str(), TempItemName.c_str() ) )
                        {
                            if( m_nBeforeStrLength < strItemName.GetLength() && m_vecString.size() != 0 )
                            {
                                std::vector<CString> vecString = m_vecString;

                                m_vecString.clear();
                                m_nSelectString = -1;
                                m_nComboPageNum      = 0;

                                for( size_t i = 0; i < vecString.size(); i++ )
                                {
                                    if( vecString[i].Find( strItemName.GetString() ) != -1 )
                                    {
                                        m_vecString.push_back( vecString[i] );
                                    }
                                }
                            }else{
                                m_vecString.clear();
                                m_nSelectString = -1;
                                m_nComboPageNum      = 0;

                                SRCLIST_ITER iter = m_listSrcString.begin();

                                for( ; iter != m_listSrcString.end(); ++iter )
                                {
                                    CString string = *iter;
                                    if( string.Find( strItemName.GetString() ) != -1 )
                                    {
                                        m_vecString.push_back( string );
                                    }
                                }

                            }
                            UpdateComoboItem();
                            if( m_vecString.size() != 0 ) m_nBeforeStrLength = strItemName.GetLength();

                            m_strBeforeEdit = sc::string::format( "%s", strItemName );
                        }
                    }
                }				
            }else{
                ComboInit();
            }

            if( DxInputDevice::GetInstance().IsUpdatKeyState() )
            {
                if ( m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->IsBegin() )
                {
                    if( UIKeyCheck::GetInstance()->Check( DIK_RETURN, DXKEY_DOWN )
                        || UIKeyCheck::GetInstance()->Check( DIK_NUMPADENTER, DXKEY_DOWN ) )
                    {
                        m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->EndEdit();
                        m_pComboRollSearchOver->SetVisibleSingle( FALSE );

                        ResetViewSearchWordList();

                        SearchItemList();
                    }
                }
            }

            if ( m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->IsBegin() )
            {
                if ( (UIMSG_LB_DOWN & dwMsg) ||
                    (UIMSG_MB_DOWN & dwMsg) ||
                    (UIMSG_RB_DOWN & dwMsg)
                    )
                {
                    m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->EndEdit();
                }
            }
        }
        break;

    case POINT_SHOP_SEARCH_COMBO_ROLLOVER:
            {
                if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
                {				
                    DWORD dwMsg = GetMessageEx();
                    if ( dwMsg & UIMSG_LB_UP )
                    {
                        dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
                        ResetMessageEx();
                        AddMessageEx( dwMsg );						
                    }
                    break;
                }

                if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
                {				
                    int nIndex = m_pComboRollSearchOver->GetSelectIndex();
                    if ( nIndex < 0 ) return ;
                    CString strTemp = m_pComboRollSearchOver->GetSelectText( nIndex );								
                    m_pItemNameEdit->SetEditString( POINT_SHOP_ITEM_SEARCH_EDIT, strTemp );
                    ComboInit();
                }
            }
            break;

     case POINT_SHOP_ITEM_SEARCH_BUTTON:
            {
                if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
                {
                    m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->EndEdit();
                    SearchItemList();
                }
            }
            break;

    case POINT_SHOP_ORDER_BUTTON:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                if ( m_nOrderButtonState == ORDER_BUTTON_NONE_STATE )
                {
                    m_nOrderButtonState= ORDER_BUTTON_DESC_STATE;

                    m_pNone_Image[ 0 ]->SetVisibleSingle( FALSE );
                    m_pDESC_Image[ 0 ]->SetVisibleSingle( TRUE );
                    m_pASC_Image[ 0 ]->SetVisibleSingle( FALSE );
                }
                else if ( m_nOrderButtonState == ORDER_BUTTON_DESC_STATE )
                {
                    m_nOrderButtonState= ORDER_BUTTON_ASC_STATE;

                    m_pNone_Image[ 0 ]->SetVisibleSingle( FALSE );
                    m_pDESC_Image[ 0 ]->SetVisibleSingle( FALSE );
                    m_pASC_Image[ 0 ]->SetVisibleSingle( TRUE );
                }
                else if ( m_nOrderButtonState == ORDER_BUTTON_ASC_STATE )
                {
                    m_nOrderButtonState= ORDER_BUTTON_NONE_STATE;

                    m_pNone_Image[ 0 ]->SetVisibleSingle( TRUE );
                    m_pDESC_Image[ 0 ]->SetVisibleSingle( FALSE );
                    m_pASC_Image[ 0 ]->SetVisibleSingle( FALSE );
                }

                ReLoadSellItemList();
                ReLoadSellItemNodes();
            }
        }
        break;

    case POINT_SHOP_PRE_BUTTON:
        {
            if ( CHECK_MOUSE_IN( dwMsg )
                && !m_pPrePageButton->IsFlip() )
                m_pPrePageButtonOver->SetVisibleSingle( TRUE );
            else
                m_pPrePageButtonOver->SetVisibleSingle( FALSE );

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                PreItemListPage();
            }
        }
        break;

    case POINT_SHOP_NEXT_BUTTON:
        {
            if ( CHECK_MOUSE_IN( dwMsg )
                && !m_pNextPageButton->IsFlip() )
                m_pNextPageButtonOver->SetVisibleSingle( TRUE );
            else
                m_pNextPageButtonOver->SetVisibleSingle( FALSE );

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                NextItemListPage();
            }
        }
        break;

    case POINT_SHOP_INCLUDE_TEXT_CHECK:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                if ( m_bCheckIncludeText )
                {
                    UnCheckIncludeText();

                    ReLoadSellItemList();
                    ReLoadSellItemNodes();
                }
                else
                {
                    CheckIncludeText();

                    ReLoadSellItemList();
                    ReLoadSellItemNodes();
                }
            }
        }
        break;

    case POINT_SHOP_EMPTY_CART_ICON:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                m_pEmptyCartHelpBack->SetVisibleSingle( TRUE );
                m_pEmptyCartHelpText->SetVisibleSingle( TRUE );
            }
            else
            {
                m_pEmptyCartHelpBack->SetVisibleSingle( FALSE );
                m_pEmptyCartHelpText->SetVisibleSingle( FALSE );
            }

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                ResetCartList();
            }
        }
        break;

    case POINT_SHOP_VIEW_SEARCH_WORD_BUTTON:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                if ( m_bViewSearchWord )
                {
                    UnViewSearchWordList();
                }
                else
                {
                    ViewSearchWordList();
                }
            }
        }
        break;

    case POINT_SHOP_SEARCH_LIST_COMBO_ROLLOVER:
        {
            if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
            {				
                DWORD dwMsg = GetMessageEx();
                if ( dwMsg & UIMSG_LB_UP )
                {
                    dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
                    ResetMessageEx();
                    AddMessageEx( dwMsg );						
                }
                break;
            }

            if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
            {				
                int nIndex = m_pSearchWordRollOver->GetSelectIndex();
                if ( nIndex < 0 ) return ;
                CString strTemp = m_pSearchWordRollOver->GetSelectText( nIndex );								
                m_pItemNameEdit->SetEditString( POINT_SHOP_ITEM_SEARCH_EDIT, strTemp );

                m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->EndEdit();
                UnViewSearchWordList();
                SearchItemList();
            }
        }
        break;

    case POINT_SHOP_NOTIFY_FULL_CART_BACK:
    case POINT_SHOP_NOTIFY_FULL_CART_TEXT:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
                UnViewNotifyFullCart();
        }
        break;

    case POINT_SHOP_CATEGORY_RELOAD_BUTTON:
        {
            if ( CHECK_MOUSE_IN( dwMsg )
                && !m_pCategoryReloadButton->IsFlip() )
            {
                m_pCategoryReloadButtonOver->SetVisibleSingle( TRUE );

                m_pNotifyCategoryReloadButton->SetVisibleSingle( TRUE );
            }
            else
            {
                m_pCategoryReloadButtonOver->SetVisibleSingle( FALSE );

                m_pNotifyCategoryReloadButton->SetVisibleSingle( FALSE );
            }

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
                ReloadCategoryTypeForUser();
        }
        break;

    case POINT_SHOP_BUY_BUTTON:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                // Note : 도움말 보기.
                if (m_bHelpButton)
                    m_pBuyButtonTooltip->SetVisibleSingle( TRUE );
            }
            else
            {
                // Note : 도움말 끄기.
                m_pBuyButtonTooltip->SetVisibleSingle( FALSE );
            }

            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
                BuyItemList();
        }
        break;

    case POINT_SHOP_HELP_BUTTON:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                // Note : 도움말 보기.
                m_pHelpButtonTooltip->SetVisibleSingle( TRUE );
            }
            else
            {
                // Note : 도움말 끄기.
                m_pHelpButtonTooltip->SetVisibleSingle( FALSE );
            }

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                if ( m_bHelpButton )
                {
                    m_bHelpButton = false;
                    m_pHelpButtonOver->SetVisibleSingle( FALSE );
                }
                else
                {
                    m_bHelpButton = true;
                    m_pHelpButtonOver->SetVisibleSingle( TRUE );
                }
            }
        }
        break;

    case ET_CONTROL_BUTTON:
        {
            if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->EndEdit();
                m_pInterface->UiHideGroup( GetWndID() );
            }
        }
        break;
    }

    CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

    if( DxInputDevice::GetInstance().IsUpdatKeyState() )
    {
        if( UIKeyCheck::GetInstance()->Check( DIK_ESCAPE, DXKEY_DOWN )  )
        {
            m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->EndEdit();
            //m_pInterface->UiHideGroup( GetWndID() );
        }
    }
}

void CPointShopWindow::SetVisibleSingle( BOOL bVisible )
{
    CUIWindowEx::SetVisibleSingle( bVisible );

    if ( bVisible )
    {
        m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->EndEdit();
        m_pComboRollSearchOver->ClearText();
    }
    else
    {
        m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->EndEdit();
    }
}

void CPointShopWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( m_pNotifyFullCartBack->IsVisible() )
    {
        m_fNotifyFullCartTime -= fElapsedTime;

        if ( m_fNotifyFullCartTime <= 0.0f )
        {
            m_fNotifyFullCartFadeTime -= fElapsedTime;
            float fPercent = m_fNotifyFullCartFadeTime / fNOTIFY_FULL_CART_FADE_TIME;
            WORD wDiffuse = static_cast<WORD>( 255 * fPercent );

            m_pNotifyFullCartBack->SetDiffuseAlpha( wDiffuse );
            m_pNotifyFullCartText->SetDiffuseAlpha( wDiffuse );

            if ( m_fNotifyFullCartFadeTime <= 0.0f )
                UnViewNotifyFullCart();
        }
    }

    if ( !m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->IsBegin() )
    {
        if ( UIKeyCheck::GetInstance()->Check( DIK_RETURN, DXKEY_DOWN ) ||
            UIKeyCheck::GetInstance()->Check( DIK_NUMPADENTER, DXKEY_DOWN ) )
        {
            CUIControl::AddMessageEx( UIMSG_KEY_FOCUSED );

            m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->BeginEdit();
        }
    }
}

void CPointShopWindow::ResetAll()
{
    m_pItemNameEdit->ClearEdit( POINT_SHOP_ITEM_SEARCH_EDIT );

    m_pItemNameEdit->Init();
    m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->EndEdit();
    m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->BeginEdit();

    m_nSelectString = 0;
    m_nComboPageNum = 0;
}

void CPointShopWindow::RefreshWindow()
{
    // Note : 현재 상점 정보가 최신인지 확인한다.
    if ( m_UpdateDate != m_pGaeaClient->GetPointShopClient()->UpdateDate() )
        ReLoadAll();
}

void CPointShopWindow::ReLoadAll()
{
    m_UpdateDate = m_pGaeaClient->GetPointShopClient()->UpdateDate();

    ReLoadSellItemList();
    ReLoadSellItemNodes();
    ReLoadCategory();
    LoadSearchList();

    if ( !m_bInit )
    {
        m_bInit = true;

        InitCartList();
    }
}

void CPointShopWindow::ReLoadSellItemList()
{
    m_nCurPage = 0;
    PrintPage();

    m_SellItemList.clear();
    std::string SearchName("");
    std::vector<EMITEM_TYPE> TypeList;
    std::vector<pointshop::SEARCH_ORDER> OrderList;

    if ( m_pItemNameEdit->GetEditLength( POINT_SHOP_ITEM_SEARCH_EDIT ) != 0
        && m_bCheckIncludeText )
    {
        SearchName = sc::string::format( "%s", m_pItemNameEdit->GetEditString( POINT_SHOP_ITEM_SEARCH_EDIT ) );
    }

    GetCategoryCheckType( TypeList );
    GetOrderState( OrderList );

    m_pGaeaClient->GetPointShopClient()->GetList( m_SellItemList, SearchName, TypeList, OrderList );
}

void CPointShopWindow::GetCategoryCheckType( std::vector<EMITEM_TYPE>& TypeList )
{
    {
        const std::vector<pointshop::UI_OPTION> vecUI_Option = m_pGaeaClient->GetPointShopClient()->GetUiOption();

        int nUI_OptionSize = static_cast<int>( vecUI_Option.size() );

        for ( int i = 0; i < NUM_POINT_SHOP_CATEGORY; i++ )
        {
            if ( i >= nUI_OptionSize ) break;

            if ( m_pCategory[ i ]->IsCheck() )
            {
                pointshop::UI_OPTION sUI_Option = vecUI_Option[ i ];
                int nTypeVecSize = static_cast<int>( sUI_Option.m_TypeVec.size() );

                for ( int i = 0; i < nTypeVecSize; i++ )
                    TypeList.push_back( sUI_Option.m_TypeVec[ i ] );
            }
        }
    }

    {
        if ( m_pCategoryETC->IsCheck() )
        {
            pointshop::UI_OPTION sUI_Option = m_sETC_UI_Option;
            int nTypeVecSize = static_cast<int>( sUI_Option.m_TypeVec.size() );

            for ( int i = 0; i < nTypeVecSize; i++ )
                TypeList.push_back( sUI_Option.m_TypeVec[ i ] );
        }
    }
}

void CPointShopWindow::GetOrderState( std::vector<pointshop::SEARCH_ORDER>& OrderList )
{
    switch ( m_nOrderButtonState )
    {
    case ORDER_BUTTON_DESC_STATE:
        {
            pointshop::SEARCH_ORDER sOrder( pointshop::ITEM_PRICE, pointshop::SQL_DESC );
            OrderList.push_back( sOrder );
        }
        break;

    case ORDER_BUTTON_ASC_STATE:
        {
            pointshop::SEARCH_ORDER sOrder( pointshop::ITEM_PRICE, pointshop::SQL_ASC );
            OrderList.push_back( sOrder );
        }
        break;

    default:
        {
            pointshop::SEARCH_ORDER sOrder( pointshop::ITEM_NAME, pointshop::SQL_ASC );
            OrderList.push_back( sOrder );
        }
        break;
    }
}

void CPointShopWindow::ReLoadSellItemNodes()
{
    m_pSelectItem->SetVisibleSingle( FALSE );

    for ( int i = 0; i < NUM_POINT_SHOP_ITEM_LIST_ONE_PAGE; i++ )
        m_pItemList[ i ]->SetVisibleSingle( FALSE );

    int nStartIndex = NUM_POINT_SHOP_ITEM_LIST_ONE_PAGE * m_nCurPage;
    int nItemListSize = static_cast<int>(m_SellItemList.size());
    if ( nStartIndex >= nItemListSize ) nStartIndex = 0;

    for ( int i = 0; i < NUM_POINT_SHOP_ITEM_LIST_ONE_PAGE; i++ )
    {
        if ( nStartIndex >= nItemListSize ) break;

        // 리스트 노드의 정보 설정.
        pointshop::SellItemClient& SellItem = m_SellItemList[ nStartIndex++ ];
        m_pItemList[ i ]->SetItem( SellItem.ItemId(), SellItem.m_ItemName, SellItem.Price() );

        // 리스트 노드의 위치 설정.
        int nLineSpace = 2;

        if ( i % 2 == 0 )
        {   // 왼쪽 열.
            const UIRECT& rcWindowLP = m_pItemListLeftTemp->GetLocalPos();
            D3DXVECTOR2 vPos;

            int nIndex = static_cast<int>( floor( i / 2.0 ) );

            vPos.x = rcWindowLP.left;
            vPos.y = rcWindowLP.top + ( ( rcWindowLP.sizeY + nLineSpace ) * nIndex );
            m_pItemList[ i ]->SetLocalPos( vPos );

            const UIRECT& rcWindowGP = m_pItemListLeftTemp->GetGlobalPos();
            vPos.x = rcWindowGP.left;
            vPos.y = rcWindowGP.top + ( ( rcWindowGP.sizeY + nLineSpace ) * nIndex );
            m_pItemList[ i ]->SetGlobalPos( vPos );
        }
        else
        {   // 오른쪽 열.
            const UIRECT& rcWindowLP = m_pItemListRightTemp->GetLocalPos();
            D3DXVECTOR2 vPos;

            int nIndex = static_cast<int>( floor( i / 2.0 ) );

            vPos.x = rcWindowLP.left;
            vPos.y = rcWindowLP.top + ( ( rcWindowLP.sizeY + nLineSpace ) * nIndex );
            m_pItemList[ i ]->SetLocalPos( vPos );

            const UIRECT& rcWindowGP = m_pItemListRightTemp->GetGlobalPos();
            vPos.x = rcWindowGP.left;
            vPos.y = rcWindowGP.top + ( ( rcWindowGP.sizeY + nLineSpace ) * nIndex );
            m_pItemList[ i ]->SetGlobalPos( vPos );
        }

        m_pItemList[ i ]->SetVisibleSingle( TRUE );
    }
}

void CPointShopWindow::ReLoadCategory()
{
    for ( int i = 0; i < NUM_POINT_SHOP_CATEGORY; i++ )
    {
        m_pCategory[ i ]->SetVisibleSingle( FALSE );
    }
    m_pCategoryETC->SetVisibleSingle( FALSE );

    const std::vector<pointshop::UI_OPTION> vecUI_Option = m_pGaeaClient->GetPointShopClient()->GetUiOption();

    const UIRECT& rcWindowLP = m_pCategoryStart->GetLocalPos();
    const UIRECT& rcWindowGP = m_pCategoryStart->GetGlobalPos();
    D3DXVECTOR2 vPos;

    int nUI_OptionSize = static_cast<int>( vecUI_Option.size() );

    bool bTypeInCategory[ ITEM_NSIZE ] = { false, };

    int nCategoryIndex = 0;
    for ( nCategoryIndex = 0; nCategoryIndex < NUM_POINT_SHOP_CATEGORY; nCategoryIndex++ )
    {
        if ( nCategoryIndex >= nUI_OptionSize ) break;

        // 카테고리 위치 설정.
        int nLineSpace = 7;

        vPos.x = rcWindowLP.left;
        vPos.y = rcWindowLP.top + ( ( rcWindowLP.sizeY + nLineSpace ) * nCategoryIndex );
        m_pCategory[ nCategoryIndex ]->SetLocalPos( vPos );

        vPos.x = rcWindowGP.left;
        vPos.y = rcWindowGP.top + ( ( rcWindowGP.sizeY + nLineSpace ) * nCategoryIndex );
        m_pCategory[ nCategoryIndex ]->SetGlobalPos( vPos );

        m_pCategory[ nCategoryIndex ]->SetVisibleSingle( TRUE );

        std::vector<pointshop::SellItemClient> vecFindItemList;
        vecFindItemList.clear();
        std::string SearchName("");
        std::vector<pointshop::SEARCH_ORDER> OrderList;
        pointshop::UI_OPTION sUI_Option = vecUI_Option[ nCategoryIndex ];

        m_pGaeaClient->GetPointShopClient()->GetList( vecFindItemList, SearchName,
            sUI_Option.TypeVec(), OrderList );

        int nCount = static_cast<int>( vecFindItemList.size() );
        m_pCategory[ nCategoryIndex ]->SetCategoryData( sUI_Option.OptionName(), nCount );

        // ui option category vector에 있는 item type 체크.
        int nTypeVecSize = static_cast<int>( sUI_Option.m_TypeVec.size() );
        for ( int i = 0; i < nTypeVecSize; i++ )
            bTypeInCategory[ sUI_Option.m_TypeVec[ i ] ] = true;
    }

    {
        m_sETC_UI_Option.m_TypeVec.clear();
        for ( int i = 0; i < ITEM_NSIZE; i++ )
        {
            if ( !bTypeInCategory[ i ] )
                m_sETC_UI_Option.m_TypeVec.push_back( static_cast<EMITEM_TYPE>( i ) );
        }

        // 그 외 항목 카테고리.
        int nLineSpace = 7;

        vPos.x = rcWindowLP.left;
        vPos.y = rcWindowLP.top + ( ( rcWindowLP.sizeY + nLineSpace ) * nCategoryIndex );
        m_pCategoryETC->SetLocalPos( vPos );

        vPos.x = rcWindowGP.left;
        vPos.y = rcWindowGP.top + ( ( rcWindowGP.sizeY + nLineSpace ) * nCategoryIndex );
        m_pCategoryETC->SetGlobalPos( vPos );

        m_pCategoryETC->SetVisibleSingle( TRUE );

        std::vector<pointshop::SellItemClient> vecFindItemList;
        vecFindItemList.clear();
        std::string SearchName("");
        std::vector<pointshop::SEARCH_ORDER> OrderList;

        m_pGaeaClient->GetPointShopClient()->GetList( vecFindItemList, SearchName,
            m_sETC_UI_Option.TypeVec(), OrderList );

        int nCount = static_cast<int>( vecFindItemList.size() );
        m_pCategoryETC->SetCategoryData( m_sETC_UI_Option.OptionName(), nCount );
    }
}

void CPointShopWindow::PreItemListPage()
{
    if ( m_nCurPage <= 0 )
        return;

    m_nCurPage--;

    ReLoadSellItemNodes();
    PrintPage();
}

void CPointShopWindow::NextItemListPage()
{
    int nListCount = static_cast<int>(m_SellItemList.size()) - 1;
    int nLimitPage = static_cast<int>( floor( nListCount / 20.0 ) );

    if ( m_nCurPage >= nLimitPage )
        return;

    m_nCurPage++;

    ReLoadSellItemNodes();
    PrintPage();
}

void CPointShopWindow::PrintPage()
{
    std::string strCurPage = sc::string::format( "%d", m_nCurPage + 1 );
    m_pCurPageText->SetText( strCurPage.c_str(), NS_UITEXTCOLOR::WHITE );
}
void CPointShopWindow::ComboInit()
{
    m_pComboRollSearchOver->SetVisibleSingle(FALSE);
    m_pComboRollSearchOver->ClearText();
    m_vecString.clear();
    m_nBeforeStrLength = 0;
    m_nSelectString = -1;
    m_nComboPageNum = 0;

    ResetViewSearchWordList();
}

void CPointShopWindow::UpdateComoboItem()
{
    if( m_vecString.size() != 0 )
    {
        m_pComboRollSearchOver->SetVisibleSingle(TRUE);
        m_pComboRollSearchOver->ClearText();
        UnViewSearchWordList();
        size_t count = 0;
        for( size_t i = (m_nComboPageNum*MAX_ROLLOVER_TEXT); i < m_vecString.size(); i++ )
        {						
            if( m_vecString.size() > MAX_ROLLOVER_TEXT ) 
                m_pComboRollSearchOver->AddText(m_vecString[i], TRUE, TRUE );
            else				  
                m_pComboRollSearchOver->AddText(m_vecString[i], FALSE, TRUE );

            count++;
            if( count >= MAX_ROLLOVER_TEXT ) break;
        }						
    }else{
        m_pComboRollSearchOver->SetVisibleSingle(FALSE);
        m_pComboRollSearchOver->ClearText();

        ResetViewSearchWordList();
    }
}

void CPointShopWindow::LoadSearchList()
{
    m_listSrcString.clear();

    int nItemListSize = static_cast<int>( m_SellItemList.size() );
    for ( int i = 0; i < nItemListSize; i++ )
    {
        pointshop::SellItemClient& SellItem = m_SellItemList[ i ];
        CString strItemName;
        strItemName.Format( "%s", SellItem.Name().c_str() );

        m_listSrcString.push_back( strItemName );
    }
}

void CPointShopWindow::SetPointText( __int64 nPoint )
{
    m_pCurPointText->ClearText();

    std::string strPoint = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( nPoint, 3, "," );
    m_pCurPointText->SetText( strPoint.c_str(), NS_UITEXTCOLOR::ORANGE );
}

void CPointShopWindow::SearchItemList()
{
    CheckIncludeText();

    ReLoadSellItemList();
    ReLoadSellItemNodes();

    std::string& strSearchWord = sc::string::format( "%s", m_pItemNameEdit->GetEditString( POINT_SHOP_ITEM_SEARCH_EDIT ) );
    AddSearchWordList( strSearchWord );
}

void CPointShopWindow::AddCartList( SNATIVEID ItemID, __int64 Price )
{
    int nCartListSize = static_cast<int>( m_listCart.size() );

    if ( nCartListSize >= NUM_CART_LIST_ITEM )
    {
        ViewNotifyFullCart();
        return;
    }

    pointshop::SellItem SelectItem( ItemID, Price );

    bool bNewAdd = true;
    CART_ITER itr = m_listCart.begin();
    for ( int nIndex = 0; itr != m_listCart.end(); itr++ , nIndex++ )
    {
        pointshop::SellItem& CartItem = (*itr);
        if ( CartItem.ItemId() == SelectItem.ItemId() )
        {
            const SITEM* pItem = GLogicData::GetInstance().GetItem( CartItem.ItemId() );
            if ( !pItem ) return;

            // Note : 추후 아이템 갯수 하나 더 추가할때 아래에 코드를 추가한다.
            //bNewAdd = false;
        }
    }

    if ( bNewAdd )
        m_listCart.push_back( SelectItem );

    ReLoadCartList();
}

void CPointShopWindow::ReLoadCartList()
{
    for ( int i = 0; i < NUM_CART_LIST_ITEM; i++ )
    {
        m_pCartItemSlot->ResetItemImage( i );

        RANPARAM::dwPointShopWishList[ i ] = NATIVEID_NULL().dwID;
    }

    __int64 nTotalPrice = 0;
    CART_ITER itr = m_listCart.begin();
    for ( int i = 0; itr != m_listCart.end(); itr++, i++ )
    {        
        SINVENITEM sInvenItem;
        pointshop::SellItem& CartItem = (*itr);
        sInvenItem.sItemCustom = SITEMCUSTOM( CartItem.ItemId() );
        {
            SNATIVEID sItemID = CartItem.ItemId();
            const SITEM* pItemData = GLogicData::GetInstance().GetItem(sItemID);
            if (pItemData)
            {
                sInvenItem.sItemCustom.TurnNumSet(pItemData->GETAPPLYNUM());
                sInvenItem.sItemCustom.GenTypeSet(EMGEN_POINTSHOP);
            }
        }
        m_pCartItemSlot->SetItemImage( i, sInvenItem );

        nTotalPrice += CartItem.Price();

        RANPARAM::dwPointShopWishList[ i ] = CartItem.ItemId().dwID;
    }

    m_pBuyButtonPointText->ClearText();

    if ( nTotalPrice != 0 )
    {
        m_nTotalPrice = nTotalPrice;

        std::string strTotalPrice = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( nTotalPrice, 3, "," );

        __int64 nCurCharPoint = m_pGaeaClient->PointShopChaPoint();
        if ( nTotalPrice <= nCurCharPoint )
            m_pBuyButtonPointText->SetText( strTotalPrice.c_str(), NS_UITEXTCOLOR::WHITE );
        else
            m_pBuyButtonPointText->SetText( strTotalPrice.c_str(), NS_UITEXTCOLOR::RED );
    }
}

void CPointShopWindow::ResetCartList()
{
    for ( int i = 0; i < NUM_CART_LIST_ITEM; i++ )
        m_pCartItemSlot->ResetItemImage( i );

    m_listCart.clear();

    ReLoadCartList();
}

void CPointShopWindow::SetAllCategory( bool bCheck )
{
    if ( bCheck )
    {
        m_pCategoryAll->Check();

        for ( int i = 0; i < NUM_POINT_SHOP_CATEGORY; i++ )
        {
            m_pCategory[ i ]->Check();
        }
        m_pCategoryETC->Check();
    }
    else
    {
        m_pCategoryAll->UnCheck();

        for ( int i = 0; i < NUM_POINT_SHOP_CATEGORY; i++ )
        {
            m_pCategory[ i ]->UnCheck();
        }
        m_pCategoryETC->UnCheck();
    }
}

void CPointShopWindow::CheckIncludeText()
{
    m_bCheckIncludeText = true;

    ResetAutoCategory();

    m_pCheckResearch->SetVisibleSingle( TRUE );
    m_pUnCheckResearch->SetVisibleSingle( FALSE );

    m_SellItemList.clear();
    std::string SearchName("");
    std::vector<EMITEM_TYPE> TypeList;
    std::vector<pointshop::SEARCH_ORDER> OrderList;

    if ( m_pItemNameEdit->GetEditLength( POINT_SHOP_ITEM_SEARCH_EDIT ) != 0 )
    {
        SearchName = sc::string::format( "%s", m_pItemNameEdit->GetEditString( POINT_SHOP_ITEM_SEARCH_EDIT ) );
        GetOrderState( OrderList );
        m_pGaeaClient->GetPointShopClient()->GetList( m_SellItemList, SearchName, TypeList, OrderList );
        CheckAutoCategory();
    }
}

void CPointShopWindow::UnCheckIncludeText()
{
    m_bCheckIncludeText = false;

    ResetAutoCategory();

    m_pCheckResearch->SetVisibleSingle( FALSE );
    m_pUnCheckResearch->SetVisibleSingle( TRUE );
}

void CPointShopWindow::CheckAutoCategory()
{
    for ( int j = 0; j < NUM_POINT_SHOP_CATEGORY; j++ )
    {
        m_pCategory[ j ]->UnCheck();
    }

    int nItemListSize = static_cast<int>( m_SellItemList.size() );

    const std::vector<pointshop::UI_OPTION> vecUI_Option = m_pGaeaClient->GetPointShopClient()->GetUiOption();
    int nUI_OptionSize = static_cast<int>( vecUI_Option.size() );

    bool bFind = false;
    for ( int i = 0; i < nItemListSize; i++ )
    {
        pointshop::SellItemClient& SellItem = m_SellItemList[ i ];
        for ( int j = 0; j < NUM_POINT_SHOP_CATEGORY; j++ )
        {
            if ( j >= nUI_OptionSize ) break;
            if ( m_pCategory[ j ]->IsForceChecked() ) continue;

            pointshop::UI_OPTION sUI_Option = vecUI_Option[ j ];
            std::vector<EMITEM_TYPE> vecTypeOption = sUI_Option.m_TypeVec;
            std::vector<EMITEM_TYPE>::iterator itr = vecTypeOption.begin();

            for ( ; itr != vecTypeOption.end(); itr++ )
            {
                if ( SellItem.Type() == (*itr) )
                {
                    m_pCategory[ j ]->SetForceChecked( true );
                    m_pCategory[ j ]->Check();

                    bFind = true;
                }
            }
        }

        {
            if ( m_pCategoryETC->IsForceChecked() ) continue;

            pointshop::UI_OPTION sUI_Option = m_sETC_UI_Option;
            std::vector<EMITEM_TYPE> vecTypeOption = sUI_Option.m_TypeVec;
            std::vector<EMITEM_TYPE>::iterator itr = vecTypeOption.begin();

            for ( ; itr != vecTypeOption.end(); itr++ )
            {
                if ( SellItem.Type() == (*itr) )
                {
                    m_pCategoryETC->SetForceChecked( true );
                    m_pCategoryETC->Check();

                    bFind = true;
                }
            }
        }
    }

    if ( bFind )
        m_pCategoryAll->UnCheck();
}

void CPointShopWindow::ResetAutoCategory()
{
    for ( int i = 0; i < NUM_POINT_SHOP_CATEGORY; i++ )
        m_pCategory[ i ]->SetForceChecked( false );
    m_pCategoryETC->SetForceChecked( false );
}

void CPointShopWindow::BuyItemList()
{
    CART_ITER itr;

    CART_LIST tempList = m_listCart;
    itr = tempList.begin();
    for ( int nIndex = 0; itr != tempList.end(); itr++, nIndex++ )
    {
        pointshop::SellItem& CartItem = (*itr);

        m_pGaeaClient->PointShopBuyItem( CartItem.ItemId() );
    }
}

void CPointShopWindow::AddSearchWordList( std::string& strSearchWord )
{
    if ( strSearchWord.length() != 0 )
    {
        int nListSize = static_cast<int>( m_listSearchWord.size() );

        if ( nListSize >= RANPARAM::MAX_SEARCH_WORD )
        {
            m_listSearchWord.erase( m_listSearchWord.begin() );
        }

        m_listSearchWord.push_back( strSearchWord );

        SetSearchWorldListText();
    }
}

void CPointShopWindow::SetSearchWorldListText()
{
    int nListSize = static_cast<int>( m_listSearchWord.size() );

    std::vector<std::string> vecTemp;
    vecTemp.reserve( nListSize );

    std::list<std::string>::iterator itr = m_listSearchWord.begin();
    for ( int i = 0 ; itr != m_listSearchWord.end(); itr++, i++ )
    {
        vecTemp.push_back( (*itr) );
    }

    m_pSearchWordRollOver->ClearText();
    int nVectorSize = static_cast<int>( vecTemp.size() );
    for ( int i = nVectorSize - 1; i >= 0; i-- )
    {
        std::string& strSearchWord = vecTemp[ i ];

        // 롤오버에 텍스트 추가.
        m_pSearchWordRollOver->AddText( strSearchWord.c_str(), true, true, NS_UITEXTCOLOR::BLACK );

        RANPARAM::strPointShopSearchWord[ i ] = strSearchWord;
    }
}

void CPointShopWindow::ViewSearchWordList()
{
    m_bViewSearchWord = true;

    m_pViewSearchWordButton->SetVisibleSingle( TRUE );
    m_pUnViewSearchWordButton->SetVisibleSingle( FALSE );
    m_pSearchWordRollOver->SetVisibleSingle( TRUE );
    m_pComboRollSearchOver->SetVisibleSingle( FALSE );
}

void CPointShopWindow::UnViewSearchWordList()
{
    m_bViewSearchWord = false;

    m_pViewSearchWordButton->SetVisibleSingle( FALSE );
    m_pUnViewSearchWordButton->SetVisibleSingle( TRUE );
    m_pSearchWordRollOver->SetVisibleSingle( FALSE );

    if ( m_pComboRollSearchOver->IsVisible() )
    {
        m_pViewSearchWordButton->SetVisibleSingle( FALSE );
        m_pUnViewSearchWordButton->SetVisibleSingle( FALSE );
    }
}

void CPointShopWindow::ResetViewSearchWordList()
{
    if ( m_bViewSearchWord )
    {
        m_pViewSearchWordButton->SetVisibleSingle( TRUE );
        m_pUnViewSearchWordButton->SetVisibleSingle( FALSE );
    }
    else
    {
        m_pViewSearchWordButton->SetVisibleSingle( FALSE );
        m_pUnViewSearchWordButton->SetVisibleSingle( TRUE );
    }
}

void CPointShopWindow::ViewNotifyFullCart()
{
    m_fNotifyFullCartTime = fNOTIFY_FULL_CART_TIME;
    m_fNotifyFullCartFadeTime = fNOTIFY_FULL_CART_FADE_TIME;

    m_pNotifyFullCartBack->SetVisibleSingle( TRUE );
    m_pNotifyFullCartText->SetVisibleSingle( TRUE );

    m_pNotifyFullCartBack->SetDiffuseAlpha( 255 );
    m_pNotifyFullCartText->SetDiffuseAlpha( 255 );

    m_pNotifyFullCartText->ClearText();
    m_pNotifyFullCartText->AddText( ID2GAMEINTEXT("POINT_SHOP_NOTIFY_FULL_CART_TEXT0"), NS_UITEXTCOLOR::YELLOW );
    m_pNotifyFullCartText->AddText( ID2GAMEINTEXT("POINT_SHOP_NOTIFY_FULL_CART_TEXT1"), NS_UITEXTCOLOR::WHITE );
}

void CPointShopWindow::UnViewNotifyFullCart()
{
    m_fNotifyFullCartTime = fNOTIFY_FULL_CART_TIME;
    m_fNotifyFullCartFadeTime = fNOTIFY_FULL_CART_FADE_TIME;

    m_pNotifyFullCartBack->SetVisibleSingle( FALSE );
    m_pNotifyFullCartText->SetVisibleSingle( FALSE );
}

void CPointShopWindow::ReloadCategoryTypeForUser()
{
    // Note : 카테고리 옵션 파일 리로드.
    m_pGaeaClient->GetPointShopClient()->Load();
    ComboInit();
    UnViewSearchWordList();
    m_pCategoryAll->UnCheck();
    SetAllCategory( false );
    ResetAutoCategory();
    UnCheckIncludeText();

    ReLoadAll();
}

__int64 CPointShopWindow::GetPriceInCart()
{
    return m_nTotalPrice;
}

void CPointShopWindow::ResetCartItem( SNATIVEID sItemID )
{
    CART_ITER itr;

    itr = m_listCart.begin();
    for ( int nIndex = 0; itr != m_listCart.end(); itr++, nIndex++ )
    {
        pointshop::SellItem& CartItem = (*itr);

        if ( sItemID == CartItem.ItemId() )
        {
            m_pCartItemSlot->ResetItemImage( nIndex );

            m_listCart.erase( itr );
            break;
        }
    }

    ReLoadCartList();
}

void CPointShopWindow::InitCartList()
{
    m_listCart.clear();

    std::vector<pointshop::SellItemClient> vecSellItemList;
    std::string SearchName("");
    std::vector<EMITEM_TYPE> TypeList;
    std::vector<pointshop::SEARCH_ORDER> OrderList;

    m_pGaeaClient->GetPointShopClient()->GetList( vecSellItemList, SearchName, TypeList, OrderList );

    for ( int i = 0; i < RANPARAM::MAX_POINT_SHOP_WISH_LIST; i++ )
    {
        DWORD sWishListId = RANPARAM::dwPointShopWishList[ i ];
        if ( sWishListId != NATIVEID_NULL().dwID )
        {
            int nSellItemListSize = static_cast<int>( vecSellItemList.size() );
            for ( int j = 0; j < nSellItemListSize; j++ )
            {
                pointshop::SellItemClient& SellItem = vecSellItemList[ j ];
                if ( SellItem.ItemId().dwID == sWishListId )
                {
                    int nCartListSize = static_cast<int>( m_listCart.size() );

                    if ( nCartListSize >= NUM_CART_LIST_ITEM )
                    {
                        ViewNotifyFullCart();
                        return;
                    }

                    pointshop::SellItem SelectItem( SellItem.ItemId(), SellItem.Price() );

                    const SITEM* pItem = GLogicData::GetInstance().GetItem( SelectItem.ItemId() );
                    if ( !pItem ) return;

                    m_listCart.push_back( SelectItem );
                    break;
                }
            }
        }
    }

    ReLoadCartList();
}

void CPointShopWindow::SearchItemByName( std::string& strItemName )
{
    ReLoadCategory();

    std::string strSearchName = strItemName;

    int nLimitInput = m_pItemNameEdit->GetLimitInput( POINT_SHOP_ITEM_SEARCH_EDIT );
	if ( strSearchName.length() > (std::string::size_type)nLimitInput )
    {
#ifndef NDEBUG
        GASSERT( 0 && "void CPointShopWindow::SearchItemByName( std::string& strItemName ) - strItemName문자열 길이가 제한된 길이를 초과하였습니다." );
#endif
        strSearchName = strItemName.substr( 0, nLimitInput );
    }
    m_pItemNameEdit->SetEditString( POINT_SHOP_ITEM_SEARCH_EDIT, strSearchName.c_str() );
    m_pItemNameEdit->GetEditBox( POINT_SHOP_ITEM_SEARCH_EDIT )->EndEdit();
    m_pComboRollSearchOver->SetVisibleSingle( FALSE );
    ResetViewSearchWordList();

    SearchItemList();
}

std::string CPointShopWindow::GetSearchEditValue()
{
	return m_pItemNameEdit->GetEditString( POINT_SHOP_ITEM_SEARCH_EDIT ).GetString();
}

void CPointShopWindow::SetForceCloseWindow( bool _bForceClose )
{
    m_bForceClose = _bForceClose;
}

bool CPointShopWindow::IsForceCloseWindow()
{
    return m_bForceClose;
}

MyPointShopWindow::MyPointShopWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CPointShopWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyPointShopWindow::CreateUIWindowAndRegisterOwnership()
{
    CPointShopWindow::Create( POINT_SHOP_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CPointShopWindow::CreateBaseWindowLightGray( "POINT_SHOP_WINDOW", (char*)ID2GAMEINTEXT("POINT_SHOP_TITLE_TEXT") );
    CPointShopWindow::CreateSubControl();
    CPointShopWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
    CPointShopWindow::m_pInterface->UiRegisterControl( this, true );
    CPointShopWindow::m_pInterface->UiShowGroupFocus( POINT_SHOP_WINDOW );
}

void MyPointShopWindow::RefreshWindow()
{
    CPointShopWindow::RefreshWindow();
}

void MyPointShopWindow::ReLoadItemList()
{
    CPointShopWindow::ReLoadSellItemList();
    CPointShopWindow::ReLoadSellItemNodes();
}

void MyPointShopWindow::ReLoadAll()
{
    CPointShopWindow::ReLoadAll();
}

void MyPointShopWindow::SetPointText( __int64 nPoint )
{
    CPointShopWindow::SetPointText( nPoint );
}

void MyPointShopWindow::SetAllCategory( bool bCheck )
{
    CPointShopWindow::SetAllCategory( bCheck );
}

void MyPointShopWindow::UnCheckIncludeTextSearch()
{
    CPointShopWindow::UnCheckIncludeText();
}

__int64 MyPointShopWindow::GetPriceInCart()
{
    return CPointShopWindow::GetPriceInCart();
}

void MyPointShopWindow::ResetCartItem( SNATIVEID sItemID )
{
    CPointShopWindow::ResetCartItem( sItemID );
}

void MyPointShopWindow::SearchItem( std::string& strSearchName )
{
    SearchItemByName( strSearchName );
}

std::string MyPointShopWindow::GetSearchName()
{
	return CPointShopWindow::GetSearchEditValue();
}

void MyPointShopWindow::SetForceCloseWindow( bool _bForceClose )
{
    CPointShopWindow::SetForceCloseWindow( _bForceClose );
}

bool MyPointShopWindow::IsForceCloseWindow()
{
    return CPointShopWindow::IsForceCloseWindow();
}