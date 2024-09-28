#include "StdAfx.h"
#include "./RnPageBase.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "../PointShop/PointShopWindow.h"
#include "../../../RanLogicClient/StaticHoldItem.h"
#include "./ItemSlot.h"

#include "../../../SigmaCore/DebugInclude.h"

CRnPageBase::CRnPageBase( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIGroupHelperMessageBox( pEngineDevice )

	, m_pGaeaClient( pGaeaClient )
	, m_pInterface( pInterface )

	, m_pPointTextBox( NULL )
	, m_pMoneyTextBox( NULL )

	, m_nPageMax( 0 )
	, m_nPage( 0 )

	, m_nFocusX( INT_MIN )
	, m_nFocusY( INT_MIN )
{
	for( int i=0; i<ESlotY; ++i )
		m_pItemSlot[ i ] = NULL;
}

CRnPageBase::~CRnPageBase()
{
}

void CRnPageBase::CreateSubControl()
{
	CreateLineBox( "INVENTORY_WINDOW_ITEM_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "INVENTORY_WINDOW_POINT_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "INVENTORY_WINDOW_MONEY_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );

	for( int i=0; i<m_nPageMax; ++i )
	{
		CString strButtonControl;
		strButtonControl.Format( "INVENTORY_WINDOW_RADIO_BUTTON_%d", i + 1 );

		CString strTextValue;
		strTextValue.Format( "%d", i + 1 );

		DEFAULT_RADIO_BUTTON* pRadio = new DEFAULT_RADIO_BUTTON( m_pEngineDevice );
		pRadio->CreateSub( this, strButtonControl.GetString(), UI_FLAG_DEFAULT, ERadioBegin + i );
		pRadio->CreateSubControl(
			strTextValue.GetString(),
			TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP,
			"INVENTORY_WINDOW_RADIO_BUTTON_TEXT",
			NS_UITEXTUTIL::BLACK,
			"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX",
			"CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO",
			"CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO" );
		RegisterControl( pRadio );
		RegisterTapButton( pRadio );
	}

	CreateFlipButton( "INVENTORY_WINDOW_HELP_BUTTON", "INVENTORY_WINDOW_HELP_BUTTON_OVER", EHelpButton, CBasicButton::MOUSEIN_FLIP );

	for( int i=0; i<ESlotY; ++i )
	{
		CItemSlot* pItemSlot = new CItemSlot( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pItemSlot->CreateSub( this, "INVENTORY_ITEM_SLOT", UI_FLAG_DEFAULT, EItemBegin + i );
		pItemSlot->CreateSubControl( ESlotX, TRUE );
		RegisterControl( pItemSlot );
		m_pItemSlot[ i ] = pItemSlot;

		UIRECT rcLocalPos = pItemSlot->GetLocalPos();
		float fAddHeight = ( rcLocalPos.sizeY + 1.0f ) * static_cast< float >( i );
		rcLocalPos.top += fAddHeight;
		rcLocalPos.bottom += fAddHeight;
		pItemSlot->SetLocalPos( rcLocalPos );
	}

	{
		CBasicButton* pButton = CreateFlipButton( "INVENTORY_POINT_BUTTON_RENEW", "INVENTORY_POINT_BUTTON_RENEW_F", EPointButton, CBasicButton::CLICK_FLIP );
		pButton->CreateMouseOver( "INVENTORY_POINT_BUTTON_RENEW_F" );
		pButton->SetAlignFlag( UI_FLAG_BOTTOM );
		pButton->SetUseDynamic( TRUE );

		CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
		m_pPointTextBox = CreateStaticControl( "INVENTORY_POINT_TEXT_RENEW", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		m_pPointTextBox->SetAlignFlag( UI_FLAG_BOTTOM );
	}

	{
		CBasicButton* pButton = CreateFlipButton( "INVENTORY_MONEY_BUTTON_RENEW", "INVENTORY_MONEY_BUTTON_RENEW_F", EMoneyButton, CBasicButton::CLICK_FLIP );
		pButton->CreateMouseOver( "INVENTORY_MONEY_BUTTON_RENEW_F" );
		pButton->SetAlignFlag( UI_FLAG_BOTTOM );
		pButton->SetUseDynamic( TRUE );

		CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
		m_pMoneyTextBox = CreateStaticControl( "INVENTORY_MONEY_TEXT_RENEW", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		m_pMoneyTextBox->SetAlignFlag( UI_FLAG_BOTTOM );
	}

	{
		const float EventPressedTime = 0.05f;

		RnButton::CreateArg arg;
		arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
		arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
		arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
		arg.pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );

		RnButton* pButton = new RnButton( m_pEngineDevice );
		pButton->CreateSub( this, "INVENTORY_WINDOW_LOCKER_BUTTON", UI_FLAG_DEFAULT, ELockerButton );
		pButton->CreateSubControl( arg );
		pButton->SetPressedEventTime( EventPressedTime );
		RegisterControl( pButton );

		CUIControl* pImage = new CUIControl( m_pEngineDevice );
        pImage->CreateSub( this, "INVENTORY_WINDOW_LOCKER_IMAGE" );
        RegisterControl( pImage );

		pButton = new RnButton( m_pEngineDevice );
		pButton->CreateSub( this, "INVENTORY_WINDOW_EXTEND_BUTTON", UI_FLAG_DEFAULT, EExtendButton );
		pButton->CreateSubControl( arg );
		pButton->SetPressedEventTime( EventPressedTime );
		RegisterControl( pButton );

		pImage = new CUIControl( m_pEngineDevice );
        pImage->CreateSub( this, "INVENTORY_WINDOW_EXTEND_IMAGE" );
        RegisterControl( pImage );

		pButton = new RnButton( m_pEngineDevice );
		pButton->CreateSub( this, "INVENTORY_WINDOW_TRASH_BUTTON", UI_FLAG_DEFAULT, ETrashButton );
		pButton->CreateSubControl( arg );
		pButton->SetPressedEventTime( EventPressedTime );
		RegisterControl( pButton );

		pImage = new CUIControl( m_pEngineDevice );
        pImage->CreateSub( this, "INVENTORY_WINDOW_TRASH_IMAGE" );
        RegisterControl( pImage );

		pButton = new RnButton( m_pEngineDevice );
		pButton->CreateSub( this, "INVENTORY_WINDOW_SORT_BUTTON", UI_FLAG_DEFAULT, ESortButton );
		pButton->CreateSubControl( arg );
		pButton->SetPressedEventTime( EventPressedTime );
		RegisterControl( pButton );

		pImage = new CUIControl( m_pEngineDevice );
        pImage->CreateSub( this, "INVENTORY_WINDOW_SORT_IMAGE" );
		RegisterControl( pImage );

		if ( GLUseFeatures::GetInstance().IsUsingItemDecompose() )
		{
			pButton = new RnButton( m_pEngineDevice );
			pButton->CreateSub( this, "INVENTORY_WINDOW_SPLIT_BUTTON", UI_FLAG_DEFAULT, ESplitButton );
			pButton->CreateSubControl(arg);
			pButton->SetPressedEventTime( EventPressedTime );
			RegisterControl( pButton );

			pImage = new CUIControl( m_pEngineDevice );
			pImage->CreateSub( this, "INVENTORY_WINDOW_SPLIT_IMAGE" );
			RegisterControl( pImage );
		}
	}

    CreateMessageBox();
	EventSelectedTap( ERadioBegin );
}

void CRnPageBase::SetVisibleSingle( BOOL bVisible )
{
    CUIGroupHelperMessageBox::SetVisibleSingle( bVisible );
}

void CRnPageBase::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelperMessageBox::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return;

	CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pChar->PointShopPoint(), 3, "," );
	m_pPointTextBox->SetOneLineText( strText, NS_UITEXTCOLOR::ORANGE );

	strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pChar->GetInvenMoney(), 3, "," );
	m_pMoneyTextBox->SetOneLineText( strText );
}

void CRnPageBase::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );
	CUIGroupHelperMessageBox::TranslateUIMessage( ControlID, dwMsg );

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return;

	switch( ControlID )
	{
	case EPointButton:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->MultiLineCommentUtil(
				ID2GAMEINTEXT( "INVENTORY_POINT_TEXT" ),
				m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG ),
				800.0f,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,
				NS_UITEXTCOLOR::LIGHTSKYBLUE );

			if( CHECK_LB_UP_LIKE( dwMsg ) )
				m_pInterface->ToggleWindowPointshop();
		}
		break;

	case EMoneyButton:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->MultiLineCommentUtil(
				ID2GAMEINTEXT( "INVENTORY_MONEY_TEXT" ),
				m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG ),
				800.0f,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,
				NS_UITEXTCOLOR::LIGHTSKYBLUE );

			if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				if( m_pInterface->UiIsVisibleGroup( STORAGE_WINDOW ) )
				{
					m_pInterface->DoModal(
						ID2GAMEINTEXT( "PLAYINTERFACE_31" ),
						UI::MODAL_INPUT,
						UI::EDITBOX_MONEY,
						UI::MODAL_SAVEMONEY );
				}
				else if( m_pInterface->UiIsVisibleGroup( CLUB_STORAGE_WINDOW ) )
				{
					m_pInterface->DoModal(
						ID2GAMEINTEXT( "PLAYINTERFACE_31" ),
						UI::MODAL_INPUT,
						UI::EDITBOX_MONEY,
						UI::MODAL_CLUB_SAVEMONEY );
				}
				else if( m_pInterface->UiIsVisibleGroup( TRADE_WINDOW ) )
				{
					if( m_pGaeaClient->GetTradeClient()->GetMyTrade().IsLock() )
					{
						m_pGaeaClient->PrintMsgText(
							NS_UITEXTCOLOR::DISABLE,
							ID2GAMEINTEXT( "TRADE_LOCK_MESSAGE" ) );
					}
					else
					{
						m_pInterface->DoModal(
							ID2GAMEINTEXT( "TRADE_MONEY" ),
							UI::MODAL_INPUT,
							UI::EDITBOX_MONEY,
							UI::MODAL_TRADEMONEY );
					}
				}
				else
				{
					if( GLCONST_CHAR::bMONEY_DROP2FIELD )
					{
						m_pInterface->DoModal(
							ID2GAMEINTEXT( "MONEYTO_FIELD" ),
							UI::MODAL_INPUT,
							UI::EDITBOX_MONEY,
							UI::MODAL_INVENTORY_MONEY_TO_FIELD );
					}
				}
			}
		}
		break;

	case ELockerButton:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->SHOW_COMMON_LINEINFO(
				ID2GAMEWORD( "INVENTORY_BUTTON_TEXT", 0 ),
				NS_UITEXTCOLOR::LIGHTSKYBLUE,
				TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );

			if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				if( m_pInterface->IsStorageWindowOpen() )
				{
					m_pInterface->UiHideGroup( STORAGE_WINDOW );
					pChar->ReqStorageCloseCard();
				}
				else
				{
					pChar->ReqStorageOpenFromInven();
				}
			}
		}
		break;

	case EExtendButton:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->SHOW_COMMON_LINEINFO(
				ID2GAMEWORD( "INVENTORY_BUTTON_TEXT", 1 ),
				NS_UITEXTCOLOR::LIGHTSKYBLUE,
				TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );

			if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				if( !GLUseFeatures::GetInstance().IsUsingPointShop() )
					return;

				IPointShopWindow* pWindow = m_pInterface->GetPointShopWindow();
				if( !pWindow )
					return;

				if( m_pInterface->UiIsVisibleGroup( POINT_SHOP_WINDOW ) )
				{
					if( pWindow->GetSearchName() == GLCONST_CHAR::sInvenExItemSearch )
						m_pInterface->UiHideGroup( POINT_SHOP_WINDOW );
					else
						pWindow->SearchItem( GLCONST_CHAR::sInvenExItemSearch );
				}
				else
				{
					m_pInterface->UiShowGroupFocus( POINT_SHOP_WINDOW );
 					pWindow->SearchItem( GLCONST_CHAR::sInvenExItemSearch );
				}
			}
		}
		break;

	case ETrashButton:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->SHOW_COMMON_LINEINFO(
				ID2GAMEWORD( "INVENTORY_BUTTON_TEXT", 2 ),
				NS_UITEXTCOLOR::LIGHTSKYBLUE,
				TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );

			if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				if ( holditem::HaveHoldItem() )
				{
					holditem::ReleaseHoldItem();
					break;
				}

				if( m_pInterface->UiIsVisibleGroup( ITEM_GARBAGE_WINDOW ) )
				{
					m_pInterface->CloseItemGarbageWindow();
				}
				else
				{
					WORD nCardPosX;
					WORD nCardPosY;
					pChar->GetItemPosInInven( ITEM_GARBAGE_CARD, nCardPosX, nCardPosY );

					if( nCardPosX != USHRT_MAX && nCardPosY != USHRT_MAX )
						pChar->ReqGabargeOpenCard( nCardPosX, nCardPosY );
					else
						m_pInterface->OpenItemGarbageWindow();
				}
			}
		}
		break;

	case ESortButton:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->SHOW_COMMON_LINEINFO(
				ID2GAMEWORD( "INVENTORY_BUTTON_TEXT", 3 ),
				NS_UITEXTCOLOR::LIGHTSKYBLUE,
				TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );

			if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				if( pChar->IsEnalbeItemSort() )
				{
					pChar->ReqInvenItemSort();
				}
				else
				{
					//m_pGaeaClient->PrintMsgText(
					//	NS_UITEXTCOLOR::RED,
					//	ID2GAMEINTEXT( "ITEM_SORT_DELAY_TIME" ) );
				}
			}
		}
		break;
	}

	if( ControlID == EItemBegin )
	{
		m_nFocusX = INT_MIN;
		m_nFocusY = INT_MIN;
	}

	if( ControlID >= EItemBegin && ControlID <= EItemEnd )
	{
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_nFocusY = ControlID - EItemBegin;
			m_nFocusX = m_pItemSlot[ m_nFocusY ]->GetItemIndex();
		}
	}
}

