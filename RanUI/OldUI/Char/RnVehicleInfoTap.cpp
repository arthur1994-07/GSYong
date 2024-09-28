
#include "stdafx.h"

#include "RnVehicleInfoTap.h"



#include "CharacterView.h"


#include "../../../EngineLib/DeviceMan.h"
#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/BasicButton.h"


#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../RanLogicClient/PointShop/PointShopClient.h"

#include "../../../RanLogic/GLogicData.h" 

#include "../Item/ItemImage.h"
#include "../Item/ItemMove.h"


#include "../Util/RnButton.h"
#include "../Util/CheckBox.h"

#include "../enginelib/DxTools/DxFontMan.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const char* ID2GAMEWORD_BOOST_USING_EXPLAIN = "N키를 눌러 부스터 사용 가능";
	const char* ID2GAMEWORD_BOOST_EMPTY = "부스터 없음";

	const char* ID2GAMEWORD_VEHICLE_MOVING = "이동 모습";
	
	const char* ID2GAMEWORD_BOARD = "호버보드";
	const char* ID2GAMEWORD_BIKE = "오토바이크";
	const char* ID2GAMEWORD_PARTS = "튜닝 부속품";
	const char* ID2GAMEWORD_BATTERY = "연료";

	const char* ID2GAMEINTEXT_VEHICLE_PARTS_QUESTION_SENTENCE = "인벤토리에 보유 즁인 튜닝 부품을 아래 슬롯에 장착하여, 탈 것의 성능을 향상 시킬 수 있습니다.";

}


RnVehicleInfoTap::RnVehicleInfoTap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice ) : 
	CUIGroupHelper(pEngineDevice)
{
	m_pGaeaClient = pGaeaClient;
	m_pInterface  = pInterface;

	m_MyListCurrentPageNum = 1;
	m_PointShopListCurrentPageNum = 1;

	//////////////////////////////////////////////////////////////////////////

	ID2GAMEWORD_BOOST_USING_EXPLAIN = ID2GAMEINTEXT("CHARACTER_WINDOW_VEHICLE_BOOST_USING_EXPLAIN");
	ID2GAMEWORD_BOOST_EMPTY = ID2GAMEINTEXT("CHARACTER_WINDOW_VEHICLE_BOOST_EMPTY");

	ID2GAMEWORD_VEHICLE_MOVING = ID2GAMEINTEXT("CHARACTER_WINDOW_VEHICLE_MOVING_ANIMATION");
	ID2GAMEWORD_BOARD = ID2GAMEWORD("HOVERBOARD");
	ID2GAMEWORD_BIKE = ID2GAMEWORD("AUTOBIKE");
	ID2GAMEWORD_PARTS = ID2GAMEWORD("TUNING_PARTS");
	ID2GAMEWORD_BATTERY = ID2GAMEWORD("OIL");

	ID2GAMEINTEXT_VEHICLE_PARTS_QUESTION_SENTENCE = ID2GAMEINTEXT("CHARACTER_WINDOW_VEHICLE_PARTS_QUESTION");

}

