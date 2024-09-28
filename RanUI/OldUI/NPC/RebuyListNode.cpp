#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DeviceMan.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "../Item/ItemSlot.h"
#include "../Util/RnButton.h"
#include "./RebuyListNode.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CRebuyListNode::CRebuyListNode(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pBack(NULL)
    , m_pImage(NULL)
    , m_pName(NULL)
    , m_pPrice(NULL)
    , m_pBuyButton(NULL)
    , m_pSelectItem(NULL)

    , m_bVisibleBuyButton(false)
{
}

CRebuyListNode::~CRebuyListNode()
{
}

void CRebuyListNode::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "REBUY_LIST_NODE_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE, NODE_BACK );
        pLinBoxSmart->CreateSubControl( "REBUY_LIST_NODE_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pBack = pLinBoxSmart;
    }

    {
        CItemSlot* pItemSlot = new CItemSlot( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pItemSlot->CreateSub( this, "REBUY_LIST_NODE_IMAGE", UI_FLAG_DEFAULT, NODE_IMAGE );
        pItemSlot->CreateSubControl( 1, TRUE );
        RegisterControl( pItemSlot );
        m_pImage = pItemSlot;
        m_pImage->SetVisibleSingle( FALSE );
    }

    {
        CD3DFontPar* pFont11Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_EX_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "REBUY_LIST_NODE_NAME", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont11Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pName = pTextBox;
    }

    {
        CD3DFontPar* pFont11Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_EX_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "REBUY_LIST_NODE_PRICE", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont11Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pPrice = pTextBox;
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton* pRnButton = NULL;

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
        //////////////////////////////////////////////////////////////////////////
        arg.pFont = pFont10Shadow;
        arg.text = sc::string::format( "%1%", ID2GAMEWORD( "REBIRTH_WINDOW_BUY_BUTTON_TEXT" ) );
        arg.dwColor = NS_UITEXTCOLOR::WHITE;

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"REBUY_LIST_NODE_BUY_BUTTON",UI_FLAG_DEFAULT,BUY_BUTTON);
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pBuyButton = pRnButton;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "REBUY_LIST_NODE_SELECT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "REBUY_LIST_NODE_SELECT_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pSelectItem = pLinBoxSmart;
        m_pSelectItem->SetVisibleSingle( FALSE );
    }
}

void CRebuyListNode::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CRebuyListNode::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case NODE_BACK:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                SetVisibleBuyButton( true );
                m_pSelectItem->SetVisibleSingle( TRUE );
            }
            else
            {
                SetVisibleBuyButton( false );
                m_pSelectItem->SetVisibleSingle( FALSE );
            }
        }
        break;

    case NODE_IMAGE:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                int nPosX = m_pImage->GetItemIndex();
                if (nPosX < 0)
                    return;
                const SINVENITEM* pInvenItem = m_pImage->GetItemImage( nPosX );
                {				
                    if (pInvenItem && (pInvenItem->sItemCustom.GetNativeID() != NATIVEID_NULL()))
                    {
                        m_pInterface->SHOW_ITEM_INFO(
							pInvenItem->sItemCustom,
							FALSE, FALSE, FALSE, FALSE, FALSE,
							USHRT_MAX, USHRT_MAX );
                    }
                }
            }
        }
        break;

    case BUY_BUTTON:
        {
            if ( m_bVisibleBuyButton )
            {
                if ( dwMsg & UIMSG_RNBUTTON_CLICK )
                    BuyRebuyListItem();
            }
        }
        break;
    };

    CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CRebuyListNode::SetItem( RebuyItem::SREBUY_ITEM& sRebuyItem )
{
    SITEMCUSTOM& sItemCustom = sRebuyItem.ItemCustom();
    if ( sItemCustom.GetNativeID() == NATIVEID_NULL() ) return;

    const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.Id() );
    if ( !pItem )
    {
        GASSERT ( 0 && "등록되지 않은 ID입니다." );
        return ;
    }

    SINVENITEM sInvenItem;
    sInvenItem.sItemCustom = sRebuyItem.sItemCustom;
    m_pImage->SetItemImage( 0, sInvenItem );
    m_pImage->SetVisibleSingle( TRUE );

    const SITEM* pItemData = GLogicData::GetInstance().GetItem( sInvenItem.Id() );
    if ( !pItemData )
        return;

    m_sItemCustom = sRebuyItem.sItemCustom;
    m_Price = sRebuyItem.Price();

    m_pName->SetText( sItemCustom.GETNAME().c_str(), NS_UITEXTCOLOR::WHITE );
    CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( m_Price, 3, "," );
    std::string strMoney = sc::string::format( "%s", strTemp );

    LONGLONG llInvenMoney = m_pGaeaClient->GetCharacter()->GetInvenMoney();

    if ( m_Price <= llInvenMoney )
    {
        m_pPrice->ClearText();
        int nIndex = m_pPrice->AddText( strMoney.c_str(), NS_UITEXTCOLOR::WHITE );
        m_pPrice->SetUseTextColor( nIndex, TRUE );
        m_pPrice->SetTextColor( nIndex, NS_UITEXTCOLOR::WHITE );
    }
    else
    {
        m_pPrice->ClearText();
        int nIndex = m_pPrice->AddText( strMoney.c_str(), NS_UITEXTCOLOR::RED );
        m_pPrice->SetUseTextColor( nIndex, TRUE );
        m_pPrice->SetTextColor( nIndex, NS_UITEXTCOLOR::RED );
    }

    SetVisibleBuyButton( false );
}

void CRebuyListNode::ResetItem()
{
    m_pImage->ResetItemImage( 0 );
}

void CRebuyListNode::SetVisibleBuyButton( bool bVisible )
{
    m_bVisibleBuyButton = bVisible;
    if ( bVisible )
    {
        m_pBuyButton->SetVisibleSingle( TRUE );
    }
    else
    {
        m_pBuyButton->SetVisibleSingle( FALSE );
    }
}

void CRebuyListNode::BuyRebuyListItem()
{
    LONGLONG llInvenMoney = m_pGaeaClient->GetCharacter()->GetInvenMoney();
    if (llInvenMoney < m_Price)
    {
        m_pGaeaClient->PrintMsgText( 
            NS_UITEXTCOLOR::DISABLE,
            ID2GAMEINTEXT("BUY_REBUYITEM_NOT_ENOUGH_MONEY"));
        return;
    }

    const SITEM* pItem = GLogicData::GetInstance().GetItem(m_sItemCustom.Id());
    if (pItem)
    {
        GLMSG::SNET_BUY_REBUY_ITEM_CA NetMsg(m_sItemCustom, m_Price);
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
}
