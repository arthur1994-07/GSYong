#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLogicEx.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Post/GLPostClient.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImageEx.h"

#include "PostBoxWindow.h"
#include "PostBoxWindowObject.h"
#include "PostBoxReadPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxReadPage::CPostBoxReadPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CPostBoxPage(pGaeaClient, pInterface, pEngineDevice)
	, m_pItem						( NULL )
	, m_pImage_Money				( NULL )
	, m_pText_Static_Date			( NULL )
	, m_pText_Static_From			( NULL )
	, m_pText_Static_NoneItem		( NULL )
	, m_pText_Static_ReceiveItem	( NULL )
	, m_pText_Static_ReceiveMoney	( NULL )
	, m_pText_Static_BillingMoney	( NULL )
	, m_pText_Static_MyMoney		( NULL )
	, m_pText_Static_AttachItem		( NULL )
	, m_pText_From					( NULL )
	, m_pText_Date					( NULL )
	, m_pText_LeftTime				( NULL )
	, m_pText_Subject				( NULL )
	, m_pText_Contents				( NULL )
	, m_pText_Money					( NULL )
	, m_pText_MyMoney				( NULL )
	, m_pButton_Take				( NULL )
	, m_pButton_Return				( NULL )
	, m_pButton_Delete				( NULL )
	, m_pButton_Reply				( NULL )
	, m_pButton_Close				( NULL )
	, m_pLineBox_Contents			( NULL )
	, m_pLineBox_Item				( NULL )
	, m_pLineBox_Item_Billing		( NULL )
	, m_pLineBox_Money				( NULL )
	, m_pLineBox_MyMoney			( NULL )
	, m_pPost						( NULL )
{

}

CPostBoxReadPage::~CPostBoxReadPage()
{

}

