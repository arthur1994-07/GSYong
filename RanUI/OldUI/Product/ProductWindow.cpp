#include "StdAfx.h"
#include "./ProductWindow.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SmartComboBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogicClient/Product/GLProductClient.h"

#include "../Util/CheckBox.h"
#include "../Util/RnButton.h"
#include "../Util/MessageBox.h"

#include "./ProductType.h"
#include "./ProductList.h"
#include "./ProductGoods.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CProductWindow::CProductWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIWindowEx( pInterface, pEngineDevice )
    , m_pInterface(pInterface)
    , m_pGaeaClient(pGaeaClient)
    , m_pGoods(NULL)
    , m_pList(NULL)
    , m_pType(NULL)
    , m_pTypeListText(NULL)
    , m_pGoodsName(NULL)
    , m_pClassFirterComboBoxRollOver(NULL)
    , m_pClassFirterComboBox(NULL)
    , m_pShowProductAbleCheckBox(NULL)
    , m_bIsShowProductAbleCheckBoxOn(false)
    , m_pProductionButton(NULL)
    , m_eFindClass(GLCL_NUM_CLASS)
    , m_bProducting(false)
    , m_pShowSkillCheckBox(NULL)
    , m_bIsShowSkillCheckBoxOn(false)
    , m_pStudyPointTextBox(NULL)
    , m_pHelpBox(NULL)
    , m_pHelpText_A(NULL)
    , m_pHelpText_B(NULL)
    , m_pHelpText_C(NULL)
    , m_pStudyMsgBox(NULL)
    , m_bDataInit(false)
{
}

