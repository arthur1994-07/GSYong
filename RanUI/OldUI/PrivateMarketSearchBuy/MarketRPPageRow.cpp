#include "StdAfx.h"

#include "MarketRPPage.h"

#include "../../../enginelib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

//#include "../../../RanLogicClient/Char/GLogicDataMan.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/Item/GLItemMan.h"

#include "../../../RanLogic/Item/GLItem.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImageEx.h"
#include "../Item/ItemImage.h"

#include "../Util/CheckBox.h"

namespace
{
    //////////////////////////////////////////////////////////////////////////
    const char* ID2GAMEWORD_ITEM_NAME = "달빛섬광의 목걸이";
    const char* ID2GAMEWORD_USE= "(사용가능)";
    const char* ID2GAMEWORD_NOUSE= "(사용불가)";
    const char* ID2GAMEWORD_SELLER_NAME = "판매자캐릭터명";
    const char* ID2GAMEWORD_EXPIRATIONDATE = "0시간";
    const char* ID2GAMEWORD_COST = "14,000,000,000";
    const char* ID2GAMEWORD_BUY_COUNT = "구입개수";

    const char* MARKET_RPPAGE_ROW_TEXT0 = "마감 %d시간 이내";
    const char* MARKET_RPPAGE_ROW_TEXT1 = "기간만료";
    const char* MARKET_RPPAGE_ROW_TEXT2 = "판매완료";

}


MarketRPPage::MarketRPPageRow::MarketRPPageRow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice ) : CUIGroupHelper( pEngineDevice )
, m_pGaeaClient( pGaeaClient )
, m_pInterface( pInterface )
{
    MARKET_RPPAGE_ROW_TEXT0 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 22);
	MARKET_RPPAGE_ROW_TEXT1 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 27);
	MARKET_RPPAGE_ROW_TEXT2 = ID2GAMEWORD("MARKET_RPPAGE_TEXT", 28);

	ID2GAMEWORD_USE= ID2GAMEWORD("MARKET_SEARCH_BUY_ITEM_USED",0);
	ID2GAMEWORD_NOUSE= ID2GAMEWORD("MARKET_SEARCH_BUY_ITEM_NOTUSED",0);
	ID2GAMEWORD_BUY_COUNT = ID2GAMEWORD("MARKET_SEARCH_BUY_COUNT",0);
    // MARKET_RPPAGE_ROW_TEXT1
    // MARKET_RPPAGE_ROW_TEXT2
}