void CPostBoxReadPage::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nAlignLeft         = TEXT_ALIGN_LEFT;
	const int nAlignLeftCenterY  = TEXT_ALIGN_LEFT     | TEXT_ALIGN_CENTER_Y;	
	const int nAlignRightCenterY = TEXT_ALIGN_RIGHT    | TEXT_ALIGN_CENTER_Y;	
	const int nAlignCenter       = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

	// Note : 배경색
	{
		CreateBackgroundControl ( "POSTBOX_PAGE_READ_BACKGROUND" );
		CreateBackgroundControl ( "POSTBOX_PAGE_READ_BACKGROUND_ATTACHITEM" );
	}

	// Note : 라인박스
	{
		CBasicLineBoxSmart* pLineBox = NULL;

		pLineBox = m_pLineBox_Contents = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub ( this, "POSTBOX_PAGE_READ_LINEBOX_CONTENTS", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateSubControl( "POSTBOX_PAGE_READ_LINEBOX_CONTENTS_TEXINFO" );
		RegisterControl ( pLineBox );

		pLineBox = m_pLineBox_Item = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub ( this, "POSTBOX_PAGE_READ_LINEBOX_ITEM", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateSubControl( "POSTBOX_PAGE_READ_LINEBOX_ITEM_TEXINFO" );
		RegisterControl ( pLineBox );

		pLineBox = m_pLineBox_Item_Billing = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub ( this, "POSTBOX_PAGE_READ_LINEBOX_ITEM_BILLING", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateSubControl( "POSTBOX_PAGE_READ_LINEBOX_ITEM_BILLING_TEXINFO" );
		RegisterControl ( pLineBox );

		pLineBox = m_pLineBox_Money = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub ( this, "POSTBOX_PAGE_READ_LINEBOX_MONEY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateSubControl( "POSTBOX_PAGE_READ_LINEBOX_MONEY_TEXINFO" );
		RegisterControl ( pLineBox );

		pLineBox = m_pLineBox_MyMoney = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub ( this, "POSTBOX_PAGE_READ_LINEBOX_MYMONEY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateSubControl( "POSTBOX_PAGE_READ_LINEBOX_MYMONEY_TEXINFO" );
		RegisterControl ( pLineBox );
	}

	// Note : 텍스트	
	{
		// Note : 정적 텍스트
		m_pText_Static_Date			= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_STATIC_DATE",				pFont, nAlignLeftCenterY );
		m_pText_Static_From			= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_STATIC_FROM",				pFont, nAlignLeftCenterY );
		m_pText_Static_ReceiveItem	= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_STATIC_RECEIVEITEM",		pFont, nAlignLeftCenterY );
		m_pText_Static_ReceiveMoney	= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_STATIC_RECEIVEMONEY",		pFont, nAlignLeftCenterY );
		m_pText_Static_BillingMoney	= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_STATIC_BILLINGMONEY",		pFont, nAlignLeftCenterY );
		m_pText_Static_MyMoney		= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_STATIC_MYMONEY",			pFont, nAlignLeftCenterY );
		m_pText_Static_NoneItem		= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_STATIC_NONEITEM",			pFont, nAlignCenter );
		m_pText_Static_AttachItem	= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_STATIC_ATTACHITEM",			pFont, nAlignCenter );

		m_pText_Static_Date			->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_TEXTBOX", 0 ) );
		m_pText_Static_From			->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_TEXTBOX", 1 ) );
		m_pText_Static_ReceiveItem	->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_TEXTBOX", 2 ) );
		m_pText_Static_ReceiveMoney	->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_TEXTBOX", 3 ) );
		m_pText_Static_BillingMoney	->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_TEXTBOX", 4 ) );
		m_pText_Static_MyMoney		->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_TEXTBOX", 5 ) );
		m_pText_Static_NoneItem		->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_TEXTBOX", 6 ), NS_UITEXTCOLOR::LIGHTGREY );
		m_pText_Static_AttachItem	->AddText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_TEXTBOX", 7 ) );

		// Note : 가변 텍스트
		m_pText_Date				= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_DATE",						pFont, nAlignLeftCenterY );
		m_pText_From				= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_FROM",						pFont, nAlignLeftCenterY );
		m_pText_LeftTime			= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_LEFTTIME",					pFont, nAlignLeftCenterY );
		m_pText_Money				= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_MONEY",						pFont, nAlignRightCenterY );
		m_pText_MyMoney				= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_MYMONEY",					pFont, nAlignRightCenterY );
		m_pText_Subject				= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_SUBJECT",					pFont, nAlignLeft );
		m_pText_Contents			= CreateStaticControl ( "POSTBOX_PAGE_READ_TEXT_CONTENTS",					pFont, nAlignLeft );
	}

	// Note : 아이템
	{
		m_pItem = CreateItemImage ( "POSTBOX_PAGE_READ_ITEM", POSTBOX_PAGE_READ_ITEM );
	}

	// Note : 버튼
	{
		CBasicTextButton* pButton;
		m_pButton_Take = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_READ_BUTTON_TAKE );
		pButton->CreateBaseButton( "POSTBOX_PAGE_READ_BUTTON_TAKE", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_BUTTON", 0 ) );
		RegisterControl( pButton );

		m_pButton_Return = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_READ_BUTTON_RETURN );
		pButton->CreateBaseButton( "POSTBOX_PAGE_READ_BUTTON_RETURN", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_BUTTON", 1 ) );
		RegisterControl( pButton );

		m_pButton_Delete = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_READ_BUTTON_DELETE );
		pButton->CreateBaseButton( "POSTBOX_PAGE_READ_BUTTON_DELETE", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_BUTTON", 2 ) );
		RegisterControl( pButton );

		m_pButton_Reply = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_READ_BUTTON_REPLY );
		pButton->CreateBaseButton( "POSTBOX_PAGE_READ_BUTTON_REPLY", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_BUTTON", 3 ) );
		RegisterControl( pButton );

		m_pButton_Close = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_READ_BUTTON_CLOSE );
		pButton->CreateBaseButton( "POSTBOX_PAGE_READ_BUTTON_CLOSE", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_BUTTON", 3 ) );
		RegisterControl( pButton );
	}

	// Note : 이미지
	{
		m_pImage_Money = CreateSwapImage ( "POSTBOX_PAGE_READ_IMAGE_MONEY" );
		m_pImage_Money->SetImage ( "POSTBOX_PAGE_READ_IMAGE_MONEY_DEFAULT" );
	}

	Reset ();
}

void CPostBoxReadPage::OpenSubControl ()
{
	Reset ();
}

void CPostBoxReadPage::CloseSubControl()
{
	// Note : 현재 창을 닫는다.
	RequestPush( CPostBoxWindow::POSTBOX_PAGE_REQ_RECEIVE_SELECT_RESET );
}

void CPostBoxReadPage::Reset ()
{
	if ( !m_pPost )
	{
		m_pText_Date	 ->ClearText ();
		m_pText_From	 ->ClearText ();
		m_pText_Subject  ->ClearText ();
		m_pText_Contents ->ClearText ();
		m_pText_LeftTime ->ClearText ();
		m_pText_Money	 ->ClearText ();

		m_pImage_Money->SetImage ( "POSTBOX_PAGE_READ_IMAGE_MONEY_DEFAULT" );

		m_pItem->SetVisibleSingle ( FALSE );

		// Note : 가변 텍스트 업데이트
		UpdateVariableText ();

		// Note : 가변 컨트롤 업데이트
		UpdateVariableControl ();

		return;
	}

    CTime sendDate ( m_pPost->m_SendDate );
    m_pGaeaClient->GetConvertServerTime( sendDate );
    CString strTime;
    strTime.Format( "%04d-%02d-%02d %02d:%02d:%02d", sendDate.GetYear(), sendDate.GetMonth(), sendDate.GetDay(),
        sendDate.GetHour(), sendDate.GetMinute(), sendDate.GetSecond());
	m_pText_Date	->SetOneLineText ( strTime							   );
	m_pText_From	->SetOneLineText ( m_pPost->m_SenderName, NS_UITEXTCOLOR::DODGERBLUE );
	m_pText_Subject ->SetOneLineText ( m_pPost->m_Title,		cTITLE_COLOR			   );

	// Note : 내용
	m_pText_Contents->ClearText();
	m_pText_Contents->AddText ( m_pPost->m_Content );

	// Note : 남은 날짜
	{
		DWORD dwDay = m_pPost->GetExpireDay(m_pGaeaClient);

		CString strLeftTime;
		strLeftTime.Format( "%d%s", dwDay, ID2GAMEWORD("POSTBOX_EXPIRE_DATE", 0) );
		m_pText_LeftTime->SetOneLineText ( strLeftTime, NS_UITEXTCOLOR::ORANGE );
	}

	// Note : 디자인카드
	ResetDesignCard ();

	// Note : 가변 컨트롤 업데이트
	UpdateVariableControl ();

	// Note : 가변 텍스트 업데이트
	UpdateVariableText ();
}

void CPostBoxReadPage::ResetDesignCard ()
{
	if ( !m_pPost )
	{
		return;
	}

	DWORD dwDesign = m_pPost->m_iDesign;

	if ( dwDesign != 0 )
	{
		DWORD dwPosX   = ((dwDesign-1)%nDESIGN_CARD_NUM_WIDTH) * nDESIGN_CARD_SIZE_WIDTH;
		DWORD dwPosY   = ((dwDesign-1)/nDESIGN_CARD_NUM_WIDTH) * nDESIGN_CARD_SIZE_HEIGHT;

		m_pLineBox_Contents->SetBackgroundImageEx	( "POSTBOX_DISIGN_CARD", dwPosX, dwPosY, nDESIGN_CARD_SIZE_WIDTH, nDESIGN_CARD_SIZE_HEIGHT );
		m_pLineBox_Contents->SetBackgroundView		( TRUE );
	}
	else
	{
		m_pLineBox_Contents->SetBackgroundView		( FALSE );
	}
}

void CPostBoxReadPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	// Note : 우편의 상태에 따른 금액,아이템 업데이트
	//
	if ( m_pPost )
	{
		if ( m_pPost->m_byMoneyAccept )
		{
			m_pPost->m_llAttachMoney = 0;
		}

		if ( m_pPost->m_byChargePayment )
		{
			m_pPost->m_llCharge = 0;
		}

		if ( m_pPost->m_byAttach01Accept )
		{
			m_pPost->m_sAttach01.GetNativeID() == NATIVEID_NULL();
		}
	}
	// Note : 가변 컨트롤 업데이트
	UpdateVariableControl ();

	// Note : 가변 텍스트
	UpdateVariableText ();

	CPostBoxPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPostBoxReadPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{	
	case POSTBOX_PAGE_READ_ITEM:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				// Note : 아이템 정보를 보여준다.
				if ( m_pPost && !m_pPost->m_byAttach01Accept && m_pPost->m_sAttach01.GetNativeID() != NATIVEID_NULL() )
				{
					m_pInterface->SHOW_ITEM_INFO(
						m_pPost->m_sAttach01,
						FALSE, FALSE, FALSE, FALSE, FALSE,
						USHRT_MAX, USHRT_MAX,
						NATIVEID_NULL() );
				}
			}
		}
		break;

	// Note : 받기
	case POSTBOX_PAGE_READ_BUTTON_TAKE:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if ( !m_pPost )
				{
					break;
				}

				// Note : 대금청구일 경우
				if ( m_pPost->m_byChargePayment || m_pPost->m_llCharge )
				{
					std::string strModal  = sc::string::format ( ID2GAMEINTEXT("MODAL_POSTBOX_2005", 1), m_pPost->m_llCharge );
					m_pInterface->DoModal( strModal, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_POSTBOX_PAGE_READ_BUTTON_TAKE );
				}
				else
				{
					ReqTakeItem();
				}
			}
		}
		break;

	// Note : 반송
	case POSTBOX_PAGE_READ_BUTTON_RETURN:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if ( !m_pPost )
				{
					break;
				}

				// Note : 대금청구일 경우
				if ( m_pPost->m_byChargePayment || m_pPost->m_llCharge )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_POSTBOX", 7),  UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_POSTBOX_PAGE_READ_BUTTON_RETURN );
				}
				else
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_POSTBOX", 1),  UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_POSTBOX_PAGE_READ_BUTTON_RETURN );
				}
			}
		}
		break;

	// Note : 삭제
	case POSTBOX_PAGE_READ_BUTTON_DELETE:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_POSTBOX", 0),  UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_POSTBOX_PAGE_READ_BUTTON_DELETE );
			}
		}
		break;

	// Note : 답장
	case POSTBOX_PAGE_READ_BUTTON_REPLY:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				RequestPush( CPostBoxWindow::POSTBOX_PAGE_REQ_READ_REPLY );
			}
		}
		break;

	// Note : 현재창 닫기
	case POSTBOX_PAGE_READ_BUTTON_CLOSE:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				AddMessageEx ( UIMSG_POSTBOX_PAGE_CLOSE );
			}
		}
		break;
	};

	CPostBoxPage::TranslateUIMessage ( ControlID, dwMsg );
}

