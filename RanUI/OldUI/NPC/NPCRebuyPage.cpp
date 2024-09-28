#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DeviceMan.h"

#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/StaticHoldItem.h"

#include "../../InnerInterface.h"
#include "./RebuyListNode.h"
#include "./NPCRebuyPage.h"

const int CNPCRebuyPage::s_nRebuyListColumnNum = 2;
const int CNPCRebuyPage::s_nRebuyListLinePerOneView = 9;

CNPCRebuyPage::CNPCRebuyPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup( pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
    , m_pInterface( pInterface )
    , m_pRebuyPageBack(NULL)
    , m_pListScrollBar(NULL)
    , m_pRebuyListNodeLeftTemp(NULL)
    , m_pRebuyListNodeRightTemp(NULL)
    , m_pNotifyAllDelItems(NULL)
    , m_pCheckRebuySupplies(NULL)
    , m_pUncheckRebuySupplies(NULL)
    , m_pRebuySuppliesText(NULL)
    , m_pCheckAllDelItems(NULL)
    , m_pUncheckAllDelItems(NULL)
    , m_pAllDelItemsText(NULL)
    , m_pCheckSellRBtn(NULL)
    , m_pUncheckSellRBtn(NULL)
    , m_pSellRBtnText(NULL)
    , m_pCheckConfirmSell(NULL)
    , m_pUncheckConfirmSell(NULL)
    , m_pConfirmSellText(NULL)

    , m_nTotalLine(0)
    , m_nRebuyListStartIndex(0)
{
    m_vecRebuy.clear();
}

CNPCRebuyPage::~CNPCRebuyPage()
{
}

void CNPCRebuyPage::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmart->CreateSub( this, "DIALOGUE_WINDOW_REBUY_LIST_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE, REBUY_PAGE_BACK );
        pLinBoxSmart->CreateSubControl( "DIALOGUE_WINDOW_REBUY_LIST_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pRebuyPageBack = pLinBoxSmart;

        //	스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, REBUY_LIST_SCROLLBAR );
        pScrollBar->CreateBaseScrollBar( "DIALOGUE_WINDOW_REBUY_LIST_SCROLLBAR" );
        pScrollBar->GetThumbFrame()->SetState( 1, m_nTotalLine );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pRebuyPageBack );
        RegisterControl( pScrollBar );
        m_pListScrollBar = pScrollBar;
    }

    {
        for ( int i = 0; i < MAX_REBUY_LIST_NODE; i++ )
        {
            CRebuyListNode* pRebuyListNode = new CRebuyListNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
            pRebuyListNode->CreateSub( this, "REBUY_LIST_NODE", UI_FLAG_DEFAULT, (REBUY_LIST_NODE_0 + i) );
            pRebuyListNode->CreateSubControl();
            RegisterControl( pRebuyListNode );
            pRebuyListNode->SetVisibleSingle( FALSE );
            m_pRebuyListNode[ i ] = pRebuyListNode;
        }

        CRebuyListNode* pRebuyListNode = new CRebuyListNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pRebuyListNode->CreateSub( this, "REBUY_LIST_NODE_LEFT_TEMP" );
        pRebuyListNode->CreateSubControl();
        RegisterControl( pRebuyListNode );
        pRebuyListNode->SetVisibleSingle( FALSE );
        m_pRebuyListNodeLeftTemp = pRebuyListNode;

        pRebuyListNode = new CRebuyListNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pRebuyListNode->CreateSub( this, "REBUY_LIST_NODE_RIGHT_TEMP" );
        pRebuyListNode->CreateSubControl();
        RegisterControl( pRebuyListNode );
        pRebuyListNode->SetVisibleSingle( FALSE );
        m_pRebuyListNodeRightTemp = pRebuyListNode;
    }

    CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_EX_FLAG );
    CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_EX_FLAG );

    {
        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "DIALOGUE_WINDOW_REBUY_NOTIFY_ALL_DEL_ITEMS", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pNotifyAllDelItems = pTextBox;
        m_pNotifyAllDelItems->SetText( ID2GAMEINTEXT( "DIALOGUE_WINDOW_NOTIFY_ALL_DEL_ITEMS" ), NS_UITEXTCOLOR::ORANGE );
    }

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "DIALOGUE_WINDOW_REBUY_SUPPLIES_CHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pCheckRebuySupplies = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "DIALOGUE_WINDOW_REBUY_SUPPLIES_UNCHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pUncheckRebuySupplies = pControl;
        m_pUncheckRebuySupplies->SetVisibleSingle( FALSE );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "DIALOGUE_WINDOW_REBUY_SUPPLIES_BACK", UI_FLAG_DEFAULT, CHECK_REBUY_SUPPLIES );
        RegisterControl( pControl );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "DIALOGUE_WINDOW_REBUY_SUPPLIES_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        m_pRebuySuppliesText = pTextBox;
        m_pRebuySuppliesText->SetText( ID2GAMEINTEXT( "DIALOGUE_WINDOW_REBUY_SUPPLES" ), NS_UITEXTCOLOR::WHITE );
    }

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "DIALOGUE_WINDOW_ALL_DEL_ITEMS_CHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pCheckAllDelItems = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "DIALOGUE_WINDOW_ALL_DEL_ITEMS_UNCHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pUncheckAllDelItems = pControl;
        m_pUncheckAllDelItems->SetVisibleSingle( FALSE );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "DIALOGUE_WINDOW_ALL_DEL_ITEMS_BACK", UI_FLAG_DEFAULT, CHECK_ALL_DEL_ITEMS );
        RegisterControl( pControl );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "DIALOGUE_WINDOW_ALL_DEL_ITEMS_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        m_pAllDelItemsText = pTextBox;
        m_pAllDelItemsText->SetText( ID2GAMEINTEXT( "DIALOGUE_WINDOW_ALL_DEL_ITEMS" ), NS_UITEXTCOLOR::WHITE );
    }

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "DIALOGUE_WINDOW_UNDER_LINE" );
        RegisterControl( pControl );
    }

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "MARKET_SELL_RIGHT_BUTTON_CHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pCheckSellRBtn = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "MARKET_SELL_RIGHT_BUTTONS_UNCHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pUncheckSellRBtn = pControl;
        m_pUncheckSellRBtn->SetVisibleSingle( FALSE );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "MARKET_SELL_RIGHT_BUTTON_BACK", UI_FLAG_DEFAULT, CHECK_SELL_R_BTN );
        RegisterControl( pControl );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "MARKET_SELL_RIGHT_BUTTON_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        m_pSellRBtnText = pTextBox;
        m_pSellRBtnText->SetText( ID2GAMEINTEXT( "MARKET_SELL_RIGHT_BUTTON_TEXT" ), NS_UITEXTCOLOR::WHITE );
    }

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "MARKET_COFIRM_SELL_ITEM_CHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pCheckConfirmSell = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "MARKET_COFIRM_SELL_ITEM_UNCHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pUncheckConfirmSell = pControl;
        m_pUncheckConfirmSell->SetVisibleSingle( FALSE );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "MARKET_COFIRM_SELL_ITEM_BACK", UI_FLAG_DEFAULT, CHECK_CONFIRM_SELL_ITEM );
        RegisterControl( pControl );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "MARKET_COFIRM_SELL_ITEM_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        m_pConfirmSellText = pTextBox;
        m_pConfirmSellText->SetText( ID2GAMEINTEXT( "MARKET_CONFIRM_SELL_ITEM_TEXT" ), NS_UITEXTCOLOR::WHITE );
    }
}

void CNPCRebuyPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !CUIControlNpcActionable::IsUseable( m_pGaeaClient ) )
    {
        m_pInterface->SetDialogueWindowClose();
        return;
    }

    {
        CBasicScrollThumbFrame* const pThumbFrame = m_pListScrollBar->GetThumbFrame();	
        pThumbFrame->SetState( m_nTotalLine, s_nRebuyListLinePerOneView );
        if ( s_nRebuyListLinePerOneView < m_nTotalLine )
        {
            const int nMovableLine = m_nTotalLine - s_nRebuyListLinePerOneView;
            float fPercent = pThumbFrame->GetPercent();
            int nPos = (int)floor(fPercent * nMovableLine);
            if ( nPos != m_nRebuyListStartIndex )
            {
                m_nRebuyListStartIndex = nPos;
                LoadRebuyListNodes();
            }
            else
                m_nRebuyListStartIndex = nPos;
        }
    }

    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CNPCRebuyPage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case CHECK_REBUY_SUPPLIES:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                m_pGaeaClient->GetCharacter()->ReqCheckRebuySupplies( !RANPARAM::bCheckRebuySuppliesRebuyPage );
            }
        }
        break;

    case CHECK_ALL_DEL_ITEMS:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                SetCheckAllDelItems( !RANPARAM::bCheckAllDelItemsRebuyPage );
            }
        }
        break;

    case REBUY_PAGE_BACK:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                //	NOTE
                //		손에 들고 있는 아이템 처리
                if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) || CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
                {
                    SNATIVEID sNativeID = holditem::GetHoldItem().GetNativeID();
                    if ( sNativeID != NATIVEID_NULL() )
                    {
                        //	NOTE
                        //		물건 팔기
                        const SITEM* pItemData = GLogicData::GetInstance().GetItem( sNativeID );
                        if ( pItemData )
                        {
                            if ( RANPARAM::bCheckConfirmSellItem )
                            {
                                const std::string& strItemName = holditem::GetHoldItem().GETNAME();
                                std::string strCombine = sc::string::format( ID2GAMEINTEXT( "SELL_ITEM_CONFIRM" ), strItemName );
                                m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_SELL_ITEM );
                            }
                            else
                            {
                                int nPosX = CInnerInterface::nOUTOFRANGE;
                                int nPosY = CInnerInterface::nOUTOFRANGE;

                                m_pInterface->GetDialogueWindowMarketItemIndex( &nPosX, &nPosY );
                                if ( nPosX < 0 || nPosY < 0 )
                                    return;

                                SNATIVEID sNPCID = m_pInterface->GetDialogueWindowNPCID();
                                DWORD dwNPCGlobID = m_pInterface->GetDialogueWindowNPCGlobID();
                                int nPageIndex = m_pInterface->GetDialogueWindowPageIndex();

                                m_pGaeaClient->GetCharacter()->ReqNpcTo(
                                    sNPCID, dwNPCGlobID,
                                    nPageIndex, nPosX, nPosY );
                            }
                        }
                        else
                        {
                            GASSERT( 0 && "아이템을 찾을 수 없습니다." );
                        }
                    }
                }
            }
        }
        break;

    case CHECK_SELL_R_BTN:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                SetCheckSellRBtnItem( !RANPARAM::bCheckSellItemRButton );
            }
        }
        break;

    case CHECK_CONFIRM_SELL_ITEM:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                SetCheckConfirmSellItem( !RANPARAM::bCheckConfirmSellItem );
            }
        }
        break;
    };

    CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CNPCRebuyPage::LoadRebuyListNodes()
{
    for ( int i = 0; i < MAX_REBUY_LIST_NODE; i++ )
        m_pRebuyListNode[ i ]->SetVisibleSingle( FALSE );

    int nStartIndex = m_nRebuyListStartIndex * s_nRebuyListColumnNum;
    int nItemListSize = static_cast<int>( m_vecRebuy.size() );
    if ( nStartIndex >= GLCONST_CHAR::wMaxRebuyList )
        nStartIndex = 0;

    for ( int i = 0; i < MAX_REBUY_LIST_NODE; i++ )
    {
        if ( nStartIndex >= nItemListSize ) break;

        // 리스트 노드의 정보 설정.
        RebuyItem::SREBUY_ITEM& sRebuyItem = m_vecRebuy[ nStartIndex++ ];
        m_pRebuyListNode[ i ]->SetItem( sRebuyItem );

        // 리스트 노드의 위치 설정.
        const int nLineSpace = 2;

        if ( i % 2 == 0 )
        {   // 왼쪽 열.
            const UIRECT& rcWindowLP = m_pRebuyListNodeLeftTemp->GetLocalPos();
            D3DXVECTOR2 vPos;

            float fCalIndex = floor( i / static_cast<float>(s_nRebuyListColumnNum) );
            int nIndex = static_cast<int>( fCalIndex );

            vPos.x = rcWindowLP.left;
            vPos.y = rcWindowLP.top + ( ( rcWindowLP.sizeY + s_nRebuyListColumnNum ) * nIndex );
            m_pRebuyListNode[ i ]->SetLocalPos( vPos );

            const UIRECT& rcWindowGP = m_pRebuyListNodeLeftTemp->GetGlobalPos();
            vPos.x = rcWindowGP.left;
            vPos.y = rcWindowGP.top + ( ( rcWindowGP.sizeY + s_nRebuyListColumnNum ) * nIndex );
            m_pRebuyListNode[ i ]->SetGlobalPos( vPos );
        }
        else
        {   // 오른쪽 열.
            const UIRECT& rcWindowLP = m_pRebuyListNodeRightTemp->GetLocalPos();
            D3DXVECTOR2 vPos;

            float fCalIndex = floor( i / static_cast<float>(s_nRebuyListColumnNum) );
            int nIndex = static_cast<int>( fCalIndex );

            vPos.x = rcWindowLP.left;
            vPos.y = rcWindowLP.top + ( ( rcWindowLP.sizeY + nLineSpace ) * nIndex );
            m_pRebuyListNode[ i ]->SetLocalPos( vPos );

            const UIRECT& rcWindowGP = m_pRebuyListNodeRightTemp->GetGlobalPos();
            vPos.x = rcWindowGP.left;
            vPos.y = rcWindowGP.top + ( ( rcWindowGP.sizeY + nLineSpace ) * nIndex );
            m_pRebuyListNode[ i ]->SetGlobalPos( vPos );
        }

        m_pRebuyListNode[ i ]->SetVisibleSingle( TRUE );
    }
}

