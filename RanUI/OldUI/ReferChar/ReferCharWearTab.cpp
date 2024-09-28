#include "StdAfx.h"
#include "ReferCharWearTab.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogicClient/ReferChar/ReferChar.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../Ranlogicclient/GLGaeaClient.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/MSG/GLContrlMsg.h"
#include "../../../RanLogic/s_NetGlobal.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../Item/ItemImage.h"
#include "../Util/CheckBox.h"
#include "../Util/RnButton.h"
#include "../Char/CharacterView.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    float PART_INTERVAL_DEFAULT = 2.5f; // 줄 간격 윈도우 크기가 달라지면 계산 필요
    float PART_INTERVAL_PREVIEW = 6.0f; // 입혀보기, 비교하기, 연마수치 표시의 경우 간격이 다름
}

CReferCharWearTab::CReferCharWearTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIGroupHelper(pEngineDevice)

	, m_pInterface( pInterface )
	, m_pGaeaClient( pGaeaClient )

	, m_pCharacterView( NULL )

	, m_pTarget( NULL )

	, m_pCheckBoxAllWear( NULL )
	, m_pCheckBoxPreview( NULL )
	, m_pCheckBoxCompare( NULL )
	, m_pCheckBoxGrade( NULL )
	, m_pAnotherCharBadge( NULL )
	, m_pAnotherCharLevel( NULL )
	, m_pAnotherCharStatus( NULL )
	, m_pAnotherCharAbility( NULL )
	, m_pAnotherCharResistance( NULL )
	, m_pMyCharLevel( NULL )
	, m_pMyCharStatus( NULL )
	, m_pMyCharAbility( NULL )
	, m_pMyCharResistance( NULL )
{
    ZeroMemory( m_pArrayItemImage, sizeof( m_pArrayItemImage ) );
    ZeroMemory( m_pArrayCheckBox, sizeof( m_pArrayCheckBox ) );
    ZeroMemory( m_pArrayGradeCount, sizeof( m_pArrayGradeCount ) );
    ZeroMemory( m_pArrayCartImage, sizeof( m_pArrayCartImage ) );
}