void CProductWindow::CreateSubControl()
{
    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    { 
		/*
		// 제조분야 타이틀
        // X="8" Y="24"
        CBasicLineBoxSmart* pLinBoxSmartTypeTitle = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmartTypeTitle->CreateSub( this, "PRODUCT_WINDOW_TYPE_TITLE", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        pLinBoxSmartTypeTitle->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
        RegisterControl( pLinBoxSmartTypeTitle );

        // 타이틀 텍스트
        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, "PRODUCT_WINDOW_TYPE_TITLE", UI_FLAG_DEFAULT );
        pTextBox->SetFont ( pFont9 );	
        pTextBox->SetTextAlign( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y  );
        RegisterControl ( pTextBox );

        pTextBox->SetText( ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 1 ), NS_UITEXTCOLOR::LIGHTGREY );
		*/
	}

	{
		// hsshin 숙련도 시스템 추가 (2013.1.22)
		{
			//숙련도 정보 타이틀
			CBasicLineBoxSmart* pLinBoxSmartTypeTitle = new CBasicLineBoxSmart( m_pEngineDevice );
			pLinBoxSmartTypeTitle->CreateSub( this, "PRODUCT_WINDOW_SHOW_SKILL_TITLE", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
			pLinBoxSmartTypeTitle->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
			RegisterControl( pLinBoxSmartTypeTitle );

			//숙련도 정보 텍스트
			CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
			pTextBox->CreateSub( this, "PRODUCT_WINDOW_SHOW_SKILL_TITLE_TEXT", UI_FLAG_DEFAULT );
			pTextBox->SetFont ( pFont9 );	
			pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y  );
			RegisterControl ( pTextBox );
			pTextBox->SetText( ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 9 ), NS_UITEXTCOLOR::YELLOW );

			//숙련도 정보 보기버튼 (체크박스형태로 동작)
			CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
			pCheckBox->CreateSub( this, "PRODUCT_WINDOW_SHOW_SKILL_BUTTON", UI_FLAG_DEFAULT, ID_PRODUCT_SHOW_SKILL_CHECKBOX );
			pCheckBox->CreateSubControl ( "PRODUCT_WINDOW_SHOW_SKILL_BUTTON_CHECKED", "PRODUCT_WINDOW_SHOW_SKILL_BUTTON_UNCHECKED" );
			RegisterControl ( pCheckBox );
			m_pShowSkillCheckBox = pCheckBox;
		}

		{
			//학습정보 타이틀
			CBasicLineBoxSmart* pLinBoxSmartTypeTitle = new CBasicLineBoxSmart( m_pEngineDevice );
			pLinBoxSmartTypeTitle->CreateSub( this, "PRODUCT_WINDOW_STUDY_INFO_TITLE", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
			pLinBoxSmartTypeTitle->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
			RegisterControl( pLinBoxSmartTypeTitle );
			//학습정보 텍스트
			CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
			pTextBox->CreateSub(this, "PRODUCT_WINDOW_STUDY_INFO_TITLE", UI_FLAG_DEFAULT );
			pTextBox->SetFont ( pFont9 );	
			pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y  );
			RegisterControl ( pTextBox );

            WORD MaxPonit = m_pGaeaClient->GetProductClient()->GetProductTypeMaxStudyPoint();
			WORD CurrentPonit = m_pGaeaClient->GetProductClient()->GetProductTypeStudyPoint();
			CString strCombine = sc::string::format( ID2GAMEWORD( "PRODUCT_WINDOW_TEXT", 10 ), CurrentPonit, MaxPonit - CurrentPonit, MaxPonit ).c_str();

			pTextBox->SetText( strCombine, NS_UITEXTCOLOR::YELLOW );
            m_pStudyPointTextBox = pTextBox;
		}
	}

    {   // 제작목록 타이틀
        // X="8" Y="107"
        CBasicLineBoxSmart* pLinBoxSmartListTitle = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmartListTitle->CreateSub( this, "PRODUCT_WINDOW_LIST_TITLE", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        pLinBoxSmartListTitle->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
        RegisterControl( pLinBoxSmartListTitle );

        // 제작목록 텍스트
        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, "PRODUCT_WINDOW_LIST_TITLE", UI_FLAG_DEFAULT );
        pTextBox->SetFont ( pFont9 );	
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y  );
        RegisterControl ( pTextBox );
        m_pTypeListText = pTextBox;

    }

    {   // 제작물품 타이틀
        // X="8" Y="107"
        CBasicLineBoxSmart* pLinBoxSmartGoodsTitle = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmartGoodsTitle->CreateSub( this, "PRODUCT_WINDOW_GOODS_TITLE", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        pLinBoxSmartGoodsTitle->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
        RegisterControl( pLinBoxSmartGoodsTitle );

        // 제작물품
        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_TITLE", UI_FLAG_DEFAULT );
        pTextBox->SetFont ( pFont9 );	
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y  );
        RegisterControl ( pTextBox );
        m_pGoodsName = pTextBox;
    }

    {   // UI_그룹

        // 제조물품 x 252,y 135
        CProductGoods* pGoods = new CProductGoods( m_pGaeaClient, m_pEngineDevice, m_pInterface );
        pGoods->CreateSub( this, "PRODUCT_WINDOW_GOODS", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_PRODUCT_GOODS );
        pGoods->CreateSubControl(); // 함수내부에서 CreateSubControl이 다시 호출됨.
        RegisterControl( pGoods );
        m_pGoods = pGoods;

        // 제작목록 x 8, y 135
        CProductList* pList = new CProductList(m_pGaeaClient,  m_pEngineDevice, m_pInterface );
        pList->CreateSub( this, "PRODUCT_WINDOW_LIST",  UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_PRODUCT_LIST );
        pList->CreateSubControl(); // 함수내부에서 CreateSubControl이 다시 호출됨.
        RegisterControl( pList );
        m_pList = pList;

        // 제조분야 x 8, y 52
        CProductType* pType = new CProductType( m_pGaeaClient, m_pEngineDevice, m_pInterface );
        pType->CreateSub( this, "PRODUCT_WINDOW_TYPE",  UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_PRODUCT_TYPE );
        pType->CreateSubControl(); // 함수내부에서 CreateSubControl이 다시 호출됨.
        RegisterControl( pType );
        m_pType = pType;

    }

    {   // 체크박스
        CBasicLineBoxSmart* pLinBoxSmartTypeTitle = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmartTypeTitle->CreateSub( this, "PRODUCT_WINDOW_LIST_CHECKLINEBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        pLinBoxSmartTypeTitle->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
        RegisterControl( pLinBoxSmartTypeTitle );

        // 제작목록 텍스트
        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, "PRODUCT_WINDOW_LIST_CHECKLINEBOX_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont ( pFont9 );	
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
        RegisterControl ( pTextBox );
        std::string strTemp = sc::string::format( "      %s", ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 2 ) );
        pTextBox->SetText( strTemp.c_str() );

        m_pTypeListText->SetText( strTemp.c_str(), NS_UITEXTCOLOR::LIGHTGREY );

        //strControl.Format( "SNS_WINDOW_PAGE_TWITTER_CHECK_%d", i );
        CCheckBox* pCheckBox = new CCheckBox(m_pEngineDevice);
        pCheckBox->CreateSub ( this, "PRODUCT_WINDOW_LIST_CHECKBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_PRODUCT_FILTERING_CHECKBOX );
        pCheckBox->CreateSubControl ( "PRODUCT_WINDOW_LIST_CHECK", "PRODUCT_WINDOW_LIST_UNCHECK" );
        RegisterControl ( pCheckBox );
        m_pShowProductAbleCheckBox = pCheckBox;
    }

    {   // 생산하기 버튼
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton* pRnButton = NULL;
        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
        //////////////////////////////////////////////////////////////////////////
        arg.pFont = pFont10Shadow;
        arg.text = ID2GAMEWORD( "PRODUCT_BUTTON", 0 );
        arg.dwColor = NS_UITEXTCOLOR::WHITE;

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"PRODUCT_WINDOW_PRODUCTION_BUTTON",UI_FLAG_DEFAULT,ID_PRODUCT_PRODUCTION_BUTTON);
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pProductionButton = pRnButton;
    }

    {   // 콤보박스 
        // 소스코드 맨아래 위치 하지 않으면 출력시 사소한 우선권 문제가 있음
        CSmartComboBoxBasic* pComboBox; 
        pComboBox =  new CSmartComboBoxBasic(m_pEngineDevice);
        pComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_PRODUCT_FILTERING_COMBOBOX_OPEN);
        pComboBox->CreateSmartComboBox ( "PRODUCT_WINDOW_COMBOBOX_DESIGN", "PRODUCT_WINDOW_COMBOBOX_DESIGN_TEXINFO");
        RegisterControl( pComboBox );
        m_pClassFirterComboBox = pComboBox;

        CBasicComboBoxRollOver* pComboBoxRollOver;
        pComboBoxRollOver =  new CBasicComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_PRODUCT_FILTERING_COMBOBOX_ROLLOVER );
        pComboBoxRollOver->CreateBaseComboBoxRollOver( "PRODUCT_WINDOW_COMBOBOX_ROLLOVER_DESIGN");
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        pComboBoxRollOver->SetVisibleThumb( TRUE );
        RegisterControl( pComboBoxRollOver );
        m_pClassFirterComboBoxRollOver = pComboBoxRollOver;

        //int nMaxListFilter = 8;
        for(int nClassIndex=0; nClassIndex < GLCL_NUM_CLASS; nClassIndex++)
        {
            std::string strTemp = sc::string::format( " %s ", ID2GAMEWORD("PRODUCT_LIST_FILTER", nClassIndex ) );
            pComboBoxRollOver->AddText( strTemp.c_str() );
        }
        CString strTemp = m_pClassFirterComboBoxRollOver->GetSelectText( 0 );	
        pComboBox->SetText( strTemp );
    }
	{
		// hsshin 도움말 추가
		// 도움말 아이콘
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = "POINT_SHOP_HELP_BUTTON";
		arg.mouseOverTextureKey = "POINT_SHOP_HELP_BUTTON_F";
		arg.mouseClickTextureKey = "POINT_SHOP_HELP_BUTTON";
		//////////////////////////////////////////////////////////////////////////
		arg.dwColor = NS_UITEXTCOLOR::WHITE;

		RnButton* pHelpButton = new RnButton(m_pEngineDevice);
		pHelpButton->CreateSub(this, "PRODUCT_WINDOW_HELP_BUTTON", UI_FLAG_DEFAULT, ID_PRODUCT_HELP_BUTTON );
		pHelpButton->CreateSubControl(arg);
		RegisterControl(pHelpButton);

		// 도움말 윈도우타이틀
		CBasicLineBoxSmart* pLinBoxSmartTypeTitle = new CBasicLineBoxSmart( m_pEngineDevice );
		pLinBoxSmartTypeTitle->CreateSub( this, "PRODUCT_WINDOW_HELP_TEXTBOX", UI_FLAG_DEFAULT);
		pLinBoxSmartTypeTitle->CreateSubControl( "POINT_SHOP_TOOLTIP_BACK_LINE" );
		RegisterControl( pLinBoxSmartTypeTitle );
		m_pHelpBox = pLinBoxSmartTypeTitle;
		m_pHelpBox->SetVisibleSingle(FALSE);

		// 도움말 텍스트
		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub(this, "PRODUCT_WINDOW_HELP_TEXT_A", UI_FLAG_DEFAULT );
		pTextBox->SetFont ( pFont9 );	
		pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
		RegisterControl ( pTextBox );
		m_pHelpText_A = pTextBox;
		m_pHelpText_A->SetVisibleSingle(FALSE);

		CString strCombine = sc::string::format( ID2GAMEINTEXT( "PRODUCT_WINDOW_HELP_TEXT_STUDY" ) , " ").c_str();
		m_pHelpText_A->SetText( strCombine, NS_UITEXTCOLOR::WHITE );

		pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub(this, "PRODUCT_WINDOW_HELP_TEXT_B", UI_FLAG_DEFAULT );
		pTextBox->SetFont ( pFont9 );	
		pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
		RegisterControl ( pTextBox );
		m_pHelpText_B = pTextBox;
		m_pHelpText_B->SetVisibleSingle(FALSE);

		strCombine = sc::string::format( ID2GAMEINTEXT( "PRODUCT_WINDOW_HELP_TEXT_LOCKED" ) , " ").c_str();
		m_pHelpText_B->SetText( strCombine, NS_UITEXTCOLOR::WHITE );

		pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub(this, "PRODUCT_WINDOW_HELP_TEXT_C", UI_FLAG_DEFAULT );
		pTextBox->SetFont ( pFont9 );	
		pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
		RegisterControl ( pTextBox );
		m_pHelpText_C = pTextBox;
		m_pHelpText_C->SetVisibleSingle(FALSE);

		strCombine = sc::string::format( ID2GAMEINTEXT( "PRODUCT_WINDOW_HELP_TEXT_SKILL" ) , " ").c_str();
		m_pHelpText_C->SetText( strCombine, NS_UITEXTCOLOR::WHITE );
	}
	{
		// hsshin 제작분야 학습 모달형 메시지박스
		CMessageBox* pMessageBox = new CMessageBox( m_pEngineDevice );
		pMessageBox->CreateSub( this, "UTILE_MESSAGEBOX_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_PRODUCT_STUDY_MSGBOX );
		pMessageBox->CreateSubControl();
		RegisterControl( pMessageBox );
		m_pStudyMsgBox = pMessageBox;
	}

	CreateMessageBox();
}

void CProductWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB
                            , int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	IProductProgress* pProductProgress = m_pInterface->GetProductProgress();
	if( pProductProgress != NULL && pProductProgress->IsProdution() )
    {
		m_pShowProductAbleCheckBox->SetCheck( m_bIsShowProductAbleCheckBoxOn );
        m_pShowSkillCheckBox->SetCheck( m_bIsShowSkillCheckBoxOn );
    }

	if ( IsOpenMessageBox() )
	{
		m_pGoods->ShowRandomOptionMaterialBorder();

		IInventoryWindow* pInvenWindow = m_pInterface->GetInventoryWindow();
		if( pInvenWindow && pInvenWindow->IsLocked() )
			pInvenWindow->UnLockTrade();

		IInventory* pInventory = m_pInterface->GetInventory();
		if( pInventory && pInventory->IsLocked() )
			pInventory->UnLockTrade();
	}
}

void CProductWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	// hssshin 제작분야 학습 확인 메시지박스를 모달형태로 구현
	if ( m_pStudyMsgBox->IsVisible() )
	{
		return;
	}
    CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
    IProductProgress* pProductProgress = m_pInterface->GetProductProgress();
	if( pProductProgress != NULL && pProductProgress->IsProdution() )
	{
		if( ControlID != ID_PRODUCT_PRODUCTION_BUTTON )
			return;
	}

    switch( ControlID )
    {
    case ID_PRODUCT_FILTERING_CHECKBOX:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ))
        {
            m_pList->SetFilter( m_eFindClass, m_pShowProductAbleCheckBox->IsChecked() );
			m_bIsShowProductAbleCheckBoxOn = m_pShowProductAbleCheckBox->IsChecked();

            SetItemMix( m_pList->Select( 0 ) );
        }
        break;

    case ID_PRODUCT_FILTERING_COMBOBOX_OPEN:
        if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
        {				
            if( CHECK_LB_DOWN_LIKE( dwMsg ) )
            {
                if( m_pClassFirterComboBoxRollOver->IsVisible() == TRUE )
                    m_pClassFirterComboBoxRollOver->SetVisibleSingle( FALSE );
                else
                    m_pClassFirterComboBoxRollOver->SetVisibleSingle( TRUE );
            }
        }
        break;

    case ID_PRODUCT_FILTERING_COMBOBOX_ROLLOVER:
        if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) &&  m_pClassFirterComboBoxRollOver->IsVisible() == TRUE  )
        {				
            int nIndex = m_pClassFirterComboBoxRollOver->GetSelectIndex();
            if ( nIndex < 0 )
                return;

            CString strTemp = m_pClassFirterComboBoxRollOver->GetSelectText( nIndex );								
            m_pClassFirterComboBox->SetText( strTemp, NS_UITEXTCOLOR::WHITE );
            m_pClassFirterComboBoxRollOver->SetVisibleSingle( FALSE );

            MatchingEum( nIndex );

            m_pList->SetFilter( m_eFindClass, m_pShowProductAbleCheckBox->IsChecked() );

            SetItemMix( m_pList->Select( 0 ) );
        }
        break;

    case ID_PRODUCT_PRODUCTION_BUTTON:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            if( m_bProducting )
                m_pGoods->DoCancel();
            else
                m_pGoods->DoStart();
        }
        break;

	case ID_PRODUCT_SHOW_SKILL_CHECKBOX:
		if ( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
		{
			m_bIsShowSkillCheckBoxOn = m_pShowSkillCheckBox->IsChecked();
			m_pType->SetShowSkill(m_bIsShowSkillCheckBoxOn);
		}
		break;
	case ID_PRODUCT_HELP_BUTTON:
		if ( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pHelpBox->SetVisibleSingle(TRUE);
			m_pHelpText_A->SetVisibleSingle(TRUE);
			m_pHelpText_B->SetVisibleSingle(TRUE);
			m_pHelpText_C->SetVisibleSingle(TRUE);
		}
		else
		{
			m_pHelpBox->SetVisibleSingle(FALSE);
			m_pHelpText_A->SetVisibleSingle(FALSE);
			m_pHelpText_B->SetVisibleSingle(FALSE);
			m_pHelpText_C->SetVisibleSingle(FALSE);
		}
		break;
	case ID_PRODUCT_TYPE:
		if ( dwMsg & ProductTypeMSG::UI_MSG_PRODUCT_STUDY ) //학습창을 띄워달라는 요청메시지
		{
			// Type값이 추가가 된다면 들어오는 값들이 정확하게 매치되지 않을 가능성이 있음
			// (이유는 Enum Type과 게임워드 인덱스가 일치하지 않을 수 있기 때문)
			DWORD tmpType = dwMsg & (~ ProductTypeMSG::UI_MSG_PRODUCT_STUDY);   // UI_MSG_PRODUCT_STUDY 비트를 제거
            if ( tmpType <= Product::ETypeHeadGear || tmpType > Product::ETypeTotal )   // 혹시 잘못되어 예외적인 값이 들어오는걸 막음
            {
                return;
            }
			m_pStudyMsgBox->OpenMessageBox( ID2GAMEWORD("MODAL_TITLE", 1)
				, sc::string::format( ID2GAMEINTEXT("PRODUCT_WINDOW_STUDY_MSG") , ID2GAMEWORD("PRODUCT_TYPE_STRING",tmpType) )
				, CMessageBox::TYPE_YESNO, tmpType );

            // 다른 윈도우의 업데이트를 막아보자
            m_pList->SetNoUpdate(TRUE);
            m_pType->SetNoUpdate(TRUE);
            m_pGoods->SetNoUpdate(TRUE);
            m_pShowProductAbleCheckBox->SetNoUpdate(TRUE);
            m_pShowSkillCheckBox->SetNoUpdate(TRUE);
            m_pProductionButton->SetNoUpdate(TRUE);
		}
		break;
	case ID_PRODUCT_STUDY_MSGBOX:
		if ( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
		{
			DWORD dwStudyTapID = m_pStudyMsgBox->GetIdentifier();   // 배우려는 제작분야를 ID로 넘겨줌
            WORD  wStudyType = (WORD)dwStudyTapID -1;   // ID값은 +1 더해져 있기 때문에 빼줌 (0값을 방지하기위해 더했음)

            GLProductClient* pProduct = m_pGaeaClient->GetProductClient();
            if ( !pProduct )
            {
                return;
            }
            else
            {
                pProduct->ReqProductTypeAdd( wStudyType );
            }
            
            // 다른 윈도우의 업데이트를 허용
            m_pList->SetNoUpdate(FALSE);
            m_pType->SetNoUpdate(FALSE);
            m_pGoods->SetNoUpdate(FALSE);
            m_pShowProductAbleCheckBox->SetNoUpdate(FALSE);
            m_pShowSkillCheckBox->SetNoUpdate(FALSE);
            m_pProductionButton->SetNoUpdate(FALSE);
            Refresh();
		}
        else if ( dwMsg & UIMSG_MESSAGEBOX_NEGATIVE )
        {
            // 다른 윈도우의 업데이트를 허용
            m_pList->SetNoUpdate(FALSE);
            m_pType->SetNoUpdate(FALSE);
            m_pGoods->SetNoUpdate(FALSE);
            m_pShowProductAbleCheckBox->SetNoUpdate(FALSE);
            m_pShowSkillCheckBox->SetNoUpdate(FALSE);
            m_pProductionButton->SetNoUpdate(FALSE);
        }
		break;
	}
}

