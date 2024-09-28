#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Crow/GLCrowClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/StaticHoldItem.h"

#include "../../InnerInterface.h"
#include "MarketWindow.h"
#include "MarketPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	int	CMarketWindow::nOUTOFRANGE = -1;

CMarketWindow::CMarketWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_nPageIndex(nOUTOFRANGE)
{
	memset( m_pPage, 0, sizeof( CMarketPage* ) * nMAXPAGE );
	memset( m_pPageButton, 0, sizeof( CBasicTextButton* ) * nMAXPAGE );	
}

CMarketWindow::~CMarketWindow()
{
}

void CMarketWindow::CreateSubControl()
{
	CString strButtonKeyword[nMAXPAGE] = 
	{
		"MARKET_WINDOW_PAGE_BUTTON0",
		"MARKET_WINDOW_PAGE_BUTTON1",
		"MARKET_WINDOW_PAGE_BUTTON2"
	};

	for ( int i = 0; i < nMAXPAGE; i++ )
	{
		m_pPage[i] = CreateMarketPage( MARKET_PAGE0 + i );		
		m_pPageButton[i] = CreateTextButton( strButtonKeyword[i].GetString(), MARKET_WINDOW_PAGE_BUTTON0 + i, (char*)ID2GAMEWORD( "MARKET_WINDOW_PAGE_BUTTON", i ) );
	}

	SetVisiblePage( MARKET_PAGE0 );
}

CMarketPage* CMarketWindow::CreateMarketPage( UIGUID ControlID )
{
	CMarketPage* pPage = new CMarketPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pPage->CreateSub( this, "MARKET_PAGE", UI_FLAG_DEFAULT, ControlID );
	pPage->CreateSubControl();
	RegisterControl( pPage );
	return pPage;
}

CBasicTextButton*  CMarketWindow::CreateTextButton( const char* szButton, UIGUID ControlID, const char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE19;
	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton( szButton, nBUTTONSIZE, CBasicButton::RADIO_FLIP, szText );
	RegisterControl( pTextButton );
	return pTextButton;
}

BOOL CMarketWindow::IsUsingMaketPage( int nPage )
{
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( CUIControlNpcActionable::GetNPCID() );

    if ( !pCrowData )
    {
        return FALSE;
    }

	GLInventory& ref_Inventory = *pCrowData->GetSaleInven(nPage);

	if( ref_Inventory.GetNumItems() > 0 )
		return TRUE;
	else
		return FALSE;
}

void CMarketWindow::SetVisiblePage( int nPage )
{
	if ( nPage < MARKET_PAGE0 || MARKET_PAGE2 < nPage )
	{
		GASSERT ( 0 && "영역을 넘어서는 페이지입니다." );
		return ;
	}

	{	//	리셋
		for ( int i = 0; i < nMAXPAGE; i++ )
		{
			m_pPage[i]->SetVisibleSingle( FALSE );
			m_pPageButton[i]->SetFlip( FALSE );
		}
	}

	m_nPageIndex = nPage - MARKET_PAGE0;
	m_pPage[m_nPageIndex]->SetVisibleSingle( TRUE );
	m_pPageButton[m_nPageIndex]->SetFlip( TRUE );
}

void CMarketWindow::InitMarket( SNATIVEID sNPCID, DWORD dwNPCGlobID )
{
    SetNpcActionableNPCID( sNPCID, dwNPCGlobID );

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( CUIControlNpcActionable::GetNPCID() );
	if ( pCrowData )
	{
		for ( int i = 0; i < nMAXPAGE; i++ )
		{
			GLInventory& ref_Inventory = *pCrowData->GetSaleInven(i);
			m_pPage[i]->UnLoadItemPage();
			m_pPage[i]->LoadItemPage( ref_Inventory );

// 			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 			{
// 				//	추후에 수정
// 				CString strTabText = pCrowData->GetSaleType(i).c_str();
// 				const int nSaleType = atoi(strTabText.GetString());
// 				const CString strSaleType = CGameTextMan::GetInstance().GetText(
// 					"CROWSALE_SALETYPE"
// 					, nSaleType
// 					, CGameTextMan::EM_COMMENT_TEXT);
// 				m_pPageButton[i]->SetOneLineText( strSaleType.GetString() );
// 			}
// 			else
// 			{
				//	추후에 수정
				CString strTabText = pCrowData->GetSaleType(i).c_str();
				m_pPageButton[i]->SetOneLineText( strTabText );
//			}
		}
	}	

	SetVisiblePage( MARKET_PAGE0 );
}

void CMarketWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !CUIControlNpcActionable::IsUseable ( m_pGaeaClient ) )
    {
        m_pInterface->SetMarketWindowClose();
        return;
    }

    CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CMarketWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );


    if( ControlID == ET_CONTROL_TITLE || ET_CONTROL_TITLE_F == ControlID )
	{
		if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->SetDefaultPosInterface( MARKET_WINDOW );
			return ;
		}
	}	
	if ( ControlID == ET_CONTROL_BUTTON )
	{
		if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			m_pInterface->SetMarketWindowClose();
			return ;
		}
	}

	if ( MARKET_WINDOW_PAGE_BUTTON0 <= ControlID && ControlID < MARKET_WINDOW_PAGE_BUTTON_END )
	{
		if ( CHECK_MOUSE_IN( dwMsg ) )
		{
			int nIndex = ControlID - MARKET_WINDOW_PAGE_BUTTON0;

			if ( dwMsg & UIMSG_LB_DOWN )
			{
				if ( IsUsingMaketPage( nIndex ) )
				{
					SetVisiblePage( MARKET_PAGE0 + nIndex );
				}
				else
				{	
					return;
				}

			}
		}
	}
	else if ( MARKET_PAGE0 <= ControlID && ControlID < MARKET_PAGE_END )
	{
		if ( CHECK_MOUSE_IN( dwMsg ) )
		{
			int nPageIndex = ControlID - MARKET_PAGE0;

			//	NOTE
			//		손에 들고 있는 아이템 처리
			if ( dwMsg & UIMSG_LB_UP || dwMsg & UIMSG_RB_UP )
			{
				SNATIVEID sNativeID = holditem::GetHoldItem().GetNativeID();
				if ( sNativeID != NATIVEID_NULL() )
				{
					//	NOTE
					//		물건 팔기
					const SITEM* pItemData = GLogicData::GetInstance().GetItem( sNativeID );
					if ( pItemData )
					{
						const std::string& strItemName = holditem::GetHoldItem().GETNAME();
						std::string strCombine = sc::string::format( ID2GAMEINTEXT("SELL_ITEM_CONFIRM"), strItemName.c_str() ).c_str();
						m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_SELL_ITEM );
					}
					else
					{
						GASSERT ( 0 && "아이템을 찾을 수 없습니다." );
					}

					return ;
				}
			}

			int nPosX, nPosY;
			m_pPage[nPageIndex]->GetItemIndex( &nPosX, &nPosY );			
			SetItemIndex( nPosX, nPosY );
			if ( nPosX < 0 || nPosY < 0 ) return ;

//			if ( CUIMan::GetFocusControl () == this )
			const SINVENITEM* pInvenItem = m_pPage[nPageIndex]->GetItem ( nPosX, nPosY );
			{				
				if (pInvenItem && pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL () )
				{
					m_pInterface->SHOW_ITEM_INFO(
						pInvenItem->sItemCustom,
						TRUE, TRUE, FALSE, TRUE, FALSE,
						pInvenItem->wPosX, pInvenItem->wPosY,
						CUIControlNpcActionable::GetNPCID() );
				}
			}

			if( dwMsg & UIMSG_LB_UP )
			{
				if( pInvenItem && pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL() )
				{
					if( DxInputDevice::GetInstance().GetKeyState( DIK_LSHIFT ) & DXKEY_PRESSED )
					{
						SNATIVEID sItemID = pInvenItem->sItemCustom.GetNativeID();
						const SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemID );
						if( !pItemData )
							return;

						GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
						if( !pLandMan )
							return;

						std::tr1::shared_ptr<GLCrowClient> pCrow = pLandMan->GetCrow( CUIControlNpcActionable::GetNPCGlobalID() );
						if( !pCrow )
							return;

						WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
						if( !pWishList )
							return;

						const GLMapAxisInfo& sMapAxis = pLandMan->GetMapAxisInfo();
						D3DXVECTOR3 vPos = pCrow->GetPosition();

						int nMapX = 0;
						int nMapY = 0;
						sMapAxis.Convert2MapPos( vPos.x, vPos.z, nMapX, nMapY );

						LONGLONG dwPrice = pCrow->m_pCrowData->GetNpcSellPrice( sItemID.Id() );
						if( dwPrice == 0 )
							dwPrice = pItemData->sBasicOp.dwBuyPrice;

						pWishList->NewNpcStoreItem(
							pInvenItem->sItemCustom,
							CUIControlNpcActionable::GetNPCID(),
							m_pGaeaClient->GetActiveMapID().getGaeaMapID(),
							nMapX,
							nMapY,
							dwPrice );
						return;
					}
				}
			}

			if ( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED )
			{
				//		아이템 바로 사기
				if ( dwMsg & UIMSG_LB_UP )
				{
                    SNATIVEID sNativeID = holditem::GetHoldItem().GetNativeID();
					if ( sNativeID != NATIVEID_NULL() ) return ;

					m_pGaeaClient->GetCharacter()->ReqNpcTo(
						CUIControlNpcActionable::GetNPCID(),
						CUIControlNpcActionable::GetNPCGlobalID(),
						m_nPageIndex,
						nPosX,
						nPosY );
					return;
				}


				if ( dwMsg & UIMSG_RB_UP )
				{
					const SINVENITEM* pInvenItem = m_pPage[nPageIndex]->GetItem ( nPosX, nPosY );
					{				
						if (pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()))
						{					
							SINVEN_POS sInvenPos;
							sInvenPos.wPosX = nPosX;
							sInvenPos.wPosY = nPosY;

							SLINK_DATA_BASIC sLinkDataBasic;
							sLinkDataBasic.sLinkType.wMType = ITEM_LINK;
							sLinkDataBasic.sLinkType.wSType = ITEM_LINK_CROW_SALE;
							
							DWORD dwData = ( CUIControlNpcActionable::GetNPCGlobalID() << 16 );
							dwData |= ( m_nPageIndex & 0x0000FFFF );

							sLinkDataBasic.dwData1 = dwData;
							sLinkDataBasic.dwData2 = sInvenPos.dwPos;					
							
							m_pInterface->ChatToLink(sLinkDataBasic, &pInvenItem->sItemCustom);
							return ;
						}
					}
				}
			}

			if ( dwMsg & UIMSG_LB_UP )
			{
				SNATIVEID sNativeID = holditem::GetHoldItem().GetNativeID();
				if ( sNativeID != NATIVEID_NULL() ) return ;

				const SITEM* pITEM = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
				if ( pITEM )
				{
					// Note : 겹침이 가능하고 한개씩 파는 아이템의 경우 구입 수량 입력. ( 화살묶음, 부적묶음. )
					if( pITEM->ISPILE() && (pITEM->GETAPPLYNUM()==1) )
					{
						m_pInterface->DoModal( ID2GAMEINTEXT( "BUY_PILE_ITEM" ), UI::MODAL_INPUT, UI::EDITBOX_NUMBER, UI::MODAL_PILE_ITEM );
						return ;
					}
					else
					{
						//	NOTE
						//		물건 사기							
						const std::string& strItemName = pInvenItem->sItemCustom.GETNAME();
						std::string strCombine = sc::string::format( ID2GAMEINTEXT("BUY_ITEM_CONFIRM"), strItemName.c_str() ).c_str();
						m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_BUY_ITEM );							
						return ;
					}
				}
			}
		}
	}
}

