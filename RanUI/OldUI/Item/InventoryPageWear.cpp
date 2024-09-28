#include "StdAfx.h"
#include "InventoryPageWear.h"
#include "../Item/ItemImage.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../InnerInterface.h"
#include "ItemMove.h"
#include "../Feature/CostumColorInfo.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CInventoryPageWear::nOUTOFRANGE = -1;

CInventoryPageWear::CInventoryPageWear(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) 
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pMouseOver ( NULL )
{
}

CInventoryPageWear::~CInventoryPageWear ()
{
}

void CInventoryPageWear::CreateSubControl ( int nClassType )
{
    CString	strInvenWearBack[GLCI_NUM_ETC] = 
    {
        "INVENTORY_PAGE_WEAR_FIGHTER_M",
        "INVENTORY_PAGE_WEAR_ARMS_M",
        "INVENTORY_PAGE_WEAR_ARCHER_W",
        "INVENTORY_PAGE_WEAR_SPIRIT_W",
        "INVENTORY_PAGE_WEAR_EXTREME_M",
        "INVENTORY_PAGE_WEAR_EXTREME_W",
        "INVENTORY_PAGE_WEAR_FIGHTER_W",
        "INVENTORY_PAGE_WEAR_ARMS_W",
        "INVENTORY_PAGE_WEAR_ARCHER_M",
        "INVENTORY_PAGE_WEAR_SPIRIT_M",	
        "INVENTORY_PAGE_WEAR_SCIENTIST_M",
        "INVENTORY_PAGE_WEAR_SCIENTIST_W",	
        "INVENTORY_PAGE_WEAR_ASSASSIN_M",
        "INVENTORY_PAGE_WEAR_ASSASSIN_W",
		"INVENTORY_PAGE_WEAR_TRICKER_M",
		"INVENTORY_PAGE_WEAR_TRICKER_W",
        "INVENTORY_PAGE_WEAR_ETC_M",
        "INVENTORY_PAGE_WEAR_ETC_W",
    };

    CreateControl ( strInvenWearBack[nClassType].GetString () );

    {	//	아이템 이미지 ( 0 - 11 )
        //  아이템 추가시 uiinnercfg02.xml 파일 수정
        CString strInvenWearItem[ITEM_IMAGE_SIZE] = 
        {
            "INVENTORY_WEAR_ITEM_IMAGE0",
            "INVENTORY_WEAR_ITEM_IMAGE1",
            "INVENTORY_WEAR_ITEM_IMAGE2",
            "INVENTORY_WEAR_ITEM_IMAGE3",
            "INVENTORY_WEAR_ITEM_IMAGE4",
            "INVENTORY_WEAR_ITEM_IMAGE5",
            "INVENTORY_WEAR_ITEM_IMAGE6",
            "INVENTORY_WEAR_ITEM_IMAGE7",
            "INVENTORY_WEAR_ITEM_IMAGE8",
            "INVENTORY_WEAR_ITEM_IMAGE9",
            "INVENTORY_WEAR_ITEM_IMAGE10",
            "INVENTORY_WEAR_ITEM_IMAGE11",
            "INVENTORY_WEAR_ITEM_IMAGE12",
            "INVENTORY_WEAR_ITEM_IMAGE13",
            "INVENTORY_WEAR_ITEM_IMAGE14",
            "INVENTORY_WEAR_ITEM_IMAGE15",
            "INVENTORY_WEAR_ITEM_IMAGE16",
        };
         //  아이템 추가시 uiinnercfg02.xml 파일 수정
        CString strInvenWearOver[ITEM_IMAGE_SIZE] = 
        {
            "INVENTORY_WEAR_OVER_IMAGE0",
            "INVENTORY_WEAR_OVER_IMAGE1",
            "INVENTORY_WEAR_OVER_IMAGE2",
            "INVENTORY_WEAR_OVER_IMAGE3",
            "INVENTORY_WEAR_OVER_IMAGE4",
            "INVENTORY_WEAR_OVER_IMAGE5",
            "INVENTORY_WEAR_OVER_IMAGE6",
            "INVENTORY_WEAR_OVER_IMAGE7",
            "INVENTORY_WEAR_OVER_IMAGE8",
            "INVENTORY_WEAR_OVER_IMAGE9",
            "INVENTORY_WEAR_OVER_IMAGE10",
            "INVENTORY_WEAR_OVER_IMAGE11",
            "INVENTORY_WEAR_OVER_IMAGE12",
            "INVENTORY_WEAR_OVER_IMAGE13",
            "INVENTORY_WEAR_OVER_IMAGE14",
            "INVENTORY_WEAR_OVER_IMAGE15",
            "INVENTORY_WEAR_OVER_IMAGE16",
        };
        //  아이템 추가시 uiinnercfg03.xml 파일 수정
        CString	strColorBack[ITEM_IMAGE_SIZE] = 
        {
            "COLOR1_WEAR_INFO_BOX0",
            "COLOR1_WEAR_INFO_BOX1",
            "COLOR1_WEAR_INFO_BOX2",
            "COLOR1_WEAR_INFO_BOX3",
            "COLOR1_WEAR_INFO_BOX4",
            "COLOR1_WEAR_INFO_BOX5",
            "COLOR1_WEAR_INFO_BOX6",
            "COLOR1_WEAR_INFO_BOX7",
            "COLOR1_WEAR_INFO_BOX8",
            "COLOR1_WEAR_INFO_BOX9",
            "COLOR1_WEAR_INFO_BOX10",
            "COLOR1_WEAR_INFO_BOX11",
            "COLOR1_WEAR_INFO_BOX12",
            "COLOR1_WEAR_INFO_BOX13",
            "COLOR1_WEAR_INFO_BOX14",
            "COLOR1_WEAR_INFO_BOX15",
            "COLOR1_WEAR_INFO_BOX16",
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
    //	m_pSlotDisplay = CreateControl ( "TRADE_INVENTORY_OVER_0" );
    //	m_pSlotBlockDisplay = CreateControl ( "TRADE_OVER_0" );
}

CUIControl*	CInventoryPageWear::CreateControl ( const char* szControl )
{
    CUIControl* pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub ( this, szControl );
    RegisterControl ( pControl );

    return pControl;
}

CItemImage*	CInventoryPageWear::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
    CItemImage* pItemImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
    pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
    pItemImage->CreateSubControl ();
    RegisterControl ( pItemImage );

    return pItemImage;
}

CCostumColorInfo* CInventoryPageWear::CreateColorInfo( const char* szControl, UIGUID ControlID )
{

    CCostumColorInfo* pItemColorInfo = new CCostumColorInfo(m_pEngineDevice);
    pItemColorInfo->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
    pItemColorInfo->CreateSubControl ();
    RegisterControl ( pItemColorInfo );

    return pItemColorInfo;
}


EMSLOT CInventoryPageWear::IMAGE2EMSLOT ( int nIndex )
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
    case ITEM_IMAGE11:	return SLOT_VEHICLE;	// 탈것;
    case ITEM_IMAGE12:	return SLOT_WAISTBAND;  //  허리띠
    case ITEM_IMAGE13:	return SLOT_EARRINGS;   //  왼쪽 귀걸이
    case ITEM_IMAGE14:	return SLOT_DECORATION;   //  오른쪽 귀걸이
    case ITEM_IMAGE15:	return SLOT_LACCESSORIES;// 왼쪽 장신구
    case ITEM_IMAGE16:	return SLOT_RACCESSORIES;// 오른쪽 장신구
    }

    GASSERT ( 0 && "범위를 벗어납니다." );
    return SLOT_TSIZE;
}

