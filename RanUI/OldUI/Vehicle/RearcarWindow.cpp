#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DeviceMan.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImage.h"
#include "../Item/ItemMove.h"

#include "RearCarWindow.h"

#include "../../../SigmaCore/DebugInclude.h"

CRearcarWindow::CRearcarWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pVehicleInfo( NULL )
, m_pExplainText( NULL )
, m_pLearning( NULL )
, m_pBoostText( NULL )
, m_pVehicleName( NULL )
, m_pVehicleType( NULL )
, m_pBatteryText( NULL )
, m_pBatteryPBar( NULL )
, m_pMouseOver( NULL )
, m_pIconOver( NULL )
, m_nBatteryBack( -1 )
, m_pOver( NULL )
, m_pInfo( NULL )
{

}

CRearcarWindow::~CRearcarWindow()
{

}

void CRearcarWindow::CreateSubControl()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "VEHICLE_BG_LLINEBOX" );
	RegisterControl ( pBasicLineBox );

	pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "VEHICLE_BG_RLINEBOX" );
	RegisterControl ( pBasicLineBox );

	// 슬롯;
	{
	}

	// 탈것 이름;
	CBasicTextBox*	pTextBox(NULL);
	pTextBox = CreateTextBox( "VEHICLE_NAME_STATIC", pFont9 );
	pTextBox->AddText( ID2GAMEWORD("VEHICLE_TEXT_STATIC",0) );
	pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X );
	m_pVehicleName = CreateTextBox ( "VEHICLE_NAME_TEXTBOX", pFont9 );

	// 종류;
	pTextBox = CreateTextBox( "VEHICLE_TYPE_STATIC", pFont9 );
	pTextBox->AddText( ID2GAMEWORD("VEHICLE_TEXT_STATIC",1) );
	pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X );
	m_pVehicleType = CreateTextBox( "VEHICLE_TYPE_TEXTBOX", pFont9 );
	m_pVehicleParts = CreateTextBox( "VEHICLE_PARTS_TEXTBOX", pFont9 );

	// 연료;
	pTextBox = CreateTextBox ( "VEHICLE_BATTERY_STATIC", pFont9 );
	pTextBox->AddText ( ID2GAMEWORD("VEHICLE_TEXT_STATIC",2) );
	pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );

	m_pBatteryPBar = new CBasicProgressBar(m_pEngineDevice);
	m_pBatteryPBar->CreateSub ( this, "VEHICLE_BATTERY_PROGRESSBAR" );
	m_pBatteryPBar->CreateOverImage ( "VEHICLE_BATTERY_PROGRESSBAR_OVERIMAGE" );
	RegisterControl ( m_pBatteryPBar );

	m_pBatteryText = new CBasicTextBox(m_pEngineDevice);
	m_pBatteryText->CreateSub( this, "VEHICLE_BETTERY_TEXT", UI_FLAG_DEFAULT, VEHICLE_BATTERY_TEXT );
	m_pBatteryText->SetFont( pFont8 );
	m_pBatteryText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	m_pBatteryText->SetText("1");
	RegisterControl ( m_pBatteryText );

	// 슬롯;
	{
		CString strVehicleSlot[SLOT_PARTS_SIZE] = 
		{
			"SLOT_RICKSHAW_PARTS_FRAME",
			"SLOT_RICKSHAW_PARTS_SADDLE",
			"SLOT_RICKSHAW_PARTS_TIRE",
			"SLOT_RICKSHAW_PARTS_ROOF",
		};
		CString strVehicleSlotOver[SLOT_PARTS_SIZE] = 
		{
			"SLOT_RICKSHAW_PARTS_FRAME_OVER",
			"SLOT_RICKSHAW_PARTS_SADDLE_OVER",
			"SLOT_RICKSHAW_PARTS_TIRE_OVER",
			"SLOT_RICKSHAW_PARTS_ROOF_OVER",
		};

		int i;
		for ( i = 0; i < SLOT_PARTS_SIZE; ++i )
		{
			m_pItemDisplay[i] = CreateItemImage( strVehicleSlot[i].GetString(), SLOT_PARTS_FRAME + i );
			m_pSlotDisplay[i] = CreateControl( strVehicleSlotOver[i].GetString() );
			m_pSlotDisplay[i]->SetVisibleSingle( FALSE );
		}
		m_pMouseOver = CreateControl( "ITEM_MOUSE_OVER" );
		m_pMouseOver->SetVisibleSingle( FALSE );
	}

	// 배경이미지;
	m_pBackground = CreateControl( "RICKSHAW_UI_BACK_IMAGE" );
	m_pBackground->SetVisibleSingle( FALSE );

	// 부스트;
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox( m_pEngineDevice );
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "RICKSHAW_BOOST_OVER" );
		RegisterControl ( pBasicLineBox );

		m_pExplainText = CreateTextBox ( "RICKSHAW_BOOST_EXPLAIN", pFont9 );
		m_pExplainText->SetTextAlign ( TEXT_ALIGN_CENTER_Y );

		m_pLearning = CreateTextBox ( "RICKSHAW_EXPLAN", pFont9 );
		m_pLearning->AddText ( ID2GAMEINTEXT("BIKE_BOOST_EXPLAIN") );
		m_pLearning->SetTextAlign ( TEXT_ALIGN_CENTER_Y );

		m_pInfo = new CBasicVarTextBox(m_pEngineDevice);
		m_pInfo->CreateSub ( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pInfo->CreateSubControl ();
		m_pInfo->SetVisibleSingle ( FALSE );
		RegisterControl ( m_pInfo );

		m_pOver = CreateControl( "RICKSHAW_BOOST_OVER", UI_FLAG_DEFAULT, REARCAR_BOOST_OVER );

		CreateControl( "RICKSHAW_BOOST_ICON" );
		m_pIconOver = CreateControl( "RICKSHAW_BOOST_OVER" );

		m_pBoostText = CreateTextBox( "RICKSHAW_BOOST_TEXT", pFont9 );
		m_pBoostText->AddText( ID2GAMEINTEXT( "RICKSHAW_BOOST_FUNCTION" ) );
		m_pBoostText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	}
}

void CRearcarWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );
	if ( bVisible )
	{
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if ( !pChar )
			return;

		SITEMCUSTOM& sItemCustom = pChar->m_PutOnItems[SLOT_VEHICLE];
		if ( sItemCustom.GetNativeID() == NATIVEID_NULL() || !pChar->VehicleActiveValue())
			return;

		SNATIVEID sNativeID = pChar->VehicleId();
		const SITEM* pItem = GLogicData::GetInstance().GetItem( sNativeID );
		if ( !pItem )
			return;

		VEHICLE_TYPE emTYPE = pChar->VehicleType();

		if( emTYPE == VEHICLE_TYPE_NONE )
		{
			SetName( ID2GAMEWORD("VEHICLE_TEXT_STATIC",3) );
			SetType( ID2GAMEWORD("VEHICLE_TEXT_STATIC",3) );
		}
		else 
		{
			// 아래의 COMMENT는 데이터에서 읽어오지않는다;
			SetName( COMMENT::VEHICLE_TYPE[emTYPE] );
			SetType( pItem->GetName() );
		}
		m_pBackground->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pInterface->UiHideGroup( REARCAR_WINDOW );
	}
}

void CRearcarWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	// 활성화 여부 체크;
	if (!pChar->VehicleActiveValue())
	{
		m_pInterface->UiHideGroup( GetWndID() );
		return ;
	}

	if( m_pMouseOver ) 
		m_pMouseOver->SetVisibleSingle ( FALSE );

	// 스냅, 스킬 이미지;
	IItemMove*	pItemMove = m_pInterface->GetItemMove();
	SNATIVEID	sHoldItem(false);
	if ( pItemMove )
		sHoldItem = pItemMove->GetItem();

	// 슬롯 업데이트;
	int i;
	for ( i = 0; i < SLOT_PARTS_SIZE; ++i )
	{
		SITEMCUSTOM& sItemCustomOld = m_ItemCustom[i];
		const SITEMCUSTOM& sItemCustom = pChar->VehiclePutOnItemRef(VEHICLE_ACCESSORYTYPE(i+1));
		if ( sItemCustomOld != sItemCustom )
		{
			if ( sItemCustom.GetNativeID() != NATIVEID_NULL() )
			{
				LoadItem ( i, sItemCustom );
			}
			else
			{
				UnLoadItem ( i );
			}
		}
		m_pSlotDisplay[i]->SetVisibleSingle ( FALSE );
		if( sHoldItem != NATIVEID_NULL() )
		{
			// 장착될 위치 표시;
			if (pChar->VehicleCheckSlotItem(sHoldItem, IMAGE2EMSLOT(i+SLOT_PARTS_FRAME)))
			{
				const UIRECT& rcImagePos = m_pItemDisplay[i]->GetGlobalPos ();
				D3DXVECTOR2 vPos( rcImagePos.left, rcImagePos.top );

				m_pSlotDisplay[i]->SetVisibleSingle( TRUE );
			}
		}
	}

	CString strText;
	// 이동속도 변화량 체크 및 UI 출력;
	const SITEMCUSTOM sVehicleItem = pChar->GET_SLOT_ITEM( SLOT_VEHICLE );
	const SITEM* pItemData = GLogicData::GetInstance().GetItem( pChar->VehicleId() );
	if ( pItemData )
	{
		EMITEM_VAR emITEM_VOL = pItemData->sSuitOp.sVOLUME.emTYPE;
		//	이동속도 변화량;
		if ( emITEM_VOL != EMVAR_NONE)
		{
			if ( emITEM_VOL == EMVAR_MOVE_SPEED )
			{
				float fVOLUME = sVehicleItem.GETMOVESPEED();

				const float fAddOnSpeed = pChar->VehiclePutOnItemMoveSpeedTotal();
				if ( fAddOnSpeed != 0.0f )
					strText.Format("%s %.2f(+%.2f)", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ), fVOLUME, fAddOnSpeed);
				else 
					strText.Format("%s %.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ), fVOLUME );

				m_pVehicleParts->SetOneLineText( strText, NS_UITEXTCOLOR::PRIVATE );
			}
			else
			{
				float fVOLUME = pItemData->sSuitOp.sVOLUME.fVolume;
				strText.Format("%s %.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ), fVOLUME );
				m_pVehicleParts->SetOneLineText( strText, NS_UITEXTCOLOR::PRIVATE );
			}
		}
	}

	// 부스트 배움 여부에 따른 텍스트;
	strText.Format( "%s %s", ID2GAMEWORD("BIKE_BOOST"), ID2GAMEWORD("BIKE_BOOST_LEARNING", pChar->VehicleBoosterId() ? 1 : 0 ) );
	m_pExplainText->SetOneLineText( strText, NS_UITEXTCOLOR::PRIVATE );
	m_pIconOver->SetVisibleSingle( pChar->VehicleBoosterId() ? FALSE : TRUE );

	m_pInfo->SetVisibleSingle( FALSE );
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	SetBattery(pChar->VehicleFull());
}

void CRearcarWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case REARCAR_BOOST_OVER:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInterface->SHOW_COMMON_LINEINFO ( pChar->VehicleBoosterId() ? ID2GAMEINTEXT("BIKE_BOOST_EXPLAIN_OVER_ON") : ID2GAMEINTEXT("BIKE_BOOST_EXPLAIN_OVER_OFF"), NS_UITEXTCOLOR::SILVER );
			}
		}
		break;
	case SLOT_PARTS_FRAME:
	case SLOT_PARTS_SADDLE:	
	case SLOT_PARTS_TIRE:	
	case SLOT_PARTS_ROOF:	
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{	
				int nIndex = ControlID - SLOT_PARTS_FRAME;
				if (!m_pInterface->IsFirstItemSlot())
					return;

				IItemMove* pItemMove = m_pInterface->GetItemMove();
				if ( !pItemMove )
				{
					GASSERT ( 0 && "CItemMove가 널입니다." );
					return ;
				}

				if ( pItemMove->GetItem () != NATIVEID_NULL () )
				{
					const UIRECT& rcSlotPos = m_pItemDisplay[nIndex]->GetGlobalPos ();
					pItemMove->SetUIWindowGlobalPos ( rcSlotPos );						

					AddMessageEx ( UIMSG_USER1 | UIMSG_TOTOPPARENT );
				}				

				//	마우스 표시 테두리;
				if ( pItemMove->GetItem () == NATIVEID_NULL () && m_pItemDisplay[nIndex]->GetItemImageSlotPos () != NATIVEID_NULL () )
				{
					const UIRECT& rcImagePos = m_pItemDisplay[nIndex]->GetGlobalPos ();
					m_pMouseOver->SetGlobalPos ( rcImagePos );
					m_pMouseOver->SetVisibleSingle ( TRUE );
				}

				if ( dwMsg & UIMSG_LB_UP )
				{
					pChar->ReqVehicleChangeAccessory( IMAGE2EMSLOT(ControlID) );
				}

				if ( dwMsg & UIMSG_RB_UP )
				{
					pChar->ReqVehicleRemoveSlotItem( IMAGE2EMSLOT(ControlID) );
				}

				if( m_ItemCustom[nIndex].GetNativeID() != NATIVEID_NULL () )
				{
					m_pInterface->SHOW_ITEM_INFO(
						m_ItemCustom[nIndex],
						FALSE, FALSE, FALSE, FALSE, FALSE,
						USHRT_MAX, USHRT_MAX );
				}
			}
		}
		break;
	}
}

