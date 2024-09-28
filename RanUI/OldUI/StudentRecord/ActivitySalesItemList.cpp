#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../RanLogic/Activity/Activity.h"
#include "../../../RanLogic/Activity/ActivitySalesItem.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Inventory/GLInventory.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/WishList/WishList.h"

#include "../../InnerInterface.h"

#include "../Item/ItemImageEx.h"

#include "./ActivitySalesItemList.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CActivitySalesItemList::CActivitySalesItemList(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient ( pGaeaClient )
    , m_pInterface  ( pInterface  )
    , m_nScrollPos  ( 0 )
    , m_pSalesData  ( NULL )
{
    //Blank
}

CActivitySalesItemList::~CActivitySalesItemList ()
{
    //Blank
}

void CActivitySalesItemList::CreateSubControl ()
{
    // 라인박스
    {
        CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
        pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pBasicLineBox->CreateBaseBoxQuestList ( "ACTIVITY_SALESITEM_LIST_LINEBOX" );
        RegisterControl ( pBasicLineBox );
    }

    // 수행작업
    for ( UINT i=0; i<MAX_SLOT; ++i )
    {
        SITEMSLOT* pSlot = CreateSlot (i);
        m_SlotList.push_back( pSlot );
    }

    // 스크롤
    {
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        pScrollBar->CreateBaseScrollBar ( "ACTIVITY_SALESITEM_LIST_SCROLLBAR" );
        pScrollBar->GetThumbFrame()->SetState ( 0, (int)m_SlotList.size() );
        pScrollBar->GetThumbFrame()->SetScrollParent( this ); 
        RegisterControl ( pScrollBar );
        m_pScroll = pScrollBar;
    }
}

void CActivitySalesItemList::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    {
        CBasicScrollThumbFrame* const pThumbFrame = m_pScroll->GetThumbFrame ();
        const int nTotal       = pThumbFrame->GetTotal ();
        const int nViewPerPage = pThumbFrame->GetViewPerPage ();

        if ( nViewPerPage < nTotal )
        {
            int nCurPos = 0;
            const int nMovableLine = nTotal - nViewPerPage;
            float fPercent = pThumbFrame->GetPercent ();

            nCurPos = (int)floor(fPercent * nMovableLine);
            if ( nCurPos < 0 ) nCurPos = 0;

            if ( m_nScrollPos != nCurPos )
            {
                m_nScrollPos = nCurPos;
                UpdateList();
            }
        }
    }
}

void CActivitySalesItemList::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CActivitySalesItemList::ClearList ()
{
    m_pScroll->GetThumbFrame()->SetPercent( 0.0f );
    m_nScrollPos = 0;

    m_ItemList.clear();

    UpdateList();
}

void CActivitySalesItemList::AddList ( SACTIVITYSALESITEM* pSaleData )
{
    if ( !pSaleData )
    {
        return;
    }

    const GLInventory::CELL_MAP *pItemList = pSaleData->pITEMS->GetItemList();
    {	
        GLInventory::CELL_MAP_CITER iter     = pItemList->begin();
        GLInventory::CELL_MAP_CITER iter_end = pItemList->end();

        for ( ; iter!=iter_end; ++iter )
        {
            SINVENITEM* pInvenItem  = (*iter).second;
            DWORD       dwItemID    = pInvenItem->Id();

            const SITEM* pItemData = GLogicData::GetInstance().GetItem ( dwItemID );

            if ( !pItemData )
            {
                continue;
            }

            LONGLONG llSellPrice = (*pSaleData->pSELLPRICEMAP)[dwItemID];

            if( llSellPrice == 0 )
            {
                llSellPrice = (LONGLONG)pItemData->sBasicOp.dwBuyPrice;
            }

            m_ItemList.push_back( SITEMINFO(pInvenItem->sItemCustom, llSellPrice) );
        }
    }

    m_pScroll->GetThumbFrame()->SetState ( (int)m_ItemList.size(), (int)m_SlotList.size() );
}