void RnVehicleInfoTap::CreateSubControl()
{

	CreateLineBox( "CHARACTER_WINDOW_VEHICLE_MY_VEHICLE_LIST_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_VEHICLE_MY_VEHICLE_LIST_PAGE_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );

	CreateLineBox( "CHARACTER_WINDOW_VEHICLE_SETTING_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_VEHICLE_MESHRENDER_LINE_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_VEHICLE_MESHRENDER_REGION", "CHARACTER_WINDOW_REGION_TEXTUREINFO" );

	CreateLineBox( "CHARACTER_WINDOW_VEHICLE_BUY_TYPE_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_VEHICLE_BUY_LIST_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_VEHICLE_BUY_PAGE_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );


	CreateControlHelper(m_MyVehicleImage,"CHARACTER_WINDOW_VEHICLE_MY_VEHICLE_SLOT",D3DCOLOR_XRGB(255,0,0),MY_SLOT);
	m_MyVehicleImage->SetVisibleOverImage(true);

	for (int i=0; i < MY_VEHICLE_SLOT_COUNT; ++i)
	{
		std::string key = sc::string::format( "CHARACTER_WINDOW_VEHICLE_MY_VEHICLE_LIST_SLOT%1%",i+1);
		CreateControlHelper(m_MyVehicleImageList[i],key,D3DCOLOR_XRGB(255,255,255),MY_LIST_SLOT_1+i);
	}


	CreatePageHelper("CHARACTER_WINDOW_VEHICLE_MY_LIST_PRE_PAGE_BUTTON_NORMAL",MY_LIST_PRE_BUTTON,
		"CHARACTER_WINDOW_VEHICLE_MY_LIST_NEXT_PAGE_BUTTON_NORMAL",MY_LIST_NEXT_BUTTON,
		"CHARACTER_WINDOW_VEHICLE_MY_LIST_PAGE_NUMBER",m_MyListCurrentPageNumText);




	m_pCharacterView = new CharacterView(m_pEngineDevice);
	m_pCharacterView->SetPeaceModeData(m_pGaeaClient->GetCharacter(),m_pGaeaClient);

	m_pCharacterView->CreateSub( this, "CHARACTER_WINDOW_VEHICLE_MESHRENDER_REGION", UI_FLAG_DEFAULT, VEHICLE_RENDER_VIEW );
	RegisterControl ( m_pCharacterView );

	m_MovingAnimationCheckBox = new CCheckBox( m_pEngineDevice ); 
	m_MovingAnimationCheckBox->CreateSub( this, "CHARACTER_WINDOW_VEHICLE_ANIMATION_CHECKBOX" , UI_FLAG_DEFAULT , CHECKBOX_ANIMATION );
	m_MovingAnimationCheckBox->CreateSubControl ( "CHARACTER_WINDOW_VEHICLE_ANIMATION_CHECKBOX_ON", "CHARACTER_WINDOW_VEHICLE_ANIMATION_CHECKBOX_OFF" );
	m_MovingAnimationCheckBox->SetCheck( FALSE );
	RegisterControl( m_MovingAnimationCheckBox );

	CreateText(ID2GAMEWORD_VEHICLE_MOVING,"CHARACTER_WINDOW_VEHICLE_ANIMATION_CHECKBOX_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

	m_pBoosterIcon = CreateControl("CHARACTER_WINDOW_VEHICLE_BOOST_ICON");

	m_pVehicleNameText  = CreateText("VEHICLE_NAME","CHARACTER_WINDOW_VEHICLE_NAME",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pVehiclePartsSpeedText = CreateText("VEHICLE_PARTS_NAME","CHARACTER_WINDOW_VEHICLE_PARTS_SPEED_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pVehicleBatteryText = CreateText("BATTERY", "CHARACTER_WINDOW_VEHICLE_BATTERY_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pVehicleBoostText = CreateText("BOOST_TEXT","CHARACTER_WINDOW_VEHICLE_BOOST_TEXT",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);

	for ( int i = 0; i < PARTS_SLOT_COUNT; i++ )
	{
		std::string slotKey = sc::string::format("CHARACTER_WINDOW_VEHICLE_PARTS_SLOT%1%",i+1);
		std::string slotOverKey = slotKey + "_OVER";

		m_pSlotImage[i] = CreateItemImage( slotKey.c_str(), PARTS_SLOT_1 + i );
		m_pSlotDisplay[i] = CreateControl( slotOverKey.c_str() );
		m_pSlotDisplay[i]->SetVisibleSingle( FALSE );
	}

	m_pMouseOver = CreateControl( "ITEM_MOUSE_OVER" );
	m_pMouseOver->SetVisibleSingle( FALSE );

	CreatePageHelper("CHARACTER_WINDOW_VEHICLE_POINT_SHOP_LIST_PRE_PAGE_BUTTON_NORMAL",POINT_SHOP_LIST_PRE_BUTTON,
		"CHARACTER_WINDOW_VEHICLE_POINT_SHOP_LIST_NEXT_PAGE_BUTTON_NORMAL",POINT_SHOP_LIST_NEXT_BUTTON,
		"CHARACTER_WINDOW_VEHICLE_POINT_SHOP_LIST_PAGE_NUMBER",m_PointShopListCurrentPageNumText);

	CreateFlipButton("CHARACTER_WINDOW_VEHICLE_QUESTION_BUTTON","CHARACTER_WINDOW_VEHICLE_QUESTION_BUTTON_OVER",VEHICLE_QUESTION_BUTTON,CBasicButton::MOUSEIN_FLIP);

	for (int i=0; i < POINT_SHOP_SLOT_COUNT; ++i)
	{
		std::string key = sc::string::format( "CHARACTER_WINDOW_VEHICLE_POINT_SHOP_VEHICLE_LIST_SLOT%1%",i+1);
		CreateControlHelper(m_PointShopVehicleImageList[i],key,D3DCOLOR_XRGB(255,255,255),POINT_SHOP_LIST_SLOT_1+i);
	}

	//////////////////////////////////////////////////////////////////////////
	RnButton* pBuyBtn = NULL;
	RnButton::CreateArg arg;
	arg.defaultTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_NORMAL_TEXTUREINFO";
	arg.mouseOverTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_MOUSEOVER_TEXTUREINFO";
	arg.mouseClickTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_MOUSECLICK_TEXTUREINFO";
	arg.text = ID2GAMEWORD( "POINT_SHOP_BUY_BUTTON_TEXT" );

	m_pPointShopBuyButton = new RnButton(m_pEngineDevice);
	m_pPointShopBuyButton->CreateSub(this,"CHARACTER_WINDOW_VEHICLE_POINT_SHOP_LIST_BUY_BUTTON",UI_FLAG_DEFAULT,POINT_SHOP_BUY_BUTTON);
	m_pPointShopBuyButton->CreateSubControl(arg);
	RegisterControl(m_pPointShopBuyButton);

	DEFAULT_RADIO_BUTTON* pBoardRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
	pBoardRadio->CreateSub(this,"CHARACTER_WINDOW_VEHICLE_POINT_SHOP_BOARD_RADIO_BUTTON",UI_FLAG_DEFAULT,POINT_SHOP_BOARD_RADIO_BUTTON);
	pBoardRadio->CreateSubControl(ID2GAMEWORD_BOARD, TEXT_ALIGN_LEFT| TEXT_ALIGN_TOP ,"CHARACTER_WINDOW_VEHICLE_POINT_SHOP_RADIO_TEXT",NS_UITEXTUTIL::WHITE,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX","CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO","CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO");
	RegisterTapButton(pBoardRadio);
	RegisterControl(pBoardRadio);
	
	DEFAULT_RADIO_BUTTON* pBikeRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
	pBikeRadio->CreateSub(this,"CHARACTER_WINDOW_VEHICLE_POINT_SHOP_BIKE_RADIO_BUTTON",UI_FLAG_DEFAULT,POINT_SHOP_BIKE_RADIO_BUTTON);
	pBikeRadio->CreateSubControl(ID2GAMEWORD_BIKE,TEXT_ALIGN_LEFT| TEXT_ALIGN_TOP,"CHARACTER_WINDOW_VEHICLE_POINT_SHOP_RADIO_TEXT",NS_UITEXTUTIL::WHITE,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX","CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO","CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO");
	RegisterTapButton(pBikeRadio);
	RegisterControl(pBikeRadio);
	
	DEFAULT_RADIO_BUTTON* pPartsRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
	pPartsRadio->CreateSub(this,"CHARACTER_WINDOW_VEHICLE_POINT_SHOP_PARTS_RADIO_BUTTON",UI_FLAG_DEFAULT,POINT_SHOP_PARTS_RADIO_BUTTON);
	pPartsRadio->CreateSubControl(ID2GAMEWORD_PARTS,TEXT_ALIGN_LEFT| TEXT_ALIGN_TOP,"CHARACTER_WINDOW_VEHICLE_POINT_SHOP_RADIO_TEXT",NS_UITEXTUTIL::WHITE,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX","CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO","CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO");
	RegisterTapButton(pPartsRadio);
	RegisterControl(pPartsRadio);
	
	DEFAULT_RADIO_BUTTON* pBatteryRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
	pBatteryRadio->CreateSub(this,"CHARACTER_WINDOW_VEHICLE_POINT_SHOP_BATTERY_RADIO_BUTTON",UI_FLAG_DEFAULT,POINT_SHOP_BATTERY_RADIO_BUTTON);
	pBatteryRadio->CreateSubControl(ID2GAMEWORD_BATTERY,TEXT_ALIGN_LEFT| TEXT_ALIGN_TOP,"CHARACTER_WINDOW_VEHICLE_POINT_SHOP_RADIO_TEXT",NS_UITEXTUTIL::WHITE,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX","CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO","CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO");
	RegisterTapButton(pBatteryRadio);
	RegisterControl(pBatteryRadio);
	


	

}

CItemImage*	RnVehicleInfoTap::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage ( m_pGaeaClient, m_pEngineDevice );
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}


void RnVehicleInfoTap::CreateControlHelper( CItemImage*& vehicleImage,const std::string& key,DWORD boardColor,UIGUID id )
{
	vehicleImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
	vehicleImage->CreateSub( this, key.c_str(), UI_FLAG_DEFAULT,id);
	vehicleImage->CreateSubControl("ITEM_MOUSE_OVER",boardColor);
	vehicleImage->SetVisibleOverImage(false);

	RegisterControl( vehicleImage );
}



void RnVehicleInfoTap::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	UpdateCharacterView(false);

	UpdateVehicleParts();


	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


void RnVehicleInfoTap::UpdateCharacterView(bool isForce)
{
	if( !m_pCharacterView->GetDevice() )
		return;

	SNATIVEID curMyVehicleId = m_pGaeaClient->GetCharacter()->GET_SLOT_ITEM(SLOT_VEHICLE).Id();

	if( m_MyVehicleImage->GetItemID() != curMyVehicleId )
		isForce = true;

	m_MyVehicleImage->SetItem(curMyVehicleId);

	if( isForce )
	{
		m_myVehicleListCacheVec.clear();
		ShowMyVehicleList();
	}


	SNATIVEID selectedVehicle = GetSelectedVehicleFromList();
	if( selectedVehicle == NATIVEID_NULL() )
	{
		selectedVehicle = m_MyVehicleImage->GetItemID();
		if( selectedVehicle == NATIVEID_NULL() )
		{
			m_pCharacterView->Clear();
			return;
		}		
	}

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	m_pCharacterView->SetUpdateCharacter(pCharacter,isForce);

	CharacterView::VehicleData vehicleData;
	vehicleData.VehicleID = selectedVehicle;
	if( vehicleData.VehicleID == curMyVehicleId )
	{
		for (int i = 0; i < ACCE_TYPE_SIZE; ++i)
		{
			vehicleData.Parts[i] = pCharacter->VehiclePutOnItem(VEHICLE_ACCESSORYTYPE(i));
			vehicleData.ColorArray[i].dwMainColor = pCharacter->VehicleColorMain(VEHICLE_ACCESSORYTYPE(i));
			vehicleData.ColorArray[i].dwSubColor = pCharacter->VehicleColorSub(VEHICLE_ACCESSORYTYPE(i));
		}
	}

	m_pCharacterView->SetUpdateVehicle(vehicleData,isForce);
	if( m_MovingAnimationCheckBox->IsChecked() )
		m_pCharacterView->SetAnimationWalk();
	else
		m_pCharacterView->SetAnimationStop();

}

void RnVehicleInfoTap::UpdateVehicleParts()
{
	// 자기것인지 아닌지
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	bool isMySlotVehicle = GetSelectedVehicleFromList() == NATIVEID_NULL();

	for (int i = 0; i < PARTS_SLOT_COUNT; ++i)
	{
		m_pSlotImage[i]->SetVisibleSingle(isMySlotVehicle);
		m_pSlotDisplay[i]->SetVisibleSingle(isMySlotVehicle);
	}
		
	if( m_pMouseOver ) 
		m_pMouseOver->SetVisibleSingle ( false );

	//	스냅, 스킬 이미지 붙이기
	IItemMove* pItemMove = m_pInterface->GetItemMove();
	SNATIVEID sHOLD_ITEM_ID(false);
	if (pItemMove)
		sHOLD_ITEM_ID = pItemMove->GetItem();

	for (int i = 0; i < PARTS_SLOT_COUNT; ++i)
	{
		SITEMCUSTOM sItemCustomOld = m_ItemCustomArray[i];
		SITEMCUSTOM sItemCustom = GetPartsItem(i);//pChar->VehiclePutOnItemRef(VEHICLE_ACCESSORYTYPE(i+1));

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
			if( pCharacter->GET_SLOT_ITEM(SLOT_VEHICLE).Id() != NATIVEID_NULL().Id() )
			{
				// 장착될 위치 표시
				if (pCharacter->VehicleCheckSlotItem(sHOLD_ITEM_ID, IMAGE2EMSLOT(i+PARTS_SLOT_1)))
				{
					m_pSlotDisplay[i]->SetVisibleSingle( TRUE );
				}
			}
		}
	}


	SNATIVEID selectedVehicle = GetSelectedVehicleFromList();
	bool isCurrentMyVehicle = false;
	if( selectedVehicle == NATIVEID_NULL() )
	{
		selectedVehicle = m_MyVehicleImage->GetItemID();
		isCurrentMyVehicle = true;
		if( selectedVehicle == NATIVEID_NULL() )
		{
			// 아무것도 세팅안되어 있으므로 리턴하자
			m_pVehicleNameText->SetText("");
			m_pVehiclePartsSpeedText->SetText("");
			m_pVehicleBatteryText->SetText("");
			m_pVehicleBoostText->SetText("");
			m_pBoosterIcon->SetVisibleSingle(FALSE);
			return;
		}
	}

	CString strMoveSpeedText;
	const SITEM* pItemData = GLogicData::GetInstance().GetItem(selectedVehicle);
    if (pItemData)
	    m_pVehicleNameText->SetText(pItemData->GetName());

	float fAddOnSpeed = 0;
	if(isCurrentMyVehicle)
		fAddOnSpeed = pCharacter->VehiclePutOnItemMoveSpeedTotal();

	if (pItemData)
	{
		EMITEM_VAR emITEM_VOL = pItemData->sSuitOp.sVOLUME.emTYPE;
		//	이동속도 변화량
		//
		if ( emITEM_VOL != EMVAR_NONE)
		{
			if ( emITEM_VOL == EMVAR_MOVE_SPEED )
			{
				float fVOLUME = SITEMCUSTOM(selectedVehicle).GETMOVESPEED();

				if ( fAddOnSpeed != 0.0f )
					strMoveSpeedText.Format("%s %.2f(+%.2f)", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ), fVOLUME, fAddOnSpeed);
				else 
					strMoveSpeedText.Format("%s %.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ), fVOLUME );

				m_pVehiclePartsSpeedText->SetOneLineText( strMoveSpeedText );
			}
			else
			{
				float fVOLUME = pItemData->sSuitOp.sVOLUME.fVolume;
				strMoveSpeedText.Format("%s %.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ), fVOLUME );
				m_pVehiclePartsSpeedText->SetOneLineText( strMoveSpeedText );
			}
		}
	}

	CString strBattery;
	if( isCurrentMyVehicle )
		strBattery.Format("%s : %.2f%%",ID2GAMEWORD("VEHICLE_TEXT_STATIC",2),(float)(pCharacter->VehicleFull())/10.f);

	m_pVehicleBatteryText->SetText( strBattery , NS_UITEXTCOLOR::PRIVATE);

	if( ! isMySlotVehicle )
	{
		m_pVehicleBoostText->SetVisibleSingle(FALSE);
		m_pBoosterIcon->SetVisibleSingle(FALSE);
	}
	else if (pCharacter->VehicleBoosterId())
	{
		m_pVehicleBoostText->SetVisibleSingle(TRUE);
		m_pBoosterIcon->SetVisibleSingle(TRUE);
		m_pBoosterIcon->SetDiffuseAlpha(255);		
		m_pVehicleBoostText->SetText(ID2GAMEWORD_BOOST_USING_EXPLAIN,NS_UITEXTCOLOR::PRIVATE);
	}
	else
	{
		m_pVehicleBoostText->SetVisibleSingle(TRUE);
		m_pBoosterIcon->SetVisibleSingle(TRUE);
		m_pBoosterIcon->SetDiffuseAlpha(123);
		m_pVehicleBoostText->SetText(ID2GAMEWORD_BOOST_EMPTY,NS_UITEXTCOLOR::PRIVATE);
	}



}

void RnVehicleInfoTap::SetVisibleSingle( BOOL bVisible )
{
	CUIGroupHelper::SetVisibleSingle(bVisible);

	if( bVisible )
	{
		// 서버로부터 락커룸정보 요청
		// 락커룸 탈것정보는 일단 비우고 다시 받자
// 		m_StorageVehicleVec.clear();
// 		m_myVehicleListCacheVec.clear();
	//	ShowMyVehicleList();

		m_pGaeaClient->GetCharacter()->ReqGetStorageSpecificItem(ITEM_VEHICLE);
	//	EventSelectedTap(POINT_SHOP_BOARD_RADIO_BUTTON);
		TapSelect(POINT_SHOP_BOARD_RADIO_BUTTON);

	}

	UpdateCharacterView(bVisible ? true : false);
}

void RnVehicleInfoTap::ShowMyVehicleList()
{
	m_myVehicleListCacheVec.reserve(10);

	GLCHARLOGIC& ref_CharData = m_pGaeaClient->GetCharacterLogic();
	const GLInventory& ref_Inventory = ref_CharData.m_cInventory;	
	const GLInventory::CELL_MAP* pItemList = ref_Inventory.GetItemList();
	GLInventory::CELL_MAP_CITER iter = pItemList->begin();
	GLInventory::CELL_MAP_CITER iter_end = pItemList->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM *pINVENITEM = (*iter).second;

		const SITEM* pItemData = GLogicData::GetInstance().GetItem ( pINVENITEM->GetNativeID() );
		if( pItemData->BasicType() == ITEM_VEHICLE )
		{
			m_myVehicleListCacheVec.push_back(pINVENITEM->GetNativeID());
		}
	}

	m_myVehicleListCacheVec.insert(m_myVehicleListCacheVec.end(),m_StorageVehicleVec.begin(),m_StorageVehicleVec.end());

	UpdateMyVehicleListPage();
}

void RnVehicleInfoTap::ShowPointShopVehicleList(UIGUID radio)
{
	// 포인트 샵으로 부터 정보를 얻고
	m_PointShopVehicleListCacheVec.clear();
	m_PointShopVehicleListCacheVec.reserve(50);

	std::vector<pointshop::SellItemClient> vecSellItemList;
	std::string SearchName("");
	std::vector<EMITEM_TYPE> TypeList;
	std::vector<pointshop::SEARCH_ORDER> OrderList;

	m_pGaeaClient->GetPointShopClient()->GetList( vecSellItemList, SearchName, TypeList, OrderList );


	if( radio == POINT_SHOP_BOARD_RADIO_BUTTON )
	{
		for (size_t i=0; i < vecSellItemList.size(); ++i)
		{
			if( vecSellItemList[i].m_Type == ITEM_VEHICLE )
			{
				const SITEM* pItemData = GLogicData::GetInstance().GetItem ( vecSellItemList[i].ItemId() );
				if( pItemData->VehicleType() == VEHICLE_TYPE_BOARD )
				{
					m_PointShopVehicleListCacheVec.push_back(vecSellItemList[i].ItemId());
				}
			}
		}
	}
	else if( radio == POINT_SHOP_BIKE_RADIO_BUTTON )
	{
		for (size_t i=0; i < vecSellItemList.size(); ++i)
		{
			if( vecSellItemList[i].m_Type == ITEM_VEHICLE )
			{
				const SITEM* pItemData = GLogicData::GetInstance().GetItem ( vecSellItemList[i].ItemId() );
				if( pItemData->VehicleType() == VEHICLE_TYPE_BIKE )
				{
					m_PointShopVehicleListCacheVec.push_back(vecSellItemList[i].ItemId());
				}
			}
		}		
	}
	else if( radio == POINT_SHOP_PARTS_RADIO_BUTTON )
	{
		for (size_t i=0; i < vecSellItemList.size(); ++i)
		{
			if( vecSellItemList[i].m_Type == ITEM_SUIT )
			{
				if( vecSellItemList[i].m_Suit >= SUIT_VEHICLE_PARTS_A && 
					vecSellItemList[i].m_Suit <= SUIT_VEHICLE_PARTS_F )
				{
					m_PointShopVehicleListCacheVec.push_back(vecSellItemList[i].ItemId());
				}
			}
		}
	}
	else if( radio == POINT_SHOP_BATTERY_RADIO_BUTTON )
	{

		for (size_t i=0; i < vecSellItemList.size(); ++i)
		{
			if( vecSellItemList[i].m_Type == ITEM_VEHICLE_OIL )
			{
				m_PointShopVehicleListCacheVec.push_back(vecSellItemList[i].ItemId());
			}
		}		
	}
	else
	{
		GASSERT(false);
	}

	m_pPointShopBuyButton->SetVisibleSingle(!m_PointShopVehicleListCacheVec.empty());

	m_PointShopListCurrentPageNum = 1;
	UpdatePointShopVehicleListPage();
}

void RnVehicleInfoTap::UpdateMyVehicleListPage()
{
	UpdateVehicleListPageHelper(m_MyListCurrentPageNumText,GetMyListCurrentPage(),MY_VEHICLE_SLOT_COUNT,m_MyVehicleImageList,m_myVehicleListCacheVec);
}

void RnVehicleInfoTap::UpdatePointShopVehicleListPage()
{
	UpdateVehicleListPageHelper(m_PointShopListCurrentPageNumText,GetPointShopCurrentPage(),POINT_SHOP_SLOT_COUNT,m_PointShopVehicleImageList,m_PointShopVehicleListCacheVec);
}

void RnVehicleInfoTap::UpdateVehicleListPageHelper(CBasicTextBox* pageTextBox,int currentPage,int MaxSlot,
												   CItemImage** ImageList,const std::vector<SNATIVEID>& vehicleVec)
{
	CString pageText;
	pageText.Format("%d",currentPage);
	pageTextBox->SetText(pageText);

	int start = MaxSlot * ( currentPage - 1 );
	int end   = MaxSlot * currentPage; 

	for (int i = start,k=0; i < end; ++i,++k)
	{
		ImageList[k]->SetVisibleOverImage(false);

		if( i >= int(vehicleVec.size()) )
			ImageList[k]->SetItem(NATIVEID_NULL());
		else
			ImageList[k]->SetItem(vehicleVec[i]);
	}
}

int RnVehicleInfoTap::GetMyListCurrentPage()
{
	return m_MyListCurrentPageNum;	
}

int RnVehicleInfoTap::GetPointShopCurrentPage()
{
	return m_PointShopListCurrentPageNum;
}

bool RnVehicleInfoTap::PlusPage(int& curPage,std::vector<SNATIVEID>& itemCacheVec,int maxSize)
{
	++curPage;
	int limitPage = int(itemCacheVec.size())/maxSize;
	if(limitPage == 0)
		limitPage = 1;
	else
		++limitPage;

	if( curPage > limitPage )
	{
		curPage = limitPage;
		return false;
	}

	return true;
}

bool RnVehicleInfoTap::MinusPage(int& curPage)
{
	--curPage;
	if( curPage < 1 )
	{
		curPage = 1;
		return false;
	}

	return true;
}

void RnVehicleInfoTap::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{

	switch(ControlID)
	{
	case MY_SLOT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				ClearOverImage();

				UpdateCharacterView(false);
			}
			break;
		}
	case MY_LIST_SLOT_1:
	case MY_LIST_SLOT_2:
	case MY_LIST_SLOT_3:
	case MY_LIST_SLOT_4:
	case MY_LIST_SLOT_5:
	case MY_LIST_SLOT_6:
	case MY_LIST_SLOT_7:
		{
			if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				int index = ControlID - MY_LIST_SLOT_1;
				if( m_MyVehicleImageList[index]->GetItemID() != NATIVEID_NULL() )
				{
					ClearOverImage();
					m_MyVehicleImageList[index]->SetVisibleOverImage(true);

					UpdateCharacterView(false);
				}
			}
			break;
		}
	case MY_LIST_PRE_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK )
			{
				if( MinusPage(m_MyListCurrentPageNum) )
				{
					UpdateMyVehicleListPage();
				}
			}
			break;
		}
	case MY_LIST_NEXT_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK )
			{
				if( PlusPage(m_MyListCurrentPageNum,m_myVehicleListCacheVec,MY_VEHICLE_SLOT_COUNT) )
				{
					UpdateMyVehicleListPage();
				}
			}
			break;
		}

	case CHECKBOX_ANIMATION:
		{
			if( dwMsg & UIMSG_CHECKBOX_PUSH )
			{
				UpdateCharacterView(false);
			}
			break;
		}

	case VEHICLE_QUESTION_BUTTON:
		{
			if( CHECK_MOUSE_IN ( dwMsg ) )
			{
				CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
			
				m_pInterface->BEGIN_COMMON_LINEINFO_MULTI(TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
				NS_UITEXTUTIL::MULTILINETEXT multiLine = NS_UITEXTUTIL::GetMLTextWithoutCRLF(ID2GAMEINTEXT_VEHICLE_PARTS_QUESTION_SENTENCE,350,pFont);
				for (size_t i=0; i < multiLine.size(); ++i)
					m_pInterface->ADD_COMMON_LINEINFO_MULTI(multiLine[i].strLine, NS_UITEXTUTIL::ENABLE);
				
				m_pInterface->END_COMMON_LINEINFO_MULTI();
			}
			break;
		}
	case PARTS_SLOT_1:
	case PARTS_SLOT_2:
	case PARTS_SLOT_3:
	case PARTS_SLOT_4:
	case PARTS_SLOT_5:
	case PARTS_SLOT_6:
		{	
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{	
				int nIndex = ControlID - PARTS_SLOT_1;
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
				}				

				//	마우스 표시 테두리
				if ( pItemMove->GetItem () == NATIVEID_NULL () && m_pSlotImage[nIndex]->GetItemImageSlotPos () != NATIVEID_NULL () )
				{
					const UIRECT& rcImagePos = m_pSlotImage[nIndex]->GetGlobalPos ();
					m_pMouseOver->SetGlobalPos ( rcImagePos );
					m_pMouseOver->SetVisibleSingle ( TRUE );
				}

				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

				if ( dwMsg & UIMSG_LB_UP )
				{
					pCharacter->ReqVehicleChangeAccessory( IMAGE2EMSLOT(ControlID) );
				}

				if ( dwMsg & UIMSG_RB_UP )
				{
					pCharacter->ReqVehicleRemoveSlotItem( IMAGE2EMSLOT(ControlID) );
				}

				if( m_ItemCustomArray[nIndex].GetNativeID() != NATIVEID_NULL () )
				{
					m_pInterface->SHOW_ITEM_INFO ( m_ItemCustomArray[nIndex], FALSE, FALSE, FALSE, FALSE,FALSE, USHRT_MAX, USHRT_MAX );
				}
			}
			break;
		}

	case POINT_SHOP_LIST_PRE_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK )
			{
				if( MinusPage(m_PointShopListCurrentPageNum) )
				{
					UpdatePointShopVehicleListPage();
				}
			}
			break;
		}
	case POINT_SHOP_LIST_NEXT_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK )
			{
				if( PlusPage(m_PointShopListCurrentPageNum,m_PointShopVehicleListCacheVec,POINT_SHOP_SLOT_COUNT) )
				{
					UpdatePointShopVehicleListPage();
				}
			}
			break;
		}

	case POINT_SHOP_LIST_SLOT_1:
	case POINT_SHOP_LIST_SLOT_2:
	case POINT_SHOP_LIST_SLOT_3:
	case POINT_SHOP_LIST_SLOT_4:
	case POINT_SHOP_LIST_SLOT_5:
	case POINT_SHOP_LIST_SLOT_6:
	case POINT_SHOP_LIST_SLOT_7:
	case POINT_SHOP_LIST_SLOT_8:
		{
			int index = ControlID - POINT_SHOP_LIST_SLOT_1;
			SNATIVEID id = m_PointShopVehicleImageList[index]->GetItemID();

			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( id != NATIVEID_NULL() )
				{
					m_pInterface->SHOW_ITEM_INFO(
						SITEMCUSTOM(id),
						FALSE, FALSE, FALSE, FALSE, FALSE,
						USHRT_MAX, USHRT_MAX );
				}
			}

			if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) | CHECK_MOUSEIN_RBUPLIKE(dwMsg) )
			{				
				if( id != NATIVEID_NULL() )
				{
					if( IsVehicleItem( id ) )
					{
						ClearOverImage();
					}
					else
					{
						ClearOverImage(false,true);
					}

					m_PointShopVehicleImageList[index]->SetVisibleOverImage(true);
				}

				if( CHECK_MOUSEIN_RBUPLIKE(dwMsg) )
				{
					ShowPointShop(GetSelectedPointShopItemFromList());
				}
			}
			break;
		}
	case POINT_SHOP_BUY_BUTTON:
		{
			if(dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				ShowPointShop(GetSelectedPointShopItemFromList());
			}
			break;
		}
	}

	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );

}