CBasicTextBox * CRearcarWindow::CreateTextBox( char* szKeyword, CD3DFontPar* pFont )
{
	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, szKeyword );
	pTextBox->SetFont ( pFont );
	RegisterControl ( pTextBox );

	return pTextBox;
}

CUIControl*	CRearcarWindow::CreateControl( const char* szControl, WORD wAlignFlag, UIGUID WndID )
{
	CUIControl*	pControl = new CUIControl( m_pEngineDevice );
	pControl->CreateSub( this, szControl, wAlignFlag, WndID );
	RegisterControl( pControl );
	return pControl;
}

CItemImage*	CRearcarWindow::CreateItemImage( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage( m_pGaeaClient, m_pEngineDevice );
	pItemImage->CreateSub( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl();
	RegisterControl( pItemImage );
	return pItemImage;
}

void CRearcarWindow::SetName( const std::string & strName )
{
	GASSERT( m_pVehicleName );
	m_pVehicleName->SetOneLineText( strName.c_str() );
}

void CRearcarWindow::SetType( const std::string & strType )
{
	GASSERT( m_pVehicleType );
	m_pVehicleType->SetOneLineText( strType.c_str() );
}

void CRearcarWindow::SetBattery( int nNOW )
{
	GASSERT( m_pBatteryText && m_pBatteryPBar );

	if ( m_nBatteryBack != nNOW )
	{
		m_nBatteryBack = nNOW;

		CString strTemp;
		strTemp.Format( "%.2f%%", nNOW/10.0f );

		m_pBatteryText->SetOneLineText( strTemp, NS_UITEXTCOLOR::WHITE );
		m_pBatteryPBar->SetPercent( nNOW/1000.0f ); //< 입력값은 0 과 1 사이의 값이다.
	}
}

void CRearcarWindow::LoadItem(int nIndex, const SITEMCUSTOM& ref_sItemCustom)
{
	m_ItemCustom[nIndex] = ref_sItemCustom;
	m_pItemDisplay[nIndex]->SetItem( ref_sItemCustom );
}

void CRearcarWindow::UnLoadItem(int nIndex)
{
	m_ItemCustom[nIndex] = SITEMCUSTOM( NATIVEID_NULL() );
	m_pItemDisplay[nIndex]->ResetItem ();
}

EMSUIT CRearcarWindow::IMAGE2EMSLOT ( UIGUID ControlID )
{
	switch ( ControlID )
	{	
	case SLOT_PARTS_FRAME:	return SUIT_VEHICLE_PARTS_A;
	case SLOT_PARTS_SADDLE: return SUIT_VEHICLE_PARTS_B;
	case SLOT_PARTS_TIRE:	return SUIT_VEHICLE_PARTS_C;
	case SLOT_PARTS_ROOF:	return SUIT_VEHICLE_PARTS_D;
	}

	GASSERT ( 0 && "RearcarWindow::IMAGE2EMSLOT(), 범위를 벗어납니다." );
	return SUIT_NSIZE;
}