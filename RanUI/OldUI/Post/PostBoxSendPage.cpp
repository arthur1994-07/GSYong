#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBoxSmart.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/SmartComboBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "PostBoxWindow.h"
#include "PostBoxWindowObject.h"
#include "PostBoxSendPage.h"
#include "PostBoxReadPage.h"

#include "../../InnerInterface.h"

#include "../Item/ItemImageEx.h"
//#include "../../ModalCallerID.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Post/GLPostClient.h"
#include "../../../RanLogicClient/Pet/GLPetClient.h"

#include "../../../RanLogic/s_NetGlobal.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CPostBoxSendPage::nLIMIT_STRING_TO		= CHR_ID_LENGTH;
const int CPostBoxSendPage::nLIMIT_STRING_SUBJECT	= 30;//POST_MAX_TITLE_LENGTH;
const int CPostBoxSendPage::nLIMIT_STRING_CONTENTS	= POST_MAX_CONTENT_LENGTH;

CPostBoxSendPage::CPostBoxSendPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CPostBoxPage(pGaeaClient, pInterface, pEngineDevice)
	, m_pText_Static_To				( NULL )
	, m_pText_Static_Design			( NULL )
	, m_pText_Static_Subject		( NULL )
	, m_pText_Static_Contents		( NULL )
	, m_pText_Static_SendMoney		( NULL )
	, m_pText_Static_MyMoney_Caption( NULL )
	, m_pText_Static_Billing		( NULL )
	, m_pText_Static_Item			( NULL )
	, m_pText_Bytes					( NULL )
	, m_pText_MyMoney_Money			( NULL )
	, m_pText_Tax					( NULL )
	, m_pImage_SendMoney			( NULL )
	, m_pImage_Billing				( NULL )
	, m_pItem						( NULL )
	, m_pLineBox_To					( NULL )
	, m_pLineBox_Subject			( NULL )
	, m_pLineBox_Contents			( NULL )
	, m_pLineBox_SendMoney			( NULL )
	, m_pLineBox_SendMoney_Disable	( NULL )
	, m_pLineBox_Billing			( NULL )
	, m_pLineBox_Billing_Disable	( NULL )
	, m_pLineBox_Item				( NULL )
	, m_pComboBox_Design			( NULL )
	, m_pComboBox_RollOver_Design	( NULL )
	, m_pEditBox_To					( NULL )
	, m_pEditBox_Subject			( NULL )
	, m_pEditBox_SendMoney			( NULL )
	, m_pEditBox_Billing			( NULL )
	, m_pMultiEditBox_Contents		( NULL )
	, m_pButton_Find				( NULL )
	, m_pButton_Find_Down			( NULL )
	, m_pButton_Preview				( NULL )
	, m_pButton_Send				( NULL )
{

	m_sItemID	 = NATIVEID_NULL();
	m_emMoneyReq = EMREQMONEYTYPE_NONE;

	m_RollOverID	= POSTBOX_PAGE_SEND_COMBOBOX_ROLLOVER_DESIGN;
	m_bRollOverLBUP = FALSE;
	m_dwDesign		= 0;

	m_pLastEditBox  = NULL;
}

CPostBoxSendPage::~CPostBoxSendPage ()
{

}

