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

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImage.h"
#include "../Item/ItemMove.h"
#include "BikeWindow.h"
//#include "GLCharter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBikeWindow::CBikeWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pVehicleName(NULL)
    , m_pVehicleType(NULL)
    , m_pMouseOver(NULL)
    , m_pBatteryText(NULL)
    , m_pBattery(NULL)
    , m_RollOverID(NO_ID)
    , m_nBatteryBack(-1)
    , m_dwMasterID( 0 )
    , m_pVehicleParts( NULL )
    , m_pIconOver( NULL )
    , m_pExplain( NULL )
    , m_pLearning( NULL )
    , m_pInfo( NULL )
    , m_pOver( NULL )
    , m_pBoostText( NULL )
{
	SecureZeroMemory( m_pSlotDisplay, sizeof(m_pSlotDisplay) );
	SecureZeroMemory( m_pSlotImage, sizeof(m_pSlotImage) );
}

CBikeWindow::~CBikeWindow ()
{
}

void CBikeWindow::CreateSubControl ()
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

	{
		CString strVehicleSlot[SLOT_IMAGE_SIZE] = 
		{
			"SLOT_BIKE_PARTS_IMAGE_TYPE0",
			"SLOT_BIKE_PARTS_IMAGE_TYPE1",
			"SLOT_BIKE_PARTS_IMAGE_TYPE2",
			"SLOT_BIKE_PARTS_IMAGE_TYPE3",
			"SLOT_BIKE_PARTS_IMAGE_TYPE4",
			"SLOT_BIKE_PARTS_IMAGE_TYPE5",
		};

		CString strVehicleSlotOver[SLOT_IMAGE_SIZE] = 
		{
			"SLOT_BIKE_PARTS_OVER_IMAGE_TYPE0",
			"SLOT_BIKE_PARTS_OVER_IMAGE_TYPE1",
			"SLOT_BIKE_PARTS_OVER_IMAGE_TYPE2",
			"SLOT_BIKE_PARTS_OVER_IMAGE_TYPE3",
			"SLOT_BIKE_PARTS_OVER_IMAGE_TYPE4",
			"SLOT_BIKE_PARTS_OVER_IMAGE_TYPE5",
		};


		for ( int i = 0; i < SLOT_IMAGE_SIZE; i++ )
		{
			m_pSlotImage[i] = CreateItemImage( strVehicleSlot[i].GetString(), SLOT_PARTS_IMAGE_TYPE0 + i );
			m_pSlotDisplay[i] = CreateControl( strVehicleSlotOver[i].GetString() );
			m_pSlotDisplay[i]->SetVisibleSingle( FALSE );
		}

		m_pMouseOver = CreateControl( "ITEM_MOUSE_OVER" );
		m_pMouseOver->SetVisibleSingle( FALSE );
	}

	{
		CBasicTextBox* pTextBox(NULL);

		// 펫 이름
		pTextBox = CreateTextBox ( "VEHICLE_NAME_STATIC", pFont9 );
		pTextBox->AddText ( ID2GAMEWORD("VEHICLE_TEXT_STATIC",0) );
		pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );

		m_pVehicleName = CreateTextBox ( "VEHICLE_NAME_TEXTBOX", pFont9 );

		// 펫 종류
		pTextBox = CreateTextBox ( "VEHICLE_TYPE_STATIC", pFont9 );
		pTextBox->AddText ( ID2GAMEWORD("VEHICLE_TEXT_STATIC",1) );
		pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );

		m_pVehicleType = CreateTextBox ( "VEHICLE_TYPE_TEXTBOX", pFont9 );
		
		m_pVehicleParts = CreateTextBox ( "VEHICLE_PARTS_TEXTBOX", pFont9 );

		// 포만
		pTextBox = CreateTextBox ( "VEHICLE_BATTERY_STATIC", pFont9 );
		pTextBox->AddText ( ID2GAMEWORD("VEHICLE_TEXT_STATIC",2) );
		pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );

		CBasicProgressBar* pBattery = new CBasicProgressBar(m_pEngineDevice);
		pBattery->CreateSub ( this, "VEHICLE_BATTERY_PROGRESSBAR" );
		pBattery->CreateOverImage ( "VEHICLE_BATTERY_PROGRESSBAR_OVERIMAGE" );
		RegisterControl ( pBattery );
		m_pBattery = pBattery;

		CBasicTextBox* pBatteryText = new CBasicTextBox(m_pEngineDevice);
		pBatteryText->CreateSub( this, "VEHICLE_BETTERY_TEXT", UI_FLAG_DEFAULT, VEHICLE_BATTERY_TEXT );
		pBatteryText->SetFont( pFont8 );
		pBatteryText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
		pBatteryText->SetText("1");
		RegisterControl ( pBatteryText );
		m_pBatteryText = pBatteryText;	
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "BOOST_OVER" );
		RegisterControl ( pBasicLineBox );

		CreateControl( "BOOST_ICON" );
		m_pIconOver = CreateControl( "BOOST_ICON_OVER" );

		m_pExplain = CreateTextBox ( "BOOST_EXPLAIN", pFont9 );
		m_pExplain->SetTextAlign ( TEXT_ALIGN_CENTER_Y );
		
		m_pLearning = CreateTextBox ( "BOOST_LEARNING", pFont9 );
		m_pLearning->AddText ( ID2GAMEINTEXT("BIKE_BOOST_EXPLAIN") );
		m_pLearning->SetTextAlign ( TEXT_ALIGN_CENTER_Y );

		CBasicVarTextBox* pInfo = new CBasicVarTextBox(m_pEngineDevice);
		pInfo->CreateSub ( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pInfo->CreateSubControl ();
		pInfo->SetVisibleSingle ( FALSE );
		RegisterControl ( pInfo );
		m_pInfo = pInfo;

		m_pOver = CreateControl( "BOOST_OVER", UI_FLAG_DEFAULT, BIKE_BOOST_OVER );

		m_pBoostText = CreateTextBox ( "BOOST_TEXT", pFont9 );
		m_pBoostText->AddText ( ID2GAMEINTEXT("BIKE_BOOST_FUNCTION") );
		m_pBoostText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	}
}

