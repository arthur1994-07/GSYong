#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/G-Logic/GLogic.h"
#include "../Util/CheckBox.h"
#include "../util/SmartComboBoxEx.h"
#include "../Util/RnButton.h"
#include "../Util/MessageBox.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../RanLogicClient/PrivateMarketSearchBuy/PrivateMarketClient.h"

#include "../../InnerInterface.h"

#include "Pagenavigation.h"
#include "MarketSearchRow.h"
#include "MarketSearchPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace
{
	const char* ID2GAMEWORD_TITLE = "검색결과";
	const char* ID2GAMEWORD_ICON = "아이콘";
	const char* ID2GAMEWORD_SELLER_INFO = "판매자정보";
	const char* ID2GAMEWORD_ITEMNAME = "아이템명";
	const char* ID2GAMEWORD_COST = "개당구입금액";
	const char* ID2GAMEWORD_OPTION_TITLE = "검색조건";
	const char* ID2GAMEWORD_ITEM_CATEGORY = "아이템종류";
	const char* ID2GAMEWORD_BASIC_OPTION = "기본조건";
	const char* ID2GAMEWORD_BASIC_TERM_TEXTBOX = "기간제";
	const char* ID2GAMEWORD_BASIC_WEAR_TEXTBOX = "착용위치";
	const char* ID2GAMEWORD_NOMAL_STATS = "일반 능력치";
	const char* ID2GAMEWORD_ADD_RANDOM_OPTION = "가산 및 랜덤 옵션";

	const char* ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO = "POSTBOX_PAGE_SEND_COMBOBOX_DESIGN_TEXINFO";
	const char* ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS = "MARKET_SEARCH_PAGE_BASIC_WEAR_CATEGORY_COMBO_BUTTON";

	const char* ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_NORMAL";
	const char* ID2GAMEWORD_COMBOBOX_MOUSE_OVER = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_OVER";
	const char* ID2GAMEWORD_COMBOBOX_MOUSE_CLICK = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_DOWN";
	//const char* ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL = "RNBUTTON_DEFAULT_TEXTURE";
	//const char* ID2GAMEWORD_COMBOBOX_MOUSE_OVER = "RNBUTTON_OVER_TEXTURE";
	//const char* ID2GAMEWORD_COMBOBOX_MOUSE_CLICK = "RNBUTTON_CLICK_TEXTURE";


	const char* ID2GAMEWORD_ADD_WHISH_BUTTON = "찜";
	const char* ID2GAMEWORD_BUY_BUTTON = "구입";

	const char* ID2GAMEWORD_RESET_BUTTON = "초기화";
	const char* ID2GAMEWORD_SEARCH_BUTTON = "검색";
    const char* ID2GAMEWORD_SEARCH_COOLTIME_BUTTON = "CoolTime[%1%]sec";
    const char* ID2GAMEWORD_SEARCH_NOT_EXIST_RESULT = "검색 결과가 없습니다.";

    const char* ID2GAMETEXT_WAIT_SEARCH = "잠시만 기다려 주세요";

	const char* szButtonNormal = "RNBUTTON_DEFAULT_TEXTURE";
	const char* szButtonOver = "RNBUTTON_OVER_TEXTURE";
	const char* szButtonDown = "RNBUTTON_CLICK_TEXTURE";

	const char* ITEM_CATEGORY = "모두";
	const char* LIMIT_OPTION = "없음";
	const char* LEVEL_CATEGORY = "모든등급";
	const char* CLASS_CATEGORY = "모든부서";
	const char* SEX_CATEGORY = "모두";
	const char* WEAR_CATEGORY = "모두";
	const char* NOMAL_STATS_COMBO = "없음";
	const char* ADD_RANDOM_OPTION_1 = "없음";
	const char* ADD_RANDOM_OPTION_2 = "모두";

	const char* WEAR_WEAPON ="착용무기";
	const char* WEAR_POS ="착용위치";
	const char* WEAR_NUM ="용량";

	const char* ITEM_NAME_INPUT ="아이템 이름을 입력하세요.";
	const char* SEARCH_MIN_VALUE ="최소값";
}


MarketSearchPage::MarketSearchPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx( pInterface, pEngineDevice )
, m_pd3dDevice( NULL )
, m_pGaeaClient( pGaeaClient )
, m_pInterface( pInterface )
, m_pTitleLineBox( NULL )
, m_pMiddleLineBox( NULL )
, m_pTitleBox( NULL )
, m_pSearchRow( NULL )
, m_pItemSearchEditBox ( NULL )
, m_pBasicStatsEditBox ( NULL )
, m_pComboBox_Design ( NULL )
, m_pComboBox_RollOver_Design ( NULL )
, m_RollOverID (NO_ID)
, m_pMoneyTextBox ( NULL )
, m_pPagenavigation ( NULL )
, m_pSelectLine ( NULL )
//, m_SearchOption ( pGaeaClient )
, m_nSelectPage ( NULL_PAGE )
, m_nSelectRow ( NULL_ROW )
, m_pLimiteItemCheckBox ( NULL )
, m_pSearchButton ( NULL )
, m_pBuyMessageBox ( NULL )
, m_bPageWaiting ( FALSE )
, m_pWaitingBox ( FALSE )
, m_nSortName ( 0 )
, m_nSortPrice ( 0 )
 {
	 ID2GAMEWORD_TITLE = ID2GAMEWORD("MARKET_SEARCH_BUY_TITLE", 0);//"검색결과";
	 ID2GAMEWORD_ICON = ID2GAMEWORD("MARKET_SEARCH_BUY_ITEM_IMAGE", 0);//"아이콘";
	 ID2GAMEWORD_SELLER_INFO = ID2GAMEWORD("MARKET_SEARCH_BUY_SELLER_INFO", 0);//"판매자정보";
	 ID2GAMEWORD_ITEMNAME = ID2GAMEWORD("MARKET_SEARCH_BUY_ITEM_NAME", 0);//"아이템명";
	 ID2GAMEWORD_COST = ID2GAMEWORD("MARKET_SEARCH_BUY_INDIVIDUALLY_COST", 0);//"개당구입금액";
	 ID2GAMEWORD_OPTION_TITLE = ID2GAMEWORD("MARKET_SEARCH_BUY_SEARCH_OPTION", 0);//"검색조건";
	 ID2GAMEWORD_ITEM_CATEGORY = ID2GAMEWORD("MARKET_SEARCH_BUY_ITEMCATEGORY", 0);//"아이템종류";
	 ID2GAMEWORD_BASIC_OPTION = ID2GAMEWORD("MARKET_SEARCH_BUY_BASIC_OPTION", 0);//"기본조건";
	 ID2GAMEWORD_BASIC_TERM_TEXTBOX = ID2GAMEWORD("MARKET_SEARCH_BUY_TERM_ITEM", 0);//"기간제";
	 ID2GAMEWORD_BASIC_WEAR_TEXTBOX = ID2GAMEWORD("MARKET_SEARCH_BUY_WEAR_POS", 0);//"착용위치";
	 ID2GAMEWORD_NOMAL_STATS = ID2GAMEWORD("MARKET_SEARCH_BUY_NOMAL_STATS", 0);//"일반 능력치";
	 ID2GAMEWORD_ADD_RANDOM_OPTION = ID2GAMEWORD("MARKET_SEARCH_BUY_ADD_RANDOM_OPTION", 0);//"가산 및 랜덤 옵션";
	 ID2GAMEWORD_ADD_WHISH_BUTTON = ID2GAMEWORD("MARKET_SEARCH_BUY_ADD_WHISH_BUTTON", 0);//"찜";
	 ID2GAMEWORD_BUY_BUTTON = ID2GAMEWORD("MARKET_SEARCH_BUY_BUTTON", 0);//"구입";
	 ID2GAMEWORD_RESET_BUTTON = ID2GAMEWORD("MARKET_SEARCH_BUY_RESET_BUTTON", 0);//"초기화";
	 ID2GAMEWORD_SEARCH_BUTTON = ID2GAMEWORD("MARKET_SEARCH_BUY_SEARCH_BUTTON", 0);//"검색";
     ID2GAMEWORD_SEARCH_COOLTIME_BUTTON = ID2GAMEWORD("MARKET_SEARCH_BUY_SEARCH_BUTTON", 1);//검색대기 시간 "CoolTime[%1%]sec";
     ID2GAMEWORD_SEARCH_NOT_EXIST_RESULT = ID2GAMEWORD("MARKET_SEARCH_BUY_NO_EXIST_RESULT",0);//"검색결과가 없습니다.";
     ID2GAMETEXT_WAIT_SEARCH = ID2GAMEINTEXT("WAITSERVER_MESSAGE");//"잠시만 기다려주세요";

	 WEAR_WEAPON =ID2GAMEWORD("RKET_SEARCH_BUY_WEAR_WEPON",0);//"착용무기";
	 WEAR_POS =ID2GAMEWORD("RKET_SEARCH_BUY_WEAR_POS",0);//"착용위치";
	 WEAR_NUM =ID2GAMEWORD("RKET_SEARCH_BUY_WEAR_NUM",0);//"용량";
	 ITEM_NAME_INPUT =ID2GAMEWORD("MARKET_SEARCH_BUY_IIEM_NAME_INPUT",0);//"아이템 이름을 입력하세요.";
	 SEARCH_MIN_VALUE =ID2GAMEWORD("MARKET_SEARCH_BUY_SEARCH_MIN_VALUE",0);//"최소값";
	 m_strArrowAsc = "LOWER_ARROW_SKYBULE";
	 m_strArrowDesc = "UPPER_ARROW_ORANGE";
 }

MarketSearchPage::~MarketSearchPage()
{
}