void CPostBoxSendPage::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nAlignLeft         = TEXT_ALIGN_LEFT;
	const int nAlignLeftCenterY  = TEXT_ALIGN_LEFT     | TEXT_ALIGN_CENTER_Y;	
	const int nAlignRightCenterY = TEXT_ALIGN_RIGHT    | TEXT_ALIGN_CENTER_Y;	
	const int nAlignCenter       = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;		

	// Note : 배경색
	{
		CreateBackgroundControl ( "POSTBOX_PAGE_SEND_BACKGROUND" );
	}

	// Note : 이미지
	{
		m_pImage_SendMoney  = CreateSwapImage ( "POSTBOX_PAGE_SEND_IMAGE_SENDMONEY", POSTBOX_PAGE_SEND_IMAGE_SENDMONEY );
		m_pImage_Billing	= CreateSwapImage ( "POSTBOX_PAGE_SEND_IMAGE_BILLING",   POSTBOX_PAGE_SEND_IMAGE_BILLING   );
	}

	// Note : 아이템이미지 배경
	{
		m_pLineBox_Item = CreateInputLineBox ( "POSTBOX_PAGE_SEND_LINEBOX_ITEM", POSTBOX_PAGE_SEND_LINEBOX_ITEM );
	}

	// Note : 텍스트	
	{
		// Note : 정적 텍스트
		m_pText_Static_To				= CreateStaticControl ( "POSTBOX_PAGE_SEND_TEXT_STATIC_TO",					pFont, nAlignLeftCenterY );
		m_pText_Static_Design			= CreateStaticControl ( "POSTBOX_PAGE_SEND_TEXT_STATIC_DESIGN",				pFont, nAlignLeftCenterY );
		m_pText_Static_Subject			= CreateStaticControl ( "POSTBOX_PAGE_SEND_TEXT_STATIC_SUBJECT",			pFont, nAlignLeftCenterY );
		m_pText_Static_Contents			= CreateStaticControl ( "POSTBOX_PAGE_SEND_TEXT_STATIC_CONTENTS",			pFont, nAlignLeftCenterY );
		m_pText_Static_SendMoney		= CreateStaticControl ( "POSTBOX_PAGE_SEND_TEXT_STATIC_SENDMONEY",			pFont, nAlignLeftCenterY, POSTBOX_PAGE_SEND_STATIC_TEXT_SENDMONEY );
		m_pText_Static_Billing			= CreateStaticControl ( "POSTBOX_PAGE_SEND_TEXT_STATIC_BILLING",			pFont, nAlignLeftCenterY, POSTBOX_PAGE_SEND_STATIC_TEXT_BILLING );
		m_pText_Static_MyMoney_Caption	= CreateStaticControl ( "POSTBOX_PAGE_SEND_TEXT_STATIC_MYMONEY_CAPTION",	pFont, nAlignLeftCenterY );
		m_pText_Static_Item				= CreateStaticControl ( "POSTBOX_PAGE_SEND_TEXT_STATIC_ITEM",				pFont, nAlignCenter );

		m_pText_Static_To				->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 0 ) );
		m_pText_Static_Design			->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 1 ) );
		m_pText_Static_Subject			->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 2 ) );
		m_pText_Static_Contents			->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 3 ) );
		m_pText_Static_SendMoney		->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 4 ) );
		m_pText_Static_Billing			->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 5 ) );
		m_pText_Static_MyMoney_Caption	->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 8 ) );
		m_pText_Static_Item				->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 6 ), NS_UITEXTCOLOR::LIGHTGREY );

		// Note : 가변 텍스트
		m_pText_MyMoney_Money	= CreateStaticControl ( "POSTBOX_PAGE_SEND_TEXT_MYMONEY_MONEY",	pFont, nAlignRightCenterY );
		m_pText_Bytes			= CreateStaticControl ( "POSTBOX_PAGE_SEND_TEXT_BYTES",			pFont, nAlignRightCenterY );
		m_pText_Tax				= CreateStaticControl ( "POSTBOX_PAGE_SEND_TEXT_TAX",			pFont, nAlignRightCenterY );
	}

	// Note : 아이템
	{
		m_pItem = CreateItemImage ( "POSTBOX_PAGE_SEND_ITEM" );
	}

	// Note : 에디트 박스
	{
		// Note : 배경
		{
			m_pLineBox_To		 = CreateInputLineBox ( "POSTBOX_PAGE_SEND_LINEBOX_TO"		);
			m_pLineBox_Subject	 = CreateInputLineBox ( "POSTBOX_PAGE_SEND_LINEBOX_SUBJECT"	);

			m_pLineBox_SendMoney		 = CreateInputLineBox		  ( "POSTBOX_PAGE_SEND_LINEBOX_SENDMONEY" );
			m_pLineBox_SendMoney_Disable = CreateInputLineBox_Disable ( "POSTBOX_PAGE_SEND_LINEBOX_SENDMONEY" );

			m_pLineBox_Billing			 = CreateInputLineBox		  ( "POSTBOX_PAGE_SEND_LINEBOX_BILLING"	);
			m_pLineBox_Billing_Disable	 = CreateInputLineBox_Disable ( "POSTBOX_PAGE_SEND_LINEBOX_BILLING" );
		}

		m_pEditBox_To = new CUIEditBox(m_pEngineDevice);
		m_pEditBox_To->CreateSub ( this, "POSTBOX_PAGE_SEND_EDITBOX_TO", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, POSTBOX_PAGE_SEND_EDITBOX_TO );
		m_pEditBox_To->CreateCarrat ( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		m_pEditBox_To->SetFont ( pFont );
		m_pEditBox_To->SetLimitInput ( nLIMIT_STRING_TO );
		RegisterControl ( m_pEditBox_To );

		m_pEditBox_Subject = new CUIEditBox(m_pEngineDevice);
		m_pEditBox_Subject->CreateSub ( this, "POSTBOX_PAGE_SEND_EDITBOX_SUBJECT", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, POSTBOX_PAGE_SEND_EDITBOX_SUBJECT );
		m_pEditBox_Subject->CreateCarrat ( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		m_pEditBox_Subject->SetFont ( pFont );
		m_pEditBox_Subject->SetLimitInput ( nLIMIT_STRING_SUBJECT );
		RegisterControl ( m_pEditBox_Subject );

		m_pEditBox_SendMoney = new CUIEditBox(m_pEngineDevice);
		m_pEditBox_SendMoney->CreateSub ( this, "POSTBOX_PAGE_SEND_EDITBOX_SENDMONEY", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, POSTBOX_PAGE_SEND_EDITBOX_SENDMONEY );
		m_pEditBox_SendMoney->CreateCarrat ( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		m_pEditBox_SendMoney->SetFont ( pFont );
		m_pEditBox_SendMoney->SetLimitInput ( nLIMIT_STRING_SENDMONEY );
		m_pEditBox_SendMoney->SET_MONEY_FORMAT( nMONEYUNIT, "," );
		m_pEditBox_SendMoney->SetAlign(EDIT_RIGHT_ALIGN);
		RegisterControl ( m_pEditBox_SendMoney );

		m_pEditBox_Billing = new CUIEditBox(m_pEngineDevice);
		m_pEditBox_Billing->CreateSub ( this, "POSTBOX_PAGE_SEND_EDITBOX_BILLING", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, POSTBOX_PAGE_SEND_EDITBOX_BILLING );
		m_pEditBox_Billing->CreateCarrat ( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		m_pEditBox_Billing->SetFont ( pFont );
		m_pEditBox_Billing->SetLimitInput ( nLIMIT_STRING_BILLING );
		m_pEditBox_Billing->SET_MONEY_FORMAT( nMONEYUNIT, "," );
		m_pEditBox_Billing->SetAlign(EDIT_RIGHT_ALIGN);
		RegisterControl ( m_pEditBox_Billing );
	}

	// Note : 멀티라인 에디트 박스
	{
		// Note : 배경
		{
			m_pLineBox_Contents  = CreateInputLineBox ("POSTBOX_PAGE_SEND_LINEBOX_CONTENTS");	
		}

		m_pMultiEditBox_Contents = new CUIMultiEditBoxSmart(m_pEngineDevice);
		m_pMultiEditBox_Contents->CreateSub				( this, "POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS", UI_FLAG_DEFAULT, POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS );
		m_pMultiEditBox_Contents->CreateCarrat			( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		//m_pMultiEditBox_Contents->CreateScrollVertical	();
		m_pMultiEditBox_Contents->SetFont				( pFont );
		m_pMultiEditBox_Contents->SetLimitInput			( 255 );
		m_pMultiEditBox_Contents->SetAutoTurn			( true );
		RegisterControl ( m_pMultiEditBox_Contents );
	}

	// Note : 버튼
	{
		// Note : 찾기
		{
			// Note : 클릭 상태
			m_pButton_Find_Down = new CUIControl(m_pEngineDevice);
			m_pButton_Find_Down->CreateSub ( this, "POSTBOX_PAGE_SEND_BUTTON_FIND_CLICK" );
			m_pButton_Find_Down->SetVisibleSingle( FALSE );
			RegisterControl ( m_pButton_Find_Down );
			
			// Note : 일반
			CBasicButton* pButton;
			m_pButton_Find = pButton = new CBasicButton(m_pEngineDevice);
			pButton->CreateSub ( this, "POSTBOX_PAGE_SEND_BUTTON_FIND", UI_FLAG_DEFAULT, POSTBOX_PAGE_SEND_BUTTON_FIND );
			pButton->CreateFlip ( "POSTBOX_PAGE_SEND_BUTTON_FIND_MOUSEIN", CBasicButton::MOUSEIN_FLIP );
			RegisterControl ( pButton );
		}

		CBasicTextButton* pButton;
		m_pButton_Preview = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_SEND_BUTTON_PREVIEW );
		pButton->CreateBaseButton( "POSTBOX_PAGE_SEND_BUTTON_PREVIEW", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_PAGE_SEND_BUTTON", 0 ) );
		RegisterControl( pButton );
	
		m_pButton_Send = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_SEND_BUTTON_SEND );
		pButton->CreateBaseButton( "POSTBOX_PAGE_SEND_BUTTON_SEND", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_PAGE_SEND_BUTTON", 1 ) );
		RegisterControl( pButton );

		/*;
		m_pButton_Find = pButton = new CBasicTextButton(m_pRenderDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_BUTTON_FIND );
		pButton->CreateBaseButton( "POSTBOX_WINDOW_MAIN_BUTTON_TAP_RECEIVE", CBasicTextButton::SIZE19, 
			CBasicButton::RADIO_FLIP, (char*)ID2GAMEWORD( "POSTBOX_BUTTON", 0 ) );
		RegisterControl( pButton );
		*/
	}

	
	// Note : 콤보 박스
	{
		CSmartComboBoxBasic* pComboBox; 
		pComboBox = m_pComboBox_Design = new CSmartComboBoxBasic(m_pEngineDevice);
		pComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_PAGE_SEND_COMBOBOX_DESIGN_OPEN );
		pComboBox->CreateSmartComboBox ( "POSTBOX_PAGE_SEND_COMBOBOX_DESIGN", "POSTBOX_PAGE_SEND_COMBOBOX_DESIGN_TEXINFO" );			
		RegisterControl( pComboBox );

		CBasicComboBoxRollOver * pComboBoxRollOver;
		pComboBoxRollOver = m_pComboBox_RollOver_Design = new CBasicComboBoxRollOver(m_pEngineDevice);
		pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_PAGE_SEND_COMBOBOX_ROLLOVER_DESIGN );
		pComboBoxRollOver->CreateBaseComboBoxRollOver( "POSTBOX_PAGE_SEND_COMBOBOX_ROLLOVER_DESIGN" );
		pComboBoxRollOver->SetVisibleSingle( FALSE );
		RegisterControl( pComboBoxRollOver );
	}

	Reset ();
}

void CPostBoxSendPage::OpenSubControl ()
{
	ResetDesignCombo ();

	ResetDesignCard ();

	// Note : 레벨 체크
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	if ( pChar && ((DWORD)pChar->GETLEVEL() < GLCONST_POST::dwPOST_SEND_REQUIRE_LEVEL) )
	{
		std::string strFormat = sc::string::format( ID2GAMEINTEXT("MODAL_POSTBOX", 4), GLCONST_POST::dwPOST_SEND_REQUIRE_LEVEL );
		m_pInterface->DoModal	( strFormat, UI::MODAL_WARNING, UI::OK );
	}
}

void CPostBoxSendPage::CloseSubControl ()
{
	RequestPush( CPostBoxWindow::POSTBOX_PAGE_REQ_FRIENDFIND_CLOSE );
	RequestPush( CPostBoxWindow::POSTBOX_PAGE_REQ_PREVIEW_CLOSE );
}

void CPostBoxSendPage::EndSubControl ()
{
	// Note : 에디트 박스 초기화
	{
		m_pEditBox_To			->ClearEdit();
		m_pEditBox_Subject		->ClearEdit();
		m_pEditBox_SendMoney	->ClearEdit();
		m_pEditBox_Billing		->ClearEdit();

		m_pMultiEditBox_Contents->ClearEdit();

		ResetDesignCombo ();

		ResetDesignCard ();
	}

	m_pGaeaClient->GetCharacter()->SetPostItemPos( SINVEN_POS() );
	m_pGaeaClient->GetCharacter()->SetPostItem( SITEMCUSTOM() );
}

void CPostBoxSendPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	// Note : 컨트롤 업데이트
	UpdateVariableControl ();

	// Note : 가변 텍스트
	UpdateVariableText ();

	// Note : 찾기 버튼
	m_pButton_Find		->SetVisibleSingle ( TRUE  );
	m_pButton_Find_Down ->SetVisibleSingle ( FALSE );

	// Note : 콤보박스 업데이트
	UpdateComboBox ();

	// Note : 탭으로 에디트 박스 이동
	UpdateEditBoxTabShift ();

	CPostBoxPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPostBoxSendPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	// Note : 금액 첨부 라디오버튼
	case POSTBOX_PAGE_SEND_IMAGE_SENDMONEY		 :
	case POSTBOX_PAGE_SEND_STATIC_TEXT_SENDMONEY :
	case POSTBOX_PAGE_SEND_EDITBOX_SENDMONEY	 : 
		{
			if ( CHECK_MOUSE_IN ( dwMsg )  )
			{
				if ( dwMsg&UIMSG_LB_DOWN )
				{
					SetMoneyReqType ( EMREQMONEYTYPE_SEND );
				}

				// Note : 금액 기입이 안되었을 때만 출력
				const CString& strMoney = m_pEditBox_SendMoney->GetEditString();
				if ( strMoney.IsEmpty() )
				{
					m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT( "POSTBOX_TOOLTIP_SEND_MONEY" ), NS_UITEXTCOLOR::WHITE );
				}
			}
		}
		break;

	// Note : 요금 청부 라디오버튼
	case POSTBOX_PAGE_SEND_IMAGE_BILLING	   :
	case POSTBOX_PAGE_SEND_STATIC_TEXT_BILLING :
	case POSTBOX_PAGE_SEND_EDITBOX_BILLING	   : 
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( ( m_sItemID != NATIVEID_NULL() ) && dwMsg&UIMSG_LB_DOWN )
				{
					SetMoneyReqType ( EMREQMONEYTYPE_BILLING );
				}

				// Note : 금액 기입이 안되었을 때만 출력
				const CString& strMoney = m_pEditBox_Billing->GetEditString();
				if ( strMoney.IsEmpty() )
				{
					m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT( "POSTBOX_TOOLTIP_SEND_BILLING" ), NS_UITEXTCOLOR::WHITE );
				}
			}
		}
		break;

	};

	switch ( ControlID )
	{
	case POSTBOX_PAGE_SEND_EDITBOX_TO		 : TranslateEditBoxMsg( m_pEditBox_To, dwMsg );			break;
	case POSTBOX_PAGE_SEND_EDITBOX_SUBJECT	 : TranslateEditBoxMsg( m_pEditBox_Subject, dwMsg );	break;

	case POSTBOX_PAGE_SEND_EDITBOX_SENDMONEY : 
		{
			if ( m_emMoneyReq == EMREQMONEYTYPE_SEND )
			{
				TranslateEditBoxMsg( m_pEditBox_SendMoney, dwMsg );	
			}
		}
		break;

	case POSTBOX_PAGE_SEND_EDITBOX_BILLING	 : 
		{
			if (  m_emMoneyReq == EMREQMONEYTYPE_BILLING )
			{
				TranslateEditBoxMsg( m_pEditBox_Billing, dwMsg );
			}
		}	
		break;

	//case POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS : TranslateMultiEditBoxMsg ( m_pMultiEditBox_Contents, dwMsg ); break;
	case POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS : TranslateMultiEditBoxSmartMsg ( m_pMultiEditBox_Contents, dwMsg ); break;

	// Note : 디자인카드 콤보박스 오픈
	case POSTBOX_PAGE_SEND_COMBOBOX_DESIGN_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER )
				{
					m_pComboBox_RollOver_Design->SetVisibleSingle ( TRUE );
					//m_pComboBox_RollOver_Design->SetScrollPercent ( 0.0f );

					m_RollOverID	= POSTBOX_PAGE_SEND_COMBOBOX_ROLLOVER_DESIGN;
					m_bRollOverLBUP = TRUE;
				}
			}
		}
		break;

	// Note : 디자인카드 콤보박스 롤오버
	case POSTBOX_PAGE_SEND_COMBOBOX_ROLLOVER_DESIGN:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );
				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{				
				int nIndex = m_pComboBox_RollOver_Design->GetSelectIndex ();
				if ( nIndex < 0 ) return ;

				m_dwDesign = (DWORD)nIndex;

				CString strText;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strText = CGameTextMan::GetInstance().GetCommentText("POSTBOX_DESIGN_TYPE", m_dwDesign).GetString();
				else
					strText = COMMENT::POSTBOX_DESIGN_TYPE[ m_dwDesign ].c_str();
				m_pComboBox_Design->SetText( strText );

				ResetDesignCard ();
			}
		}
		break;

	// Note : 친구 찾기
	case POSTBOX_PAGE_SEND_BUTTON_FIND:
		{			
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					m_pButton_Find		->SetVisibleSingle( FALSE );
					m_pButton_Find_Down ->SetVisibleSingle( TRUE  );
				}

				if ( CHECK_LB_UP_LIKE( dwMsg ) )
				{
					RequestPush( CPostBoxWindow::POSTBOX_PAGE_REQ_FRIENDFIND_OPEN );
				}
			}
		}
		break;

	// Note : 미리 보기
	case POSTBOX_PAGE_SEND_BUTTON_PREVIEW:
		{		
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{		
				if ( CHECK_LB_UP_LIKE( dwMsg ) )
				{
					RequestPush( CPostBoxWindow::POSTBOX_PAGE_REQ_PREVIEW_OPEN );
				}
			}
		}
		break;

	// Note : 아이템 첨부
	case POSTBOX_PAGE_SEND_LINEBOX_ITEM:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				GLCharacter* pChar = m_pGaeaClient->GetCharacter();
				if( !pChar )
					return;

				if ( dwMsg & UIMSG_LB_UP )
				{
					SITEMCUSTOM sITEMCUSTOM = pChar->GET_PREHOLD_ITEM();

					if( sITEMCUSTOM.GetNativeID() != NATIVEID_NULL () )
					{
						const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sITEMCUSTOM.GetNativeID() );
						if ( !pITEM )
							break;

						// 팻카드이면서 팻이 활성화 되어 있으면 거래할 수 없다.
						if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD )
						{
							GLPetClient* pPet = m_pGaeaClient->GetPetClient();
							if (pPet && pPet->IsVALID() && sITEMCUSTOM.PetDbNum() == pPet->DbNum())
							{
								m_pGaeaClient->PrintMsgText(
									NS_UITEXTCOLOR::DISABLE,
									ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_ITEM_TRADING") );
								break;
							}
						}

						// 탈것이면 거래할수 없다.
						if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE &&
							sITEMCUSTOM.m_VehicleDbNum != 0 )
						{
							m_pGaeaClient->PrintMsgText(
								NS_UITEXTCOLOR::DISABLE,
								ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_ITEM_TRADING") );
							break;
						}

						// 우편 테스트 및 편의성을 위해 GM은 거래 옵션, 기간제 아이템 체크하지 않도록 변경함
						if ( pChar->m_dwUserLvl < USER_USER_GM )
						{
							//	거래옵션
							// GMCharEdit 로 넣은 아이템은 버리기 불가능하다.
							if ( !( sITEMCUSTOM.IsExchange() != NULL ) || sITEMCUSTOM.IsGM_GENITEM() )
							{
								m_pGaeaClient->PrintMsgText( 
									NS_UITEXTCOLOR::DISABLE, 
									ID2GAMEINTEXT( "POSTBOX_ERROR_MSG_SEND_ITEM_TRADING" ) );
								break;
							}

							// 시간제 아이템 확인
							if ( sITEMCUSTOM.IsTimeLimit() )
							{
								m_pGaeaClient->PrintMsgText( 
									NS_UITEXTCOLOR::DISABLE, 
									ID2GAMEINTEXT( "POSTBOX_ERROR_MSG_SEND_ITEM_LIMIT" ) );
								break;
							}
						}

						pChar->SetPostItemPos( pChar->GetPreHoldItemPos() );
						pChar->SetPostItem( sITEMCUSTOM );
						pChar->m_sPreInventoryItem = SINVEN_POS();
						break;
					}
					else
					{
						pChar->SetPostItemPos( SINVEN_POS() );
						pChar->SetPostItem( SITEMCUSTOM() );
						break;
					}
				}

				if( dwMsg & UIMSG_RB_UP )
				{
					pChar->SetPostItemPos( SINVEN_POS() );
					pChar->SetPostItem( SITEMCUSTOM() );
					break;
				}

				// Note : 아이템이 없을때만 툴팁을 보여준다.
				if ( m_sItemID == NATIVEID_NULL() )
				{
					m_pInterface->SHOW_COMMON_LINEINFO( ID2GAMEINTEXT( "POSTBOX_TOOLTIP_SEND_ITEM" ), NS_UITEXTCOLOR::WHITE );
				}

				// Note : 아이템 정보를 보여준다.
				const SITEMCUSTOM& sItemCustom = pChar->GetPostItem();
				if ( sItemCustom.GetNativeID() != NATIVEID_NULL() )
				{	
					m_pInterface->SHOW_ITEM_INFO(
						sItemCustom,
						FALSE, FALSE, FALSE, FALSE, FALSE,
						USHRT_MAX, USHRT_MAX,
						NATIVEID_NULL() );
				}
			}
		}
		break;

	// Note : 우편 보내기
	case POSTBOX_PAGE_SEND_BUTTON_SEND:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				LONGLONG llAttachMoney	   = 0;
				LONGLONG llCommision	   = 0; 
				bool	 bDoCommisionModal = false;

				switch ( m_emMoneyReq )
				{
				// Note : 금액 첨부 일때
				case EMREQMONEYTYPE_SEND:
					{
						const CString& strMoney = m_pEditBox_SendMoney->GetEditString();
						llAttachMoney  = static_cast<LONGLONG>( _atoi64 ( strMoney ) );

						if ( llAttachMoney )
						{
							llCommision  = GLPostClient::GetCommision ( FALSE, FALSE, llAttachMoney );
							llCommision -=  (LONGLONG)GLCONST_POST::dwPOST_BASE_FEE; // Note : 돈에대한 수수료율만 알아낸다.

							bDoCommisionModal = true;
						}
					}
					break;
				};

				if ( bDoCommisionModal )
				{
                    std::string strFormat = sc::string::format( ID2GAMEINTEXT("MODAL_POSTBOX_2005", 0), llAttachMoney, llCommision);
					m_pInterface->DoModal ( strFormat,  UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_POSTBOX_PAGE_SEND_BUTTON_SEND_COMMISION );
					break;
				}

				ReqSendPost ();
			}
		}
		break;
	};

	CPostBoxPage::TranslateUIMessage ( ControlID, dwMsg );
}

