#include "StdAfx.h"
#include "./RnPageInven.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/RANPARAM.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/StaticHoldItem.h"
#include "../../../RanLogicClient/Pet/GLPetClient.h"

#include "../../InnerInterface.h"

#include "./ItemSlot.h"

#include "../../../SigmaCore/DebugInclude.h"

namespace
{
	const BYTE c_nTradeNormalAlpha = 255;
	const BYTE c_nTradeBlockAlpha = 77;
}

DWORD CRnPageInven::INVEN_UI_HIGHLIGHTSLOT_EVENT = UIMSG_USER1;

CRnPageInven::CRnPageInven( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CRnPageBase( pGaeaClient, pInterface, pEngineDevice )
	, m_bIsPressingLB( false )
	, m_bLock( false )
	, m_ToLockID( NO_ID )
	, m_bValidItem( false )
	, m_bMarketOpen( false )
	, m_bComsumeSplit( false )
	, m_nConsumeSplitPosX( INT_MIN )
	, m_nConsumeSplitPosY( INT_MIN )
	, m_nMouseX(0)
	, m_nMouseY(0)
	, m_bItemDecompose( false )
	, m_nItemDecomposeX( INT_MIN )
	, m_nItemDecomposeY( INT_MIN )
{
	m_nPageMax = EInvenPage;
}

void CRnPageInven::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CRnPageBase::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_nMouseX = x;
	m_nMouseY = y;

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return;

	UpdateItemSlot( pChar->m_cInventory );
	UpdateBlock();
	_UpdateItemDecomposeSlot();

	GLTradeClient* pTradeClient = m_pGaeaClient->GetTradeClient();
	if( pTradeClient )
		UpdateTradeBlock( pTradeClient->GetMyTradeBox() );

	GLPrivateMarket &sPMarket = m_pGaeaClient->GetCharacter()->m_sPMarket;
	if (m_pInterface->IsVisible(PRIVATE_MARKET_WINDOW))
		UpdatePrivateMarketBlock( sPMarket );	
}

void CRnPageInven::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CRnPageBase::TranslateUIMessage( ControlID, dwMsg );

    if( IsOpenMessageBox() == TRUE ) // 메시지 박스가 떠있다면 메시지를 무시한다.
        return;

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return;

	switch( ControlID )
	{
	case EHelpButton:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->MultiLineCommentUtil(
				ID2GAMEINTEXT( "INVENTORY_HELP_INVEN" ),
				m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG ),
				800.0f,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,
				NS_UITEXTCOLOR::LIGHTSKYBLUE );
		}
		break;
	case ESplitButton:
		if ( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->SHOW_COMMON_LINEINFO(
				ID2GAMEWORD( "INVENTORY_BUTTON_TEXT", 4 ),
				NS_UITEXTCOLOR::LIGHTSKYBLUE,
				TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
			if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				if ( m_bItemDecompose )
				{
					m_bItemDecompose = false;
					// 하이라이트를 끈다;
					this->ResetBorderItemSlot();
				}
				else
				{
					if ( m_pInterface->UiIsVisibleGroup( PRIVATE_MARKET_WINDOW ) ||
						 m_pInterface->UiIsVisibleGroup( TRADE_WINDOW ) )
					{
						// 분해 불가 메세지 출력;
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("INVEN_DECOMPOSE_UNFITSTATE", 0));
						break;
					}
					m_bItemDecompose = true;
				}
			}
		}
		break;
	}

	if( ControlID >= EItemBegin && ControlID <= EItemEnd )
	{
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			const SINVENITEM* pInvenItem = GetSlotItem( m_nFocusX, m_nFocusY );
			m_bValidItem = ( pInvenItem && pInvenItem->GetNativeID() != NATIVEID_NULL() );
			m_bMarketOpen = m_pInterface->IsOpenDialogueWindowMarketPage();

 			InvenItemInfoDisplay( pInvenItem );

			if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				InvenMouseLBUP( pInvenItem );
				return;
			}

			if( CHECK_LB_DOWN_LIKE( dwMsg ) )
			{
				InvenMouseLBDOWN();
				return;
			}

			if( CHECK_RB_UP_LIKE( dwMsg ))
			{
				InvenMouseRBUP( pInvenItem );
				return;
			}
		}
	}

}