void MarketSearchPage::CreateSubControl()
{
	//필터 XML로드
	private_market::GLPrivateMarketDefine::Instance()->LoadXmlTextData();
	//m_SearchOption.LoadUiOptionFile();
	{// 검색결과 창 배경& 라인박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_SEARCH_PAGE_BG" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );
	}
	{// 검색결과창 타이틀 박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_SEARCH_PAGE_TITLE" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );
	 // [검색결과] 텍스트 박스
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		m_pTitleBox = new CBasicTextBox(m_pEngineDevice);
		m_pTitleBox->CreateSub( this, "MARKET_SEARCH_PAGE_TITLE" );
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

		//// [아이템 정렬] 텍스트 박스
		//CBasicTextBox* pItemNameSortBox  = new CBasicTextBox(m_pEngineDevice);
		//pItemNameSortBox->CreateSub( this, "MARKET_SEARCH_PAGE_OBJECT_ITEMNAME_SORT", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_ITEM_NAME_SORT );
		//pItemNameSortBox->SetFont( pFont );
		//pItemNameSortBox->SetText( "V",NS_UITEXTCOLOR::LIGHTSKYBLUE );
		//pItemNameSortBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		//RegisterControl( pItemNameSortBox );

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

		//// [개당구입금액 정렬] 텍스트 박스
		//CBasicTextBox* pCostSortBox = new CBasicTextBox(m_pEngineDevice);
		//pCostSortBox->CreateSub( this, "MARKET_SEARCH_PAGE_OBJECT_COST_SORT",UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_ITEM_COST_SORT );
		//pCostSortBox->SetFont( pFont );
		//pCostSortBox->SetText( "V",NS_UITEXTCOLOR::LIGHTSKYBLUE );
		//pCostSortBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		//RegisterControl( pCostSortBox );
	}
	{// 검색된 아이템 행
		for (int i = 0; i < MARKET_ROW; i ++)
		{
			MarketSearchRow* pSearchRow = new MarketSearchRow( m_pGaeaClient, m_pInterface, m_pEngineDevice );
			pSearchRow->CreateSub(this,"MARKET_SEARCH_PAGE_SEARCH_ROW_BG",UI_FLAG_DEFAULT, ID_ROW1 + i );
			pSearchRow->CreateSubControl();
			//pSearchRow->SetVisibleSingle( FALSE );
			RegisterControl( pSearchRow );
			pSearchRow->SetCheckBoxVisible(FALSE);
			m_pSearchRow.push_back(pSearchRow);

			if ( i > 0)
			{
				UIRECT rcOldPos = m_pSearchRow[i - 1]->GetLocalPos();
				UIRECT rcNewPos = m_pSearchRow[i]->GetLocalPos();
				m_pSearchRow[i]->SetLocalPos( D3DXVECTOR2( rcNewPos.left, rcOldPos.bottom - 1));
			}
			pSearchRow->SetVisibleSingle(FALSE);
		}
	}

	{// 검색조건 창 배경& 라인박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_SEARCH_PAGE_OPTION_BG" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );
	}
	{// 검색조건창 타이틀 박스
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "MARKET_SEARCH_PAGE_OPTION_TITLE" );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );

		// [검색조건] 텍스트 박스
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTitleBox = new CBasicTextBox(m_pEngineDevice);
		pTitleBox->CreateSub( this, "MARKET_SEARCH_PAGE_OPTION_TITLE" );
		pTitleBox->SetFont( pFont );
		pTitleBox->SetText( ID2GAMEWORD_OPTION_TITLE,NS_UITEXTCOLOR::WHITE );
		pTitleBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pTitleBox );

		// 아이템 검색 에디트 박스 배경
		CBasicLineBoxSmart* pItemSearchBGLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pItemSearchBGLineBox->CreateSub( this, "MARKET_SEARCH_PAGE_ITEM_SEARCH_EDITBOX_BG",UI_FLAG_DEFAULT,ID_ITEM_SEARCH_EDITBOX_1_BG );
		pItemSearchBGLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pItemSearchBGLineBox );

		// [아아템 검색 에디트 박스] 
		CUIEditBoxMan* pItemSearchEditBox = new CUIEditBoxMan(m_pEngineDevice);
		pItemSearchEditBox->CreateSub ( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX",UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_ITEM_SEARCH_EDITBOX );
		pItemSearchEditBox->CreateEditBox(ID_ITEM_SEARCH_EDITBOX_1,"MARKET_SEARCH_PAGE_ITEM_SEARCH_EDITBOX", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 100);
		//pItemSearchEditBox->CreateCarrat ( "MAX_GROUP_CHAT_TALK_INPUT_BOX_CARRAT", TRUE, UINT_MAX );
		//pItemSearchEditBox->SetLimitInput ( CHAT_MSG_SIZE - 10 );
		//pItemSearchEditBox->SetFont ( pFont );
		RegisterControl ( pItemSearchEditBox );
		m_pItemSearchEditBox = pItemSearchEditBox;

		// [아아템종류] 텍스트 박스
		CBasicTextBox* pItemCategoryTextBox = new CBasicTextBox(m_pEngineDevice);
		pItemCategoryTextBox->CreateSub( this, "MARKET_SEARCH_PAGE_ITEM_CATEGORY" );
		pItemCategoryTextBox->SetFont( pFont );
		pItemCategoryTextBox->SetText( ID2GAMEWORD_ITEM_CATEGORY,NS_UITEXTCOLOR::WHITE );
		pItemCategoryTextBox->SetTextAlign( TEXT_ALIGN_LEFT| TEXT_ALIGN_CENTER_Y );
		RegisterControl( pItemCategoryTextBox );

		// 아이템종류 콤보박스
		CSmartComboBoxBasicEx* pItemCategoryComboBox; 
		pItemCategoryComboBox = m_pComboBox_Design = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pItemCategoryComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_ITEM_CATEGORY_COMBO );
		pItemCategoryComboBox->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_ITEM_CATEGORY_COMBO_BG", 
											  ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
											  ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
											  ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
											  ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
											  ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		// 아래의 COMMENT는 데이터에서 읽어오지않는다;
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_TYPE,private_market::eItemTypeSize, pItemCategoryComboBox );
		pItemCategoryComboBox->SetText(pItemCategoryComboBox->GetRollOverText(0));
		m_pSearchOptionCombox.push_back(pItemCategoryComboBox);

		// 아이템 검색 STATS 에디트 박스 배경
		CBasicLineBoxSmart* pItemSearchStatsEditBGLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pItemSearchStatsEditBGLineBox->CreateSub( this, "MARKET_SEARCH_PAGE_BASIC_STATS_EDIT_BOX_BG" );
		pItemSearchStatsEditBGLineBox->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
		RegisterControl( pItemSearchStatsEditBGLineBox );

		// 아이템 용량 에디트 박스 배경
		CBasicLineBoxSmart* pItemSearchTurnNumEditBGLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pItemSearchTurnNumEditBGLineBox->CreateSub( this, "MARKET_SEARCH_PAGE_BASIC_TURNNUM_EDIT_BG" );
		pItemSearchTurnNumEditBGLineBox->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
		RegisterControl( pItemSearchTurnNumEditBGLineBox );

		//검색 페이지 기본조건
		//기본조건 배경
		CBasicLineBoxSmart* pLineBoxSearchOptionBG = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBoxSearchOptionBG->CreateSub( this, "MARKET_SEARCH_PAGE_BASIC_OPTION_BG" );
		pLineBoxSearchOptionBG->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBoxSearchOptionBG );

		// [골드]체크박스,아이콘
		{
			CreateLineBox( "MARKET_SEARCH_PAGE_GOLD_ICON", "RAN_MONEY_ICON_TEXTURE" );

			CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
			pCheckBox->CreateSub( this, "MARKET_SEARCH_PAGE_GOLD_CHECK_BOX", UI_FLAG_DEFAULT, ID_SEARCH_SELL_TYPE_GOLD_CHECKBOX );
			pCheckBox->CreateSubControl( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
			RegisterControl( pCheckBox );
			m_pCheckBoxGold = pCheckBox;
			m_pCheckBoxGold->SetCheckOn();
		}

		// [포인트]체크박스,아이콘
		{
			CreateLineBox( "MARKET_SEARCH_PAGE_POINT_ICON", "RAN_POINT_ICON_TEXTURE" );

			CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
			pCheckBox->CreateSub( this, "MARKET_SEARCH_PAGE_POINT_CHECK_BOX", UI_FLAG_DEFAULT, ID_SEARCH_SELL_TYPE_POINT_CHECKBOX );
			pCheckBox->CreateSubControl( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
			RegisterControl( pCheckBox );
			m_pCheckBoxPoint = pCheckBox;
			m_pCheckBoxPoint->SetCheckOn();
		}

		// [아이템 검색 STATS 에디트 박스] 
		CUIEditBoxMan* pBasicStatsEditBox = new CUIEditBoxMan(m_pEngineDevice);
		pBasicStatsEditBox->CreateSub ( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX",UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_BASIC_STATS );
		pBasicStatsEditBox->CreateEditBox(ID_BASIC_STATS_1,"MARKET_SEARCH_PAGE_BASIC_STATS_EDIT_BOX", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 20,TRUE);
		RegisterControl ( pBasicStatsEditBox );
		pBasicStatsEditBox->GetEditBox(ID_BASIC_STATS_1)->SetAlign(1);
		m_pItemStatsEditBox = pBasicStatsEditBox;

		// [아이템 용량 에디트 박스] 
		CUIEditBoxMan* pBasicItemTurnNumEditBox = new CUIEditBoxMan(m_pEngineDevice);
		pBasicItemTurnNumEditBox->CreateSub ( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX",UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_BASIC_ITEM_TURN );
		pBasicItemTurnNumEditBox->CreateEditBox(ID_BASIC_ITEM_TURN_1,"MARKET_SEARCH_PAGE_BASIC_TURNNUM_EDIT_BOX", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 20,TRUE);
		RegisterControl ( pBasicItemTurnNumEditBox );
		pBasicItemTurnNumEditBox->GetEditBox(ID_BASIC_ITEM_TURN_1)->SetAlign(1);
		m_pBasicItemTurnNumEditBox = pBasicItemTurnNumEditBox;

		// [기본조건] 텍스트 박스
		CBasicTextBox* pBasicOptionTextBox = new CBasicTextBox(m_pEngineDevice);
		pBasicOptionTextBox->CreateSub( this, "MARKET_SEARCH_PAGE_BASIC_TEXT_BOX" );
		pBasicOptionTextBox->SetFont( pFont );
		pBasicOptionTextBox->SetText( ID2GAMEWORD_BASIC_OPTION,NS_UITEXTCOLOR::WHITE );
		pBasicOptionTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pBasicOptionTextBox );

		// [기간제] 텍스트 박스
		CBasicTextBox* pBasicTermTextBox = new CBasicTextBox(m_pEngineDevice);
		pBasicTermTextBox->CreateSub( this, "MARKET_SEARCH_PAGE_BASIC_TERM_TEXTBOX" );
		pBasicTermTextBox->SetFont( pFont );
		pBasicTermTextBox->SetText( ID2GAMEWORD_BASIC_TERM_TEXTBOX,NS_UITEXTCOLOR::WHITE );
		pBasicTermTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pBasicTermTextBox );

		//[기간제]체크박스
		CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
		pCheckBox->CreateSub( this, "MARKET_SEARCH_PAGE_BASIC_TERM_CHECK_BOX", UI_FLAG_DEFAULT );
		pCheckBox->CreateSubControl( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
		RegisterControl( pCheckBox );
		m_pLimiteItemCheckBox = pCheckBox;

		//[착용위치] 텍스트 박스
		CBasicTextBox* pBasicWearTextBox = new CBasicTextBox(m_pEngineDevice);
		pBasicWearTextBox->CreateSub( this, "MARKET_SEARCH_PAGE_BASIC_WEAR_POSITION_TEXTBOX" );
		pBasicWearTextBox->SetFont( pFont );
		pBasicWearTextBox->SetText( ID2GAMEWORD_BASIC_WEAR_TEXTBOX,NS_UITEXTCOLOR::WHITE );
		pBasicWearTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pBasicWearTextBox );
		m_pBasicWearTexBox = pBasicWearTextBox;

		// 착용위치 콤보박스
		CSmartComboBoxBasicEx* pBasicWearCategoryComboBox; 
		pBasicWearCategoryComboBox = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pBasicWearCategoryComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_BASIC_WEAR_CATEGORY_COMBO );
		pBasicWearCategoryComboBox->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_BASIC_WEAR_CATEGORY_COMBO_BG", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		// 아래의 COMMENT는 데이터에서 읽어오지않는다;
		SetRollOverCategory(COMMENT::PRIVATE_MARKET_ITEM_ARMOR_TYPE, private_market::eItemArmorTypeSize,pBasicWearCategoryComboBox);
		pBasicWearCategoryComboBox->SetText(pBasicWearCategoryComboBox->GetRollOverText(0));
		m_pSearchOptionCombox.push_back(pBasicWearCategoryComboBox);

		// 착용스텟 콤보박스
		CSmartComboBoxBasicEx* pBasicStatsCategoryComboBox; 
		pBasicStatsCategoryComboBox  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pBasicStatsCategoryComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_BASIC_STATS_CATEGORY_COMBO );
		pBasicStatsCategoryComboBox->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_BASIC_STATS_CATEGORY_COMBO_BG", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		// 아래의 COMMENT는 데이터에서 읽어오지않는다;
		SetRollOverCategory(COMMENT::PRIVATE_MARKET_ITEM_REQUIRE, private_market::eItemRequireSize,pBasicStatsCategoryComboBox);
		pBasicStatsCategoryComboBox->SetText(pBasicStatsCategoryComboBox->GetRollOverText(0));
		m_pSearchOptionCombox.push_back(pBasicStatsCategoryComboBox);

		// 아이템등급 콤보박스
		CSmartComboBoxBasicEx* pBasicLevelCategoryComboBox; 
		pBasicLevelCategoryComboBox  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pBasicLevelCategoryComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_BASIC_LEVEL_CATEGORY_COMBO );
		pBasicLevelCategoryComboBox->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_BASIC_ITEM_LEVEL_CATEGORY_COMBO_BG", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		// 아래의 COMMENT는 데이터에서 읽어오지않는다;
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_RANK, private_market::eItemRankSize, pBasicLevelCategoryComboBox );
		pBasicLevelCategoryComboBox->SetText(pBasicLevelCategoryComboBox->GetRollOverText(0));
		m_pSearchOptionCombox.push_back(pBasicLevelCategoryComboBox);

		// 캐릭터 클래스 학원표시 박스
		CBasicLineBoxSmart* pItemSearchClassImageBGLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pItemSearchClassImageBGLineBox->CreateSub( this, "MARKET_SEARCH_PAGE_BASIC_CLASS_IMAGE_BOX" );
		pItemSearchClassImageBGLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pItemSearchClassImageBGLineBox );

		//CUIControl* pClsaaImage = new CUIControl(m_pEngineDevice);
		//pClsaaImage->CreateSub(this,"MARKET_SEARCH_PAGE_BASIC_CLASS_IMAGE_BOX");
		//RegisterControl(pClsaaImage);
		//m_pClassImage = pClsaaImage;

		// 캐릭터 클래스 표시 콤보박스
		CSmartComboBoxBasicEx* pBasicClassCategoryComboBox; 
		pBasicClassCategoryComboBox  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pBasicClassCategoryComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_BASIC_CLASS_CATEGORY_COMBO );
		pBasicClassCategoryComboBox->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_BASIC_CLASS_CATEGORY_COMBO_BG", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		// 아래의 COMMENT는 데이터에서 읽어오지않는다;
		SetRollOverCategory(  COMMENT::PRIVATE_MARKET_ITEM_REQUIRE_CLASS, private_market::eItemRequireClassSize, pBasicClassCategoryComboBox );
		pBasicClassCategoryComboBox->SetText(pBasicClassCategoryComboBox->GetRollOverText(0));
		m_pSearchOptionCombox.push_back(pBasicClassCategoryComboBox);

		// 성별 표시 콤보박스
		CSmartComboBoxBasicEx* pBasicSexCategoryComboBox; 
		pBasicSexCategoryComboBox  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pBasicSexCategoryComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_BASIC_SEX_CATEGORY_COMBO );
		pBasicSexCategoryComboBox->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_BASIC_SEX_CATEGORY_COMBO_BG", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		// 아래의 COMMENT는 데이터에서 읽어오지않는다;
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_REQUIRE_SEX, private_market::eItemRequireSexSize, pBasicSexCategoryComboBox );
		pBasicSexCategoryComboBox->SetText(pBasicSexCategoryComboBox->GetRollOverText(0));
		//m_pSearchOptionCombox[ ID_BASIC_SEX_CATEGORY_COMBO - ID_COMBOX_START ] = pBasicSexCategoryComboBox;
		m_pSearchOptionCombox.push_back(pBasicSexCategoryComboBox);


		// [일반 능력치] EDIT 박스1
		CBasicLineBoxSmart* pNomalStatsEditBGLineBox_1 = new CBasicLineBoxSmart(m_pEngineDevice);
		pNomalStatsEditBGLineBox_1->CreateSub( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX_BG_1" );
		pNomalStatsEditBGLineBox_1->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
		RegisterControl( pNomalStatsEditBGLineBox_1 );

		// [일반 능력치] EDIT 박스2
		CBasicLineBoxSmart* pNomalStatsEditBGLineBox_2 = new CBasicLineBoxSmart(m_pEngineDevice);
		pNomalStatsEditBGLineBox_2->CreateSub( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX_BG_2" );
		pNomalStatsEditBGLineBox_2->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
		RegisterControl( pNomalStatsEditBGLineBox_2 );

		// [일반 능력치] EDIT 박스3
		CBasicLineBoxSmart* pNomalStatsEditBGLineBox_3 = new CBasicLineBoxSmart(m_pEngineDevice);
		pNomalStatsEditBGLineBox_3->CreateSub( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX_BG_3" );
		pNomalStatsEditBGLineBox_3->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
		RegisterControl( pNomalStatsEditBGLineBox_3 );

		// [일반 능력치] EDIT 박스4
		CBasicLineBoxSmart* pNomalStatsEditBGLineBox_4 = new CBasicLineBoxSmart(m_pEngineDevice);
		pNomalStatsEditBGLineBox_4->CreateSub( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX_BG_4" );
		pNomalStatsEditBGLineBox_4->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
		RegisterControl( pNomalStatsEditBGLineBox_4 );

		//검색 페이지 일반능력치 배경
		CBasicLineBoxSmart* pLineBoxSearchNomalStatsBG = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBoxSearchNomalStatsBG->CreateSub( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_BG" );
		pLineBoxSearchNomalStatsBG->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBoxSearchNomalStatsBG );

		// [일반 능력치] 텍스트 박스
		CBasicTextBox* pBasicNomalStatsTextBox = new CBasicTextBox(m_pEngineDevice);
		pBasicNomalStatsTextBox->CreateSub( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_TEXTBOX" );
		pBasicNomalStatsTextBox->SetFont( pFont );
		pBasicNomalStatsTextBox->SetText( ID2GAMEWORD_NOMAL_STATS,NS_UITEXTCOLOR::WHITE );
		pBasicNomalStatsTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pBasicNomalStatsTextBox );

		 
		// [일반능력치 에디트 박스] 
		CUIEditBoxMan* pItemStatsEditBox = new CUIEditBoxMan(m_pEngineDevice);
		pItemStatsEditBox->CreateSub ( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX",UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_NOMAL_STATS_EDITBOX );
		pItemStatsEditBox->CreateEditBox(ID_NOMAL_STATS_EDITBOX_1,"MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX_1", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 20,FALSE, TRUE);
		pItemStatsEditBox->CreateEditBox(ID_NOMAL_STATS_EDITBOX_2,"MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX_2", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 20,FALSE, TRUE);
		pItemStatsEditBox->CreateEditBox(ID_NOMAL_STATS_EDITBOX_3,"MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX_3", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 20,FALSE, TRUE);
		pItemStatsEditBox->CreateEditBox(ID_NOMAL_STATS_EDITBOX_4,"MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX_4", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 20,FALSE, TRUE);
		pItemStatsEditBox->GetEditBox(ID_NOMAL_STATS_EDITBOX_1)->SetAlign(1);
		pItemStatsEditBox->GetEditBox(ID_NOMAL_STATS_EDITBOX_2)->SetAlign(1);
		pItemStatsEditBox->GetEditBox(ID_NOMAL_STATS_EDITBOX_3)->SetAlign(1);
		pItemStatsEditBox->GetEditBox(ID_NOMAL_STATS_EDITBOX_4)->SetAlign(1);
		//pItemSearchEditBox->CreateCarrat ( "MAX_GROUP_CHAT_TALK_INPUT_BOX_CARRAT", TRUE, UINT_MAX );
		//pItemSearchEditBox->SetLimitInput ( CHAT_MSG_SIZE - 10 );
		//pItemSearchEditBox->SetFont ( pFont );
		RegisterControl ( pItemStatsEditBox );
		m_pBasicStatsEditBox = pItemStatsEditBox;

		//m_pItemSearchEditBox->EndEdit ();

		//m_pItemSearchEditBox->Init();
		//m_pItemSearchEditBox->BeginEdit();

		// 일반능력치 콤보박스
		CSmartComboBoxBasicEx* pNomalStatsComboBox_1; 
		pNomalStatsComboBox_1  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pNomalStatsComboBox_1->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_NOMAL_STATS_COMBO_1 );
		pNomalStatsComboBox_1->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_NOMAL_STATS_COMBO_BG_1", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		// 아래의 COMMENT는 데이터에서 읽어오지않는다;
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_BASIC, private_market::eItemBasicSize, pNomalStatsComboBox_1 );
		pNomalStatsComboBox_1->SetText(pNomalStatsComboBox_1->GetRollOverText(0));
		//m_pSearchOptionCombox[ ID_NOMAL_STATS_COMBO_1 - ID_COMBOX_START ] = pNomalStatsComboBox_1;
		m_pSearchOptionCombox.push_back(pNomalStatsComboBox_1);


		// 일반능력치 콤보박스
		CSmartComboBoxBasicEx* pNomalStatsComboBox_2; 
		pNomalStatsComboBox_2  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pNomalStatsComboBox_2->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_NOMAL_STATS_COMBO_2 );
		pNomalStatsComboBox_2->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_NOMAL_STATS_COMBO_BG_2", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		// 아래의 COMMENT는 데이터에서 읽어오지않는다;
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_BASIC, private_market::eItemBasicSize, pNomalStatsComboBox_2 );
		pNomalStatsComboBox_2->SetText(pNomalStatsComboBox_2->GetRollOverText(0));
		//m_pSearchOptionCombox[ ID_NOMAL_STATS_COMBO_2 - ID_COMBOX_START ] = pNomalStatsComboBox_2;
		m_pSearchOptionCombox.push_back(pNomalStatsComboBox_2);

		// 일반능력치 콤보박스
		CSmartComboBoxBasicEx* pNomalStatsComboBox_3; 
		pNomalStatsComboBox_3  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pNomalStatsComboBox_3->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_NOMAL_STATS_COMBO_3 );
		pNomalStatsComboBox_3->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_NOMAL_STATS_COMBO_BG_3", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_BASIC, private_market::eItemBasicSize, pNomalStatsComboBox_3 );
		pNomalStatsComboBox_3->SetText(pNomalStatsComboBox_3->GetRollOverText(0));
		//m_pSearchOptionCombox[ ID_NOMAL_STATS_COMBO_3 - ID_COMBOX_START ] = pNomalStatsComboBox_3;
		m_pSearchOptionCombox.push_back(pNomalStatsComboBox_3);

		// 일반능력치 콤보박스
		CSmartComboBoxBasicEx* pNomalStatsComboBox_4; 
		pNomalStatsComboBox_4  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pNomalStatsComboBox_4->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_NOMAL_STATS_COMBO_4 );
		pNomalStatsComboBox_4->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_NOMAL_STATS_COMBO_BG_4", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_BASIC, private_market::eItemBasicSize, pNomalStatsComboBox_4 );
		pNomalStatsComboBox_4->SetText(pNomalStatsComboBox_4->GetRollOverText(0));
		//m_pSearchOptionCombox[ ID_NOMAL_STATS_COMBO_4 - ID_COMBOX_START ] = pNomalStatsComboBox_4;
		m_pSearchOptionCombox.push_back(pNomalStatsComboBox_4);

		// [가산 및 랜덤 옵션] EDIT 박스1
		CBasicLineBoxSmart* pAddRandomEditBGLineBox_1 = new CBasicLineBoxSmart(m_pEngineDevice);
		pAddRandomEditBGLineBox_1->CreateSub( this, "MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION_EDITBOX_BG_1" );
		pAddRandomEditBGLineBox_1->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
		RegisterControl( pAddRandomEditBGLineBox_1 );

		// [가산 및 랜덤 옵션] EDIT 박스2
		CBasicLineBoxSmart* pAddRandomEditBGLineBox_2 = new CBasicLineBoxSmart(m_pEngineDevice);
		pAddRandomEditBGLineBox_2->CreateSub( this, "MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION_EDITBOX_BG_2" );
		pAddRandomEditBGLineBox_2->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
		RegisterControl( pAddRandomEditBGLineBox_2 );

		// [가산 및 랜덤 옵션] EDIT 박스3
		CBasicLineBoxSmart* pAddRandomEditBGLineBox_3 = new CBasicLineBoxSmart(m_pEngineDevice);
		pAddRandomEditBGLineBox_3->CreateSub( this, "MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION_EDITBOX_BG_3" );
		pAddRandomEditBGLineBox_3->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
		RegisterControl( pAddRandomEditBGLineBox_3 );

		// [가산 및 랜덤 옵션] EDIT 박스4
		CBasicLineBoxSmart* pAddRandomEditBGLineBox_4 = new CBasicLineBoxSmart(m_pEngineDevice);
		pAddRandomEditBGLineBox_4->CreateSub( this, "MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION_EDITBOX_BG_4" );
		pAddRandomEditBGLineBox_4->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
		RegisterControl( pAddRandomEditBGLineBox_4 );

		//검색 페이지  가산 및 램덤 옵션 배경
		CBasicLineBoxSmart* pLineBoxSearchAddRandomOptionBG = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBoxSearchAddRandomOptionBG->CreateSub( this, "MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION_BG" );
		pLineBoxSearchAddRandomOptionBG->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBoxSearchAddRandomOptionBG );

		// [가산 및 랜덤 옵션 에디트 박스] 
		CUIEditBoxMan* pAddRandomEditBox = new CUIEditBoxMan(m_pEngineDevice);
		pAddRandomEditBox->CreateSub ( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX",UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_ADD_RANDOM_EDITBOX );
		pAddRandomEditBox->CreateEditBox(ID_ADD_RANDOM_EDITBOX_1,"MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION_EDITBOX_1", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 20,FALSE, TRUE);
		pAddRandomEditBox->CreateEditBox(ID_ADD_RANDOM_EDITBOX_2,"MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION_EDITBOX_2", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 20,FALSE, TRUE);
		pAddRandomEditBox->CreateEditBox(ID_ADD_RANDOM_EDITBOX_3,"MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION_EDITBOX_3", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 20,FALSE, TRUE);
		pAddRandomEditBox->CreateEditBox(ID_ADD_RANDOM_EDITBOX_4,"MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION_EDITBOX_4", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 20,FALSE, TRUE);
		pAddRandomEditBox->GetEditBox(ID_ADD_RANDOM_EDITBOX_1)->SetAlign(1);
		pAddRandomEditBox->GetEditBox(ID_ADD_RANDOM_EDITBOX_2)->SetAlign(1);
		pAddRandomEditBox->GetEditBox(ID_ADD_RANDOM_EDITBOX_3)->SetAlign(1);
		pAddRandomEditBox->GetEditBox(ID_ADD_RANDOM_EDITBOX_4)->SetAlign(1);
		//pItemSearchEditBox->CreateCarrat ( "MAX_GROUP_CHAT_TALK_INPUT_BOX_CARRAT", TRUE, UINT_MAX );
		//pItemSearchEditBox->SetLimitInput ( CHAT_MSG_SIZE - 10 );
		//pItemSearchEditBox->SetFont ( pFont );
		RegisterControl ( pAddRandomEditBox );
		m_pAddRandomEditBox = pAddRandomEditBox;

		// [가산 및 램덤 옵션] 텍스트 박스
		CBasicTextBox* pAddRandomOptionTextBox = new CBasicTextBox(m_pEngineDevice);
		pAddRandomOptionTextBox->CreateSub( this, "MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION_TEXTBOX" );
		pAddRandomOptionTextBox->SetFont( pFont );
		pAddRandomOptionTextBox->SetText( ID2GAMEWORD_ADD_RANDOM_OPTION,NS_UITEXTCOLOR::WHITE );
		pAddRandomOptionTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pAddRandomOptionTextBox );

		// 가산 및 램덤 옵션 콤보 박스_1
		CSmartComboBoxBasicEx* pAddRandomOptionComboBox_1; 
		pAddRandomOptionComboBox_1  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pAddRandomOptionComboBox_1->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE,ID_ADD_RANDOM_OPTION_COMBO_BG_1 );
		pAddRandomOptionComboBox_1->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION__COMBO_BG_1", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		// 아래의 COMMENT는 데이터에서 읽어오지않는다;
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM, private_market::eItemAddonVarRandomSize, pAddRandomOptionComboBox_1 );
		pAddRandomOptionComboBox_1->SetText(pAddRandomOptionComboBox_1->GetRollOverText(0));
		//m_pSearchOptionCombox[ ID_ADD_RANDOM_OPTION_COMBO_BG_1 - ID_COMBOX_START ] = pAddRandomOptionComboBox_1;
		m_pSearchOptionCombox.push_back(pAddRandomOptionComboBox_1);

		// 가산 및 램덤 옵션 콤보 박스_2
		CSmartComboBoxBasicEx* pAddRandomOptionComboBox_2; 
		pAddRandomOptionComboBox_2  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pAddRandomOptionComboBox_2->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE,ID_ADD_RANDOM_OPTION_COMBO_BG_2 );
		pAddRandomOptionComboBox_2->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION__COMBO_BG_2", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM, private_market::eItemAddonVarRandomSize, pAddRandomOptionComboBox_2 );
		pAddRandomOptionComboBox_2->SetText(pAddRandomOptionComboBox_2->GetRollOverText(0));
		//m_pSearchOptionCombox[ ID_ADD_RANDOM_OPTION_COMBO_BG_2 - ID_COMBOX_START ] = pAddRandomOptionComboBox_2;
		m_pSearchOptionCombox.push_back(pAddRandomOptionComboBox_2);

		// 가산 및 램덤 옵션 콤보 박스_3
		CSmartComboBoxBasicEx* pAddRandomOptionComboBox_3; 
		pAddRandomOptionComboBox_3  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pAddRandomOptionComboBox_3->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE,ID_ADD_RANDOM_OPTION_COMBO_BG_3 );
		pAddRandomOptionComboBox_3->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION__COMBO_BG_3", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM, private_market::eItemAddonVarRandomSize, pAddRandomOptionComboBox_3 );
		pAddRandomOptionComboBox_3->SetText(pAddRandomOptionComboBox_3->GetRollOverText(0));
		//m_pSearchOptionCombox[ ID_ADD_RANDOM_OPTION_COMBO_BG_3 - ID_COMBOX_START ] = pAddRandomOptionComboBox_3;
		m_pSearchOptionCombox.push_back(pAddRandomOptionComboBox_3);

		// 가산 및 램덤 옵션 콤보 박스_4
		CSmartComboBoxBasicEx* pAddRandomOptionComboBox_4; 
		pAddRandomOptionComboBox_4  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pAddRandomOptionComboBox_4->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE,ID_ADD_RANDOM_OPTION_COMBO_BG_4 );
		pAddRandomOptionComboBox_4->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_ADD_RANDOM_OPTION__COMBO_BG_4", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM, private_market::eItemAddonVarRandomSize, pAddRandomOptionComboBox_4 );
		pAddRandomOptionComboBox_4->SetText(pAddRandomOptionComboBox_4->GetRollOverText(0));
		//m_pSearchOptionCombox[ ID_ADD_RANDOM_OPTION_COMBO_BG_4 - ID_COMBOX_START ] = pAddRandomOptionComboBox_4;
		m_pSearchOptionCombox.push_back(pAddRandomOptionComboBox_4);

		// 가산 및 램덤 옵션 상태이상 콤보 박스_1
		CSmartComboBoxBasicEx* pBLOWComboBox_1; 
		pBLOWComboBox_1  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pBLOWComboBox_1->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE,ID_BLOW_COMBOX_1 );
		pBLOWComboBox_1->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_ADD_RANDOM_BLOW_COMBOX_1", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		// 아래의 COMMENT는 데이터에서 읽어오지않는다;
		//m_pSearchOptionCombox[ ID_ADD_RANDOM_OPTION_COMBO_BG_5 - ID_COMBOX_START ] = pAddRandomOptionComboBox_5;
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_STATE_BLOW, private_market::eItemStateBlowSize, pBLOWComboBox_1 );
		pBLOWComboBox_1->SetVisibleSingle(FALSE);
		m_pSearchOptionCombox.push_back(pBLOWComboBox_1);

		// 가산 및 램덤 옵션 상태이상 콤보 박스_2
		CSmartComboBoxBasicEx* pBLOWComboBox_2; 
		pBLOWComboBox_2  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pBLOWComboBox_2->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE,ID_BLOW_COMBOX_2 );
		pBLOWComboBox_2->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_ADD_RANDOM_BLOW_COMBOX_2", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		//m_pSearchOptionCombox[ ID_ADD_RANDOM_OPTION_COMBO_BG_5 - ID_COMBOX_START ] = pAddRandomOptionComboBox_5;
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_STATE_BLOW, private_market::eItemStateBlowSize, pBLOWComboBox_2 );
		pBLOWComboBox_2->SetVisibleSingle(FALSE);
		m_pSearchOptionCombox.push_back(pBLOWComboBox_2);

		// 가산 및 램덤 옵션 상태이상 콤보 박스_3
		CSmartComboBoxBasicEx* pBLOWComboBox_3; 
		pBLOWComboBox_3  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pBLOWComboBox_3->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE,ID_BLOW_COMBOX_3 );
		pBLOWComboBox_3->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_ADD_RANDOM_BLOW_COMBOX_3", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		//m_pSearchOptionCombox[ ID_ADD_RANDOM_OPTION_COMBO_BG_5 - ID_COMBOX_START ] = pAddRandomOptionComboBox_5;
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_STATE_BLOW, private_market::eItemStateBlowSize, pBLOWComboBox_3 );
		pBLOWComboBox_3->SetVisibleSingle(FALSE);
		m_pSearchOptionCombox.push_back(pBLOWComboBox_3);

		// 가산 및 램덤 옵션 상태이상 콤보 박스_4
		CSmartComboBoxBasicEx* pBLOWComboBox_4; 
		pBLOWComboBox_4  = new CSmartComboBoxBasicEx(m_pEngineDevice);
		pBLOWComboBox_4->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE,ID_BLOW_COMBOX_4 );
		pBLOWComboBox_4->CreateSmartComboBoxCustom( "MARKET_SEARCH_PAGE_ADD_RANDOM_BLOW_COMBOX_4", 
			ID2GAMEWORD_COMBOBOX_DESIGN_TEXINFO,
			ID2GAMEWORD_COMBOBOX_BUTTON_SIZE_POS,
			ID2GAMEWORD_COMBOBOX_BUTTON_NOMAL,
			ID2GAMEWORD_COMBOBOX_MOUSE_OVER,
			ID2GAMEWORD_COMBOBOX_MOUSE_CLICK);
		//m_pSearchOptionCombox[ ID_ADD_RANDOM_OPTION_COMBO_BG_5 - ID_COMBOX_START ] = pAddRandomOptionComboBox_5;
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_STATE_BLOW, private_market::eItemStateBlowSize, pBLOWComboBox_4 );
		pBLOWComboBox_4->SetVisibleSingle(FALSE);
		m_pSearchOptionCombox.push_back(pBLOWComboBox_4);


		//검색 페이지  가산 및 램덤 옵션 배경
		//CBasicLineBoxSmart* pLineBxSarhAddRadmOptioBG = new CBasicLineBoxSmart(m_pEngineDevice);
		//pLineBxSarhAddRadmOptioBG->CreateSub( this, "PAGE_NAVIGATION_PAGE_POS" );
		//pLineBxSarhAddRadmOptioBG->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		//RegisterControl( pLineBxSarhAddRadmOptioBG );

		Pagenavigation* pPagenavigation = new Pagenavigation( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pPagenavigation->CreateSub(this,"PAGE_NAVIGATION_PAGE_POS", UI_FLAG_DEFAULT,  ID_PAGE_NAVIGATION);
		pPagenavigation->SetFont( pFont );
		pPagenavigation->CreateSubControl();
		RegisterControl(pPagenavigation);
		m_pPagenavigation = pPagenavigation;

		//검색 페이지  버튼이랑 구분 선
		CBasicLineBoxSmart* pLineMarketSearchBGLine = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineMarketSearchBGLine->CreateSub( this, "MARKET_SEARCH_PAGE_BG_LINE" );
		pLineMarketSearchBGLine->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineMarketSearchBGLine );


		CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		//리셋버튼
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
			arg.text = sc::string::format( "%s",ID2GAMEWORD_RESET_BUTTON);

			pLineBoxComboButton = new RnButton(m_pEngineDevice);
			pLineBoxComboButton->CreateSub(this, "MARKET_SEARCH_PAGE_RESET_BUTTON", UI_FLAG_RIGHT, ID_SEARCH_OPTION_RESET_BUTTON );
			pLineBoxComboButton->CreateSubControl(arg);
			RegisterControl(pLineBoxComboButton);
		}

		//검색 버튼
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
			arg.text = sc::string::format( "%s",ID2GAMEWORD_SEARCH_BUTTON);

			pLineBoxComboButton = new RnButton(m_pEngineDevice);
			pLineBoxComboButton->CreateSub(this, "MARKET_SEARCH_PAGE_SEARCH_BUTTON", UI_FLAG_RIGHT, ID_ITEM_SEARCH_BUTTON );
			pLineBoxComboButton->CreateSubControl(arg);
			RegisterControl(pLineBoxComboButton);
			m_pSearchButton = pLineBoxComboButton;
		}

		//찜 버튼
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
			pLineBoxComboButton->CreateSub(this, "MARKET_SEARCH_PAGE_BUY_BUTTON", UI_FLAG_RIGHT, ID_ADD_WHISH_BUTTON );
			pLineBoxComboButton->CreateSubControl(arg);
			RegisterControl(pLineBoxComboButton);
		}

		//구입 버튼
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
			pLineBoxComboButton->CreateSub(this, "MARKET_SEARCH_PAGE_ADD_WHISH_BUTTON", UI_FLAG_RIGHT, ID_BUY_BUTTON );
			pLineBoxComboButton->CreateSubControl(arg);
			RegisterControl(pLineBoxComboButton);
		}

		{
			CreateLineBox( "MARKET_SEARCH_PAGE_MONEY_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
			CBasicButton* pButton = CreateFlipButton( "MARKET_SEARCH_PAGE_MONEY_BUTTON", "MARKET_SEARCH_PAGE_MONEY_BUTTON_F",NO_ID, CBasicButton::CLICK_FLIP );
			pButton->CreateMouseOver( "MARKET_SEARCH_PAGE_MONEY_BUTTON_F" );
			pButton->SetAlignFlag( UI_FLAG_BOTTOM );
			pButton->SetUseDynamic( TRUE );

			CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );
			m_pMoneyTextBox = CreateStaticControl( "MARKET_SEARCH_PAGE_MONEY_TEXT", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
			m_pMoneyTextBox->SetAlignFlag( UI_FLAG_BOTTOM );
		}


        {
            CreateLineBox( "MARKET_SEARCH_PAGE_RANPOINT_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
            CreateLineBox( "MARKET_SEARCH_PAGE_RANPOINT_ICON", "RAN_POINT_ICON_TEXTURE" );

            CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );
            m_pRanPointTextBox = CreateStaticControl( "MARKET_SEARCH_PAGE_RANPOINT_TEXT", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
            m_pRanPointTextBox->SetAlignFlag( UI_FLAG_BOTTOM );
        }


		{//선택 상자
			m_pSelectLine = new CBasicLineBoxSmart(m_pEngineDevice);
			m_pSelectLine->CreateSub( this, "MARKET_SEARCH_ROW_BG" );
			m_pSelectLine->CreateSubControl( "LINEBOX_Y" );
			m_pSelectLine->SetVisibleSingle(FALSE);
			RegisterControl( m_pSelectLine );
		}

		CSwapImage* pClsaaImage = new CSwapImage(m_pEngineDevice);
		pClsaaImage->CreateSub(this,"MARKET_SEARCH_PAGE_BASIC_CLASS_IMAGE_BOX");
		RegisterControl(pClsaaImage);
		m_pClassImage = pClsaaImage;

		//콤보박스 맨마지막에 그려야 합니다.
		for ( int i = ID_COMBOX_COUNT; i > -1; i--)
		{
			RegisterControl( m_pSearchOptionCombox[i] );
		}

		//아이템 이름 입력 에디트  박스 롤박스
		{
			CBasicComboBoxRollOver *pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
			pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_ITEM_SEARCH_COMBOX );
			pComboBoxRollOver->CreateBaseComboBoxRollOver( "MARKET_SEARCH_PAGE_ITEM_SEARCH_COMBO_ROLLOVER", FALSE );			
			pComboBoxRollOver->SetVisibleSingle( FALSE );
			RegisterControl( pComboBoxRollOver );
			m_pComboRollSearchOver = pComboBoxRollOver;
		}
	}
    //구매 확인 모달창
    {
        CMessageBox* pMessageBox = new CMessageBox( m_pEngineDevice );
        pMessageBox->CreateSub( this, "UTILE_MESSAGEBOX_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_BUY_MESSAGEBOX );
        pMessageBox->CreateSubControl();
        RegisterControl( pMessageBox );
        m_pBuyMessageBox = pMessageBox;
    }
	//LoadSearchFile();
	//LoadSearchFileFromITEM();
	PagenavigationUpdate();

	//콤보 박스 검색 옵션 초기화
	BOOST_FOREACH( CSmartComboBoxBasicEx* vecSearchOption, m_pSearchOptionCombox )
	{
		vecSearchOption->SetSelectIndex(0);
	}

	CSwapImage* pNameSortImage = new CSwapImage(m_pEngineDevice);
	pNameSortImage->CreateSub(this,"MARKET_SEARCH_PAGE_OBJECT_ITEMNAME_SORT",UI_FLAG_XSIZE | UI_FLAG_YSIZE,ID_ITEM_NAME_SORT);
	RegisterControl(pNameSortImage);
	m_pNameSortImage = pNameSortImage;
	std::string strTexterName1 = "LOWER_ARROW_SKYBULE";
	m_pNameSortImage->SetImage(strTexterName1.c_str());

	CSwapImage* pPriceSortImage = new CSwapImage(m_pEngineDevice);
	pPriceSortImage->CreateSub(this,"MARKET_SEARCH_PAGE_OBJECT_COST_SORT",UI_FLAG_XSIZE | UI_FLAG_YSIZE,ID_ITEM_COST_SORT);
	RegisterControl(pPriceSortImage);
	m_pPriceSortImage = pPriceSortImage;
	std::string strTexterName2 = "LOWER_ARROW_SKYBULE";
	m_pPriceSortImage->SetImage(strTexterName2.c_str());

	{// 서버에서 검색결과 기다릴 때 출력할 메시지 박스
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		m_pWaitingBox = new CBasicTextBox(m_pEngineDevice);
		m_pWaitingBox->CreateSub( this, "MARKET_SEARCH_PAGE_WAITING_BOX" );
		m_pWaitingBox->SetFont( pFont );
		m_pWaitingBox->SetText( ID2GAMETEXT_WAIT_SEARCH,NS_UITEXTCOLOR::WHITE );
		m_pWaitingBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pWaitingBox );
		m_pWaitingBox->SetVisibleSingle(FALSE);
	}

    {// 검색결과가 없을때 출력할 메시지박스
        CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
        m_pNoExistResultBox = new CBasicTextBox(m_pEngineDevice);
        m_pNoExistResultBox->CreateSub( this, "MARKET_SEARCH_PAGE_NOT_EXIST_SEARCH_BOX" );
        m_pNoExistResultBox->SetFont( pFont );
        m_pNoExistResultBox->SetText( ID2GAMEWORD_SEARCH_NOT_EXIST_RESULT,NS_UITEXTCOLOR::WHITE );
        m_pNoExistResultBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( m_pNoExistResultBox );
        m_pNoExistResultBox->SetVisibleSingle(FALSE);
    }

	ResetSearchOption();
}

void MarketSearchPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{

    const DWORD dwRemainTime = m_pGaeaClient->GetRemainSearchLimitTime();
    if( dwRemainTime )
    {
        const DWORD dwRemainSec = (dwRemainTime / 1000) + 1;
        m_pSearchButton->SetOneLineText(sc::string::format(ID2GAMEWORD_SEARCH_COOLTIME_BUTTON, dwRemainSec).c_str(), NS_UITEXTCOLOR::DARKGRAY);
    }
    else
    {
        m_pSearchButton->SetOneLineText(ID2GAMEWORD_SEARCH_BUTTON, NS_UITEXTCOLOR::WHITE);
    }

	//money
	{
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( !pChar )
			return;

		CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pChar->GetInvenMoney(), 3, "," );
		m_pMoneyTextBox->SetOneLineText( strText );


        CString strText2 = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pChar->PointShopPoint(), 3, "," );
        m_pRanPointTextBox->SetOneLineText( strText2, NS_UITEXTCOLOR::ORANGE );
	}

    if ( m_pBuyMessageBox->IsVisible() )
    {
        //구매확인창 모달윈도우로 동작
        CUIControl* pBuyMessageBox = (CUIControl*)m_pBuyMessageBox;
        pBuyMessageBox->Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
        TranslateUIMessage( ID_BUY_MESSAGEBOX, pBuyMessageBox->GetMessageEx() );
    }
	else if ( m_RollOverID == NO_ID )
		CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		//Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	else 
	{
		PagenavigationUpdate();
		ResetMessageEx ();
		CUIControl* pControl = m_ControlContainer.FindControl ( m_RollOverID );
		if ( !pControl )
		{
			GASSERT ( 0 && "심각한 오류, 노드가 널입니다." );
			return ;
		}

		CSmartComboBoxBasicEx* pCombox = static_cast<CSmartComboBoxBasicEx*>(pControl);
		//DWORD dwControlMsg = pCombox->GetMessageEx ();
		//DWORD dwControlMsg = GetMessageEx();

		if ( (m_RollOverID != ID_ITEM_SEARCH_COMBOX) && 
			 (!pCombox->IsVisibleComboxRoolOver()) )
		{
			m_RollOverID = NO_ID;
			m_bFirstLBUP = FALSE;
		}

		if ( (m_RollOverID != ID_ITEM_SEARCH_COMBOX) && 
			( pCombox->IsVisibleComboxRoolOver()) )
		{
			for ( int i = 0; i < ID_COMBOX_COUNT; i++ )
			{
				if (i != ( m_RollOverID - ID_COMBOX_START) )
				m_pSearchOptionCombox[i]->SetVisibleSingleRollOver(FALSE);
			}
		}

		

		CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );


		DWORD dwControlMsg = pCombox->GetMessageEx ();
		NS_UIDEBUGSET::BlockBegin ();
		if ( dwControlMsg ) TranslateUIMessage ( m_RollOverID, dwControlMsg );
		NS_UIDEBUGSET::BlockEnd ();

		//ResetMessageEx ();

		//if ( dwControlMsg )
		//	TranslateUIMessage ( m_RollOverID, dwControlMsg );

		//	스크롤바에 관계된 메시지가 아니고		
		if ( !(dwControlMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
		{
			DWORD dwMsg = GetMessageEx ();
			if ( dwMsg & UIMSG_LB_UP )
			{
				if ( !m_bFirstLBUP )
				{
					if ( m_RollOverID != ID_ITEM_SEARCH_COMBOX )
					{
						m_RollOverID = NO_ID;
						//CSmartComboBoxBasicEx* pCombox = static_cast<CSmartComboBoxBasicEx*>(pControl);
						pCombox->SetVisibleSingleRollOver ( FALSE );
					}
					else
						m_RollOverID = NO_ID;
				}

				m_bFirstLBUP = FALSE;
			}
		}
		else
			m_bFirstLBUP = TRUE;
	}
}


