#include "StdAfx.h"
#include "InventoryPageWearEx.h"
#include "../Item/ItemImage.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../InnerInterface.h"
#include "../Item/ItemMove.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CInventoryPageWearEx::nOUTOFRANGE = -1;

CInventoryPageWearEx::CInventoryPageWearEx(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
	, m_pMouseOver(NULL)
	, m_pAButton(NULL)
	, m_pBButton(NULL)
	, m_pARHandSlot(NULL)
	, m_pALHandSlot(NULL)
	, m_pBRHandSlot(NULL)
	, m_pBLHandSlot(NULL)
{
}

CInventoryPageWearEx::~CInventoryPageWearEx ()
{
}

void CInventoryPageWearEx::CreateSubControl ( int nClassType )
{
	CString	strInvenWearBack[GLCI_NUM_EX-GLCI_NUM] = 
	{
		"INVENTORY_PAGE_WEAR_EXTREME_M",
		"INVENTORY_PAGE_WEAR_EXTREME_W",
	};

	CreateControl ( strInvenWearBack[nClassType-GLCI_NUM].GetString () );

	{	//	아이템 이미지 ( 0 - 11 )
        //  아이템 추가시 uiinnercfg02.xml 파일 수정
		CString strInvenWearItem[ITEM_IMAGE_SIZE] = 
		{
			"INVENTORY_WEAR_EX_ITEM_IMAGE0",
			"INVENTORY_WEAR_EX_ITEM_IMAGE1",
			"INVENTORY_WEAR_EX_ITEM_IMAGE2",
			"INVENTORY_WEAR_EX_ITEM_IMAGE3",
			"INVENTORY_WEAR_EX_ITEM_IMAGE4",
			"INVENTORY_WEAR_EX_ITEM_IMAGE5",
			"INVENTORY_WEAR_EX_ITEM_IMAGE6",
			"INVENTORY_WEAR_EX_ITEM_IMAGE7",
			"INVENTORY_WEAR_EX_ITEM_IMAGE8",
			"INVENTORY_WEAR_EX_ITEM_IMAGE9",
			"INVENTORY_WEAR_EX_ITEM_IMAGE10",
			"INVENTORY_WEAR_EX_ITEM_IMAGE11",
			"INVENTORY_WEAR_EX_ITEM_IMAGE12",
			"INVENTORY_WEAR_EX_ITEM_IMAGE13",
            "INVENTORY_WEAR_EX_ITEM_IMAGE14",
            "INVENTORY_WEAR_EX_ITEM_IMAGE15",
            "INVENTORY_WEAR_EX_ITEM_IMAGE16",
            "INVENTORY_WEAR_EX_ITEM_IMAGE17",
            "INVENTORY_WEAR_EX_ITEM_IMAGE18",
		};
        //  아이템 추가시 uiinnercfg02.xml 파일 수정
		CString strInvenWearOver[ITEM_IMAGE_SIZE] = 
		{
			"INVENTORY_WEAR_EX_OVER_IMAGE0",
			"INVENTORY_WEAR_EX_OVER_IMAGE1",
			"INVENTORY_WEAR_EX_OVER_IMAGE2",
			"INVENTORY_WEAR_EX_OVER_IMAGE3",
			"INVENTORY_WEAR_EX_OVER_IMAGE4",
			"INVENTORY_WEAR_EX_OVER_IMAGE5",
			"INVENTORY_WEAR_EX_OVER_IMAGE6",
			"INVENTORY_WEAR_EX_OVER_IMAGE7",
			"INVENTORY_WEAR_EX_OVER_IMAGE8",
			"INVENTORY_WEAR_EX_OVER_IMAGE9",
			"INVENTORY_WEAR_EX_OVER_IMAGE10",
			"INVENTORY_WEAR_EX_OVER_IMAGE11",
			"INVENTORY_WEAR_EX_OVER_IMAGE12",
			"INVENTORY_WEAR_EX_OVER_IMAGE13",
            "INVENTORY_WEAR_EX_OVER_IMAGE14",
            "INVENTORY_WEAR_EX_OVER_IMAGE15",
            "INVENTORY_WEAR_EX_OVER_IMAGE16",
            "INVENTORY_WEAR_EX_OVER_IMAGE17",
            "INVENTORY_WEAR_EX_OVER_IMAGE18",
		};
        //  아이템 추가시 uiinnercfg03.xml 파일 수정
		CString strColorBack[ITEM_IMAGE_SIZE] = 
		{
			"COLOR_WEAR_INFO_EX_BOX0",
			"COLOR_WEAR_INFO_EX_BOX1",
			"COLOR_WEAR_INFO_EX_BOX2",
			"COLOR_WEAR_INFO_EX_BOX3",
			"COLOR_WEAR_INFO_EX_BOX4",
			"COLOR_WEAR_INFO_EX_BOX5",
			"COLOR_WEAR_INFO_EX_BOX6",
			"COLOR_WEAR_INFO_EX_BOX7",
			"COLOR_WEAR_INFO_EX_BOX8",
			"COLOR_WEAR_INFO_EX_BOX9",
			"COLOR_WEAR_INFO_EX_BOX10",
			"COLOR_WEAR_INFO_EX_BOX11",
			"COLOR_WEAR_INFO_EX_BOX12",
			"COLOR_WEAR_INFO_EX_BOX13",
            "COLOR_WEAR_INFO_EX_BOX14",
            "COLOR_WEAR_INFO_EX_BOX15",
            "COLOR_WEAR_INFO_EX_BOX16",
            "COLOR_WEAR_INFO_EX_BOX17",
            "COLOR_WEAR_INFO_EX_BOX18",
		};

		for ( int i = 0; i < ITEM_IMAGE_SIZE; i++ )
		{
			m_pItemImage[i] = CreateItemImage ( strInvenWearItem[i].GetString (), ITEM_IMAGE0 + i );
			m_pSlotDisplay[i] = CreateControl ( strInvenWearOver[i].GetString () );
			m_pSlotDisplay[i]->SetVisibleSingle ( FALSE );

			m_pColorInfo[ i ] = CreateColorInfo( strColorBack[i].GetString(), ITEM_COLOR_INFO0 + i );
			m_pColorInfo[ i ]->SetVisibleSingle( FALSE );
		}
	}

	m_pMouseOver = CreateControl ( "ITEM_MOUSE_OVER" );

	m_pAButton = new CBasicTextButton(m_pEngineDevice);
	m_pAButton->CreateSub ( this, "BASIC_TEXT_BUTTON161", UI_FLAG_XSIZE, TEXT_A_BUTTON );
	m_pAButton->CreateBaseButton(	"INVENTORY_TAB_BUTTON_A", 
									CBasicTextButton::SIZE16s, 
									CBasicButton::RADIO_FLIP, 
									(char*)ID2GAMEWORD("INVEN_TAB_BUTTON",0) );
	m_pAButton->SetFlip ( TRUE );
	RegisterControl ( m_pAButton );

	m_pBButton = new CBasicTextButton(m_pEngineDevice);
	m_pBButton->CreateSub ( this, "BASIC_TEXT_BUTTON161", UI_FLAG_XSIZE, TEXT_B_BUTTON );
	m_pBButton->CreateBaseButton(	"INVENTORY_TAB_BUTTON_B", 
									CBasicTextButton::SIZE16s, 
									CBasicButton::RADIO_FLIP, 
									(char*)ID2GAMEWORD("INVEN_TAB_BUTTON",1) );
	m_pBButton->SetFlip ( TRUE );
	RegisterControl ( m_pBButton );

	m_pARHandSlot = CreateControl("INVENTORY_USE_SLOT_RHAND_A");
	m_pALHandSlot = CreateControl("INVENTORY_USE_SLOT_LHAND_A");
	m_pBRHandSlot = CreateControl("INVENTORY_USE_SLOT_RHAND_B");
	m_pBLHandSlot = CreateControl("INVENTORY_USE_SLOT_LHAND_B");
}

CUIControl*	CInventoryPageWearEx::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

CItemImage*	CInventoryPageWearEx::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}