void CReferCharWearTab::CreateSubControl()
{
    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFontShadow9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

    // 영역 분할
    CreateLineBox( "ANOTHER_CHARACTER_WEAR_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    CreateLineBox( "ANOTHER_CHARACTER_CHECK_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );

	CreateLineBox( "ANOTHER_CHARACTER_BADGE_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    CreateLineBox( "ANOTHER_CHARACTER_LEVEL_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    CreateLineBox( "ANOTHER_CHARACTER_MYLEVEL_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    CreateLineBox( "ANOTHER_CHARACTER_STATUS_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    CreateLineBox( "ANOTHER_CHARACTER_MYSTATUS_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    CreateLineBox( "ANOTHER_CHARACTER_ABILITY_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    CreateLineBox( "ANOTHER_CHARACTER_MYABILITY_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    CreateLineBox( "ANOTHER_CHARACTER_RESISTANCE_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    CreateLineBox( "ANOTHER_CHARACTER_MYRESISTANCE_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );

    CreateLineBox( "ANOTHER_CHARACTER_WISHLIST_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );

	m_pCharacterView = new CharacterView( m_pEngineDevice );
	m_pCharacterView->CreateSub( this, "ANOTHER_CHARACTER_WEAR_REGION", UI_FLAG_DEFAULT );
	RegisterControl( m_pCharacterView );


    
    const char* const strImageRegion[WEAR_COUNT] =
    {   
        "ANOTHER_CHARACTER_ITEMSLOT_HAT",
        "ANOTHER_CHARACTER_ITEMSLOT_TOP",
        "ANOTHER_CHARACTER_ITEMSLOT_BELT",
        "ANOTHER_CHARACTER_ITEMSLOT_PANTS",
        "ANOTHER_CHARACTER_ITEMSLOT_GLOVES",
        "ANOTHER_CHARACTER_ITEMSLOT_SHOES",
        "ANOTHER_CHARACTER_ITEMSLOT_WEAPON",
        "ANOTHER_CHARACTER_ITEMSLOT_EARRINGS1",
        "ANOTHER_CHARACTER_ITEMSLOT_EARRINGS2",
        "ANOTHER_CHARACTER_ITEMSLOT_NEACKLACE",
        "ANOTHER_CHARACTER_ITEMSLOT_BRACELET",
        "ANOTHER_CHARACTER_ITEMSLOT_RING1",
        "ANOTHER_CHARACTER_ITEMSLOT_RING2",
        "ANOTHER_CHARACTER_ITEMSLOT_ACCESSORY1",
        "ANOTHER_CHARACTER_ITEMSLOT_ACCESSORY2",
    };

    const char* const strBackImage[WEAR_COUNT] =
    {
        "ANOTHER_CHARACTER_BACK_HAT",
        "ANOTHER_CHARACTER_BACK_TOP",
        "ANOTHER_CHARACTER_BACK_BELT",
        "ANOTHER_CHARACTER_BACK_PANTS",
        "ANOTHER_CHARACTER_BACK_GLOVES",
        "ANOTHER_CHARACTER_BACK_SHOES",
        "ANOTHER_CHARACTER_BACK_WEAPON",
        "ANOTHER_CHARACTER_BACK_EARRINGS1",
        "ANOTHER_CHARACTER_BACK_EARRINGS2",
        "ANOTHER_CHARACTER_BACK_NEACKLACE",
        "ANOTHER_CHARACTER_BACK_BRACELET",
        "ANOTHER_CHARACTER_BACK_RING1",
        "ANOTHER_CHARACTER_BACK_RING2",
        "ANOTHER_CHARACTER_BACK_ACCESSORY1",
        "ANOTHER_CHARACTER_BACK_ACCESSORY2",
    };

    const char* const strCheckBox[WEAR_COUNT] =
    {
        "ANOTHER_CHARACTER_CHECKBOX_HAT",
        "ANOTHER_CHARACTER_CHECKBOX_TOP",
        "ANOTHER_CHARACTER_CHECKBOX_BELT",
        "ANOTHER_CHARACTER_CHECKBOX_PANTS",
        "ANOTHER_CHARACTER_CHECKBOX_GLOVES",
        "ANOTHER_CHARACTER_CHECKBOX_SHOES",
        "ANOTHER_CHARACTER_CHECKBOX_WEAPON",
        "ANOTHER_CHARACTER_CHECKBOX_EARRINGS1",
        "ANOTHER_CHARACTER_CHECKBOX_EARRINGS2",
        "ANOTHER_CHARACTER_CHECKBOX_NEACKLACE",
        "ANOTHER_CHARACTER_CHECKBOX_BRACELET",
        "ANOTHER_CHARACTER_CHECKBOX_RING1",
        "ANOTHER_CHARACTER_CHECKBOX_RING2",
        "ANOTHER_CHARACTER_CHECKBOX_ACCESSORY1",
        "ANOTHER_CHARACTER_CHECKBOX_ACCESSORY2",
    };


    {
        // 모든 장비 착용 체크박스
        CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice ); 
        pCheckBox->CreateSub( this, "ANOTHER_CHARACTER_CHECKBOX_ALLWEAR" , UI_FLAG_DEFAULT , ID_CHECKBOX_ALLWEAR );
        pCheckBox->CreateSubControl ( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
        pCheckBox->SetCheck( TRUE );
        RegisterControl( pCheckBox );
        m_pCheckBoxAllWear = pCheckBox;

        // 텍스트
        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_CHECKBOX_ALLWEAR_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetText( sc::string::format( "   %s", ID2GAMEWORD("REFER_WEARTAB", 0 ) ).c_str() );
        RegisterControl( pTextBox );
    }

    //착용하고 있는 장비
    for( int i=WEAR_HAT; i<WEAR_COUNT; ++i )
    {
        CreateLineBox( strImageRegion[ i ], strBackImage[ i ] );

        CItemImage* pItemImage = new CItemImage( m_pGaeaClient, m_pEngineDevice );
        pItemImage->CreateSub( this, strImageRegion[ i ], UI_FLAG_DEFAULT , ID_ITEMIMG_STATR + i );
        pItemImage->CreateSubControl();
        RegisterControl( pItemImage );
        m_pArrayItemImage[ i ] = pItemImage;

        CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice ); 
        pCheckBox->CreateSub( this, strCheckBox[ i ], UI_FLAG_DEFAULT , ID_CHECKBOX_START + i );
        pCheckBox->CreateSubControl( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
        pCheckBox->SetCheck( TRUE );
        RegisterControl( pCheckBox );
        m_pArrayCheckBox[ i ] = pCheckBox;

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, strImageRegion[ i ] );
        pTextBox->SetFont ( pFontShadow9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_BOTTOM | TEXT_ALIGN_LEFT );
        RegisterControl ( pTextBox );
        m_pArrayGradeCount[ i ] = pTextBox;
    }

    {
        // 미리보기
        CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice ); 
        pCheckBox->CreateSub( this, "ANOTHER_CHARACTER_CHECK_PREVIEW", UI_FLAG_DEFAULT, ID_CHECKBOX_PREVIEW );
        pCheckBox->CreateSubControl( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
        pCheckBox->SetCheck( FALSE );
        RegisterControl( pCheckBox );
        m_pCheckBoxPreview = pCheckBox;

        // 비교하기
        pCheckBox = new CCheckBox( m_pEngineDevice ); 
        pCheckBox->CreateSub( this, "ANOTHER_CHARACTER_CHECK_COMPARE", UI_FLAG_DEFAULT, ID_CHECKBOX_COMPARE );
        pCheckBox->CreateSubControl( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
        pCheckBox->SetCheck( TRUE );
        RegisterControl( pCheckBox );
        m_pCheckBoxCompare = pCheckBox;

        // 강화수치 표시
        pCheckBox = new CCheckBox( m_pEngineDevice ); 
        pCheckBox->CreateSub( this, "ANOTHER_CHARACTER_CHECK_INTENSION", UI_FLAG_DEFAULT, ID_CHECKBOX_GRADE );
        pCheckBox->CreateSubControl( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
        pCheckBox->SetCheck( TRUE );
        RegisterControl( pCheckBox );
        m_pCheckBoxGrade = pCheckBox;

        // 텍스트 영역
        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_CHECK_REGION_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont8 );		
        pTextBox->SetSensitive( true );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_PREVIEW );
        RegisterControl( pTextBox );
        //std::string strTemp = ;
        pTextBox->AddText( sc::string::format( "     %s", ID2GAMEWORD("REFER_WEARTAB", 1 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "     %s", ID2GAMEWORD("REFER_WEARTAB", 2 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "     %s", ID2GAMEWORD("REFER_WEARTAB", 3 ) ).c_str() );
    }

    {
        // 뱃지
        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_BADGE_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetText( sc::string::format( "  %s", ID2GAMEWORD("REFER_WEARTAB", 4 ) ).c_str() );
        RegisterControl( pTextBox );


        // 상대방 뱃지 이름
        pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_BADGE_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        m_pAnotherCharBadge = pTextBox;
    }

    {
        // 레벨
        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_LEVEL_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        pTextBox->SetText( sc::string::format( "  %s", ID2GAMEWORD("REFER_WEARTAB", 5 ) ).c_str() );

        // 상대방
        pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_LEVEL_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        m_pAnotherCharLevel = pTextBox;

        // 나의
        pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_MYLEVEL_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        m_pMyCharLevel = pTextBox;
    }

    {
        // 스테이터스 텍스트
        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_STATUS_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STAT_STATIC", 0 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STAT_STATIC", 1 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STAT_STATIC", 2 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STAT_STATIC", 4 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STAT_STATIC", 5 ) ).c_str() );
        
        // 상대방
        pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_STATUS_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        m_pAnotherCharStatus = pTextBox;

        // 나의
        pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_MYSTATUS_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        m_pMyCharStatus = pTextBox;
    }

    {
        // 능력치 정보
        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_ABILITY_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
		pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STATUS_STATIC", 15 ) ).c_str() );
		pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STATUS_STATIC", 14 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STATUS_STATIC", 3 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STATUS_STATIC", 4 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STATUS_STATIC", 5 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STATUS_STATIC", 11 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STATUS_STATIC", 9 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STATUS_STATIC", 10 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STATUS_STATIC", 6 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STATUS_STATIC", 7 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("CHARACTER_STATUS_STATIC", 8 ) ).c_str() );

        // 상대방
        pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_ABILITY_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT| TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        m_pAnotherCharAbility = pTextBox;

        // 나의
        pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_MYABILITY_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        m_pMyCharAbility = pTextBox;
    }

    {
        // 저항력 정보
        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_RESISTANCE_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("ITEM_ADVANCED_INFO_RESIST", 1 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("ITEM_ADVANCED_INFO_RESIST", 2 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("ITEM_ADVANCED_INFO_RESIST", 0 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("ITEM_ADVANCED_INFO_RESIST", 3 ) ).c_str() );
        pTextBox->AddText( sc::string::format( "  %s", ID2GAMEWORD("ITEM_ADVANCED_INFO_RESIST", 4 ) ).c_str() );

        // 상대방
        pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_RESISTANCE_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT| TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        m_pAnotherCharResistance = pTextBox;

        // 나의
        pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_MYRESISTANCE_REGION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y  );
        pTextBox->SetPartInterval( PART_INTERVAL_DEFAULT );
        RegisterControl( pTextBox );
        m_pMyCharResistance = pTextBox;
    }

    {   
        // 관심 상품 목록 ( 카트 )
        CString strTemp;
        for( int i=0; i<CART_MAX_COUNT; ++i )
        {
            strTemp.Format( "ANOTHER_CHARACTER_WISHLISTBOX_%d", i );
            CreateLineBox( strTemp.GetString(), "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );

            CItemImage* pItemImage = new CItemImage( m_pGaeaClient, m_pEngineDevice );
            pItemImage->CreateSub( this, strTemp.GetString(), UI_FLAG_DEFAULT , ID_CARTITEM_START + i );
            pItemImage->CreateSubControl();
            RegisterControl( pItemImage );
            m_pArrayCartImage[ i ] = pItemImage;
        }
    }

    { //버튼
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton* pRnButton = NULL;
        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
        //////////////////////////////////////////////////////////////////////////
        arg.pFont = pFont10Shadow;
        //arg.text = ID2GAMEWORD( "PRODUCT_BUTTON", 0 );
        arg.dwColor = NS_UITEXTCOLOR::WHITE;

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"ANOTHER_CHARACTER_BUTTON_CART",UI_FLAG_DEFAULT, ID_BUTTON_CART );
        arg.text = sc::string::format( "%s  ", ID2GAMEWORD("REFER_WEARTAB", 6 ) );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        {
            //담기 버튼 물음표
            CUIControl* pControl = new CUIControl( m_pEngineDevice );
            pControl->CreateSub(this,"ANOTHER_CHARACTER_BUTTON_CART_HELP" , UI_FLAG_DEFAULT, ID_UICONTROL_CART_HELP );
            RegisterControl( pControl );
        }

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"ANOTHER_CHARACTER_BUTTON_WHISPER",UI_FLAG_DEFAULT, ID_BUTTON_WHISPER );
        arg.text = sc::string::format( "%s", ID2GAMEWORD("REFER_WEARTAB", 7 ) );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"ANOTHER_CHARACTER_BUTTON_ADDFRIEND",UI_FLAG_DEFAULT, ID_BUTTON_ADDFRIEND );
        arg.text =  sc::string::format( "%s", ID2GAMEWORD("REFER_WEARTAB", 8 ) );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"ANOTHER_CHARACTER_BUTTON_INVITEPARTY",UI_FLAG_DEFAULT, ID_BUTTON_INVITEPARTY );
        arg.text =  sc::string::format( "%s", ID2GAMEWORD("REFER_WEARTAB", 9 ) );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"ANOTHER_CHARACTER_BUTTON_REFRESH",UI_FLAG_DEFAULT, ID_BUTTON_REFRESH );
        arg.text =  sc::string::format( "%s", ID2GAMEWORD("REFER_WEARTAB", 10 ) );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
    }
}

void CReferCharWearTab::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    if( !m_pTarget )
		return;

//  	if ( ID_ITEMIMG_STATR <= ControlID && ControlID <= ID_ITEMIMG_END )
//  	{
//  		if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ))
//  		{
// 			if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED )
// 			{
// 				UINT nIndex = ControlID - ID_ITEMIMG_STATR;
// 				EMSLOT eSlot = WEAR2EMSLOT( nIndex );
// 				if( eSlot >= SLOT_TSIZE )
// 					return;
// 
// 				const SITEMCUSTOM& sItem = m_pTarget->GET_SLOT_ITEM( eSlot );
// 				const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sItem.GetNativeID() );
// 				if ( pItemData == NULL )
// 					return;
// 
// 				SSETITEM* pSetItem = const_cast<SSETITEM*>(GLItemSet::GetInstance().GetInfoSetItem(pItemData->BasicId()));
// 				if ( pSetItem )
// 				{
// 					m_pGaeaClient->GetInterface()->SetitemGroupComposition(pSetItem, m_pTarget->m_dwGlobalID);
// 					m_pGaeaClient->GetInterface()->OpenSetitemPreviewWindow();
// 					ResetMessageEx();
// 				}
// 			}
// 		}
//  	}

	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	// 장비 장착 체크박스
    if( ID_CHECKBOX_START <= ControlID && ControlID <= ID_CHECKBOX_END )
    {
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ))
        {
            // 현재 선택된 컨트롤의 아이디를 통해 Index를 구한다.
            // 체크박스의 ID는 ID_CHECKBOX_INDEX부터 시작함으로
            // ID_CHECKBOX_INDEX로 인덱스를 계산한다.
            UINT nIndex = ControlID - ID_CHECKBOX_INDEX;
            if( WEAR_COUNT <= nIndex )
                return;

            BOOL bChecked = m_pArrayCheckBox[ nIndex ]->IsChecked();
            m_pArrayItemImage[ nIndex ]->SetVisibleSingle( bChecked );
            m_pArrayGradeCount[ nIndex ]->SetVisibleSingle( bChecked );

            m_pCheckBoxAllWear->SetCheck( IsAllCheckBoxChecked() );

			Refresh();
        }
    }

    // 툴팁 표시
    if( ID_ITEMIMG_STATR <= ControlID && ControlID <= ID_ITEMIMG_END )
    {
        if( CHECK_MOUSE_IN( dwMsg ) )
        {
			UINT nIndex = ControlID - ID_ITEMIMG_STATR;
			EMSLOT eSlot = WEAR2EMSLOT( nIndex );
			if( eSlot >= SLOT_TSIZE )
				return;

            const SITEMCUSTOM& sItem = m_pTarget->GET_SLOT_ITEM( eSlot );
            if( sItem.GetNativeID() != NATIVEID_NULL() )
            {
				BOOL bChecked = m_pCheckBoxCompare->IsChecked();

                m_pInterface->SHOW_ITEM_INFO(
                    SITEMCUSTOM( sItem ),
                    FALSE, FALSE, FALSE, TRUE, bChecked,
                    USHRT_MAX, USHRT_MAX );
            }

			if( CHECK_RB_UP_LIKE(dwMsg) )
			{
				if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED )
				{
					const SITEMCUSTOM& sItem = m_pTarget->GET_SLOT_ITEM( eSlot );
					const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sItem.GetNativeID() );
					if ( pItemData == NULL )
						return;

					SSETITEM* pSetItem = const_cast<SSETITEM*>(GLItemSet::GetInstance().GetInfoSetItem(pItemData->BasicId()));
					if ( pSetItem )
					{
						STARGETID sTargetID(CROW_PC, m_pTarget->m_dwGlobalID);
//						m_pGaeaClient->GetInterface()->SetitemGroupComposition(pSetItem, sTargetID);
//						m_pGaeaClient->GetInterface()->OpenSetitemPreviewWindow();
						ResetMessageEx();
					}
				}
				else if ( sItem.GetNativeID() != NATIVEID_NULL() )
				{
					if( DxInputDevice::GetInstance().GetKeyState( DIK_LSHIFT ) & DXKEY_PRESSED )
					{
						WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
						if( pWishList )
							pWishList->NewReferCharItem( sItem, std::string( m_pTarget->m_szName ) );
						return;
					}
				}
			}

			if( CHECK_RB_UP_LIKE( dwMsg ) )
			{
				const SITEMCUSTOM& sItem = m_pTarget->GET_SLOT_ITEM( eSlot );
				if( sItem.GetNativeID() != NATIVEID_NULL() )
					InsertWishList( sItem );
				return;
			}
        }
    }

	// 위시리스트
	if( ID_CARTITEM_START <= ControlID && ControlID <= ID_CARTITEM_END )
	{
		if( CHECK_MOUSE_IN( dwMsg ) )
        {
			UINT nIndex = ControlID - ID_CARTITEM_START;

			if( m_sCartItem[ nIndex ].GetNativeID() != NATIVEID_NULL() )
			{
				BOOL bChecked = m_pCheckBoxCompare->IsChecked();

				m_pInterface->SHOW_ITEM_INFO(
                    m_sCartItem[ nIndex ],
                    FALSE, FALSE, FALSE, TRUE, bChecked,
                    USHRT_MAX, USHRT_MAX );

				if( CHECK_RB_UP_LIKE( dwMsg ) )
					EraseWishList( nIndex );
			}
		}
	}

    switch( ControlID )
    {
    case ID_CHECKBOX_ALLWEAR:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
			BOOL bChecked = m_pCheckBoxAllWear->IsChecked();
			SetAllCheckBox( bChecked );
			SetAllGradeTextVisible( bChecked );

			Refresh();
        }
        break;

	case ID_CHECKBOX_PREVIEW:
		if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
		{
			Refresh();
		}
		break;

	case ID_CHECKBOX_GRADE:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
			BOOL bChecked = m_pCheckBoxGrade->IsChecked();
			SetAllGradeTextVisible( bChecked );
        }
        break;

	case ID_BUTTON_CART:
        if( CHECK_MOUSE_IN( dwMsg ) )
        {
			if( CHECK_LB_UP_LIKE( dwMsg ) )
				MoveWishInventory();
        }
        break;

	case ID_UICONTROL_CART_HELP:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->MultiLineCommentUtil(
				ID2GAMEINTEXT( "REFER_CHAR_CART_HELP" ),
				m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG ),
				800.0f,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,
				NS_UITEXTCOLOR::LIGHTSKYBLUE );
		}
		break;

	case ID_BUTTON_WHISPER:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
			m_pInterface->GetBasicChatWindow()->BeginPrivateChat( m_pTarget->m_szName );
			m_pInterface->ADD_FRIEND_NAME_TO_EDITBOX( m_pTarget->m_szName );
        }
        break;

	case ID_BUTTON_ADDFRIEND:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
			m_pInterface->ADD_FRIEND( m_pTarget->m_szName );
        }
        break;

	case ID_BUTTON_INVITEPARTY:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {

			std::string strText = sc::string::format( ID2GAMEINTEXT("PARTY_QUESTION"), m_pTarget->m_szName );
			m_pInterface->DoPartyModal( strText, UI::MODAL_PARTY_REQ, EMPARTY_MODE_PARTY );
			
        }
        break;

	case ID_BUTTON_REFRESH:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
			if( pReferChar )
				pReferChar->Refresh();
        }
        break;
    }
}

