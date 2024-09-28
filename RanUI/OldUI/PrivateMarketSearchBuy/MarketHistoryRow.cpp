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

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Market/MarketDefine.h"

#include "../../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/WishList/WishList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"

#include "MarketHistoryRow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace
{
	const char* ID2GAMEWORD_DATE_PURCHASE = "12.09.07";
	const char* ID2GAMEWORD_TIME = "18:00";
	const char* ID2GAMEWORD_TRANSACTION = "�Ǹ�";
	const char* ID2GAMEWORD_ITEM_NAME = "+11����Ʈ�� ���� ��� (��)";
	const char* ID2GAMEWORD_ITEM_COUNT = "1";
	const char* ID2GAMEWORD_ITEM_PRICE = "4,000,000,000";
	const char* ID2GAMEWORD_SELLER_NAME = "���̻�����ĥ��";
}

MarketHistoryhRow::MarketHistoryhRow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx( pInterface, pEngineDevice )
, m_pRanPointIcon( NULL )
{

}

MarketHistoryhRow::~MarketHistoryhRow()
{

}

void MarketHistoryhRow::CreateSubControl()
{
	{	//�ŷ�����â �� [���Գ�¥, �ð�, �ŷ�, �����۸�, ����, �ŷ��ݾ�, �ŷ�ĳ���͸�] ���� ����
		//CUIControl* pObjectBar = new CUIControl(m_pEngineDevice);
		//pObjectBar->CreateSub ( this, "MARKET_HISTORY_PAGE_OBJECT_BAR",UI_FLAG_XSIZE | UI_FLAG_BOTTOM );
		//pObjectBar->SetUseRender ( TRUE );
		//pObjectBar->SetDiffuse ( D3DCOLOR_ARGB(130,74,69,42) );
		//RegisterControl ( pObjectBar );
		{// �˻� ������ �� ���ιڽ�
			CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
			pLineBox->CreateSub( this, "MARKET_HISTORY_ROW_BG" );
			pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
			RegisterControl( pLineBox );
		}

		// [���Գ�¥] �ؽ�Ʈ �ڽ�
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CBasicTextBox* pDatePurchaseTextBox = new CBasicTextBox(m_pEngineDevice);
		pDatePurchaseTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_ROW_DATE_PURCHASE" );
		pDatePurchaseTextBox->SetFont( pFont );
		pDatePurchaseTextBox->SetText( ID2GAMEWORD_DATE_PURCHASE,NS_UITEXTCOLOR::WHITE );
		pDatePurchaseTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pDatePurchaseTextBox );
		m_pDatePurchaseTextBox = pDatePurchaseTextBox;

		// [�ð�] �ؽ�Ʈ �ڽ�
		CBasicTextBox* pTimeTextBox  = new CBasicTextBox(m_pEngineDevice);
		pTimeTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_ROW_TIME" );
		pTimeTextBox->SetFont( pFont );
		pTimeTextBox->SetText( ID2GAMEWORD_TIME,NS_UITEXTCOLOR::WHITE );
		pTimeTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pTimeTextBox );
		m_pTimeTextBox = pTimeTextBox;

		// [�ŷ�] �ؽ�Ʈ �ڽ�
		CBasicTextBox* pTransactionTextBox = new CBasicTextBox(m_pEngineDevice);
		pTransactionTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_ROW_TRANSACTION" );
		pTransactionTextBox->SetFont( pFont );
		pTransactionTextBox->SetText( ID2GAMEWORD_TRANSACTION,NS_UITEXTCOLOR::WHITE );
		pTransactionTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pTransactionTextBox );
		m_pTransactionTextBox = pTransactionTextBox;

		// [�����۸�] �ؽ�Ʈ �ڽ�
		CBasicTextBox* pItemNameBox= new CBasicTextBox(m_pEngineDevice);
		pItemNameBox->CreateSub( this, "MARKET_HISTORY_PAGE_ROW_ITEM_NAME" );
		pItemNameBox->SetFont( pFont );
		pItemNameBox->SetText( ID2GAMEWORD_ITEM_NAME,NS_UITEXTCOLOR::WHITE );
		pItemNameBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pItemNameBox );
		m_pItemNameBox = pItemNameBox;

		// [����] �ؽ�Ʈ �ڽ�
		CBasicTextBox* pItemCountTextBox = new CBasicTextBox(m_pEngineDevice);
		pItemCountTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_ROW_ITEM_COUNT" );
		pItemCountTextBox->SetFont( pFont );
		pItemCountTextBox->SetText( ID2GAMEWORD_ITEM_COUNT,NS_UITEXTCOLOR::WHITE );
		pItemCountTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pItemCountTextBox );
		m_pItemCountTextBox = pItemCountTextBox;

		// [�ŷ��ݾ�] �ؽ�Ʈ �ڽ�
		CBasicTextBox* pItemPriceTextBox = new CBasicTextBox(m_pEngineDevice);
		pItemPriceTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_ROW_ITEM_PRICE" );
		pItemPriceTextBox->SetFont( pFont );
		pItemPriceTextBox->SetText( ID2GAMEWORD_ITEM_PRICE,NS_UITEXTCOLOR::WHITE );
		pItemPriceTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pItemPriceTextBox );
		m_pItemPriceTextBox = pItemPriceTextBox;

        m_pRanPointIcon = CreateLineBox( "MARKET_HISTORY_PAGE_ROW_ITEM_PRICE_RP_ICON", "RAN_POINT_ICON_TEXTURE" );

		// [�ŷ�ĳ���͸�] �ؽ�Ʈ �ڽ�
		CBasicTextBox* pSellerNameTextBox = new CBasicTextBox(m_pEngineDevice);
		pSellerNameTextBox->CreateSub( this, "MARKET_HISTORY_PAGE_ROW_SELLER_NAME" );
		pSellerNameTextBox->SetFont( pFont );
		pSellerNameTextBox->SetText( ID2GAMEWORD_SELLER_NAME,NS_UITEXTCOLOR::WHITE );
		pSellerNameTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pSellerNameTextBox );
		m_pSellerNameTextBox = pSellerNameTextBox;
	}
}

