#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../RanLogic/Item/GLItemMan.h"

#include "../../../RanLogicClient/Club/GLClubClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/StaticHoldItem.h"

#include "../../InnerInterface.h"
#include "../Item/ItemSlot.h"
#include "./WbClubStoragePage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CWbClubStoragePage::nSTARTLINE = 0;
const int CWbClubStoragePage::nOUTOFRANGE = -1;
const int CWbClubStoragePage::nENABLE_ALPHA = 255;
const int CWbClubStoragePage::nDISABLE_ALPHA = 100;

CWbClubStoragePage::CWbClubStoragePage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pBack(NULL)
    , m_pLabel(NULL)
    , m_nCurPos(-1)
    , m_bSplitItem(false)
    , m_wSplitItemPosX(0)
    , m_wSplitItemPosY(0)
    , m_bEnable(false)
{
    memset( m_pItemSlotArray, 0, sizeof ( CItemSlot* ) * EStorageY );
    memset( m_pItemSlotArrayDummy, 0, sizeof ( CUIControl* ) * nMAX_ONE_VIEW_SLOT );
}

CWbClubStoragePage::~CWbClubStoragePage ()
{
}

void CWbClubStoragePage::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_STORAGE_LABEL", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_SEARCH_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pBack = pLinBoxSmart;
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_STORAGE_LABEL" );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pLabel = pTextBox;
    }

    CString strSlotKeyword[nMAX_ONE_VIEW_SLOT] =
    {
        "WB_STORAGE_ITEM_SLOT0",
        "WB_STORAGE_ITEM_SLOT1",
        "WB_STORAGE_ITEM_SLOT2",
        "WB_STORAGE_ITEM_SLOT3",
        "WB_STORAGE_ITEM_SLOT4",
        "WB_STORAGE_ITEM_SLOT5",
        "WB_STORAGE_ITEM_SLOT6",
        "WB_STORAGE_ITEM_SLOT7",
    };

    {	//	더미 슬롯
        for ( int i = 0; i < nMAX_ONE_VIEW_SLOT; i++ )
        {
            m_pItemSlotArrayDummy[i] = CreateControl( strSlotKeyword[i].GetString () );
        }
    }

    const int nExampleCount = EStorageY;
    {	//	실제 데이타
        for ( int i = 0; i < nExampleCount; i++ )
        {
            m_pItemSlotArray[i] = CreateItemSlot( strSlotKeyword[0], ITEM_SLOT0 + i );
        }
    }

    SetItemSlotRender( nSTARTLINE, nMAX_ONE_VIEW_SLOT );
}

CUIControl*	CWbClubStoragePage::CreateControl( const char* szControl )
{
    CUIControl* pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub( this, szControl );
    RegisterControl( pControl );
    return pControl;
}

void CWbClubStoragePage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    SetItemIndex( nOUTOFRANGE, nOUTOFRANGE );
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
    GLInventory* pInvenPage = pMyClub->GetStorage(m_nPage);
    if (!pInvenPage)
        return;

    if ( m_bEnable )
        LoadItemPage(pInvenPage);
}

void CWbClubStoragePage::ResetAllItemSlotRender( int nTotal )
{
    if ( nTotal < 0 )
        return;

    for ( int i = 0; i < nTotal; i++ )
    {
        CItemSlot* pItemSlot = m_pItemSlotArray[i];
        if ( pItemSlot )
        {
            pItemSlot->SetVisibleSingle( FALSE );
        }
    }
}

void CWbClubStoragePage::SetItemSlotRender( int nStartIndex, int nTotal )
{
    if ( nTotal < 0 )
        return;

    const UIRECT& rcParentPos = GetGlobalPos();

    for ( int i = nStartIndex; i < nTotal; i++ )
    {
        CItemSlot* pItemSlot = m_pItemSlotArray[i];
        if ( pItemSlot )
        {
            int nAbsoluteIndex = i - nStartIndex;

            CUIControl* pDummyControl = m_pItemSlotArrayDummy[nAbsoluteIndex];			
            const UIRECT& rcSlotPos = pDummyControl->GetGlobalPos();
            const UIRECT& rcSlotLocalPos = pDummyControl->GetLocalPos();

            pItemSlot->SetLocalPos( rcSlotLocalPos );
            pItemSlot->SetGlobalPos( rcSlotPos );

            pItemSlot->SetVisibleSingle( TRUE );
        }
    }
}

