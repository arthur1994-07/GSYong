#include "StdAfx.h"
 
#include "MarketRPPage.h"

#include "../../../enginelib/DxTools/DxFontMan.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"

#include "../../../RanLogic/Market/MarketDefine.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Item/GLItemMan.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/PrivateMarketSearchBuy/ConsignmentSaleClient.h"

#include "../Util/CheckBox.h"
#include "../Util/RnButton.h"
#include "../Util/SmartComboBoxEx.h"

#include "../Item/ItemImageEx.h"

#include "PageNavigation.h"

namespace
{
    //////////////////////////////////////////////////////////////////////////
    const char* MARKET_RPPAGE_TEXT = "MARKET_RPPAGE_TEXT";// = "일";    //DAY

    const char* MARKET_RPPAGE_TEXT0 = "MARKET_RPPAGE_TEXT0";// = "내가 등록한 아이템";
    const char* MARKET_RPPAGE_TEXT1 = "MARKET_RPPAGE_TEXT1";// = "등록수 : %1%/%2% (+%3%)";
    const char* MARKET_RPPAGE_TEXT2 = "MARKET_RPPAGE_TEXT2";// = "(확장) : ~ %1%. %2%. %3%";
    const char* MARKET_RPPAGE_TEXT3 = "MARKET_RPPAGE_TEXT3";// = "판매등록";
    const char* MARKET_RPPAGE_TEXT4 = "MARKET_RPPAGE_TEXT4";// = "판매 아이템";

    const char* MARKET_RPPAGE_TEXT5 = "MARKET_RPPAGE_TEXT5";// = "판매";
    const char* MARKET_RPPAGE_TEXT6 = "MARKET_RPPAGE_TEXT6";// = "게임머니";
    const char* MARKET_RPPAGE_TEXT7 = "MARKET_RPPAGE_TEXT7";// = "판매 가격(1개)";
    const char* MARKET_RPPAGE_TEXT8 = "MARKET_RPPAGE_TEXT8";// = "판매 개수";
    const char* MARKET_RPPAGE_TEXT9 = "MARKET_RPPAGE_TEXT9";// = "판매시간";

    const char* MARKET_RPPAGE_TEXT10 = "MARKET_RPPAGE_TEXT10";// = "(1일 : 24시간)";
    const char* MARKET_RPPAGE_TEXT11 = "MARKET_RPPAGE_TEXT11";// = "등록비용";
    const char* MARKET_RPPAGE_TEXT12 = "MARKET_RPPAGE_TEXT12";// = "판매수수료 : %1%%%";
    const char* MARKET_RPPAGE_TEXT13 = "MARKET_RPPAGE_TEXT13";// = "보유포인트 :";            // 변경
    const char* MARKET_RPPAGE_TEXT14 = "MARKET_RPPAGE_TEXT14";// = "등록취소";

    const char* MARKET_RPPAGE_TEXT15 = "MARKET_RPPAGE_TEXT15";// = "초기화";
    const char* MARKET_RPPAGE_TEXT16 = "MARKET_RPPAGE_TEXT16";// = "등록";
    const char* MARKET_RPPAGE_TEXT17 = "MARKET_RPPAGE_TEXT17";// = "아이콘";
    const char* MARKET_RPPAGE_TEXT18 = "MARKET_RPPAGE_TEXT18";// = "아이템명";
    const char* MARKET_RPPAGE_TEXT19 = "MARKET_RPPAGE_TEXT19";// = "판매정보";

    const char* MARKET_RPPAGE_TEXT20 = "MARKET_RPPAGE_TEXT20";// = "판매금액";
    const char* MARKET_RPPAGE_TEXT21 = "MARKET_RPPAGE_TEXT21";// = "만료된 물품입니다. 물품을 회수 하시겠습니까?";
    const char* MARKET_RPPAGE_TEXT22 = "MARKET_RPPAGE_TEXT22";// = "물품이 판매중입니다. 회수 하시겠습니까?";
    const char* MARKET_RPPAGE_TEXT23 = "MARKET_RPPAGE_TEXT23";// = "물품을 회수 하시겠습니까?";
    const char* MARKET_RPPAGE_TEXT25 = "MARKET_RPPAGE_TEXT25";// = "아이템을 등록할 수 없습니다.";

    const char* MARKET_RPPAGE_TEXT26 = "MARKET_RPPAGE_TEXT26";// = "아이템을 등록하세요.";
    const char* MARKET_RPPAGE_TEXT27 = "MARKET_RPPAGE_TEXT27";// = "%1% 이상의 금액을 입력하세요.";
    const char* MARKET_RPPAGE_TEXT28 = "MARKET_RPPAGE_TEXT28";// = "수량을 입력하세요.";
    const char* MARKET_RPPAGE_TEXT30 = "MARKET_RPPAGE_TEXT30";// = "[%1%]아이템을 등록 하시겠습니까?";

    const char* MARKET_RPPAGE_TEXT33 = "MARKET_RPPAGE_TEXT33";// = "보유머니 :";
    const char* MARKET_RPPAGE_TEXT34 = "MARKET_RPPAGE_TEXT34";// = "해당 아이템을 등록할 수 없습니다.";
    //const char* MARKET_RPPAGE_TEXT35 = "MARKET_RPPAGE_TEXT35";// = "물품받기";
    //const char* MARKET_RPPAGE_TEXT36 = "MARKET_RPPAGE_TEXT36";// = "등록취소";

    const char* MARKET_RPPAGE_TEXT37 = "MARKET_RPPAGE_TEXT37";
    const char* MARKET_RPPAGE_TEXT38 = "MARKET_RPPAGE_TEXT38";
    const char* MARKET_RPPAGE_TEXT39 = "MARKET_RPPAGE_TEXT39";

    const char* MARKET_RPPAGE_TEXT40 = "MARKET_RPPAGE_TEXT40";
    const char* MARKET_RPPAGE_TEXT41 = "MARKET_RPPAGE_TEXT41";
    
    const char* MARKET_RPPAGE_MESSAGE6;// = "포인트가 부족합니다.";


	const char* MARKET_RPPAGE_TEXT_35 = "코스튬을 분리 후 등록 가능합니다.";
	const char* MARKET_RPPAGE_TEXT_36 = "기간제 아이템은 등록할 수 없습니다.";
	const char* MARKET_RPPAGE_TEXT_37 = "(물품/대금은 우편으로 발송됩니다.)";

    const UINT  nLIMIT_NUMBER_CHAR = 10;

    const char* const INDENT = "  ";    //여백

}