void MarketSearchPage::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
    if ( m_pBuyMessageBox->IsVisible() )
    {
        //구매확인창 모달윈도우로 동작
        return;
    }
	switch ( cID )
	{
	case ID_SEARCH_SELL_TYPE_GOLD_CHECKBOX:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			SetSearchSellTypeCheckBox(ID_SEARCH_SELL_TYPE_GOLD_CHECKBOX);
		}
		break;
	case ID_SEARCH_SELL_TYPE_POINT_CHECKBOX:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			SetSearchSellTypeCheckBox(ID_SEARCH_SELL_TYPE_POINT_CHECKBOX);
		}
		break;
	case ID_ITEM_NAME_SORT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				//m_nSelectRow = NULL_ROW;
				m_pSelectLine->SetVisibleSingle(FALSE);

				//검색된 아이템이 없다면
				if ( !m_pSearchRow[0]->IsVisible())
					break;


				m_pNameSortImage->SetImage(m_strArrowAsc.c_str());

				CString strTextureName = m_pNameSortImage->GetTextureName();

				m_pPagenavigation->SetPage(0);
				m_pPagenavigation->SetCountPage(0);
				m_nSelectPage = NULL_PAGE;

				// 선택된 열 취소 함수로 옮길예정
				m_nSelectRow = NULL_ROW;
				m_pSelectLine->SetVisibleSingle(FALSE);
				SetVisibleFALSESearchRow();//검색된 아이템 열을 오프한다.

				if (m_nSortName == private_market::eItemSortMethodAsc)
				{
					m_nSortName = private_market::eItemSortMethodDesc;
					m_pNameSortImage->SetImage(m_strArrowDesc.c_str());
				}
				else
				{
					m_nSortName = private_market::eItemSortMethodAsc;
					m_pNameSortImage->SetImage(m_strArrowAsc.c_str());
				}
				
				
				ReSearchItem(private_market::eItemSortOptionName);
				
			}
		}
		break;
	case ID_ITEM_COST_SORT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				if ( !m_pSearchRow[0]->IsVisible() )
					break;


				m_pPagenavigation->SetPage(0);
				m_pPagenavigation->SetCountPage(0);
				m_nSelectPage = NULL_PAGE;

				// 선택된 열 취소 함수로 옮길예정
				m_nSelectRow = NULL_ROW;
				m_pSelectLine->SetVisibleSingle(FALSE);
				SetVisibleFALSESearchRow();//검색된 아이템 열을 오프한다.

				if (m_nSortPrice == private_market::eItemSortMethodAsc)
				{
					m_nSortPrice = private_market::eItemSortMethodDesc;
					m_pPriceSortImage->SetImage(m_strArrowDesc.c_str());
				}
				else
				{
					m_nSortPrice = private_market::eItemSortMethodAsc;
					m_pPriceSortImage->SetImage(m_strArrowAsc.c_str());
				}

				ReSearchItem(private_market::eItemSortOptionPrice);
				
			}
		}
		break;
	case ID_ITEM_CATEGORY_COMBO:
		{
			SetComBoxClickEnevt(  cID, dwMsg );

			if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
			{
				int nIndex = m_pSearchOptionCombox[ID_ITEM_CATEGORY_COMBO - ID_COMBOX_START]->GetSelectIndex();
				SetItemCategoryCombo( nIndex );
			}
		}
		break;

	case ID_BASIC_WEAR_CATEGORY_COMBO:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
		}
		break;

	case ID_BASIC_STATS_CATEGORY_COMBO:
		{
            SetComBoxClickEnevt(  cID, dwMsg );
            if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
            {
                int nIndex = m_pSearchOptionCombox[ID_BASIC_STATS_CATEGORY_COMBO - ID_COMBOX_START]->GetSelectIndex();
                if ( nIndex == 0 )
                {
                    m_pItemStatsEditBox->ClearEdit(ID_BASIC_STATS_1);
                    m_pItemStatsEditBox->SetEditString(ID_BASIC_STATS_1,SEARCH_MIN_VALUE);       
                }
            }
		}
		break;

	case ID_BASIC_LEVEL_CATEGORY_COMBO:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
		}
		break;

	case ID_BASIC_CLASS_CATEGORY_COMBO:
		{
			SetComBoxClickEnevt(  cID, dwMsg );

			if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
			{
				SetClassSexTexture();
			}
		}
		break;

	case ID_BASIC_SEX_CATEGORY_COMBO:
		{
			SetComBoxClickEnevt(  cID, dwMsg );

			if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
			{
				SetClassSexTexture();
			}
		}
		break;

	case ID_NOMAL_STATS_COMBO_1:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
            if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
            {
                int nIndex = m_pSearchOptionCombox[ID_NOMAL_STATS_COMBO_1 - ID_COMBOX_START]->GetSelectIndex();
                if ( nIndex == private_market::eItemBasicNone )
                {
                    m_pBasicStatsEditBox->ClearEdit(ID_NOMAL_STATS_EDITBOX_1);
                    m_pBasicStatsEditBox->SetEditString(ID_NOMAL_STATS_EDITBOX_1,SEARCH_MIN_VALUE);       
                }
            }
		}
		break;

	case ID_NOMAL_STATS_COMBO_2:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
            if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
            {
                int nIndex = m_pSearchOptionCombox[ID_NOMAL_STATS_COMBO_2 - ID_COMBOX_START]->GetSelectIndex();
                if ( nIndex == private_market::eItemBasicNone )
                {
                    m_pBasicStatsEditBox->ClearEdit(ID_NOMAL_STATS_EDITBOX_2);
                    m_pBasicStatsEditBox->SetEditString(ID_NOMAL_STATS_EDITBOX_2,SEARCH_MIN_VALUE);       
                }
            }
		}
		break;

	case ID_NOMAL_STATS_COMBO_3:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
            if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
            {
                int nIndex = m_pSearchOptionCombox[ID_NOMAL_STATS_COMBO_3 - ID_COMBOX_START]->GetSelectIndex();
                if ( nIndex == private_market::eItemBasicNone )
                {
                    m_pBasicStatsEditBox->ClearEdit(ID_NOMAL_STATS_EDITBOX_3);
                    m_pBasicStatsEditBox->SetEditString(ID_NOMAL_STATS_EDITBOX_3,SEARCH_MIN_VALUE);       
                }
            }
		}
		break;

	case ID_NOMAL_STATS_COMBO_4:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
            if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
            {
                int nIndex = m_pSearchOptionCombox[ID_NOMAL_STATS_COMBO_4 - ID_COMBOX_START]->GetSelectIndex();
                if ( nIndex == private_market::eItemBasicNone )
                {
                    m_pBasicStatsEditBox->ClearEdit(ID_NOMAL_STATS_EDITBOX_4);
                    m_pBasicStatsEditBox->SetEditString(ID_NOMAL_STATS_EDITBOX_4,SEARCH_MIN_VALUE);       
                }
            }
		}
		break;

	case ID_ADD_RANDOM_OPTION_COMBO_BG_1:
		{
			SetComBoxClickEnevt(  cID, dwMsg );

			if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
			{
				int nIndex = m_pSearchOptionCombox[ID_ADD_RANDOM_OPTION_COMBO_BG_1 - ID_COMBOX_START]->GetSelectIndex();
                switch ( nIndex )
                {
                case private_market::eItemStateBlow:
                    {
                        m_pSearchOptionCombox[ ID_BLOW_COMBOX_1 - ID_COMBOX_START ]->SetVisibleSingle(TRUE);
                        CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_1);
                        pEditBox->SetVisibleSingle(FALSE);
                    }
                    break;
                case private_market::eItemAddonVarRandomNone:
                    m_pAddRandomEditBox->ClearEdit(ID_ADD_RANDOM_EDITBOX_1);
                    m_pAddRandomEditBox->SetEditString(ID_ADD_RANDOM_EDITBOX_1, SEARCH_MIN_VALUE);
                default:
                    {
                        m_pSearchOptionCombox[ ID_BLOW_COMBOX_1 - ID_COMBOX_START ]->SetVisibleSingle(FALSE);
                        CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_1);
                        pEditBox->SetVisibleSingle(TRUE);
                    }
                    break;
                }
			}
		}
		break;

	case ID_ADD_RANDOM_OPTION_COMBO_BG_2:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
			if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
			{
				int nIndex = m_pSearchOptionCombox[ID_ADD_RANDOM_OPTION_COMBO_BG_2 - ID_COMBOX_START]->GetSelectIndex();
                switch ( nIndex )
                {
                case private_market::eItemStateBlow:
                    {
                        m_pSearchOptionCombox[ ID_BLOW_COMBOX_2 - ID_COMBOX_START ]->SetVisibleSingle(TRUE);
                        CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_2);
                        pEditBox->SetVisibleSingle(FALSE);
                    }
                    break;
                case private_market::eItemAddonVarRandomNone:
                    m_pAddRandomEditBox->ClearEdit(ID_ADD_RANDOM_EDITBOX_2);
                    m_pAddRandomEditBox->SetEditString(ID_ADD_RANDOM_EDITBOX_2, SEARCH_MIN_VALUE);
                default:
                    {
                        m_pSearchOptionCombox[ ID_BLOW_COMBOX_2 - ID_COMBOX_START ]->SetVisibleSingle(FALSE);
                        CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_2);
                        pEditBox->SetVisibleSingle(TRUE);
                    }
                    break;
                }
			}
		}
		break;

	case ID_ADD_RANDOM_OPTION_COMBO_BG_3:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
			if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
			{
				int nIndex = m_pSearchOptionCombox[ID_ADD_RANDOM_OPTION_COMBO_BG_3 - ID_COMBOX_START]->GetSelectIndex();
                switch ( nIndex )
                {
                case private_market::eItemStateBlow:
                    {
                        m_pSearchOptionCombox[ ID_BLOW_COMBOX_3 - ID_COMBOX_START ]->SetVisibleSingle(TRUE);
                        CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_3);
                        pEditBox->SetVisibleSingle(FALSE);
                     
                    }
                    break;
                case private_market::eItemAddonVarRandomNone:
                    m_pAddRandomEditBox->ClearEdit(ID_ADD_RANDOM_EDITBOX_3);
                    m_pAddRandomEditBox->SetEditString(ID_ADD_RANDOM_EDITBOX_3, SEARCH_MIN_VALUE);
                default:
                    {
                        m_pSearchOptionCombox[ ID_BLOW_COMBOX_3 - ID_COMBOX_START ]->SetVisibleSingle(FALSE);
                        CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_3);
                        pEditBox->SetVisibleSingle(TRUE);
                    }
                    break;
                }
			}
		}
		break;

	case ID_ADD_RANDOM_OPTION_COMBO_BG_4:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
			if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
			{
				int nIndex = m_pSearchOptionCombox[ID_ADD_RANDOM_OPTION_COMBO_BG_4 - ID_COMBOX_START]->GetSelectIndex();
                switch ( nIndex )
                {
                case private_market::eItemStateBlow:
                    {
                        m_pSearchOptionCombox[ ID_BLOW_COMBOX_4 - ID_COMBOX_START ]->SetVisibleSingle(TRUE);
                        CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_4);
                        pEditBox->SetVisibleSingle(FALSE);
                    }
                    break;
                case private_market::eItemAddonVarRandomNone:
                    m_pAddRandomEditBox->ClearEdit(ID_ADD_RANDOM_EDITBOX_4);
                    m_pAddRandomEditBox->SetEditString(ID_ADD_RANDOM_EDITBOX_4, SEARCH_MIN_VALUE);
                default:
                    {
                        m_pSearchOptionCombox[ ID_BLOW_COMBOX_4 - ID_COMBOX_START ]->SetVisibleSingle(FALSE);
                        CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_4);
                        pEditBox->SetVisibleSingle(TRUE);
                    }
                    break;
                }
			}
		}
		break;

	case ID_BLOW_COMBOX_1:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
		}
		break;

	case ID_BLOW_COMBOX_2:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
		}
		break;

	case ID_BLOW_COMBOX_3:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
		}
		break;

	case ID_BLOW_COMBOX_4:
		{
			SetComBoxClickEnevt(  cID, dwMsg );
		}
		break;

	case ID_ITEM_SEARCH_EDITBOX_1_BG:
		{
			if ( CHECK_MOUSE_IN_LBDOWNLIKE(dwMsg) )
			{
				CString strTemp1 = m_pItemSearchEditBox->GetEditString(ID_ITEM_SEARCH_EDITBOX_1);
				CString strTemp2 = ITEM_NAME_INPUT;
				if ( strTemp1 == strTemp2 )
					m_pItemSearchEditBox->ClearEdit(ID_ITEM_SEARCH_EDITBOX_1);
			}
		}
		break;

	case ID_ITEM_SEARCH_EDITBOX:
		{
			if( m_pItemSearchEditBox->GetEditLength( ID_ITEM_SEARCH_EDITBOX_1 ) != 0 )
			{
				if( DxInputDevice::GetInstance().IsUpdatKeyState() )
				{
					if( UIKeyCheck::GetInstance()->Check( DIK_RETURN, DXKEY_DOWN )  )
					{
						//ComboInit();
						m_pComboRollSearchOver->SetVisibleSingle(FALSE);
					}else if( UIKeyCheck::GetInstance()->Check( DIK_PGUP, DXKEY_DOWN )  )
					{
						if( m_nPageNum > 0 ) 
						{
							// 이전 페이지로 이동
							m_nPageNum--;
							m_nSelectString = 0;

							UpdateComoboItem();

							m_pItemSearchEditBox->SetEditString( ID_ITEM_SEARCH_EDITBOX_1, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
							m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
						}
					}else if( UIKeyCheck::GetInstance()->Check( DIK_PGDN, DXKEY_DOWN )  )
					{
						if(((m_nPageNum+1)*MAX_ROLLOVER_TEXT) < (int)m_vecString.size()) 
						{
							m_nPageNum++;
							m_nSelectString = 0;

							UpdateComoboItem();

							m_pItemSearchEditBox->SetEditString( ID_ITEM_SEARCH_EDITBOX_1, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
							m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
						}

					}else if( UIKeyCheck::GetInstance()->Check( DIK_UP, DXKEY_DOWN )  )
					{
						if(m_nSelectString > 0) 
						{
							m_nSelectString--;
							m_pItemSearchEditBox->SetEditString( ID_ITEM_SEARCH_EDITBOX_1, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
							m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
						}else if( m_nPageNum > 0 ) 
						{
							// 이전 페이지로 이동
							m_nPageNum--;
							m_nSelectString = MAX_ROLLOVER_TEXT-1;

							UpdateComoboItem();

							m_pItemSearchEditBox->SetEditString( ID_ITEM_SEARCH_EDITBOX_1, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
							m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
						}
					}else if( UIKeyCheck::GetInstance()->Check( DIK_DOWN, DXKEY_DOWN )  )
					{
						if(m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT) < (int)m_vecString.size()-1) 
						{
							if( m_nSelectString < MAX_ROLLOVER_TEXT-1 )
							{
								m_nSelectString++;
								m_pItemSearchEditBox->SetEditString( ID_ITEM_SEARCH_EDITBOX_1, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
								m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
							}else{
								// 이후 페이지로 이동
								m_nPageNum++;
								m_nSelectString = 0;

								UpdateComoboItem();

								m_pItemSearchEditBox->SetEditString( ID_ITEM_SEARCH_EDITBOX_1, m_vecString[m_nSelectString+(m_nPageNum*MAX_ROLLOVER_TEXT)] );
								m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
							}
						}
					}else{
						CString strItemName = m_pItemSearchEditBox->GetEditString( ID_ITEM_SEARCH_EDITBOX_1 );

						int nStrLength = strItemName.GetLength();

						//if( strItemName.GetLength() != m_nBeforeStrLength )
						{
							if( m_nBeforeStrLength < strItemName.GetLength() && m_vecString.size() != 0 )
							{

								std::vector<CString> vecString = m_vecString;

								m_vecString.clear();
								m_nSelectString = -1;
								m_nPageNum      = 0;

								for( size_t i = 0; i < vecString.size(); i++ )
								{
									if( vecString[i].Find( strItemName.GetString() ) != -1 )
									{
										m_vecString.push_back( vecString[i] );
									}
								}
							}
							else
							{
								m_vecString.clear();
								m_nSelectString = -1;
								m_nPageNum      = 0;

								// GLItemMan::m_vecSrcString 에서 strItemName 과 동일한 이름이 있는 아이템 이름을 vecOut 에 넘겨준다.
								GLItemMan::GetInstance().GetList_SameWord( m_vecString, strItemName );
							}
							UpdateComoboItem();
							if( m_vecString.size() != 0 ) m_nBeforeStrLength = strItemName.GetLength();

						}
					}
				}
			}else{
				ComboInit();
			}
		}
		break;

	case ID_ITEM_SEARCH_COMBOX:
		{
			if (CHECK_MOUSE_IN(dwMsg))
			{
				m_RollOverID = ID_ITEM_SEARCH_COMBOX;
				m_bFirstLBUP = TRUE;
			}
			else
			{
				m_RollOverID = NO_ID;
			}

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
				m_pItemSearchEditBox->SetEditString( ID_ITEM_SEARCH_EDITBOX_1, strTemp );
				m_pItemSearchEditBox->EndEdit();
				ComboInit();
				m_RollOverID = NO_ID;
			}
		}
		break;

	case ID_ROW1:
	case ID_ROW2:
	case ID_ROW3:
	case ID_ROW4:
	case ID_ROW5:
	case ID_ROW6:
	case ID_ROW7:
	case ID_ROW8:
		{
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( dwMsg & UIMSG_LB_UP )
                {
                    //검색한 개인상점 판매 목록에서 구매할 목록 행을 선택하는 부분
                    const int nRowNumber = cID  - ID_ROW_START;
                    UIRECT rcRowGlobalPos = m_pSearchRow[ nRowNumber ]->GetGlobalPos();
                    UIRECT rcRowLocalPos = m_pSearchRow[ nRowNumber ]->GetLocalPos();
                    UIRECT rcSelectGlobalPos = m_pSelectLine->GetGlobalPos();

                    if ( ( m_pSelectLine->IsVisible() )                 //선택된 행이 있고
                        && ( nRowNumber == m_nSelectRow )               //똑같은 행을 선택했고
                        && ( !( dwMsg & UIMSG_MESSAGEBOX_POSITIVE ) ) ) //수량 에디트박스를 선택한것이 아닐때
                    {
                        //행 선택해제
                        m_nSelectPage = NULL_PAGE;
                        m_nSelectRow = NULL_ROW;
                        m_pSelectLine->SetVisibleSingle(FALSE);
                    }
                    else
                    {
                        //행 선택
                        m_pSelectLine->SetGlobalPos(D3DXVECTOR2(rcRowGlobalPos.left,rcRowGlobalPos.top));
                        m_pSelectLine->SetLocalPos(D3DXVECTOR2(rcRowLocalPos.left,rcRowLocalPos.top));
                        m_nSelectPage = m_pPagenavigation->GetIndex();
                        m_nSelectRow = nRowNumber;
                        m_pSelectLine->SetVisibleSingle(TRUE);
                    }
                }
            }
		}
		break;
	case ID_SEARCH_OPTION_RESET_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					BOOST_FOREACH( CSmartComboBoxBasicEx* vecSearchOption, m_pSearchOptionCombox )
					{
						vecSearchOption->SetSelectIndex(0);
					}
					ResetSearchOption();
				}
			}

		}
		break;
	case ID_ITEM_SEARCH_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{

				//SetVisibleFALSESearchRow();

                //재검색 제한시간
				if ( m_pGaeaClient->GetRemainSearchLimitTime() )
				{
                    break;
				}

				m_pPagenavigation->SetPage(0);
				m_pPagenavigation->SetCountPage(0);
				m_nSelectPage = NULL_PAGE;

				// 선택된 열 취소 함수로 옮길예정
				m_nSelectRow = NULL_ROW;
				m_pSelectLine->SetVisibleSingle(FALSE);

                // 검색 결과 대기 요청 UI출력
                SetPageWaiting(TRUE);

				if ( dwMsg & UIMSG_LB_UP )
				{
					for (unsigned i = 0; i< m_pSearchOptionCombox.size(); i++ )
					{
						int nIndex;
						nIndex = m_pSearchOptionCombox[i]->GetSelectIndex();
					}
					m_vecDataPage.clear();
					m_pGaeaClient->GetPrivateMarketClient()->ClearSearchItemdate();

					/*if ( IsItemName() )
					{
						CString str = GetItemNameSearch();
						// Convert a TCHAR string to a LPCSTR
						CT2CA pszConvertedAnsiString(str);
						// Construct a std::string using the LPCSTR input
						std::string s(pszConvertedAnsiString);

						m_sName.strItemName = s;
						m_pGaeaClient->GetPrivateMarketClient()->SetSearchData(GetItemNameSearch());
						m_pGaeaClient->ReqPrivateMarketItemSearchByName(&m_sName);
					}
					else
					*/
					{
						CString str = "";

						if( IsItemName() )
						{
							str = GetItemNameSearch();
						}

						m_sBasic = m_pGaeaClient->GetPrivateMarketClient()->SetSearchData(  str,
																							GetItemType(),
																							GetTimeLimit(),
																							GetItemTurnNum(),
																							GetWeaponType(),
																							GetArmorType(),
																							GetItemStatsType(),
																							GetItemStatsValue(),
																							GetItemLevelCategory(),
																							GetClassCategory(),
																							GetSexCategory(),
																							GetNomalStatsCategoryType_1(),
																							GetNomalStatsCategoryValue_1(),
																							GetNomalStatsCategoryType_2(),
																							GetNomalStatsCategoryValue_2(),
																							GetNomalStatsCategoryType_3(),
																							GetNomalStatsCategoryValue_3(),
																							GetNomalStatsCategoryType_4(),
																							GetNomalStatsCategoryValue_4(),
																							GetAddRandomOptionCategoryType_1(),
																							GetAddRandomOptionCategoryValue_1(),
																							GetAddRandomOptionCategoryType_2(),
																							GetAddRandomOptionCategoryValue_2(),
																							GetAddRandomOptionCategoryType_3(),
																							GetAddRandomOptionCategoryValue_3(),
																							GetAddRandomOptionCategoryType_4(),
																							GetAddRandomOptionCategoryValue_4(),
																							GetSearchSellType() );
						m_pGaeaClient->ReqPrivateMarketItemSearch(&m_sBasic);
					}
				}

				RefreshSearchResult();
			}
		}
		break;
	case ID_ADD_WHISH_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				if ( m_nSelectRow != NULL_PAGE && m_nSelectPage != NULL_ROW)
				{
					if (m_pSearchRow[m_nSelectRow]->IsVisible())
					{
						bool b_Ok = m_pGaeaClient->GetPrivateMarketClient()->SetWhishPageData( m_pSearchRow[m_nSelectRow]->GetsSaleItem());

						if ( b_Ok )
						{
							m_pInterface->SetWhishPageEvent(true);
						}
						
					}

				}	
			}
		}
		break;

	case  ID_BUY_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				if (m_nSelectRow == NULL_ROW)
					break;

                if ( m_pBuyMessageBox->IsVisible() )
                {
                    m_pBuyMessageBox->CloseMessageBox();
                    break;
                }
                const int nBuyCnt = m_pSearchRow[m_nSelectRow]->GetItemCount();
                if ( nBuyCnt <= 0)
                {
                    break;
                }
                const LONGLONG llPrice = m_pSearchRow[m_nSelectRow]->GetPrice();
                const SITEMCUSTOM sItemCustom = m_pSearchRow[m_nSelectRow]->GetItemData();
                std::string str = sItemCustom.GETNAME();
                std::string strCombine = "";

				int nSellType = m_pSearchRow[m_nSelectRow]->GetsSaleItem()->nSellType;




				std::vector<std::string> vecString;

				vecString.push_back( ID2GAMEWORD( "MARKET_SEARCH_BUY_CONFIRM", 0 ) );
				vecString.push_back( sc::string::format( ID2GAMEWORD("MARKET_SEARCH_BUY_CONFIRM", 1), str, nBuyCnt ) );

				
				if ( nSellType == private_market::eSellType_ConsignmentSale_RanPoint )
					vecString.push_back( sc::string::format( ID2GAMEWORD("MARKET_SEARCH_BUY_CONFIRM", 3), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llPrice*nBuyCnt, 3, "," ) ) );
				else
					vecString.push_back( sc::string::format( ID2GAMEWORD("MARKET_SEARCH_BUY_CONFIRM", 2), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llPrice*nBuyCnt, 3, "," ) ) );

                //구매 확인창
                m_pBuyMessageBox->OpenMessageBoxMultiLine( CMessageBox::TITLE_QUESTION, vecString, CMessageBox::TYPE_YESNO, ID_BUY_BUTTON );				
			}
		}
		break;

    case ID_BUY_MESSAGEBOX:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
            {
                if (dwMsg & UIMSG_MESSAGEBOX_POSITIVE)
                {
                    if (m_nSelectRow == NULL_ROW)
                        break;
                    int nIdentifier = m_pBuyMessageBox->GetIdentifier();
                    switch (nIdentifier)
                    {
                    case ID_BUY_BUTTON:
                        {
                            private_market::sPurchaseQueue sPurchaseQueue;
                            sPurchaseQueue.sItemGUID = m_pSearchRow[m_nSelectRow]->GetUUID();
                            sPurchaseQueue.dwItemTurnNum = m_pSearchRow[m_nSelectRow]->GetItemCount();
                            sPurchaseQueue.llPrice = m_pSearchRow[m_nSelectRow]->GetPrice();

                            std::vector< private_market::sPurchaseQueue > vecPurchase;
                            vecPurchase.push_back(sPurchaseQueue);
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
			////if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			//{
			//	//페이지 이동시 리셋될 내용 들
			//	if ( m_pPagenavigation->IsClickEvent())
			//	{
			//		BOOST_FOREACH (MarketSearchRow* vecSearchRow, m_pSearchRow)
			//		{
			//			m_pSelectLine->SetVisibleSingle(FALSE);
			//			vecSearchRow->SetCheckOff();
			//		}
			//	}
			//}

			//if ( m_pPagenavigation->IsClickEvent())
			//{
			//	RefreshSearchResult();
			//}

		}
		break;
	default:
		{
			PagenavigationUpdate();

			if ( m_pPagenavigation->IsClickEvent())
			{
				SetPageWaiting(TRUE);
				SetVisibleFALSESearchRow();
				m_sBasic.wPage = m_pPagenavigation->GetPageIndex() + 1;

				HRESULT hr;

				if ( m_pGaeaClient->GetPrivateMarketClient()->GetSearchItemName() != "" )
				{
					CString str = m_pGaeaClient->GetPrivateMarketClient()->GetSearchItemName();
					// Convert a TCHAR string to a LPCSTR
					CT2CA pszConvertedAnsiString(str);
					// Construct a std::string using the LPCSTR input
					std::string s(pszConvertedAnsiString);

					m_sName.strItemName = s;
					m_sName.wPage = m_pPagenavigation->GetPageIndex() + 1;
					hr = m_pGaeaClient->ReqPrivateMarketItemSearchByName(&m_sName);
				}
				else
					hr = m_pGaeaClient->ReqPrivateMarketItemSearch(&m_sBasic);

				if ( E_FAIL == hr )
				{
					m_pWaitingBox->SetVisibleSingle(TRUE);
				}
				m_pSelectLine->SetVisibleSingle(FALSE);
			}

			if ( m_bPageWaiting )
			{
				if ( m_pGaeaClient->GetPrivateMarketClient()->GetSearchItemName() != "" )
				{
					CString str = m_pGaeaClient->GetPrivateMarketClient()->GetSearchItemName();
					// Convert a TCHAR string to a LPCSTR
					CT2CA pszConvertedAnsiString(str);
					// Construct a std::string using the LPCSTR input
					std::string s(pszConvertedAnsiString);

					m_sName.strItemName = s;
					m_sName.wPage = m_pPagenavigation->GetPageIndex() + 1;
					m_pGaeaClient->ReqPrivateMarketItemSearchByName(&m_sName);
				}
				else
				{
					m_sBasic.wPage = m_pPagenavigation->GetPageIndex() + 1;
					m_pGaeaClient->ReqPrivateMarketItemSearch(&m_sBasic);
				}

			}

			if ( !m_pItemSearchEditBox->GetEditBox(ID_ITEM_SEARCH_EDITBOX_1)->IsBegin() )
			{

				if ( m_pItemSearchEditBox->GetEditString(ID_ITEM_SEARCH_EDITBOX_1).GetLength() == 0 )
					m_pItemSearchEditBox->SetEditString(ID_ITEM_SEARCH_EDITBOX_1, ITEM_NAME_INPUT);
			}

			for ( int i = 0; i <= ID_NOMAL_STATS_EDITBOX_4 - ID_NOMAL_STATS_EDITBOX_1; i++)
			{
				if ( !m_pBasicStatsEditBox->GetEditBox(ID_NOMAL_STATS_EDITBOX_1 + i)->IsBegin() )
				{
					if ( m_pBasicStatsEditBox->GetEditString(ID_NOMAL_STATS_EDITBOX_1 + i).GetLength() == 0 )
						m_pBasicStatsEditBox->SetEditString(ID_NOMAL_STATS_EDITBOX_1 + i, SEARCH_MIN_VALUE);
				}

			}

			for ( int i = 0; i <= ID_ADD_RANDOM_EDITBOX_4 - ID_ADD_RANDOM_EDITBOX_1; i++)
			{
				if ( !m_pAddRandomEditBox->GetEditBox(ID_ADD_RANDOM_EDITBOX_1 + i)->IsBegin() )
				{
					if ( m_pAddRandomEditBox->GetEditString(ID_ADD_RANDOM_EDITBOX_1 + i).GetLength() == 0 )
						m_pAddRandomEditBox->SetEditString(ID_ADD_RANDOM_EDITBOX_1 + i, SEARCH_MIN_VALUE);
				}
			}

	
			if ( !m_pItemStatsEditBox->GetEditBox(ID_BASIC_STATS_1)->IsBegin() )
			{
				if ( m_pItemStatsEditBox->GetEditString(ID_BASIC_STATS_1).GetLength() == 0 )
				m_pItemStatsEditBox->SetEditString(ID_BASIC_STATS_1,SEARCH_MIN_VALUE);
			}
			
			if ( !m_pBasicItemTurnNumEditBox->GetEditBox(ID_BASIC_ITEM_TURN_1)->IsBegin() )
			{
				if ( m_pBasicItemTurnNumEditBox->GetEditString(ID_BASIC_ITEM_TURN_1).GetLength() == 0 )
					m_pBasicItemTurnNumEditBox->SetEditString(ID_BASIC_ITEM_TURN_1,SEARCH_MIN_VALUE);
			}

			if ( CHECK_RB_UP_LIKE ( dwMsg ) || CHECK_RB_DOWN_LIKE ( dwMsg ) )
			{
				m_pSelectLine->SetVisibleSingle(FALSE);
				m_pComboRollSearchOver->SetVisibleSingle(FALSE);
				//CloseAllRollOver();
			}

			if ( CHECK_LB_UP_LIKE ( dwMsg ) || CHECK_LB_DOWN_LIKE ( dwMsg ) )
			{
				m_pComboRollSearchOver->SetVisibleSingle(FALSE);
				//CloseAllRollOver();
			}
		}
	}
}

void MarketSearchPage::SetRollOverCategory( std::vector<UI_OPTION> UiOption, CSmartComboBoxBasicEx* pComBox ) 
{
	//unsigned nCount = pComBox->GetCount(); 
	//for( unsigned i = 0; i<UiOption.size(); i ++)
	//{
	//	pComBox->SetRollOverAddText(UiOption[i].OptionName().c_str(),nCount + i);
	//}
}

void MarketSearchPage::SetRollOverCategory( std::string* strComment,int nSize, CSmartComboBoxBasicEx* pComBox ) 
{
	pComBox->ClearText();
	for( int i = 0; i< nSize; i ++)
	{
		pComBox->SetRollOverAddText(strComment[i].c_str(), i);
	}
}
//
///*기존 LoadSearchFile()은 Glogic.rcc 파일 내부의 SerchItem.txt 파일을 사용하였는데
//기획팀의 요청으로 Item.isf파일 내의 검색가능 옵션이 있는 아이템만 목록으로 불러오는 함수*/
//void MarketSearchPage::LoadSearchFileFromITEM()
//{
//
//	std::string FullPath(GLOGIC::GetPath());
//	FullPath.append(GLogicData::GetInstance().GetItemFileName());
//
//	std::auto_ptr<sc::BaseStream> pBStream(
//		GLOGIC::openfile_basestream(
//		GLOGIC::UseLogicZipFile(), 
//		GLogicData::GetInstance().GetGlogicZipFile(),
//		FullPath.c_str(),
//		GLogicData::GetInstance().GetItemFileName(),
//		false,
//		GLOGIC::UseLogicPackFile()));
//
//	if (!pBStream.get())
//	{
//		sc::writeLogError(
//			sc::string::format(
//			"GLItemMan Load file %1% %2% %3%",
//			GLogicData::GetInstance().GetGlogicZipFile(),
//			FullPath,
//			GLogicData::GetInstance().GetItemFileName()));
//		return;
//	}
//
//	sc::BaseStream& SFile = *pBStream;
//
//	DWORD dwFILEVER;
//	char szFILETYPE[_MAX_FNAME];
//	SFile.GetFileType( szFILETYPE, _MAX_FNAME, dwFILEVER );
//	SFile.SetEncode ( (dwFILEVER>=GLItemMan::ENCODE_VER) );
//	
//	DWORD dwNum=0;
//
//	SFile >> dwNum;
//	
//	for (DWORD i=0; i<dwNum; i++)
//	{
//		SITEM sItem;
//
//		if ( !sItem.LoadFile(SFile) )
//		{
//			AfxMessageBox( "SITEM Loading 에 문제가 발생하였습니다." );
//			return;
//		}
//	
//		if( sItem.sBasicOp.bSearch )
//			m_listSrcString.push_back( CString(sItem.GetName()) ); 
//	}
//		
//}
//
//void MarketSearchPage::LoadSearchFile()
//{
//	std::string strPath;
//	strPath = GLOGIC::GetPath();
//	strPath += GLItemMan::_SEARCHLIST;
//
//	std::auto_ptr<sc::CBaseString> pBString(
//		GLOGIC::openfile_basestring(
//		GLOGIC::UseLogicZipFile(), 
//		GLogicData::GetInstance().GetGlogicZipFile(), 
//		strPath.c_str(), 
//		GLItemMan::_SEARCHLIST, 
//		true,
//		GLOGIC::UseLogicPackFile()));
//
//	if( !pBString.get() )
//	{
//		sc::writeLogError(
//			sc::string::format(
//			"GLStringTable::LOADFILE File Open %1%", GLItemMan::_SEARCHLIST));
//		return;
//	}
//
//	sc::CBaseString &cBString = *pBString;
//
//	CString resToken, strBUFFER;
//	CString strToken("\t");
//
//	SNATIVEID nId;
//
//	int strMax = 0;
//
//	while ( cBString.GetNextLine(strBUFFER) > 0 )
//	{
//		int curPos(0);
//		resToken = strBUFFER.Tokenize(strToken.GetString(), curPos);
//		m_listSrcString.push_back( resToken ); 
//
//	}
//}

void MarketSearchPage::UpdateComoboItem()
{
	if( m_vecString.size() != 0 && 	m_pItemSearchEditBox->GetEditBox(ID_ITEM_SEARCH_EDITBOX_1)->IsBegin())
	{
		m_pComboRollSearchOver->SetVisibleSingle(TRUE);
		m_pComboRollSearchOver->ClearText();
		size_t count = 0;
		for( size_t i = (m_nPageNum*MAX_ROLLOVER_TEXT); i < m_vecString.size(); i++ )
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
	}
}

void MarketSearchPage::ComboInit()
{
	m_pComboRollSearchOver->SetVisibleSingle(FALSE);
	m_pComboRollSearchOver->ClearText();
	m_vecString.clear();
	m_nBeforeStrLength = 0;
	m_nSelectString = -1;
	m_nPageNum      = 0;
}

void MarketSearchPage::SetItemImage( int nIndex, const SITEMCUSTOM& sItemCustom, WORD wTurnNum )
{
	m_pSearchRow[nIndex]->SetItemImage(sItemCustom, wTurnNum);
}

void MarketSearchPage::IsUseItem( int nIndex, const SITEMCUSTOM& sItemCustom )
{
	m_pSearchRow[nIndex]->IsUseItem(sItemCustom);
}

void MarketSearchPage::SetItemName( int nIndex, const SITEMCUSTOM& sItemCustom )
{
	m_pSearchRow[nIndex]->SetItemName(sItemCustom);
}

void MarketSearchPage::SetTitemSellerName( int nIndex, const CString& strText )
{
	m_pSearchRow[nIndex]->SetTitemSellerName(strText);
}

// void MarketSearchPage::SetSellType( int nIndex, private_market::eSellType eSellType )
// {
// 	m_pSearchRow[nIndex]->SetSellType(eSellType);
// }

void MarketSearchPage::SetTitemSellerPos()
{

}

void MarketSearchPage::SetItemPrice( int nIndex, LONGLONG lnPrice, private_market::eSellType eSellType  )
{
	m_pSearchRow[nIndex]->SetItemPrice(lnPrice, eSellType);
}

void MarketSearchPage::SetItemCount()
{

}


//성별 선택 콤보박스 해당 이미지
void MarketSearchPage::SetClassSexTexture()
{
	int nSex = //인덱스를 가지고 인덱스의 해당하는 값을 가져오는 함수 필요
		m_pSearchOptionCombox[ID_BASIC_SEX_CATEGORY_COMBO - ID_COMBOX_START]->GetSelectIndex();
	EMCLASSINDEX emclassIndex =  //인덱스를 가지고 인덱스의 해당하는 값을 가져오는 함수 필요
		static_cast<EMCLASSINDEX>(
		m_pSearchOptionCombox[ID_BASIC_CLASS_CATEGORY_COMBO - ID_COMBOX_START]->GetSelectIndex() - 1);
	
	EMCHARCLASS emCharClass = CharSexandClassIndexToCharClass( nSex, 
															   emclassIndex );
	std::string strTexterName;

	switch (emCharClass)
	{
	case GLCC_FIGHTER_M:
		strTexterName = "PRIVATE_MARKET_BRAWLER_MALE_ON";
		break;
	case GLCC_FIGHTER_W:
		strTexterName = "PRIVATE_MARKET_BRAWLER_FEMALE_ON";
		break;
	case GLCC_ARMS_M:
		strTexterName = "PRIVATE_MARKET_SWORDSMAN_MALE_ON";
		break;
	case GLCC_ARMS_W:
		strTexterName = "PRIVATE_MARKET_SWORDSMAN_FEMALE_ON";
		break;
	case GLCC_ARCHER_M:
		strTexterName = "PRIVATE_MARKET_ARCHER_MALE_ON";
		break;
	case GLCC_ARCHER_W:
		strTexterName = "PRIVATE_MARKET_ARCHER_FEMALE_ON";
		break;
	case GLCC_SPIRIT_M:
		strTexterName = "PRIVATE_MARKET_SHAMAN_MALE_ON";
		break;
	case GLCC_SPIRIT_W:
		strTexterName = "PRIVATE_MARKET_SHAMAN_FEMALE_ON";
		break;
	case GLCC_EXTREME_M:
		strTexterName = "PRIVATE_MARKET_EXTREME_MALE_ON";
		break;
	case GLCC_EXTREME_W:
		strTexterName = "PRIVATE_MARKET_EXTREME_FEMALE_ON";
		break;
	case GLCC_SCIENTIST_M:
		strTexterName = "PRIVATE_MARKET_SCIENTIST_MALE_ON";
		break;
	case GLCC_SCIENTIST_W:
		strTexterName = "PRIVATE_MARKET_SCIENTIST_FEMALE_ON";
		break;
	case GLCC_ASSASSIN_M:
		strTexterName = "PRIVATE_MARKET_ASSASSIN_MALE_ON";
		break;
	case GLCC_ASSASSIN_W:
		strTexterName = "PRIVATE_MARKET_ASSASSIN_FEMALE_ON";
		break;

	case GLCC_TRICKER_M:
		strTexterName = "PRIVATE_MARKET_TRICKER_MALE_ON";
		break;
	case GLCC_TRICKER_W:
		strTexterName = "PRIVATE_MARKET_TRICKER_FEMALE_ON";
		break;

	case GLCC_FIGHTER:
		strTexterName = "PRIVATE_MARKET_BRAWLER_ALL";
		break;
	case GLCC_ARMS:
		strTexterName = "PRIVATE_MARKET_SWORDSMAN_ALL";
		break;
	case GLCC_ARCHER:
		strTexterName = "PRIVATE_MARKET_ARCHER_ALL";
		break;
	case GLCC_SPIRIT:
		strTexterName = "PRIVATE_MARKET_SHAMAN_ALL";
		break;
	case GLCC_EXTREME:
		strTexterName = "PRIVATE_MARKET_EXTREME_ALL";
		break;
	case GLCC_SCIENTIST:
		strTexterName = "PRIVATE_MARKET_SCIENTIST_ALL";
		break;
	case GLCC_ASSASSIN:
		strTexterName = "PRIVATE_MARKET_ASSASSIN_ALL";
		break;
	case GLCC_TRICKER:
		strTexterName = "PRIVATE_MARKET_TRICKER_ALL";
		break;
	case GLCC_MAN:
		strTexterName = "PRIVATE_MARKET_MALE_ALL";
		break;
	case GLCC_WOMAN:
		strTexterName = "PRIVATE_MARKET_FEMALE_ALL";
		break;
	case GLCC_ALL_NEWSEX:
		strTexterName = "PRIVATE_MARKET_CHARACTER_ALL";
		break;

	default:
		strTexterName.clear();
		break;
	};
	if (strTexterName.length())
		m_pClassImage->SetImage(strTexterName.c_str());
}

void MarketSearchPage::SetVisibleSingleNomalStats( BOOL bBool )
{
	//일반능력치 관련 ON,OFF
}

void MarketSearchPage::SetVisibleSingleAddRandomOption( BOOL bBool )
{
	//가산및 랜덤 옵션 관련 ON, OFF
}

void MarketSearchPage::SetComBoxClickEnevt( UIGUID ControlID, DWORD dwMsg )
{
	CUIControl* pControl = m_ControlContainer.FindControl ( ControlID );
	if ( !pControl )
	{
		GASSERT ( 0 && "심각한 오류, 노드가 널입니다." );
		return ;
	}

	CSmartComboBoxBasicEx* pCombox = static_cast<CSmartComboBoxBasicEx*>(pControl);

	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		if (pCombox->IsVisibleComboxRoolOver())
		{
			m_RollOverID = ControlID;
			m_bFirstLBUP = TRUE;
		}
		else
		{
			m_RollOverID = NO_ID;
			m_bFirstLBUP = FALSE;
		}
	}
	else
	{
		m_bFirstLBUP = FALSE;
	}
}

