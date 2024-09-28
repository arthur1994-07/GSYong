#include "StdAfx.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "InventoryWindow.h"
#include "InventoryPage.h"
#include "InventoryPageWear.h"
#include "InventoryPageWearEx.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/RANPARAM.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/StaticHoldItem.h"

//#include "../../ModalCallerID.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../InnerInterface.h"
#include "MarketWindow.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../Interface/ChatType.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CInventoryWindow::CInventoryWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pPage( NULL )
	, m_pPageWear( NULL )
	, m_pPageWearEx( NULL )
	, m_pMoneyTextBox( NULL )
	, m_nONE_VIEW_SLOT( 4 )
	, m_bLock( FALSE )
    , m_pItemSortButton( NULL )
{
}

CInventoryWindow::~CInventoryWindow()
{
}

void CInventoryWindow::CreateSubControl()
{
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    const DxCharJoinData* pJoinData = pGlobalStage->GetGameStage()->GetCharJoinData();
	const EMCHARCLASS emCharClass = pJoinData->m_CharData2.m_emClass;
	const int nClassType = CharClassToIndex( emCharClass );

	// 극강부와 일반 캐릭터를 구분해서 인벤토리 장착 윈도우를 생성한다.
	if( nClassType == GLCI_EXTREME_W || nClassType == GLCI_EXTREME_M )
	{
		m_pPageWearEx = new CInventoryPageWearEx( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		m_pPageWearEx->CreateSub( this, "INVENTORY_PAGE_WEAR", UI_FLAG_DEFAULT, INVENTORY_PAGEWEAR_EX );
		m_pPageWearEx->CreateSubControl( nClassType );
		RegisterControl( m_pPageWearEx );
	}
	else
	{
		m_pPageWear = new CInventoryPageWear( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		m_pPageWear->CreateSub( this, "INVENTORY_PAGE_WEAR", UI_FLAG_DEFAULT, INVENTORY_PAGEWEAR );
		m_pPageWear->CreateSubControl( nClassType );
		RegisterControl( m_pPageWear );
	}

	m_pPage = new CInventoryPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	m_pPage->CreateSub( this, "INVENTORY_PAGE", UI_FLAG_YSIZE, INVENTORY_PAGE );
	m_pPage->CreateSubControl();
	RegisterControl( m_pPage );

	CUIControl* pControl = CreateControl( "INVENTORY_MONEY_BACK" );
	pControl->SetAlignFlag( UI_FLAG_BOTTOM );

	CBasicButton* pButton = NULL;
	pButton = CreateFlipButton( "INVENTORY_MONEY_BUTTON", "INVENTORY_MONEY_BUTTON_F", INVENTORY_MONEY_BUTTON, CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver( "INVENTORY_MONEY_BUTTON_F" );
	pButton->SetAlignFlag( UI_FLAG_BOTTOM );
	pButton->SetUseDynamic( TRUE );

    m_pItemSortButton = new CBasicTextButton(m_pEngineDevice);
    m_pItemSortButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, INVENTORY_ITEM_SORT_BUTTON );
    m_pItemSortButton->CreateBaseButton( "INVENTORY_ITEM_SORT_BUTTON", CBasicTextButton::SIZE19, 
        CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "INVENTORY_ITEM_SORT_BUTTON", 0 ) );
    m_pItemSortButton->SetAlignFlag( UI_FLAG_BOTTOM );
    RegisterControl( m_pItemSortButton );

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_pGaeaClient->GetServiceProvider() == SP_VIETNAM)
	{
		pButton = CreateFlipButton( "VNGAINSYS_INVEN_BUTTON", "VNGAINSYS_INVEN_BUTTON_F", INVENTORY_VNGAINSYS_BUTTON, CBasicButton::CLICK_FLIP );
		pButton->CreateMouseOver( "VNGAINSYS_INVEN_BUTTON_F" );
		pButton->SetAlignFlag( UI_FLAG_BOTTOM );
		pButton->SetUseDynamic( TRUE );
	}
//#endif

    CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pMoneyTextBox = CreateStaticControl( "INVENTORY_MONEY_TEXT", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMoneyTextBox->SetAlignFlag( UI_FLAG_BOTTOM );
}

void CInventoryWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	GLCHARLOGIC& ref_CharData = m_pGaeaClient->GetCharacterLogic();
	SetMoney( ref_CharData.GetInvenMoney() );

	GLInventory& ref_Inventory = ref_CharData.m_cInventory;	
	m_pPage->LoadItemPage( ref_Inventory );
}

void CInventoryWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( INVENTORY_WINDOW );
			}
		}
		break;
	case INVENTORY_PAGE:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				int nPosX, nPosY;
				m_pPage->GetItemIndex( &nPosX, &nPosY );

				//CDebugSet::ToView( 1, 1, "인벤 : %d %d", nPosX, nPosY );

				if ( nPosX < 0 || nPosY < 0 ) return;

                bool bMarketOpen = m_pInterface->IsOpenDialogueWindowMarketPage();
//				if ( CUIMan::GetFocusControl() == this )
				{
					const SINVENITEM* pInvenItem = m_pPage->GetItem( nPosX, nPosY );
					if ( pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()) )
					{
						m_pInterface->SHOW_ITEM_INFO(
							pInvenItem->sItemCustom,
							bMarketOpen, FALSE, FALSE, TRUE, FALSE,
							pInvenItem->wPosX, pInvenItem->wPosY );
					}
				}

				if ( bMarketOpen )
				{
                    if ( RANPARAM::bCheckSellItemRButton )
                    {
					    if ( CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
					    {
                            if ( holditem::HaveHoldItem() )
                                return;

                            const SINVENITEM* pInvenItem = m_pPage->GetItem( nPosX, nPosY );
                            const SITEM* pItemData = GLogicData::GetInstance().GetItem( pInvenItem->Id() );
                            if ( !pItemData )
                                return;

                            if ( RANPARAM::bCheckConfirmSellItem )
                            {
                                const std::string& strItemName = pInvenItem->sItemCustom.GETNAME();
                                std::string strCombine = sc::string::format( ID2GAMEINTEXT( "SELL_ITEM_CONFIRM" ), strItemName );
                                m_pInterface->ModalWindowSetModalData( nPosX, nPosY );
                                m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_DIRECT_SELL_ITEM );
                            }
                            else
                            {
						        const SNATIVEID& sNPCID = m_pInterface->GetDialogueWindowNPCID();
                                const DWORD dwNPCGlobalID = m_pInterface->GetDialogueWindowNPCGlobID();
						        const int nPageIndex = m_pInterface->GetDialogueWindowPageIndex();

                                m_pGaeaClient->GetCharacter()->ReqNpcDirectSaleTo( sNPCID, dwNPCGlobalID, nPosX, nPosY );
                            }

						    return;
					    }
                    }
				}

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( m_bSplitItem && !m_bLock )
					{
						m_bSplitItem = FALSE;

						WORD wSplitPosX, wSplitPosY;
						GetSplitPos( &wSplitPosX, &wSplitPosY );
						BOOL bDiffPos = !( nPosX == wSplitPosX && nPosY == wSplitPosY );
						BOOL bSplitable = m_pGaeaClient->GetCharacter()->IsInvenSplitItem( wSplitPosX, wSplitPosY );
                        if ( bDiffPos && bSplitable )
						{
							m_pInterface->DoModal( ID2GAMEINTEXT( "SPLIT_ITEM" ), UI::MODAL_QUESTION, UI::EDITBOX_NUMBER, UI::MODAL_SPLIT_ITEM );
						}
						else
						{
							m_pGaeaClient->GetCharacter()->ReqInvenTo( nPosX, nPosY );
						}
					}
					return ;
				}

				if ( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED )
				{
					if ( dwMsg & UIMSG_RB_UP )
					{
						const SINVENITEM* pInvenItem = m_pPage->GetItem ( nPosX, nPosY );
						if (pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()))	
						{
							SINVEN_POS sInvenPos;
							sInvenPos.wPosX = nPosX;
							sInvenPos.wPosY = nPosY;

							SLINK_DATA_BASIC sLinkDataBasic;
							sLinkDataBasic.sLinkType.wMType = ITEM_LINK;
							sLinkDataBasic.sLinkType.wSType = ITEM_LINK_INVEN;
							sLinkDataBasic.dwData1 = sInvenPos.dwPos;
						
							m_pInterface->ChatToLink(sLinkDataBasic, &pInvenItem->sItemCustom);
                            return;
						}                        
					}					
				}

                // 잠금 상태 체크
                if ( dwMsg & UIMSG_LB_DOWN )
                {
                    if ( m_bLock )
                    {
                        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("INVEN_LOCK_MSG") );
                        return ;
                    }

                    SetSplitPos( nPosX, nPosY );
                    m_bSplitItem = TRUE;
                    return ;
                }

                // 잠금 상태 체크
                if ( dwMsg & UIMSG_RB_UP )
                {
                    if ( m_bLock )
                    {
                        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("INVEN_LOCK_MSG") );
                        return ;
                    }
                }

				if ( dwMsg & UIMSG_RB_UP )
				{
                    // 아이템 판매
                    // (11-08-09) 실수로 팔아버리는 일이 많아서 판매하지는 않는다.
                    //if ( m_pInterface->IsMarketWindowOpen() )
                    //{
                    //    const SINVENITEM* pInvenItem = m_pPage->GetItem ( nPosX, nPosY );
                    //    const SITEM* pItemData = GLogicData::GetInstance().GetItem( pInvenItem->Id() );

                    //    if ( !pItemData )
                    //    {
                    //        return;
                    //    }

                    //    if ( !pItemData->sBasicOp.IsSALE() )
                    //    {
                    //        //	팔기 가능하지 않음.
                    //        return;
                    //    }

                    //    if ( pInvenItem->sItemCustom.IsGM_GENITEM() )
                    //    {
                    //        //	팔기 가능하지 않음.
                    //        return;
                    //    }

                    //    std::string strCombine = sc::string::format( ID2GAMEINTEXT("SELL_ITEM_CONFIRM"), pItemData->GetName() ).c_str();
                    //    m_pInterface->ModalWindowSetModalData( nPosX, nPosY );
                    //    m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_SELL_ITEM_FROM_INVEN );
                    //}
                    //// 아이템 사용
                    //else
                    {
                        m_pGaeaClient->GetCharacter()->ReqInvenDrug( nPosX, nPosY );
                    }

                    return ;
				}
			}
		}
		break;

	case INVENTORY_PAGEWEAR:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				int nIndex = m_pPageWear->GetItemIndex();
				if ( nIndex < 0 ) return ;				

				//if ( CUIMan::GetFocusControl() == this )
				{
					SITEMCUSTOM sItemCustom = m_pPageWear->GetItem( nIndex );
					if ( sItemCustom.GetNativeID() != NATIVEID_NULL() )
					{
						m_pInterface->SHOW_ITEM_INFO(
							sItemCustom,
							FALSE, FALSE, FALSE, FALSE, FALSE,
							USHRT_MAX, USHRT_MAX );
					}
				}

				if ( (dwMsg & UIMSG_LB_UP) )
				{
					if ( m_bLock )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("INVEN_LOCK_MSG") );
						return ;
					}

					EMSLOT emSlot = m_pPageWear->GetItemSlot();
					m_pGaeaClient->GetCharacter()->ReqSlotTo( emSlot );
				}

				if ( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED )
				{
					if ( dwMsg & UIMSG_RB_UP )
					{
						SITEMCUSTOM sItemCustom = m_pPageWear->GetItem( nIndex );
						if ( sItemCustom.GetNativeID() != NATIVEID_NULL() )
						{
							EMSLOT emSlot = m_pPageWear->GetItemSlot();

							if ( m_pGaeaClient->GetCharacter()->VALID_SLOT_ITEM(emSlot) ) 
							{
								SLINK_DATA_BASIC sLinkDataBasic;
								sLinkDataBasic.sLinkType.wMType = ITEM_LINK;
								sLinkDataBasic.sLinkType.wSType = ITEM_LINK_PUTON;
								sLinkDataBasic.dwData1 = emSlot;
							
								m_pInterface->ChatToLink(sLinkDataBasic, &sItemCustom);
								return ;
							}
						}
					}
				}

				if ( dwMsg & UIMSG_RB_UP )
				{	// 코스튬 염색을 위한 호출. by luxes.
					EMSLOT emSlot = m_pPageWear->GetItemSlot();
					m_pGaeaClient->GetCharacter()->ReqSlotConsume( emSlot );
					return ;
				}
			}
		}
		break;

	case INVENTORY_PAGEWEAR_EX:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				int nIndex = m_pPageWearEx->GetItemIndex();
				if ( nIndex < 0 ) return ;				

				//if ( CUIMan::GetFocusControl() == this )
				{
					SITEMCUSTOM sItemCustom = m_pPageWearEx->GetItem( nIndex );
					if ( sItemCustom.GetNativeID() != NATIVEID_NULL() )
					{
						m_pInterface->SHOW_ITEM_INFO(
							sItemCustom,
							FALSE, FALSE, FALSE, FALSE, FALSE,
							USHRT_MAX, USHRT_MAX );
					}
				}

				if ( (dwMsg & UIMSG_LB_UP) )
				{
					if ( m_bLock )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("INVEN_LOCK_MSG") );
						return ;
					}

					EMSLOT emSlot = m_pPageWearEx->GetItemSlot();
					m_pGaeaClient->GetCharacter()->ReqSlotTo( emSlot );
				}

				if ( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED )
				{
					if ( dwMsg & UIMSG_RB_UP )
					{
						SITEMCUSTOM sItemCustom = m_pPageWearEx->GetItem( nIndex );
						if ( sItemCustom.GetNativeID() != NATIVEID_NULL() ) 
						{
						
							EMSLOT emSlot = m_pPageWearEx->GetItemSlot();

							if ( m_pGaeaClient->GetCharacter()->VALID_SLOT_ITEM(emSlot) ) 
							{
								SLINK_DATA_BASIC sLinkDataBasic;
								sLinkDataBasic.sLinkType.wMType = ITEM_LINK;
								sLinkDataBasic.sLinkType.wSType = ITEM_LINK_PUTON;
								sLinkDataBasic.dwData1 = emSlot;
							
								m_pInterface->ChatToLink(sLinkDataBasic, &sItemCustom);
								return ;
							}
						}
					}
				}

				if ( dwMsg & UIMSG_RB_UP )
				{	// 코스튬 염색을 위한 호출. by luxes.
					EMSLOT emSlot = m_pPageWearEx->GetItemSlot();
					m_pGaeaClient->GetCharacter()->ReqSlotConsume( emSlot );
					return ;
				}
			}
		}
		break;

	case INVENTORY_MONEY_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if ( m_pInterface->IsStorageWindowOpen() )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("PLAYINTERFACE_31"), UI::MODAL_INPUT, UI::EDITBOX_MONEY, UI::MODAL_SAVEMONEY );
				}
				else if ( m_pInterface->IsClubStorageWindowOpen() )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("PLAYINTERFACE_31"), UI::MODAL_INPUT, UI::EDITBOX_MONEY, UI::MODAL_CLUB_SAVEMONEY );
				}
				else
				{
					if ( GLCONST_CHAR::bMONEY_DROP2FIELD )
					{
						m_pInterface->DoModal( ID2GAMEINTEXT("MONEYTO_FIELD"), UI::MODAL_INPUT, UI::EDITBOX_MONEY, UI::MODAL_INVENTORY_MONEY_TO_FIELD );
					}
				}
			}
		}
		break;

    case INVENTORY_ITEM_SORT_BUTTON:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                if ( m_pGaeaClient->GetCharacter()->IsEnalbeItemSort() )
                    m_pGaeaClient->GetCharacter()->ReqInvenItemSort();
                else
                    m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("ITEM_SORT_DELAY_TIME") );

            }
        }
        break;