void CPostBoxSendPage::TranslateEditBoxMsg ( CUIEditBox * pEditBox, DWORD dwMsg )
{
	if( !pEditBox ) return;

	if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
	{
		pEditBox->EndEdit();
		pEditBox->BeginEdit();

		m_pLastEditBox = pEditBox;
	}
	else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
	{
		pEditBox->EndEdit();
	}
}

void CPostBoxSendPage::TranslateMultiEditBoxMsg ( CUIMultiEditBox* pMultiEditBox, DWORD dwMsg )
{
	if( !pMultiEditBox ) return;

	if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
	{
		pMultiEditBox->EndEdit();
		pMultiEditBox->BeginEdit();
	}
	else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
	{
		pMultiEditBox->EndEdit();
	}
}

void CPostBoxSendPage::TranslateMultiEditBoxSmartMsg ( CUIMultiEditBoxSmart* pMultiEditBoxSmart, DWORD dwMsg )
{
	if( !pMultiEditBoxSmart ) return;

	if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
	{
		pMultiEditBoxSmart->EndEdit();
		pMultiEditBoxSmart->BeginEdit();

		m_pLastEditBox = pMultiEditBoxSmart;
	}
	else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
	{
		pMultiEditBoxSmart->EndEdit();
	}

	pMultiEditBoxSmart->TranslateUIMessage( POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS, dwMsg );
}