void MarketSearchPage::SetVisibleFALSESearchRow()
{
	for (unsigned i = 0; i < m_pSearchRow.size(); i++)
	{
		m_pSearchRow[i]->ItemCoutntEditBoxClear();
		m_pSearchRow[i]->SetVisibleSingle(FALSE);
	}
}

void MarketSearchPage::GetPageData()
{
	m_vecDataPage.clear();
	m_vecDataPage = m_pGaeaClient->GetPrivateMarketClient()->GetSearchResultData();
}

void MarketSearchPage::SetSearchRow()
{
    const unsigned int iNumItem = m_vecDataPage.size();
    const bool bExistSearchItem = iNumItem ? FALSE : TRUE;
    m_pNoExistResultBox->SetVisibleSingle(bExistSearchItem);
	for ( unsigned i = 0; i < m_vecDataPage.size(); i ++ )
	{
		
        //__time64_t tCurTime = CTime::GetCurrentTime().GetTime();
		__time64_t tCurTime = m_pGaeaClient->GetCurrentTime().GetTime();
        __time64_t tSpanTime = m_vecDataPage[i]->ExpirationDate - tCurTime;

        int nHour;
        nHour = static_cast<int> (tSpanTime / 3600);


        std::string strTemp = sc::string::format( ID2GAMEWORD("MARKET_RPPAGE_TEXT", 22), nHour );


        DWORD dwOriginalColor = D3DCOLOR_ARGB( 255, 0, 128, 192 );

        float fRate = nHour / 10.0f;
        if( 1.0f < fRate )
            fRate = 1.0f;

        BYTE byA = static_cast<BYTE>(((dwOriginalColor)&0xFF000000)>>24);
        BYTE byR = static_cast<BYTE>(((dwOriginalColor)&0x00FF0000)>>16);
        BYTE byG = static_cast<BYTE>(((dwOriginalColor)&0x0000FF00)>>8);
        BYTE byB = static_cast<BYTE>(((dwOriginalColor)&0x000000FF));

        //byR = static_cast<BYTE>( byR * fRate );
        byR = static_cast<BYTE>( byR + 0xFF * ( 1 - fRate ) );
        byG = static_cast<BYTE>( byG * fRate );
        byB = static_cast<BYTE>( byB * fRate );


        if( static_cast<private_market::eSellType>(m_vecDataPage[i]->nSellType) != private_market::eSellType_PrivateMarket )
            SetMapName(i, strTemp.c_str(), D3DCOLOR_ARGB( byA, byR, byG, byB ) );    
        else
            SetMapName(i,m_pGaeaClient->GetMapName(m_vecDataPage[i]->sSellerMapID), NS_UITEXTCOLOR::DEFAULT );    
		
		SetsSaleItem(i,m_vecDataPage[i]);
		SetUUID(i,m_vecDataPage[i]->sItemGUID);
		SetItemImage(i,m_vecDataPage[i]->sItemcustom, static_cast<WORD>(m_vecDataPage[i]->dwItemTurnNum));
		SetItemName(i,m_vecDataPage[i]->sItemcustom);
		IsUseItem(i,m_vecDataPage[i]->sItemcustom);
		SetItemPrice(i,m_vecDataPage[i]->llItemPrice, static_cast<private_market::eSellType>(m_vecDataPage[i]->nSellType) );
		SetTitemSellerName(i,m_vecDataPage[i]->szSellerName);
        //SetSellType(i,static_cast<private_market::eSellType>(m_vecDataPage[i]->nSellType));
        //SetTitemSellerName(i,m_vecDataPage[i]->szSellerName);
		m_pSearchRow[i]->SetVisibleSingle(TRUE);
	}
}