void CRnPageBase::EventSelectedTap( UIGUID controlID )
{
	if( controlID < ERadioBegin || controlID > ERadioEnd )
		return;

	if ( m_nPage == controlID - ERadioBegin )
		return;

    CloseMessageBox();

	m_nPage = controlID - ERadioBegin;

	TapSelect( controlID );

	if( m_sWindowOpenPos.nPage >= 0 )
	{
		if( m_sWindowOpenPos.nPage != m_nPage )
		{
			for( int i=0; i< ESlotY; ++i )
			{
				if( m_pItemSlot[ i ] )
					m_pItemSlot[ i ]->SetWindowOpenBox( 0 );
			}
		}
		else
		{
			if( m_sWindowOpenPos.nPosX >= 0 && m_sWindowOpenPos.nPosY >= 0)
				m_pItemSlot[ m_sWindowOpenPos.nPosY ]->SetWindowOpenBox( m_sWindowOpenPos.nPosX, TRUE );
		}
	}
}

void CRnPageBase::HelpComment( const char* pText, DWORD nColor )
{
	m_pInterface->MultiLineCommentUtil(
		pText,
		m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG ),
		800.0f,
		TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,
		nColor );
}

void CRnPageBase::UpdateItemSlot( GLInventory& cInventory )
{
	for( int y=0; y<ESlotY; ++y )
	{
		for( int x=0; x<ESlotX; ++x )
			ResetSlotItem( x, y );
	}

	const GLInventory::CELL_MAP* pItemList = cInventory.GetItemList();
	const BOOL bDecompose = m_pInterface->IsItemDecompose();

	BOOST_FOREACH( const GLInventory::CELL_MAP::value_type& it, *pItemList )
	{
		SINVENITEM* pInvenItem = it.second;
		int nX( pInvenItem->wPosX );
		int nY( pInvenItem->wPosY );
		int nPage( 0 );
		LogicPosToUIPos( nX, nY, nPage );
		if( nPage != m_nPage )
			continue;

		SetSlotItem( nX, nY, *pInvenItem );
		// 분해 작업 시, 분해가능 아이템은 강조되어야함;
// 		if ( bDecompose )
// 		{
// 			if ( pInvenItem->CustomItem().IsDecompose() )
// 				SetBorderItemSlot(pInvenItem->wPosX, pInvenItem->wPosY);
// 			else
// 				ResetSlotItem(pInvenItem->wPosX, pInvenItem->wPosY);
// 		}
	}
}

