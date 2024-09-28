#include "StdAfx.h"
#include "./RnPageBank.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../InnerInterface.h"

#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "./ItemSlot.h"

#include "../../../SigmaCore/DebugInclude.h"

CRnPageBank::CRnPageBank( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CRnPageBase( pGaeaClient, pInterface, pEngineDevice )
{
	m_nPageMax = EBankPage;
}

void CRnPageBank::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CRnPageBase::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return;

	UpdateItemSlot( pChar->m_cInvenCharged );
}

void CRnPageBank::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CRnPageBase::TranslateUIMessage( ControlID, dwMsg );

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return;

	switch( ControlID )
	{
	case EHelpButton:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->MultiLineCommentUtil(
				ID2GAMEINTEXT( "INVENTORY_HELP_BANK" ),
				m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG ),
				800.0f,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,
				NS_UITEXTCOLOR::LIGHTSKYBLUE );
		}
		break;
	}

	if( ControlID >= EItemBegin && ControlID <= EItemEnd )
	{
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			const SINVENITEM* pInvenItem = GetSlotItem( m_nFocusX, m_nFocusY );
			bool bValidItem = ( pInvenItem && pInvenItem->GetNativeID() != NATIVEID_NULL() );
			if( !bValidItem )
				return;

			m_pInterface->SHOW_ITEM_INFO(
				pInvenItem->sItemCustom,
				FALSE, FALSE, FALSE, TRUE, TRUE,
				pInvenItem->wPosX, pInvenItem->wPosY );

			if ( dwMsg & UIMSG_LB_UP )
			{
				const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
				if( !pItem )
					return;

				if( DxInputDevice::GetInstance().GetKeyState( DIK_LSHIFT ) & DXKEY_PRESSED )
				{
					WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
					if( pWishList )
						pWishList->NewBankItem( pInvenItem->sItemCustom );
					return;
				}

				if( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED )
				{
					pChar->ReqChargedItemTo( pInvenItem->wPosX, pInvenItem->wPosY );
				}
				else
				{
					std::string strText = sc::string::format(
						ID2GAMEINTEXT( "MODAL_ITEMBANK_GET_ITEM", 0 ),
						pInvenItem->sItemCustom.GETNAME().c_str() );

					m_pInterface->ModalWindowSetModalData( pInvenItem->wPosX, pInvenItem->wPosY );

					m_pInterface->DoModalTipBox(
						strText,
						ID2GAMEINTEXT( "MODAL_ITEMBANK_GET_ITEM", 1 ),
						UI::MODAL_QUESTION,
						UI::YESNO,
						UI::MODAL_ITEMBANK_GET_ITEM );
				}
				return;
			}
		}
	}
}

void CRnPageBank::SetVisibleSingle( BOOL bVisible )
{
    CUIGroupHelper::SetVisibleSingle( bVisible );

    if( bVisible )
        m_pGaeaClient->GetCharacter()->ReqItemBankInfo(false);
}

void CRnPageBank::UIPosToLogicPos( int& nX, int& nY, int nPage )
{
	int nNum = nX + nY * ESlotX;
	nNum += nPage * ESlotX * ESlotY;

	nX = nNum % EBankX;
	nY = nNum / EBankX;
}

void CRnPageBank::LogicPosToUIPos( int& nX, int& nY, int& nPage )
{
	int nNum = nX + nY * EBankX;

	nX = nNum % ESlotX;
	nY = nNum / ESlotX;
	nPage = nY / ESlotY;
	nY = nY % ESlotY;
}