CItemSlot* CWbClubStoragePage::CreateItemSlot( CString strKeyword, UIGUID ControlID )
{
    CItemSlot* pItemSlot = new CItemSlot( m_pGaeaClient, m_pInterface, m_pEngineDevice );
    pItemSlot->CreateSub( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
    pItemSlot->CreateSubControl( EStorageX, TRUE );
    RegisterControl( pItemSlot );
    return pItemSlot;
}

void CWbClubStoragePage::InitStoragePage( int nPage )
{
    m_nPage = nPage;

    m_pLabel->SetText( ID2GAMEWORD( "WB_CLUB_WINDOW_STORAGE_LABEL", m_nPage ),
        NS_UITEXTCOLOR::WHITE );

    if ( m_pGaeaClient->GetMyClub()->HaveStorageAuthority( m_pGaeaClient->GetCharacter()->CharDbNum() ) )
    {
        if ( static_cast<DWORD>(m_nPage) <= m_pGaeaClient->GetMyClub()->GetRank() )
        {
            m_bEnable = true;
            SetDiffuseAlphaAllControls( nENABLE_ALPHA );
        }
        else
        {
            m_bEnable = false;
            SetDiffuseAlphaAllControls( nDISABLE_ALPHA );
            UnLoadItemPage();
        }
    }
    else
    {
        m_bEnable = false;
        SetDiffuseAlphaAllControls( nDISABLE_ALPHA );
        UnLoadItemPage();
    }
}

void CWbClubStoragePage::SetDiffuseAlphaAllControls( int nAlpha )
{
    m_pBack->SetDiffuseAlpha( nAlpha );
    m_pLabel->SetDiffuseAlpha( nAlpha );

    const int nExampleCount = EStorageY;
    for ( int i = 0; i < nExampleCount; i++ )
    {
        for ( int j = 0; j < EStorageX; j++ )
            m_pItemSlotArray[i]->SetDiffuseAlpha( j, nAlpha );
    }
}

void CWbClubStoragePage::LoadItem( SINVENITEM& ref_InvenItem )
{	
    int nPosX = ref_InvenItem.wPosX;
    int nPosY = ref_InvenItem.wPosY;
    m_pItemSlotArray[nPosY]->SetItemImage( nPosX, ref_InvenItem );
}

void CWbClubStoragePage::LoadItemPage(GLInventory* pItemData, const bool bUSABLE)
{
    const GLInventory::CELL_MAP* pItemList = pItemData->GetItemList();

    if ( pItemList->size() > EStorageTotalSlot )
    {
        GASSERT( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
        return ;
    }


    {	//	삭제

        GLInventory::CELL_MAP_CITER iter = pItemList->begin();
        GLInventory::CELL_MAP_CITER iter_end = pItemList->end();

        for ( WORD y = 0; y < EStorageY; y++ )
        {
            for ( WORD x = 0; x < EStorageX; x++ )
            {
                const SINVENITEM* pRefInvenItem = GetItem( x, y );
                if (pRefInvenItem && (pRefInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()))
                {
                    GLInventory::CELL_KEY foundkey( x, y );
                    GLInventory::CELL_MAP_CITER found = pItemList->find( foundkey );
                    if ( found == iter_end )
                    {
                        UnLoadItem( x, y );
                    }
                }
            }
        }
    }

    {	//	등록
        GLInventory::CELL_MAP_CITER iter = pItemList->begin();
        GLInventory::CELL_MAP_CITER iter_end = pItemList->end();

        for ( ; iter!=iter_end; ++iter )
        {
            SINVENITEM* pInvenItem = (*iter).second;
            const SINVENITEM* pRefInvenItemOld = GetItem( pInvenItem->wPosX, pInvenItem->wPosY );
            if (pRefInvenItemOld && (*pInvenItem != *pRefInvenItemOld))
            {
                LoadItem( *pInvenItem );
            }	
        }
    }
}


void CWbClubStoragePage::UnLoadItemPage()
{
    for ( int y = 0; y < EStorageY; y++ )
    {
        for ( int x = 0; x < EStorageX; x++ )
        {
            UnLoadItem( x, y );
        }
    }
}

const SINVENITEM* CWbClubStoragePage::GetItem(int nPosX, int nPosY) const
{
    return m_pItemSlotArray[nPosY]->GetItemImage( nPosX );
}

void CWbClubStoragePage::UnLoadItem( int nPosX, int nPosY )
{
    m_pItemSlotArray[nPosY]->ResetItemImage( nPosX );
}


void CWbClubStoragePage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    if ( ITEM_SLOT0 <= ControlID && ControlID < ITEM_SLOT_END )
    {
        if ( CHECK_MOUSE_IN( dwMsg ) )
        {
            const int nPosY = ControlID - ITEM_SLOT0;
            CItemSlot* pSlot = m_pItemSlotArray[nPosY];
            const int nPosX = pSlot->GetItemIndex();

            if ( nPosY < 0 || nPosX < 0 ) return ;

            SetItemIndex( nPosX, nPosY );
        }
    }

    if ( ITEM_SLOT0 <= ControlID && ControlID < ITEM_SLOT_END )
    {
        if ( CHECK_MOUSE_IN( dwMsg ) )
        {
            int nPosX, nPosY;
            GetItemIndex( &nPosX, &nPosY );

            if ( nPosX < 0 || nPosY < 0 )
                return;

            const SINVENITEM* pInvenItem = GetItem( nPosX, nPosY );
            if ( pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()) )
            {
                m_pInterface->SHOW_ITEM_INFO(
                    pInvenItem->sItemCustom,
                    FALSE, FALSE, FALSE, TRUE, FALSE,
                    pInvenItem->wPosX, pInvenItem->wPosY );
            }

            if ( dwMsg & UIMSG_LB_UP )
            {
                if ( !holditem::HaveHoldItem() && !pInvenItem )
                    return;

                if ( holditem::HaveHoldItem() )
                {
                    if ( pInvenItem->GetNativeID() != NATIVEID_NULL() )
                    {
                        // Hold <-> Storage.
                        m_pGaeaClient->ReqClubItemExchangeToHoldItem( m_nPage, nPosX, nPosY );
                    }
                    else
                    {
                        // Inven -> Storage.
                        m_pGaeaClient->ReqInvenItemToClub( m_nPage, nPosX, nPosY );
                    }
                }
                else
                {
                    if ( pInvenItem->GetNativeID() != NATIVEID_NULL() )
                    {
                        // Storage -> Hold.
                        m_pGaeaClient->ReqClubItemToHoldItem( m_nPage, nPosX, nPosY );
                    }
                }
            }

            if ( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED )
            {
                if ( dwMsg & UIMSG_RB_UP )
                {
                    const SINVENITEM* pInvenItem = GetItem( nPosX, nPosY );
                    if ( pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()) )
                    {
                        SINVEN_POS sInvenPos;
                        sInvenPos.wPosX = nPosX;
                        sInvenPos.wPosY = nPosY;

                        SLINK_DATA_BASIC sLinkDataBasic;
                        sLinkDataBasic.sLinkType.wMType = ITEM_LINK;
                        sLinkDataBasic.sLinkType.wSType = ITEM_LINK_CLUB_STORAGE;
                        sLinkDataBasic.dwData1 = m_nPage;
                        sLinkDataBasic.dwData2 = sInvenPos.dwPos;

                        m_pInterface->ChatToLink(sLinkDataBasic, &pInvenItem->sItemCustom);
                        return ;
                    }
                }
            }

            if ( dwMsg & UIMSG_LB_DOWN )
            {
                SetSplitPos( nPosX, nPosY );
                m_bSplitItem = true;
                return;
            }
        }
    }
}

void CWbClubStoragePage::SetItemIndex( int nPosX, int nPosY )
{
    m_nPosX = nPosX;
    m_nPosY = nPosY;
}

void CWbClubStoragePage::GetItemIndex( int* pnPosX, int* pnPosY )
{
    *pnPosX = m_nPosX;
    *pnPosY = m_nPosY;
}


void CWbClubStoragePage::GetSplitPos ( WORD* pwPosX, WORD* pwPosY )
{
    *pwPosX = m_wSplitItemPosX;
    *pwPosY = m_wSplitItemPosY;
}

void CWbClubStoragePage::SetSplitPos ( WORD wPosX, WORD wPosY )
{
    m_wSplitItemPosX = wPosX;
    m_wSplitItemPosY = wPosY;
}