// BOOL CRnPageBase::HighLightItemSlot( EMITEM_FLAG emItemFlag )
// {
// 	for( int y=0; y<ESlotY; ++y )
// 	{
// 		for( int x=0; x<ESlotX; ++x )
// 			ResetSlotItem( x, y );
// 	}
// 
// 	BOOL bOnLight = FALSE;
// 	const GLInventory::CELL_MAP* pItemList = m_pGaeaClient->GetCharacterData().m_cInventory.GetItemList();
// 	BOOST_FOREACH( const GLInventory::CELL_MAP::value_type& it, *pItemList )
// 	{
// 		const SINVENITEM* const pInvenItem = it.second;
// 		int nPosX(pInvenItem->wPosX), nPosY(pInvenItem->wPosY);
// 		UIPosToLogicPos( nPosX, nPosY, m_nPage );
// 
// 		const SINVENITEM* const pSlotItem = this->GetSlotItem(nPosX, nPosY);
// 		if ( pSlotItem == NULL )
// 			continue;
// 
// 		const SITEM* pItemData = GLItemMan::GetInstance().GetItem( pSlotItem->Id() );
// 		switch ( emItemFlag )
// 		{
// 		case ITEM_SPLIT:
// 			if ( pSlotItem->CustomItem().IsDecompose() )
// 			{
// 				SetBorderItemSlot(pSlotItem->wPosX, pSlotItem->wPosY);
// 				bOnLight = TRUE;
// 			}
// 			break;
// 		}
// 	}
// 	return bOnLight;
// }