void CPostBoxReadPage::UpdateVariableControl ()
{
	// Note : 버튼 텍스트 바꾸기
	{
		// Note : 대금 청구 일때
		if ( m_pPost )
		{
			if ( m_pPost->m_byChargePayment || m_pPost->m_llCharge )
			{
				m_pButton_Take   -> SetOneLineText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_BUTTON", 4 ) );
				m_pButton_Return -> SetOneLineText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_BUTTON", 5 ) );
			}
			else
			{
				m_pButton_Take   -> SetOneLineText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_BUTTON", 0 ) );
				m_pButton_Return -> SetOneLineText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_READ_BUTTON", 1 ) );
			}
		}
	}

	// Note : 첨부 아이템
	//
	{
		if ( m_pPost && !m_pPost->m_byAttach01Accept && m_pPost->m_sAttach01.GetNativeID() != NATIVEID_NULL() )
		{
			const SITEM* pItemData = GLogicData::GetInstance().GetItem ( m_pPost->m_sAttach01.GetNativeID() );

			m_pItem->SetVisibleSingle ( TRUE );
			m_pItem->SetItem	( m_pPost->m_sAttach01 );
			m_pItem->SetTurnNum ( m_pPost->m_sAttach01.wTurnNum );
		}
		else
		{
			m_pItem->SetTurnNum ( 0 );
			m_pItem->SetVisibleSingle ( FALSE );
		}
	}

	// Note : 받기 버튼
	//
	if ( m_pPost )
	{
		BOOL bReceive = TRUE;

		// Note : 돈을 이미 받았거나 대금을 지불했을때.
		//
		if ( m_pPost->m_byMoneyAccept || m_pPost->m_byChargePayment )
		{
			// Note : 아이템도 없다면
			if ( m_pPost->m_sAttach01.GetNativeID() == NATIVEID_NULL() || m_pPost->m_byAttach01Accept )
			{
				bReceive = FALSE;
			}
		}

		// Note : 아이템이 없는 상태에서
		//
		if ( m_pPost->m_sAttach01.GetNativeID() == NATIVEID_NULL() || m_pPost->m_byAttach01Accept )
		{
			if ( m_pPost->m_llAttachMoney == 0 && m_pPost->m_llCharge == 0 )
			{
				bReceive = FALSE;
			}
		}

		if ( bReceive )
		{
			m_pButton_Take->SetVisibleSingle( TRUE );
		}
		else
		{
			m_pButton_Take->SetVisibleSingle( FALSE );
		}
	}
	else
	{
		m_pButton_Take->SetVisibleSingle( FALSE );
	}


	// Note : 반송 버튼
	//
	if ( m_pPost )
	{
		// Note : 반송이 가능하다면
		if (m_pGaeaClient->GetPostClient()->IsReturnAblePost(m_pPost))
		{
			m_pButton_Return->SetVisibleSingle( TRUE );
		}
		else
		{
			m_pButton_Return->SetVisibleSingle( FALSE );
		}
	}
	else
	{
		m_pButton_Return->SetVisibleSingle( FALSE );
	}

	// Note : 삭제 버튼
	//
	if ( m_pPost )
	{
		// Note : 삭제가 가능하다면
		if (m_pGaeaClient->GetPostClient()->IsDeleteAblePost(m_pPost))
		{
			m_pButton_Delete->SetVisibleSingle( TRUE );
		}
		else
		{
			m_pButton_Delete->SetVisibleSingle( FALSE );
		}
	}
	else
	{
		m_pButton_Return->SetVisibleSingle( FALSE );
	}

	// Note : 금액 아이콘
	//
	if ( m_pPost )
	{
		LONGLONG llMoney = 0;

		if ( m_pPost->m_byChargePayment == FALSE && m_pPost->m_llCharge )
		{
			llMoney = m_pPost->m_llCharge;
			m_pImage_Money->SetImage ( "POSTBOX_PAGE_READ_IMAGE_MONEY_BILLING" );
		}
		else if ( m_pPost->m_byMoneyAccept == FALSE && m_pPost->m_llAttachMoney )
		{
			llMoney = m_pPost->m_llAttachMoney;
			m_pImage_Money->SetImage ( "POSTBOX_PAGE_READ_IMAGE_MONEY_ENABLE" );
		}

		if ( !llMoney )
		{
			m_pImage_Money->SetImage ( "POSTBOX_PAGE_READ_IMAGE_MONEY_DEFAULT" );
		}
	}
}