void CReferCharWearTab::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if( !IsVisible() )
		return;

	if( m_pTarget )
		m_pCharacterView->SetUpdateCharacter( &m_cRenderTarget, false );

	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CReferCharWearTab::Refresh()
{
    ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
    if( !pReferChar )
        return;

    m_pTarget = pReferChar->GetCompleteData();
	m_pCharacterView->SetVisibleSingle( m_pTarget ? TRUE : FALSE );

	SetCharacterData();
    SetAnotherCharInformation();
    SetMyCharInformation();
}

void CReferCharWearTab::SetVisibleSingle( BOOL isVisible )
{
    CUIGroupHelper::SetVisibleSingle(isVisible);
    if( isVisible )
    {
        UpdateCharacterView();
    }
}

void CReferCharWearTab::UpdateCharacterView()
{
	if( m_pTarget && m_pCharacterView->GetDevice() )
		m_pCharacterView->SetUpdateCharacter( &m_cRenderTarget, true );
}

void CReferCharWearTab::DeleteUIMeshData()
{
    m_pCharacterView->DeleteUIMeshData();
}


EMSLOT CReferCharWearTab::WEAR2EMSLOT( int nIndex )
{
    // RnCharacterWearWindow::IMAGE2EMSLOT( int nIndex ); 함수를 옮겨서 수정함
    switch ( nIndex )
    {	
    case WEAR_HAT:          return SLOT_HEADGEAR;	    // 머리
    case WEAR_TOP:	        return SLOT_UPPER;		    // 상의
    case WEAR_PANTS:        return SLOT_LOWER;          // 하의
    case WEAR_GLOVES:       return SLOT_HAND;           // 장갑
    case WEAR_SHOES:        return SLOT_FOOT;           // 신발
    case WEAR_WEAPON:       return SLOT_RHAND;          // 오른손도구
    case WEAR_RING_A:       return SLOT_RFINGER;        // 오른손 손가락	
    case WEAR_RING_B:       return SLOT_LFINGER;        // 왼손 손가락
    case WEAR_BRACELET:     return SLOT_WRIST;          // 손목
    case WEAR_NECKLACE:	    return SLOT_NECK;		    // 목걸이
    //case ITEM_IMAGE_LEFT_WEAPON:	return SLOT_LHAND;  // 왼손도구
    //case ITEM_IMAGE_LEFT_WEAPON_S:	return SLOT_LHAND_S;//	오른손도구, 극강부
    //case ITEM_IMAGE_RIGHT_WEAPON_S:	return SLOT_RHAND_S;//	왼손도구, 극강부
    //case ITEM_IMAGE_VEHICLE:	return SLOT_VEHICLE;    //  탈것		

    // 차후 구현될것들
    case WEAR_BELT:         return SLOT_WAISTBAND;
    case WEAR_ACCESSORY_A:  return SLOT_RACCESSORIES;
    case WEAR_ACCESSORY_B:  return SLOT_LACCESSORIES;
    case WEAR_DECORATION:  return SLOT_DECORATION;
    case WEAR_EARRINGS:  return SLOT_EARRINGS;
        break;
    }

    return SLOT_TSIZE;
}

