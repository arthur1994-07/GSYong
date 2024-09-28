#include "StdAfx.h"
#include "./RnPageWish.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../InnerInterface.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "./ItemSlot.h"

#include "../../../SigmaCore/DebugInclude.h"

CRnPageWish::CRnPageWish( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CRnPageBase( pGaeaClient, pInterface, pEngineDevice )

	, m_nMouseX( 0 )
	, m_nMouseY( 0 )
{
	m_nPageMax = WishList::EPage;
}

void CRnPageWish::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CRnPageBase::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_nMouseX = x;
	m_nMouseY = y;

	UpdateItemSlot();
}

void CRnPageWish::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CRnPageBase::TranslateUIMessage( ControlID, dwMsg );

	WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
	if( !pWishList )
		return;

	switch( ControlID )
	{
	case EHelpButton:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->MultiLineCommentUtil(
				ID2GAMEINTEXT( "INVENTORY_HELP_WISH" ),
				m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG ),
				800.0f,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,
				NS_UITEXTCOLOR::LIGHTSKYBLUE );
		}
		break;
	}

	if( ControlID >= EItemBegin && ControlID <= EItemEnd )
	{
		if( dwMsg == UIMSG_EXCHANGE_LIST)
		{
			int nFocusY = ControlID - EItemBegin;
			int nFocusX = m_pItemSlot[ nFocusY ]->GetItemIndex();

			const SINVENITEM* pInvenItem = GetSlotItem( nFocusX, nFocusY );
			bool bValidItem = ( pInvenItem && pInvenItem->GetNativeID() != NATIVEID_NULL() );
			if( !bValidItem )
				return;

			DWORD nSlot = MAKELONG( pInvenItem->wPosX, pInvenItem->wPosY );

			WishList::ItemSPtr pItem = pWishList->GetItem( nSlot );
			if( !pItem )
				return;

			{
				switch( pItem->GetType())
				{
				case WishList::ETypeNpcStoreEx :
					{
						WishList::SItemNpcExStore* pStore = dynamic_cast<WishList::SItemNpcExStore*>(pItem.get());

						m_pInterface->GetInvenItemTooltipGroup()->ShowItemToolTipMulti( pStore->m_sItem, pStore->m_iX, pStore->m_iY, pStore->m_sCrowID, pStore->m_iPage );
					}
					return;
				default :
					{
						//m_pInterface->GetWishItemTooltipGroup()->ShowItemToolTipMulti( pItem, m_nMouseX, m_nMouseY );
					}
					return;
				}

				return;
			}
		}
		else
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			const SINVENITEM* pInvenItem = GetSlotItem( m_nFocusX, m_nFocusY );
			bool bValidItem = ( pInvenItem && pInvenItem->GetNativeID() != NATIVEID_NULL() );
			if( !bValidItem )
				return;

			DWORD nSlot = MAKELONG( pInvenItem->wPosX, pInvenItem->wPosY );

			WishList::ItemSPtr pItem = pWishList->GetItem( nSlot );
			if( !pItem )
				return;

			m_pInterface->SHOW_ITEM_INFO( pItem );

			if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				switch( pItem->GetType())
				{
				case WishList::ETypeNpcStoreEx :
					{
						WishList::SItemNpcExStore* pStore = dynamic_cast<WishList::SItemNpcExStore*>(pItem.get());

						m_pInterface->GetInvenItemTooltipGroup()->ShowItemToolTipMulti( pStore->m_sItem, pStore->m_iX, pStore->m_iY, pStore->m_sCrowID, pStore->m_iPage );
					}
					return;
				default :
					{
						m_pInterface->GetWishItemTooltipGroup()->ShowItemToolTipMulti( pItem, m_nMouseX, m_nMouseY );
					}
					return;
				}
                
				return;
			}

			if( dwMsg & UIMSG_RB_UP )
			{
				pWishList->RemoveItem( nSlot );
				return;
			}
		}
	}
}

void CRnPageWish::UpdateItemSlot()
{
	for( int y=0; y<ESlotY; ++y )
	{
		for( int x=0; x<ESlotX; ++x )
			ResetSlotItem( x, y );
	}

	WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
	if( !pWishList )
		return;

	const WishList::ItemMap& mapItem = pWishList->GetItemMap();

	BOOST_FOREACH( const WishList::ItemMapValue& it, mapItem )
	{
		WORD nPosX = LOWORD( it.first );
		WORD nPosY = HIWORD( it.first );

		int nX( nPosX );
		int nY( nPosY );
		int nPage( 0 );
		LogicPosToUIPos( nX, nY, nPage );

		if( nPage != m_nPage )
			continue;

		WishList::ItemSPtr pItem = it.second;

		SINVENITEM sInvenItem;
		sInvenItem.wPosX = nPosX;
		sInvenItem.wPosY = nPosY;
		sInvenItem.sItemCustom = pItem->m_sItem;

		SetSlotItem( nX, nY, sInvenItem );
	}
}

void CRnPageWish::UIPosToLogicPos( int& nX, int& nY, int nPage )
{
	int nNum = nX + nY * ESlotX;
	nNum += nPage * ESlotX * ESlotY;

	nX = nNum % WishList::ESlotX;
	nY = nNum / WishList::ESlotX;
}

void CRnPageWish::LogicPosToUIPos( int& nX, int& nY, int& nPage )
{
	int nNum = nX + nY * WishList::ESlotX;

	nX = nNum % ESlotX;
	nY = nNum / ESlotX;
	nPage = nY / ESlotY;
	nY = nY % ESlotY;
}