void CPostBoxReadPage::UpdateVariableText ()
{
	GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();

	CString strText;

	// Note : 보유금액
	{
		strText = NS_UITEXTUTIL::MAKE_MONEY_FORMAT ( sCharData.GetInvenMoney(), nMONEYUNIT, "," );
		m_pText_MyMoney->SetOneLineText ( strText );
	}

	// Note : 돈 관련 정적 텍스트
	if ( m_pPost )
	{
		if ( !m_pPost->m_byChargePayment && m_pPost->m_llCharge )
		{
			m_pText_Static_ReceiveMoney->SetVisibleSingle( FALSE );
			m_pText_Static_BillingMoney->SetVisibleSingle( TRUE  );
		}
		else
		{
			m_pText_Static_ReceiveMoney->SetVisibleSingle( TRUE  );
			m_pText_Static_BillingMoney->SetVisibleSingle( FALSE );
		}
	}

	// Note : 첨부,청구 금액
	if ( m_pPost )
	{
		CString  strMoney;
		D3DCOLOR cColor = NS_UITEXTCOLOR::ENABLE;

		if ( !m_pPost->m_byChargePayment && m_pPost->m_llCharge )
		{
			// Note : 대금 청구일시 -를 붙인다.
			//
			strMoney  = "-";
			strMoney += NS_UITEXTUTIL::MAKE_MONEY_FORMAT ( m_pPost->m_llCharge, nMONEYUNIT, "," );

			cColor = NS_UITEXTCOLOR::RED;
		}
		else if ( !m_pPost->m_byMoneyAccept && m_pPost->m_llAttachMoney )
		{
			strMoney = NS_UITEXTUTIL::MAKE_MONEY_FORMAT ( m_pPost->m_llAttachMoney, nMONEYUNIT, "," );
			cColor	 = NS_UITEXTCOLOR::ENABLE;
		}
		else
		{
			strMoney = "0";
			cColor	 = NS_UITEXTCOLOR::ENABLE;
		}

		m_pText_Money->SetOneLineText( strMoney, cColor );
	}
	else
	{
		m_pText_Money->SetOneLineText( "0", NS_UITEXTCOLOR::ENABLE );
	}
}

