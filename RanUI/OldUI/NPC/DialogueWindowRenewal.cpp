#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../../RanLogic/Crow/GLCrowDefine.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"

#include "./NPCDialoguePage.h"
#include "./NPCMarketPage.h"
#include "./NPCPromotionPage.h"
#include "./NPCRebuyPage.h"
#include "./DialogueWindowRenewal.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CDialogueWindowRenewal::CDialogueWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIWindowEx( pInterface, pEngineDevice )
    , m_pGaeaClient(pGaeaClient)
    , m_pWindowBack(NULL)
    , m_pBack(NULL)
    , m_pDialoguePage(NULL)
    , m_pMarketPage(NULL)
    , m_pPromotionPage(NULL)
    , m_pRebuyPage(NULL)

    , m_nCurTabPage(TAB_DIALOGUE_PAGE)
    , m_nRebuyTabIndex(-1)
    , m_bEnableMarket(true)
{
    for ( int i = 0; i < MAX_DIALOGUE_WIN_TAB; i++ )
    {
        m_pActiveTab[ i ] = NULL;
        m_pInActiveTab[ i ] = NULL;
        m_pTabText[ i ] = NULL;

        m_bEnableTab[ i ] = false;
    }

    m_bEnableTab[ 0 ] = true;
}

CDialogueWindowRenewal::~CDialogueWindowRenewal()
{
}