MarketRPPage::MarketRPPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroupHelperMessageBox( pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
    , m_pInterface( pInterface )
    // 초기화 리스트
    , m_pPagenavigation( NULL )
    , m_pSelectImg( NULL )
    , m_pRegistCountText( NULL )
    , m_pExtendedHoursText( NULL )
    , m_pSaleCommissionText( NULL )
    , m_pRegistValue( NULL )
    , m_pMyPointText( NULL )
    , m_pPeriodComboBox( NULL )
    , m_pItemImage( NULL )
    , m_pSaleCount( NULL )
    , m_pSalePrice( NULL )
    , m_bListUpdated( FALSE )

    , m_nSeleted(-1)
    , m_nSeletedTemp(-1)
{
    m_vecItemList.clear();

    for( int i=0 ; i<emRowSize ; i++)
        m_pRow[i] = NULL;

    //////////////////////////////////////////////////////////////////////////
    MARKET_RPPAGE_TEXT  = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 0);
    MARKET_RPPAGE_TEXT0 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 1);
    MARKET_RPPAGE_TEXT1 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 2);
    MARKET_RPPAGE_TEXT2 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 3);
    MARKET_RPPAGE_TEXT3 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 4);

    MARKET_RPPAGE_TEXT4 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 5);
    MARKET_RPPAGE_TEXT5 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 6);
    MARKET_RPPAGE_TEXT6 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 7);
    MARKET_RPPAGE_TEXT7 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 8);
    MARKET_RPPAGE_TEXT8 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 9);

    MARKET_RPPAGE_TEXT9 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 10);
    MARKET_RPPAGE_TEXT10 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 11);
    MARKET_RPPAGE_TEXT11 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 12);
    MARKET_RPPAGE_TEXT12 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 13);
    //MARKET_RPPAGE_TEXT13 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 14);

    MARKET_RPPAGE_TEXT14 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 15);
    MARKET_RPPAGE_TEXT15 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 16);
    MARKET_RPPAGE_TEXT16 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 17);
    MARKET_RPPAGE_TEXT17 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 18);
    MARKET_RPPAGE_TEXT18 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 19);

    MARKET_RPPAGE_TEXT19 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 20);
    MARKET_RPPAGE_TEXT20 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 21);

    MARKET_RPPAGE_TEXT25 =ID2GAMEINTEXT("MARKET_RPPAGE_MESSAGE0");
    MARKET_RPPAGE_TEXT26 =ID2GAMEINTEXT("MARKET_RPPAGE_MESSAGE1");
    MARKET_RPPAGE_TEXT27 =ID2GAMEINTEXT("MARKET_RPPAGE_MESSAGE2");
    MARKET_RPPAGE_TEXT28 =ID2GAMEINTEXT("MARKET_RPPAGE_MESSAGE3");
    MARKET_RPPAGE_TEXT30 =ID2GAMEINTEXT("MARKET_RPPAGE_MESSAGE4");
    MARKET_RPPAGE_MESSAGE6 = ID2GAMEINTEXT("MARKET_RPPAGE_MESSAGE6");

    //MARKET_RPPAGE_TEXT35 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 22);
    //MARKET_RPPAGE_TEXT36 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 23);
    MARKET_RPPAGE_TEXT37 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 24);
    MARKET_RPPAGE_TEXT38 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 25);
    MARKET_RPPAGE_TEXT39 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 26);

    MARKET_RPPAGE_TEXT40 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 27);
    MARKET_RPPAGE_TEXT41 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 28);

    MARKET_RPPAGE_TEXT21 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 29);
    MARKET_RPPAGE_TEXT22 =ID2GAMEWORD("MARKET_RPPAGE_TEXT", 30);
    


	MARKET_RPPAGE_TEXT_35 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 35);
	MARKET_RPPAGE_TEXT_36 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 36);
	MARKET_RPPAGE_TEXT_37 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 37);


}


