#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../Item/ItemImage.h"
#include "../Item/ItemMove.h"
#include "../../InnerInterface.h"
#include "VehicleWindow.h"
//#include "GLCharter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CVehicleWindow::CVehicleWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
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
{
	SecureZeroMemory( m_pSlotDisplay, sizeof(m_pSlotDisplay) );
	SecureZeroMemory( m_pSlotImage, sizeof(m_pSlotImage) );
	SecureZeroMemory( m_pSlotBack, sizeof( m_pSlotBack ) );
}

CVehicleWindow::~CVehicleWindow()
{
}

void CVehicleWindow::CreateSubControl()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );


	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList( "VEHICLE_BG_LLINEBOX" );
	RegisterControl( pBasicLineBox );
	
	pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList( "VEHICLE_BG_RLINEBOX" );
	RegisterControl( pBasicLineBox );

	{
		CString strVehicleSlot[SLOT_IMAGE_SIZE] = 
		{
			"SLOT_SKIN_IMAGE_TYPE0",
			"SLOT_PARTS_IMAGE_TYPE0",
			"SLOT_PARTS_IMAGE_TYPE1",
			"SLOT_PARTS_IMAGE_TYPE2",
		};

		CString strVehicleSlotOver[SLOT_IMAGE_SIZE] = 
		{
			"SLOT_SKIN_OVER_IMAGE_TYPE0",
			"SLOT_PARTS_OVER_IMAGE_TYPE0",
			"SLOT_PARTS_OVER_IMAGE_TYPE1",
			"SLOT_PARTS_OVER_IMAGE_TYPE2",
		};


		CString strBack;
		for ( int i = 0; i < VEHICLE_TYPE_SIZE - 1; ++i )
		{
			strBack.Format( "%s%d","SLOT_BACK_IMAGE",i);
			m_pSlotBack[i] = CreateControl( strBack );
			m_pSlotBack[i]->SetVisibleSingle( FALSE );
		}
			

		for ( int i = 0; i < SLOT_IMAGE_SIZE; i++ )
		{
			m_pSlotImage[i] = CreateItemImage( strVehicleSlot[i].GetString(), SLOT_SKIN_IMAGE_TYPE0 + i );
			m_pSlotDisplay[i] = CreateControl( strVehicleSlotOver[i].GetString() );
			m_pSlotDisplay[i]->SetVisibleSingle( FALSE );
		}

		m_pMouseOver = CreateControl( "ITEM_MOUSE_OVER" );
		m_pMouseOver->SetVisibleSingle( FALSE );
	}

	{
		CBasicTextBox* pTextBox(NULL);

		// 펫 이름
		pTextBox = CreateTextBox( "VEHICLE_NAME_STATIC", pFont9 );
		pTextBox->AddText( ID2GAMEWORD("VEHICLE_TEXT_STATIC",0) );
		pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X );

		m_pVehicleName = CreateTextBox( "VEHICLE_NAME_TEXTBOX", pFont9 );
	
		// 펫 종류
		pTextBox = CreateTextBox( "VEHICLE_TYPE_STATIC", pFont9 );
		pTextBox->AddText( ID2GAMEWORD("VEHICLE_TEXT_STATIC",1) );
		pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X );

		m_pVehicleType = CreateTextBox( "VEHICLE_TYPE_TEXTBOX", pFont9 );

		// 포만
		pTextBox = CreateTextBox( "VEHICLE_BATTERY_STATIC", pFont9 );
		pTextBox->AddText( ID2GAMEWORD("VEHICLE_TEXT_STATIC",2) );
		pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X );

		CBasicProgressBar* pBattery = new CBasicProgressBar(m_pEngineDevice);
		pBattery->CreateSub( this, "VEHICLE_BATTERY_PROGRESSBAR" );
		pBattery->CreateOverImage( "VEHICLE_BATTERY_PROGRESSBAR_OVERIMAGE" );
		RegisterControl( pBattery );
		m_pBattery = pBattery;

		CBasicTextBox* pBatteryText = new CBasicTextBox(m_pEngineDevice);
		pBatteryText->CreateSub( this, "VEHICLE_BETTERY_TEXT", UI_FLAG_DEFAULT, VEHICLE_BATTERY_TEXT );
		pBatteryText->SetFont( pFont8 );
		pBatteryText->SetTextAlign( TEXT_ALIGN_CENTER_X );
		pBatteryText->SetText("1");
		RegisterControl( pBatteryText );
		m_pBatteryText = pBatteryText;	
	}
}