void CDialogueWindowRenewal::CreateSubControl()
{
    {    
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_EX_FLAG );

        char* strActiveTab[ MAX_DIALOGUE_WIN_TAB ] =
        {
            "DIALOGUE_WINDOW_ACTIVE_TAB0"
            , "DIALOGUE_WINDOW_ACTIVE_TAB1"
            , "DIALOGUE_WINDOW_ACTIVE_TAB2"
            , "DIALOGUE_WINDOW_ACTIVE_TAB3"
            , "DIALOGUE_WINDOW_ACTIVE_TAB4"
        };

        char* strInActiveTab[ MAX_DIALOGUE_WIN_TAB ] =
        {
            "DIALOGUE_WINDOW_INACTIVE_TAB0"
            , "DIALOGUE_WINDOW_INACTIVE_TAB1"
            , "DIALOGUE_WINDOW_INACTIVE_TAB2"
            , "DIALOGUE_WINDOW_INACTIVE_TAB3"
            , "DIALOGUE_WINDOW_INACTIVE_TAB4"
        };

        char* strActiveTabLine[ MAX_DIALOGUE_WIN_TAB ] =
        {
            "DIALOGUE_WINDOW_ACTIVE_TAB0_LINE"
            , "DIALOGUE_WINDOW_ACTIVE_TAB1_LINE"
            , "DIALOGUE_WINDOW_ACTIVE_TAB2_LINE"
            , "DIALOGUE_WINDOW_ACTIVE_TAB3_LINE"
            , "DIALOGUE_WINDOW_ACTIVE_TAB4_LINE"
        };

        char* strInActiveTabLine[ MAX_DIALOGUE_WIN_TAB ] =
        {
            "DIALOGUE_WINDOW_INACTIVE_TAB0_LINE"
            , "DIALOGUE_WINDOW_INACTIVE_TAB1_LINE"
            , "DIALOGUE_WINDOW_INACTIVE_TAB2_LINE"
            , "DIALOGUE_WINDOW_INACTIVE_TAB3_LINE"
            , "DIALOGUE_WINDOW_INACTIVE_TAB4_LINE"
        };

        char* strUITabText[ MAX_DIALOGUE_WIN_TAB ] =
        {
            "DIALOGUE_WINDOW_TAB_TEXT_0"
            , "DIALOGUE_WINDOW_TAB_TEXT_1"
            , "DIALOGUE_WINDOW_TAB_TEXT_2"
            , "DIALOGUE_WINDOW_TAB_TEXT_3"
            , "DIALOGUE_WINDOW_TAB_TEXT_4"
        };

        for ( int i = 0; i < MAX_DIALOGUE_WIN_TAB; i++ )
        {
            CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
            pLinBoxSmart->CreateSub( this, strActiveTab[ i ], UI_FLAG_XSIZE | UI_FLAG_YSIZE, (DIALOGUE_TAB_0 + i) );
            pLinBoxSmart->CreateSubControl( strActiveTabLine[ i ] );
            RegisterControl( pLinBoxSmart );
            m_pActiveTab[ i ] = pLinBoxSmart;

            pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
            pLinBoxSmart->CreateSub( this, strInActiveTab[ i ], UI_FLAG_XSIZE | UI_FLAG_YSIZE, (DIALOGUE_IN_TAB_0 + i) );
            pLinBoxSmart->CreateSubControl( strInActiveTabLine[ i ] );
            RegisterControl ( pLinBoxSmart );
            m_pInActiveTab[ i ] = pLinBoxSmart;
            m_pInActiveTab[ i ]->SetVisibleSingle( FALSE );

            CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
            pTextBox->CreateSub( this, strUITabText[ i ], UI_FLAG_DEFAULT );
            pTextBox->SetFont( pFont10Shadow );
            pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
            RegisterControl( pTextBox );
            m_pTabText[ i ] = pTextBox;
        }

        m_pTabText[ 0 ]->SetText( ID2GAMEWORD("DIALOGUE_WINDOW_STATIC", 0 ), NS_UITEXTCOLOR::WHITE );

        SelectTab( TAB_DIALOGUE_PAGE ); // default tab select.
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "DIALOGUE_WINDOW_RENEWAL_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "DIALOGUE_WINDOW_RENEWAL_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pWindowBack = pLinBoxSmart;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "DIALOGUE_WINDOW_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "DIALOGUE_WINDOW_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pBack = pLinBoxSmart;
    }

    {
        CNPCDialoguePage* pDialoguePage = new CNPCDialoguePage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pDialoguePage->CreateSub( this, "DIALOGUE_WINDOW_RENEWAL", UI_FLAG_DEFAULT, DIALOGUE_PAGE );
        pDialoguePage->CreateSubControl();
        RegisterControl( pDialoguePage );
        m_pDialoguePage = pDialoguePage;
    }

    {
        CNPCMarketPage* pMarketPage = new CNPCMarketPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pMarketPage->CreateSub( this, "DIALOGUE_WINDOW_RENEWAL", UI_FLAG_DEFAULT, MARKET_PAGE );
        pMarketPage->CreateSubControl();
        RegisterControl( pMarketPage );
        m_pMarketPage = pMarketPage;
    }

    {
        CNPCRebuyPage* pRebuyPage = new CNPCRebuyPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pRebuyPage->CreateSub( this, "DIALOGUE_WINDOW_RENEWAL", UI_FLAG_DEFAULT, REBUY_PAGE );
        pRebuyPage->CreateSubControl();
        RegisterControl( pRebuyPage );
        m_pRebuyPage = pRebuyPage;
    }

    // Note : 아이템 소개 페이지.
    {
        CNPCPromotionPage* pPromotionPage = new CNPCPromotionPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pPromotionPage->CreateSub( this, "DIALOGUE_WINDOW_RENEWAL", UI_FLAG_DEFAULT, PROMOTION_PAGE );
        pPromotionPage->CreateSubControl();
        RegisterControl( pPromotionPage );
        m_pPromotionPage = pPromotionPage;
    }
}