CBasicLineBoxSmart* CPostBoxSendPage::CreateInputLineBox ( const char* szControl, UIGUID ControlID )
{
	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub ( this, szControl, UI_FLAG_XSIZE | UI_FLAG_YSIZE, ControlID );
	pLineBox->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_TEXINFO" );
	RegisterControl ( pLineBox );		

	return pLineBox;
}
CBasicLineBoxSmart* CPostBoxSendPage::CreateInputLineBox_Disable ( const char* szControl, UIGUID ControlID )
{
	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub ( this, szControl, UI_FLAG_XSIZE | UI_FLAG_YSIZE, ControlID );
	pLineBox->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_DISABLE_TEXINFO" );
	RegisterControl ( pLineBox );		

	return pLineBox;
}

void CPostBoxSendPage::Reset ()
{
	// Note : 에디트 박스 초기화
	{
		m_pEditBox_Subject		->ClearEdit();
		m_pEditBox_SendMoney	->ClearEdit();
		m_pEditBox_Billing		->ClearEdit();

		m_pMultiEditBox_Contents->ClearEdit();
	}

	// Note : 아이템 첨부 관련
	{
		m_pGaeaClient->GetCharacter()->SetPostItemPos( SINVEN_POS() );
		m_pGaeaClient->GetCharacter()->SetPostItem( SITEMCUSTOM() );

		m_sItemID = NATIVEID_NULL();
	}

	// Note : 금액 첨부 관련
	{
		m_pImage_SendMoney->SetVisibleSingle( TRUE  );
		m_pImage_Billing  ->SetVisibleSingle( FALSE );

		SetMoneyReqType ( EMREQMONEYTYPE_SEND );
	}

	// Note : 디자인 콤보 리셋
	ResetDesignCombo ();

	// Note : 디자인 카드 리셋
	ResetDesignCard ();

	// Note : 컨트롤 업데이트
	UpdateVariableControl ();

	// Note : 가변 텍스트
	UpdateVariableText ();

	// Note : 콤보박스 업데이트
	UpdateComboBox ();
}