void CReferCharWearTab::SetCharacterData()
{
	if( !m_pTarget )
		return;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return;

	BOOL bChecked = m_pCheckBoxPreview->IsChecked();

	// RenderTarget
	{
		if( bChecked )
		{
			m_cRenderTarget = *pCharacter;
			CheckPutonMyCharacter( &m_cRenderTarget );
		}
		else
		{
			m_cRenderTarget = *m_pTarget;
			CheckPutonTargetCharacter( &m_cRenderTarget );
		}
	}

	// StatTarget
	{
		m_cStatTarget = *m_pTarget;

		if( !bChecked )
			CheckPutonTargetCharacter( &m_cStatTarget );

		m_cStatTarget.INIT_DATA( FALSE, FALSE );
		m_cStatTarget.UPDATE_DATA( 0.0f, 0.0f, TRUE );
	}
}

void CReferCharWearTab::CheckPutonMyCharacter( GLCHARLOGIC* pTarget )
{
	for( int i=WEAR_HAT; i<WEAR_COUNT; ++i )
	{
		m_pArrayItemImage[ i ]->SetDiffuse( D3DCOLOR_XRGB( 255, 255, 255 ) );

		EMSLOT eSlot = WEAR2EMSLOT( i );
		if( eSlot >= SLOT_TSIZE )
			continue;

		BOOL bChecked = m_pArrayCheckBox[ i ]->IsChecked();
		if( !bChecked )
			continue;

		if( m_pTarget->m_PutOnItems[ eSlot ].GetNativeID() == NATIVEID_NULL() )
			continue;

		if( pTarget->ACCEPT_ITEM( m_pTarget->m_PutOnItems[ eSlot ] ) )
		{
			pTarget->m_PutOnItems[ eSlot ] = m_pTarget->m_PutOnItems[ eSlot ];
		}
		else
		{
			m_pArrayItemImage[ i ]->SetDiffuse( D3DCOLOR_ARGB( 128, 255, 0, 0 ) );
		}
	}
}