CCostumColorInfo* CInventoryPageWearEx::CreateColorInfo( const char* szControl, UIGUID ControlID )
{

	CCostumColorInfo* pItemColorInfo = new CCostumColorInfo(m_pEngineDevice);
	pItemColorInfo->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemColorInfo->CreateSubControl ();
	RegisterControl ( pItemColorInfo );

	return pItemColorInfo;
}

EMSLOT CInventoryPageWearEx::IMAGE2EMSLOT ( int nIndex )
{
	switch ( nIndex )
	{	
	case ITEM_IMAGE0:	return SLOT_HEADGEAR;	//	머리
	case ITEM_IMAGE1:	return SLOT_NECK;		//	목걸이
	case ITEM_IMAGE2:	return SLOT_UPPER;		//	상의
	case ITEM_IMAGE3:	return SLOT_LHAND;		//	왼손도구
	case ITEM_IMAGE4:	return SLOT_WRIST;		//	손목
	case ITEM_IMAGE5:	return SLOT_HAND;		//	장갑
	case ITEM_IMAGE6:	return SLOT_LOWER;		//	하의
	case ITEM_IMAGE7:	return SLOT_LFINGER;	//	왼손 손가락
	case ITEM_IMAGE8:	return SLOT_RHAND;		//	오른손도구
	case ITEM_IMAGE9:	return SLOT_FOOT;		//	신발
	case ITEM_IMAGE10:	return SLOT_RFINGER;	//	오른손 손가락	
	case ITEM_IMAGE11:	return SLOT_RHAND_S;	//	오른손도구, 극강부
	case ITEM_IMAGE12:	return SLOT_LHAND_S;	//	왼손도구, 극강부
	case ITEM_IMAGE13:	return SLOT_VEHICLE;	//  탈것
    case ITEM_IMAGE14:	return SLOT_WAISTBAND;	//  탈것
    case ITEM_IMAGE15:	return SLOT_DECORATION;	//  탈것
    case ITEM_IMAGE16:	return SLOT_EARRINGS;	//  탈것
    case ITEM_IMAGE17:	return SLOT_RACCESSORIES;	//  탈것
    case ITEM_IMAGE18:	return SLOT_LACCESSORIES;	//  탈것
	}

	GASSERT ( 0 && "범위를 벗어납니다." );
	return SLOT_TSIZE;
}

