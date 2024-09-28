#include "StdAfx.h"

#include "../../InnerInterface.h"
#include "./ItemRepairWindow.h"

#include <boost/foreach.hpp>
// #include "../../../EngineLib/GUInterface/NSGUI.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../enginelib/GUInterface/BasicScrollBarEx.h"
#include "../../../enginelib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Item/ItemDurability.h"
#include "../../../RanLogic/Msg/ItemRepairMsg.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../Util/RnButton.h"
#include "../Item/ItemImage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const char * szTitle = "아이템수리";
const char * szText1 = "장착된 아이템 중 수리할 아이템을 선택하세요.";
const char * szText2 = "총 수리비용 ";
const char * szText3 = "수리";
const char * szText4 = "전체수리";

ItemRepairWindow::WindowLine::WindowLine( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroupHelper( pEngineDevice )
    , m_pInterface( pInterface )
    , m_pGaeaClient( pGaeaClient )
{
}

void ItemRepairWindow::WindowLine::CreateSubControl()
{
    CreateLineBox( "ITEM_REPAIR_WINDOW_LINE_SIZE", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );

    {
        CItemImage* pItemImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
        pItemImage->CreateSub( this, "ITEM_REPAIR_WINDOW_LINE_ICON"
            , UI_FLAG_DEFAULT , UI_ID_ICON );
        pItemImage->CreateSubControl();
        RegisterControl( pItemImage );
        m_pItemImage = pItemImage;
    }

    m_pTextLine0    = CreateText(" ", "ITEM_REPAIR_WINDOW_LINE_TEXT0", TEXT_ALIGN_LEFT|TEXT_ALIGN_CENTER_Y );
    m_pTextLine1_L  = CreateText(" ", "ITEM_REPAIR_WINDOW_LINE_TEXT1", TEXT_ALIGN_LEFT|TEXT_ALIGN_CENTER_Y );
    m_pTextLine1_R  = CreateText(" ", "ITEM_REPAIR_WINDOW_LINE_TEXT2", TEXT_ALIGN_RIGHT|TEXT_ALIGN_CENTER_Y );

    
}

void ItemRepairWindow::WindowLine::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void ItemRepairWindow::WindowLine::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );

    switch( ControlID )
    {
    case UI_ID_ICON:
        if( CHECK_MOUSE_IN( dwMsg ) )
        {
            m_pInterface->SHOW_ITEM_INFO( m_sItemCustom,
            FALSE, FALSE, FALSE, TRUE, FALSE, USHRT_MAX, USHRT_MAX );
        }
    }

        
}

void ItemRepairWindow::WindowLine::SetData( const SITEMCUSTOM& itemCustom )
{
    m_sItemCustom = itemCustom;
    const SNATIVEID sID = itemCustom.GetNativeID();
    const SITEM* pItem = GLogicData::GetInstance().GetItem( sID );
    if( !pItem )
        return;
 
    CString strLeft;
    strLeft.Format( "%d/%d (%d%%)"
        , (int)ItemDurability::GetInstance().Get(itemCustom)
        , (int)ItemDurability::GetInstance().GetMax(itemCustom)
        , ItemDurability::GetInstance().GetPercent(itemCustom) );


   
    DWORD dwItemTextColor = ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ];	// 텍스트 컬러(해당아이템 등급)에 따라
    CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( 
        ItemDurability::GetInstance().GetRepairPriceAllPoint(itemCustom), 3, "," );
    strTemp.Append(" ");

    m_pItemImage->SetItem( itemCustom );
    m_pTextLine0->SetText( itemCustom.GETNAME().c_str(), dwItemTextColor );
    m_pTextLine1_L->SetText( strLeft, ItemDurability::GetInstance().GetTextColor(itemCustom, NS_UITEXTCOLOR::WHITE) );
    m_pTextLine1_R->SetText( strTemp, NS_UITEXTCOLOR::ORANGE );

    m_pItemImage->SetVisibleSingle( true );
    m_pTextLine0->SetVisibleSingle( true );
    m_pTextLine1_L->SetVisibleSingle( true );
    m_pTextLine1_R->SetVisibleSingle( true );
}

void ItemRepairWindow::WindowLine::ResetData()
{
    m_pItemImage->SetVisibleSingle( false );
    m_pTextLine0->SetVisibleSingle( false );
    m_pTextLine1_L->SetVisibleSingle( false );
    m_pTextLine1_R->SetVisibleSingle( false );
    m_sItemCustom = SITEMCUSTOM();
}


ItemRepairWindow::ItemRepairWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice ) 
    : CUIWindowEx( pInterface, pEngineDevice )
    , m_pInterface( pInterface )
    , m_pGaeaClient(pGaeaClient)
    , m_pScrollBar(NULL)
    , m_pText1(NULL)
    , m_pText2(NULL)
    , m_nSeletedLine(-1)
    , m_nScrollCurPos(0)
{
    for( int i=0 ; i < LINE_MAX ; i++ )
    {
        m_Lines[i] = NULL;
    }
    szTitle = ID2GAMEWORD( "ITEM_REPAIR_UI", 0 );
    szText1 = ID2GAMEWORD( "ITEM_REPAIR_UI", 1 );
    szText2 = ID2GAMEWORD( "ITEM_REPAIR_UI", 2 );
    szText3 = ID2GAMEWORD( "ITEM_REPAIR_UI", 3 );
    szText4 = ID2GAMEWORD( "ITEM_REPAIR_UI", 4 );
}