void CReferCharWearTab::CheckPutonTargetCharacter( GLCHARLOGIC* pTarget )
{
	for( int i=WEAR_HAT; i<WEAR_COUNT; ++i )
	{
		m_pArrayItemImage[ i ]->SetDiffuse( D3DCOLOR_XRGB( 255, 255, 255 ) );

		EMSLOT eSlot = WEAR2EMSLOT( i );
		if( eSlot >= SLOT_TSIZE )
			continue;

		BOOL bChecked = m_pArrayCheckBox[ i ]->IsChecked();
		if( !bChecked )
			pTarget->m_PutOnItems[ eSlot ] = SITEMCUSTOM();
	}
}

void CReferCharWearTab::SetAnotherCharInformation()
{
	m_pAnotherCharBadge->ClearText();
	m_pAnotherCharLevel->ClearText();
	m_pAnotherCharStatus->ClearText();
    m_pAnotherCharAbility->ClearText();
    m_pAnotherCharResistance->ClearText();

	for( int i=WEAR_HAT; i<WEAR_COUNT; ++i )
	{
        m_pArrayItemImage[ i ]->ResetItem();
		m_pArrayGradeCount[ i ]->ClearText();
	}

	if( !m_pTarget )
		return;

	// 뱃지
	CString strTitle = m_cStatTarget.m_szTitle;
	if( strTitle.IsEmpty() )
		strTitle = ID2GAMEWORD( "CAHRWINDOW_TITLE", 0 );
	else
		strTitle = ID2GAMEINTEXT( strTitle.GetString() );
    m_pAnotherCharBadge->SetText( strTitle );

    // 레벨
    SetAnotherCharText( m_pAnotherCharLevel, m_cStatTarget.GETLEVEL() );

    // 힘, 민첩, 정신, 체력, 근력
	//2014.9.18 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014)
	const SCHARSTATS& sStats = m_cStatTarget.m_sSUMSTATS_SKILL;

    SetAnotherCharText( m_pAnotherCharStatus, sStats.wPow );
    SetAnotherCharText( m_pAnotherCharStatus, sStats.wDex );
    SetAnotherCharText( m_pAnotherCharStatus, sStats.wSpi );
    SetAnotherCharText( m_pAnotherCharStatus, sStats.wStr );
    SetAnotherCharText( m_pAnotherCharStatus, sStats.wSta );

	// 최소 공격력, 최대 공격력
	SetAnotherCharText( m_pAnotherCharAbility, m_cStatTarget.GETFORCE_LOW() );
	SetAnotherCharText( m_pAnotherCharAbility, m_cStatTarget.GETFORCE_HIGH() );

    // 방어력, 격투치, 사격치, 기력치, 명중률, 회피율, Hp, Mp, Sp
    SetAnotherCharText( m_pAnotherCharAbility, m_cStatTarget.m_nDEFENSE );
    SetAnotherCharText( m_pAnotherCharAbility, m_cStatTarget.m_sumPowerAttribute[SKILL::EMAPPLY_MELEE] );
    SetAnotherCharText( m_pAnotherCharAbility, m_cStatTarget.m_sumPowerAttribute[SKILL::EMAPPLY_RANGE] );
    SetAnotherCharText( m_pAnotherCharAbility, m_cStatTarget.m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC] );
    SetAnotherCharText( m_pAnotherCharAbility, m_cStatTarget.m_nHIT );
    SetAnotherCharText( m_pAnotherCharAbility, m_cStatTarget.m_nAVOID );
    SetAnotherCharText( m_pAnotherCharAbility, m_cStatTarget.GETMAXHP() );
    SetAnotherCharText( m_pAnotherCharAbility, m_cStatTarget.GETMAXMP() );
    SetAnotherCharText( m_pAnotherCharAbility, m_cStatTarget.GETMAXSP() );

    // 화염, 냉기, 전기, 맹독, 정기
	const SRESIST& sResist = m_cStatTarget.m_sSUMRESIST;
    SetAnotherCharText( m_pAnotherCharResistance, sResist.nFire );
    SetAnotherCharText( m_pAnotherCharResistance, sResist.nIce );
    SetAnotherCharText( m_pAnotherCharResistance, sResist.nElectric );
    SetAnotherCharText( m_pAnotherCharResistance, sResist.nPoison );
    SetAnotherCharText( m_pAnotherCharResistance, sResist.nSpirit );

    for( int i=WEAR_HAT; i<WEAR_COUNT; ++i )
    {
		EMSLOT eSlot = WEAR2EMSLOT( i );
		if( eSlot >= SLOT_TSIZE )
			continue;

        SITEM* pItem = m_cStatTarget.GET_SLOT_ITEMDATA( eSlot );
		if( !pItem )
            continue;

        SetItemImage( i, pItem );

        //무기까지만 강화가 존재한다. 장신구류는 강화가 없다.
        //if( i <= WEAR_WEAPON )
		BOOL bGrinding = (pItem->sBasicOp.emItemType == ITEM_SUIT) && (pItem->sSuitOp.wReModelNum > 0);

		if( bGrinding )
		{
			const SITEMCUSTOM& sItem = m_cStatTarget.GET_SLOT_ITEM( eSlot );
            SetGradeText( i, sItem );
		}
    }
}   

