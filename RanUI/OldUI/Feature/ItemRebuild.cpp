#include "StdAfx.h"
#include "ItemRebuild.h"

#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../Item/ItemImage.h"
#include "../Item/ItemMove.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../InnerInterface.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
//#include "../../ModalCallerID.h"

#include "../Feature/CostumColorInfo.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//--------------------------------------------------------------------
// ItemRebuild
//--------------------------------------------------------------------
CItemRebuild::CItemRebuild( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )	// ITEMREBUILD_MARK
	: CUIWindowEx ( pInterface, pEngineDevice )
	, m_pGaeaClient ( pGaeaClient )
	, m_pItemImage( NULL )
	, m_pCostTextBoxStatic( NULL )
	, m_pCostTextBox( NULL )
	, m_pMoneyTextBox( NULL )
	, m_pOkButton(NULL)
	, m_pColorInfo(NULL)
{
}

CItemRebuild::~CItemRebuild()
{
}

VOID CItemRebuild::CreateSubControl()
{
	// 아이템 슬롯
	{
		m_pItemImage = new CItemImage ( m_pGaeaClient, m_pEngineDevice );
		m_pItemImage->CreateSub( this, "ITEM_REBUILD_ITEM_SLOT", UI_FLAG_DEFAULT, ITEM_REBUILD_ITEM_SLOT );
		m_pItemImage->CreateSubControl();
		RegisterControl( m_pItemImage );
	}

	// 코스튬 컬러 정보. by luxes.
	{
		m_pColorInfo = new CCostumColorInfo(m_pEngineDevice);
		m_pColorInfo->CreateSub( this, "COLOR_REBUILD_BOX", UI_FLAG_DEFAULT, ITEM_COLOR_INFO );
		m_pColorInfo->CreateSubControl();
		RegisterControl( m_pColorInfo );
		m_pColorInfo->SetVisibleSingle( FALSE );
	}

	// 개조비용 : 0
	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9 );
		m_pCostTextBoxStatic = CreateStaticControl( "ITEM_REBUILD_COAST_TEXT_STATIC", pFont, NS_UITEXTCOLOR::SILVER, TEXT_ALIGN_LEFT );
		m_pCostTextBoxStatic->AddText( ID2GAMEINTEXT( "ITEM_REBUILD_COST" ), NS_UITEXTCOLOR::SILVER );

		m_pCostTextBox = CreateStaticControl( "ITEM_REBUILD_COAST_TEXT", pFont, NS_UITEXTCOLOR::SILVER, TEXT_ALIGN_LEFT );
	}

	// 내야되는돈?
	{
		CUIControl* pControl = CreateControl( "ITEM_REBUILD_MONEY_BACK" );
		pControl->SetAlignFlag( UI_FLAG_BOTTOM );

		pControl = CreateControl( "ITEM_REBUILD_MONEY_IMAGE" );
		pControl->SetAlignFlag( UI_FLAG_BOTTOM );

		CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
		m_pMoneyTextBox = CreateStaticControl( "ITEM_REBUILD_MONEY_TEXT", pFont, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		m_pMoneyTextBox->SetAlignFlag( UI_FLAG_BOTTOM );
	}

	// 승인, 거절
	{
		m_pOkButton = new CBasicTextButton(m_pEngineDevice);
		m_pOkButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_DEFAULT, ITEM_REBUILD_OK_BUTTON );
		m_pOkButton->CreateBaseButton( "ITEM_REBUILD_OK_BUTTON", CBasicTextButton::SIZE14,
									CBasicButton::CLICK_FLIP, ID2GAMEINTEXT( "ITEM_REBUILD_OK" ) );
		m_pOkButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		m_pOkButton->SetVisibleSingle( FALSE );
		RegisterControl( m_pOkButton );

		CBasicTextButton * pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_DEFAULT, ITEM_REBUILD_CANCEL_BUTTON );
		pButton->CreateBaseButton( "ITEM_REBUILD_CANCEL_BUTTON", CBasicTextButton::SIZE14,
									CBasicButton::CLICK_FLIP, ID2GAMEINTEXT( "ITEM_REBUILD_CANCEL" ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}
}