void CMarketWindow::GetItemIndex( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}

void CMarketWindow::SetItemIndex( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

int	 CMarketWindow::GetPageIndex()
{
	return m_nPageIndex;
}

SNATIVEID	CMarketWindow::GetMarketID()
{
	return GetNPCID();
}

const SINVENITEM* CMarketWindow::GetItem( int nPosX, int nPosY ) const
{
	return m_pPage[m_nPageIndex]->GetItem( nPosX, nPosY );
}

MyMarketWindow::MyMarketWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CMarketWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyMarketWindow::CreateUIWindowAndRegisterOwnership()
{
    CMarketWindow::Create( MARKET_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CMarketWindow::CreateBaseWindowLightGray( "MARKET_WINDOW", (char*)ID2GAMEWORD("MARKET_NAME_STATIC") );
    CMarketWindow::CreateSubControl();
    CMarketWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
    CMarketWindow::m_pInterface->UiRegisterControl( this, true );
    CMarketWindow::m_pInterface->UiRegisterNpcActionableControl( this );
    CMarketWindow::m_pInterface->UiShowGroupFocus( MARKET_WINDOW );
}

void MyMarketWindow::InitMarket( SNATIVEID sNPCID, DWORD dwNpcGlobID )
{
    CMarketWindow::InitMarket( sNPCID, dwNpcGlobID );
}

void MyMarketWindow::GetItemIndex( int* pnPosX, int* pnPosY )
{
    CMarketWindow::GetItemIndex( pnPosX, pnPosY );
}

const SINVENITEM* MyMarketWindow::GetItem( int nPosX, int nPosY ) const
{
    return CMarketWindow::GetItem( nPosX, nPosY );
}

SNATIVEID MyMarketWindow::GetMarketID()
{
    return CMarketWindow::GetMarketID();
}

int MyMarketWindow::GetPageIndex()
{
    return CMarketWindow::GetPageIndex();
}

SNATIVEID MyMarketWindow::GetNPCID()
{
    return CMarketWindow::GetNPCID();
}

DWORD MyMarketWindow::GetNPCGlobalID()
{
    return CMarketWindow::GetNPCGlobalID();
}
