#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../Util/CheckBox.h"
#include "../util/SmartComboBoxEx.h"
#include "../Util/RnButton.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"

#include "../../../RanLogicClient/PrivateMarketSearchBuy/PrivateMarketClient.h"
#include "../Util/MessageBox.h"

#include "MarketCalculationRow.h"
#include "Pagenavigation.h"
#include "MarketSearchRow.h"
#include "MarketSearchWhishPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const char* ID2GAMEWORD_TITLE = "찜리스트";
	const char* ID2GAMEWORD_ICON = "아이콘";
	const char* ID2GAMEWORD_SELLER_INFO = "판매자정보";
	const char* ID2GAMEWORD_ITEMNAME = "아이템명";
	const char* ID2GAMEWORD_COST = "개당구입금액";
	const char* ID2GAMEWORD_OPTION_TITLE = "계산대";

	const char* ID2GAMEWORD_ITEM_COST = "총 구입 금액";
	const char* ID2GAMEWORD_HAVE_MONEY = "보유 금액";
	const char* ID2GAMEWORD_AFTER_MONEY = "구매 후 금액";


	const char* ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO = "POSTBOX_PAGE_SEND_COMBOBOX_DESIGN_TEXINFO";
	const char* ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS = "MARKET_SEARCH_PAGE_BASIC_WEAR_CATEGORY_COMBO_BUTTON";
	const char* ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_NORMAL";
	const char* ID2GAMEWORD_COMBOBOX_MOUSE_OVER = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_OVER";
	const char* ID2GAMEWORD_COMBOBOX_MOUSE_CLICK = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_DOWN";

	const char* ID2GAMEWORD_ADD_WHISH_BUTTON = "체크항목 모두구입";
	const char* ID2GAMEWORD_BUY_BUTTON = "정보출력";

	const char* ID2GAMEWORD_REASER_MSG = "체크된 아이템을 모두 삭제하시겠습니까?";
	const char* ID2GAMEWORD_BUY_MSG = "계산대 내용으로 구입하시겠습니까?";


	const char* szButtonNormal = "RNBUTTON_DEFAULT_TEXTURE";
	const char* szButtonOver = "RNBUTTON_OVER_TEXTURE";
	const char* szButtonDown = "RNBUTTON_CLICK_TEXTURE";
}

MarketSearchWhishPage::MarketSearchWhishPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx( pInterface, pEngineDevice )
, m_pGaeaClient( pGaeaClient )
, m_pInterface( pInterface )
, m_pTitleLineBox( NULL )
, m_pMiddleLineBox( NULL )
, m_pTitleBox( NULL )
, m_pHaveMoneyTextBox( NULL )
, m_pAllCheckBox ( NULL )
{
	ID2GAMEWORD_TITLE = ID2GAMEWORD("MARKET_SEARCH_BUY_WHISH_LIST", 0);//"찜리스트";
	ID2GAMEWORD_ICON = ID2GAMEWORD("MARKET_SEARCH_BUY_ITEM_IMAGE", 0);//"아이콘";
	ID2GAMEWORD_SELLER_INFO = ID2GAMEWORD("MARKET_SEARCH_BUY_SELLER_INFO", 0);//"판매자정보";
	ID2GAMEWORD_ITEMNAME = ID2GAMEWORD("MARKET_SEARCH_BUY_ITEM_NAME", 0);//"아이템명";
	ID2GAMEWORD_COST = ID2GAMEWORD("MARKET_SEARCH_BUY_INDIVIDUALLY_COST", 0);//"개당구입금액";
	ID2GAMEWORD_OPTION_TITLE = ID2GAMEWORD("MARKET_SEARCH_BUY_CALCULATION", 0);//"계산대";

	ID2GAMEWORD_ITEM_COST = ID2GAMEWORD("MARKET_SEARCH_BUY_TOTAL_COST", 0);//"총 구입 금액";
	ID2GAMEWORD_HAVE_MONEY = ID2GAMEWORD("MARKET_SEARCH_BUY_HAVE_MONEY", 0);//"보유 금액";
	ID2GAMEWORD_AFTER_MONEY = ID2GAMEWORD("MARKET_SEARCH_BUY_BUY_AFTER_MONEY", 0);//"구매 후 금액";

	ID2GAMEWORD_ADD_WHISH_BUTTON = ID2GAMEWORD("MARKET_SEARCH_BUY_BUY_CHECK", 0);//"체크항목 모두구입";
	ID2GAMEWORD_BUY_BUTTON = ID2GAMEWORD("MARKET_SEARCH_BUY_SHOW_INFO", 0);//"정보출력/비교";

	ID2GAMEWORD_REASER_MSG = ID2GAMEINTEXT("MAKET_SEARCH_BUY_REASER_MSG", 0);//"체크된 아이템을 모두 삭제하시겠습니까?";
	ID2GAMEWORD_BUY_MSG = ID2GAMEINTEXT("MAKET_SEARCH_BUY_MSG", 0);//"계산대 내용으로 구입하시겠습니까?";
}