void CPostBoxSendPage::ResetDesignCombo ()
{
	m_dwDesign = 0;

	m_pComboBox_RollOver_Design->ClearText();

	CString strText;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		for ( WORD i=0; i<EMPOSTBOX_DESIGN_TYPE_SIZE; ++i )
		{
			CString strText = CGameTextMan::GetInstance().GetCommentText("POSTBOX_DESIGN_TYPE", i).GetString();
			int		nIndex  = m_pComboBox_RollOver_Design->AddText( strText, FALSE, FALSE );
			m_pComboBox_RollOver_Design->SetTextData( nIndex, nIndex );
		}
		strText = CGameTextMan::GetInstance().GetCommentText("POSTBOX_DESIGN_TYPE", m_dwDesign).GetString();
	}
	else
	{
		for ( WORD i=0; i<EMPOSTBOX_DESIGN_TYPE_SIZE; ++i )
		{
			CString strText = COMMENT::POSTBOX_DESIGN_TYPE[i].c_str();
			int		nIndex  = m_pComboBox_RollOver_Design->AddText( strText, FALSE, FALSE );
			m_pComboBox_RollOver_Design->SetTextData( nIndex, nIndex );
		}
		strText = COMMENT::POSTBOX_DESIGN_TYPE[ m_dwDesign ].c_str();
	}
	m_pComboBox_Design->SetText( strText );
}

