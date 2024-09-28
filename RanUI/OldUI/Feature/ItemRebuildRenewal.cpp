#include "StdAfx.h"
#include "ItemRebuildRenewal.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLogicData.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../Display/ItemInfoRebuildToolTip.h"
#include "../Item/ItemImage.h"
#include "../Item/ItemMove.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const char* ID2GAMEINTEXT_ITEM_REBUILD_ITEM_SLOT_TOOLTIP = "개조할 아이템을 장착하는 슬롯 입니다";
	const char* ID2GAMEINTEXT_ITEM_REBUILD_STAMP_SLOT_TOOLTIP = "인장카드를 장착하는 슬롯 입니다";
}

//--------------------------------------------------------------------
// ItemRebuild
//--------------------------------------------------------------------
ItemRebuildRenewal::ItemRebuildRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )	// ITEMREBUILD_MARK
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

ItemRebuildRenewal::~ItemRebuildRenewal()
{
}

void ItemRebuildRenewal::CreateUIWindowAndRegisterOwnership(void)
{
	Create( ITEM_REBUILD_RENEWAL_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CreateBaseWindowLightGray( "ITEM_REBUILD_RENEWAL_WINDOW", NULL );

	CreateLineBox( "ITEM_REBUILD_RENEWAL_HELP_REGION", "PARTY_INFO_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "ITEM_REBUILD_RENEWAL_OPTION_REGION_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO" );

	// 아이템 슬롯
	{
		m_pItemImage = new CItemImage ( m_pGaeaClient, m_pEngineDevice );
		m_pItemImage->CreateSub( this, "ITEM_REBUILD_ITEM_SLOT", UI_FLAG_DEFAULT, ITEM_REBUILD_ITEM_SLOT );
		m_pItemImage->CreateSubControl();
		RegisterControl( m_pItemImage );
	}
	{
		m_pStampImage = new CItemImage ( m_pGaeaClient, m_pEngineDevice );
		m_pStampImage->CreateSub( this, "ITEM_STAMP_ITEM_SLOT", UI_FLAG_DEFAULT, ITEM_STAMP_ITEM_SLOT );
		m_pStampImage->CreateSubControl();
		RegisterControl( m_pStampImage );
	}
	
	// 슬롯 툴팁;
	{
		ID2GAMEINTEXT_ITEM_REBUILD_ITEM_SLOT_TOOLTIP = ID2GAMEINTEXT("ITEM_REBUILD_ITEM_SLOT_TOOLTIP");
		ID2GAMEINTEXT_ITEM_REBUILD_STAMP_SLOT_TOOLTIP = ID2GAMEINTEXT("ITEM_REBUILD_STAMP_SLOT_TOOLTIP");
	}

	// 코스튬 컬러 정보. by luxes.
	{
		m_pColorInfo = new CCostumColorInfo(m_pEngineDevice);
		m_pColorInfo->CreateSub( this, "COLOR_REBUILD_BOX", UI_FLAG_DEFAULT, ITEM_COLOR_INFO );
		m_pColorInfo->CreateSubControl();
		RegisterControl( m_pColorInfo );
		m_pColorInfo->SetVisibleSingle( FALSE );
	}

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9 );
	// 개조비용 : 0
	{		
		m_pCostTextBoxStatic = CreateStaticControl( "ITEM_REBUILD_COAST_TEXT_STATIC", pFont, NS_UITEXTCOLOR::SILVER, TEXT_ALIGN_LEFT );
		m_pCostTextBoxStatic->AddText( ID2GAMEINTEXT( "ITEM_REBUILD_COST" ), NS_UITEXTCOLOR::SILVER );

		m_pCostTextBox = CreateStaticControl( "ITEM_REBUILD_COAST_TEXT", pFont, NS_UITEXTCOLOR::SILVER, TEXT_ALIGN_LEFT );
	}

	// 설명서;
	{
		m_pGuideTextBox = CreateStaticControl( "ITEM_REBUILD_HELP_TEXT_BOX", pFont, NS_UITEXTCOLOR::SILVER, TEXT_ALIGN_LEFT );
		m_pGuideTextBox->SetVisibleSingle(TRUE);
	}

	// 내야되는돈?
	{
		CUIControl* pControl = CreateControl( "ITEM_REBUILD_RENEWAL_MONEY_BACK" );
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
		m_pOkButton->CreateBaseButton( "ITEM_REBUILD_RENEWAL_OK_BUTTON", CBasicTextButton::SIZE14,
			CBasicButton::CLICK_FLIP, ID2GAMEINTEXT( "ITEM_REBUILD_OK" ) );
		m_pOkButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		m_pOkButton->SetVisibleSingle( FALSE );
		RegisterControl( m_pOkButton );

		CBasicTextButton * pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_DEFAULT, ITEM_REBUILD_CANCEL_BUTTON );
		pButton->CreateBaseButton( "ITEM_REBUILD_RENEWAL_CANCEL_BUTTON", CBasicTextButton::SIZE14,
			CBasicButton::CLICK_FLIP, ID2GAMEINTEXT( "ITEM_REBUILD_CANCEL" ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}

	m_pItemOption = new ItemInfoRebuildToolTip( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pItemOption->CreateSub(this, "ITEM_REBUILD_RENEWAL_OPTION_REGION", UI_FLAG_DEFAULT);
	m_pItemOption->CreateSubControl();
	m_pItemOption->SetVisibleSingle( FALSE );	
	RegisterControl( m_pItemOption );





	ResetAlignFlag();
	SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
	SetTitleName( ID2GAMEINTEXT("ITEMREBUILD_WINDOW_TITLE") );
	m_pInterface->UiRegisterControl( this, true );
	m_pInterface->UiRegisterNpcActionableControl( this );
	m_pInterface->UiShowGroupFocus( ITEM_REBUILD_RENEWAL_WINDOW );
}

void ItemRebuildRenewal::SetVisibleSingle(BOOL bVisible)
{
	CUIWindowEx::SetVisibleSingle(bVisible);

	if ( bVisible == FALSE )
	{		
		m_pGaeaClient->GetCharacter()->ReqRebuildClose();
	}
}
const UIRECT& ItemRebuildRenewal::GetGlobalPos(void)
{
	return CUIWindowEx::GetGlobalPos();
}
void ItemRebuildRenewal::SetGlobalPos(const D3DXVECTOR2& vPosition)
{
	CUIWindowEx::SetGlobalPos(vPosition);
}
void ItemRebuildRenewal::SetNpc(const SNATIVEID& sNPCID, const DWORD dwNPCGlobalID)
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

void ItemRebuildRenewal::SetPreInventoryItem( WORD wPosX, WORD wPosY )
{
	m_sPreIventoryItem.SET( wPosX, wPosY );
}

void ItemRebuildRenewal::GetPreInventoryItemPos( WORD& wOutPosX, WORD& wOutPosY )
{
	wOutPosX = m_sPreIventoryItem.wPosX;
	wOutPosY = m_sPreIventoryItem.wPosY;
}

void ItemRebuildRenewal::ResetPreInventoryItem()
{
	m_sPreIventoryItem.RESET();
}

void ItemRebuildRenewal::SetPreStampItem( WORD wPosX, WORD wPosY )
{
	m_sPreStampItem.SET( wPosX, wPosY );
}

void ItemRebuildRenewal::GetPreStampItemPos( WORD& wOutPosX, WORD& wOutPosY )
{
	wOutPosX = m_sPreStampItem.wPosX;
	wOutPosY = m_sPreStampItem.wPosY;
}

void ItemRebuildRenewal::ResetPreStampItem()
{
	m_sPreStampItem.RESET();
}

void ItemRebuildRenewal::SetItemRebuildResetCheckBox(void)
{
	m_pItemOption->resetCheckBox();
}
void ItemRebuildRenewal::SetItemRebuildSetCheckBox(const SITEMCUSTOM& sItemCustom, const RandomOpt::FixOption& fixOption)
{	
	m_pItemOption->setCheckBox(sItemCustom, fixOption);
}


VOID ItemRebuildRenewal::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
	if ( !CUIControlNpcActionable::IsUseable(m_pGaeaClient) )
	{
		m_pInterface->SetItemRebuildWindowClose();
		return;
	}

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

	bool bSetRebuildItem(false);
	bool bSetStampItem(false);
	DWORD nCheckStampOption(0);


	GLCharacter* const pCharacter(m_pGaeaClient->GetCharacter());
	const SITEMCUSTOM& sRebuildItem = pCharacter->GET_REBUILD_ITEM();
	const SITEMCUSTOM& sStampItem = pCharacter->GET_STAMP_ITEM();
	if( m_pItemImage )
	{
		if( sRebuildItem.GetNativeID() == NATIVEID_NULL() )
		{
			m_pItemImage->ResetItem();
			m_pItemOption->SetVisibleSingle( FALSE );
		}
		else
		{
			const SITEM* pItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );

			// 개조카드 사용 불능. 이 아닌경우 개조 가능
			if( pItem && pItem->sRandomOpt.bUnableRemodelCard == false )
			{
				bSetRebuildItem = true;
				if ( m_pItemImage->SetItem( sRebuildItem ) == true )
					m_pItemOption->resetCheckBox();
				m_pItemOption->setItemInformation(sRebuildItem);
				m_pItemOption->SetVisibleSingle( TRUE );
			}
			else
				m_pItemImage->ResetItem();
		}
	}

	const SITEM* const pItemStamp(GLogicData::GetInstance().GetItem( sStampItem.GetNativeID()));
	if ( m_pStampImage )
	{
		if( sStampItem.GetNativeID() == NATIVEID_NULL() )
		{
			pCharacter->RESET_FIX_OPTION();
			m_pStampImage->ResetItem();
			m_pItemOption->setVisibleCheckBox(false);
		}
		else
		{			
			if( pItemStamp )
			{				
				bSetStampItem = true;
				nCheckStampOption = pCharacter->GetNFixOption();
				m_pStampImage->SetItem( sStampItem );
				m_pItemOption->setVisibleCheckBox(true, pItemStamp->sSuitOp.wReModelNum);
			}
			else
			{
				pCharacter->RESET_FIX_OPTION();
				m_pStampImage->ResetItem();
				m_pItemOption->setVisibleCheckBox(false);
			}
		}
	}

	m_pGuideTextBox->ClearText();
	CString strGuideText;

	if ( bSetRebuildItem == true )
	{
		if ( (bSetStampItem == true) && (pItemStamp != NULL) )
		{
			if ( nCheckStampOption == pItemStamp->sSuitOp.wReModelNum )
			{
				strGuideText.Format((char*)ID2GAMEINTEXT("ITEM_REBUILD_USE_ITEM_WITH_STAMP_CARD"));
			}
			else
			{
				strGuideText.Format(
					( char* ) ID2GAMEINTEXT( "ITEM_REBUILD_USE_ONLY_STAMP_CARD_NO_LIMIT_NEW" ), 
					pItemStamp->sSuitOp.wReModelNum, 
					pItemStamp->sSuitOp.wReModelNum, 
					pItemStamp->GetName() );
			}
		}
		else
		{
			strGuideText.Format((char*)ID2GAMEINTEXT("ITEM_REBUILD_USE_ITEM"));	
		}
	}
	else
	{
		strGuideText.Format((char*)ID2GAMEINTEXT("ITEM_REBUILD_USE_REBUILD"));	
	}
	m_pGuideTextBox->AddText(strGuideText);

	LONGLONG i64RebuildCost = m_pGaeaClient->GetCharacter()->GetRebuildInput();

	if( i64RebuildCost > 0 || sRebuildItem.GetNativeID() != NATIVEID_NULL() ) 
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
	if ( sRebuildItem.GetNativeID() != NATIVEID_NULL () )
	{
		if ( sRebuildItem.dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
			&& sRebuildItem.dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
		{
			m_pColorInfo->SetVisibleSingle( TRUE );

			m_pColorInfo->SetSelectColor( sRebuildItem.dwMainColor, sRebuildItem.dwSubColor );
		}
		else
		{
			m_pColorInfo->SetVisibleSingle( FALSE );
		}
	}
	else
		m_pColorInfo->SetVisibleSingle( FALSE );	
}

VOID ItemRebuildRenewal::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage(ControlID, dwMsg);
	switch( ControlID )
	{
	case ITEM_REBUILD_ITEM_SLOT:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			IItemMove* pItemMove = m_pInterface->GetItemMove();
			if( !pItemMove )
			{
				GASSERT( 0 && "ItemRebuildRenewal::TranslateUIMessage, pItemMove == NULL" );
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
				m_pGaeaClient->GetCharacter()->ReqRebuildMoveItem(false);

			SITEMCUSTOM sItemCustom = m_pGaeaClient->GetCharacter()->GET_REBUILD_ITEM();
			if( sItemCustom.GetNativeID() != NATIVEID_NULL() )
			{
				m_pInterface->SHOW_ITEM_INFO(
					sItemCustom,
					FALSE, FALSE, FALSE, TRUE, FALSE,
					0, 0 );
			}
			// 아이템이없다면 툴팁을 보여줌;
			else
			{
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEINTEXT_ITEM_REBUILD_ITEM_SLOT_TOOLTIP, NS_UITEXTCOLOR::LIGHTSKYBLUE );
			}
		}
		break;
	case ITEM_STAMP_ITEM_SLOT:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			IItemMove* pItemMove = m_pInterface->GetItemMove();
			if( !pItemMove )
			{
				GASSERT( 0 && "ItemRebuildRenewal::TranslateUIMessage, pItemMove == NULL" );
				break;
			}

			SNATIVEID sItem = pItemMove->GetItem();
			if( sItem != NATIVEID_NULL() )
			{
				const UIRECT& rcSlotPos = m_pStampImage->GetGlobalPos();
				pItemMove->SetUIWindowGlobalPos( rcSlotPos );
				m_pInterface->SetSnapItem();
			}

			if( dwMsg & UIMSG_LB_UP )
				m_pGaeaClient->GetCharacter()->ReqRebuildMoveStamp();

			SITEMCUSTOM sItemCustom = m_pGaeaClient->GetCharacter()->GET_STAMP_ITEM();
			if( sItemCustom.GetNativeID() != NATIVEID_NULL() )
			{
				m_pInterface->SHOW_ITEM_INFO(
					sItemCustom,
					FALSE, FALSE, FALSE, TRUE, FALSE,
					0, 0 );
			}
			// 아이템이없다면 툴팁을 보여줌;
			else
			{
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEINTEXT_ITEM_REBUILD_STAMP_SLOT_TOOLTIP, NS_UITEXTCOLOR::LIGHTSKYBLUE );
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

