#include "StdAfx.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogic/Inventory/GLInventory.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../InnerInterface.h"
#include "TradeControlPage.h"
#include "TradePage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CTradeControlPage::nOUTOFRANGE = -1;

CTradeControlPage::CTradeControlPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
	, m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pMoney ( NULL )
    , m_pPlayerName ( NULL )
    , m_pLock(NULL)
{
}

CTradeControlPage::~CTradeControlPage ()
{
}

void CTradeControlPage::CreateSubControl ( BOOL bMyPage )
{
	m_bMyPage = bMyPage;

	CreateControl ( "TRADE_CONTROL_PAGE_NAME_BACK" );
	//	뒷 배경은 컨트롤로 제작할만큼 중요하지 않은듯하여,
	//	두개로 잘라서 그냥 뿌리도록 합니다.
	CreateControl ( "TRADE_CONTROL_PAGE_MONEY_BACK_LEFT" );
	CreateControl ( "TRADE_CONTROL_PAGE_MONEY_BACK_RIGHT" );

	CreateControl ( "TRADE_MONEY_BUTTON" );

//	CBasicButton* pButton = NULL;
//	pButton = CreateFlipButton ( "TRADE_MONEY_BUTTON", "TRADE_MONEY_BUTTON_F", TRADE_MONEY_BUTTON, CBasicButton::CLICK_FLIP );
//	pButton->CreateMouseOver ( "TRADE_MONEY_BUTTON_F" );
//	pButton->SetUseDynamic ( TRUE );

	{
		m_pOK = CreateTextButton ( "TRADE_OK_BUTTON", TRADE_OK_BUTTON, (char*)ID2GAMEWORD ( "TRADE_OKCANCEL_BUTTON", 0 ) );
		m_pOK->SetFlip ( FALSE );

		if ( m_bMyPage )
		{
			m_pCANCEL = CreateTextButton ( "TRADE_CANCEL_BUTTON", TRADE_CANCEL_BUTTON, (char*)ID2GAMEWORD ( "TRADE_OKCANCEL_BUTTON", 1 ) );
			m_pCANCEL->SetFlip ( TRUE );
		}

		// create TRADE_LOCK_BUTTON. by luxes.
		m_pLock = CreateTextButton( "TRADE_LOCK_BUTTON", TRADE_LOCK_BUTTON, (char*)ID2GAMEWORD( "TRADE_LOCK_BUTTON" ) );
		m_pLock->SetFlip( TRUE );
		//
	}

	CTradePage*	pTradePage = new CTradePage ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pTradePage->CreateSub ( this, "TRADE_CONTROL_PAGE_SLOT_PAGE", UI_FLAG_DEFAULT, TRADE_SLOT_PAGE );
	pTradePage->CreateSubControl ();
	RegisterControl ( pTradePage );
	m_pPage = pTradePage;

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont8Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pPlayerName = CreateStaticControl ( "TRADE_NAME_BOX", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMoney = CreateStaticControl ( "TRADE_MONEY_BOX", pFont8Shadow, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMoney->SetOneLineText ( "0" );
}

CUIControl*	CTradeControlPage::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextButton*	 CTradeControlPage::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::RADIO_FLIP, szText );
	pTextButton->SetFlip ( FALSE );
	RegisterControl ( pTextButton );
	return pTextButton;
}


CBasicButton* CTradeControlPage::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID, WORD wFlipType )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, wFlipType );
	RegisterControl ( pButton );
	return pButton;
}


CBasicTextBox* CTradeControlPage::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CTradeControlPage::SetPlayerName ( CString strName )
{
	if ( m_pPlayerName ) m_pPlayerName->SetOneLineText ( strName );
}

void CTradeControlPage::SetMoney ( LONGLONG lnMoney )
{
	CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( lnMoney, 3, "," );
	if ( m_pMoney ) m_pMoney->SetOneLineText ( strTemp, MONEYCOLOR::GetMoneyColor((LONGLONG)lnMoney) );
}


