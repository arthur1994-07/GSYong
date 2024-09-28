#include "stdafx.h"

#include "RnCharacterWearWindow.h"
#include "CharacterView.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"

#include "../../../SigmaCore/gassert.h"

#include "../Item/ItemImage.h"
#include "../Item/ItemMove.h"
#include "../Item/RnInventoryWindow.h"
#include "../../../RanLogic/Item/ItemDurability.h"

#include "../../../RanLogic/GLogicData.h"

RnCharacterWearWindow::RnCharacterWearWindow( GLGaeaClient* pGaeaClient,CInnerInterface* pInterface,EngineDeviceMan* pEngineDevice ) : CUIGroupHelper(pEngineDevice) 
{
	MIN_STATIC_ASSERT( (ITEM_IMAGE_SIZE-ITEM_IMAGE_HEAD) == (ITEM_COLOR_SIZE-ITEM_COLOR_HEAD) );

	m_pGaeaClient = pGaeaClient;
	m_pInterface = pInterface;

	m_pCharacterView = NULL;
	m_pMouseOver = NULL;

	for (int i =0; i < ITEM_IMAGE_SIZE; ++i)
	{
		m_pSlotDisplay[i] = NULL;
		m_pItemImage[i] = NULL;
		m_pColorInfo[i] = NULL; 
	}
}	