MarketSearchWhishPage::~MarketSearchWhishPage()
{
}

void MarketSearchWhishPage::CreateSubControl()
{
	{// 검색결과 창 배경& 라인박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_WHISH_PAGE_BG" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );
	}
	{// 검색결과창 타이틀 박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_WHISH_PAGE_TITLE" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );
		// [검색결과] 텍스트 박스
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		m_pTitleBox = new CBasicTextBox(m_pEngineDevice);
		m_pTitleBox->CreateSub( this, "MARKET_WHISH_PAGE_TITLE" );
		m_pTitleBox->SetFont( pFont );
		m_pTitleBox->SetText( ID2GAMEWORD_TITLE,NS_UITEXTCOLOR::WHITE );
		m_pTitleBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pTitleBox );
	}
	{// 검색결과 창 배경& 라인박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_SEARCH_PAGE_OBJECT_BAR" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );
	}
	{	//검색결과창 내 [아이콘 아이템명 ▼  판매정보   판매가격] 제목 상자
		CUIControl* pObjectBar = new CUIControl(m_pEngineDevice);
		pObjectBar->CreateSub ( this, "MARKET_SEARCH_PAGE_OBJECT_BAR",UI_FLAG_XSIZE | UI_FLAG_BOTTOM );
		pObjectBar->SetUseRender ( TRUE );
		pObjectBar->SetDiffuse ( D3DCOLOR_ARGB(130,74,69,42) );
		RegisterControl ( pObjectBar );

		// [아이콘] 텍스트 박스
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CBasicTextBox* pIconBox = new CBasicTextBox(m_pEngineDevice);
		pIconBox->CreateSub( this, "MARKET_SEARCH_PAGE_OBJECT_ITEM" );
		pIconBox->SetFont( pFont );
		pIconBox->SetText( ID2GAMEWORD_ICON,NS_UITEXTCOLOR::WHITE );
		pIconBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pIconBox );

		// [아이템명] 텍스트 박스
		CBasicTextBox* pItemNameBox  = new CBasicTextBox(m_pEngineDevice);
		pItemNameBox->CreateSub( this, "MARKET_SEARCH_PAGE_OBJECT_ITEMNAME" );
		pItemNameBox->SetFont( pFont );
		pItemNameBox->SetText( ID2GAMEWORD_ITEMNAME,NS_UITEXTCOLOR::WHITE );
		pItemNameBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pItemNameBox );

		// [판매정보] 텍스트 박스
		CBasicTextBox* pSellerInfoBox = new CBasicTextBox(m_pEngineDevice);
		pSellerInfoBox->CreateSub( this, "MARKET_SEARCH_PAGE_OBJECT_SELLERINFO" );
		pSellerInfoBox->SetFont( pFont );
		pSellerInfoBox->SetText( ID2GAMEWORD_SELLER_INFO,NS_UITEXTCOLOR::WHITE );
		pSellerInfoBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pSellerInfoBox );

		// [개당구입금액] 텍스트 박스
		CBasicTextBox* pCostBox = new CBasicTextBox(m_pEngineDevice);
		pCostBox->CreateSub( this, "MARKET_SEARCH_PAGE_OBJECT_COST" );
		pCostBox->SetFont( pFont );
		pCostBox->SetText( ID2GAMEWORD_COST,NS_UITEXTCOLOR::WHITE );
		pCostBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pCostBox );

		//[기간제]체크박스
		CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
		pCheckBox->CreateSub( this, "MARKET_SEARCH_PAGE_ALL_CHECK_BOX", UI_FLAG_DEFAULT,ID_ALL_CHECKBOX );
		pCheckBox->CreateSubControl( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
		RegisterControl( pCheckBox );
		m_pAllCheckBox = pCheckBox;
	}
	{// 검색된 아이템 행
		for (int i = 0; i < 8; i ++)
		{
			MarketSearchRow* pSearchRow = new MarketSearchRow( m_pGaeaClient, m_pInterface, m_pEngineDevice );
			pSearchRow->CreateSub(this,"MARKET_SEARCH_PAGE_SEARCH_ROW_BG",UI_FLAG_DEFAULT, ID_ROW1 + i );
			pSearchRow->CreateSubControl();
			//pSearchRow->SetVisibleSingle( FALSE );
			RegisterControl( pSearchRow );
			m_pSearchRow.push_back(pSearchRow);

			if ( i > 0)
			{
				UIRECT rcOldPos = m_pSearchRow[i - 1]->GetLocalPos();
				UIRECT rcNewPos = m_pSearchRow[i]->GetLocalPos();
				m_pSearchRow[i]->SetLocalPos( D3DXVECTOR2( rcNewPos.left, rcOldPos.bottom - 1));
			}
			pSearchRow->SetVisibleSingle( FALSE );

		}
	}

	{
		//찜 페이지  버튼이랑 구분 선
		CBasicLineBoxSmart* pLineMarketSearchBGLine = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineMarketSearchBGLine->CreateSub( this, "MARKET_SEARCH_PAGE_BG_LINE" );
		pLineMarketSearchBGLine->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineMarketSearchBGLine );
	}
	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//휴지통 버튼
	{
		RnButton* pButton = NULL;
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = szButtonNormal;
		arg.mouseOverTextureKey = szButtonOver;
		arg.mouseClickTextureKey = szButtonDown;
		//////////////////////////////////////////////////////////////////////////

		arg.pFont = pFont8;
		arg.dwColor = NS_UITEXTCOLOR::WHITE;
		arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
		//arg.text = sc::string::format( "%s",ID2GAMEWORD_ADD_WHISH_BUTTON);

		pButton = new RnButton( m_pEngineDevice );
		pButton->CreateSub( this, "MARKET_WHISH_PAGE_TRASH_BUTTON", UI_FLAG_DEFAULT, ID_DEL_CHECK_ROW );
		pButton->CreateSubControl( arg );
		RegisterControl( pButton );

		CUIControl* pImage = new CUIControl( m_pEngineDevice );
		pImage->CreateSub( this, "MARKET_WHISH_PAGE_TRASH_IMAGE" );
		RegisterControl( pImage );
	}
	//체크항목모두구입
	{
		RnButton* pLineBoxComboButton = NULL;
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = szButtonNormal;
		arg.mouseOverTextureKey = szButtonOver;
		arg.mouseClickTextureKey = szButtonDown;
		//////////////////////////////////////////////////////////////////////////

		arg.pFont = pFont8;
		arg.dwColor = NS_UITEXTCOLOR::WHITE;
		arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
		arg.text = sc::string::format( "%s",ID2GAMEWORD_ADD_WHISH_BUTTON);

		pLineBoxComboButton = new RnButton(m_pEngineDevice);
		pLineBoxComboButton->CreateSub(this, "MARKET_WHISH_PAGE_INFO_BUTTON", UI_FLAG_RIGHT, ID_CHECK_BUY );
		pLineBoxComboButton->CreateSubControl(arg);
		RegisterControl(pLineBoxComboButton);
	}

	//정보 출력
	{
		RnButton* pLineBoxComboButton = NULL;
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = szButtonNormal;
		arg.mouseOverTextureKey = szButtonOver;
		arg.mouseClickTextureKey = szButtonDown;
		//////////////////////////////////////////////////////////////////////////

		arg.pFont = pFont8;
		arg.dwColor = NS_UITEXTCOLOR::WHITE;
		arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
		arg.text = sc::string::format( "%s",ID2GAMEWORD_BUY_BUTTON);

		pLineBoxComboButton = new RnButton(m_pEngineDevice);
		pLineBoxComboButton->CreateSub(this, "MARKET_WHISH_PAGE_BUY_BUTTON", UI_FLAG_RIGHT,ID_ITEM_COMPARE_BUTTON );
		pLineBoxComboButton->CreateSubControl(arg);
		RegisterControl(pLineBoxComboButton);
	}

	{
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		Pagenavigation* pPagenavigation = new Pagenavigation( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pPagenavigation->CreateSub(this,"PAGE_NAVIGATION_PAGE_POS", UI_FLAG_DEFAULT, ID_PAGE_NAVIGATION);
		pPagenavigation->SetFont( pFont );
		pPagenavigation->CreateSubControl();
		RegisterControl(pPagenavigation);
		m_pPagenavigation = pPagenavigation;
	}

	{// 계산대 창 배경& 라인박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_WHISH_CALCULATION_BG" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );
	}
	{// 계산대 타이틀 박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_WHISH_PAGE_CALCULATION_TITLE" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );

		// [계산대] 텍스트 박스
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTitleBox = new CBasicTextBox(m_pEngineDevice);
		pTitleBox->CreateSub( this, "MARKET_WHISH_PAGE_CALCULATION_TITLE" );
		pTitleBox->SetFont( pFont );
		pTitleBox->SetText( ID2GAMEWORD_OPTION_TITLE,NS_UITEXTCOLOR::WHITE );
		pTitleBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pTitleBox );
	}
	{
		// 계산대 배경
		CBasicLineBoxSmart* pCalculationBGLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pCalculationBGLineBox->CreateSub( this, "MARKET_WHISH_CALCULATION_LIST_BG" );
		pCalculationBGLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pCalculationBGLineBox );

		MarketCalculationRow* pMarketCalculation = new MarketCalculationRow( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pMarketCalculation->CreateSub( this, "MARKET_WHISH_CALCULATION_LIST_BG" );
		pMarketCalculation->CreateSubControl();
		RegisterControl( pMarketCalculation );
		m_pMarketCalculation = pMarketCalculation;
	}

	// [총구입 금액] 텍스트 박스
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CBasicTextBox* pIconBox = new CBasicTextBox(m_pEngineDevice);
	pIconBox->CreateSub( this, "ID2GAMEWORD_ITEM_COST" );
	pIconBox->SetFont( pFont );
	pIconBox->SetText( ID2GAMEWORD_ITEM_COST,NS_UITEXTCOLOR::WHITE );
	pIconBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
	RegisterControl( pIconBox );

	//총구입 금액 표시
	CreateLineBox( "MARKET_WHISH_PAGE_ITEM_COST_TEXT_BG", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CD3DFontPar* pFontSize8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );
	CBasicTextBoxEx* pItemCostTextBox  = new CBasicTextBoxEx(m_pEngineDevice);
	pItemCostTextBox->CreateSub( this, "MARKET_WHISH_PAGE_ITEM_COST_TEXT" );
	pItemCostTextBox->SetFont( pFontSize8 );
	pItemCostTextBox->SetText( "999,999,999" );
	pItemCostTextBox->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_MONEY_BUTTON");
	pItemCostTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
	RegisterControl( pItemCostTextBox );
	m_pItemCostTextBox = pItemCostTextBox;

	// [보유금액] 텍스트 박스
	CBasicTextBox* pItemNameBox  = new CBasicTextBox(m_pEngineDevice);
	pItemNameBox->CreateSub( this, "ID2GAMEWORD_HAVE_MONEY" );
	pItemNameBox->SetFont( pFont );
	pItemNameBox->SetText( ID2GAMEWORD_HAVE_MONEY,NS_UITEXTCOLOR::WHITE );
	pItemNameBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
	RegisterControl( pItemNameBox );

	//보유금액 표시
	CreateLineBox( "MARKET_WHISH_PAGE_HAVE_MONEY_TEXT_BG", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CBasicTextBoxEx* pHaveMoneyTextBox  = new CBasicTextBoxEx(m_pEngineDevice);
	pHaveMoneyTextBox->CreateSub( this, "MARKET_WHISH_PAGE_HAVE_MONEY_TEXT" );
	pHaveMoneyTextBox->SetFont( pFontSize8 );
	pHaveMoneyTextBox->SetText( "999,999,999" );
	pHaveMoneyTextBox->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_MONEY_BUTTON");
	pHaveMoneyTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
	RegisterControl( pHaveMoneyTextBox );
	m_pHaveMoneyTextBox = pHaveMoneyTextBox;

	// [구매후 금액] 텍스트 박스
	CBasicTextBox* pSellerInfoBox = new CBasicTextBoxEx(m_pEngineDevice);
	pSellerInfoBox->CreateSub( this, "ID2GAMEWORD_AFTER_MONEY" );
	pSellerInfoBox->SetFont( pFont );
	pSellerInfoBox->SetText( ID2GAMEWORD_AFTER_MONEY,NS_UITEXTCOLOR::WHITE );
	pSellerInfoBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
	RegisterControl( pSellerInfoBox );

	//구매후 금액 표시
	CreateLineBox( "MARKET_WHISH_PAGE_AFTER_MONEY_TEXT_BG", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CBasicTextBoxEx* pAfter_MoneyTextBox  = new CBasicTextBoxEx(m_pEngineDevice);
	pAfter_MoneyTextBox->CreateSub( this, "MARKET_WHISH_PAGE_AFTER_MONEY_TEXT" );
	pAfter_MoneyTextBox->SetFont( pFontSize8 );
	pAfter_MoneyTextBox->SetText( "999,999,999" );
	pAfter_MoneyTextBox->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_MONEY_BUTTON");
	pAfter_MoneyTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
	RegisterControl( pAfter_MoneyTextBox );
	m_pAfter_MoneyTextBox = pAfter_MoneyTextBox;

    {
        // [총구입 금액] 텍스트 박스
        CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
        CBasicTextBox* pIconBox = new CBasicTextBox(m_pEngineDevice);
        pIconBox->CreateSub( this, "ID2GAMEWORD_ITEM_COST_PR" );
        pIconBox->SetFont( pFont );
        pIconBox->SetText( ID2GAMEWORD_ITEM_COST,NS_UITEXTCOLOR::WHITE );
        pIconBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pIconBox );

        //총구입 금액 표시
        CreateLineBox( "MARKET_WHISH_PAGE_ITEM_COST_TEXT_BG_PR", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
        CD3DFontPar* pFontSize8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );
        CBasicTextBoxEx* pItemCostTextBox  = new CBasicTextBoxEx(m_pEngineDevice);
        pItemCostTextBox->CreateSub( this, "MARKET_WHISH_PAGE_ITEM_COST_TEXT_PR" );
        pItemCostTextBox->SetFont( pFontSize8 );
        pItemCostTextBox->SetText( "999,999,999" );
        pItemCostTextBox->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_MONEY_BUTTON");
        pItemCostTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
        RegisterControl( pItemCostTextBox );
        m_pItemCostTextBoxRP = pItemCostTextBox;

        // [보유금액] 텍스트 박스
        CBasicTextBox* pItemNameBox  = new CBasicTextBox(m_pEngineDevice);
        pItemNameBox->CreateSub( this, "ID2GAMEWORD_HAVE_MONEY_PR" );
        pItemNameBox->SetFont( pFont );
        pItemNameBox->SetText( ID2GAMEWORD_HAVE_MONEY,NS_UITEXTCOLOR::WHITE );
        pItemNameBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pItemNameBox );

        //보유금액 표시
        CreateLineBox( "MARKET_WHISH_PAGE_HAVE_MONEY_TEXT_BG_PR", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
        CBasicTextBoxEx* pHaveMoneyTextBox  = new CBasicTextBoxEx(m_pEngineDevice);
        pHaveMoneyTextBox->CreateSub( this, "MARKET_WHISH_PAGE_HAVE_MONEY_TEXT_PR" );
        pHaveMoneyTextBox->SetFont( pFontSize8 );
        pHaveMoneyTextBox->SetText( "999,999,999" );
        pHaveMoneyTextBox->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_MONEY_BUTTON");
        pHaveMoneyTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
        RegisterControl( pHaveMoneyTextBox );
        m_pHaveMoneyTextBoxRP = pHaveMoneyTextBox;

        // [구매후 금액] 텍스트 박스
        CBasicTextBox* pSellerInfoBox = new CBasicTextBoxEx(m_pEngineDevice);
        pSellerInfoBox->CreateSub( this, "ID2GAMEWORD_AFTER_MONEY_PR" );
        pSellerInfoBox->SetFont( pFont );
        pSellerInfoBox->SetText( ID2GAMEWORD_AFTER_MONEY,NS_UITEXTCOLOR::WHITE );
        pSellerInfoBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pSellerInfoBox );

        //구매후 금액 표시
        CreateLineBox( "MARKET_WHISH_PAGE_AFTER_MONEY_TEXT_BG_PR", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
        CBasicTextBoxEx* pAfter_MoneyTextBox  = new CBasicTextBoxEx(m_pEngineDevice);
        pAfter_MoneyTextBox->CreateSub( this, "MARKET_WHISH_PAGE_AFTER_MONEY_TEXT_PR" );
        pAfter_MoneyTextBox->SetFont( pFontSize8 );
        pAfter_MoneyTextBox->SetText( "999,999,999" );
        pAfter_MoneyTextBox->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_MONEY_BUTTON");
        pAfter_MoneyTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
        RegisterControl( pAfter_MoneyTextBox );
        m_pAfter_MoneyTextBoxRP = pAfter_MoneyTextBox;
    }

	CMessageBox* pMessageBox = new CMessageBox( m_pEngineDevice );
	pMessageBox->CreateSub( this, "UTILE_MESSAGEBOX_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_MESSAGEBOX );
	pMessageBox->CreateSubControl();
	RegisterControl( pMessageBox );
	m_pMessageBox = pMessageBox;

}

void MarketSearchWhishPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	//보유금액
	{
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( !pChar )
			return;

		CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pChar->GetInvenMoney(), 3, "," );
		if ( m_pHaveMoneyTextBox )
		{
			m_pHaveMoneyTextBox->SetText( strText );
			m_pHaveMoneyTextBox->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_MONEY_BUTTON");
		}

        CString strTextRP = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pChar->PointShopPoint(), 3, "," );
        if ( m_pHaveMoneyTextBoxRP )
        {
            m_pHaveMoneyTextBoxRP->SetText( strTextRP );
            m_pHaveMoneyTextBoxRP->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_RANPOINT_BUTTON");
        }
	}

	//for (unsigned i = 0; i < m_pSearchRow.size(); i++)
	//{
	//	m_pSearchRow[i]->SetVisibleSingle(FALSE);
	//}

	
	//m_vecDataPage.clear();
	//m_vecDataPage = m_pGaeaClient->GetPrivateMarketClient()->GetWhishPageData( m_pPagenavigation->GetPageIndex() );

	//for (unsigned int i = 0; i < m_vecDataPage.size(); i ++)
	//{
	//	SetMapName(i,m_pGaeaClient->GetMapName(m_vecDataPage[i]->sSellerMapID));
	//	SetsSaleItem(i,m_vecDataPage[i]);
	//	SetUUID(i,m_vecDataPage[i]->sItemGUID);
	//	SetItemImage(i,m_vecDataPage[i]->sItemcustom, static_cast<WORD>(m_vecDataPage[i]->dwItemTurnNum) );
	//	SetItemName(i,m_vecDataPage[i]->sItemcustom);
	//	IsUseItem(i,m_vecDataPage[i]->sItemcustom);
	//	SetItemPrice(i,m_vecDataPage[i]->llItemPrice);
	//	SetTitemSellerName(i,m_vecDataPage[i]->szSellerName);
	//	m_pSearchRow[i]->SetVisibleSingle(TRUE);
	//}


	//체크항목 계산대에 표시
	m_pMarketCalculation->ClearMarketCalculation();
	for (unsigned i = 0; i < m_pSearchRow.size(); i++)
	{
		if ( m_pSearchRow[i]->IsChecked() )
		{
			if ( i >= m_vecDataPage.size() )
				return;

			SITEMCUSTOM sItemCustom = m_vecDataPage[i]->sItemcustom;
			const SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
			if ( !pItemData )
				return;

			m_pMarketCalculation->SetItemName(sItemCustom.GETNAME().c_str(), ITEM_INFOR_COLOR::dwItemRank[ pItemData->sBasicOp.emLevel ]);
			m_pMarketCalculation->SetItemCount(m_pSearchRow[i]->GetItemCount());
            m_pMarketCalculation->SetItemPrice(m_vecDataPage[i]->llItemPrice * m_pSearchRow[i]->GetItemCount(), static_cast<private_market::eSellType>(m_vecDataPage[i]->nSellType) );
		}
	}

	//체크항목 금액 계산
	m_nCalculationItemCost = 0;
    m_nCalculationItemCostRP = 0;
	for (unsigned i = 0; i < m_pSearchRow.size(); i++)
	{
		if ( m_pSearchRow[i]->IsChecked() )
		{
			if ( i >= m_vecDataPage.size() )
				return;

            if( m_vecDataPage[i]->nSellType == private_market::eSellType_ConsignmentSale_RanPoint)
                m_nCalculationItemCostRP += m_vecDataPage[i]->llItemPrice * m_pSearchRow[i]->GetItemCount();
            else
                m_nCalculationItemCost += m_vecDataPage[i]->llItemPrice * m_pSearchRow[i]->GetItemCount();
			
		}
	}

    {
        CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( m_nCalculationItemCost, 3, "," );
        m_pItemCostTextBox->SetText( strText );
        m_pItemCostTextBox->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_MONEY_BUTTON");
    }
    {
        CString strTextRP = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( m_nCalculationItemCostRP, 3, "," );
        m_pItemCostTextBoxRP->SetText( strTextRP );
        m_pItemCostTextBoxRP->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_RANPOINT_BUTTON");
    }


	//차액
	{
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( !pChar )
			return;

		CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pChar->GetInvenMoney()- m_nCalculationItemCost, 3, "," );
        CString strTextRP = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pChar->PointShopPoint() - m_nCalculationItemCostRP, 3, "," );
		if ( m_pAfter_MoneyTextBox )
		{
			m_pAfter_MoneyTextBox->SetText( strText );
			m_pAfter_MoneyTextBox->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_MONEY_BUTTON");
		}
        if ( m_pAfter_MoneyTextBoxRP )
        {
            m_pAfter_MoneyTextBoxRP->SetText( strTextRP );
            m_pAfter_MoneyTextBoxRP->SetTextImage( 0, "MARKET_WHISH_PAGE_HAVE_RANPOINT_BUTTON");
        }
	}


	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	m_pPagenavigation->PagenavigationUpdate( m_pGaeaClient->GetPrivateMarketClient()->GetWhishPageCount() );
}