void CInventoryPageWearEx::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetItemIndex ( nOUTOFRANGE );
	if ( m_pMouseOver ) m_pMouseOver->SetVisibleSingle ( FALSE );

	//	스냅, 스킬 이미지 붙이기
	IItemMove* pItemMove = m_pInterface->GetItemMove();
	SNATIVEID sHOLD_ITEM_ID(false);
	if ( pItemMove ) sHOLD_ITEM_ID = pItemMove->GetItem();


	for ( int i = 0; i < SLOT_NSIZE_S_2; i++ )
	{
		const SITEMCUSTOM& sItemCustomOld = GetItem ( i );

		EMSLOT emSlot = IMAGE2EMSLOT ( i + ITEM_IMAGE0 );
		SITEMCUSTOM sItemCustom = m_pGaeaClient->GetCharacter()->GET_SLOT_ITEM ( emSlot );

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

		m_pSlotDisplay[i]->SetVisibleSingle ( FALSE );

		//	NOTE
		//		장착될 위치 표시
		if ( sHOLD_ITEM_ID != NATIVEID_NULL () /*&& !bFOUND_TOWEAR*/ )
		{
			if ( EMSLOTCHECK_OK == m_pGaeaClient->GetCharacter()->CHECKSLOT_ITEM ( sHOLD_ITEM_ID, emSlot ) )
			{
				const UIRECT& rcImagePos = m_pItemImage[i]->GetGlobalPos ();
				D3DXVECTOR2 vPos ( rcImagePos.left, rcImagePos.top );

				if ( m_pGaeaClient->GetCharacter()->ACCEPT_ITEM ( sItemCustom ) )
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

void CInventoryPageWearEx::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ITEM_IMAGE0:
	case ITEM_IMAGE1:
	case ITEM_IMAGE2:
	case ITEM_IMAGE3:
	case ITEM_IMAGE4:
	case ITEM_IMAGE5:
	case ITEM_IMAGE6:
	case ITEM_IMAGE7:
	case ITEM_IMAGE8:
	case ITEM_IMAGE9:
	case ITEM_IMAGE10:	
	case ITEM_IMAGE11:	
	case ITEM_IMAGE12:	
	case ITEM_IMAGE13:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nIndex = ControlID - ITEM_IMAGE0;
				SetItemIndex ( nIndex );

				if ( !m_pInterface->IsFirstItemSlot () ) return ;

				//	스냅, 스킬 이미지 붙이기
				IItemMove* pItemMove = m_pInterface->GetItemMove();
				if ( !pItemMove )
				{
					GASSERT ( 0 && "CItemMove가 널입니다." );
					return ;
				}

				if ( pItemMove->GetItem () != NATIVEID_NULL () )
				{
					const UIRECT& rcSlotPos = m_pItemImage[nIndex]->GetGlobalPos();
					pItemMove->SetUIWindowGlobalPos( rcSlotPos );						

					AddMessageEx( UIMSG_MOUSEIN_WEARSLOTEX | UIMSG_TOTOPPARENT );
				}				

				//	마우스 표시 테두리
				if ( pItemMove->GetItem () == NATIVEID_NULL () && m_pItemImage[m_nIndex]->GetItemImageSlotPos () != NATIVEID_NULL () )
				{
					const UIRECT& rcImagePos = m_pItemImage[nIndex]->GetGlobalPos ();
					m_pMouseOver->SetGlobalPos( rcImagePos );
					m_pMouseOver->SetVisibleSingle( TRUE );
				}
			}
		}
		break;

	case TEXT_A_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pGaeaClient->GetCharacter()->ReqSlotChange();
			}
		}
		break;

	case TEXT_B_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pGaeaClient->GetCharacter()->ReqSlotChange();
			}
		}
		break;
	}
}