void RnCharacterWearWindow::CreateSubControl()
{
	m_pCharacterView = new CharacterView(m_pEngineDevice);
	m_pCharacterView->SetPeaceModeData(m_pGaeaClient->GetCharacter(),m_pGaeaClient);

	m_pCharacterView->CreateSub( this, "CHARACTER_WINDOW_CHARACTER_MESHRENDER", UI_FLAG_DEFAULT, CHARACTER_RENDER_VIEW );
	RegisterControl ( m_pCharacterView );

	CreateSlotHelper(ITEM_IMAGE_HEAD,"CHARACTER_WINDOW_WEAR_SLOT_HEAD",ITEM_IMAGE_HEAD,ITEM_COLOR_HEAD);
	CreateSlotHelper(ITEM_IMAGE_UPPER,"CHARACTER_WINDOW_WEAR_SLOT_UPPER",ITEM_IMAGE_UPPER,ITEM_COLOR_UPPER);
	CreateSlotHelper(ITEM_IMAGE_WAIST,"CHARACTER_WINDOW_WEAR_SLOT_WAIST",ITEM_IMAGE_WAIST,ITEM_COLOR_WAIST);
	CreateSlotHelper(ITEM_IMAGE_LOWER,"CHARACTER_WINDOW_WEAR_SLOT_LOWER",ITEM_IMAGE_LOWER,ITEM_COLOR_LOWER);
	CreateSlotHelper(ITEM_IMAGE_HAND,"CHARACTER_WINDOW_WEAR_SLOT_HAND",ITEM_IMAGE_HAND,ITEM_COLOR_HAND);
	CreateSlotHelper(ITEM_IMAGE_FOOT,"CHARACTER_WINDOW_WEAR_SLOT_FOOT",ITEM_IMAGE_FOOT,ITEM_COLOR_FOOT);
	CreateSlotHelper(ITEM_IMAGE_LEFT_ACCESSORY,"CHARACTER_WINDOW_WEAR_SLOT_LEFT_ACCESSORY",ITEM_IMAGE_LEFT_ACCESSORY,ITEM_COLOR_LEFT_ACCESSORY);
	CreateSlotHelper(ITEM_IMAGE_EARRINGS,"CHARACTER_WINDOW_WEAR_SLOT_LEFT_EARRING",ITEM_IMAGE_EARRINGS,ITEM_COLOR_LEFT_EARRING);
	CreateSlotHelper(ITEM_IMAGE_DECORATION,"CHARACTER_WINDOW_WEAR_SLOT_RIGHT_EARRING",ITEM_IMAGE_DECORATION,ITEM_COLOR_RIGHT_EARRING);
	CreateSlotHelper(ITEM_IMAGE_NECK,"CHARACTER_WINDOW_WEAR_SLOT_NECK",ITEM_IMAGE_NECK,ITEM_COLOR_NECK);
	CreateSlotHelper(ITEM_IMAGE_WRIST,"CHARACTER_WINDOW_WEAR_SLOT_WRIST",ITEM_IMAGE_WRIST,ITEM_COLOR_WRIST);
	CreateSlotHelper(ITEM_IMAGE_LEFT_FINGER,"CHARACTER_WINDOW_WEAR_SLOT_LEFT_FINGER",ITEM_IMAGE_LEFT_FINGER,ITEM_COLOR_LEFT_FINGER);
	CreateSlotHelper(ITEM_IMAGE_RIGHT_FINGER,"CHARACTER_WINDOW_WEAR_SLOT_RIGHT_FINGER",ITEM_IMAGE_RIGHT_FINGER,ITEM_COLOR_RIGHT_FINGER);
	CreateSlotHelper(ITEM_IMAGE_RIGHT_ACCESSORY,"CHARACTER_WINDOW_WEAR_SLOT_RIGHT_ACCESSORY",ITEM_IMAGE_RIGHT_ACCESSORY,ITEM_COLOR_RIGHT_ACCESSORY);

// 	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
// 	const EMCHARCLASS emCharClass = pGlobalStage->GetGameStage()->GetCharJoinData().m_CharData2.m_emClass;
// 	const int nClassType = CharClassToIndex( emCharClass );

	// 극강부와 일반 캐릭터를 구분해서 인벤토리 장착 윈도우를 생성한다.
	if( IsExtreme() )
	{
		CreateSlotHelper(ITEM_IMAGE_LEFT_WEAPON,"CHARACTER_WINDOW_WEAR_SLOT_EX_FIRST_LEFT_WEAPON",ITEM_IMAGE_LEFT_WEAPON,ITEM_COLOR_LEFT_WEAPON);
		CreateSlotHelper(ITEM_IMAGE_RIGHT_WEAPON,"CHARACTER_WINDOW_WEAR_SLOT_EX_FIRST_RIGHT_WEAPON",ITEM_IMAGE_RIGHT_WEAPON,ITEM_COLOR_RIGHT_WEAPON);

		CreateSlotHelper(ITEM_IMAGE_LEFT_WEAPON_S,"CHARACTER_WINDOW_WEAR_SLOT_EX_SECOND_LEFT_WEAPON",ITEM_IMAGE_LEFT_WEAPON_S,ITEM_COLOR_LEFT_WEAPON_S);
		CreateSlotHelper(ITEM_IMAGE_RIGHT_WEAPON_S,"CHARACTER_WINDOW_WEAR_SLOT_EX_SECOND_RIGHT_WEAPON",ITEM_IMAGE_RIGHT_WEAPON_S,ITEM_COLOR_RIGHT_WEAPON_S);
	


 		DEFAULT_RADIO_BUTTON* pExtremFirstRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
 		pExtremFirstRadio->CreateSub(this,"CHARACTER_WINDOW_WEAR_SLOT_EX_FIRST_RADIO_BUTTON",UI_FLAG_DEFAULT,RADIO_EXTREME_FIRST);
 		pExtremFirstRadio->CreateSubControl("", TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y ,"",0,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX","CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO","CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO");
		RegisterTapButton(pExtremFirstRadio);
		RegisterControl(pExtremFirstRadio);


		DEFAULT_RADIO_BUTTON* pExtremSecondRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
		pExtremSecondRadio->CreateSub(this,"CHARACTER_WINDOW_WEAR_SLOT_EX_SECOND_RADIO_BUTTON",UI_FLAG_DEFAULT,RADIO_EXTREME_SECOND);
		pExtremSecondRadio->CreateSubControl("", TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y ,"",0,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX","CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO","CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO");
		RegisterTapButton(pExtremSecondRadio);
		RegisterControl(pExtremSecondRadio);

		EventSelectedTap( !m_pGaeaClient->GetCharacter()->IsUseArmSub() ? RADIO_EXTREME_FIRST : RADIO_EXTREME_SECOND);
	}
	else
	{
		CreateSlotHelper(ITEM_IMAGE_LEFT_WEAPON,"CHARACTER_WINDOW_WEAR_SLOT_LEFT_WEAPON",ITEM_IMAGE_LEFT_WEAPON,ITEM_COLOR_LEFT_WEAPON);
		CreateSlotHelper(ITEM_IMAGE_RIGHT_WEAPON,"CHARACTER_WINDOW_WEAR_SLOT_RIGHT_WEAPON",ITEM_IMAGE_RIGHT_WEAPON,ITEM_COLOR_RIGHT_WEAPON);
	}

	CreateSlotHelper(ITEM_IMAGE_VEHICLE,"CHARACTER_WINDOW_WEAR_SLOT_VEHICLE",ITEM_IMAGE_VEHICLE,ITEM_COLOR_VEHICLE);


	m_pMouseOver = CreateControl ( "ITEM_MOUSE_OVER" );

}

void RnCharacterWearWindow::UpdateCharacterView()
{
	if( m_pCharacterView->GetDevice() )
		m_pCharacterView->SetUpdateCharacter(m_pGaeaClient->GetCharacter(),true);
}

void RnCharacterWearWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
//	m_nIndex =  nOUTOFRANGE;
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	m_pCharacterView->SetUpdateCharacter(pCharacter,false);
	
	if ( m_pMouseOver ) 
		m_pMouseOver->SetVisibleSingle ( FALSE );

	if( IsExtreme() )
		EventSelectedTap( !m_pGaeaClient->GetCharacter()->IsUseArmSub() ? RADIO_EXTREME_FIRST : RADIO_EXTREME_SECOND);

	SNATIVEID sHOLD_ITEM_ID(false);

	//	스냅, 스킬 이미지 붙이기
	IItemMove* pItemMove = m_pInterface->GetItemMove();
	if ( pItemMove )
		sHOLD_ITEM_ID = pItemMove->GetItem();

	if( sHOLD_ITEM_ID == NATIVEID_NULL() )
	{
		IInventory* pInventory = m_pInterface->GetInventory();
		if( pInventory )
		{
			if( pInventory->GetTapSelected() == CRnInventoryWindow::ETapInven )
			{
				const SINVENITEM* pInvenItem = pInventory->GetFocusItem();
				if( pInvenItem && pInvenItem->GetNativeID() != NATIVEID_NULL() )
					sHOLD_ITEM_ID = pInvenItem->Id();
			}
		}
	}

	for ( int i = 0; i < ITEM_IMAGE_SIZE; i++ )
	{
		const SITEMCUSTOM& sItemCustomOld = GetItem ( i );

		EMSLOT emSlot = IMAGE2EMSLOT( i + ITEM_IMAGE_HEAD );
		if( SLOT_TSIZE == emSlot )
			continue;

		if( m_pItemImage[i] == NULL)
			continue;

		SITEMCUSTOM sItemCustom = pCharacter->GET_SLOT_ITEM ( emSlot );
		//	NOTE
		//		이전 프레임과 비교하여,
		//		데이타가 달라진 경우에만,
		//		로드/언로드 작업을 진행한다.
		if ( sItemCustom != sItemCustomOld )
		{		
			if ( sItemCustom.GetNativeID() != NATIVEID_NULL () )
			{
				LoadItem ( i, sItemCustom );
			}
			else
			{
				UnLoadItem ( i );
			}
		}


		switch(emSlot) // 캐릭터 창에 아이템 연마 수치 표시
		{
		case SLOT_HEADGEAR:
		case SLOT_UPPER:
		case SLOT_LOWER:
		case SLOT_HAND:
		case SLOT_FOOT:
		case SLOT_RHAND:
		case SLOT_RHAND_S:
        case SLOT_RFINGER:
        case SLOT_LFINGER:
        case SLOT_WAISTBAND:
        case SLOT_EARRINGS:
        case SLOT_DECORATION:
        case SLOT_RACCESSORIES:
        case SLOT_LACCESSORIES:		
			{
				if( sItemCustom.GetNativeID() != NATIVEID_NULL () )
				{
					BYTE grade = sItemCustom.GETGRADE(EMGRINDING_NUNE);
			
					int selectIndex = -1;
					for (int k = 0; k < ITEM_INFOR_COLOR::ITEM_GRIND::TOTAL_COUNT; ++k)
					{
						GASSERT(ITEM_INFOR_COLOR::dwItemGrind[k].RangeStart <= ITEM_INFOR_COLOR::dwItemGrind[k].RangeEnd);

						if( grade >= ITEM_INFOR_COLOR::dwItemGrind[k].RangeStart && 
							grade <= ITEM_INFOR_COLOR::dwItemGrind[k].RangeEnd )
						{
							selectIndex = k;
							break;
						}
					}
					
					GASSERT( selectIndex != -1);
					CString strCombine;
					strCombine.Format( "+%d", grade);
					m_pItemGradeText[i]->SetText( strCombine, ITEM_INFOR_COLOR::dwItemGrind[selectIndex].GrindColor);
				}
				break;
			}
		}

		m_pSlotDisplay[i]->SetVisibleSingle ( FALSE );

		//	NOTE
		//		장착될 위치 표시
		if ( sHOLD_ITEM_ID != NATIVEID_NULL () /*&& !bFOUND_TOWEAR*/ )
		{
			if ( EMSLOTCHECK_OK == pCharacter->CHECKSLOT_ITEM ( sHOLD_ITEM_ID, emSlot ) )
			{
				const UIRECT& rcImagePos = m_pItemImage[i]->GetGlobalPos ();
				D3DXVECTOR2 vPos ( rcImagePos.left, rcImagePos.top );

				if ( pCharacter->ACCEPT_ITEM ( sHOLD_ITEM_ID ) )
				{
					m_pSlotDisplay[i]->SetVisibleSingle ( TRUE );
				}

				//				bFOUND_TOWEAR = true;
			}
		}

		// Note : 코스츔 염색 컬러 정보. by luxes.
		if ( sItemCustom.GetNativeID() != NATIVEID_NULL () )
		{
			if ( sItemCustom.dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
				&& sItemCustom.dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
			{
				m_pColorInfo[ i ]->SetVisibleSingle( TRUE );

				m_pColorInfo[ i ]->SetSelectColor( sItemCustom.dwMainColor, sItemCustom.dwSubColor );
			}
			else
			{
				m_pColorInfo[ i ]->SetVisibleSingle( FALSE );
			}
		}
		else
			m_pColorInfo[ i ]->SetVisibleSingle( FALSE );
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void RnCharacterWearWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{

	switch ( ControlID )
	{
	case ITEM_IMAGE_HEAD:
	case ITEM_IMAGE_UPPER:
	case ITEM_IMAGE_WAIST:
	case ITEM_IMAGE_LOWER:
	case ITEM_IMAGE_HAND:
	case ITEM_IMAGE_FOOT:
	case ITEM_IMAGE_LEFT_ACCESSORY:
	case ITEM_IMAGE_NECK:
	case ITEM_IMAGE_WRIST:
	case ITEM_IMAGE_LEFT_FINGER:
	case ITEM_IMAGE_RIGHT_FINGER:
	case ITEM_IMAGE_EARRINGS:
	case ITEM_IMAGE_DECORATION:
	case ITEM_IMAGE_RIGHT_ACCESSORY:
	case ITEM_IMAGE_LEFT_WEAPON:
	case ITEM_IMAGE_RIGHT_WEAPON:
	case ITEM_IMAGE_LEFT_WEAPON_S:
	case ITEM_IMAGE_RIGHT_WEAPON_S:
	case ITEM_IMAGE_VEHICLE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nIndex = ControlID - ITEM_IMAGE_HEAD;
				//m_nIndex =  nIndex;

				SITEMCUSTOM sItemCustom = GetItem( nIndex );
				if ( sItemCustom.GetNativeID() != NATIVEID_NULL() )
				{
					if( ( ControlID == ITEM_IMAGE_RIGHT_WEAPON || ControlID == ITEM_IMAGE_LEFT_WEAPON || ControlID == ITEM_IMAGE_LEFT_WEAPON_S || ControlID == ITEM_IMAGE_RIGHT_WEAPON_S )
						&& GLUseFeatures::GetInstance().IsUsingItemDurability() && ItemDurability::GetInstance().IsUse(sItemCustom.GetNativeID()) )
					{
						m_pInterface->SHOW_ITEM_INFO_DURABILITY(sItemCustom,
							FALSE, FALSE, FALSE, FALSE, FALSE,
							USHRT_MAX, USHRT_MAX );
					}
					else
					{
						m_pInterface->SHOW_ITEM_INFO(
							sItemCustom,
							FALSE, FALSE, FALSE, FALSE, FALSE,
							USHRT_MAX, USHRT_MAX );
					}					
				}

				if ( !m_pInterface->IsFirstItemSlot () ) 
					return ;

				//	스냅, 스킬 이미지 붙이기
				IItemMove* pItemMove = m_pInterface->GetItemMove();
				if ( !pItemMove )
				{
					GASSERT ( 0 && "CItemMove가 널입니다." );
					return ;
				}
				//////////////////////////////////////////////////////////////////////////

				if ( (dwMsg & UIMSG_LB_UP) )
				{
// 					if ( m_bLock )
// 					{
// 						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("INVEN_LOCK_MSG") );
// 						return ;
// 					}

					EMSLOT emSlot = IMAGE2EMSLOT ( nIndex + ITEM_IMAGE_HEAD );
					m_pGaeaClient->GetCharacter()->ReqSlotTo( emSlot );
				}

				if ( CHECK_RB_UP_LIKE(dwMsg) && (sItemCustom.GetNativeID() != NATIVEID_NULL()) )
				{
					const BOOL bVirtualKeyDown(
						DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED
						|| DxInputDevice::GetInstance().GetKeyState( DIK_LSHIFT ) & DXKEY_PRESSED
						|| DxInputDevice::GetInstance().GetKeyState( DIK_LALT ) & DXKEY_PRESSED);
					if ( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED )
					{						
						EMSLOT emSlot = IMAGE2EMSLOT(nIndex + ITEM_IMAGE_HEAD);

						if ( m_pGaeaClient->GetCharacter()->VALID_SLOT_ITEM(emSlot) ) 
						{
							SLINK_DATA_BASIC sLinkDataBasic;
							sLinkDataBasic.sLinkType.wMType = ITEM_LINK;
							sLinkDataBasic.sLinkType.wSType = ITEM_LINK_PUTON;
							sLinkDataBasic.dwData1 = emSlot;
							m_pInterface->ChatToLink(sLinkDataBasic, &sItemCustom);
							return;
						}
					}
// 					else if ( DxInputDevice::GetInstance().GetKeyState( DIK_LALT ) & DXKEY_PRESSED )
// 					{
// 						const SITEM* pItemData = GLogicData::GetInstance().GetItem ( m_sNativeID );
// 						if ( pItemData == NULL )
// 							break;
// 
// 						SSETITEM* pSetItem = const_cast<SSETITEM*>(GLItemSet::GetInstance().GetInfoSetItem(pItemData->BasicId()));
// 						if ( pSetItem )
// 						{
// 							m_pGaeaClient->GetInterface()->SetitemGroupComposition(pSetItem);
// 							m_pGaeaClient->GetInterface()->OpenSetitemPreviewWindow();
// 							ResetMessageEx();
// 						}
// 					}
					else if ( bVirtualKeyDown == FALSE )// 코스튬 염색을 위한 호출. by luxes;
					{
						EMSLOT emSlot = IMAGE2EMSLOT(nIndex + ITEM_IMAGE_HEAD);
						m_pGaeaClient->GetCharacter()->ReqSlotConsume( emSlot );
						return;
					}
				}

				//////////////////////////////////////////////////////////////////////////

				if ( pItemMove->GetItem () != NATIVEID_NULL () )
				{
					const UIRECT& rcSlotPos = m_pItemImage[nIndex]->GetGlobalPos();
					pItemMove->SetUIWindowGlobalPos( rcSlotPos );						

					//AddMessageEx( UIMSG_MOUSEIN_WEARSLOTEX | UIMSG_TOTOPPARENT );
					m_pInterface->SetSnapItem();
				}				

				//	마우스 표시 테두리
				if ( pItemMove->GetItem () == NATIVEID_NULL () && m_pItemImage[nIndex]->GetItemImageSlotPos () != NATIVEID_NULL () )
				{
					const UIRECT& rcImagePos = m_pItemImage[nIndex]->GetGlobalPos ();
					m_pMouseOver->SetGlobalPos( rcImagePos );
					m_pMouseOver->SetVisibleSingle( TRUE );
				}

				//////////////////////////////////////////////////////////////////////////
			}
		}
		break;
	}



	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );
}

// CItemImage*	RnCharacterWearWindow::CreateItemImage( const char* szControl, UIGUID ControlID )
// {
// 	CItemImage* pItemImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
// 	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
// 	pItemImage->CreateSubControl ();
// 	RegisterControl ( pItemImage );
// 
// 	return pItemImage;
// }

CCostumColorInfo* RnCharacterWearWindow::CreateColorInfo( const char* szControl, UIGUID ControlID )
{

	CCostumColorInfo* pItemColorInfo = new CCostumColorInfo(m_pEngineDevice);
	pItemColorInfo->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemColorInfo->CreateSubControl ();
	RegisterControl ( pItemColorInfo );

	return pItemColorInfo;
}

void RnCharacterWearWindow::CreateSlotHelper(int index,const std::string& ItemImageKey, UIGUID ItemImageID,UIGUID ItemColorID)
{
	index -= ITEM_IMAGE_HEAD;

	GASSERT( index >=0 && index < ITEM_IMAGE_SIZE);

	m_pItemImage[index]   = CreateItemImage ( ItemImageKey.c_str(), ItemImageID );
	m_pSlotDisplay[index] = CreateControl( (ItemImageKey + "_OVER").c_str() );
	m_pSlotDisplay[index]->SetVisibleSingle ( FALSE );

//	m_pColorInfo[index] = CreateColorInfo(  (ItemImageKey + "_COLOR").c_str(), ItemColorID );
	m_pColorInfo[index] = CreateColorInfo(  ItemImageKey.c_str(), ItemColorID );
	m_pColorInfo[index]->SetUseRender(FALSE);
	m_pColorInfo[index]->SetVisibleSingle( FALSE );

	m_pItemImageLocalPosRect[index] = m_pColorInfo[index]->GetLocalPos();
	m_pItemImageGlobalPosRect[index] = m_pColorInfo[index]->GetGlobalPos();

	m_pItemGradeText[index] = CreateText("",ItemImageKey,TEXT_ALIGN_RIGHT|TEXT_ALIGN_BOTTOM);
	m_pItemGradeText[index]->SetUseRender(FALSE);
}


EMSLOT RnCharacterWearWindow::IMAGE2EMSLOT( int nIndex )
{
	switch ( nIndex )
	{	
	case ITEM_IMAGE_HEAD:	return SLOT_HEADGEAR;	//	머리
	case ITEM_IMAGE_NECK:	return SLOT_NECK;		//	목걸이
	case ITEM_IMAGE_UPPER:	return SLOT_UPPER;		//	상의
	case ITEM_IMAGE_LEFT_WEAPON:	return SLOT_LHAND;		//	왼손도구
	case ITEM_IMAGE_WRIST:	return SLOT_WRIST;		//	손목
	case ITEM_IMAGE_HAND:	return SLOT_HAND;		//	장갑
	case ITEM_IMAGE_LOWER:	return SLOT_LOWER;		//	하의
	case ITEM_IMAGE_LEFT_FINGER:	return SLOT_LFINGER;	//	왼손 손가락
	case ITEM_IMAGE_RIGHT_WEAPON:	return SLOT_RHAND;		//	오른손도구
	case ITEM_IMAGE_FOOT:	return SLOT_FOOT;		//	신발
	case ITEM_IMAGE_RIGHT_FINGER:	return SLOT_RFINGER;	//	오른손 손가락	
	case ITEM_IMAGE_LEFT_WEAPON_S:	return SLOT_LHAND_S;	//	오른손도구, 극강부
	case ITEM_IMAGE_RIGHT_WEAPON_S:	return SLOT_RHAND_S;	//	왼손도구, 극강부
	case ITEM_IMAGE_VEHICLE:	return SLOT_VEHICLE;	//  탈것
	
		// 차후 구현될것들
	case ITEM_IMAGE_WAIST:          return SLOT_WAISTBAND;
    case ITEM_IMAGE_LEFT_ACCESSORY: return SLOT_LACCESSORIES;
	case ITEM_IMAGE_RIGHT_ACCESSORY:    return SLOT_RACCESSORIES;
	case ITEM_IMAGE_EARRINGS:   return SLOT_EARRINGS;
	case ITEM_IMAGE_DECORATION:  return SLOT_DECORATION;
		break;
	}

	return SLOT_TSIZE;
}

SITEMCUSTOM& RnCharacterWearWindow::GetItem( int nIndex )
{
	return m_ItemCustomArray[nIndex];
}


void RnCharacterWearWindow::LoadItem( int nIndex, SITEMCUSTOM& ref_sItemCustom )
{
	m_ItemCustomArray[nIndex] = ref_sItemCustom;

	m_pItemImage[nIndex]->SetItem ( ref_sItemCustom );

	SITEMCUSTOM &sItemCustom = ref_sItemCustom;

	// Note : 코스츔 염색 컬러 정보. by luxes.
	if ( sItemCustom.dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
		&& sItemCustom.dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
	{
		m_pColorInfo[ nIndex ]->SetVisibleSingle( TRUE );

		m_pColorInfo[ nIndex ]->SetSelectColor( sItemCustom.dwMainColor, sItemCustom.dwSubColor );
	}

	m_pItemGradeText[nIndex]->SetVisibleSingle(TRUE);
}

void RnCharacterWearWindow::UnLoadItem( int nIndex )
{
	m_ItemCustomArray[nIndex] = SITEMCUSTOM( NATIVEID_NULL() );
	m_pItemImage[nIndex]->ResetItem ();

	// Note : 코스츔 염색 컬러 정보. by luxes.
	if ( m_ItemCustomArray[nIndex].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
		&& m_ItemCustomArray[nIndex].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
	{
		m_pColorInfo[ nIndex ]->SetVisibleSingle( FALSE );
	}

	m_pItemGradeText[nIndex]->SetVisibleSingle(FALSE);
}

void RnCharacterWearWindow::EventSelectedTap( UIGUID controlID )
{
	const DWORD DARK_COLOR = D3DCOLOR_XRGB(60,60,60);

// 	m_pItemImage[ITEM_IMAGE_LEFT_WEAPON-ITEM_IMAGE_HEAD]->SetNoUpdate(FALSE);
// 	m_pItemImage[ITEM_IMAGE_RIGHT_WEAPON-ITEM_IMAGE_HEAD]->SetNoUpdate(FALSE);
// 
// 	m_pItemImage[ITEM_IMAGE_LEFT_WEAPON_S-ITEM_IMAGE_HEAD]->SetNoUpdate(FALSE);
// 	m_pItemImage[ITEM_IMAGE_LEFT_WEAPON_S-ITEM_IMAGE_HEAD]->SetNoUpdate(FALSE);

	if( controlID == RADIO_EXTREME_FIRST )
	{
		if( m_pGaeaClient->GetCharacter()->IsUseArmSub() )
			m_pGaeaClient->GetCharacter()->ReqSlotChange();

		m_pItemImage[ITEM_IMAGE_LEFT_WEAPON-ITEM_IMAGE_HEAD]->SetDiffuse( NS_UITEXTCOLOR::WHITE);
		m_pItemImage[ITEM_IMAGE_RIGHT_WEAPON-ITEM_IMAGE_HEAD]->SetDiffuse( NS_UITEXTCOLOR::WHITE);
		m_pItemImage[ITEM_IMAGE_LEFT_WEAPON_S-ITEM_IMAGE_HEAD]->SetDiffuse( DARK_COLOR );
		m_pItemImage[ITEM_IMAGE_RIGHT_WEAPON_S-ITEM_IMAGE_HEAD]->SetDiffuse( DARK_COLOR );

// 		m_pItemImage[ITEM_IMAGE_LEFT_WEAPON_S-ITEM_IMAGE_HEAD]->SetNoUpdate(TRUE);
// 		m_pItemImage[ITEM_IMAGE_RIGHT_WEAPON_S-ITEM_IMAGE_HEAD]->SetNoUpdate(TRUE);
	}
	else if( controlID == RADIO_EXTREME_SECOND )
	{
		if( !m_pGaeaClient->GetCharacter()->IsUseArmSub() )
			m_pGaeaClient->GetCharacter()->ReqSlotChange();

		m_pItemImage[ITEM_IMAGE_LEFT_WEAPON-ITEM_IMAGE_HEAD]->SetDiffuse( DARK_COLOR );
		m_pItemImage[ITEM_IMAGE_RIGHT_WEAPON-ITEM_IMAGE_HEAD]->SetDiffuse( DARK_COLOR );
		m_pItemImage[ITEM_IMAGE_LEFT_WEAPON_S-ITEM_IMAGE_HEAD]->SetDiffuse( NS_UITEXTCOLOR::WHITE);
		m_pItemImage[ITEM_IMAGE_RIGHT_WEAPON_S-ITEM_IMAGE_HEAD]->SetDiffuse( NS_UITEXTCOLOR::WHITE);

// 		m_pItemImage[ITEM_IMAGE_LEFT_WEAPON-ITEM_IMAGE_HEAD]->SetNoUpdate(TRUE);
// 		m_pItemImage[ITEM_IMAGE_RIGHT_WEAPON-ITEM_IMAGE_HEAD]->SetNoUpdate(TRUE);
	}

}

HRESULT RnCharacterWearWindow::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT ret = CUIGroupHelper::RestoreDeviceObjects(pd3dDevice);


	for(int i = 0; i < ITEM_IMAGE_SIZE; ++i)
	{
		if( m_pColorInfo[i] == NULL )
			continue;

	
		m_pColorInfo[i]->SetLocalPos( D3DXVECTOR2(m_pItemImageLocalPosRect[i].left+3,m_pItemImageLocalPosRect[i].top+26));
		m_pColorInfo[i]->SetGlobalPos( D3DXVECTOR2(m_pItemImageGlobalPosRect[i].left+3,m_pItemImageGlobalPosRect[i].top+26));
	}

	return ret;
}

bool RnCharacterWearWindow::IsExtreme()
{
	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	const EMCHARCLASS emCharClass = pGlobalStage->GetGameStage()->GetCharJoinData()->m_CharData2.m_emClass;
	const int nClassType = CharClassToIndex( emCharClass );

	// 극강부와 일반 캐릭터를 구분해서 인벤토리 장착 윈도우를 생성한다.
	return ( nClassType == GLCI_EXTREME_W || nClassType == GLCI_EXTREME_M );
}

void RnCharacterWearWindow::DeleteUIMeshData()
{
	m_pCharacterView->DeleteUIMeshData();
}

void RnCharacterWearWindow::SetArmSwapTabButton()
{
	if( m_pGaeaClient->GetCharacter()->IsUseArmSub() )
		TapSelect( RADIO_EXTREME_SECOND );
	else
		TapSelect( RADIO_EXTREME_FIRST );
}