void CPostBoxSendPage::ResetDesignCard ()
{
	if ( m_dwDesign )
	{
		DWORD dwPosX   = ((m_dwDesign-1)%CPostBoxReadPage::nDESIGN_CARD_NUM_WIDTH) * CPostBoxReadPage::nDESIGN_CARD_SIZE_WIDTH;
		DWORD dwPosY   = ((m_dwDesign-1)/CPostBoxReadPage::nDESIGN_CARD_NUM_WIDTH) * CPostBoxReadPage::nDESIGN_CARD_SIZE_HEIGHT;

		m_pLineBox_Contents->SetBackgroundImageEx	( "POSTBOX_DISIGN_CARD", dwPosX, dwPosY, CPostBoxReadPage::nDESIGN_CARD_SIZE_WIDTH, CPostBoxReadPage::nDESIGN_CARD_SIZE_HEIGHT );
		m_pLineBox_Contents->SetBackgroundView		( TRUE );
	}
	else
	{
		m_pLineBox_Contents->SetBackgroundView		( FALSE );
	}
}

void CPostBoxSendPage::UpdateVariableText ()
{
	GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();

	CString  strText;
	LONGLONG llCommision = 0;

	// Note : 이용금액
	{
		BOOL	 bAttachDesign = FALSE;
		BOOL	 bAttachItem   = FALSE;
		LONGLONG llAttachMoney = 0;

		if ( m_dwDesign )
		{
			bAttachDesign = TRUE;
		}

		if ( m_sItemID != NATIVEID_NULL() )
		{
			bAttachItem = TRUE;
		}

		switch ( m_emMoneyReq )
		{
			// 금액 첨부 일때
		case EMREQMONEYTYPE_SEND:
			{
				const CString& strMoney = m_pEditBox_SendMoney->GetEditString();
				llAttachMoney = static_cast<LONGLONG>( _atoi64 ( strMoney ) );
			}
			break;
		}

		llCommision = GLPostClient::GetCommision ( bAttachDesign, bAttachItem, llAttachMoney );

		strText = ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 7 );
		strText += " ";
		strText += NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( llCommision, nMONEYUNIT, "," );

		m_pText_Tax->SetOneLineText ( strText, NS_UITEXTCOLOR::ORANGE );
	}

	// Note : 보유금액
	{
		strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( sCharData.GetInvenMoney(), nMONEYUNIT, "," );

		// Note : 수수료보다 금액이 적을시
		if ( sCharData.GetInvenMoney() < llCommision )
		{
			m_pText_MyMoney_Money->SetOneLineText ( strText, NS_UITEXTCOLOR::RED );
		}
		else
		{
			m_pText_MyMoney_Money->SetOneLineText ( strText );
		}
	}

	// Note : 내용 바이트
	{
		INT nLength = m_pMultiEditBox_Contents->GetEditLength();
		strText.Format( (char*)ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 9 ), nLength, nLIMIT_STRING_CONTENTS-1 );

		m_pText_Bytes->SetOneLineText ( strText );
	}
}