void MarketRPPage::MarketRPPageRow::CreateSubControl()
{
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    {
        CBasicLineBoxSmart* pItemCountEditBoxBG = new CBasicLineBoxSmart(m_pEngineDevice);
        pItemCountEditBoxBG->CreateSub( this, "MARKET_SEARCH_ROW_ITEM_COUNT_EDITBOX_BG", UI_FLAG_DEFAULT );
        pItemCountEditBoxBG->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
        pItemCountEditBoxBG->SetVisibleSingle(FALSE);
        RegisterControl( pItemCountEditBoxBG );
    }

    {// 검색 아이템 행 라인박스
        CBasicLineBoxSmart* pRowBGLine = new CBasicLineBoxSmart(m_pEngineDevice);
        pRowBGLine->CreateSub( this, "MARKET_SEARCH_ROW_BG" );
        pRowBGLine->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        RegisterControl( pRowBGLine );

        CUIControl* pSelectRow = new CUIControl(m_pEngineDevice);
        pSelectRow->CreateSub( this, "MARKET_SEARCH_ROW_BG",ID_ROW );
        RegisterControl( pSelectRow );
    }
    {	//아이콘 이미지
        CItemImageEx* pItemImage = new CItemImageEx( m_pGaeaClient, m_pEngineDevice );
        pItemImage->CreateSub( this, "MARKET_SEARCH_ROW_ITEM_IMAGE", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_ITEM_IMAGE );
        pItemImage->CreateSubControl();
        RegisterControl( pItemImage );
        m_pItemImage = pItemImage;

        // 아이템 이름
        CBasicTextBox* pITEM_NAME = new CBasicTextBox(m_pEngineDevice);
        pITEM_NAME->CreateSub( this, "MARKET_SEARCH_ROW_ITEM_NAME" );
        pITEM_NAME->SetFont( pFont );
        pITEM_NAME->SetText( ID2GAMEWORD_ITEM_NAME,NS_UITEXTCOLOR::WHITE );
        pITEM_NAME->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pITEM_NAME );
        m_pITEM_NAME = pITEM_NAME;

        // 착용 가능 불가능
        CBasicTextBox* pISUSE  = new CBasicTextBox(m_pEngineDevice);
        pISUSE->CreateSub( this, "MARKET_SEARCH_ROW_ISUSE" );
        pISUSE->SetFont( pFont );
        pISUSE->SetText( ID2GAMEWORD_USE,NS_UITEXTCOLOR::WHITE );
        pISUSE->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pISUSE );
        m_pISUSE = pISUSE;

        // 판매자 이름
        CBasicTextBox* pSELLER_NAME = new CBasicTextBox(m_pEngineDevice);
        pSELLER_NAME->CreateSub( this, "MARKET_SEARCH_ROW_SELLER_NAME" );
        pSELLER_NAME->SetFont( pFont );
        pSELLER_NAME->SetText( ID2GAMEWORD_SELLER_NAME,NS_UITEXTCOLOR::WHITE );
        pSELLER_NAME->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pSELLER_NAME );
        m_pSELLER_NAME = pSELLER_NAME;

        // 마감시간
        CBasicTextBox* pExpirationDate = new CBasicTextBox(m_pEngineDevice);
        pExpirationDate->CreateSub( this, "MARKET_SEARCH_ROW_SELLER_POS" );
        pExpirationDate->SetFont( pFont );
        pExpirationDate->SetText( ID2GAMEWORD_EXPIRATIONDATE,NS_UITEXTCOLOR::WHITE );
        pExpirationDate->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pExpirationDate );
        m_pExpirationDate = pExpirationDate;

        // 구입가격
        CBasicTextBox* pITEM_COST = new CBasicTextBox(m_pEngineDevice);
        pITEM_COST->CreateSub( this, "MARKET_SEARCH_ROW_ITEM_COST" );
        pITEM_COST->SetFont( pFont );
        pITEM_COST->SetText( ID2GAMEWORD_COST,NS_UITEXTCOLOR::ORANGE );
        pITEM_COST->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pITEM_COST );
        m_pITEM_COST = pITEM_COST;

        // 구입개수
        m_pTurnNum= new CBasicTextBox(m_pEngineDevice);
        m_pTurnNum->CreateSub( this, "MARKET_SEARCH_ROW_ITEM_TURN_NUM" );
        m_pTurnNum->SetFont( pFont );
        m_pTurnNum->SetText( "(65536/65536)",NS_UITEXTCOLOR::WHITE );
        m_pTurnNum->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        //m_pTurnNum->SetVisibleSingle(FALSE);
        RegisterControl( m_pTurnNum );

        // 구입개수
        CBasicTextBox* pITEM_COUNT= new CBasicTextBox(m_pEngineDevice);
        pITEM_COUNT->CreateSub( this, "MARKET_SEARCH_ROW_ITEM_COUNT" );
        pITEM_COUNT->SetFont( pFont );
        pITEM_COUNT->SetText( ID2GAMEWORD_BUY_COUNT,NS_UITEXTCOLOR::WHITE );
        pITEM_COUNT->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        pITEM_COUNT->SetVisibleSingle(FALSE);
        RegisterControl( pITEM_COUNT );

        //구입개수 EDIT BOX
        CUIEditBoxMan* pItemCountEditBox = new CUIEditBoxMan(m_pEngineDevice);
        pItemCountEditBox->CreateSub ( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX",UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_COUNT_EDITBOX );
        pItemCountEditBox->CreateEditBox(ID_COUNT_EDITBOX_1,"MARKET_SEARCH_ROW_ITEM_COUNT_EDITBOX", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 10,FALSE, TRUE);
        pItemCountEditBox->SetVisibleSingle(FALSE);
        RegisterControl ( pItemCountEditBox );

        // 란포인트 거래 아이콘 포인트 아이콘
        m_pRanPointIcon = CreateLineBox( "MARKET_SEARCH_ROW_ITEM_COST_ICON", "RAN_POINT_ICON_TEXTURE" );
    }

}