void CNPCRebuyPage::LoadRebuyList()
{
    m_vecRebuy = m_pGaeaClient->GetCharacter()->RebuyList();

    int nRebuyListSize = static_cast<int>( m_vecRebuy.size() );

    float fCalTotalLine = ceil( nRebuyListSize / static_cast<float>(s_nRebuyListColumnNum) );
    m_nTotalLine = static_cast<int>( fCalTotalLine );

    LoadRebuyListNodes();

    SetCheckRebuySupplies( RANPARAM::bCheckRebuySuppliesRebuyPage );
    SetCheckAllDelItems( RANPARAM::bCheckAllDelItemsRebuyPage );
    m_pInterface->DialogueWindowSetRebuyCountText( GetRebuyListCount() );

    SetCheckSellRBtnItem( RANPARAM::bCheckSellItemRButton );
    SetCheckConfirmSellItem( RANPARAM::bCheckConfirmSellItem );
}

void CNPCRebuyPage::SetCheckRebuySupplies( bool bCheck )
{
    RANPARAM::bCheckRebuySuppliesRebuyPage = bCheck;

    if ( bCheck )
    {
        m_pCheckRebuySupplies->SetVisibleSingle( TRUE );
        m_pUncheckRebuySupplies->SetVisibleSingle( FALSE );
    }
    else
    {
        m_pCheckRebuySupplies->SetVisibleSingle( FALSE );
        m_pUncheckRebuySupplies->SetVisibleSingle( TRUE );
    }
}

