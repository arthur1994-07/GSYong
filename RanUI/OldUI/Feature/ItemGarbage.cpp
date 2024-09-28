#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImage.h"
#include "../Item/ItemMove.h"
#include "ItemGarbage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//--------------------------------------------------------------------
// CItemGarbage
//--------------------------------------------------------------------
CItemGarbage::CItemGarbage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )	// 휴지통
	: CUIWindowEx( pInterface, pEngineDevice )
	, m_pGaeaClient ( pGaeaClient )
	, m_pItemImage( NULL )
	, m_pOkButton(NULL)
{
}

CItemGarbage::~CItemGarbage()
{
}

VOID CItemGarbage::CreateSubControl()
{
	// 아이템 슬롯
	{
		m_pItemImage = new CItemImage ( m_pGaeaClient, m_pEngineDevice );
		m_pItemImage->CreateSub( this, "ITEM_GARBAGE_ITEM_SLOT", UI_FLAG_DEFAULT, ITEM_GARBAGE_ITEM_SLOT );
		m_pItemImage->CreateSubControl();
		RegisterControl( m_pItemImage );
	}

	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9 );
		CBasicTextBox* pTextBox = NULL;
		pTextBox = CreateStaticControl( "ITEM_GARBAGE_INFO_TEXT_STATIC", pFont, NS_UITEXTCOLOR::SILVER, TEXT_ALIGN_CENTER_X );
		pTextBox->AddText( ID2GAMEINTEXT( "ITEM_GARBAGE_INFO" ), NS_UITEXTCOLOR::SILVER );
	}

	// 승인, 거절
	{
		m_pOkButton = new CBasicTextButton(m_pEngineDevice);
		m_pOkButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_DEFAULT, ITEM_GARBAGE_OK_BUTTON );
		m_pOkButton->CreateBaseButton( "ITEM_GARBAGE_OK_BUTTON", CBasicTextButton::SIZE14,
									CBasicButton::CLICK_FLIP, ID2GAMEWORD( "ITEM_GARBAGE_BUTTON", 0 ) );
		m_pOkButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( m_pOkButton );

		CBasicTextButton * pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_DEFAULT, ITEM_GARBAGE_CANCEL_BUTTON );
		pButton->CreateBaseButton( "ITEM_GARBAGE_CANCEL_BUTTON", CBasicTextButton::SIZE14,
									CBasicButton::CLICK_FLIP, ID2GAMEWORD( "ITEM_GARBAGE_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}
}

VOID CItemGarbage::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	SITEMCUSTOM sItemCustom( NATIVEID_NULL() );	
	if( m_pItemImage )
	{
		if( m_pGaeaClient->GetCharacter()->GET_GARBAGE_ITEM( sItemCustom ) == FALSE )
		{
			m_pItemImage->ResetItem();
		}
		else
		{
			const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
			if( pItem )
				m_pItemImage->SetItem( sItemCustom );
			else
				m_pItemImage->ResetItem();
		}
	}
}

VOID CItemGarbage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch( ControlID )
	{
	case ITEM_GARBAGE_ITEM_SLOT:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			IItemMove* pItemMove = m_pInterface->GetItemMove();
			if( !pItemMove )
			{
				GASSERT( 0 && "CItemGarbage::TranslateUIMessage, pItemMove == NULL" );
				break;
			}

			SNATIVEID sItem = pItemMove->GetItem();
			if( sItem != NATIVEID_NULL() )
			{
				const UIRECT& rcSlotPos = m_pItemImage->GetGlobalPos();
				pItemMove->SetUIWindowGlobalPos( rcSlotPos );
				m_pInterface->SetSnapItem();
			}

			if( dwMsg & UIMSG_LB_UP )
				m_pGaeaClient->GetCharacter()->ReqGarbageMoveItem();

			SITEMCUSTOM sItemCustom(NATIVEID_NULL());
			if( m_pGaeaClient->GetCharacter()->GET_GARBAGE_ITEM( sItemCustom ) == TRUE )
			{
				m_pInterface->SHOW_ITEM_INFO(
					sItemCustom,
					FALSE, FALSE, FALSE, FALSE, FALSE,
					0, 0 );
			}
		}
		break;

	case ITEM_GARBAGE_OK_BUTTON:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			SITEMCUSTOM sItemCustom(NATIVEID_NULL());
			if ( m_pGaeaClient->GetCharacter()->GET_GARBAGE_ITEM(sItemCustom) == FALSE )
				break;

			BYTE nType = 0;
			LONGLONG nCost = 0;
			bool bOk = m_pGaeaClient->GetCharacter()->IsValidGarbageUse( sItemCustom.GetNativeID(), nType, nCost );
			if( !bOk )
			{
				if( nType == GLCHARLOGIC::EGarbageMoney ||
					nType == GLCHARLOGIC::EGarbagePoint )
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DISABLE,
						ID2GAMEINTEXT("EMGARBAGE_RESULT_FB_MSG", EMGARBAGE_RESULT_FB_MONEY) );
				}
				else
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DISABLE,
						ID2GAMEINTEXT("EMGARBAGE_RESULT_FB_MSG", EMGARBAGE_RESULT_FB_NOITEM) );
				}
				break;
			}

			m_pInterface->DoModal(
				ID2GAMEINTEXT( "ITEM_GARBAGE_OKQUESTION" ),
				UI::MODAL_QUESTION,
				UI::YESNO,
				UI::MODAL_ITEMGARBAGE_OKQUESTION );

			// Focus 문제로 메세지 삭제
			SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED );
		}
		break;

	case ITEM_GARBAGE_CANCEL_BUTTON:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			m_pInterface->CloseItemGarbageWindow();
		break;

	case ET_CONTROL_BUTTON:
		if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			m_pInterface->CloseItemGarbageWindow();
		break;
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}