//#if defined(VN_PARAM) //vietnamtest%%%
	case INVENTORY_VNGAINSYS_BUTTON:
		{
			if (m_pGaeaClient->GetServiceProvider() != SP_VIETNAM)
				break;

			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( (dwMsg & UIMSG_LB_UP) )
				{
					if ( m_bLock )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("INVEN_LOCK_MSG") );
						return ;
					}

					if ( !m_pInterface->UiIsVisibleGroup( VNGAINSYS_WINDOW ) )
					{
						m_pInterface->UiShowGroupFocus( VNGAINSYS_WINDOW );
					}
					else
					{
						m_pInterface->UiHideGroup( VNGAINSYS_WINDOW );
					}
				}
			}
		}
		break;
//#endif
	case ET_CONTROL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
                if ( m_pInterface->UiIsVisibleGroup( MARKET_WINDOW ) )
                    m_pInterface->SetMarketWindowClose();

                if ( m_pInterface->UiIsVisibleGroup( STORAGE_WINDOW ) )
                    m_pInterface->SetStorageWindowClose();

                if ( m_pInterface->UiIsVisibleGroup( CLUB_STORAGE_WINDOW ) )
                    m_pInterface->SetClubStorageWindowClose();
			}
		}
		break;
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CInventoryWindow::SetMoney( LONGLONG Money )
{
	//	돈자리수
	CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( Money, 3, "," );
	m_pMoneyTextBox->SetOneLineText( strTemp );
}