void MarketSearchWhishPage::LoadItemInfo()
{

}

void MarketSearchWhishPage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ID_ITEM_COMPARE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
                if ( m_pInterface->GetMarketWishItemTooltipGroup()->IsShowItemToolTipMulti() ) //기존에 정보출력툴팁창이 켜져있다면 전부 끈다.
                {
                    m_pInterface->GetMarketWishItemTooltipGroup()->CloseItemToolTipMultiAll();
                }
                else    //정보출력창 띄움
                {
                    int count = 0;
                    for (unsigned i = 0; i < m_pSearchRow.size(); i++)
                    {
                        if ( m_pSearchRow[i]->IsChecked() )
                        {
                            m_pInterface->GetMarketWishItemTooltipGroup()->ShowItemToolTipMulti( m_pSearchRow[i]->GetItemData(), 100 + (count*50), 100 + (count*50) );
                            count++;
                        }
                    }
                }
			}
		}
		break;
	case ID_DEL_CHECK_ROW:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				if ( m_pMessageBox->IsVisible() )
				{
					m_pMessageBox->CloseMessageBox();
					break;
				}
				int nCheckCount = 0;
				BOOST_FOREACH (MarketSearchRow* vecSearchRow, m_pSearchRow)
				{
					if ( vecSearchRow->IsChecked() )
						nCheckCount++;
				}
				if (!nCheckCount)
					break;
				m_pMessageBox->CloseMessageBox();
				m_pMessageBox->OpenMessageBox( CMessageBox::TITLE_QUESTION, ID2GAMEWORD_REASER_MSG, CMessageBox::TYPE_YESNO, ID_DEL_CHECK_ROW );
			}
		}
		break;
	case ID_MESSAGEBOX:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				if (dwMsg & UIMSG_MESSAGEBOX_POSITIVE)
				{
					int nIdentifier = m_pMessageBox->GetIdentifier();

					switch (nIdentifier)
					{
					case ID_DEL_CHECK_ROW:
						{
							DelCheckRow();
							m_pMessageBox->CloseMessageBox();
						}
						break;

					case ID_CHECK_BUY:
						{
							private_market::sPurchaseQueue sPurchaseQueue;
							std::vector< private_market::sPurchaseQueue > vecPurchase;

							BOOST_FOREACH (MarketSearchRow* vecSearchRow, m_pSearchRow)
							{
								if ( vecSearchRow->IsChecked() )
								{
									vecSearchRow->SetCheckOff();
									sPurchaseQueue.sItemGUID = vecSearchRow->GetUUID();
									sPurchaseQueue.dwItemTurnNum = vecSearchRow->GetItemCount();
									sPurchaseQueue.llPrice = vecSearchRow->GetPrice();
									vecPurchase.push_back(sPurchaseQueue);
								}

							}
							m_pGaeaClient->ReqPrivateMarketItemBuy(vecPurchase);
						}
						break;
					}
				}
			}

		}
		break;
	case ID_PAGE_NAVIGATION:
		{
			//페이지 이동시 리셋될 내용 들
			if ( m_pPagenavigation->IsClickEvent())
			{
				m_pAllCheckBox->SetCheckOff();
				BOOST_FOREACH (MarketSearchRow* vecSearchRow, m_pSearchRow)
				{
					vecSearchRow->SetCheckOff();
				}

				Refresh();
			}
		}
		break;
	case  ID_ALL_CHECKBOX:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				if ( m_pAllCheckBox->IsChecked() )
				{
					BOOST_FOREACH (MarketSearchRow* vecSearchRow, m_pSearchRow)
					{
						if ( vecSearchRow->IsVisible() )
							vecSearchRow->SetCheckOn();
					}
				}
				else
				{
					BOOST_FOREACH (MarketSearchRow* vecSearchRow, m_pSearchRow)
					{
						if ( vecSearchRow->IsVisible() )
							vecSearchRow->SetCheckOff();
					}
				}
			}
		}
		break;
	case ID_CHECK_BUY:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				if ( m_pMessageBox->IsVisible() )
				{
					m_pMessageBox->CloseMessageBox();
					break;
				}

				int nCheckCount = 0;
				BOOST_FOREACH (MarketSearchRow* vecSearchRow, m_pSearchRow)
				{
					if ( vecSearchRow->IsChecked() )
						nCheckCount++;
				}
				if (!nCheckCount)
					break;
				m_pMessageBox->CloseMessageBox();
				m_pMessageBox->OpenMessageBox( CMessageBox::TITLE_QUESTION, ID2GAMEWORD_BUY_MSG, CMessageBox::TYPE_YESNO, ID_CHECK_BUY );
			}
		}
		break;
	default:
		{
			if ( CHECK_RB_UP_LIKE ( dwMsg ) || CHECK_RB_DOWN_LIKE ( dwMsg ) )
			{
			}

			if ( CHECK_LB_UP_LIKE ( dwMsg ) || CHECK_LB_DOWN_LIKE ( dwMsg ) )
			{
			}
		}
	}

}