ItemRepairWindow::~ItemRepairWindow()
{

}

void ItemRepairWindow::CreateSubControl()
{
    CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
    CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

    CreateBaseWindowLightGray( "ITEM_REPAIR_WINDOW",  szTitle );
    CreateLineBox( "ITEM_REPAIR_WINDOW_BG_SIZE", "ANOTHER_CHAR_REGION_WINDOW_TEXTUREINFO" );

    CreateLineBox( "ITEM_REPAIR_WINDOW_BOX1", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
    m_pText1 = CreateText(szText1, "ITEM_REPAIR_WINDOW_BOX1", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y );

    CreateLineBox( "ITEM_REPAIR_WINDOW_BOX2", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
    std::string strLineText[LINE_MAX] =
    {
        "ITEM_REPAIR_WINDOW_LINE0",
        "ITEM_REPAIR_WINDOW_LINE1",
        "ITEM_REPAIR_WINDOW_LINE2",
        "ITEM_REPAIR_WINDOW_LINE3",
        "ITEM_REPAIR_WINDOW_LINE4"
    };


    { // 스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        pScrollBar->CreateBaseScrollBar( "ITEM_REPAIR_WINDOW_SCROLL" );
        pScrollBar->GetThumbFrame()->SetScrollParent( this );
        pScrollBar->GetThumbFrame()->SetScrollMoveLine( 1 );
        RegisterControl ( pScrollBar );
        m_pScrollBar = pScrollBar;
    }


/*    CreateLineBox( "ITEM_REPAIR_WINDOW_SCROLL", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );*/
    for( int i=0 ; i < LINE_MAX ; i++ )
    {
        ItemRepairWindow::WindowLine* pLine;
        pLine = new ItemRepairWindow::WindowLine(m_pGaeaClient, m_pInterface, m_pEngineDevice);
        pLine->CreateSub( this, strLineText[i].c_str(), UI_FLAG_DEFAULT, UI_ID_LINE_0 + i );
        pLine->CreateSubControl();
        RegisterControl( pLine );

        m_Lines[i] = pLine;
    }
    

    //CreateLineBox( "ITEM_REPAIR_WINDOW_BOX3_1", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
    m_pText1 = CreateText(szText2, "ITEM_REPAIR_WINDOW_BOX3_1", TEXT_ALIGN_RIGHT|TEXT_ALIGN_CENTER_Y );

    CreateLineBox( "ITEM_REPAIR_WINDOW_BOX3_2", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
    CreateLineBox( "ITEM_REPAIR_WINDOW_BOX3_2_ICON", "TEXTTURE_MONEY_ICON" );
    /*CreateLineBox( "ITEM_REPAIR_WINDOW_BOX3_2_TEXT", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );*/
    m_pText2 = CreateText("999,999,999,999 ", "ITEM_REPAIR_WINDOW_BOX3_2_TEXT", TEXT_ALIGN_RIGHT|TEXT_ALIGN_CENTER_Y );

//     CreateLineBox( "ITEM_REPAIR_WINDOW_BOX4_L", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
//     CreateLineBox( "ITEM_REPAIR_WINDOW_BOX4_R", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );


    RnButton::CreateArg arg;
    arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
    arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
    arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
    arg.pFont = pFont9;
    arg.dwColor = NS_UITEXTCOLOR::WHITE;

    {
        RnButton* pRnButton = new RnButton( m_pEngineDevice );
        pRnButton->CreateSub(this,"ITEM_REPAIR_WINDOW_BOX4_L",UI_FLAG_DEFAULT, UI_ID_BTN_REPAIR );
        arg.text = szText3;
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
    }

    {
        RnButton* pRnButton = new RnButton( m_pEngineDevice );
        pRnButton->CreateSub(this,"ITEM_REPAIR_WINDOW_BOX4_R",UI_FLAG_DEFAULT, UI_ID_BTN_REPAIR_ALL );
        arg.text = szText4;
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
    }

    m_pSeleted = static_cast<CBasicLineBoxSmart*>( CreateLineBox( "ITEM_REPAIR_WINDOW_LINE_SIZE", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_YELLOWLINE" ) );
    m_pSeleted->SetVisibleSingle( FALSE );
}

void ItemRepairWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();
    const int nTotal       = m_mapRepair.size();
  
    m_pScrollBar->GetThumbFrame()->SetState( nTotal, LINE_MAX );

    const int nMovableLine = nTotal - LINE_MAX;
    float fPercent = pThumbFrame->GetPercent();
    m_nScrollCurPos = (int)floor(fPercent * nMovableLine);
    if ( m_nScrollCurPos < 0 ) m_nScrollCurPos = 0;

    std::multimap<WORD, EMSLOT>::iterator it = m_mapRepair.begin();

    int nRowCount=0;
    int nCount=0;
    
    if( m_pSeleted->IsVisible() )
        m_pSeleted->SetVisibleSingle( FALSE );

    while( it != m_mapRepair.end() )
    {
        if( m_nScrollCurPos <= nCount && nRowCount < LINE_MAX )
        {
            if( nCount == m_nSeletedLine )
            {
                m_pSeleted->SetGlobalPos( m_Lines[nRowCount]->GetGlobalPos() );
                m_pSeleted->SetVisibleSingle( TRUE );
            }

            EMSLOT slot = (it->second);
            const SITEMCUSTOM& itemCustom = m_pGaeaClient->GetCharacter()->GET_SLOT_ITEM( slot );    
            m_Lines[nRowCount]->SetData( itemCustom );
            nRowCount++;
        }
        nCount++;
        it++;
    }

    for( int i=nRowCount ; i < LINE_MAX ; i++ )
        m_Lines[i]->ResetData();
    

    Refresh();
}

void ItemRepairWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

    switch( ControlID )
    {
    case UI_ID_LINE_0:
    case UI_ID_LINE_1:
    case UI_ID_LINE_2:
    case UI_ID_LINE_3:
    case UI_ID_LINE_4:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            int nRow = ControlID - UI_ID_LINE_0;
            m_nSeletedLine = nRow + m_nScrollCurPos;
            break;
        }
    case UI_ID_BTN_REPAIR:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            Repairing();
        }
        break;
    case UI_ID_BTN_REPAIR_ALL:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            RepairingAll();
        }
        break;
    }
}