void CProductWindow::TranslateMessageBox( unsigned int nIdentity
										 , DWORD dwMsg )
{
	if ( !m_pGoods || !m_pInterface )
	{
		return;
	}

	switch( nIdentity )
	{
	case CProductWindow::MESSAGEBOX_CONFIRM_PRODUCT:
		if ( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
		{
			m_pGoods->DoStart( true );
		}
		else if ( dwMsg & UIMSG_MESSAGEBOX_NEGATIVE )
		{
			m_pGoods->DoCancel();

			m_pInterface->MsgProductCancel();
		}
		break;
	}
}

void CProductWindow::SetVisibleSingle( BOOL bVisible )
{
    CUIWindowEx::SetVisibleSingle( bVisible );

    if( bVisible )
    {
        if ( !m_bDataInit ) // 최초1회 창을 열었을 때 (접속과 동시에 자동적으로 발생)
        {
            m_pGaeaClient->GetProductClient()->ReqProductRecipeList();
            m_pGaeaClient->GetProductClient()->ReqProductTypeList();
            m_pGaeaClient->GetProductClient()->ReqProductExpList();

            SetSeletedTabID( Product::ETypeTotal );

            m_bDataInit = true;
        }
        Refresh();
    }
}

void CProductWindow::Refresh()
{
    if( !IsVisible() )
        return;

    WORD MaxPonit = m_pGaeaClient->GetProductClient()->GetProductTypeMaxStudyPoint();
    WORD CurrentPonit = m_pGaeaClient->GetProductClient()->GetProductTypeStudyPoint();
    CString strCombine = sc::string::format( ID2GAMEWORD( "PRODUCT_WINDOW_TEXT", 10 ), CurrentPonit, MaxPonit - CurrentPonit, MaxPonit ).c_str();
    m_pStudyPointTextBox->SetText( strCombine, NS_UITEXTCOLOR::YELLOW );

    if( m_pList )   // 제작목록 초기화
    {
        m_pList->ResetTextBox();
    }

    if( m_pGoods )  // 선택한 제작 정보창 갱신
    {
        SetItemMix( m_pGoods->GetItemMix() );
    }

    if( m_pType )   // 제작분야탭 갱신
    {
        m_pType->RefreshTapButton();
        m_pType->SetShowSkill(m_bIsShowSkillCheckBoxOn);
        // 제작버튼 활성화 갱신
        if ( m_pProductionButton )
        {
            const UIGUID CurrentTapID = m_pType->GetTapID();
            GLProductClient* pProductClient = m_pGaeaClient->GetProductClient();
            if ( pProductClient )
            {
                WORD wType; // 탭 ID를 제작분야 인덱스로 매칭시켜줌
                if ( CurrentTapID != 0 )    // 전체 분야 탭이 아닐경우 ( 0은 전체분야 탭인덱스 )
                {
                    wType = WORD(CurrentTapID - 1);
					if ( pProductClient->IsUsableProductType( wType ) || wType == Product::ETypeTotal ) // 제작가능항목이나 전체탭일 경우 생산버튼 활성화
                    {
                        m_pProductionButton->SetVisibleSingle( TRUE );
                    }
                    else    // 그 외 경우 생산버튼 비활성화
                    {
                        m_pProductionButton->SetVisibleSingle( FALSE );
                    }
                }
            }
        }
    }
}

void CProductWindow::DialogMaterialConfirm()
{
	OpenMessageBox( CMessageBox::TITLE_QUESTION, 
		ID2GAMEINTEXT("PRODUCT_WINDOW_CONFIRM_MATERIAL_MSG"),
		CMessageBox::TYPE_YESNO,
		CProductWindow::MESSAGEBOX_CONFIRM_PRODUCT );
}

void CProductWindow::MatchingEum( int nIndex )
{
    // 텍스트 인덱스와 ENUM값이 달라서 매칭시키는 코드
    switch( nIndex )
    {
    case 1:
        m_eFindClass = GLCL_ARMS;
        break;

    case 2:
        m_eFindClass = GLCL_FIGHTER;
        break;

    case 3:
        m_eFindClass = GLCL_ARCHER;
        break;

    case 4:
        m_eFindClass = GLCL_SPIRIT;
        break;

    case 5:
        m_eFindClass = GLCL_SCIENTIST;
        break;

    case 6:
        m_eFindClass = GLCL_EXTREME;
        break;

    case 7:
        m_eFindClass = GLCL_ASSASSIN;
        break;

	case 8:
		m_eFindClass = GLCL_TRICKER;
		break;

    default:
        m_eFindClass = GLCL_NUM_CLASS;
        break;
    }
}


void CProductWindow::CreateUIWindowAndRegisterOwnership()
{
    Create( PRODUCT_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CreateBaseWindowLightGray( "PRODUCT_WINDOW_BASE", (char*)ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 0 ) );
    CreateSubControl();
    SetControlNameEx( (char*)ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 0 ) );
    SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
    m_pInterface->UiRegisterControl( this, true );
    m_pInterface->UiShowGroupFocus( GetWndID() );
}