void MarketRPPage::MarketRPPageRow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void MarketRPPage::MarketRPPageRow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroupHelper::TranslateUIMessage ( ControlID, dwMsg );

    switch( ControlID )
    {
    case ID_ITEM_IMAGE:
        if( CHECK_MOUSE_IN( dwMsg ) )
        {
            m_bMouseOnImage = true;

            if( m_SaleItem.GetStrGUID().empty() )
                return;

            SITEM* pItem = GLItemMan::GetInstance().GetItem( m_SaleItem.sItemcustom.GetNativeID() );
            if( pItem == NULL)
                return;

            SITEMCUSTOM& sItemcustom = m_SaleItem.sItemcustom;
            m_pInterface->SHOW_ITEM_INFO(
                m_SaleItem.sItemcustom,
                FALSE, FALSE, FALSE, TRUE, FALSE,
                USHRT_MAX, USHRT_MAX );

            if( CHECK_LB_UP_LIKE( dwMsg ) )
            {
                if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED )
                {
                    if ( m_pInterface->ItemPreviewWindowIsPreviewItem( sItemcustom ) )
                    {
                        m_pInterface->ItemPreviewWindowOpen( sItemcustom );
                        ResetMessageEx();
                    }                        
                }
            }
            else if( CHECK_RB_UP_LIKE( dwMsg ) )
            {
                if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED )
                {
                    if (pItem->BasicType() == ITEM_BOX || pItem->BasicType() == ITEM_RANDOMITEM )
                    {
                        m_pInterface->GetInvenItemTooltipGroup()->ShowItemToolTipMulti( sItemcustom, DxInputDevice::GetInstance().GetMouseLocateX(), DxInputDevice::GetInstance().GetMouseLocateY() );
                    }
                }
            }
        }
        else
        {
            m_bMouseOnImage = false;
        }
        break;
    }


    if( false )
    {
        GLCharacter* pChar = m_pGaeaClient->GetCharacter();
        pChar->m_ConfignmentSaleSlotExpansionDate;
    }
}

