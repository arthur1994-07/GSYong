#include "stdafx.h"

#include "../../InnerInterface.h"
#include "../../StaticUIManager.h"

#include "./ItemInfoTooltipGroup.h"

CItemToolTipGroup::CItemToolTipGroup( CInnerInterface* _pInterface, GLGaeaClient* _pGaeaClient, EngineDeviceMan* _pEngineDevice, UIGUID iBeginGUID, int iNumTooltipMulti )
{
    if ( !_pInterface || !_pGaeaClient || !_pEngineDevice )
        return;

    m_pInterface = _pInterface;
    m_BeginGUID = iBeginGUID;
    for( int i=0; i<iNumTooltipMulti; ++i )
    {
        CItemInfoTooltip* pToolTip = new CItemInfoTooltip( _pGaeaClient, m_pInterface, _pEngineDevice );
        pToolTip->Create( iBeginGUID + i, "ITEM_INFOR_TOOLTIP_RN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pToolTip->CreateSubControl();
        pToolTip->CreateCloseButton();
        pToolTip->SetBlockMouseTracking( true );
        pToolTip->SetVisibleSingle( FALSE );

        m_pInterface->UiRegisterControl( pToolTip, true );
        m_pInterface->UiShowGroupFocus( iBeginGUID + i );
        m_pInterface->UiHideGroup( iBeginGUID + i );

        SItemToolTip sToolTip;
        sToolTip.nID = iBeginGUID + i;
        sToolTip.pControl = pToolTip;
        m_vecItemToolTipRegist.push_back( sToolTip );
        m_vecItemToolTipPool.push_back( sToolTip );
    }
}

BOOL CItemToolTipGroup::IsShowItemToolTipMulti()
{
    if ( m_vecItemToolTipUsed.size() > 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

CItemInfoTooltip* const CItemToolTipGroup::ShowItemToolTip( const SITEMCUSTOM& sItemCustom,
                                                                const BOOL bShopOpen, const BOOL bInMarket,
                                                                const BOOL bInPrivateMarket, const BOOL bUsePreview,
                                                                const BOOL bIsWEAR_ITEM, WORD wPosX, WORD wPosY,
                                                                SNATIVEID sNpcNativeID, int iPageIndex /*= -1*/ )
{
    if( m_vecItemToolTipPool.empty() )
        return NULL;

    SItemToolTip sToolTip = m_vecItemToolTipPool.back();
    m_vecItemToolTipPool.pop_back();

    sToolTip.pControl->ResetInformation();
    sToolTip.pControl->DisplayItemInformation( sItemCustom, bShopOpen, bInMarket, bInPrivateMarket, bUsePreview, bIsWEAR_ITEM, wPosX, wPosY, sNpcNativeID, iPageIndex );
    sToolTip.pControl->SetVisibleSingle( TRUE );

    sToolTip.pControl->SetGroup();

    m_pInterface->UiShowGroupFocus( sToolTip.nID );

    m_vecItemToolTipUsed.push_back( sToolTip );
    return m_vecItemToolTipUsed.back().pControl;
}

CItemInfoTooltip* const CItemToolTipGroup::ShowItemToolTipMulti( const SITEMCUSTOM& sItemCustom,
                          const BOOL bShopOpen, const BOOL bInMarket,
                          const BOOL bInPrivateMarket, const BOOL bUsePreview,
                          const BOOL bIsWEAR_ITEM, WORD wPosX, WORD wPosY,
                          SNATIVEID sNpcNativeID, int iPageIndex /*= -1 */ )
{
    if ( !m_pInterface )
        return NULL;

    if( uiman::GetInnerUIMan().IsExclusiveControl() )
        return NULL;

    BOOST_FOREACH( SItemToolTip& sToolTip, m_vecItemToolTipUsed )
    {
        if( sToolTip.pControl->GetSItemcustomList() == sItemCustom && iPageIndex < 0 )
            return NULL;
    }
    CItemInfoTooltip* _pControl = ShowItemToolTip( sItemCustom, bShopOpen, bInMarket, bInPrivateMarket, bUsePreview, bIsWEAR_ITEM, wPosX, wPosY, sNpcNativeID, iPageIndex );

    if ( _pControl )
    {
        _pControl->SetSItemcustomList( sItemCustom );

		if( iPageIndex >= 0 )
		{
			_pControl->AllControlRePos(DxInputDevice::GetInstance().GetMouseLocateX(), DxInputDevice::GetInstance().GetMouseLocateY());
		}
    }
    return m_vecItemToolTipUsed.back().pControl;
}

CItemInfoTooltip* const CItemToolTipGroup::ShowItemToolTipMulti( WishList::ItemSPtr pWishItem,
                          const BOOL bShopOpen, const BOOL bInMarket,
                          const BOOL bInPrivateMarket, const BOOL bUsePreview,
                          const BOOL bIsWEAR_ITEM, WORD wPosX, WORD wPosY,
                          SNATIVEID sNpcNativeID )
{
    if ( !m_pInterface )
        return NULL;

    if( uiman::GetInnerUIMan().IsExclusiveControl() )
        return NULL;

    BOOST_FOREACH( SItemToolTip& sToolTip, m_vecItemToolTipUsed )
    {
        if( sToolTip.pControl->GetWishListItem() == pWishItem )
            return NULL;
    }
    CItemInfoTooltip* _pControl = ShowItemToolTip( pWishItem->m_sItem, bShopOpen, bInMarket, bInPrivateMarket, bUsePreview, bIsWEAR_ITEM, wPosX, wPosY, sNpcNativeID );
    
    if ( _pControl )
    {
        _pControl->SetWishListItem( pWishItem );
    }
    return m_vecItemToolTipUsed.back().pControl;
}

CItemInfoTooltip* const CItemToolTipGroup::ShowItemToolTipMulti( WishList::ItemSPtr pWishItem, int x, int y )
{
    return ShowItemToolTipMulti( pWishItem, FALSE, FALSE, FALSE, FALSE, FALSE, x, y, NATIVEID_NULL() );
}

CItemInfoTooltip* const CItemToolTipGroup::ShowItemToolTipMulti( SITEMCUSTOM& sItemCustom, int x, int y )
{
    return ShowItemToolTipMulti( sItemCustom, FALSE, FALSE, FALSE, FALSE, FALSE, x, y, NATIVEID_NULL() );
}

CItemInfoTooltip* const CItemToolTipGroup::ShowItemToolTipMulti( SITEMCUSTOM& sItemCustom, int x, int y, SNATIVEID sNpcNativeID, int iPageIndex /* = -1 */)
{
	return ShowItemToolTipMulti( sItemCustom, FALSE, FALSE, FALSE, FALSE, FALSE, x, y, sNpcNativeID, iPageIndex );
}

void CItemToolTipGroup::CloseItemToolTipMulti( UIGUID ControlID )
{
    ItemToolTipVecIter it = m_vecItemToolTipUsed.begin();
    for( ; it != m_vecItemToolTipUsed.end(); ++it )
    {
        SItemToolTip& sToolTip = *it;

        if( sToolTip.nID != ControlID )
            continue;

        m_vecItemToolTipPool.push_back( sToolTip );
        m_vecItemToolTipUsed.erase( it );

        return;
    }
}

void CItemToolTipGroup::CloseItemToolTipMultiAll()
{
    ItemToolTipVec TempItemToolTipUsed = m_vecItemToolTipUsed;
    BOOST_FOREACH( SItemToolTip& sToolTip, TempItemToolTipUsed )
    {
        sToolTip.pControl->SetVisibleSingle( FALSE );
    }

    m_vecItemToolTipUsed.clear();
}

void CItemToolTipGroup::TranslateMessage( UIGUID ControlID, DWORD dwMsg )
{
    UINT nIndex = ControlID - m_BeginGUID;
    m_vecItemToolTipRegist[ nIndex ].pControl->TranslateMessage( dwMsg );
}