void CReferCharWearTab::SetAnotherCharText( CBasicTextBoxEx* pTextBox, int wData )
{
    if( !pTextBox )
        return;

    //텍스트 영역에 대한 영역(XML데이터)을 따로 사용하지 않기 때문에
    //문자열 공백으로 여백을 조절한다.
    std::string strRightMargin = "  ";

    CString strFormat;
    strFormat.Format( "%d%s", wData, strRightMargin.c_str() );//, strRightMargin);
    pTextBox->AddText( strFormat, NS_UITEXTUTIL::ENABLE );
}

void CReferCharWearTab::SetMyCharInformation()
{
	m_pMyCharLevel->ClearText();
	m_pMyCharStatus->ClearText();
    m_pMyCharAbility->ClearText();
    m_pMyCharResistance->ClearText();

	if( !m_pTarget )
		return;

	const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
    if( !pCharacter )
        return;

	const GLCHARLOGIC& sMyChar = *pCharacter;

	// 레벨
	int nLevel = sMyChar.GETLEVEL() - m_cStatTarget.GETLEVEL();
    SetAnotherCharText( m_pMyCharLevel, nLevel );

    // 힘, 민첩, 정신, 체력, 근력
	//2014.9.18 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014)
	int nPow, nDex, nSpi, nStr, nSta;
    nPow = sMyChar.m_sSUMSTATS_SKILL.wPow - m_cStatTarget.m_sSUMSTATS_SKILL.wPow;
    nDex = sMyChar.m_sSUMSTATS_SKILL.wDex - m_cStatTarget.m_sSUMSTATS_SKILL.wDex;
    nSpi = sMyChar.m_sSUMSTATS_SKILL.wSpi - m_cStatTarget.m_sSUMSTATS_SKILL.wSpi;
    nStr = sMyChar.m_sSUMSTATS_SKILL.wStr - m_cStatTarget.m_sSUMSTATS_SKILL.wStr;
    nSta = sMyChar.m_sSUMSTATS_SKILL.wSta - m_cStatTarget.m_sSUMSTATS_SKILL.wSta;

    SetMyCharText( m_pMyCharStatus, nPow );
    SetMyCharText( m_pMyCharStatus, nDex );
    SetMyCharText( m_pMyCharStatus, nSpi );
    SetMyCharText( m_pMyCharStatus, nStr );
    SetMyCharText( m_pMyCharStatus, nSta );

	// 최소 공격력, 최대 공격력
	int nForceLow, nForceHigh;
	nForceLow = sMyChar.GETFORCE_LOW() - m_cStatTarget.GETFORCE_LOW();
	nForceHigh = sMyChar.GETFORCE_HIGH() - m_cStatTarget.GETFORCE_HIGH();
	SetMyCharText( m_pMyCharAbility, nForceLow );
	SetMyCharText( m_pMyCharAbility, nForceHigh );

    // 방어력, 격투치, 사격치, 기력치, 명중률, 회피율, Hp, Mp, Sp
	int nDP_Diff, nPA_Diff, nSA_Diff, nMA_Diff, nHit_Diff, nAvoid_Diff, nHP_Diff, nMP_Diff, nSP_Diff;
    nDP_Diff = sMyChar.m_nDEFENSE - m_cStatTarget.m_nDEFENSE;
    nPA_Diff = sMyChar.m_sumPowerAttribute[SKILL::EMAPPLY_MELEE] - m_cStatTarget.m_sumPowerAttribute[SKILL::EMAPPLY_MELEE];
    nSA_Diff = sMyChar.m_sumPowerAttribute[SKILL::EMAPPLY_RANGE] - m_cStatTarget.m_sumPowerAttribute[SKILL::EMAPPLY_RANGE];
    nMA_Diff = sMyChar.m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC] - m_cStatTarget.m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC];
    nHit_Diff = sMyChar.m_nHIT - m_cStatTarget.m_nHIT;
    nAvoid_Diff = sMyChar.m_nAVOID - m_cStatTarget.m_nAVOID;
    nHP_Diff = sMyChar.GETMAXHP() - m_cStatTarget.GETMAXHP();
    nMP_Diff = sMyChar.GETMAXMP() - m_cStatTarget.GETMAXMP();
    nSP_Diff = sMyChar.GETMAXSP() - m_cStatTarget.GETMAXSP();
    SetMyCharText( m_pMyCharAbility, nDP_Diff );
    SetMyCharText( m_pMyCharAbility, nPA_Diff );
    SetMyCharText( m_pMyCharAbility, nSA_Diff );
    SetMyCharText( m_pMyCharAbility, nMA_Diff );
    SetMyCharText( m_pMyCharAbility, nHit_Diff );
    SetMyCharText( m_pMyCharAbility, nAvoid_Diff );
    SetMyCharText( m_pMyCharAbility, nHP_Diff );
    SetMyCharText( m_pMyCharAbility, nMP_Diff );
    SetMyCharText( m_pMyCharAbility, nSP_Diff );

    // 화염, 냉기, 전기, 맹독, 정기
	SRESIST sResistDiff = sMyChar.m_sSUMRESIST - m_cStatTarget.m_sSUMRESIST;
    SetMyCharText( m_pMyCharResistance, sResistDiff.nFire );
    SetMyCharText( m_pMyCharResistance, sResistDiff.nIce );
    SetMyCharText( m_pMyCharResistance, sResistDiff.nElectric );
    SetMyCharText( m_pMyCharResistance, sResistDiff.nPoison );
    SetMyCharText( m_pMyCharResistance, sResistDiff.nSpirit );
}