void CRnPageBase::ResetSlotItem( int nX, int nY )
{
	if( nX < 0 || nX >= ESlotX )
		return;

	if( nY < 0 || nY >= ESlotY )
		return;

	m_pItemSlot[ nY ]->ResetItemImage( nX );
}

void CRnPageBase::SetSlotExchange( int nX, int nY, bool bValue )
{
	for( int i=0; i<ESlotY; ++i )
	{
		if( m_pItemSlot[ i ] )
			m_pItemSlot[ i ]->SetExchangeSlot( bValue );
	}
}

void CRnPageBase::SetSlotItem( int nX, int nY, SINVENITEM& sInvenItem )
{
	if( nX < 0 || nX >= ESlotX )
		return;

	if( nY < 0 || nY >= ESlotY )
		return;

	m_pItemSlot[ nY ]->SetItemImage( nX, sInvenItem );
}

const SINVENITEM* CRnPageBase::GetSlotItem( int nX, int nY )
{
	if( nX < 0 || nX >= ESlotX )
		return NULL;

	if( nY < 0 || nY >= ESlotY )
		return NULL;

	return m_pItemSlot[ nY ]->GetItemImage( nX );
}

const SINVENITEM* CRnPageBase::GetFocusItem()
{
	return GetSlotItem( m_nFocusX, m_nFocusY );
}