void CProductWindow::SetSeletedTabID( UIGUID ControlID )
{
    if ( ControlID < Product::ETypeHeadGear || ControlID > Product::ETypeTotal )
    {
        GASSERT( ControlID && "잘못된 UI 아이디");
        return;
    }
    GLProductClient* pProductClient = m_pGaeaClient->GetProductClient();
    if ( pProductClient == NULL )
    {
        return;
    }
    if ( pProductClient->IsUsableProductType( ControlID ) || ( ControlID == Product::ETypeTotal ) ) // 제작가능항목이나 전체탭일 경우 생산버튼 활성화
    {
        m_pProductionButton->SetVisibleSingle( TRUE );
    }
    else    // 그 외 경우 생산버튼 비활성화
    {
        m_pProductionButton->SetVisibleSingle( FALSE );
    }

    /*
     ControlID와 텍스트ID가 일치하지 않기 때문에
     전체탭의 경우 텍스트 0번 인덱스값으로 만들고
     나머지는 1씩 밀린 값을 가지게 해주는 작업
     */
    int nTextIndex;
    if ( ControlID == Product::ETypeTotal )
    {
        nTextIndex = 0;
    }
    else
    {
        nTextIndex = ControlID + 1;
    }

    if ( m_pTypeListText )
    {
        m_pTypeListText->SetText(
            ID2GAMEWORD("PRODUCT_TYPE_TEXT", nTextIndex ),
            NS_UITEXTCOLOR::LIGHTGREY );
    }
    if ( m_pList )
    {
        m_pList->SetSeletedTabID( ControlID );
        SetItemMix( m_pList->Select( 0 ) );
    }
    if ( m_pGoods )
    {
        m_pGoods->Clear();
    }
    m_pGoodsName->ClearText();
}