void CReferCharWearTab::SetMyCharText( CBasicTextBoxEx* pTextBox, int wData )
{
    if( !pTextBox )
        return;

    std::string strLeftMargin = "  ";
    std::string strSymbol = " ";
    DWORD dwColor;

	if( 0 < wData )
    {
        strSymbol = "+";
        dwColor = D3DCOLOR_XRGB( 0, 255, 0 );
    }
    else if( wData < 0 )
    {
        dwColor = NS_UITEXTCOLOR::RED;
    }
    else
    {
        dwColor = NS_UITEXTUTIL::ENABLE;
    }

	CString strFormat;
    strFormat.Format( "%s%d%s", strSymbol.c_str(), wData, strLeftMargin.c_str() );
    pTextBox->AddText( strFormat, dwColor );
}

void CReferCharWearTab::SetItemImage( WORD eWear, SITEM* pItem )
{
    if( eWear < WEAR_HAT || eWear >= WEAR_COUNT )
        return;

    if( pItem ) 
    {
		m_pArrayItemImage[ eWear ]->SetItem( pItem->sBasicOp.sNativeID );
    }
    else
    {
        m_pArrayItemImage[ eWear ]->ResetItem();
    }   
}

void CReferCharWearTab::SetGradeText( WORD eWear, const SITEMCUSTOM& sItem )
{
    UINT nGrade = sItem.GETGRADE( EMGRINDING_NUNE );
    DWORD dwColor;

	if( nGrade == 0 )
    {
        dwColor = NS_UITEXTUTIL::ENABLE;
    }
    else if( nGrade <= 4 )
    {
        dwColor = D3DCOLOR_ARGB( 0xFF, 0xFF, 0xEA, 0x00);
    }
    else if( nGrade <= 9 )
    {
        dwColor = D3DCOLOR_ARGB( 0xFF, 0xFF, 0x00, 0x00);
    }
    else if( nGrade <= 254 )
    {
        dwColor = D3DCOLOR_ARGB( 0xFF, 0x00, 0xB4, 0xFF);
    }
    else
    {
        return;
    }

    CString strGrade;
    strGrade.Format("+%d", nGrade );
    m_pArrayGradeCount[ eWear ]->SetText( strGrade, dwColor );
}