void CInventoryPageWear::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    SetItemIndex ( nOUTOFRANGE );
    if ( m_pMouseOver ) m_pMouseOver->SetVisibleSingle ( FALSE );
    //	if ( m_pSlotDisplay ) m_pSlotDisplay->SetVisibleSingle ( FALSE );
    //	if ( m_pSlotBlockDisplay ) m_pSlotBlockDisplay->SetVisibleSingle ( FALSE );

    //	스냅, 스킬 이미지 붙이기
    IItemMove* pItemMove = m_pInterface->GetItemMove();
    SNATIVEID sHOLD_ITEM_ID(false);
    //	bool bFOUND_TOWEAR = false;
    if ( pItemMove ) sHOLD_ITEM_ID = pItemMove->GetItem();

    for ( int i = 0; i < SLOT_NSIZE_2; i++ )
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

void CInventoryPageWear::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
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
    case ITEM_IMAGE12:
    case ITEM_IMAGE13:
    case ITEM_IMAGE14:
    case ITEM_IMAGE15:
    case ITEM_IMAGE16:
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

                    AddMessageEx ( UIMSG_MOUSEIN_WEARSLOT | UIMSG_TOTOPPARENT );
                }				

                //	마우스 표시 테두리
                if ( pItemMove->GetItem () == NATIVEID_NULL () && m_pItemImage[m_nIndex]->GetItemImageSlotPos () != NATIVEID_NULL () )
                {
                    const UIRECT& rcImagePos = m_pItemImage[nIndex]->GetGlobalPos();
                    m_pMouseOver->SetGlobalPos( rcImagePos );
                    m_pMouseOver->SetVisibleSingle( TRUE );
                }
            }
        }
        break;
    }
}

EMSLOT CInventoryPageWear::GetItemSlot ()
{
    return IMAGE2EMSLOT ( GetItemIndex () + ITEM_IMAGE0 );
}

void CInventoryPageWear::LoadItem ( int nIndex, SITEMCUSTOM& ref_sItemCustom )
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

SITEMCUSTOM& CInventoryPageWear::GetItem ( int nIndex )
{
    return m_ItemCustomArray[nIndex];
}

void CInventoryPageWear::UnLoadItem ( int nIndex )
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