void CPostBoxReadPage::ReqDelete( bool bConfirmAttach )
{
	// Note : 삭제가 가능하다면
	if ( m_pPost )
	{
        if (  bConfirmAttach &&
            m_pGaeaClient->GetPostClient()->IsAttachPost( m_pPost ) )
        {
            m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_POSTBOX", 8),  UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_POSTBOX_PAGE_READ_BUTTON_DELETE_CONFIRM_ATTACH );
        }
	    else
	    {
            if ( m_pGaeaClient->GetPostClient()->IsDeleteAblePost(m_pPost) )
		        RequestPush( CPostBoxWindow::POSTBOX_PAGE_REQ_READ_DELETE );
	    }
	}
}

void CPostBoxReadPage::ReqReturn ()
{
	// Note : 반송이 가능하다면
	if ( m_pPost )
	{
		if (m_pGaeaClient->GetPostClient()->IsReturnAblePost(m_pPost))
		{
			RequestPush( CPostBoxWindow::POSTBOX_PAGE_REQ_READ_RETURN );
		}
	}
}

void CPostBoxReadPage::ReqTakeItem()
{
	if  ( m_pPost )
	{
		// Note : 대금 청구일때 소지한 금액보다 대금이 많을경우 실패
		if ( m_pPost->m_byChargePayment == FALSE && m_pPost->m_llCharge )
		{
			GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();

			if ( sCharData.GetInvenMoney() < m_pPost->m_llCharge )
			{
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_MSG_TAKE_CHAR_MONEY_LACK") );
				return;
			}
		}

		m_pGaeaClient->GetPostClient()->ReqTakeItem(m_pPost);
	}
}
