#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../enginelib/GUInterface/UIEditBox.h"
#include "../../../enginelib/GUInterface/UIEditBoxMan.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../Util/CheckBox.h"
#include "../Util/MessageBox.h"
#include "../../InnerInterface.h"
#include "../Item/ItemImageEx.h"

#include "MarketSearchRow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace
{
	const char* ID2GAMEWORD_ITEM_NAME = "달빛섬광의 목걸이";
	const char* ID2GAMEWORD_USE= "(사용가능)";
	const char* ID2GAMEWORD_NOUSE= "(사용불가)";
	const char* ID2GAMEWORD_SELLER_NAME = "판매자캐릭터명";
	const char* ID2GAMEWORD_EXPIRATIONDATE = "세이트사림교도소(999/999)";
	const char* ID2GAMEWORD_COST = "14,000,000,000";
	const char* ID2GAMEWORD_BUY_COUNT = "구입개수";
}


MarketSearchRow::MarketSearchRow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx( pInterface, pEngineDevice )
, m_pGaeaClient( pGaeaClient )
, m_pInterface( pInterface )
, m_pITEM_IMAGE( NULL )
, m_pITEM_NAME( NULL )
, m_pISUSE( NULL )
, m_pSELLER_NAME( NULL )
, m_pSELLER_POS( NULL )
, m_pITEM_COST( NULL )
, m_pITEM_COUNT_BUY( NULL )
, m_pCheckBox( NULL )
, m_pRowBGLine( NULL )
, m_llPrice( 0 )
, m_pItemCountEditBox( NULL )
, m_pItemCountEditBoxBG( NULL )
{
	ID2GAMEWORD_USE= ID2GAMEWORD("MARKET_SEARCH_BUY_ITEM_USED",0);
	ID2GAMEWORD_NOUSE= ID2GAMEWORD("MARKET_SEARCH_BUY_ITEM_NOTUSED",0);
	ID2GAMEWORD_BUY_COUNT = ID2GAMEWORD("MARKET_SEARCH_BUY_COUNT",0);
}

MarketSearchRow::~MarketSearchRow()
{
}