void MarketSearchWhishPage::SetItemImage( int nIndex, const SITEMCUSTOM& sItemCustom, WORD wTurnNum )
{
	m_pSearchRow[nIndex]->SetItemImage(sItemCustom, wTurnNum);
}

void MarketSearchWhishPage::IsUseItem( int nIndex, const SITEMCUSTOM& sItemCustom )
{
	m_pSearchRow[nIndex]->IsUseItem(sItemCustom);
}

void MarketSearchWhishPage::SetItemName( int nIndex, const SITEMCUSTOM& sItemCustom )
{
	m_pSearchRow[nIndex]->SetItemName(sItemCustom);
}

void MarketSearchWhishPage::SetTitemSellerName( int nIndex, const CString& strText )
{
	m_pSearchRow[nIndex]->SetTitemSellerName(strText);
}

void MarketSearchWhishPage::SetTitemSellerPos()
{

}

void MarketSearchWhishPage::SetItemPrice( int nIndex, LONGLONG lnPrice, private_market::eSellType eSellType )
{
    m_pSearchRow[nIndex]->SetItemPrice(lnPrice, eSellType );
}

void MarketSearchWhishPage::SetItemCount()
{

}

void MarketSearchWhishPage::DelCheckRow()
{
	for ( unsigned i = 0; i < m_pSearchRow.size(); i++ )
	{
		if( m_pSearchRow[i]->IsChecked() )
		{
			
			int nIndex = m_pPagenavigation->GetIndex();
			m_pGaeaClient->GetPrivateMarketClient()->DelWhishPageData( m_pSearchRow[i]->GetUUID() );
			m_pSearchRow[i]->SetCheckOff();
			m_pSearchRow[i]->ItemCoutntEditBoxClear();
		}
	}

	Refresh();
}