void CPostBoxSendPage::UpdateVariableControl ()
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter )
		return;

	// Note :  첨부 아이템 이미지 관련
	{
		const SITEMCUSTOM& sItemCustom = pCharacter->GetPostItem();
		if ( sItemCustom.GetNativeID() == NATIVEID_NULL() )
		{
			m_pItem->SetTurnNum( 0 );
			m_pItem->SetVisibleSingle( FALSE );
			m_pItem->ResetItem();

			m_sItemID = NATIVEID_NULL();
		}
		else
		{
			if ( m_pItem->GetItemImageSlotPos() != sItemCustom.GetNativeID() )
			{
				m_pItem->SetVisibleSingle( TRUE );
				m_pItem->SetItem( sItemCustom );
				m_pItem->SetTurnNum( sItemCustom.wTurnNum );

				m_sItemID = sItemCustom.GetNativeID();
			}
		}
	}

	// Note : 금액 첨부 관련
	{
		m_pImage_SendMoney->SetVisibleSingle ( TRUE );

		if ( m_sItemID != NATIVEID_NULL() )
		{
			m_pImage_Billing->SetVisibleSingle ( TRUE );
		}
		else
		{
			m_pImage_Billing->SetVisibleSingle ( FALSE );
			SetMoneyReqType ( EMREQMONEYTYPE_SEND );
		}
	}

	// Note : 금액 첨부가 최대치와 현재 자신이 가지고 있는 금액만큼 넘지 못하도록 한다.
	{
		GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();
		CUIEditBox*  pEditBox  = NULL;
		LONGLONG	 llMoney   = 0;

		switch ( m_emMoneyReq )
		{
		// 금액 첨부
		case EMREQMONEYTYPE_SEND:
			{
				const CString& strMoney = m_pEditBox_SendMoney->GetEditString();
				llMoney  = static_cast<LONGLONG>( _atoi64 ( strMoney ) );
				pEditBox = m_pEditBox_SendMoney;

				BOOL bAttachDesign	= FALSE;
				BOOL bAttachItem	= FALSE;

				if ( m_dwDesign )
				{
					bAttachDesign = TRUE;
				}

				if ( m_sItemID != NATIVEID_NULL() )
				{
					bAttachItem = TRUE;
				}

				LONGLONG llMaxCommision = GLPostClient::GetCommision ( bAttachDesign, bAttachItem, sCharData.GetInvenMoney() );

				// Note : 첨부금액 + 세금만큼 넘지 못하도록 한다.
				if ( sCharData.GetInvenMoney() < (llMoney+llMaxCommision) )
				{
					LONGLONG llApplyMoney = sCharData.GetInvenMoney() - llMaxCommision;

					if ( llApplyMoney < 0 )
					{
						llApplyMoney = 0;
					}

					CString strFormat;
					strFormat.Format( "%lld", llApplyMoney );
					m_pEditBox_SendMoney->SetEditString ( strFormat ); 
					break;
				}
			}
			break;

		// 대금 청구
		case EMREQMONEYTYPE_BILLING:
			{
				const CString& strMoney = m_pEditBox_Billing->GetEditString();
				llMoney  = static_cast<LONGLONG>( _atoi64 ( strMoney ) );
				pEditBox = m_pEditBox_Billing;
			}
			break;
		};

		// Note : 첨부 최대치를 넘지 못하도록 한다.
		if  ( pEditBox )
		{
			if ( GLCONST_POST::llPOST_MAX_ATTACH_MONEY < llMoney )
			{
				CString strFormat;
				strFormat.Format( "%lld", GLCONST_POST::llPOST_MAX_ATTACH_MONEY );
				pEditBox->SetEditString ( strFormat ); 
			}
		}
	}
}