void CDialogueWindowRenewal::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CDialogueWindowRenewal::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case DIALOGUE_IN_TAB_0:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                int nIndex = ControlID - DIALOGUE_IN_TAB_0;
                SelectTab( nIndex );
            }
        }
        break;

    case DIALOGUE_IN_TAB_1:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                int nIndex = ControlID - DIALOGUE_IN_TAB_0;
                SelectTab( nIndex );
            }
        }
        break;

    case DIALOGUE_IN_TAB_2:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                int nIndex = ControlID - DIALOGUE_IN_TAB_0;
                SelectTab( nIndex );
            }
        }
        break;

    case DIALOGUE_IN_TAB_3:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                int nIndex = ControlID - DIALOGUE_IN_TAB_0;
                SelectTab( nIndex );
            }
        }
        break;

    case DIALOGUE_IN_TAB_4:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                int nIndex = ControlID - DIALOGUE_IN_TAB_0;
                SelectTab( nIndex );
            }
        }
        break;
    };
}

void CDialogueWindowRenewal::SetVisibleSingle( BOOL bVisible )
{
    CUIWindowEx::SetVisibleSingle( bVisible );
    if( bVisible == FALSE )
        m_pInterface->ItemRepairWindowOpen( false );
}

bool CDialogueWindowRenewal::SetDialogueData( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID, GLCHARLOGIC* pCharLogic )
{
    std::tr1::shared_ptr<GLCrowClient> pCrow = m_pGaeaClient->GetActiveMap()->GetCrow( dwNPCGlobalID );
    if ( !pCrow )
        return false;

    if ( pCrow->GETCROW() != CROW_NPC )
        return false;

    if ( m_pDialoguePage->SetDialogueData( sNPCID, dwNPCGlobalID, pCharLogic ) )
    {
        m_pMarketPage->SetMarketData( sNPCID, dwNPCGlobalID );
        m_pRebuyPage->SetNPCID( sNPCID, dwNPCGlobalID );
        m_pPromotionPage->SetRecommendItemImage();

        CheckMarketTab( dwNPCGlobalID );
        SelectTab( TAB_DIALOGUE_PAGE );

        return true;
    }

    return false;
}

void CDialogueWindowRenewal::CheckMarketTab( const DWORD dwNPCGlobalID )
{
    if (!m_bEnableMarket)
    {
        UnShowAllMarketTab();
        return;
    }

    std::tr1::shared_ptr<GLCrowClient> pCrowClinet = m_pGaeaClient->GetActiveMap()->GetCrow( dwNPCGlobalID );
    if ( !pCrowClinet )
    {
        UnShowAllMarketTab();
        return;
    }

    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
    if ( !pCharacter )
    {
        UnShowAllMarketTab();
        return;
    }

    // NPC 유효성 체크
    if ( !pCrowClinet->IsNpcActionable(pCharacter->GetPosition(),
        pCharacter->GETBODYRADIUS() ) )
    {
        UnShowAllMarketTab();
        return;
    }

    SCROWDATA* pCrowData = pCrowClinet->m_pCrowData;
    if ( pCrowData )
    {
        bool bMarketTab = false;
        int nCurTabIndex = 1;
        m_nRebuyTabIndex = -1;
        for ( int i = 0; i < SCROWACTION::SALENUM; i++ )
        {
            std::string& strFileName = pCrowData->m_sAction.m_strSaleFile[ i ];

            if ( strFileName.length() != 0 )
            {
                bMarketTab = true;

                m_bEnableTab[ nCurTabIndex ] = true;

				//  [12/31/2014 gbgim];
				// - NPC 대화창에서 탭 부분의 문자열을 삽입해주는 부분;
				// - 기존 데이터에는 문자열이 그대로 들어가있어서 사용하지 못함;
				// - 일단 기존 문자를 사용하고 데이터 통합이되면 문자들을 인덱싱화해서;
				// - 사용하기로함;
// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					const std::string& strMarketTabText = pCrowData->m_strSaleType[ i ];
// 					const int nSaleType = COMMENT::GetSaleType(strMarketTabText);
// 					const CString strSaleType = CGameTextMan::GetInstance().GetText(
// 						"CROWSALE_SALETYPE"
// 						, nSaleType
// 						, CGameTextMan::EM_COMMENT_TEXT);
// 					if ( strMarketTabText.length() != 0 )
// 						m_pTabText[ nCurTabIndex ]->SetText( strSaleType.GetString(),
// 						NS_UITEXTCOLOR::WHITE );
// 				}
// 				else
// 				{
					std::string& strMarketTabText = pCrowData->m_strSaleType[ i ];
					if ( strMarketTabText.length() != 0 )
						m_pTabText[ nCurTabIndex ]->SetText( strMarketTabText.c_str(),
						NS_UITEXTCOLOR::WHITE );
//				}

                nCurTabIndex++;
            }
        }

        if ( bMarketTab )
        {
            m_bEnableTab[ nCurTabIndex ] = true;
            m_nRebuyTabIndex = nCurTabIndex;
            int nRebuyListCount = m_pRebuyPage->GetRebuyListCount();
            SetRebuyTabCountText( nRebuyListCount );
            nCurTabIndex++;
        }

        for ( int i = nCurTabIndex; i < MAX_DIALOGUE_WIN_TAB; i++ )
        {
            m_bEnableTab[ i ] = false;
            m_pTabText[ i ]->ClearText();
        }

        SetVisibleMarketTab();

        return;
    }
}

