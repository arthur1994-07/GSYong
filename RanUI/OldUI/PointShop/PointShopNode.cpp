#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImage.h"
#include "./PointShopNode.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPointShopNode::CPointShopNode( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pBack(NULL)
    , m_pImage(NULL)
    , m_pName(NULL)
    , m_pPrice(NULL)
{
}

CPointShopNode::~CPointShopNode()
{
}

void CPointShopNode::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "POINT_SHOP_ITEM_LIST_NODE_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_LIST_NODE_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pBack = pLinBoxSmart;
    }

    {
        CItemImage* pItemImage = new CItemImage( m_pGaeaClient, m_pEngineDevice );
        pItemImage->CreateSub( this, "POINT_SHOP_ITEM_LIST_NODE_IMAGE", UI_FLAG_DEFAULT, POINT_SHOP_IMAGE );
        pItemImage->CreateSubControl();
        RegisterControl( pItemImage );
        m_pImage = pItemImage;
        m_pImage->SetVisibleSingle( FALSE );
    }

    {
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_ITEM_LIST_NODE_NAME", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        pTextBox->SetAutoChangeFontSize( true );
        RegisterControl( pTextBox );
        m_pName = pTextBox;
    }

    {
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_ITEM_LIST_NODE_PRICE", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pPrice = pTextBox;
    }
}

void CPointShopNode::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPointShopNode::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case POINT_SHOP_IMAGE:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                if ( DxInputDevice::GetInstance().GetKeyState( DIK_LALT ) & DXKEY_PRESSED )
                {
                    SITEMCUSTOM sItemcustom( m_sID );

                    if ( m_pInterface->ItemPreviewWindowIsPreviewItem( sItemcustom ) )
                    {
                        m_pInterface->ItemPreviewWindowOpen( sItemcustom );
                    }
                }
            }
            else if ( CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
            {
                if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED )
                {
                    SITEMCUSTOM sItemCustom( m_sID );

                    const SITEM* pItem = GLItemMan::GetInstance().GetItem(sItemCustom.GetNativeID());
                    if (pItem->BasicType() == ITEM_BOX || pItem->BasicType() == ITEM_RANDOMITEM )
                    {
                        m_pInterface->GetInvenItemTooltipGroup()->ShowItemToolTipMulti( sItemCustom, DxInputDevice::GetInstance().GetMouseLocateX(), DxInputDevice::GetInstance().GetMouseLocateY() );

                        ResetMessageEx();
                    }
                }
            }
        }
        break;
    }
}

void CPointShopNode::SetItem( SNATIVEID _sID, std::string _strName, __int64 _Price )
{
    m_sID = _sID;
    m_Name = _strName;
    m_Price = _Price;

    if ( m_sID == NATIVEID_NULL() ) return;

    const SITEM* pItem = GLogicData::GetInstance().GetItem( m_sID );
    if ( !pItem )
    {
        GASSERT ( 0 && "등록되지 않은 ID입니다." );
        return ;
    }

    m_pImage->SetItem( pItem->sBasicOp.sNativeID );
    m_pImage->SetVisibleSingle( TRUE );

    m_pName->SetText( m_Name.c_str(), NS_UITEXTCOLOR::WHITE );

    std::string strPrice = sc::string::format( ID2GAMEINTEXT("POINT_SHOP_PRICE"), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( _Price, 3, "," ) );
    DWORD dwPriceColor = D3DCOLOR_ARGB( 0xFF,0xFF,0xCC,0x00 );
    m_pPrice->SetText( strPrice.c_str(), dwPriceColor);
}

void CPointShopNode::ResetItem()
{
    m_sID = NATIVEID_NULL();
    m_pImage->ResetItem();
}