SNATIVEID RnVehicleInfoTap::GetSelectedVehicleFromList()
{
	for (int i=0; i < MY_VEHICLE_SLOT_COUNT; ++i)
	{
		if( m_MyVehicleImageList[i]->IsVisibleOverImage() )
			return m_MyVehicleImageList[i]->GetItemID();
	}

	for (int i=0; i < POINT_SHOP_SLOT_COUNT; ++i)
	{
		if( m_PointShopVehicleImageList[i]->IsVisibleOverImage() )
		{
			SNATIVEID id = m_PointShopVehicleImageList[i]->GetItemID();
			if(IsVehicleItem(id))
				return id;
		}
	}

	return NATIVEID_NULL();
}

void RnVehicleInfoTap::LoadItem( int nIndex, const SITEMCUSTOM& ref_sItemCustom )
{
	m_ItemCustomArray[nIndex] = ref_sItemCustom;
	m_pSlotImage[nIndex]->SetItem(ref_sItemCustom);
}

void RnVehicleInfoTap::UnLoadItem( int nIndex )
{
	m_ItemCustomArray[nIndex] = SITEMCUSTOM( NATIVEID_NULL() );
	m_pSlotImage[nIndex]->ResetItem();
}


VEHICLE_TYPE RnVehicleInfoTap::GetVehicleType()
{
	SNATIVEID curMyVehicleId = m_pGaeaClient->GetCharacter()->GET_SLOT_ITEM(SLOT_VEHICLE).Id();
	const SITEM* pItemData = GLogicData::GetInstance().GetItem ( curMyVehicleId );
    if (pItemData)
	    return pItemData->VehicleType();
    else
        return VEHICLE_TYPE_NONE;
}