void MarketRPPage::CreateSubControl()
{
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    // 검색결과 창 배경& 라인박스
    CreateLineBox( "MARKET_SEARCH_PAGE_BG", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );

    // 타이틀 박스
    CreateLineBox( "MARKET_SEARCH_PAGE_TITLE", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );

    // 배경& 라인박스
    CreateLineBox( "MARKET_SEARCH_PAGE_OBJECT_BAR", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
     
    // 타이틀 텍스트
    CreateText( MARKET_RPPAGE_TEXT0, "MARKET_SEARCH_PAGE_TITLE"
            , TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );

    // 배경색 지정
    {
        CUIControl* pControl = CreateControl( "MARKET_SEARCH_PAGE_OBJECT_BAR" );        
        pControl->SetUseRender ( TRUE );
        pControl->SetDiffuse ( D3DCOLOR_ARGB(130,74,69,42) );
    }

    // [아이콘] 텍스트 박스
    CreateText( MARKET_RPPAGE_TEXT17, "MARKET_SEARCH_PAGE_OBJECT_ITEM"
        , TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );

    // [아이템명] 텍스트 박스
    CreateText( MARKET_RPPAGE_TEXT18, "MARKET_SEARCH_PAGE_OBJECT_ITEMNAME"
        , TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );

    // [판매정보] 텍스트 박스
    CreateText( MARKET_RPPAGE_TEXT19, "MARKET_SEARCH_PAGE_OBJECT_SELLERINFO"
        , TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );

    CreateText( MARKET_RPPAGE_TEXT20, "MARKET_SEARCH_PAGE_OBJECT_COST"
        , TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
   

    {// 아이템 행
        for (int i = 0; i < emRowSize; i++ )
        {
            CString strTemp;
            strTemp.Format( "%s%d", "MARKET_RP_PAGE_ROW_", i );

            MarketRPPageRow* pRow = new MarketRPPageRow( m_pGaeaClient, m_pInterface, m_pEngineDevice );
            pRow->CreateSub(this, strTemp.GetString(), UI_FLAG_DEFAULT, ID_ROW_START+i );
            pRow->CreateSubControl();
            pRow->SetVisibleSingle( FALSE );
            RegisterControl( pRow );

            m_pRow[i] = pRow;
        }

    }
    //테두리[노란선]
    m_pSelectImg = new CBasicLineBoxSmart( m_pEngineDevice );
    m_pSelectImg->CreateSub( this, "RNSKILL_SLOT_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    m_pSelectImg->CreateSubControl( "LINEBOX_Y" );
    m_pSelectImg->SetVisibleSingle( FALSE );
    RegisterControl( m_pSelectImg );

    //검색 페이지  버튼이랑 구분 선
    CreateLineBox( "MARKET_SEARCH_PAGE_BG_LINE", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );

    //페이지 네비게이터
    Pagenavigation* pPagenavigation = new Pagenavigation( m_pGaeaClient, m_pInterface, m_pEngineDevice );
    pPagenavigation->CreateSub(this,"PAGE_NAVIGATION_PAGE_POS", UI_FLAG_DEFAULT, ID_PAGENAVIGATION );
    pPagenavigation->SetFont( pFont );
    pPagenavigation->CreateSubControl();
    RegisterControl(pPagenavigation);
    m_pPagenavigation = pPagenavigation;
    m_pPagenavigation->PagenavigationUpdate(1);

    
    m_pRegistCountText = CreateText( sc::string::format( MARKET_RPPAGE_TEXT1, 3, 8, 8 ), "MARKET_RP_PAGE_TEXT1"
        , TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );

    m_pExtendedHoursText = CreateText( sc::string::format( MARKET_RPPAGE_TEXT2, 13, 12, 12 ), "MARKET_RP_PAGE_TEXT2"
        , TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );


    // 등록취소
    m_pButton = CreateRnButton( MARKET_RPPAGE_TEXT14, "MARKET_SEARCH_PAGE_ADD_WHISH_BUTTON", UI_FLAG_DEFAULT, ID_BUTTON_UNREGIST );


    //////////////////////////////////////////////////////////////////////////
    // 판매 등록

    std::string strTemp;

    {
        CreateLineBox( "MARKET_SEARCH_PAGE_OPTION_BG", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        CreateLineBox( "MARKET_SEARCH_PAGE_OPTION_TITLE", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        strTemp = sc::string::format("%s%s", INDENT, MARKET_RPPAGE_TEXT3 );
        CreateText( strTemp, "MARKET_SEARCH_PAGE_OPTION_TITLE"
            , TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE ); 
    }

    {
        // 판매 아이템
        CreateLineBox( "MARKET_RP_PAGE_R_BG0", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        
        CreateText( sc::string::format("%s%s", INDENT, MARKET_RPPAGE_TEXT4 ), "MARKET_RP_PAGE_R_BG0_TEXT1", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
        CreateText( sc::string::format("%s%s", INDENT, MARKET_RPPAGE_TEXT37 ), "MARKET_RP_PAGE_R_BG0_TEXT2", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );


        CreateLineBox( "MARKET_RP_PAGE_R_BG0_ICON", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        CItemImageEx* pItemImage = new CItemImageEx(m_pGaeaClient, m_pEngineDevice);
        pItemImage->CreateSub( this, "MARKET_RP_PAGE_R_BG0_ICON", UI_FLAG_DEFAULT, ID_IMAGE_ITEM );
        pItemImage->CreateSubControl();
        RegisterControl( pItemImage );
        m_pItemImage = pItemImage;
    }


    {
        CSmartComboBoxBasicEx* pComboBox; 
        //CreateLineBox( "MARKET_RP_PAGE_R_BG1", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );

        //strTemp = sc::string::format("%s%s", INDENT, ID2GAMEWORD("MARKET_RPPAGE_TEXT", 24) );
        //CBasicTextBox* pText = CreateText( strTemp, "MARKET_RP_PAGE_R_BG1", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
        CreateLineBox( "MARKET_RP_PAGE_R_BG1_INPUT1", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );


        // 아이템종류 콤보박스
        pComboBox = new CSmartComboBoxBasicEx(m_pEngineDevice);
        pComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_MONEY_COMBO );
        pComboBox->CreateSmartComboBoxCustom( "MARKET_RP_PAGE_R_BG1_INPUT1", 
            "POSTBOX_PAGE_SEND_COMBOBOX_DESIGN_TEXINFO",
            "MARKET_SEARCH_PAGE_BASIC_WEAR_CATEGORY_COMBO_BUTTON",
            "ANOTHER_CHARACTER_COMBOBOX_BUTTON_NORMAL",
            "ANOTHER_CHARACTER_COMBOBOX_BUTTON_OVER",
            "ANOTHER_CHARACTER_COMBOBOX_BUTTON_DOWN");
        m_pPeriodGameMoney = pComboBox;

        std::vector<private_market::GameMoneyItemFee> vecMoneyPeriodFee;
        vecMoneyPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();

        std::string strTemp;
        strTemp = MARKET_RPPAGE_TEXT37;
        pComboBox->SetRollOverAddText(strTemp.c_str(), 0);

        for( UINT i=0 ; i<vecMoneyPeriodFee.size() ; i++ )
        {
            strTemp.clear();
            const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[i];

            SITEM* pItem = GLItemMan::GetInstance().GetItem( value.sNativeID );
            if( pItem == NULL )
            {
                GASSERT( 0 && "[PrivateMarket.lua] File SellGameMoneySetting Is Error!!!!" );
				continue;
            }
            
            if( pItem->BasicType() != ITEM_CONSIGMENT_SALE_GAME_MONEY )
            {
                GASSERT( 0 && "[PrivateMarket.lua] File SellGameMoneySetting Item Type Is Not [RanGameMoney]" );
            }

            
            

            strTemp = sc::string::format( "%s", NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pItem->sBasicOp.dwSellPrice, 3, "," ) );
            pComboBox->SetRollOverAddText(strTemp.c_str(), i+1);
        }

        if( 0 < vecMoneyPeriodFee.size() )
            pComboBox->SetSelectIndex( 0 );

    }

    {
        // 판매 가격 판매 개수
        CreateLineBox( "MARKET_RP_PAGE_R_BG2", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );

        m_pCheckBoxPoint = new CCheckBox( m_pEngineDevice ); 
        m_pCheckBoxPoint->CreateSub( this, "MARKET_RP_PAGE_R_BG2_CHECKBOX1", UI_FLAG_DEFAULT , ID_CHECKBOX_RANPOINT );
        m_pCheckBoxPoint->CreateSubControl( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
        m_pCheckBoxPoint->SetCheck( TRUE );
        RegisterControl( m_pCheckBoxPoint );

        m_pCheckBoxMoney = new CCheckBox( m_pEngineDevice ); 
        m_pCheckBoxMoney->CreateSub( this, "MARKET_RP_PAGE_R_BG2_CHECKBOX2", UI_FLAG_DEFAULT , ID_CHECKBOX_RANMONEY );
        m_pCheckBoxMoney->CreateSubControl( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
        m_pCheckBoxMoney->SetCheck( FALSE );
        RegisterControl( m_pCheckBoxMoney );
        //m_pCheckBoxPoint = pCheckBox;

        CreateText( sc::string::format("%s%s", INDENT, ID2GAMEWORD("MARKET_RPPAGE_TEXT", 33) ), "MARKET_RP_PAGE_R_BG2_CHECKTEXT1", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
        m_pTextBoxMoney = CreateText( sc::string::format("%s%s", INDENT, ID2GAMEWORD("MARKET_RPPAGE_TEXT", 7) ), "MARKET_RP_PAGE_R_BG2_CHECKTEXT2", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );

        CreateText( sc::string::format("%s%s", INDENT, ID2GAMEWORD("MARKET_RPPAGE_TEXT", 32) ), "MARKET_RP_PAGE_R_BG2_TEXT1", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
        CreateText( sc::string::format("%s%s", INDENT, MARKET_RPPAGE_TEXT7 ), "MARKET_RP_PAGE_R_BG2_TEXT2", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
        CreateText( sc::string::format("%s%s", INDENT, MARKET_RPPAGE_TEXT8 ), "MARKET_RP_PAGE_R_BG2_TEXT3", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );


//         strTemp = sc::string::format("%s%s", INDENT, MARKET_RPPAGE_TEXT7 );
//         CBasicTextBox* pText = CreateText( strTemp, "MARKET_RP_PAGE_R_BG2", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
//         strTemp = sc::string::format("%s%s", INDENT, MARKET_RPPAGE_TEXT8 );
//         pText->AddText( strTemp.c_str(), NS_UITEXTCOLOR::WHITE );
//         pText->SetPartInterval( 10.0f );



        CreateLineBox( "MARKET_RP_PAGE_R_BG2_INPUT1_BG", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        CUIEditBox* pEditBox = new CUIEditBox(m_pEngineDevice);
        pEditBox->CreateSub( this, "MARKET_RP_PAGE_R_BG2_INPUT1", UI_FLAG_LEFT | UI_FLAG_CENTER_Y, ID_TEXTBOX_SALE_PRICE );
        pEditBox->CreateCarrat( "MODAL_CARRAT", TRUE, UINT_MAX );
        pEditBox->SetLimitInput( nLIMIT_NUMBER_CHAR );
        pEditBox->SetFont( pFont );
        pEditBox->DoUSE_NUMBER( true );
        pEditBox->SET_MONEY_FORMAT( 3 );
        RegisterControl( pEditBox );
        m_pSalePrice = pEditBox;


        CreateLineBox( "MARKET_RP_PAGE_R_BG2_INPUT2_BG", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        pEditBox = new CUIEditBox(m_pEngineDevice);
        pEditBox->CreateSub( this, "MARKET_RP_PAGE_R_BG2_INPUT2", UI_FLAG_LEFT | UI_FLAG_CENTER_Y, ID_TEXTBOX_NUM_OF_SALE );
        pEditBox->CreateCarrat( "MODAL_CARRAT", TRUE, UINT_MAX );
        pEditBox->SetLimitInput( 5 );
        pEditBox->SetFont( pFont );
        pEditBox->DoUSE_NUMBER( true );
        //pEditBox->SET_MONEY_FORMAT( 3 );
        RegisterControl( pEditBox );
        m_pSaleCount = pEditBox;
        

		m_pSalePriceText = CreateText( "MARKET_RP_PAGE_R_BG2_INPUT1_BG", "MARKET_RP_PAGE_R_BG2_INPUT1_BG", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
		m_pSaleCountText = CreateText( "MARKET_RP_PAGE_R_BG2_INPUT2_BG", "MARKET_RP_PAGE_R_BG2_INPUT2_BG", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );

		ResetSalePriceBox();
		ResetSaleCountBox();
		
    }

    CSmartComboBoxBasicEx* pItemCategoryComboBox; 
    {
        // 판매시간 1일 24시간
        CreateLineBox( "MARKET_RP_PAGE_R_BG3", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        strTemp = sc::string::format("%s%s", INDENT, MARKET_RPPAGE_TEXT9 );
        CBasicTextBox* pText = CreateText( strTemp, "MARKET_RP_PAGE_R_BG3", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
        strTemp = sc::string::format("%s%s", INDENT, MARKET_RPPAGE_TEXT10 );
        pText->AddText( strTemp.c_str(), NS_UITEXTCOLOR::WHITE );
        pText->SetPartInterval( 10.0f );

        CreateLineBox( "MARKET_RP_PAGE_R_BG3_COMBO1", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );


        // 아이템종류 콤보박스
        pItemCategoryComboBox = new CSmartComboBoxBasicEx(m_pEngineDevice);
        pItemCategoryComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_COMBO );
        pItemCategoryComboBox->CreateSmartComboBoxCustom( "MARKET_RP_PAGE_R_BG3_COMBO1", 
            "POSTBOX_PAGE_SEND_COMBOBOX_DESIGN_TEXINFO",
            "MARKET_SEARCH_PAGE_BASIC_WEAR_CATEGORY_COMBO_BUTTON",
            "ANOTHER_CHARACTER_COMBOBOX_BUTTON_NORMAL",
            "ANOTHER_CHARACTER_COMBOBOX_BUTTON_OVER",
            "ANOTHER_CHARACTER_COMBOBOX_BUTTON_DOWN");
        m_pPeriodComboBox = pItemCategoryComboBox;

        std::vector<private_market::PeriodFee> vecPeriodFee;
        vecPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetSellPeriod();

        std::string strTemp;
        for( UINT i=0 ; i<vecPeriodFee.size() ; i++ )
        {
            strTemp.clear();
            const private_market::PeriodFee& value = vecPeriodFee[i];
            strTemp = sc::string::format( "%d%s", value.wPeriod/24, MARKET_RPPAGE_TEXT );
            pItemCategoryComboBox->SetRollOverAddText(strTemp.c_str(), i);
        }

        if( 0 < vecPeriodFee.size() )
            pItemCategoryComboBox->SetSelectIndex( 0 );

    }

    {
        // 등록 비용
        CreateLineBox( "MARKET_RP_PAGE_R_BG4", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        strTemp = sc::string::format("%s%s", INDENT, MARKET_RPPAGE_TEXT11 );
        CreateText( strTemp, "MARKET_RP_PAGE_R_BG4", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );

        // 금액
        m_pRegistValue = CreateText( strTemp, "MARKET_RP_PAGE_R_BG4", TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
        //m_pRegistValue->ClearText();
        m_pRegistValue->SetText( sc::string::format( "%1%%2%%3%%4%", "0", INDENT, INDENT, INDENT ).c_str() );

        CreateLineBox( "MARKET_RP_PAGE_R_BG4_ICON", "RAN_POINT_ICON_TEXTURE" );
    }

    {
        // 판매 수수료
        float fSalesCompletionFee = private_market::GLPrivateMarketDefine::Instance()->SalesCompletionFee();
        strTemp = sc::string::format( MARKET_RPPAGE_TEXT12, static_cast<int>( fSalesCompletionFee*100) );
        strTemp = strTemp + INDENT;
        m_pSaleCommissionText = CreateText( strTemp, "MARKET_RP_PAGE_R_TEXT9", TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
    }

    {
        // 보유 금액
        //strTemp = sc::string::format( "%s%s", INDENT, MARKET_RPPAGE_TEXT33 );
        //CreateText( strTemp, "MARKET_RP_PAGE_R_TEXT11", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );

        CreateLineBox( "MARKET_RP_PAGE_R_TEXT11", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        m_pMyMoneyText = CreateText( strTemp, "MARKET_RP_PAGE_R_TEXT11", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
        CreateLineBox( "MARKET_RP_PAGE_R_TEXT11_ICON", "RAN_MONEY_ICON_TEXTURE" );

        CreateLineBox( "MARKET_RP_PAGE_R_TEXT10", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        m_pMyPointText = CreateText( strTemp, "MARKET_RP_PAGE_R_TEXT10", TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, NS_UITEXTCOLOR::WHITE );
        CreateLineBox( "MARKET_RP_PAGE_R_TEXT10_ICON", "RAN_POINT_ICON_TEXTURE" );
    }

    {
        RnButton* pButton = CreateRnButton( MARKET_RPPAGE_TEXT15, "MARKET_SEARCH_PAGE_RESET_BUTTON", UI_FLAG_RIGHT, ID_BUTTON_RESET );
    }

    {
        RnButton* pButton = CreateRnButton( MARKET_RPPAGE_TEXT16, "MARKET_SEARCH_PAGE_SEARCH_BUTTON", UI_FLAG_RIGHT, ID_BUTTON_REGIST );
    }

    m_pGaeaClient->GetConsignmentSale();

    // 콤보박스 등록은 가장 마지막에 한다.
    RegisterControl(m_pPeriodGameMoney);
    RegisterControl(pItemCategoryComboBox);
    
    CreateMessageBox();
    m_bListUpdated =false;
}

void MarketRPPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroupHelperMessageBox::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
    if( m_bListUpdated == false )
    {
        m_bListUpdated = true;
        ListUpdate();
    }
    
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();

    WORD nListSize = m_vecItemList.size();
    WORD MaxPage = static_cast<int>( (nListSize-1) / emRowSize );
    int nPage = m_pPagenavigation->GetPageIndex(); // 현재 페이지

    // 페이지 네비게이터
    if( 0 < nListSize )
    {
        //m_pSelectImg->SetVisibleSingle( TRUE );
        m_pPagenavigation->SetVisibleSingle( TRUE );
        m_pPagenavigation->PagenavigationUpdate( MaxPage+1 );
    }
    else
    {
        //m_pSelectImg->SetVisibleSingle( FALSE );
        m_pPagenavigation->SetVisibleSingle( FALSE );
    }


    for (int i = 0; i < emRowSize; i++)
    {
        unsigned int nCurrentPage = ((nPage)*emRowSize) + i;
        if( nCurrentPage < nListSize )
        {
            m_pRow[i]->SetItem( m_vecItemList[nCurrentPage] );
            m_pRow[i]->SetVisibleSingle( TRUE );
        }
        else
        {
            m_pRow[i]->SetVisibleSingle( FALSE );
        }
    }


	if (m_pSaleCount->IsBegin())
		m_pSaleCountText->ClearText();

	if (m_pSalePrice->IsBegin())
		m_pSalePriceText->ClearText();
    

    //MONEYCOLOR::GetMoneyColor( SaleItem.dwPrice )
    //m_pGaeaClient->GetCharacter()->PointShopPoint()
    if( m_sInvenItem.sItemCustom.GetNativeID() != NATIVEID_NULL() )
    {
        SITEM* pItem = GLItemMan::GetInstance().GetItem( m_sInvenItem.sItemCustom.GetNativeID() );
        if( m_sInvenItem.sItemCustom.wTurnNum <= 1 || ITEM_CONSIGMENT_SALE_GAME_MONEY == pItem->BasicType() )
		{
            m_pSaleCount->SetEditString( "1" );
			m_pSaleCountText->SetVisibleSingle(FALSE);
		}

    }

    // 보유 포인트
    {
        //SetPeeText( m_sInvenItem.GetNativeID() );

        m_pMyMoneyText->ClearText();
        m_pMyPointText->ClearText();

        std::string strTemp = sc::string::format( "%1%%2%%3%%4%", INDENT, INDENT, INDENT, NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(pChar->GetInvenMoney(), 3, "," ) );
        m_pMyMoneyText->SetOneLineText( strTemp.c_str() );

        __int64 nPoint = pChar->PointShopPoint();
        strTemp = sc::string::format( "%1%%2%%3%%4%", INDENT, INDENT, INDENT , NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(nPoint, 3, "," ));
        m_pMyPointText->SetOneLineText( strTemp.c_str(), NS_UITEXTCOLOR::ORANGE );
    }
}

void MarketRPPage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroupHelperMessageBox::TranslateUIMessage ( ControlID, dwMsg );

    bool bMouseIn   = CHECK_MOUSE_IN( dwMsg );
    bool bMouseLBUP = CHECK_LB_UP_LIKE( dwMsg );    

    switch( ControlID )
    {
    case ID_CHECKBOX_RANPOINT:
        if( bMouseIn && CHECK_LB_DOWN_LIKE( dwMsg ) )
        {
            if( m_pPeriodComboBox->IsVisibleComboxRoolOver() || m_pPeriodGameMoney->IsVisibleComboxRoolOver() )
                return;

            m_pCheckBoxMoney->SetCheckOff();
            m_pCheckBoxPoint->SetCheckOn();
        }
        break;
    case ID_CHECKBOX_RANMONEY:
        if( bMouseIn && CHECK_LB_DOWN_LIKE( dwMsg ) )
        {
            if( m_pPeriodComboBox->IsVisibleComboxRoolOver() || m_pPeriodGameMoney->IsVisibleComboxRoolOver() )
                return;

            m_pCheckBoxPoint->SetCheckOff();
            m_pCheckBoxMoney->SetCheckOn();
        }
        break;
    case ID_IMAGE_ITEM:
        if( bMouseIn )
        {
            if( bMouseLBUP )
            {
                GLCharacter* pChar = m_pGaeaClient->GetCharacter();
                SINVEN_POS& sClickPos = pChar->m_sPreInventoryItem;

				// 인벤에 아이템이 있는가?
                SINVENITEM* sInvenItemNow = pChar->m_cInventory.FindPosItem( sClickPos.wPosX, sClickPos.wPosY );
                if( sInvenItemNow == NULL )
                {
                    Reset();
                    break;
                }

                if( sInvenItemNow->GetNativeID() != m_sInvenItem.GetNativeID() )
                    Reset();

                if( sInvenItemNow == NULL )
                    break;

                m_sInvenItem = *sInvenItemNow;
                sClickPos.RESET();


                SITEM* pItem = GLItemMan::GetInstance().GetItem( m_sInvenItem.GetNativeID() );
                if( pItem == NULL )
                    return;

 
				
				if ( sInvenItemNow->sItemCustom.nidDISGUISE != NATIVEID_NULL() )
				{
					// 코스튬이 붙어 있다면 거래 불가
					m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, MARKET_RPPAGE_TEXT_35 );
					Reset();
					return;
				}
				else if ( sInvenItemNow->sItemCustom.IsTimeLimit() == true )
				{
					// 시효성 아이템 거래 불가
					m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, MARKET_RPPAGE_TEXT_36 );
					Reset();
					return;
				}
				else if ( m_sInvenItem.sItemCustom.IsExchange() == false )
                {
                    if( pItem->sBasicOp.RanPointTradeCount() <= m_sInvenItem.sItemCustom.wRanPointTradeCount )
					{
                        Reset();
                        break;
                    }
                    m_pCheckBoxMoney->SetCheckOff();
                    m_pCheckBoxPoint->SetCheckOn();

                    m_pCheckBoxMoney->SetVisibleSingle( FALSE );
                    m_pPeriodComboBox->SetSelectIndex( 0 );
                }

				// 포장지 이미지는 꼭 SITEMCUSTOM을 넘겨야한다;
				// 덧씨우는 형식으로;
				SetImage( m_sInvenItem.sItemCustom );
				if ( m_sInvenItem.sItemCustom.IsBoxWrapped() )
				{
					SetPeeText( m_sInvenItem.sItemCustom.GetBoxWrappedItemID() );
				}
				else
				{
					//SetImage( m_sInvenItem.GetNativeID(), m_sInvenItem.TurnNum() );
					SetPeeText( m_sInvenItem.GetNativeID() );
				}

                m_pSaleCount->SetEditString( sc::string::format("%d", sInvenItemNow->sItemCustom.wTurnNum).c_str() );
				m_pSaleCountText->SetText("");

                //sClickPos.RESET();
            }


            if( m_sInvenItem.sItemCustom.GetNativeID() != NATIVEID_NULL() )
                m_pInterface->SHOW_ITEM_INFO( m_sInvenItem.sItemCustom, FALSE, FALSE, FALSE, FALSE, FALSE, 0, 0 );


        }
        break;
    case ID_BUTTON_UNREGIST:
        if( bMouseIn && bMouseLBUP )
        {
            if( 0 <= m_nSeleted && m_nSeleted < ID_ROW_END-100 )
            {
                m_nSeletedTemp = m_nSeleted;
                const private_market::ConsignmentSaleItem& Item = m_pRow[m_nSeletedTemp]->GetItem();               

                float fSalesCompletionFee = private_market::GLPrivateMarketDefine::Instance()->SalesCompletionFee();

                std::string strTitle;
                std::vector<std::string> vecString;
                if( 0 == Item.wTurnNum )
                {
                    // 판매 완료
                    vecString.push_back( MARKET_RPPAGE_TEXT22 );
                    strTitle = MARKET_RPPAGE_TEXT41;
                }
                else if( m_pRow[m_nSeletedTemp]->GetRemainTime() < 0 )
                {
                    // 시간 만료
                    vecString.push_back( MARKET_RPPAGE_TEXT21 );
                    strTitle = MARKET_RPPAGE_TEXT40;
                }
                else
                {
                    // 등록 취소
                    //vecString.push_back( MARKET_RPPAGE_TEXT23 );
                    vecString.push_back( MARKET_RPPAGE_TEXT21 );
                    strTitle = MARKET_RPPAGE_TEXT14;
                }

                vecString.push_back( sc::string::format( "[%1%] : %2%/%3%"
                    , Item.sItemcustom.GETNAME().c_str()
                    , Item.wTurnNum
                    , Item.wTotalTurnNum ) );

                std::string strTemp;
                strTemp.append( MARKET_RPPAGE_TEXT20 );
                strTemp.append( " : %1%(%2%%%)" );

                vecString.push_back( sc::string::format( strTemp.c_str()
                    , NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( (Item.wTotalTurnNum-Item.wTurnNum)*Item.llPrice, 3, "," )
                    , static_cast<int>( fSalesCompletionFee*100) ) );

				vecString.push_back( MARKET_RPPAGE_TEXT_37 );


                OpenMessageBoxMultiLine( strTitle, vecString, CMessageBox::TYPE_YESNO, ID_BUTTON_UNREGIST );
            }
        }
        break;
    case ID_BUTTON_RESET:
        if( bMouseIn && bMouseLBUP )
        {
            Reset();
        }
        break;
    case ID_BUTTON_REGIST:
        if( bMouseIn && bMouseLBUP )
        {
            if( RegistCheck() )
            {
                SITEM* pItem = GLItemMan::GetInstance().GetItem( m_sInvenItem.sItemCustom.GetBoxWrappedItemID() );
                if( pItem == NULL )
                {
                    OpenMessageBox(CMessageBox::TITLE_ERROR, MARKET_RPPAGE_TEXT26, CMessageBox::TYPE_OK );
                    return;
                }
                if( m_sInvenItem.sItemCustom.wTurnNum < m_nCount )
                {
                    OpenMessageBox(CMessageBox::TITLE_ERROR, MARKET_RPPAGE_TEXT28, CMessageBox::TYPE_OK );
                    m_pSaleCount->SetEditString(sc::string::format("%1%", 1).c_str());
                    return;
                }

                std::string strLine1 =  sc::string::format( MARKET_RPPAGE_TEXT30, m_sInvenItem.sItemCustom.GETNAME() );

                std::string strTemp;
                strTemp.append( MARKET_RPPAGE_TEXT20 );
                strTemp.append( ":%1% " );
                strTemp.append( MARKET_RPPAGE_TEXT8 );
                strTemp.append( ":%2%" );
                std::string strLine2 = sc::string::format( strTemp.c_str(), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( m_llPrice, 3, "," ), m_nCount ) ;

     
                std::string strTemp2;
                strTemp2.append( MARKET_RPPAGE_TEXT11 );
                strTemp2.append( ":%1% " );
                //const int& nFee = m_pGaeaClient->GetConsignmentSale()->GetFee( m_pPeriodComboBox->GetSelectIndex(), m_sInvenItem.sItemCustom );
                DWORD nFee = private_market::GLPrivateMarketDefine::Instance()->CalcFee( m_pPeriodComboBox->GetSelectIndex(), m_sInvenItem.sItemCustom );
                std::string strLine3 = sc::string::format( strTemp2.c_str(), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( nFee, 3, "," )  );



                std::vector<std::string> vecString;
                vecString.push_back( strLine1 );
                vecString.push_back( strLine2 );

                vecString.push_back( strLine3 );
				//vecString.push_back( MARKET_RPPAGE_TEXT_37 );
                
                OpenMessageBoxMultiLine( CMessageBox::TITLE_QUESTION, vecString, CMessageBox::TYPE_YESNO, ID_BUTTON_REGIST );
            }
            
        }
        break;
    case ID_COMBO:
        if( !bMouseIn && bMouseLBUP )
        {
            m_pPeriodComboBox->SetVisibleSingleRollOver(FALSE);
        }
        if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
        {

            std::vector<private_market::GameMoneyItemFee> vecMoneyPeriodFee;
            vecMoneyPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();

            bool bIsGameMoney = false;
            for( unsigned int i=0; i<vecMoneyPeriodFee.size(); i++ )
            {
                const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[i];

                if( value.sNativeID == m_sInvenItem.GetNativeID() )
                {
                    bIsGameMoney = true;
                    break;
                }
            }

            if( bIsGameMoney )
            {
//                 unsigned int nIndex = m_pPeriodComboBox->GetSelectIndex();
//                 SetPeeGameMoneyText( nIndex+1 );
                SetPeeText( NATIVEID_NULL() );
            }
            else
            {
                SetPeeText( m_sInvenItem.GetNativeID() );
            }

            
        }
        break;
    case ID_MONEY_COMBO:
        if( !bMouseIn && bMouseLBUP )
        {
            m_pPeriodGameMoney->SetVisibleSingleRollOver(FALSE);
        }

        if( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
        {	
            m_pCheckBoxMoney->SetCheckOff();
            m_pCheckBoxPoint->SetCheckOn();

            m_pCheckBoxMoney->SetVisibleSingle( FALSE );
            m_pPeriodComboBox->SetSelectIndex( 0 );

            std::vector<private_market::GameMoneyItemFee> vecMoneyPeriodFee;
            vecMoneyPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();

            unsigned int nIndex = m_pPeriodGameMoney->GetSelectIndex();

            if( nIndex == 0 )
            {
                Reset();
                return;
            }
            if( 0 < nIndex && nIndex <= vecMoneyPeriodFee.size() )
            {
                const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[nIndex-1];

                SITEM* pItem = GLItemMan::GetInstance().GetItem( value.sNativeID );

                if( m_pGaeaClient->GetCharacter()->GetInvenMoney() < pItem->sBasicOp.dwSellPrice )
                {
                    // 소지 골드가 부족합니다.
                    Reset();
                    return;
                }

                m_sInvenItem.sItemCustom = SITEMCUSTOM(value.sNativeID);
                SetImage( value.sNativeID, 1 );
                //SetPeeGameMoneyText( nIndex );
                SetPeeText( NATIVEID_NULL() );
            }
        }

        break;
    case ID_TEXTBOX_NUM_OF_SALE:
        if( bMouseLBUP )
        {
            if( bMouseIn )
            {
                m_pSaleCount->BeginEdit();
            }
            else
            {
                m_pSaleCount->EndEdit();
                CString strSaleCount = m_pSaleCount->GetEditString();
                int nCount = _ttoi( strSaleCount );
                if( nCount <= 0 )
                {
                    ResetSaleCountBox();
                }


                
            }
        }
        break;
    case ID_TEXTBOX_SALE_PRICE:
        if( bMouseLBUP )
        {
            if( bMouseIn )
            {
                m_pSalePrice->BeginEdit();
            }
            else
            {
                m_pSalePrice->EndEdit();
                LONGLONG llPrice = _ttoi64( m_pSalePrice->GetEditString() );
                if( llPrice <= 0 )
                {
                    ResetSalePriceBox();
                }
            }
        }
        break;
    }

    if( ID_ROW_START <= ControlID && ControlID < ID_ROW_END )
    {
        if( bMouseIn  )
        {
            
            int nIndex = ControlID - ID_ROW_START;
            const private_market::ConsignmentSaleItem& consigmentItem = m_pRow[nIndex]->GetItem();
            SITEM* pItem = GLItemMan::GetInstance().GetItem( consigmentItem.sItemcustom.GetNativeID() );
            if( pItem == NULL )
                return;

            CTime tEndDate(consigmentItem.ExpirationDate);

//             m_pInterface->BEGIN_COMMON_LINEINFO_MULTI( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
//             m_pInterface->ADD_COMMON_LINEINFO_MULTI( const CString& strText, D3DCOLOR dwColor );
//             m_pInterface->END_COMMON_LINEINFO_MULTI();

            // 툴팁 출력
            if( m_pRow[nIndex]->GetRemainTime() <= 0 || consigmentItem.wTurnNum <= 0  )
            {
                const float& fSalesCompletionFee = private_market::GLPrivateMarketDefine::Instance()->SalesCompletionFee();

                std::string strTemp;
                strTemp.append( MARKET_RPPAGE_TEXT20 );
                strTemp.append( " : %1%(%2%%%)" );

                if( m_pRow[nIndex]->IsMouseOnImageICON() == false )
                {
                    strTemp = sc::string::format( strTemp.c_str()
                        , NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( (consigmentItem.wTotalTurnNum-consigmentItem.wTurnNum)*consigmentItem.llPrice, 3, "," )
                        , static_cast<int>( fSalesCompletionFee*100) );
                }

                if( m_pRow[nIndex]->IsMouseOnImageICON() == false )
                    m_pInterface->SHOW_COMMON_LINEINFO( strTemp.c_str(), D3DCOLOR_XRGB( 0x50, 0x82, 0xC0 ), TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
            }
            else
            {
                if( m_pRow[nIndex]->IsMouseOnImageICON() == false )
                    m_pInterface->SHOW_COMMON_LINEINFO(
                        sc::string::format("%s:%d/%02d/%02d %02d:%02d", MARKET_RPPAGE_TEXT38, tEndDate.GetYear(), tEndDate.GetMonth(), tEndDate.GetDay(), tEndDate.GetHour(), tEndDate.GetMinute()).c_str(),
                        D3DCOLOR_XRGB( 0x50, 0x82, 0xC0 ),
                        TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );

            }
            

            // 클릭시
            if( bMouseLBUP )
            {
                // 기간만료
                const private_market::ConsignmentSaleItem& Item = m_pRow[nIndex]->GetItem();
                if( Item.wTurnNum == 0 )
                    m_pButton->SetText( MARKET_RPPAGE_TEXT41, NS_UITEXTCOLOR::DEFAULT );
                else if( m_pRow[nIndex]->GetRemainTime() < 0 )
                    m_pButton->SetText( MARKET_RPPAGE_TEXT40, NS_UITEXTCOLOR::DEFAULT );
                else
                    m_pButton->SetText( MARKET_RPPAGE_TEXT14, NS_UITEXTCOLOR::DEFAULT );

                m_pSelectImg->SetVisibleSingle( TRUE );
                if( m_pSelectImg->IsVisible() )
                {
                    m_nSeleted = ControlID - ID_ROW_START;
                    m_pSelectImg->SetLocalPos( m_pRow[m_nSeleted]->GetLocalPos() );
                    m_pSelectImg->SetGlobalPos( m_pRow[m_nSeleted]->GetGlobalPos() );
                }
            }

        }

    }
    else
    {
        if( bMouseIn && bMouseLBUP )
        {
            m_pSelectImg->SetVisibleSingle( FALSE );
            m_nSeleted = -1;
        }
    }

    if( DxInputDevice::GetInstance().IsUpdatKeyState() )
    {
        if( UIKeyCheck::GetInstance()->Check( DIK_ESCAPE, DXKEY_DOWN )  )
        {
            AllEndEdit();
        }

        if( UIKeyCheck::GetInstance()->Check( DIK_RETURN, DXKEY_DOWN )  )
        {
            AllEndEdit();
        }        

        if( UIKeyCheck::GetInstance()->Check( DIK_NUMPADENTER, DXKEY_DOWN )  )
        {
            AllEndEdit();
        }     
    }

}

void MarketRPPage::TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg )
{
    switch( nIdentity )
    {
    case ID_BUTTON_REGIST:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            Regist();
        }
        break;
    case ID_BUTTON_UNREGIST:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            Unregist();
        }
        break;
    }
}

void MarketRPPage::SetVisibleSingle( BOOL bVisible )
{
    m_nSeleted = -1;
    m_nSeletedTemp = -1;

    //m_sInvenItem = SINVENITEM();
    m_pPeriodGameMoney->SetVisibleSingleRollOver(FALSE);
    m_pPeriodComboBox->SetVisibleSingleRollOver(FALSE);

    CUIGroupHelperMessageBox::SetVisibleSingle( bVisible );
    // 창을 다시 열었다면 목록을 다시 받아오자.
    if( bVisible )
	{
        m_bListUpdated = false;
		Reset();
	}
       
}

void MarketRPPage::AllEndEdit()
{
    if( m_pSaleCount )
        m_pSaleCount->EndEdit();
    if( m_pSalePrice )
        m_pSalePrice->EndEdit();
}

void MarketRPPage::ListUpdate()
{
    m_pGaeaClient->GetConsignmentSale()->ReqListUpdate();
}


void MarketRPPage::Reset()
{
    m_sInvenItem = SINVENITEM();
    m_pPeriodGameMoney->SetSelectIndex( 0 );
    m_pPeriodComboBox->SetSelectIndex( 0 );
    m_pCheckBoxMoney->SetCheckOff();
    m_pCheckBoxPoint->SetCheckOn();

	if( private_market::GLPrivateMarketDefine::Instance()->GoldTrade() ) 
	{
		m_pCheckBoxMoney->SetVisibleSingle(TRUE);
	}
	else
	{
		m_pTextBoxMoney->SetVisibleSingle(FALSE);
		m_pCheckBoxMoney->SetVisibleSingle(FALSE);
	}

    m_pCheckBoxPoint->SetVisibleSingle(TRUE);

    SetPeeText( NATIVEID_NULL() );
    SetImage( NATIVEID_NULL(), 0 );
    ResetSaleCountBox();
    ResetSalePriceBox();
}

void MarketRPPage::Regist()
{
    AllEndEdit();
    RegistCheck();

    private_market::eSellType sellType = private_market::eSellType_ConsignmentSale_RanPoint;
    
    if (m_pCheckBoxMoney->IsChecked())
        sellType = private_market::eSellType_ConsignmentSale_Gold;
   
    if( 0 < m_nCount && 0 < m_llPrice )
        m_pGaeaClient->GetConsignmentSale()->ReqRegistItem( m_sInvenItem, m_llPrice, m_nCount, m_pPeriodComboBox->GetSelectIndex(), sellType ) ;

    Reset();
}

void MarketRPPage::Unregist()
{

    if( 0 <= m_nSeletedTemp && m_nSeletedTemp < emRowSize )
    {
        private_market::ConsignmentSaleItem saleItem = m_pRow[m_nSeletedTemp]->GetItem();
        m_pGaeaClient->GetConsignmentSale()->ReqUnregist( saleItem.GetGUID() );
    }
    m_nSeletedTemp = -1;

}

void MarketRPPage::Refresh()
{
    CUIGroupHelperMessageBox::Refresh();
    m_vecItemList = m_pGaeaClient->GetConsignmentSale()->GetList();
     
    

    // 확장 슬롯 시간 계산
    {
        WORD ExpandAddNum = 0;
        WORD wCurrentSlotNum = private_market::GLPrivateMarketDefine::Instance()->SellResgisterBasicNum();

        __time64_t tExpansionDate = m_pGaeaClient->GetCharacterData().GetConsignmentSaleSlotExpansionDate();    
        __time64_t tCurTime = CTime::GetCurrentTime().GetTime();

        CString strTemp = sc::string::format( "%1% %%1%%/%%2%%", MARKET_RPPAGE_TEXT1 ).c_str();
        CTime cTimm(tExpansionDate);

        if (0 < tExpansionDate - tCurTime)
        {
            ExpandAddNum = private_market::GLPrivateMarketDefine::Instance()->SellResgisterExpandAddNum();
            wCurrentSlotNum += ExpandAddNum;

            //m_pExtendedHoursText->SetVisibleSingle(TRUE);
            m_pExtendedHoursText->SetText(sc::string::format( MARKET_RPPAGE_TEXT2, cTimm.GetYear(), cTimm.GetMonth(), cTimm.GetDay() ).c_str());
            strTemp.Append( " (+%3%)" );
        }
        else
        {
            //m_pExtendedHoursText->SetVisibleSingle(FALSE);
            m_pExtendedHoursText->SetText(ID2GAMEWORD("MARKET_RPPAGE_TEXT", 34));
        }


        m_pRegistCountText->SetText(sc::string::format( strTemp.GetString(), m_vecItemList.size(), wCurrentSlotNum, ExpandAddNum ).c_str());

    }
    
}

void MarketRPPage::SetRegistCountText( UINT nNow, UINT nMax, UINT nExtended )
{
    if( m_pRegistCountText == NULL )
        return;

    m_pRegistCountText->SetText( sc::string::format( MARKET_RPPAGE_TEXT1, nNow, nMax, nExtended ).c_str() );
}

void MarketRPPage::SetSaleCommissionText( float fRate )
{
    if( m_pSaleCommissionText == NULL )
        return;

    std::string strTemp = sc::string::format( MARKET_RPPAGE_TEXT12, fRate );
    strTemp = strTemp + "  ";    //여백

    m_pSaleCommissionText->SetText( strTemp.c_str() );
}

void MarketRPPage::SetPeeText( const SNATIVEID ItemID )
{
    m_pRegistValue->ClearText(); 
    std::string strTemp = sc::string::format("%d%s%s%s", 0, INDENT, INDENT, INDENT );

    if( ItemID != NATIVEID_NULL() )
    {
        SINVENITEM* sInvenItem = m_pGaeaClient->GetCharacter()->m_cInventory.FindItem( ItemID );
        if( sInvenItem )
        {
            //WORD wFee = m_pGaeaClient->GetConsignmentSale()->GetFee( m_pPeriodComboBox->GetSelectIndex(), sInvenItem->CustomItem() );
            DWORD wFee = private_market::GLPrivateMarketDefine::Instance()->CalcFee( m_pPeriodComboBox->GetSelectIndex(), sInvenItem->CustomItem() );
            strTemp = sc::string::format("%s%s%s%s", NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( wFee, 3, "," ), INDENT, INDENT, INDENT );
        }
    }
    else
    {
        if( 0 < m_pPeriodGameMoney->GetSelectIndex() )
        {

            std::vector<private_market::GameMoneyItemFee> vecMoneyPeriodFee;
            vecMoneyPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();

            unsigned int nIndex = m_pPeriodGameMoney->GetSelectIndex();

            if( 0 < nIndex && nIndex <= vecMoneyPeriodFee.size() )
            {
                const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[nIndex-1];
                //WORD wFee = m_pGaeaClient->GetConsignmentSale()->GetFee( m_pPeriodComboBox->GetSelectIndex(), SITEMCUSTOM(value.sNativeID) );
                DWORD wFee = private_market::GLPrivateMarketDefine::Instance()->CalcFee( m_pPeriodComboBox->GetSelectIndex(), SITEMCUSTOM(value.sNativeID) );
                strTemp = sc::string::format("%s%s%s%s", NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( wFee, 3, "," ), INDENT, INDENT, INDENT );
            }

        }
    }

    m_pRegistValue->AddOneLineText( strTemp.c_str(), NS_UITEXTCOLOR::ORANGE );
}


void MarketRPPage::SetPeeGameMoneyText( unsigned int nIndex )
{
    m_pRegistValue->ClearText(); 
    std::string strTemp = sc::string::format("%d%s%s%s", 0, INDENT, INDENT, INDENT );

    std::vector<private_market::GameMoneyItemFee> vecMoneyPeriodFee;
    vecMoneyPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();

    if( 0 < nIndex && nIndex <= vecMoneyPeriodFee.size() )
    {
        const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[nIndex-1];

        SITEM* pItem = GLItemMan::GetInstance().GetItem( value.sNativeID );

        if( value.sNativeID != NATIVEID_NULL() )
        {
            strTemp = sc::string::format("%d%s%s%s", value.wFee, INDENT, INDENT, INDENT );
        }

    }


    m_pRegistValue->AddOneLineText( strTemp.c_str(), NS_UITEXTCOLOR::ORANGE );


}

void MarketRPPage::SetImage( const SNATIVEID ItemID, WORD wTurnNum )
{
    m_pItemImage->SetItem( ItemID );
    m_pItemImage->SetTurnNum( wTurnNum );    
}

void MarketRPPage::SetImage( const SITEMCUSTOM& sItemCustom )
{
	m_pItemImage->SetItem(sItemCustom);
	m_pItemImage->SetTurnNum(sItemCustom.TurnNum());
}

void MarketRPPage::ResetSaleCountBox()
{
    if( m_pSaleCount == NULL )
        return;

    //m_pSaleCount->SetEditString( MARKET_RPPAGE_TEXT28 );

	m_pSaleCount->ClearEdit();
	m_pSaleCountText->SetText( sc::string::format("%s", MARKET_RPPAGE_TEXT28 ).c_str() );
}

void MarketRPPage::ResetSalePriceBox()
{
    if( m_pSalePrice == NULL )
        return;

    //m_pSalePrice->SetEditString( MARKET_RPPAGE_TEXT27 );

	WORD wMinPrice = private_market::GLPrivateMarketDefine::Instance()->RanPointSaleMinPrice();

	m_pSalePrice->ClearEdit();
	m_pSalePriceText->SetText( sc::string::format( MARKET_RPPAGE_TEXT27, wMinPrice ).c_str() );

}

bool MarketRPPage::RegistCheck()
{

    if( m_sInvenItem.GetNativeID() == NATIVEID_NULL() )
    {
        OpenMessageBox(CMessageBox::TITLE_ERROR, MARKET_RPPAGE_TEXT25, CMessageBox::TYPE_OK );
        return false;
    }

    //WORD wFee = m_pGaeaClient->GetConsignmentSale()->GetFee( m_pPeriodComboBox->GetSelectIndex(), m_sInvenItem.sItemCustom );
    DWORD wFee = private_market::GLPrivateMarketDefine::Instance()->CalcFee( m_pPeriodComboBox->GetSelectIndex(), m_sInvenItem.sItemCustom );

    if( !m_pSaleCount && !m_pSalePrice  )
        return false;

    int nCount=0;
    if( m_pSaleCount->GetEditLength() != 0 )
        nCount = _ttoi( m_pSaleCount->GetEditString() );

    LONGLONG llPrice=0;
    if( m_pSalePrice->GetEditLength() != 0 )
        llPrice = _ttoi64( m_pSalePrice->GetEditString() );


    // 확장 슬롯 시간 계산
    WORD wCurrentSlotNum = private_market::GLPrivateMarketDefine::Instance()->SellResgisterBasicNum();

    const __time64_t& tExpansionDate = m_pGaeaClient->GetCharacterData().GetConsignmentSaleSlotExpansionDate();    
    const __time64_t& tCurTime = CTime::GetCurrentTime().GetTime();

    if (0 < tExpansionDate - tCurTime)
        wCurrentSlotNum += private_market::GLPrivateMarketDefine::Instance()->SellResgisterExpandAddNum();

    if( wCurrentSlotNum <= m_vecItemList.size() )
    {
        OpenMessageBox(CMessageBox::TITLE_ERROR, MARKET_RPPAGE_TEXT39, CMessageBox::TYPE_OK );
        return false;
    }


    WORD wMinPrice = private_market::GLPrivateMarketDefine::Instance()->RanPointSaleMinPrice();
	
	std::string strMinPrice = sc::string::format( MARKET_RPPAGE_TEXT27, wMinPrice );

    if( m_pCheckBoxPoint->IsChecked() && llPrice < wMinPrice )
    {
        OpenMessageBox(CMessageBox::TITLE_ERROR, strMinPrice, CMessageBox::TYPE_OK );
        ResetSalePriceBox();
        return false;
        //m_pSalePrice->SetEditString( sc::string::format("%1%", wMinPrice).c_str());
    }
    if( llPrice == 0 )
    {
        OpenMessageBox(CMessageBox::TITLE_ERROR, strMinPrice, CMessageBox::TYPE_OK );
        return false;
    }
    if( nCount == 0 )
    {
        OpenMessageBox(CMessageBox::TITLE_ERROR, MARKET_RPPAGE_TEXT28, CMessageBox::TYPE_OK );
        return false;
    }
    if( llPrice > LL_LIMITED_A_UNIT_PRICE )
    {
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("PRIVATE_LIMITED_A_UINIT_PRICE") );
        return false;
    }

    // 수수료 부족
    if( m_pGaeaClient->PointShopChaPoint() < wFee )
    {
        OpenMessageBox(CMessageBox::TITLE_ERROR, MARKET_RPPAGE_MESSAGE6, CMessageBox::TYPE_OK );
        return false;
    }

    if( nCount <= 0 || llPrice <= 0 )
    {
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, MARKET_RPPAGE_TEXT34 );
        return false;
    }


    m_llPrice = llPrice;
    m_nCount = nCount;

    return true;
}