void CPostBoxSendPage::UpdateEditBoxTabShift ()
{
	CUIEditBox** pEditBoxList = NULL;
	CUIEditBox*  pEditBoxList_Money   [] = { m_pEditBox_To, m_pEditBox_Subject, m_pMultiEditBox_Contents, m_pEditBox_SendMoney,	NULL };
	CUIEditBox*  pEditBoxList_Billing [] = { m_pEditBox_To, m_pEditBox_Subject, m_pMultiEditBox_Contents, m_pEditBox_Billing,	NULL };

	switch ( m_emMoneyReq )
	{
	// Note : 금액 첨부
	case EMREQMONEYTYPE_SEND:
		{
			pEditBoxList = pEditBoxList_Money;
		}
		break;

	// Note : 대금 청구
	case EMREQMONEYTYPE_BILLING:
		{
			pEditBoxList = pEditBoxList_Billing;
		}
		break;

	default:
		return;
	};

	if ( UIKeyCheck::GetInstance()->Check ( DIK_TAB, DXKEY_DOWN ) )
	{
		if ( !m_pLastEditBox || !m_pLastEditBox->IsBegin() )
		{
			return;
		}

		int i = 0;
		CUIEditBox* pEditBox = pEditBoxList[i];
		for ( i=0; pEditBox; ++i )
		{
			pEditBox = pEditBoxList[i];

			if ( pEditBox == m_pLastEditBox )
			{
				break;
			}
		}

		if ( !pEditBox )
		{
			return;
		}

		CUIEditBox* pNextEditBox = pEditBoxList[i+1];
		if ( !pNextEditBox )
		{
			pNextEditBox = pEditBoxList[0];

			if ( !pNextEditBox )
			{
				return;
			}
		}

		pEditBox	 ->EndEdit();
		pNextEditBox ->BeginEdit();
		m_pLastEditBox = pNextEditBox;
	}
}

void CPostBoxSendPage::UpdateComboBox ()
{
	if ( m_RollOverID != NO_ID )
	{
		CUIControl* pControl = m_ControlContainer.FindControl ( m_RollOverID );
		if ( !pControl )
		{
			GASSERT ( 0 && "심각한 오류, 노드가 널입니다." );
			return ;
		}

		DWORD dwControlMsg = pControl->GetMessageEx ();
		//CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		ResetMessageEx ();

		//pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		/*
		NS_UIDEBUGSET::BlockBegin ();
		if ( dwControlMsg ) TranslateUIMessage ( m_RollOverID, dwControlMsg );
		NS_UIDEBUGSET::BlockEnd ();
		*/

		//	스크롤바에 관계된 메시지가 아니고		
		if ( !(dwControlMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
		{
			DWORD dwMsg = dwControlMsg;//GetMessageEx ();
			if ( dwMsg & UIMSG_LB_UP )
			{
				if ( !m_bRollOverLBUP )
				{
					m_RollOverID = NO_ID;
					pControl->SetVisibleSingle ( FALSE );					
				}
				m_bRollOverLBUP = FALSE;
			}
		}
	}
}