void CTradeControlPage::ApplyButtonUpdate ()
{
	if ( m_bMyPage )
	{
		if ( m_pGaeaClient->GetTradeClient()->GetMyTrade().IsAgreeAble() )
		{
			m_pOK->SetVisibleSingle ( TRUE );
			m_pLock->SetVisibleSingle( TRUE );

			BOOL bLock = m_pGaeaClient->GetTradeClient()->GetMyTrade().IsLock();
			m_pLock->SetFlip( !bLock );
			m_pPage->SetAllFlipItem ( bLock );

			BOOL bLockAgree = ( bLock && m_pGaeaClient->GetTradeClient()->GetTarTrade().IsLock() );
			BOOL bAgree = m_pGaeaClient->GetTradeClient()->GetMyTrade().GetAgree();

			if ( bLockAgree )
				m_pOK->SetFlip( !bAgree );
			else
				m_pOK->SetFlip( bLockAgree );
		}
		else
		{
			m_pOK->SetVisibleSingle ( FALSE );
			m_pPage->SetAllFlipItem ( FALSE );
			m_pLock->SetVisibleSingle( TRUE );
		}
	}
	else
	{
		if ( m_pGaeaClient->GetTradeClient()->GetTarTrade().IsAgreeAble() )
		{
			m_pOK->SetVisibleSingle ( TRUE );
			m_pLock->SetVisibleSingle( TRUE );

			BOOL bLock = m_pGaeaClient->GetTradeClient()->GetTarTrade().IsLock();
			m_pLock->SetFlip( !bLock );
			m_pPage->SetAllFlipItem ( bLock );

			BOOL bLockAgree = ( bLock && m_pGaeaClient->GetTradeClient()->GetMyTrade().IsLock() );
			BOOL bAgree = m_pGaeaClient->GetTradeClient()->GetTarTrade().GetAgree();

			if ( bLockAgree )
				m_pOK->SetFlip( !bAgree );
			else
				m_pOK->SetFlip( bLockAgree );
		}
		else
		{
			m_pOK->SetVisibleSingle ( FALSE );
			m_pPage->SetAllFlipItem ( FALSE );
			m_pLock->SetVisibleSingle( TRUE );
		}
	}
}

void CTradeControlPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	ApplyButtonUpdate ();

	SetItemIndex ( nOUTOFRANGE, nOUTOFRANGE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime,bFirstControl );

	if ( m_bMyPage )
	{
		GLTrade& MyTrade = m_pGaeaClient->GetTradeClient()->GetMyTrade ();
		SetMoney ( MyTrade.GetMoney () );

		GLInventory& ref_ItemData = m_pGaeaClient->GetTradeClient()->GetMyTradeBox();		
		if ( m_pPage ) m_pPage->LoadItemPage ( ref_ItemData );
	}
	else
	{
		GLTrade& TarTrade = m_pGaeaClient->GetTradeClient()->GetTarTrade ();
		SetMoney ( TarTrade.GetMoney () );

		GLInventory& ref_ItemData = m_pGaeaClient->GetTradeClient()->GetTarTradeBox();
		if ( m_pPage ) m_pPage->LoadItemPage ( ref_ItemData );
	}
}

void CTradeControlPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case TRADE_SLOT_PAGE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nPosX, nPosY;
				m_pPage->GetItemIndex ( &nPosX, &nPosY );

				CDebugSet::ToView ( 1, 1, "인벤 : %d %d", nPosX, nPosY );

				SetItemIndex ( nPosX, nPosY );

				if ( nPosX < 0 || nPosY < 0 ) return;

////				if ( CUIMan::GetFocusControl () == this )
//				{
//					SINVENITEM sInvenItem = m_pPage->GetItem ( nPosX, nPosY );
//					if ( sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL () )
//					{
//						m_pItemInfo->LoadInvenItemInfo ( sInvenItem.sItemCustom, FALSE, FALSE );
//						m_pItemInfo->SetVisibleSingle ( TRUE );
//					}
//				}

				if ( m_bMyPage )
				{
					if ( dwMsg & UIMSG_LB_UP )
					{
						if ( m_pGaeaClient->GetTradeClient()->GetMyTrade().IsLock() )
						{	// 거래 창 잠겨 있으면 아이템 올릴 수 없음. by luxes.
							m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_LOCK_MESSAGE") );
						}
						else
						{
							m_pGaeaClient->GetCharacter()->ReqTradeBoxTo ( nPosX, nPosY );
						}
					}
				}
			}
		}
		break;

	case TRADE_OK_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			if ( m_bMyPage )
			{
				if ( !m_pGaeaClient->GetTradeClient()->GetMyTrade().IsLock() )
				{	// 거래 창 잠겨 있지 않으면, 승인 할 수 없음.
					m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_NEED_LOCK") );
				}
				else
				{
					//	승인 버튼 누름
					//
					m_pGaeaClient->GetCharacter()->ReqTradeAgree();
				}
			}
		}
		break;

	case TRADE_CANCEL_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			if ( m_bMyPage )
			{
				m_pInterface->SetTradeWindowCloseReq ();
			}
		}
		break;

	case TRADE_LOCK_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if ( m_bMyPage )
				{
					m_pGaeaClient->GetCharacter()->ReqTradeLock();
					m_pGaeaClient->GetTradeClient()->GetMyTrade().SetLock( TRUE );
				}
			}
		}
		break;
	}
}


void CTradeControlPage::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void CTradeControlPage::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}

const SINVENITEM* CTradeControlPage::GetItem(int nPosX, int nPosY) const
{
	return m_pPage->GetItem ( nPosX, nPosY );
}