void CRnPageInven::TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg )
{

    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
    if( !pChar )
        return;

    switch( nIdentity )
    {
    case MESSAGEBOX_CHECK_ITEM_BOX:
	case MESSAGEBOX_CHECK_POINT_CARD:
	case MESSAGEBOX_CHECK_EXP_ITEM:
        if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
        {
            pChar->ReqInvenDrug( m_wMessageBox_ItemPosX, m_wMessageBox_ItemPosY );
        }
    }        
}

void CRnPageInven::SetVisibleSingle( BOOL bVisible )
{
	// reset;
	m_bItemDecompose = false;
	CRnPageBase::SetVisibleSingle(bVisible);
}

void CRnPageInven::UpdateBlock()
{
	for( int i=0; i<ESlotY; ++i )
	{
		if( IsValidLine( i ) )
			m_pItemSlot[ i ]->ResetBLOCK( true );
		else
			m_pItemSlot[ i ]->SetBLOCK( true );
	}
}

void CRnPageInven::UpdateTradeBlock( GLInventory& cInventory )
{
	for( int y=0; y<ESlotY; ++y )
	{
		for( int x=0; x<ESlotX; ++x )
			m_pItemSlot[ y ]->SetDiffuseAlpha( x, c_nTradeNormalAlpha );
	}

	const GLInventory::CELL_MAP* pItemList = cInventory.GetItemList();

	BOOST_FOREACH( const GLInventory::CELL_MAP::value_type& it, *pItemList )
	{
		SINVENITEM* pInvenItem = it.second;

		int nX( pInvenItem->wBackX );
		int nY( pInvenItem->wBackY );
		int nPage( 0 );
		LogicPosToUIPos( nX, nY, nPage );

		if( nPage != m_nPage )
			continue;

		m_pItemSlot[ nY ]->SetDiffuseAlpha( nX, c_nTradeBlockAlpha );
	}
}

void CRnPageInven::UpdatePrivateMarketBlock( GLPrivateMarket& sPMarket )
{
	for( int y=0; y<ESlotY; ++y )
	{
		for( int x=0; x<ESlotX; ++x )
			m_pItemSlot[ y ]->SetDiffuseAlpha( x, c_nTradeNormalAlpha );
	}

	BOOST_FOREACH( GLPrivateMarket::MAPITEM::value_type& ItemList, sPMarket.GetItemList() )
	{
		const SSALEITEM& pInvenItem = ItemList.second;
		SNATIVEID sNID = pInvenItem.sITEMCUSTOM.GetNativeID();
		const SITEM *pTurnITEM = GLogicData::GetInstance().GetItem(sNID);

		//현재의 인벤토리를 가져온다
		GLInventory& Inven = m_pGaeaClient->GetCharacterData().m_cInventory;

		//개인상점 창에 올려진 겹침 아이템 총 개수
		DWORD dwTurnNum = pInvenItem.dwNUMBER;
		DWORD dwCountTurnNum = 0;

		//겹침 아이템 이면

		//현재 인벤토리와 비교하며 총개수 만큼 블럭한다.
		BOOST_FOREACH( const GLInventory::CELL_MAP::value_type& it, *Inven.GetItemList() )
		{
			SINVENITEM* pInvenTurnItem = it.second;
			int nX( pInvenTurnItem->wPosX );
			int nY( pInvenTurnItem->wPosX );
			int nPagePos( 0 );

			const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenTurnItem->sItemCustom.GetNativeID() );
			if( pItem && pItem->sBasicOp.sNativeID == sNID )
			{
				if (dwTurnNum > dwCountTurnNum)
				{
					dwCountTurnNum += pInvenTurnItem->sItemCustom.wTurnNum;
					if (pTurnITEM->ISPILE())
					{
						nX = pInvenTurnItem->wPosX;
						nY = pInvenTurnItem->wPosY;
					}
					else
					{
						nX = pInvenItem.sINVENPOS.wMainID;
						nY = pInvenItem.sINVENPOS.wSubID;
					}

					LogicPosToUIPos( nX, nY, nPagePos );

					if( nPagePos != m_nPage )
						continue;
					m_pItemSlot[ nY ]->SetDiffuseAlpha( nX, c_nTradeBlockAlpha );

					if (!pTurnITEM->ISPILE())
					{
						// 겹침 아이템이 아닐 경우, for문을 더 돌 필요는 없다.
						break;
					}
				}//if
			}//if
		}//BOOST_FOREACH
	}//BOOST_FOREACH
}