void CDialogueWindowRenewal::UnShowAllMarketTab()
{
    for ( int i = 1; i < MAX_DIALOGUE_WIN_TAB; i++ )
        m_bEnableTab[ i ] = false;

    SetVisibleMarketTab();
}

void CDialogueWindowRenewal::UpdateExChangeTab()
{
	if( m_pMarketPage->IsCurPageMarketType(GLInventorySale::CROW_SALE_V_2) )
	{
		m_pMarketPage->SetMarketPage( m_pMarketPage->GetPageIndex() );
	}
}

void CDialogueWindowRenewal::Refresh()
{
	CUIWindowEx::Refresh();

	UpdateExChangeTab();
}

void CDialogueWindowRenewal::SelectTab( int nIndex )
{
    if ( nIndex < 0 || nIndex >= MAX_DIALOGUE_WIN_TAB )
        return;

    m_nCurTabPage = nIndex;

    SetVisibleTab( m_nCurTabPage );

    AllUnShowPage();

    if ( m_nCurTabPage == m_nRebuyTabIndex )
        m_nCurTabPage = TAB_REBUY_PAGE;

    switch ( m_nCurTabPage )
    {
    case TAB_DIALOGUE_PAGE:
        {
            if ( m_pDialoguePage )
                m_pDialoguePage->SetVisibleSingle( TRUE );
        };
        break;
        
    case TAB_MARKET_0_PAGE:
    case TAB_MARKET_1_PAGE:
    case TAB_MARKET_2_PAGE:
        {
            if ( m_pMarketPage )
            {
                int nIndex = m_nCurTabPage - TAB_MARKET_0_PAGE;
                m_pMarketPage->SetMarketPage( nIndex );
                m_pMarketPage->SetVisibleSingle( TRUE );
            }
        };
        break;

    case TAB_REBUY_PAGE:
        {
            if ( m_pRebuyPage )
            {
                m_pRebuyPage->LoadRebuyList();
                m_pRebuyPage->SetVisibleSingle( TRUE );
            }
        };
        break;
    };
}

void CDialogueWindowRenewal::AllUnShowPage()
{
    if ( m_pDialoguePage )
        m_pDialoguePage->SetVisibleSingle( FALSE );

    if ( m_pMarketPage )
        m_pMarketPage->SetVisibleSingle( FALSE );

    if ( m_pRebuyPage )
        m_pRebuyPage->SetVisibleSingle( FALSE );
}


void CDialogueWindowRenewal::SetVisibleTab( int nIndex )
{
    if ( nIndex < 0 || nIndex >= MAX_DIALOGUE_WIN_TAB )
        return;

    SetVisibleMarketTab();

    if ( m_bEnableTab[ nIndex ] )
    {
        m_pActiveTab[ nIndex ]->SetVisibleSingle( TRUE );
        m_pInActiveTab[ nIndex ]->SetVisibleSingle( FALSE );
    }
}