void MarketSearchWhishPage::SetUUID( int nIndex, UUID uuidID )
{
	if ( nIndex > 8)
		return;
	m_pSearchRow[nIndex]->SetUUID(uuidID);
}

void MarketSearchWhishPage::SetsSaleItem( int nIndex, private_market::sSaleItem* sSaleItem )
{
	m_pSearchRow[nIndex]->SetsSaleItem(sSaleItem);
}

void MarketSearchWhishPage::SetMapName( int nindex, const CString strText )
{
	m_pSearchRow[nindex]->SetMapName(strText);
}

void MarketSearchWhishPage::Refresh()
{
	for (unsigned i = 0; i < m_pSearchRow.size(); i++)
	{
		m_pSearchRow[i]->SetVisibleSingle(FALSE);
	}

	m_pPagenavigation->PagenavigationUpdate( m_pGaeaClient->GetPrivateMarketClient()->GetWhishPageCount() );

	m_vecDataPage.clear();
	m_vecDataPage = m_pGaeaClient->GetPrivateMarketClient()->GetWhishPageData( m_pPagenavigation->GetPageIndex() );


	for (unsigned int i = 0; i < m_vecDataPage.size(); i ++)
	{
		SetMapName(i,m_pGaeaClient->GetMapName(m_vecDataPage[i]->sSellerMapID));
		SetsSaleItem(i,m_vecDataPage[i]);
		SetUUID(i,m_vecDataPage[i]->sItemGUID);
		SetItemImage(i,m_vecDataPage[i]->sItemcustom, static_cast<WORD>(m_vecDataPage[i]->dwItemTurnNum) );
		SetItemName(i,m_vecDataPage[i]->sItemcustom);
		IsUseItem(i,m_vecDataPage[i]->sItemcustom);
        SetItemPrice(i,m_vecDataPage[i]->llItemPrice, static_cast<private_market::eSellType>(m_vecDataPage[i]->nSellType));
		SetTitemSellerName(i,m_vecDataPage[i]->szSellerName);
		m_pSearchRow[i]->SetVisibleSingle(TRUE);
	}
}