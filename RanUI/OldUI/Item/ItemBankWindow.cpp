#include "StdAfx.h"

#include "ItemBankWindow.h"
#include "ItemBankPage.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CItemBankWindow::CItemBankWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pPage(NULL)
{
}

CItemBankWindow::~CItemBankWindow ()
{
}

void CItemBankWindow::CreateSubControl ()
{	
//	CreateTextButton ( "ITEMBANK_REFRESH_BUTTON", ITEMBANK_REFRESH_BUTTON, const_cast<char*>(ID2GAMEWORD("ITEMBANK_REFRESH_BUTTON")) );

	CItemBankPage* pItemBankPage = new CItemBankPage ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pItemBankPage->CreateSub ( this, "ITEMBANK_PAGE", UI_FLAG_DEFAULT, ITEMBANK_PAGE );
	pItemBankPage->CreateSubControl ();	
	RegisterControl ( pItemBankPage );
	m_pPage = pItemBankPage;
}

CBasicTextButton*  CItemBankWindow::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pTextButton );

	return pTextButton;
}


void CItemBankWindow::InitItemBank ()
{
	m_pPage->UnLoadItemPage ();

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	m_pPage->LoadItemPage ( pCharacter->m_cInvenCharged );
}

void CItemBankWindow::ClearItemBank()
{ 
	m_pPage->UnLoadItemPage ();
}

void CItemBankWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	
	if ( ET_CONTROL_TITLE == ControlID || ET_CONTROL_TITLE_F == ControlID ) 
	{
		if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
		{
			m_pInterface->SetDefaultPosInterface( ITEMBANK_WINDOW );
			return;
		}		
	}
	else if ( ITEMBANK_PAGE == ControlID )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			int nPosX, nPosY;
			m_pPage->GetItemIndex ( &nPosX, &nPosY );

			//CDebugSet::ToView ( 1, 3, "[itembank] Page:%d %d / %d", nPosX, nPosY );

			if (nPosX < 0 || nPosY < 0)
                return;

			const SINVENITEM* pInvenItem = m_pPage->GetItem ( nPosX, nPosY );
			if (pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()))
			{
				m_pInterface->SHOW_ITEM_INFO(
					pInvenItem->sItemCustom,
					FALSE, FALSE, FALSE, TRUE, FALSE,
					pInvenItem->wPosX, pInvenItem->wPosY );
			}
            else
            {
                return;
            }

			if ( dwMsg & UIMSG_LB_UP )
			{
                const SITEM* pItem =  GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );

                if ( !pItem )
                {
                    return;
                }

                if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_PRESSED )
                {
                    m_pGaeaClient->GetCharacter()->ReqChargedItemTo ( static_cast<WORD>(nPosX), static_cast<WORD>(nPosY) );
                }
                else
                {
                    std::string strCombine = sc::string::format( ID2GAMEINTEXT("MODAL_ITEMBANK_GET_ITEM", 0), pInvenItem->sItemCustom.GETNAME().c_str() );
                    m_pInterface->DoModalTipBox( strCombine.c_str(), ID2GAMEINTEXT("MODAL_ITEMBANK_GET_ITEM", 1), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_ITEMBANK_GET_ITEM );
                    m_pInterface->ModalWindowSetModalData( static_cast<int>(nPosX), static_cast<int>(nPosY) );
                }

				return ;
			}
		}
	}
}