CBasicTextBox*	CBikeWindow::CreateTextBox ( char* szKeyword, CD3DFontPar* pFont )
{
	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, szKeyword );
	pTextBox->SetFont ( pFont );
	RegisterControl ( pTextBox );

	return pTextBox;
}

CUIControl*	CBikeWindow::CreateControl ( const char* szControl, WORD wAlignFlag, UIGUID WndID )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl, wAlignFlag, WndID );
	RegisterControl ( pControl );

	return pControl;
}

CItemImage*	CBikeWindow::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage ( m_pGaeaClient, m_pEngineDevice );
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}

EMSUIT CBikeWindow::IMAGE2EMSLOT ( UIGUID ControlID )
{
	switch ( ControlID )
	{	
	case SLOT_PARTS_IMAGE_TYPE0: return SUIT_VEHICLE_PARTS_A;
	case SLOT_PARTS_IMAGE_TYPE1: return SUIT_VEHICLE_PARTS_B;
	case SLOT_PARTS_IMAGE_TYPE2: return SUIT_VEHICLE_PARTS_C;
	case SLOT_PARTS_IMAGE_TYPE3: return SUIT_VEHICLE_PARTS_D;
	case SLOT_PARTS_IMAGE_TYPE4: return SUIT_VEHICLE_PARTS_E;
	case SLOT_PARTS_IMAGE_TYPE5: return SUIT_VEHICLE_PARTS_F;
	}

	GASSERT ( 0 && "CVehicleWindow::IMAGE2EMSLOT(), 범위를 벗어납니다." );
	return SUIT_NSIZE;
}

void CBikeWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	// 활성화 여부 체크
	if (!pChar->VehicleActiveValue())
	{
		m_pInterface->UiHideGroup( GetWndID() );
		return ;
	}

	if( m_pMouseOver ) m_pMouseOver->SetVisibleSingle ( FALSE );

	//	스냅, 스킬 이미지 붙이기
	IItemMove* pItemMove = m_pInterface->GetItemMove();
	SNATIVEID sHOLD_ITEM_ID(false);
	if (pItemMove)
        sHOLD_ITEM_ID = pItemMove->GetItem();

	for (int i = 0; i < SLOT_IMAGE_SIZE; ++i)
	{
		SITEMCUSTOM& sItemCustomOld = m_ItemCustomArray[i];
		const SITEMCUSTOM& sItemCustom = pChar->VehiclePutOnItemRef(VEHICLE_ACCESSORYTYPE(i+1));

		if (sItemCustomOld != sItemCustom)
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

		if( sHOLD_ITEM_ID != NATIVEID_NULL() )
		{
			if (pChar->VehicleCheckSlotItem(sHOLD_ITEM_ID, IMAGE2EMSLOT(i+SLOT_PARTS_IMAGE_TYPE0)))
			{
				const UIRECT& rcImagePos = m_pSlotImage[i]->GetGlobalPos ();
				D3DXVECTOR2 vPos( rcImagePos.left, rcImagePos.top );

				m_pSlotDisplay[i]->SetVisibleSingle( TRUE );
			}
		}
	}

	float fAddOnSpeed = pChar->VehiclePutOnItemMoveSpeedTotal();

	CString strText;
	const SITEMCUSTOM sVehicleItem = pChar->GET_SLOT_ITEM( SLOT_VEHICLE );
	const SITEM* pItemData = GLogicData::GetInstance().GetItem(pChar->VehicleId());
	if (pItemData)
	{
		EMITEM_VAR emITEM_VOL = pItemData->sSuitOp.sVOLUME.emTYPE;
		//	
		//
		if ( emITEM_VOL != EMVAR_NONE)
		{
			if ( emITEM_VOL == EMVAR_MOVE_SPEED )
			{
				float fVOLUME = sVehicleItem.GETMOVESPEED();

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

	if (pChar->VehicleBoosterId())
	{
		strText.Format("%s %s", ID2GAMEWORD("BIKE_BOOST"), ID2GAMEWORD("BIKE_BOOST_LEARNING",1) );
		m_pExplain->SetOneLineText( strText, NS_UITEXTCOLOR::PRIVATE );

		m_pIconOver->SetVisibleSingle( FALSE );

	}
	else
	{
		strText.Format("%s %s", ID2GAMEWORD("BIKE_BOOST"), ID2GAMEWORD("BIKE_BOOST_LEARNING",0) );
		m_pExplain->SetOneLineText( strText, NS_UITEXTCOLOR::PRIVATE );

		m_pIconOver->SetVisibleSingle( TRUE );
	}
	
	m_pInfo->SetVisibleSingle( FALSE );

	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	SetBattery(pChar->VehicleFull());
}

void CBikeWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( VEHICLE_WINDOW );
			}
		}
		break;
	case SLOT_PARTS_IMAGE_TYPE0:
	case SLOT_PARTS_IMAGE_TYPE1:
	case SLOT_PARTS_IMAGE_TYPE2:
	case SLOT_PARTS_IMAGE_TYPE3:
	case SLOT_PARTS_IMAGE_TYPE4:
	case SLOT_PARTS_IMAGE_TYPE5:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{	
				int nIndex = ControlID - SLOT_PARTS_IMAGE_TYPE0;
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
					const UIRECT& rcSlotPos = m_pSlotImage[nIndex]->GetGlobalPos ();
					pItemMove->SetUIWindowGlobalPos ( rcSlotPos );						

					AddMessageEx ( UIMSG_MOUSEIN_BIKE_WEARSLOT | UIMSG_TOTOPPARENT );
				}				

				//	마우스 표시 테두리
				if ( pItemMove->GetItem () == NATIVEID_NULL () && m_pSlotImage[nIndex]->GetItemImageSlotPos () != NATIVEID_NULL () )
				{
					const UIRECT& rcImagePos = m_pSlotImage[nIndex]->GetGlobalPos ();
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

				if( m_ItemCustomArray[nIndex].GetNativeID() != NATIVEID_NULL () )
				{
					m_pInterface->SHOW_ITEM_INFO(
						m_ItemCustomArray[nIndex],
						FALSE, FALSE, FALSE, FALSE, FALSE,
						USHRT_MAX, USHRT_MAX );
				}
			}
		}
		break;

	case BIKE_BOOST_OVER:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if (pChar->VehicleBoosterId())
				{
					m_pInterface->SHOW_COMMON_LINEINFO ( ID2GAMEINTEXT("BIKE_BOOST_EXPLAIN_OVER_ON"), NS_UITEXTCOLOR::SILVER );
				}
				else
				{
					m_pInterface->SHOW_COMMON_LINEINFO ( ID2GAMEINTEXT("BIKE_BOOST_EXPLAIN_OVER_OFF"), NS_UITEXTCOLOR::SILVER );
				}
			}
		}
		break;
	}
}

void CBikeWindow::SetName( const std::string & strName )
{
	GASSERT( m_pVehicleName );
	m_pVehicleName->SetOneLineText( strName.c_str() );
}

void CBikeWindow::SetType( const std::string & strType )
{
	GASSERT( m_pVehicleType );
	m_pVehicleType->SetOneLineText( strType.c_str() );
}


void CBikeWindow::SetBattery( int nNOW )
{
	GASSERT( m_pBatteryText && m_pBattery );

	if ( m_nBatteryBack != nNOW )
	{
		m_nBatteryBack = nNOW;

		CString strTemp;
		strTemp.Format( "%.2f%%", nNOW/10.0f );

		m_pBatteryText->SetOneLineText( strTemp, NS_UITEXTCOLOR::WHITE );
		m_pBattery->SetPercent( nNOW/1000.0f ); //< 입력값은 0 과 1 사이의 값이다.
	}
}

void CBikeWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if ( !pChar )
            return;
		SITEMCUSTOM& sItemCustom = pChar->m_PutOnItems[SLOT_VEHICLE];
		if ( sItemCustom.GetNativeID() == NATIVEID_NULL() )
            return;
		if (!pChar->VehicleActiveValue())
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
	}
	else
	{
		m_pInterface->UiHideGroup( VEHICLE_WINDOW );
	}
}

void CBikeWindow::LoadItem ( int nIndex, const SITEMCUSTOM& ref_sItemCustom )
{
	m_ItemCustomArray[nIndex] = ref_sItemCustom;
    m_pSlotImage[nIndex]->SetItem( ref_sItemCustom );
}

void CBikeWindow::UnLoadItem ( int nIndex )
{
	m_ItemCustomArray[nIndex] = SITEMCUSTOM( NATIVEID_NULL() );
	m_pSlotImage[nIndex]->ResetItem ();
}