// int RnVehicleInfoTap::GetPartsSlotSize()
// {
// 	VEHICLE_TYPE type = GetVehicleType();
// 	if( type == VEHICLE_TYPE_BIKE ) 
// 	{
// 		return 6;
// 	}
// 	else if( type == VEHICLE_TYPE_BOARD )
// 	{
// 		return 4;
// 	}
// 
// 	GASSERT(false);
// 	return 0;
// }

EMSUIT RnVehicleInfoTap::IMAGE2EMSLOT( UIGUID ControlID )
{
	VEHICLE_TYPE type = GetVehicleType();
	if( type == VEHICLE_TYPE_BIKE ) 
	{
		switch ( ControlID )
		{	
		case PARTS_SLOT_1: return SUIT_VEHICLE_PARTS_A;
		case PARTS_SLOT_2: return SUIT_VEHICLE_PARTS_B;
		case PARTS_SLOT_3: return SUIT_VEHICLE_PARTS_C;
		case PARTS_SLOT_4: return SUIT_VEHICLE_PARTS_D;
		case PARTS_SLOT_5: return SUIT_VEHICLE_PARTS_E;
		case PARTS_SLOT_6: return SUIT_VEHICLE_PARTS_F;
		}
	}
	else if( type == VEHICLE_TYPE_BOARD )
	{
		switch ( ControlID )
		{	
		case PARTS_SLOT_1: return SUIT_VEHICLE_SKIN;
		case PARTS_SLOT_2: return SUIT_VEHICLE_PARTS_A;
		case PARTS_SLOT_3: return SUIT_VEHICLE_PARTS_B;
		case PARTS_SLOT_4: return SUIT_VEHICLE_PARTS_C;
		}
	}

	//GASSERT(false);
	return SUIT_NSIZE;
}