CBasicTextBox*	CVehicleWindow::CreateTextBox( char* szKeyword, CD3DFontPar* pFont )
{
	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub( this, szKeyword );
	pTextBox->SetFont( pFont );
	RegisterControl( pTextBox );

	return pTextBox;
}

CUIControl*	CVehicleWindow::CreateControl( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub( this, szControl );
	RegisterControl( pControl );

	return pControl;
}

CItemImage*	CVehicleWindow::CreateItemImage( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage( m_pGaeaClient, m_pEngineDevice ) ;
	pItemImage->CreateSub( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl();
	RegisterControl( pItemImage );

	return pItemImage;
}

EMSUIT CVehicleWindow::IMAGE2EMSLOT( UIGUID ControlID )
{
	switch ( ControlID )
	{	
	case SLOT_SKIN_IMAGE_TYPE0: return SUIT_VEHICLE_SKIN;
	case SLOT_PARTS_IMAGE_TYPE0: return SUIT_VEHICLE_PARTS_A;
	case SLOT_PARTS_IMAGE_TYPE1: return SUIT_VEHICLE_PARTS_B;
	case SLOT_PARTS_IMAGE_TYPE2: return SUIT_VEHICLE_PARTS_C;
	}

	GASSERT ( 0 && "CVehicleWindow::IMAGE2EMSLOT(), 범위를 벗어납니다." );
	return SUIT_NSIZE;
}

void CVehicleWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	// 활성화 여부 체크
	if (!pChar->VehicleActiveValue())
	{
		m_pInterface->UiHideGroup( GetWndID() );
		return ;
	}

	if (m_pMouseOver)
        m_pMouseOver->SetVisibleSingle( FALSE );

	//	스냅, 스킬 이미지 붙이기
	IItemMove* pItemMove = m_pInterface->GetItemMove();
	SNATIVEID sHOLD_ITEM_ID(false);
	if (pItemMove)
        sHOLD_ITEM_ID = pItemMove->GetItem();

	for (int i=0; i<SLOT_IMAGE_SIZE; ++i )
	{
		SITEMCUSTOM& sItemCustomOld = m_ItemCustomArray[i];
		const SITEMCUSTOM& sItemCustom = pChar->VehiclePutOnItemRef(VEHICLE_ACCESSORYTYPE(i));

		if (sItemCustomOld != sItemCustom)
		{		
			if ( sItemCustom.GetNativeID() != NATIVEID_NULL() )
			{
				LoadItem(i, sItemCustom);
			}
			else
			{
				UnLoadItem( i );
			}
		}

		m_pSlotDisplay[i]->SetVisibleSingle( FALSE );

		if( sHOLD_ITEM_ID != NATIVEID_NULL() )
		{
			// 장착될 위치 표시
			if (pChar->VehicleCheckSlotItem(sHOLD_ITEM_ID, IMAGE2EMSLOT(i + SLOT_SKIN_IMAGE_TYPE0)))
			{
				const UIRECT& rcImagePos = m_pSlotImage[i]->GetGlobalPos();
				D3DXVECTOR2 vPos( rcImagePos.left, rcImagePos.top );

				m_pSlotDisplay[i]->SetVisibleSingle( TRUE );
			}
		}
	}

	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	
	SetBattery(pChar->VehicleFull());
}

void CVehicleWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( VEHICLE_WINDOW );
			}
		}
		break;
	case SLOT_SKIN_IMAGE_TYPE0:
	case SLOT_PARTS_IMAGE_TYPE0:
	case SLOT_PARTS_IMAGE_TYPE1:
	case SLOT_PARTS_IMAGE_TYPE2:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{	
				int nIndex = ControlID - SLOT_SKIN_IMAGE_TYPE0;

				if ( !m_pInterface->IsFirstItemSlot() ) return ;

				IItemMove* pItemMove = m_pInterface->GetItemMove();
				if ( !pItemMove )
				{
					GASSERT ( 0 && "CItemMove가 널입니다." );
					return ;
				}

				if ( pItemMove->GetItem() != NATIVEID_NULL() )
				{
					const UIRECT& rcSlotPos = m_pSlotImage[nIndex]->GetGlobalPos();
					pItemMove->SetUIWindowGlobalPos( rcSlotPos );						

					AddMessageEx( UIMSG_MOUSEIN_VEHICLE_WEARSLOT | UIMSG_TOTOPPARENT );
				}				

				//	마우스 표시 테두리
				if ( pItemMove->GetItem() == NATIVEID_NULL() && m_pSlotImage[nIndex]->GetItemImageSlotPos() != NATIVEID_NULL() )
				{
					const UIRECT& rcImagePos = m_pSlotImage[nIndex]->GetGlobalPos();
					m_pMouseOver->SetGlobalPos( rcImagePos );
					m_pMouseOver->SetVisibleSingle( TRUE );
				}

				if ( dwMsg & UIMSG_LB_UP )
				{
					m_pGaeaClient->GetCharacter()->ReqVehicleChangeAccessory( IMAGE2EMSLOT(ControlID) );
				}

				if ( dwMsg & UIMSG_RB_UP )
				{
					m_pGaeaClient->GetCharacter()->ReqVehicleRemoveSlotItem( IMAGE2EMSLOT(ControlID) );
				}

				if( m_ItemCustomArray[nIndex].GetNativeID() != NATIVEID_NULL() )
				{
					m_pInterface->SHOW_ITEM_INFO(
						m_ItemCustomArray[nIndex],
						FALSE, FALSE, FALSE, FALSE, FALSE,
						USHRT_MAX, USHRT_MAX );
				}
			}
		}
		break;
	}
}

void CVehicleWindow::SetName( const std::string & strName )
{
	GASSERT( m_pVehicleName );
	m_pVehicleName->SetOneLineText( strName.c_str() );
}

void CVehicleWindow::SetType( const std::string & strType )
{
	GASSERT( m_pVehicleType );
	m_pVehicleType->SetOneLineText( strType.c_str() );
}


void CVehicleWindow::SetBattery( int nNOW )
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

void CVehicleWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

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

		m_pSlotBack[ VEHICLE_TYPE_BOARD ]->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pInterface->UiHideGroup( VEHICLE_WINDOW );
	}
}

void CVehicleWindow::LoadItem(int nIndex, const SITEMCUSTOM& ref_sItemCustom)
{
	m_ItemCustomArray[nIndex] = ref_sItemCustom;
	m_pSlotImage[nIndex]->SetItem( ref_sItemCustom );
}

void CVehicleWindow::UnLoadItem( int nIndex )
{
	m_ItemCustomArray[nIndex] = SITEMCUSTOM( NATIVEID_NULL() );
	m_pSlotImage[nIndex]->ResetItem();
}

MyVehicleWindow::MyVehicleWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CVehicleWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyVehicleWindow::CreateUIWindowAndRegisterOwnership()
{
    CVehicleWindow::Create( VEHICLE_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CVehicleWindow::CreateBaseWindowLightGray( "VEHICLE_WINDOW", (char*)ID2GAMEWORD("VEHICLE_NAME_STATIC",0 ) );
    CVehicleWindow::CreateSubControl();
    CVehicleWindow::SetAlignFlag( UI_FLAG_CENTER_Y );
    CVehicleWindow::m_pInterface->UiRegisterControl( this, true );
    CVehicleWindow::m_pInterface->UiShowGroupFocus( VEHICLE_WINDOW );
}

void MyVehicleWindow::SetVehicleInfoID( DWORD dwMasterID )
{
    CVehicleWindow::SetVehicleInfo( dwMasterID );
}

DWORD MyVehicleWindow::GetMasterID()
{
    return CVehicleWindow::GetMasterID();
}