bool CRnPageInven::IsPrivateMarketBlock( const SINVENITEM* pInvenItem )
{
	if( !pInvenItem )
		return false;

	bool bOpender = false;
	DWORD nGaeaID = GAEAID_NULL;
	m_pInterface->GetPrivateMarketInfo( bOpender, nGaeaID );

	if( bOpender )
	{
		int nPosX = pInvenItem->wPosX;
		int nPosY = pInvenItem->wPosY;
		LogicPosToUIPos( nPosX, nPosY, m_nPage );

		if( nPosX < 0 || nPosX >= ESlotX )
			return false;

		if( nPosY < 0 || nPosY >= ESlotY )
			return false;

		D3DCOLOR dwItemAlpha = m_pItemSlot[ nPosY ]->GetDiffuseAlpha( nPosX );

		if( dwItemAlpha == NULL )
			return false;

		if( dwItemAlpha == c_nTradeBlockAlpha )
		{
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_REGITEM"));
			return true;
		}
	}	

	return false;
}

bool CRnPageInven::IsMessageBoxItem( const SINVENITEM* pInvenItem, const EMITEM_TYPE& emItemType )
{
	switch( emItemType )
	{
	case ITEM_SUIT:
		{
			if ( pInvenItem->CustomItem().IsBoxWrapped() )
			{
				m_wMessageBox_ItemPosX = pInvenItem->wPosX;
				m_wMessageBox_ItemPosY = pInvenItem->wPosY;

				CString strMessage;
				strMessage.Format( "[%s] %s", pInvenItem->sItemCustom.GETNAME().c_str() , ID2GAMEINTEXT("RNINVEN_ITEM_BOX_OPEN") );

				OpenMessageBox( CMessageBox::TITLE_QUESTION, strMessage.GetString(), CMessageBox::TYPE_YESNO, MESSAGEBOX_CHECK_ITEM_BOX );
				return true;
			}
		}
		break;
	case ITEM_LOCK_BOX:
		{
			if ( pInvenItem->sItemCustom.m_VehicleDbNum )	// 잠겨있다면, 열겟다는 메세지박스 오픈하지않음;
				break;
		}
	case ITEM_BOX:
    case ITEM_WRAPPED_BOX:
		{
			m_wMessageBox_ItemPosX = pInvenItem->wPosX;
			m_wMessageBox_ItemPosY = pInvenItem->wPosY;

			CString strMessage;
			strMessage.Format( "[%s] %s", pInvenItem->sItemCustom.GETNAME().c_str() , ID2GAMEINTEXT("RNINVEN_ITEM_BOX_OPEN") );

			OpenMessageBox( CMessageBox::TITLE_QUESTION, strMessage.GetString(), CMessageBox::TYPE_YESNO, MESSAGEBOX_CHECK_ITEM_BOX );
			return true;
		}
	case ITEM_POINT_CARD_REFUND:
	case ITEM_POINT_CARD_NOT_REFUND:
		{
			m_wMessageBox_ItemPosX = pInvenItem->wPosX;
			m_wMessageBox_ItemPosY = pInvenItem->wPosY;

			CString strMessage;
			strMessage.Format( ID2GAMEINTEXT("RNINVEN_ITEM_USE"), pInvenItem->sItemCustom.GETNAME().c_str() );

			OpenMessageBox( CMessageBox::TITLE_QUESTION, strMessage.GetString(), CMessageBox::TYPE_YESNO, MESSAGEBOX_CHECK_POINT_CARD );
			return true;
		}
	case ITEM_EXP_COMPERSSOR:
		{
			m_wMessageBox_ItemPosX = pInvenItem->wPosX;
			m_wMessageBox_ItemPosY = pInvenItem->wPosY;

			CString strMessage;
			strMessage.Format( ID2GAMEINTEXT("EXP_ITEM_COMPRESSOR_USE"), pInvenItem->sItemCustom.GETNAME().c_str() );

			OpenMessageBox( CMessageBox::TITLE_QUESTION, strMessage.GetString(), CMessageBox::TYPE_YESNO, MESSAGEBOX_CHECK_EXP_ITEM );
			return true;
		}
	case ITEM_EXP_CAPSULE:
		{
			m_wMessageBox_ItemPosX = pInvenItem->wPosX;
			m_wMessageBox_ItemPosY = pInvenItem->wPosY;

			CString strMessage;
			strMessage.Format( ID2GAMEINTEXT("EXP_ITEM_CAPSULE_USE"), pInvenItem->sItemCustom.GETNAME().c_str() );

			OpenMessageBox( CMessageBox::TITLE_QUESTION, strMessage.GetString(), CMessageBox::TYPE_YESNO, MESSAGEBOX_CHECK_EXP_ITEM );
			return true;
		}
	default:
		return false;
	}

	return false;
}