void CActivitySalesItemList::UpdateList ()
{
    UINT nSlotSize = (UINT)m_SlotList.size();
    for ( UINT i = 0; i < nSlotSize; ++i ) 
    {
        m_SlotList[i]->Reset();
    }

    UINT nSlotIndex = 0;
    UINT nItemSize  = (UINT)m_ItemList.size();
    for ( UINT i = m_nScrollPos; i < nItemSize; ++i )
    {
        if ( nSlotIndex >= nSlotSize ) break;

        m_SlotList[nSlotIndex++]->SetItem( &m_ItemList[i] );
    }
}

CActivitySalesItemList::SITEMSLOT* CActivitySalesItemList::CreateSlot ( const UINT nIndex )
{
    SITEMSLOT* pSlot;
    pSlot = new SITEMSLOT ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
    pSlot->CreateSub ( this, "ACTIVITY_SALESITEM_LIST_ITEM_BASE", UI_FLAG_DEFAULT, SALESITEMLIST_LIST+nIndex );
    pSlot->CreateSubControl ();
    RegisterControl( pSlot );

    // 슬롯의 위치를 설정
    {
        CUIControl GapControl(m_pEngineDevice); 
        GapControl.Create ( 1, "ACTIVITY_SALESITEM_LIST_ITEM_GAP_Y" );
        const UIRECT& rcGapLocal  = GapControl.GetLocalPos();
        const UIRECT& rcBaseLocal = pSlot->GetLocalPos();

        pSlot->AddPosition( D3DXVECTOR2( 0.0f, (rcBaseLocal.sizeY + rcGapLocal.sizeY)*(float)nIndex ) );
    }

    return pSlot;
}

//-----------------------------------------------------------------------------------------------//

void CActivitySalesItemList::SITEMSLOT::CreateSubControl()
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    // 라인박스
    {
        CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
        pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pBasicLineBox->CreateBaseBoxQuestList ( "ACTIVITY_SALESITEM_ITEM_LINEBOX" );
        RegisterControl ( pBasicLineBox );
    }

    ITEM = new CItemImageEx ( GAEACLIENT, m_pEngineDevice );
    ITEM->CreateSub ( this, "ACTIVITY_SALESITEM_ITEM_ITEMIMAGE", UI_FLAG_DEFAULT, ID_ITEM );
    ITEM->CreateSubControl ();
    RegisterControl ( ITEM );

    ITEMNAME = new CBasicTextBox(m_pEngineDevice);
    ITEMNAME->CreateSub( this, "ACTIVITY_SALESITEM_ITEM_ITEMNAME" );
    ITEMNAME->SetFont( pFont );
    ITEMNAME->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
    RegisterControl( ITEMNAME );

    PRICE = new CBasicTextBox(m_pEngineDevice);
    PRICE->CreateSub( this, "ACTIVITY_SALESITEM_ITEM_PRICE" );
    PRICE->SetFont( pFont );
    PRICE->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
    RegisterControl( PRICE );

    REQPOINT = new CBasicTextBox(m_pEngineDevice);
    REQPOINT->CreateSub( this, "ACTIVITY_SALESITEM_ITEM_REQPOINT" );
    REQPOINT->SetFont( pFont );
    REQPOINT->SetTextAlign( TEXT_ALIGN_LEFT| TEXT_ALIGN_CENTER_Y );
    RegisterControl( REQPOINT );

    Reset ();
}

void CActivitySalesItemList::SITEMSLOT::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case ID_ITEM:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( !ITEMDATA || ITEMDATA->sITEM.GetNativeID() == NATIVEID_NULL() )
                    break;

                INNERINTERFACE->SHOW_ITEM_INFO(
					ITEMDATA->sITEM,
					FALSE, FALSE, FALSE, TRUE, FALSE,
					USHRT_MAX, USHRT_MAX,
					NATIVEID_NULL() );

				if( dwMsg & UIMSG_LB_UP )
				{
					if( DxInputDevice::GetInstance().GetKeyState( DIK_LSHIFT ) & DXKEY_PRESSED )
					{
						WishList::CClient* pWishList = GAEACLIENT->GetWishList();
						if( pWishList )
							pWishList->NewActivateItem( ITEMDATA->sITEM, ITEMDATA->llPRICE );
						return;
					}
				}

                if ( CHECK_LB_UP_LIKE( dwMsg ) && (DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED) )
                {
                    SITEMCUSTOM& sItemcustom = ITEMDATA->sITEM;

                    if ( INNERINTERFACE->ItemPreviewWindowIsPreviewItem( sItemcustom ) )
                    {
                        INNERINTERFACE->ItemPreviewWindowOpen( sItemcustom );
                    }                        
                }
                else if ( CHECK_RB_UP_LIKE( dwMsg ) && (DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED) )
                {
                    SITEMCUSTOM& sItemCustom = ITEMDATA->sITEM;

                    const SITEM* pItem = GLItemMan::GetInstance().GetItem(sItemCustom.GetNativeID());
                    if (pItem->BasicType() == ITEM_BOX || pItem->BasicType() == ITEM_RANDOMITEM )
                    {
                        INNERINTERFACE->GetInvenItemTooltipGroup()->ShowItemToolTipMulti( sItemCustom, DxInputDevice::GetInstance().GetMouseLocateX(), DxInputDevice::GetInstance().GetMouseLocateY() );

                        ResetMessageEx();
                    }
                }
            }
            break;
        }
    };
}