void MarketSearchRow::CreateSubControl()
{
	{
		CBasicLineBoxSmart* pItemCountEditBoxBG = new CBasicLineBoxSmart(m_pEngineDevice);
		pItemCountEditBoxBG->CreateSub( this, "MARKET_SEARCH_ROW_ITEM_COUNT_EDITBOX_BG", UI_FLAG_DEFAULT );
		pItemCountEditBoxBG->CreateSubControl( "GRAY_BACKGROUND_WHITE_LINE_TEXTURE" );
		pItemCountEditBoxBG->SetVisibleSingle(FALSE);
		RegisterControl( pItemCountEditBoxBG );
		m_pItemCountEditBoxBG = pItemCountEditBoxBG;
	}

	{// 검색 아이템 행 라인박스
		m_pRowBGLine = new CBasicLineBoxSmart(m_pEngineDevice);
		m_pRowBGLine->CreateSub( this, "MARKET_SEARCH_ROW_BG" );
		m_pRowBGLine->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( m_pRowBGLine );

		m_pSelectRow = new CUIControl(m_pEngineDevice);
		m_pSelectRow->CreateSub( this, "MARKET_SEARCH_ROW_BG",ID_ROW );
		RegisterControl( m_pSelectRow );
	}
	{	//아이콘 이미지
		CItemImageEx* pItemImage = new CItemImageEx( m_pGaeaClient, m_pEngineDevice );
		pItemImage->CreateSub( this, "MARKET_SEARCH_ROW_ITEM_IMAGE", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_ITEM_IMAGE );
		pItemImage->CreateSubControl();
		RegisterControl( pItemImage );
		m_pITEM_IMAGE = pItemImage;

		// 아이템 이름
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		m_pITEM_NAME = new CBasicTextBox(m_pEngineDevice);
		m_pITEM_NAME->CreateSub( this, "MARKET_SEARCH_ROW_ITEM_NAME" );
		m_pITEM_NAME->SetFont( pFont );
		m_pITEM_NAME->SetText( ID2GAMEWORD_ITEM_NAME,NS_UITEXTCOLOR::WHITE );
		m_pITEM_NAME->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pITEM_NAME );

		// 착용 가능 불가능
		m_pISUSE  = new CBasicTextBox(m_pEngineDevice);
		m_pISUSE->CreateSub( this, "MARKET_SEARCH_ROW_ISUSE" );
		m_pISUSE->SetFont( pFont );
		m_pISUSE->SetText( ID2GAMEWORD_USE,NS_UITEXTCOLOR::WHITE );
		m_pISUSE->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pISUSE );

		// 판매자 이름
		m_pSELLER_NAME = new CBasicTextBox(m_pEngineDevice);
		m_pSELLER_NAME->CreateSub( this, "MARKET_SEARCH_ROW_SELLER_NAME" );
		m_pSELLER_NAME->SetFont( pFont );
		m_pSELLER_NAME->SetText( ID2GAMEWORD_SELLER_NAME,NS_UITEXTCOLOR::WHITE );
		m_pSELLER_NAME->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pSELLER_NAME );

		// 판매자 위치
		m_pSELLER_POS = new CBasicTextBox(m_pEngineDevice);
		m_pSELLER_POS->CreateSub( this, "MARKET_SEARCH_ROW_SELLER_POS" );
		m_pSELLER_POS->SetFont( pFont );
		m_pSELLER_POS->SetText( ID2GAMEWORD_EXPIRATIONDATE,NS_UITEXTCOLOR::WHITE );
		m_pSELLER_POS->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pSELLER_POS );

		// 구입가격
		m_pITEM_COST = new CBasicTextBox(m_pEngineDevice);
		m_pITEM_COST->CreateSub( this, "MARKET_SEARCH_ROW_ITEM_COST" );
		m_pITEM_COST->SetFont( pFont );
		m_pITEM_COST->SetText( ID2GAMEWORD_COST,NS_UITEXTCOLOR::ORANGE );
		m_pITEM_COST->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pITEM_COST );

        // 란포인트 거래 아이콘 포인트 아이콘
        m_pRanPointIcon = CreateLineBox( "MARKET_SEARCH_ROW_ITEM_COST_ICON", "RAN_POINT_ICON_TEXTURE" );
		// [골드]체크박스,아이콘
		m_pGoldIcon = CreateLineBox( "MARKET_SEARCH_ROW_ITEM_COST_ICON", "RAN_MONEY_ICON_TEXTURE" );


		// 구입개수
		m_pITEM_COUNT_BUY = new CBasicTextBox(m_pEngineDevice);
		m_pITEM_COUNT_BUY->CreateSub( this, "MARKET_SEARCH_ROW_ITEM_COUNT" );
		m_pITEM_COUNT_BUY->SetFont( pFont );
		m_pITEM_COUNT_BUY->SetText( ID2GAMEWORD_BUY_COUNT,NS_UITEXTCOLOR::WHITE );
		m_pITEM_COUNT_BUY->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		m_pITEM_COUNT_BUY->SetVisibleSingle(FALSE);
		RegisterControl( m_pITEM_COUNT_BUY );

		//구입개수 EDIT BOX
		m_pItemCountEditBox = new CUIEditBoxMan(m_pEngineDevice);
		m_pItemCountEditBox->CreateSub ( this, "MARKET_SEARCH_PAGE_NOMAL_STATS_EDITBOX",UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ID_COUNT_EDITBOX );
		m_pItemCountEditBox->CreateEditBox(ID_COUNT_EDITBOX_1,"MARKET_SEARCH_ROW_ITEM_COUNT_EDITBOX", "DEFAULT_CARRAT", TRUE, UINT_MAX,pFont, 10,FALSE, TRUE);
		m_pItemCountEditBox->SetVisibleSingle(FALSE);
		RegisterControl ( m_pItemCountEditBox );

		//[기간제]체크박스
		CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
		pCheckBox->CreateSub( this, "MARKET_SEARCH_ROW_CHECK_BOX", UI_FLAG_DEFAULT,ID_CHECK_BOX );
		pCheckBox->CreateSubControl( "ANOTHER_CHARACTER_ALLWEAR_CHECK", "ANOTHER_CHARACTER_ALLWEAR_UNCHECK" );
		RegisterControl( pCheckBox );
		m_pCheckBox = pCheckBox;
	}


}