void CNPCRebuyPage::SetCheckAllDelItems( bool bCheck )
{
    RANPARAM::bCheckAllDelItemsRebuyPage = bCheck;

    if ( bCheck )
    {
        m_pCheckAllDelItems->SetVisibleSingle( TRUE );
        m_pUncheckAllDelItems->SetVisibleSingle( FALSE );
    }
    else
    {
        m_pCheckAllDelItems->SetVisibleSingle( FALSE );
        m_pUncheckAllDelItems->SetVisibleSingle( TRUE );
    }
}

int CNPCRebuyPage::GetRebuyListCount()
{
    return static_cast<int>( m_vecRebuy.size() );
}

void CNPCRebuyPage::SetCheckSellRBtnItem( bool bCheck )
{
    RANPARAM::bCheckSellItemRButton = bCheck;

    if ( bCheck )
    {
        m_pCheckSellRBtn->SetVisibleSingle( TRUE );
        m_pUncheckSellRBtn->SetVisibleSingle( FALSE );
    }
    else
    {
        m_pCheckSellRBtn->SetVisibleSingle( FALSE );
        m_pUncheckSellRBtn->SetVisibleSingle( TRUE );
    }
}

void CNPCRebuyPage::SetCheckConfirmSellItem( bool bCheck )
{
    RANPARAM::bCheckConfirmSellItem = bCheck;

    if ( bCheck )
    {
        m_pCheckConfirmSell->SetVisibleSingle( TRUE );
        m_pUncheckConfirmSell->SetVisibleSingle( FALSE );
    }
    else
    {
        m_pCheckConfirmSell->SetVisibleSingle( FALSE );
        m_pUncheckConfirmSell->SetVisibleSingle( TRUE );
    }
}

void CNPCRebuyPage::SetNPCID( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID )
{
    SetNpcActionableNPCID( sNPCID, dwNPCGlobalID );
    m_pGaeaClient->GetCharacter()->ReqCheckRebuySupplies( RANPARAM::bCheckRebuySuppliesRebuyPage );
}