void ItemRepairWindow::Refresh()
{
    m_mapRepair.clear();
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();

    LONGLONG llRepairPriceAll = 0;
    for( int i=SLOT_HEADGEAR ; i < SLOT_TSIZE ; i++ )
    {
        const SITEMCUSTOM& itemCustom = pChar->GET_SLOT_ITEM((EMSLOT)i);
        
        if( itemCustom.GetNativeID() == NATIVEID_NULL () )
            continue;

		if( !ItemDurability::GetInstance().IsUse( itemCustom.GetNativeID() ) )
			continue;
        
        llRepairPriceAll += ItemDurability::GetInstance().GetRepairPriceAllPoint(itemCustom);
        WORD wPercent = ItemDurability::GetInstance().GetPercent(itemCustom);
        
        if( wPercent < 100 )
            m_mapRepair.insert( std::make_pair((WORD)ItemDurability::GetInstance().Get(itemCustom), (EMSLOT)i ) );
    }

    CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llRepairPriceAll, 3, "," );
    strTemp.Append(" ");

    m_pText2->SetText( strTemp );
}

void ItemRepairWindow::ItemRepairCardUseReq( WORD wPosX, WORD wPosY )
{
    GLMSG::NET_ITEM_REPAIR_CARD_USE_REQ SendData;
    {
        SendData.m_dwChaDbNum = m_pGaeaClient->GetCharacter()->CharDbNum();
        SendData.m_nInvecPosX = wPosX;
        SendData.m_nInvecPosY = wPosY;
    }

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    m_pGaeaClient->NETSENDTOFIELD(NET_MSG_ITEM_REPAIR_CARD_REQ, PackBuffer);

}

void ItemRepairWindow::Repairing()
{
    GLMSG::NET_ITEM_REPAIR_REQ SendData;
    {
        SendData.m_dwChaDbNum = m_pGaeaClient->GetCharacter()->CharDbNum();

        std::multimap<WORD, EMSLOT>::iterator it = m_mapRepair.begin();
        int nCount=0;
        while( it != m_mapRepair.end() )
        {
            if( nCount == m_nSeletedLine )
            {
                SendData.m_vecSlot.push_back( it->second );
                break;
            }
            nCount++;it++;
        }

    }

    if( SendData.m_vecSlot.empty() || m_mapRepair.empty() )
        return;

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    m_pGaeaClient->NETSENDTOFIELD(NET_MSG_ITEM_REPAIR_REQ, PackBuffer);
}

void ItemRepairWindow::RepairingAll()
{
    if( m_mapRepair.size() <= 0 )
        return;

    GLMSG::NET_ITEM_REPAIR_REQ SendData;
    {
        SendData.m_dwChaDbNum = m_pGaeaClient->GetCharacter()->CharDbNum();

        std::multimap<WORD, EMSLOT>::iterator it = m_mapRepair.begin();
        int nCount=0;
        while( it != m_mapRepair.end() )
        {
            SendData.m_vecSlot.push_back( it->second );
            it++;
        }
    }

    if( SendData.m_vecSlot.empty() || m_mapRepair.empty() )
        return;

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    m_pGaeaClient->NETSENDTOFIELD(NET_MSG_ITEM_REPAIR_REQ, PackBuffer);
}