void MarketRPPage::MarketRPPageRow::SetItem( const private_market::ConsignmentSaleItem& SaleItem )
{
    m_SaleItem = SaleItem;

    const SITEMCUSTOM& sItemCustom = SaleItem.sItemcustom;
    const SITEM* pBoxItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
    if ( !pBoxItemData )
        return;

    const SITEM* pContentItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetBoxWrappedItemID() );
    if ( !pContentItemData )
        return;

    //아이템 이미지 세팅
    {
        m_pItemImage->ResetItem();

        m_pItemImage->SetItem( sItemCustom );
        m_pItemImage->SetTurnNum( SaleItem.wTurnNum );
    }

    
    m_pRanPointIcon->SetVisibleSingle( SaleItem.SellType == private_market::eSellType_ConsignmentSale_RanPoint );

    //아이템 이름
    {
        m_pITEM_NAME->SetTextToLimit( sItemCustom.GETNAME().c_str(), ITEM_INFOR_COLOR::dwItemRank[ pContentItemData->sBasicOp.emLevel ] );
    }

    // 아이템 착용가능 여부
    {
        int nUseCount = 0;
        GLCharacter* pGLCharacter = m_pGaeaClient->GetCharacter();

        if ( !pGLCharacter )
            return;

        DWORD	dwCharClass = CharIndexToClass(pGLCharacter->GetCharIndex());
        WORD	dwSchool = pGLCharacter->GetCharSchool();
        WORD	wLevel = pGLCharacter->GetLevel();
        const int nMelee(pGLCharacter->m_powerAttribute[SKILL::EMAPPLY_MELEE]);
        const int nRange(pGLCharacter->m_powerAttribute[SKILL::EMAPPLY_RANGE]);
		const int nMagic(pGLCharacter->m_powerAttribute[SKILL::EMAPPLY_MAGIC]);
		//2014.9.18 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014)
        const SCHARSTATS& sstats = pGLCharacter->m_sSUMSTATS_SKILL;

        if ( !(pContentItemData->sBasicOp.dwReqCharClass&dwCharClass) )
            nUseCount++;

        if ( !(pContentItemData->sBasicOp.dwReqSchool & index2school(dwSchool) ) )
            nUseCount++;

        if ( !(wLevel >= pContentItemData->sBasicOp.wReqLevelDW) )
            nUseCount++;

        //상한 레벨이 0으로 셋팅되어 있어 비교가 안됨
        //if ( !(wLevel <= pItemData->sBasicOp.wReqLevelUP) )
        //	nUseCount++;

        if ( !(nMelee >= int(pContentItemData->sBasicOp.wReqPA)) )
            nUseCount++;

        if ( !(nRange >= int(pContentItemData->sBasicOp.wReqSA)) )
            nUseCount++;

// 		if ( !(nMagic >= int(pContentItemData->sBasicOp.wReqMA)) )
// 			nUseCount++;

        if ( !(pContentItemData->sBasicOp.sReqStats.wDex <= sstats.wDex ) )
            nUseCount++;

        if ( !(pContentItemData->sBasicOp.sReqStats.wInt <= sstats.wInt ) )
            nUseCount++;

        if ( !(pContentItemData->sBasicOp.sReqStats.wPow <= sstats.wPow ) )
            nUseCount++;

        if ( !(pContentItemData->sBasicOp.sReqStats.wSpi <= sstats.wSpi ) )
            nUseCount++;

        if ( !(pContentItemData->sBasicOp.sReqStats.wSta <= sstats.wSta ) )
            nUseCount++;

        if ( !(pContentItemData->sBasicOp.sReqStats.wStr <= sstats.wStr ) )
            nUseCount++;

        if ( nUseCount>0 )
            m_pISUSE->SetTextNoSplit( ID2GAMEWORD_NOUSE, NS_UITEXTCOLOR::RED );
        else
            m_pISUSE->SetTextNoSplit( ID2GAMEWORD_USE, NS_UITEXTCOLOR::WHITE );
    }

    // 판매자 이름
    {    
        m_pSELLER_NAME->SetOneLineText( SaleItem.strChaName.c_str() );
    }

    // 마감시간
    {
        int nH = GetRemainTime();
        DWORD dwOriginalColor = D3DCOLOR_ARGB( 255, 0, 128, 192 );

        if( SaleItem.wTurnNum == 0 && SaleItem.wTotalTurnNum != 0 )
        {
            m_pExpirationDate->SetOneLineText( MARKET_RPPAGE_ROW_TEXT2, dwOriginalColor );
        }
        else if( 0 <nH )
        {
            std::string strTemp = sc::string::format( MARKET_RPPAGE_ROW_TEXT0, nH );

            float fRate = nH / 10.0f;
            if( 1.0f < fRate )
                fRate = 1.0f;

            BYTE byA = static_cast<BYTE>(((dwOriginalColor)&0xFF000000)>>24);
            BYTE byR = static_cast<BYTE>(((dwOriginalColor)&0x00FF0000)>>16);
            BYTE byG = static_cast<BYTE>(((dwOriginalColor)&0x0000FF00)>>8);
            BYTE byB = static_cast<BYTE>(((dwOriginalColor)&0x000000FF));

            //byR = static_cast<BYTE>( byR * fRate );
            byR = static_cast<BYTE>( byR + 0xFF * ( 1 - fRate ) );
            byG = static_cast<BYTE>( byG * fRate );
            byB = static_cast<BYTE>( byB * fRate );

            m_pExpirationDate->SetOneLineText( strTemp.c_str(), D3DCOLOR_ARGB( byA, byR, byG, byB ) );
        }
        else
        {
            m_pExpirationDate->SetOneLineText( MARKET_RPPAGE_ROW_TEXT1, NS_UITEXTCOLOR::RED );

        }
        
    }

    // 금액
    if( SaleItem.SellType == private_market::eSellType_ConsignmentSale_RanPoint )
    {
        CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( SaleItem.llPrice, 3, "," );
        m_pITEM_COST->SetOneLineText( strText , NS_UITEXTCOLOR::ORANGE );
    }
    else
    {        
        CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( SaleItem.llPrice, 3, "," );
        m_pITEM_COST->SetOneLineText( strText , MONEYCOLOR::GetMoneyColor( SaleItem.llPrice ) );
    }
    //수량
    {
        std::string strText = sc::string::format( "(%1%/%2%)", SaleItem.wTurnNum, SaleItem.wTotalTurnNum );
        m_pTurnNum->SetOneLineText( strText.c_str(), D3DCOLOR_ARGB(255,163,239,45) );
    }
}

private_market::ConsignmentSaleItem MarketRPPage::MarketRPPageRow::GetItem()
{
    return m_SaleItem;
}


int MarketRPPage::MarketRPPageRow::GetRemainTime()
{
    if( m_SaleItem.sItemcustom.GetNativeID() == NATIVEID_NULL() )
        return 0;

    //__time64_t tCurTime = CTime::GetCurrentTime().GetTime();
	__time64_t tCurTime = m_pGaeaClient->GetCurrentTime().GetTime();
    __time64_t tSpanTime = m_SaleItem.ExpirationDate - tCurTime;

    int nHour;
    nHour = static_cast<int> (tSpanTime / 3600);

    return nHour;
}