void MarketSearchRow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( m_pItemCountEditBox->GetEditBox( ID_COUNT_EDITBOX_1 )->MouseUpdate( x, y ) )
    {
        AddMessageEx( UIMSG_MESSAGEBOX_POSITIVE );
    }

	if( m_psSaleItem && static_cast<int>(m_psSaleItem->dwItemTurnNum) <GetItemCount())
		m_pItemCountEditBox->SetEditString(ID_COUNT_EDITBOX_1, sc::string::format( "%d", m_psSaleItem->dwItemTurnNum ).c_str());
}


void MarketSearchRow::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
	//CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	switch (cID)
	{
	case ID_ITEM_IMAGE:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				//마우스 커서를 업했을때 해당 라인의 아이템 정보 표시
				if( m_sItemCustom.GetNativeID() != NATIVEID_NULL() )
				{
					BOOL bChecked = TRUE;

					m_pInterface->SHOW_ITEM_INFO(
						SITEMCUSTOM( m_sItemCustom ),
						FALSE, FALSE, FALSE, TRUE, bChecked,
						USHRT_MAX, USHRT_MAX );
				}
			}
		}
		break;
	}

}

void MarketSearchRow::SetCheckBoxVisible( BOOL bBool )
{
	m_pCheckBox->SetVisibleSingle(bBool);
}

void MarketSearchRow::SetItemImage( const SITEMCUSTOM& sItemCustom, WORD wTurnNum)
{
	m_sItemCustom = sItemCustom;

	m_pITEM_COUNT_BUY->SetVisibleSingle( FALSE );
	m_pItemCountEditBox->SetVisibleSingle( FALSE );
	m_pItemCountEditBoxBG->SetVisibleSingle( FALSE );

	m_pITEM_IMAGE->ResetItem();

	const SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
	if ( !pItemData )
		return;
	m_pITEM_IMAGE->SetItem( sItemCustom );
	m_pITEM_IMAGE->SetTurnNum( wTurnNum );

	if ( pItemData->ISPILE())
	{
		m_pITEM_COUNT_BUY->SetVisibleSingle( TRUE );
		m_pItemCountEditBox->SetVisibleSingle( TRUE );
		m_pItemCountEditBoxBG->SetVisibleSingle( TRUE );
	}

}

void MarketSearchRow::IsUseItem( const SITEMCUSTOM& sItemCustom )
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


	SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
	if ( !pItemData )
		return;

	if ( !(pItemData->sBasicOp.dwReqCharClass&dwCharClass) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.dwReqSchool & index2school(dwSchool) ) )
		nUseCount++;

	if ( !(wLevel >= pItemData->sBasicOp.wReqLevelDW) )
		nUseCount++;

	//상한 레벨이 0으로 셋팅되어 있어 비교가 안됨
	//if ( !(wLevel <= pItemData->sBasicOp.wReqLevelUP) )
	//	nUseCount++;

	if ( !(nMelee >= int(pItemData->sBasicOp.wReqPA)) )
		nUseCount++;

	if ( !(nRange >= int(pItemData->sBasicOp.wReqSA)) )
		nUseCount++;