bool CRnPageInven::IsValidLine( int nLine )
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return false;

	int nInvenLine( GLCONST_CHAR::wInvenDefaultLine + pChar->GetOnINVENLINE() );
	nInvenLine = min( nInvenLine, EInvenTotalLine );

	nLine = nLine + m_nPage * ESlotY;

	return ( nLine < nInvenLine ) ? true : false;
}

void CRnPageInven::GetConsumeSplitInvenPos( WORD& nPosX, WORD& nPosY )
{
	int nInvenPosX = m_nConsumeSplitPosX;
	int nInvenPosY = m_nConsumeSplitPosY;
	UIPosToLogicPos( nInvenPosX, nInvenPosY, m_nPage );

	nPosX = static_cast< WORD >( nInvenPosX );
	nPosY = static_cast< WORD >( nInvenPosY );
}

void CRnPageInven::UIPosToLogicPos( int& nX, int& nY, int nPage )
{
	int nNum = nX + nY * ESlotX;
	nNum += nPage * ESlotX * ESlotY;

	nX = nNum % EInvenX;
	nY = nNum / EInvenX;
}

void CRnPageInven::LogicPosToUIPos( int& nX, int& nY, int& nPage )
{
	int nNum = nX + nY * EInvenX;

	nX = nNum % ESlotX;
	nY = nNum / ESlotX;
	nPage = nY / ESlotY;
	nY = nY % ESlotY;
}

void CRnPageInven::InvenItemInfoDisplay( const SINVENITEM* pInvenItem )
{
	if( m_bValidItem )
	{
		m_pInterface->SHOW_ITEM_INFO(
			pInvenItem->sItemCustom,
			m_bMarketOpen,
			FALSE, FALSE, TRUE, FALSE,
			pInvenItem->wPosX, pInvenItem->wPosY );
	}
	else if( m_pItemSlot[ m_nFocusY ]->IsBLOCK() )
	{
		m_pInterface->MultiLineCommentUtil(
			ID2GAMEINTEXT( "INVENTORY_INVALID_SLOT" ),
			m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG ),
			800.0f,
			TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,
			NS_UITEXTCOLOR::LIGHTSKYBLUE );
	}
}