VOID CItemRebuild::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
    if ( !CUIControlNpcActionable::IsUseable(m_pGaeaClient) )
    {
        m_pInterface->SetItemRebuildWindowClose();
        return;
    }

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	SITEMCUSTOM sItemCustom = m_pGaeaClient->GetCharacter()->GET_REBUILD_ITEM();
	if( m_pItemImage )
	{
		if( sItemCustom.GetNativeID() == NATIVEID_NULL() )
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

	LONGLONG i64RebuildCost = m_pGaeaClient->GetCharacter()->GetRebuildInput();

	if( i64RebuildCost > 0 || sItemCustom.GetNativeID() != NATIVEID_NULL() ) 
		m_pOkButton->SetVisibleSingle( TRUE );
	else
		m_pOkButton->SetVisibleSingle( FALSE );

	if( m_pCostTextBox && m_pMoneyTextBox )
	{
		CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( i64RebuildCost, 3, "," );
		m_pCostTextBox->SetOneLineText( strTemp );
		m_pMoneyTextBox->SetOneLineText( strTemp );
	}

	// Note : 코스츔 염색 컬러 정보. by luxes.
	if ( sItemCustom.GetNativeID() != NATIVEID_NULL () )
	{
		if ( sItemCustom.dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
			&& sItemCustom.dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
		{
			m_pColorInfo->SetVisibleSingle( TRUE );

			m_pColorInfo->SetSelectColor( sItemCustom.dwMainColor, sItemCustom.dwSubColor );
		}
		else
		{
			m_pColorInfo->SetVisibleSingle( FALSE );
		}
	}
	else
		m_pColorInfo->SetVisibleSingle( FALSE );

	//if( m_pMoneyTextBox )
	//{
	//	LONGLONG i64RebuildInput = m_pGaeaClient->GetCharacter()->GetRebuildInput();
	//	CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( i64RebuildInput, 3, "," );
 //       m_pMoneyTextBox->SetOneLineText( strTemp );
	//}
}

VOID CItemRebuild::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch( ControlID )
	{
	case ITEM_REBUILD_ITEM_SLOT:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			IItemMove* pItemMove = m_pInterface->GetItemMove();
			if( !pItemMove )
			{
				GASSERT( 0 && "CItemRebuild::TranslateUIMessage, pItemMove == NULL" );
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
				m_pGaeaClient->GetCharacter()->ReqRebuildMoveItem();

			SITEMCUSTOM sItemCustom = m_pGaeaClient->GetCharacter()->GET_REBUILD_ITEM();
			if( sItemCustom.GetNativeID() != NATIVEID_NULL() )
			{
				m_pInterface->SHOW_ITEM_INFO(
					sItemCustom,
					FALSE, FALSE, FALSE, TRUE, FALSE,
					0, 0 );
			}
		}
		break;

	case ITEM_REBUILD_OK_BUTTON:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			m_pInterface->DoModal( ID2GAMEINTEXT("ITEM_REBUILD_OKQUESTION"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_ITEMREBUILD_OKQUESTION );
			SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제
		}
		break;

	case ITEM_REBUILD_CANCEL_BUTTON:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			m_pInterface->SetItemRebuildWindowClose();
		break;

	case ET_CONTROL_BUTTON:
		if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			m_pInterface->SetItemRebuildWindowClose();
		break;
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CItemRebuild::SetNpc( const SNATIVEID& sNPCID, const DWORD dwNPCGlobalID )
{
    if ( sNPCID == SNATIVEID(false) || dwNPCGlobalID == EMTARGET_NULL )
    {
        CUIControlNpcActionable::SetNpcActionableRemote();
    }
    else
    {
        CUIControlNpcActionable::SetNpcActionableNPCID( sNPCID, dwNPCGlobalID );
    }
}

void CItemRebuild::SetPreInventoryItem( WORD wPosX, WORD wPosY )
{
    m_sPreIventoryItem.SET( wPosX, wPosY );
}

void CItemRebuild::GetPreInventoryItemPos( WORD& wOutPosX, WORD& wOutPosY )
{
    wOutPosX = m_sPreIventoryItem.wPosX;
    wOutPosY = m_sPreIventoryItem.wPosY;
}

void CItemRebuild::ResetPreInventoryItem()
{
    m_sPreIventoryItem.RESET();
}