bool CRnPageBase::IsCurrentPage ( int& nX, int& nY )
{
	int iPage = 0;
	LogicPosToUIPos( nX, nY, iPage );
	if ( m_nPage == iPage )
		return true;

	return false;
}

void CRnPageBase::ChangePage ( int nPage )
{
	EventSelectedTap( nPage + ERadioBegin );

	
}

void CRnPageBase::SetBorderItemSlot ( int nX, int nY )
{
	if( nX < 0 || nX >= ESlotX )
		return;

	if( nY < 0 || nY >= ESlotY )
		return;

	m_pItemSlot[ nY ]->SetBorder( nX );
}

void CRnPageBase::SetWindowOpenBox( int nX, int nY, BOOL bShow )
{
	if( nX < 0 || nX >= ESlotX )
		return;

	if( nY < 0 || nY >= ESlotY )
		return;
	
	if( bShow ) {
		m_sWindowOpenPos.nPage = m_nPage;
		m_sWindowOpenPos.nPosX = nX;
		m_sWindowOpenPos.nPosY = nY;
	}
	else
	{
		m_sWindowOpenPos.reset();
	}

	m_pItemSlot[ nY ]->SetWindowOpenBox(nX, bShow);
}

void CRnPageBase::ResetBorderItemSlot ()
{
	for ( int i=0; i<ESlotY; ++i )
	{
		m_pItemSlot[ i ]->ResetBorder();
	}
}