BOOL CRnPageInven::_UpdateItemDecomposeSlot()
{
//	this->ResetBorderItemSlot();
	if( !m_bItemDecompose )
		return FALSE;

	BOOL bHighLight = FALSE;

	for ( int nPosY = 0; nPosY < ESlotY; ++nPosY )
	{
//		m_pItemSlot[nPosY]->ResetBorder();
		for ( int nPosX = 0; nPosX < ESlotX; ++nPosX )
		{
			int nRelativeX(nPosX), nRelativeY(nPosY);
			UIPosToLogicPos(nRelativeX, nRelativeY, m_nPage);

			const GLInventory& inven = m_pGaeaClient->GetCharacterData().m_cInventory;
			const SINVENITEM* const pSlotItem = inven.GetItem(nRelativeX, nRelativeY);
			if ( pSlotItem && pSlotItem->sItemCustom.IsDecompose() )
			{
				SetBorderItemSlot(nPosX, nPosY);
				bHighLight = TRUE;
				//m_pItemSlot[nPosY]->AddMessageEx(INVEN_UI_HIGHLIGHTSLOT_EVENT|UIMSG_TOTOPPARENT);
			}
// 			else
// 				m_pItemSlot[nPosY]->m_pEmphasisSlot[nPosX]->SetVisibleSingle(FALSE);
		}
	}
	return bHighLight;
}

void CRnPageInven::GetItemDecomposePos(WORD& wPosX, WORD& wPosY)
{
	wPosX = m_nItemDecomposeX;
	wPosY = m_nItemDecomposeY;
}

void CRnPageInven::InvenMouseLBDOWN()
{
	if( m_bLock && !m_bIsPressingLB )
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::RED,
			ID2GAMEINTEXT( "INVEN_LOCK_MSG" ) );
	}
	else if( !m_bLock )
	{
		m_bComsumeSplit = true;
		m_nConsumeSplitPosX = m_nFocusX;
		m_nConsumeSplitPosY = m_nFocusY;
	}

	m_bIsPressingLB = true;

	return;
}

void CRnPageInven::InvenMouseLBUP( const SINVENITEM* pInvenItem )
{
	m_bIsPressingLB = false;

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return;

	if( m_bValidItem )
	{
		if( DxInputDevice::GetInstance().GetKeyState( DIK_LSHIFT ) & DXKEY_PRESSED )
		{
			WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
			if( pWishList )
				pWishList->NewInventoryItem( pInvenItem->sItemCustom );
			return;
		}	
	}
	else
	{
		if ( m_bItemDecompose )
			m_bItemDecompose = false;
	}

	if ( !m_bLock && m_bItemDecompose )
	{
		if ( m_nFocusX == m_nConsumeSplitPosX || m_nFocusY == m_nConsumeSplitPosY )
		{
			m_nItemDecomposeX = m_nFocusX;
			m_nItemDecomposeY = m_nFocusY;

			int nRelativeX(m_nItemDecomposeX), nRelativeY(m_nItemDecomposeY);
			UIPosToLogicPos(nRelativeX, nRelativeY, m_nPage);

			if ( pChar->IsInvenDecomposeItem(nRelativeX,nRelativeY) )
			{
				m_pInterface->DoModal(
					 ID2GAMEINTEXT( "ITEM_DECOMPOSE_OKQUESTION" )
					, UI::MODAL_QUESTION
					, UI::YESNO
					, UI::MODAL_INVENTORY_ITEMDECOMPOSE_OKQUESTION );
			}
		}
	}
	else if( m_bComsumeSplit && !m_bLock )
	{
		m_bComsumeSplit = false;

		int nPosX = m_nConsumeSplitPosX;
		int nPosY = m_nConsumeSplitPosY;
		UIPosToLogicPos( nPosX, nPosY, m_nPage );

		bool bDiffPos = ( m_nFocusX != m_nConsumeSplitPosX || m_nFocusY != m_nConsumeSplitPosY );
		bool bSplitable = pChar->IsInvenSplitItem( WORD( nPosX ), WORD( nPosY ) );
		if( bDiffPos && bSplitable )
		{
			m_pInterface->DoModal(
				ID2GAMEINTEXT( "SPLIT_ITEM" ),
				UI::MODAL_QUESTION,
				UI::EDITBOX_NUMBER,
				UI::MODAL_SPLIT_ITEM );
		}
		else
		{
			nPosX = m_nFocusX;
			nPosY = m_nFocusY;
			UIPosToLogicPos( nPosX, nPosY, m_nPage );

			if( m_pInterface->UiIsVisibleGroup( PRIVATE_MARKET_WINDOW ) )
			{
				bool bOpender = false;
				DWORD nGaeaID = GAEAID_NULL;
				m_pInterface->GetPrivateMarketInfo( bOpender, nGaeaID );

				if( bOpender )
				{
					if( !m_bValidItem )
						return;

					GLPrivateMarket& sPMarket = pChar->m_sPMarket;
					if( sPMarket.IsOpen() )
						return;

					const SITEM* pItemData = GLItemMan::GetInstance().GetItem( pInvenItem->Id() );
					if( !pItemData )
						return;

					bool bPileItem = false;
					if( pItemData->ISPILE() )
					{
						if( pInvenItem->sItemCustom.wTurnNum > 1 )
							bPileItem = true;
					}

					m_pInterface->MODAL_PRIVATE_MARKET_SELLITEM(
						bPileItem,
						WORD( nPosX ), WORD( nPosY ) );
				}
			}
			else
			{
				pChar->ReqInvenTo( WORD( nPosX ), WORD( nPosY ) );
			}
		}
	}
}