// 	if ( !(nMagic >= int(pItemData->sBasicOp.wReqMA)) )
// 		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wDex <= sstats.wDex ) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wInt <= sstats.wInt ) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wPow <= sstats.wPow ) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wSpi <= sstats.wSpi ) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wSta <= sstats.wSta ) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wStr <= sstats.wStr ) )
		nUseCount++;

	if ( nUseCount>0 )
		m_pISUSE->SetTextNoSplit( ID2GAMEWORD_NOUSE, NS_UITEXTCOLOR::RED );
	else
		m_pISUSE->SetTextNoSplit( ID2GAMEWORD_USE, NS_UITEXTCOLOR::WHITE );
}

void MarketSearchRow::SetItemName( const SITEMCUSTOM& sItemCustom )
{
	const SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
	if ( !pItemData )
		return;

	m_pITEM_NAME->SetTextToLimit( sItemCustom.GETNAME().c_str(), ITEM_INFOR_COLOR::dwItemRank[ pItemData->sBasicOp.emLevel ] );
}

void MarketSearchRow::SetTitemSellerName( const CString& strText )
{
	m_pSELLER_NAME->SetOneLineText( strText );
}

// void MarketSearchRow::SetSellType( private_market::eSellType eSellType )
// {
//     if( eSellType == private_market::eSellType_ConsignmentSale_RanPoint )
//         m_pRanPointIcon->SetVisibleSingle( TRUE );
//     else
//         m_pRanPointIcon->SetVisibleSingle( FALSE );
// }

void MarketSearchRow::SetTitemSellerPos()
{
	m_pSELLER_POS;
}

void MarketSearchRow::SetItemPrice( LONGLONG lnPrice, private_market::eSellType eSellType  )
{
    if( eSellType == private_market::eSellType_ConsignmentSale_RanPoint )
    {
        m_pRanPointIcon->SetVisibleSingle( TRUE );
		m_pGoldIcon->SetVisibleSingle( FALSE );

        m_llPrice = lnPrice;
        CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( lnPrice, 3, "," );
        m_pITEM_COST->SetOneLineText( strText , NS_UITEXTCOLOR::ORANGE );
    }
    else
    {
        m_pRanPointIcon->SetVisibleSingle( FALSE );
		m_pGoldIcon->SetVisibleSingle( TRUE );

        m_llPrice = lnPrice;
        CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( lnPrice, 3, "," );
        m_pITEM_COST->SetOneLineText( strText , MONEYCOLOR::GetMoneyColor(lnPrice) );
    }

}

void MarketSearchRow::SetItemCount()
{
	m_pITEM_COUNT_BUY;
}

BOOL MarketSearchRow::IsChecked()
{
	return 	m_pCheckBox->IsChecked();
}

void MarketSearchRow::SetCheckOff()
{
	m_pCheckBox->SetCheckOff();
}

SITEMCUSTOM MarketSearchRow::GetItemData()
{
	return m_sItemCustom;
}

void MarketSearchRow::SetCheckOn()
{
	m_pCheckBox->SetCheckOn();
}

void MarketSearchRow::SetUUID( UUID uuidID )
{
	m_sItemGUID = uuidID;
}

UUID MarketSearchRow::GetUUID()
{
	return m_sItemGUID;
}

LONGLONG MarketSearchRow::GetPrice()
{
	return m_llPrice;
}

void MarketSearchRow::SetsSaleItem( private_market::sSaleItem* sSaleItem )
{
	m_psSaleItem = sSaleItem;
}

private_market::sSaleItem* MarketSearchRow::GetsSaleItem()
{
	return m_psSaleItem;
}

void MarketSearchRow::SetMapName( const CString strMapName, DWORD dwColor )
{
	m_pSELLER_POS->SetText(strMapName, dwColor);
}

int MarketSearchRow::GetItemCount()
{
	int nItemCount  = atoi( m_pItemCountEditBox->GetEditString(ID_COUNT_EDITBOX_1));

	if ( nItemCount <= 0 )
		nItemCount = 1;
	return nItemCount;
}

void MarketSearchRow::ItemCoutntEditBoxClear()
{
	m_pItemCountEditBox->ClearEdit(ID_COUNT_EDITBOX_1);
}