void CActivitySalesItemList::SITEMSLOT::Reset ()
{
    SetVisibleSingle( FALSE );
}

void CActivitySalesItemList::SITEMSLOT::SetItem ( SITEMINFO* pItemInfo )
{
    if ( !pItemInfo )
    {
        return;
    }

    const SITEM* pItemData = GLogicData::GetInstance().GetItem ( pItemInfo->sITEM.GetNativeID() );

    if ( !pItemData )
    {
        return;
    }

    // 아이템 이미지 설정
    ITEM->SetItem    ( pItemInfo->sITEM );
    ITEM->SetTurnNum ( 0 );

    // 아이템 값 설정
    ITEMDATA = pItemInfo;

    // 이름 설정.
    ITEMNAME->SetOneLineText( pItemInfo->sITEM.GETNAME().c_str(), NS_UITEXTCOLOR::PALEGREEN );

    // 가격 설정
    std::string strMoney = sc::string::format( ID2GAMEWORD( "ACTIVITY_SALESITEM_ITEM_TEXT", 0 ), 
        NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( pItemInfo->llPRICE, 3, ",") );

	PRICE->SetOneLineText( strMoney.c_str(), NS_UITEXTCOLOR::YELLOW );

	UIRECT rcRect;
	if ( PRICE->GetPartInfo(0,rcRect) )
	{
		CUIControl BaseControl(m_pEngineDevice); 
		BaseControl.Create ( 1, "ACTIVITY_SALESITEM_ITEM_ITEMNAME" );
		const UIRECT& rcBaseLocal = BaseControl.GetLocalPos();

		const UIRECT& rcParent = GetGlobalPos();
		float		  fWidth   = rcRect.left - (rcBaseLocal.left + rcParent.left) - 10.0f;

		ITEMNAME->SetLocalPos( UIRECT(rcBaseLocal.left, rcBaseLocal.top, fWidth, rcBaseLocal.sizeY) );
		ITEMNAME->SetGlobalPos( UIRECT(rcParent.left+rcBaseLocal.left, rcParent.top+rcBaseLocal.top, fWidth, rcBaseLocal.sizeY) );
	}

    // 요구 포인트 설정
    ENUM_ACTIVITY_CLASS emActivityClass =  ENUM_ACTIVITY_CLASS(pItemData->sBasicOp.emReqActivityType);

    std::string strReqPoint = sc::string::format ( ID2GAMEWORD( "ACTIVITY_SALESITEM_ITEM_TEXT", 1 ), 
                ID2GAMEWORD( "ITEM_BASIC_ACT_POINT", emActivityClass ),
                pItemData->sBasicOp.wReqActPointDW );

    DWORD     dwReqPointColor = NS_UITEXTCOLOR::WHITE;
    LONGLONG llPoint          = GAEACLIENT->GetCharacter()->m_llActivityPoint[emActivityClass];

    if ( llPoint < (LONGLONG)pItemData->sBasicOp.wReqActPointDW )
    {
        dwReqPointColor = NS_UITEXTCOLOR::RED;
    }

    REQPOINT->SetOneLineText( strReqPoint.c_str(), dwReqPointColor );

    SetVisibleSingle( TRUE );
}