void CRnPageInven::InvenMouseRBUP( const SINVENITEM* pInvenItem )
{
	if( m_bLock )
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::RED,
			ID2GAMEINTEXT( "INVEN_LOCK_MSG" ) );
		return;
	}

	bool bHold = holditem::HaveHoldItem();

	if ( m_bItemDecompose )
	{
		m_bItemDecompose = false;
		return;
	}
	if( !m_bValidItem )
		return;

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return;

	SITEMCUSTOM sItemCustom = pInvenItem->sItemCustom;
    if ( DxInputDevice::GetInstance().GetKeyState( DIK_LALT ) & DXKEY_PRESSED )
    {
        const SITEM* pItem = GLItemMan::GetInstance().GetItem(sItemCustom.GetNativeID());
		SSETITEM* pSetItem = NULL;
        if (pItem->BasicType() == ITEM_BOX || pItem->BasicType() == ITEM_RANDOMITEM )
            m_pInterface->GetInvenItemTooltipGroup()->ShowItemToolTipMulti( sItemCustom, m_nMouseX, m_nMouseY );
// 		else if ( (pSetItem = const_cast<SSETITEM*>(GLItemSet::GetInstance().GetInfoSetItem(pItem->BasicId()))) )
// 		{
// 			m_pInterface->SetitemGroupComposition(pSetItem);
// 			m_pInterface->OpenSetitemPreviewWindow();
// 		}
        return;
    }

	if( m_bMarketOpen && RANPARAM::bCheckSellItemRButton )
	{
		if( bHold )
			return;

		const SITEM* pItemData = GLogicData::GetInstance().GetItem( pInvenItem->Id() );
		if( !pItemData )
			return;
		
		// 판매 가능 여부
		bool bSale = true;
		{
			//if( false == pItemData->sBasicOp.IsSALE() || true == pInvenItem->sItemCustom.IsGM_GENITEM() )
			if( false == ( pInvenItem->sItemCustom.IsSale() != NULL ) || true == pInvenItem->sItemCustom.IsGM_GENITEM() )
				bSale = false;

			// 팻카드일 경우
			if( pItemData->sBasicOp.emItemType == ITEM_PET_CARD )
			{
				const SITEMCUSTOM& sHoldItem = pInvenItem->sItemCustom;
				GLPetClient* pMyPet = m_pGaeaClient->GetPetClient();
				if( true == pMyPet->IsVALID() && sHoldItem.PetDbNum() == pMyPet->DbNum() )
					bSale = false;
			}
		}
		

		if( RANPARAM::bCheckConfirmSellItem )
		{
			if( false == bSale )
			{
				m_pInterface->DoModal( ID2GAMEINTEXT( "IMPOSSIBLE_SALE_ITEM" ) );
				return;
			}

			std::string strText = sc::string::format(
				ID2GAMEINTEXT( "SELL_ITEM_CONFIRM" ),
				pInvenItem->sItemCustom.GETNAME().c_str() );

			m_pInterface->ModalWindowSetModalData( pInvenItem->wPosX, pInvenItem->wPosY );

			m_pInterface->DoModal(
				strText,
				UI::MODAL_QUESTION,
				UI::YESNO,
				UI::MODAL_DIRECT_SELL_ITEM );
		}
		else
		{
			if( false == bSale )
			{
				m_pGaeaClient->PrintConsoleText( ID2GAMEINTEXT( "IMPOSSIBLE_SALE_ITEM" ) );
				return;
			}

			pChar->ReqNpcDirectSaleTo(
				m_pInterface->GetDialogueWindowNPCID(),
				m_pInterface->GetDialogueWindowNPCGlobID(),
				pInvenItem->wPosX,
				pInvenItem->wPosY );
		}
		return;
	}

	if( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED )
	{
		SINVEN_POS sInvenPos;
		sInvenPos.wPosX = pInvenItem->wPosX;
		sInvenPos.wPosY = pInvenItem->wPosY;

		SLINK_DATA_BASIC sLinkDataBasic;
		sLinkDataBasic.sLinkType.wMType = ITEM_LINK;
		sLinkDataBasic.sLinkType.wSType = ITEM_LINK_INVEN;
		sLinkDataBasic.dwData1 = sInvenPos.dwPos;

		m_pInterface->ChatToLink(sLinkDataBasic, &pInvenItem->sItemCustom);
		return;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetRealNativeID() );
	if( pItem == NULL )
		return;

	EMITEM_TYPE eItemType = pChar->GetInvenItemType( pInvenItem->wPosX, pInvenItem->wPosY );

	// 아이템 조건 확인
	EMINVEN_USE_ITEM emItemMsg = EMINVEN_USE_ITEM_OK;
	if ( sItemCustom.nidDISGUISE.IsValidNativeID() )
	{
		pChar->SIMPLE_CHECK_ITEM(pInvenItem->sItemCustom, sItemCustom.nidDISGUISE, emItemMsg);
	}
	else
	{
		pChar->SIMPLE_CHECK_ITEM(pInvenItem->sItemCustom, emItemMsg);
	}

	if ( emItemMsg != EMINVEN_USE_ITEM_OK && !bHold )
	{
		if ( InvenMsg( emItemMsg ) == false )
			return;
	}

	// 메시지박스 필요한 아이템일 경우 선택 팝업을 띄운다.( ex: 선물상자, 포인트카드 )
	if( IsMessageBoxItem( pInvenItem, eItemType ) )
		return;

	if( m_pInterface->UiIsVisibleGroup( PRIVATE_MARKET_WINDOW ) )
	{
		if( eItemType == ITEM_PRIVATEMARKET )
			return;

		if( IsPrivateMarketBlock( pInvenItem ) )
			return;
	}

	pChar->ReqInvenDrug( pInvenItem->wPosX, pInvenItem->wPosY );
}