SITEMCUSTOM RnVehicleInfoTap::GetPartsItem( int parts )
{
	if( NATIVEID_NULL() == m_pGaeaClient->GetCharacter()->GET_SLOT_ITEM(SLOT_VEHICLE).Id() )
		return NATIVEID_NULL();

	int index = parts;
	VEHICLE_TYPE type = GetVehicleType();
	if(type == VEHICLE_TYPE_BIKE)
		index++;
	
	return m_pGaeaClient->GetCharacter()->VehiclePutOnItemRef(VEHICLE_ACCESSORYTYPE(index));
}

void RnVehicleInfoTap::CreatePageHelper(const std::string& prePageKey,UIGUID prePageID,
										const std::string& nextPageKey,UIGUID nextPageID,
										const std::string& pageTextKey,CBasicTextBox*& pageText)
{

	RnButton::CreateArg arg;
	//////////////////////////////////////////////////////////////////////////
	arg.defaultTextureKey = "CHARACTER_WINDOW_PRE_PAGE_BUTTON_NORMAL_TEXTUREINFO";
	arg.mouseOverTextureKey = "CHARACTER_WINDOW_PRE_PAGE_BUTTON_OVER_TEXTUREINFO";
	arg.mouseClickTextureKey = "CHARACTER_WINDOW_PRE_PAGE_BUTTON_CLICK_TEXTUREINFO";
	//////////////////////////////////////////////////////////////////////////
	RnButton* pPreBtn = new RnButton(m_pEngineDevice);
	pPreBtn->CreateSub(this,prePageKey.c_str(),UI_FLAG_DEFAULT,prePageID);
	pPreBtn->CreateSubControl(arg);
	RegisterControl(pPreBtn);

	arg.defaultTextureKey = "CHARACTER_WINDOW_NEXT_PAGE_BUTTON_NORMAL_TEXTUREINFO";
	arg.mouseOverTextureKey = "CHARACTER_WINDOW_NEXT_PAGE_BUTTON_OVER_TEXTUREINFO";
	arg.mouseClickTextureKey = "CHARACTER_WINDOW_NEXT_PAGE_BUTTON_CLICK_TEXTUREINFO";

	RnButton* pNextBtn = new RnButton(m_pEngineDevice);
	pNextBtn->CreateSub(this,nextPageKey.c_str(),UI_FLAG_DEFAULT,nextPageID);
	pNextBtn->CreateSubControl(arg);
	RegisterControl(pNextBtn);

	pageText = CreateText("",pageTextKey.c_str(),TEXT_ALIGN_CENTER_X|TEXT_ALIGN_TOP);

}