EMSLOT CInventoryPageWearEx::GetItemSlot ()
{
	return IMAGE2EMSLOT ( GetItemIndex () + ITEM_IMAGE0 );
}

void CInventoryPageWearEx::LoadItem ( int nIndex, SITEMCUSTOM& ref_sItemCustom )
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
}

SITEMCUSTOM& CInventoryPageWearEx::GetItem ( int nIndex )
{
	return m_ItemCustomArray[nIndex];
}

void CInventoryPageWearEx::UnLoadItem ( int nIndex )
{
	m_ItemCustomArray[nIndex] = SITEMCUSTOM( NATIVEID_NULL() );
	m_pItemImage[nIndex]->ResetItem ();

	// Note : 코스츔 염색 컬러 정보. by luxes.
	if ( m_ItemCustomArray[nIndex].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
		&& m_ItemCustomArray[nIndex].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
	{
		m_pColorInfo[ nIndex ]->SetVisibleSingle( FALSE );
	}
}

void CInventoryPageWearEx::SetTabButton( BOOL bMain )
{
	if( bMain )
	{
		m_pAButton->SetFlip( TRUE );
		m_pBButton->SetFlip( FALSE );

		m_pARHandSlot->SetVisibleSingle( FALSE );
		m_pALHandSlot->SetVisibleSingle( FALSE );
		m_pBRHandSlot->SetVisibleSingle( TRUE );
		m_pBLHandSlot->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pAButton->SetFlip( FALSE );
		m_pBButton->SetFlip( TRUE );

		m_pARHandSlot->SetVisibleSingle( TRUE );
		m_pALHandSlot->SetVisibleSingle( TRUE );
		m_pBRHandSlot->SetVisibleSingle( FALSE );
		m_pBLHandSlot->SetVisibleSingle( FALSE );
	}
}