const bool CRnPageInven::InvenMsg( EMINVEN_USE_ITEM emInvenMsg )
{
	switch( emInvenMsg )
	{
		case EMINVEN_USE_ITEM_FAIL:
			//
			return false;

		case EMINVEN_USE_ITEM_NOITEM:
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_NOITEM"));
			return false;

		case EMINVEN_USE_ITEM_BADITEM:
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("EMINVEN_RENAME_FB_BADITEM"));
			return false;

		case EMINVEN_USE_ITEM_BADCLASS:
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_BADCLASS"));
			return false;

		case EMINVEN_USE_ITEM_BADLEVEL:
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("EMINVEN_ITEM_WRONG_LEVEL"));
			return false;

		case EMINVEN_USE_ITEM_COSTUMEBADLEVEL:
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("EMINVEN_ITEM_WRONG_COSTUMELEVEL"));
			return false;
	}

	return true;
}

void CRnPageInven::SetLock( UIGUID ControlID, bool bLock )
{
	m_bLock = bLock;
	if( m_bLock )
		m_ToLockID = ControlID;
	else
		m_ToLockID = NO_ID;
}

void CRnPageInven::SetLock( bool bLock )
{
	m_bLock = bLock;
	if( !m_bLock )
		m_ToLockID = NO_ID;
}