void CProductWindow::SetItemMix( SProductRecipe* pProductRecipe )
{
    if( pProductRecipe == NULL )
        return;
    if ( m_pClassFirterComboBoxRollOver && m_pClassFirterComboBoxRollOver->IsVisible() )
    {
        m_pList->Select( -1 );

        return;
    }

    if( pProductRecipe )
    {
        SITEM* pItem = GLItemMan::GetInstance().GetItem( pProductRecipe->sResultItem.sNID );
        if( pItem )
            m_pGoodsName->SetText( pItem->GetName(), NS_UITEXTCOLOR::LIGHTGREY );
    }
    else
    {
        m_pGoodsName->ClearText();
    }

    m_pGoods->SetItemInformation( pProductRecipe );
}

void CProductWindow::SetProducting( bool bProducting )
{
    m_bProducting = bProducting;

    m_pProductionButton->SetText(
        ID2GAMEWORD( "PRODUCT_BUTTON", bProducting ? 1 : 0 ),
        NS_UITEXTCOLOR::WHITE );
}

///////////////////////////////////////////////////////////////////////
// 인터페이스용  클래스
///////////////////////////////////////////////////////////////////////

MyProductWindow::MyProductWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CProductWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyProductWindow::CreateUIWindowAndRegisterOwnership()
{
    CProductWindow::CreateUIWindowAndRegisterOwnership( );
}

void MyProductWindow::SetSeletedTabID( UIGUID ControlID )
{
    CProductWindow::SetSeletedTabID( ControlID );
}

void MyProductWindow::SetItemMix( SProductRecipe* pProductRecipe )
{
    CProductWindow::SetItemMix( pProductRecipe );
}

void MyProductWindow::SetProducting( bool bProducting )
{
    CProductWindow::SetProducting( bProducting );
}

void MyProductWindow::DialogMaterialConfirm()
{
	CProductWindow::DialogMaterialConfirm();
}