void CDialogueWindowRenewal::SetVisibleMarketTab()
{
    for ( int i = 0; i < MAX_DIALOGUE_WIN_TAB; i++ )
    {
        if ( m_bEnableTab[ i ] )
        {
            m_pActiveTab[ i ]->SetVisibleSingle( FALSE );
            m_pInActiveTab[ i ]->SetVisibleSingle( TRUE );
            m_pTabText[ i ]->SetVisibleSingle( TRUE );
        }
        else
        {
            m_pActiveTab[ i ]->SetVisibleSingle( FALSE );
            m_pInActiveTab[ i ]->SetVisibleSingle( FALSE );
            m_pTabText[ i ]->SetVisibleSingle( FALSE );
        }
    }
}

SNATIVEID CDialogueWindowRenewal::GetNPCID()
{
    return m_pMarketPage->GetNPCID();
}

DWORD CDialogueWindowRenewal::GetNPCGlobalID()
{
    return m_pMarketPage->GetNPCGlobalID();
}

int CDialogueWindowRenewal::GetPageIndex()
{
    return m_pMarketPage->GetPageIndex();
}

bool CDialogueWindowRenewal::IsCurPageMarketType(GLInventorySale::CROW_SALE_TYPE_VERSION type)
{
	return m_pMarketPage->IsCurPageMarketType(type);
}

void CDialogueWindowRenewal::GetItemIndex( int* pnPosX, int* pnPosY )
{
    m_pMarketPage->GetItemIndex( pnPosX, pnPosY );
}

const SINVENITEM* CDialogueWindowRenewal::GetItem( int nPosX, int nPosY ) const
{
    return m_pMarketPage->GetItem( nPosX, nPosY );
}

void CDialogueWindowRenewal::AddCartList( int nPageIndex, int nPosX, int nPosY, WORD wBuyNum /* = 1 */ )
{
    m_pMarketPage->AddCartList( nPageIndex, nPosX, nPosY, wBuyNum );
}

void CDialogueWindowRenewal::ResetCartListInDialogueWindowMarketPage( int nPageIndex, int nPosX, int nPosY )
{
    m_pMarketPage->ResetCartListInDialogueWindowMarketPage( nPageIndex, nPosX, nPosY );
}

void CDialogueWindowRenewal::LoadRebuyList()
{
    m_pRebuyPage->LoadRebuyList();
}

void CDialogueWindowRenewal::SetRebuyTabCountText( int nCount )
{
    if ( m_nRebuyTabIndex < 0 || m_nRebuyTabIndex >= MAX_DIALOGUE_WIN_TAB )
        return;

    if ( nCount <= 0 )
    {
        m_pTabText[ m_nRebuyTabIndex ]->SetText( ID2GAMEWORD( "DIALOGUE_TAB_REBUY", 0 ),
            NS_UITEXTCOLOR::WHITE );
    }
    else
    {
        std::string strRebuyTabText = sc::string::format( ID2GAMEWORD( "DIALOGUE_TAB_REBUY", 1 ), nCount );
        m_pTabText[ m_nRebuyTabIndex ]->SetText( strRebuyTabText.c_str(),
            NS_UITEXTCOLOR::WHITE );
    }
}

int CDialogueWindowRenewal::GetRebuyItemCount()
{
    return m_pRebuyPage->GetRebuyListCount();
}

bool CDialogueWindowRenewal::IsOpenMarketPage()
{
    if ( m_nCurTabPage == TAB_DIALOGUE_PAGE )
        return false;

    return true;
}

void CDialogueWindowRenewal::SetEnableMarketPage( bool bEnable )
{
    m_bEnableMarket = bEnable;
}

void CDialogueWindowRenewal::SetCheckRebuySupplies(bool bCheck )
{
    m_pRebuyPage->SetCheckRebuySupplies( bCheck );
}