void MarketHistoryhRow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{

	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void MarketHistoryhRow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}

void MarketHistoryhRow::SetDatePurchaseText( const CString strText )
{
	m_pDatePurchaseTextBox->SetText( strText );
}

void MarketHistoryhRow::SetTimeTextBox( const CString strText )
{
	m_pTimeTextBox->SetText( strText );
}

void MarketHistoryhRow::SetTransactionText( const CString strText, DWORD dwColor  )
{
	m_pTransactionTextBox->SetText( strText, dwColor );
}

void MarketHistoryhRow::SetItiemNamText( const SNATIVEID sItemID )
{
	const SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemID );

	pItemData->GetName();

	m_pItemNameBox->SetText( pItemData->GetName(), ITEM_INFOR_COLOR::dwItemRank[ pItemData->sBasicOp.emLevel ]  );
}

void MarketHistoryhRow::SetItemCountText( const DWORD dwCount )
{
	std::string strText = stringformat("%s",dwCount);
	m_pItemCountTextBox->SetText( strText.c_str() );
}

void MarketHistoryhRow::SetItemPriceText( const LONGLONG llPrice, int nSellType, const bool bIsCommission, const float fCommissionRate )
{
    if( static_cast<private_market::eSellType>(nSellType) == private_market::eSellType_ConsignmentSale_RanPoint )
    {
        CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llPrice, 3, "," );
        m_pItemPriceTextBox->SetText( strText , NS_UITEXTCOLOR::ORANGE );
    }
    else
    {
        CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(llPrice, 3, "," );
        m_pItemPriceTextBox->SetText( strText, MONEYCOLOR::GetMoneyColor(llPrice) );
    }


	if ( bIsCommission )
	{
		std::string strCommission = sc::string::format( " (%0d%%)", fCommissionRate*100 );
		m_pItemPriceTextBox->AddString(0,strCommission.c_str());
	}


    if( nSellType == private_market::eSellType_ConsignmentSale_RanPoint )
        m_pRanPointIcon->SetVisibleSingle( TRUE );
    else
        m_pRanPointIcon->SetVisibleSingle( FALSE );
}

void MarketHistoryhRow::SetSellerNameText( const CString strText )
{
	m_pSellerNameTextBox->SetText( strText );
}