void CInventoryWindow::GetSplitPos( WORD* pwPosX, WORD* pwPosY )
{
	*pwPosX = m_wSplitItemPosX;
	*pwPosY = m_wSplitItemPosY;
}

void CInventoryWindow::SetSplitPos( WORD wPosX, WORD wPosY )
{
	m_wSplitItemPosX = wPosX;
	m_wSplitItemPosY = wPosY;
}

void	CInventoryWindow::SetOneViewSlot( const int& nONE_VIEW_SLOT )
{
	CUIControl TempControl(m_pEngineDevice);
	TempControl.Create( 1, "INVENTORY_ITEM_SLOT0" );

	CUIControl TempControl2(m_pEngineDevice);
	TempControl2.Create( 2, "INVENTORY_ITEM_SLOT1" );

	const UIRECT& rcTempGlobalPos = TempControl.GetGlobalPos();
	D3DXVECTOR2 vONE_SLOT_SIZE( rcTempGlobalPos.sizeX, rcTempGlobalPos.sizeY );

	const UIRECT& rcTempGlobalPos2 = TempControl2.GetGlobalPos();

	const float fSlotGap = rcTempGlobalPos2.top - rcTempGlobalPos.bottom;

	const float fOldSizeY = (vONE_SLOT_SIZE.y+fSlotGap) * m_nONE_VIEW_SLOT;
	const float fNewSizeY = (vONE_SLOT_SIZE.y+fSlotGap) * nONE_VIEW_SLOT;
    const float fDIFF_SIZE_Y = fNewSizeY - fOldSizeY;

	const UIRECT& rcGlobalPos = GetGlobalPos();
	UIRECT rcNewGlobalPos ( rcGlobalPos.left, rcGlobalPos.top,
		rcGlobalPos.sizeX, rcGlobalPos.sizeY + fDIFF_SIZE_Y );

	WORD wFlagAlignBACK = GetAlignFlag();
	SetAlignFlag( UI_FLAG_YSIZE );
	
	AlignSubControl( rcGlobalPos, rcNewGlobalPos );
	SetGlobalPos( rcNewGlobalPos );
	
	SetAlignFlag( wFlagAlignBACK );

	m_pPage->SetOneViewSlot( nONE_VIEW_SLOT );
	m_nONE_VIEW_SLOT = nONE_VIEW_SLOT;
}

void CInventoryWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	if( bVisible )
	{
		if( m_pPageWearEx )
		{
			if( m_pGaeaClient->GetCharacter()->IsUseArmSub() )
			{
				m_pPageWearEx->SetTabButton( FALSE );
			}
			else
			{
				m_pPageWearEx->SetTabButton( TRUE );
			}

		}
	}
}

void CInventoryWindow::SetArmSwapTabButton( BOOL bArmSub )
{
	if( m_pPageWearEx )
	{
		m_pPageWearEx->SetTabButton( bArmSub );
	}
}

MyInventoryWindow::MyInventoryWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CInventoryWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyInventoryWindow::CreateUIWindowAndRegisterOwnership()
{
    CInventoryWindow::Create( INVENTORY_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CInventoryWindow::CreateBaseWindowLightGray( "INVENTORY_WINDOW", (char*)ID2GAMEWORD("INVENTORY_NAME_STATIC") );
    CInventoryWindow::CreateSubControl();
    CInventoryWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
    CInventoryWindow::m_pInterface->UiRegisterControl( this, true );
    CInventoryWindow::m_pInterface->UiShowGroupFocus( INVENTORY_WINDOW );
}

void MyInventoryWindow::SetOneViewSlot( const int& nONE_VIEW_SLOT )
{
    CInventoryWindow::SetOneViewSlot( nONE_VIEW_SLOT );
}

const int& MyInventoryWindow::GetOneViewSlot() const
{
    return CInventoryWindow::GetOneViewSlot();
}

void MyInventoryWindow::SetArmSwapTabButton( BOOL bArmSub )
{
    CInventoryWindow::SetArmSwapTabButton( bArmSub );
}

const UIRECT& MyInventoryWindow::GetUIWindowGlobalPos()
{
    return CInventoryWindow::GetGlobalPos();
}

const UIRECT& MyInventoryWindow::GetInventoryPageGlobalPos()
{
    return CInventoryWindow::GetInventoryGlobalPos();
}

void MyInventoryWindow::LockTrade()
{
    CInventoryWindow::SetLock();
}

void MyInventoryWindow::UnLockTrade()
{
    CInventoryWindow::SetUnLock();
}

const bool MyInventoryWindow::IsLocked ()
{
    return CInventoryWindow::IsLocked ();
}

void MyInventoryWindow::GetSplitPos( WORD* pwPosX, WORD* pwPosY )
{
    CInventoryWindow::GetSplitPos( pwPosX, pwPosY );
}