void RnVehicleInfoTap::ClearOverImage(bool myVehicleList,bool PointShopVehicleList)
{
	if( myVehicleList )
	{
		for (int i=0; i < MY_VEHICLE_SLOT_COUNT; ++i)
		{
			m_MyVehicleImageList[i]->SetVisibleOverImage(false);
		}
	}

	if( PointShopVehicleList )
	{
		for (int i=0; i < POINT_SHOP_SLOT_COUNT; ++i)
		{
			m_PointShopVehicleImageList[i]->SetVisibleOverImage(false);
		}
	}
}

bool RnVehicleInfoTap::IsVehicleItem( const SNATIVEID& id )
{
	if( id == NATIVEID_NULL() )
		return false;

	const SITEM* pItemData = GLogicData::GetInstance().GetItem( id );
	GASSERT(pItemData);
	if( pItemData->BasicType() == ITEM_VEHICLE )
		return true;
    else	
	    return false;
}

void RnVehicleInfoTap::EventSelectedTap( UIGUID ControlID )
{
	ShowPointShopVehicleList(ControlID);
}

void RnVehicleInfoTap::ShowPointShop(SNATIVEID id)
{
	if( id == NATIVEID_NULL() )
		return;

	m_pInterface->UiShowGroupFocus(POINT_SHOP_WINDOW);

	const SITEM* pItemData = GLogicData::GetInstance().GetItem(id);
    if (pItemData)
    {
	    std::string itemName = pItemData->GetName();
 	    m_pInterface->GetPointShopWindow()->SearchItem(itemName);
    }
}

SNATIVEID RnVehicleInfoTap::GetSelectedPointShopItemFromList()
{
	for (int i=0; i < POINT_SHOP_SLOT_COUNT; ++i)
	{
		if( m_PointShopVehicleImageList[i]->IsVisibleOverImage() )
		{
			return m_PointShopVehicleImageList[i]->GetItemID();
		}
	}

	return NATIVEID_NULL();
}

void RnVehicleInfoTap::SetStorageItemToVehicleTapWindow( const std::vector<DWORD>& vehicleIDVec )
{
	m_StorageVehicleVec.clear();
	m_myVehicleListCacheVec.clear();

	for (size_t i = 0; i < vehicleIDVec.size(); ++i)
	{
		m_StorageVehicleVec.push_back( SNATIVEID(vehicleIDVec[i]));
	}

	ShowMyVehicleList();
}

void RnVehicleInfoTap::DeleteUIMeshData()
{
	m_pCharacterView->DeleteUIMeshData();
}