BOOL CReferCharWearTab::IsAllCheckBoxChecked()
{
    for( int i=WEAR_HAT; i<WEAR_COUNT; ++i )
    {
        if( !m_pArrayCheckBox[ i ]->IsChecked() )
            return FALSE;
    }

    return TRUE;
}

void CReferCharWearTab::SetAllCheckBox( BOOL bCheck )
{
    for( int i=WEAR_HAT; i<WEAR_COUNT; ++i )
    {
        m_pArrayCheckBox[ i ]->SetCheck( bCheck );
        m_pArrayItemImage[ i ]->SetVisibleSingle( bCheck );
    }
}

void CReferCharWearTab::SetAllGradeTextVisible( BOOL bVisible )
{
    for( int i=WEAR_HAT; i<WEAR_COUNT; ++i )
    {
        m_pArrayGradeCount[ i ]->SetVisibleSingle( bVisible );
    }
}

void CReferCharWearTab::InsertWishList( const SITEMCUSTOM& sItem )
{
	SITEM* pItem = GLItemMan::GetInstance().GetItem( sItem.Id() );
	if( !pItem )
		return;

	for( int i=0; i<CART_MAX_COUNT; ++i )
	{
		if( m_sCartItem[ i ] == sItem )
		{
			m_pGaeaClient->PrintMsgText(
				NS_UITEXTCOLOR::RED,
				ID2GAMEINTEXT( "REFER_CHAR_EXIST_ITEM" ) );
			return;
		}
	}

	int iIndex = 0;
	for( ; iIndex < CART_MAX_COUNT; ++iIndex )
	{
		if( m_sCartItem[ iIndex ].GetNativeID() == NATIVEID_NULL() )
			break;
	}

	if( iIndex < CART_MAX_COUNT )
	{
		m_sCartItem[ iIndex ] = sItem;
		m_pArrayCartImage[ iIndex ]->SetItem( sItem );
		m_pArrayCartImage[ iIndex ]->SetVisibleSingle(TRUE);
	}
	else
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::RED,
			ID2GAMEINTEXT( "REFER_CHAR_FULL" ) );
	}
}

void CReferCharWearTab::EraseWishList( UINT nIndex )
{
	if( nIndex >= CART_MAX_COUNT )
		return;

	m_sCartItem[ nIndex ] = SITEMCUSTOM();
	m_pArrayCartImage[ nIndex ]->ResetItem();

	if( nIndex >= CART_MAX_COUNT-1 )
		return;

	for( UINT i=nIndex; i<CART_MAX_COUNT-1; ++i )
	{
		m_sCartItem[ i ] = m_sCartItem[ i + 1 ];
		m_pArrayCartImage[ i ]->SetItem( m_pArrayCartImage[ i + 1 ]->GetItemID() );

		m_sCartItem[ i + 1 ] = SITEMCUSTOM();
		m_pArrayCartImage[ i + 1 ]->ResetItem();

		m_pArrayCartImage[ i ]->SetVisibleSingle(m_pArrayCartImage[ i ]->GetItemID() != NATIVEID_NULL());
	}
}

void CReferCharWearTab::MoveWishInventory()
{
	if( !m_pTarget )
		return;

	WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
	if( !pWishList )
		return;

	for( int i=0; i<CART_MAX_COUNT; ++i )
	{
		if( m_sCartItem[ i ].GetNativeID() == NATIVEID_NULL() )
			break;

		pWishList->NewReferCharItem( m_sCartItem[ i ], std::string( m_pTarget->m_szName ) );

		m_sCartItem[ i ] = SITEMCUSTOM();
		m_pArrayCartImage[ i ]->ResetItem();
	}
}