void MarketSearchPage::PagenavigationUpdate()
{
	m_pPagenavigation->PagenavigationUpdate(m_pGaeaClient->GetPrivateMarketClient()->GetSearchPageCount());
}

void MarketSearchPage::RefreshSearchResult()
{
	// 모든 검색내용의 창을 VisibleOff 한다
	SetVisibleFALSESearchRow();

	//현재 페이지에 출력할 아이템 데이터를 가져 온다
	GetPageData();

	//위에서 가져온 데이터를 UI에 출력한다.
	SetSearchRow();

	//페이지 갯수를 기준으로 페이지 네비게이션를 업데이트 한다.
	PagenavigationUpdate();
}

CString MarketSearchPage::GetItemNameSearch()
{
	return m_pItemSearchEditBox->GetEditString(ID_ITEM_SEARCH_EDITBOX_1);
}

int MarketSearchPage::GetItemType()
{
	int nComboxIndex = static_cast<int>(ID_ITEM_CATEGORY_COMBO - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

BOOL MarketSearchPage::GetTimeLimit()
{
	return m_pLimiteItemCheckBox->IsChecked();
}

int MarketSearchPage::GetWeaponType()
{
	// 무기타입 방어구 타입 분할 필요
	int nIndex = m_pSearchOptionCombox[ID_ITEM_CATEGORY_COMBO - ID_COMBOX_START]->GetSelectIndex();
	if ( nIndex == 1)
	{
		int nComboxIndex = static_cast<int>(ID_BASIC_WEAR_CATEGORY_COMBO - ID_COMBOX_START);
		int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

		if ( nType < 0 && nType >nComboxIndex)
			return 0;
		else
			return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
	}
	else
		return 0;
}

int MarketSearchPage::GetArmorType()
{
	// 무기타입 방어구 타입 분할 필요
	int nIndex = m_pSearchOptionCombox[ID_ITEM_CATEGORY_COMBO - ID_COMBOX_START]->GetSelectIndex();
	if ( nIndex == 2)
	{
		int nComboxIndex = static_cast<int>(ID_BASIC_WEAR_CATEGORY_COMBO - ID_COMBOX_START);
		int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

		if ( nType < 0 && nType >nComboxIndex)
			return 0;
		else
			return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
	}
	else
		return 0;
}

int MarketSearchPage::GetItemStatsType()
{
	int nComboxIndex = static_cast<int>(ID_BASIC_STATS_CATEGORY_COMBO - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

int MarketSearchPage::GetItemStatsValue()
{
	return atoi( m_pItemStatsEditBox->GetEditString(ID_BASIC_STATS_1));
}

int MarketSearchPage::GetItemLevelCategory()
{
	int nComboxIndex = static_cast<int>(ID_BASIC_LEVEL_CATEGORY_COMBO - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

int MarketSearchPage::GetClassCategory()
{
	int nComboxIndex = static_cast<int>(ID_BASIC_CLASS_CATEGORY_COMBO - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

int MarketSearchPage::GetSexCategory()
{
	int nComboxIndex = static_cast<int>(ID_BASIC_SEX_CATEGORY_COMBO - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

int MarketSearchPage::GetNomalStatsCategoryType_1()
{
	int nComboxIndex = static_cast<int>(ID_NOMAL_STATS_COMBO_1 - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

int MarketSearchPage::GetNomalStatsCategoryType_2()
{
	int nComboxIndex = static_cast<int>(ID_NOMAL_STATS_COMBO_2 - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

int MarketSearchPage::GetNomalStatsCategoryType_3()
{
	int nComboxIndex = static_cast<int>(ID_NOMAL_STATS_COMBO_3- ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

int MarketSearchPage::GetNomalStatsCategoryType_4()
{
	int nComboxIndex = static_cast<int>(ID_NOMAL_STATS_COMBO_4 - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

float MarketSearchPage::GetNomalStatsCategoryValue_1()
{
	_CRT_FLOAT fltval; 
	_atoflt(&fltval,const_cast<char*>(m_pBasicStatsEditBox->GetEditString(ID_NOMAL_STATS_EDITBOX_1).GetString()) );
	return fltval.f;
}

float MarketSearchPage::GetNomalStatsCategoryValue_2()
{
	_CRT_FLOAT fltval; 
	_atoflt(&fltval,const_cast<char*>(m_pBasicStatsEditBox->GetEditString(ID_NOMAL_STATS_EDITBOX_2).GetString()) );
	return fltval.f;
}

float MarketSearchPage::GetNomalStatsCategoryValue_3()
{
	_CRT_FLOAT fltval; 
	_atoflt(&fltval,const_cast<char*>(m_pBasicStatsEditBox->GetEditString(ID_NOMAL_STATS_EDITBOX_3).GetString()) );
	return fltval.f;
}

float MarketSearchPage::GetNomalStatsCategoryValue_4()
{
	_CRT_FLOAT fltval; 
	_atoflt(&fltval,const_cast<char*>(m_pBasicStatsEditBox->GetEditString(ID_NOMAL_STATS_EDITBOX_4).GetString()) );
	return fltval.f;
}

int MarketSearchPage::GetAddRandomOptionCategoryType_1()
{
	int nComboxIndex = static_cast<int>(ID_ADD_RANDOM_OPTION_COMBO_BG_1 - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

int MarketSearchPage::GetAddRandomOptionCategoryType_2()
{
	int nComboxIndex = static_cast<int>(ID_ADD_RANDOM_OPTION_COMBO_BG_2 - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

int MarketSearchPage::GetAddRandomOptionCategoryType_3()
{
	int nComboxIndex = static_cast<int>(ID_ADD_RANDOM_OPTION_COMBO_BG_3 - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

int MarketSearchPage::GetAddRandomOptionCategoryType_4()
{
	int nComboxIndex = static_cast<int>(ID_ADD_RANDOM_OPTION_COMBO_BG_4 - ID_COMBOX_START);
	int nType = m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();

	if ( nType < 0 && nType >nComboxIndex)
		return 0;
	else
		return m_pSearchOptionCombox[nComboxIndex]->GetSelectIndex();
}

float MarketSearchPage::GetAddRandomOptionCategoryValue_1()
{
	CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_1);
	if ( pEditBox->IsVisible() )
	{
		_CRT_FLOAT fltval; 
		_atoflt(&fltval,const_cast<char*>(m_pAddRandomEditBox->GetEditString(ID_ADD_RANDOM_EDITBOX_1).GetString()) );
		return fltval.f;
	}
	else
	{
		return static_cast<float>(m_pSearchOptionCombox[ ID_BLOW_COMBOX_1 - ID_COMBOX_START ]->GetSelectIndex());

	}
}

float MarketSearchPage::GetAddRandomOptionCategoryValue_2()
{
	CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_2);
	if ( pEditBox->IsVisible() )
	{
		_CRT_FLOAT fltval; 
		_atoflt(&fltval,const_cast<char*>(m_pAddRandomEditBox->GetEditString(ID_ADD_RANDOM_EDITBOX_2).GetString()) );
		return fltval.f;
	}
	else
	{
		return static_cast<float>(m_pSearchOptionCombox[ ID_BLOW_COMBOX_2 - ID_COMBOX_START ]->GetSelectIndex());

	}
}

float MarketSearchPage::GetAddRandomOptionCategoryValue_3()
{
	CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_3);
	if ( pEditBox->IsVisible() )
	{
		_CRT_FLOAT fltval; 
		_atoflt(&fltval,const_cast<char*>(m_pAddRandomEditBox->GetEditString(ID_ADD_RANDOM_EDITBOX_3).GetString()) );
		return fltval.f;
	}
	else
	{
		return static_cast<float>(m_pSearchOptionCombox[ ID_BLOW_COMBOX_3 - ID_COMBOX_START ]->GetSelectIndex());

	}
}

float MarketSearchPage::GetAddRandomOptionCategoryValue_4()
{
	CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_4);
	if ( pEditBox->IsVisible() )
	{
		_CRT_FLOAT fltval; 
		_atoflt(&fltval,const_cast<char*>(m_pAddRandomEditBox->GetEditString(ID_ADD_RANDOM_EDITBOX_4).GetString()) );
		return fltval.f;
	}
	else
	{
		return static_cast<float>(m_pSearchOptionCombox[ ID_BLOW_COMBOX_4 - ID_COMBOX_START ]->GetSelectIndex());

	}
}

int MarketSearchPage::GetSearchSellType()
{
	if ( m_pCheckBoxPoint && m_pCheckBoxGold )
	{
		bool bGold = m_pCheckBoxGold->IsChecked();
		bool bPoint = m_pCheckBoxPoint->IsChecked();

		if( bGold == false )
			return (int)private_market::eSearchMoneyType_RanPoint;

		if( bPoint == false )
			return (int)private_market::eSearchMoneyType_Gold;
	}
	return (int)private_market::eSearchMoneyType_ALL;
}

void MarketSearchPage::SetUUID(int nIndex, UUID uuidID )
{
	if ( nIndex > 8)
		return;
	m_pSearchRow[nIndex]->SetUUID(uuidID);
}

void MarketSearchPage::ResetSearchOption()
{
	ResetSearchItemName();//검색 아이템 이름
	ResetBasicOption();//기본조건
	ResetNomalOPtion();//일반능력치
	ResetRandoOption();//가산및 랜덤 옵션
	ResetBLOWCombox();
	SetClassSexTexture();
}

void MarketSearchPage::ResetNomalOPtion()
{
	m_pBasicStatsEditBox->ClearEdit(ID_NOMAL_STATS_EDITBOX_1);
	m_pBasicStatsEditBox->SetEditString(ID_NOMAL_STATS_EDITBOX_1, SEARCH_MIN_VALUE);
	m_pBasicStatsEditBox->ClearEdit(ID_NOMAL_STATS_EDITBOX_2);
	m_pBasicStatsEditBox->SetEditString(ID_NOMAL_STATS_EDITBOX_2, SEARCH_MIN_VALUE);
	m_pBasicStatsEditBox->ClearEdit(ID_NOMAL_STATS_EDITBOX_3);
	m_pBasicStatsEditBox->SetEditString(ID_NOMAL_STATS_EDITBOX_3, SEARCH_MIN_VALUE);
	m_pBasicStatsEditBox->ClearEdit(ID_NOMAL_STATS_EDITBOX_4);
	m_pBasicStatsEditBox->SetEditString(ID_NOMAL_STATS_EDITBOX_4, SEARCH_MIN_VALUE);
}

void MarketSearchPage::ResetRandoOption()
{
	m_pAddRandomEditBox->ClearEdit(ID_ADD_RANDOM_EDITBOX_1);
	m_pAddRandomEditBox->SetEditString(ID_ADD_RANDOM_EDITBOX_1, SEARCH_MIN_VALUE);
	m_pAddRandomEditBox->ClearEdit(ID_ADD_RANDOM_EDITBOX_2);
	m_pAddRandomEditBox->SetEditString(ID_ADD_RANDOM_EDITBOX_2, SEARCH_MIN_VALUE);
	m_pAddRandomEditBox->ClearEdit(ID_ADD_RANDOM_EDITBOX_3);
	m_pAddRandomEditBox->SetEditString(ID_ADD_RANDOM_EDITBOX_3, SEARCH_MIN_VALUE);
	m_pAddRandomEditBox->ClearEdit(ID_ADD_RANDOM_EDITBOX_4);
	m_pAddRandomEditBox->SetEditString(ID_ADD_RANDOM_EDITBOX_4, SEARCH_MIN_VALUE);
}

void MarketSearchPage::ResetBasicOption()
{
	m_pCheckBoxGold->SetCheckOn();
	m_pCheckBoxPoint->SetCheckOn();

	m_pLimiteItemCheckBox->SetCheckOff();
	m_pItemStatsEditBox->ClearEdit(ID_BASIC_STATS_1);
	m_pItemStatsEditBox->SetEditString(ID_BASIC_STATS_1,SEARCH_MIN_VALUE);
	m_pBasicItemTurnNumEditBox->ClearEdit(ID_BASIC_ITEM_TURN_1);
	SetItemCategoryCombo( m_pSearchOptionCombox[ID_ITEM_CATEGORY_COMBO - ID_COMBOX_START]->GetSelectIndex() );
}

void MarketSearchPage::SetsSaleItem( int nIndex, private_market::sSaleItem* sSaleItem )
{
	m_pSearchRow[nIndex]->SetsSaleItem(sSaleItem);
}

void MarketSearchPage::SetMapName( int nIndex, const CString& strtext, DWORD dwColor )
{
	m_pSearchRow[nIndex]->SetMapName(strtext, dwColor );
}

void MarketSearchPage::ResetSearchItemName()
{
	m_pItemSearchEditBox->ClearEdit(ID_ITEM_SEARCH_EDITBOX_1);
	m_pItemSearchEditBox->SetEditString(ID_ITEM_SEARCH_EDITBOX_1,ITEM_NAME_INPUT);
}

DWORD MarketSearchPage::GetItemTurnNum()
{
	return static_cast<DWORD>(atoi( m_pBasicItemTurnNumEditBox->GetEditString(ID_BASIC_ITEM_TURN_1)));
}

void MarketSearchPage::CloseAllRollOver()
{
	BOOST_FOREACH( CSmartComboBoxBasicEx* vecSearchOption, m_pSearchOptionCombox )
	{
		vecSearchOption->SetVisibleSingleRollOver(FALSE);
	}
}

void MarketSearchPage::ResetBLOWCombox()
{
	for (int i = 0; i < 4; i ++)
	{
		m_pSearchOptionCombox[ ID_BLOW_COMBOX_1+i - ID_COMBOX_START ]->SetVisibleSingle(FALSE);
		CUIEditBox* pEditBox = (CUIEditBox*)m_pAddRandomEditBox->FindControl(ID_ADD_RANDOM_EDITBOX_1 + i);
		pEditBox->SetVisibleSingle(TRUE);
	}
}

void MarketSearchPage::SetPageWaiting( bool bIsWaiting )
{
	m_bPageWaiting = bIsWaiting;
	m_pWaitingBox->SetVisibleSingle(bIsWaiting);
}

bool MarketSearchPage::IsPageWaiting()
{
	return m_bPageWaiting;
}

bool MarketSearchPage::IsItemName()
{
	CString strTemp1 = m_pItemSearchEditBox->GetEditString( ID_ITEM_SEARCH_EDITBOX_1);
	CString strTemp2 = ITEM_NAME_INPUT;

	if ( (strTemp1 != "") && (strTemp1 != strTemp2) )
		return true;
	else
		return false;
}

void MarketSearchPage::SetSearchSellTypeCheckBox( int nCheckBoxID )
{
	if ( nCheckBoxID == ID_SEARCH_SELL_TYPE_GOLD_CHECKBOX && m_pCheckBoxGold )
	{
		bool bCheck = m_pCheckBoxGold->IsChecked();

		if( bCheck == true )
			return;
		
		if ( m_pCheckBoxPoint && m_pCheckBoxPoint->IsChecked() == false )
			m_pCheckBoxGold->SetCheckOn();		
	}


	if ( nCheckBoxID == ID_SEARCH_SELL_TYPE_POINT_CHECKBOX && m_pCheckBoxPoint  )
	{
		bool bCheck = m_pCheckBoxPoint->IsChecked();

		if( bCheck == true )
			return;

		if ( m_pCheckBoxGold && m_pCheckBoxGold->IsChecked() == false )
			m_pCheckBoxPoint->SetCheckOn();		
	}
}

void MarketSearchPage::ReSearchItem(private_market::eItemSortOption emSortOption)
{
	m_sBasic.wPage = 1;
	m_sBasic.nSortOption = emSortOption;
	m_sName.nSortOption = emSortOption;

	if (private_market::eItemSortOptionName == emSortOption )
	{
		m_sBasic.nSortMethod = m_nSortName;
		m_sName.nSortMethod = m_nSortName;
	}
	else
	{
		m_sBasic.nSortMethod = m_nSortPrice;
		m_sName.nSortMethod = m_nSortPrice;
	}

	HRESULT hr;
	SetPageWaiting(TRUE);
	if ( IsItemName() )
	{
		CString str = GetItemNameSearch();
		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString(str);
		// Construct a std::string using the LPCSTR input
		std::string s(pszConvertedAnsiString);

		
		m_sName.strItemName = s;
		hr = m_pGaeaClient->ReqPrivateMarketItemSearchByName(&m_sName);
	}
	else
		hr = m_pGaeaClient->ReqPrivateMarketItemSearch(&m_sBasic);

	if ( E_FAIL == hr )
	{
		m_pWaitingBox->SetVisibleSingle(TRUE);
	}
}

void MarketSearchPage::SetItemCategoryCombo( int nIndex )
{
	// 아래의 COMMENT는 데이터에서 읽어오지않는다;
	if ( nIndex == 1)
	{
		m_pBasicItemTurnNumEditBox->SetVisibleSingle(FALSE);
		m_pSearchOptionCombox[ID_BASIC_WEAR_CATEGORY_COMBO- ID_COMBOX_START]->SetSelectIndex(0);
		m_pSearchOptionCombox[ID_BASIC_WEAR_CATEGORY_COMBO- ID_COMBOX_START]->SetVisibleSingle(TRUE);
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_WEAPON_TYPE, private_market::eItemWeaponTypeSize, m_pSearchOptionCombox[ID_BASIC_WEAR_CATEGORY_COMBO- ID_COMBOX_START] );
		m_pBasicWearTexBox->SetText( WEAR_WEAPON,NS_UITEXTCOLOR::WHITE  );
	}
	else if ( nIndex == 2)
	{
		m_pBasicItemTurnNumEditBox->SetVisibleSingle(FALSE);
		m_pSearchOptionCombox[ID_BASIC_WEAR_CATEGORY_COMBO- ID_COMBOX_START]->SetSelectIndex(0);
		m_pSearchOptionCombox[ID_BASIC_WEAR_CATEGORY_COMBO- ID_COMBOX_START]->SetVisibleSingle(TRUE);
		SetRollOverCategory( COMMENT::PRIVATE_MARKET_ITEM_ARMOR_TYPE, private_market::eItemArmorTypeSize, m_pSearchOptionCombox[ID_BASIC_WEAR_CATEGORY_COMBO- ID_COMBOX_START] );
		m_pBasicWearTexBox->SetText( WEAR_POS,NS_UITEXTCOLOR::WHITE  );
	}
	else
	{
		m_pBasicItemTurnNumEditBox->SetVisibleSingle(TRUE);
		m_pSearchOptionCombox[ID_BASIC_WEAR_CATEGORY_COMBO- ID_COMBOX_START]->SetSelectIndex(0);
		m_pSearchOptionCombox[ID_BASIC_WEAR_CATEGORY_COMBO- ID_COMBOX_START]->SetVisibleSingle(FALSE);
		m_pBasicWearTexBox->SetText( WEAR_NUM,NS_UITEXTCOLOR::WHITE  );
	}
}
