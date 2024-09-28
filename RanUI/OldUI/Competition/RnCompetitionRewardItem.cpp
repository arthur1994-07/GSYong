#include "StdAfx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../enginelib/DeviceMan.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../Item/ItemImageEx.h"

#include "RnCompetitionRewardItem.h"

RnCompetitionRewardItem::RnCompetitionRewardItem( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pTurnNum(NULL)
{
}

RnCompetitionRewardItem::~RnCompetitionRewardItem()
{

}

void RnCompetitionRewardItem::CreateSubControl()
{
	m_pItemImage = new CItemImageEx( m_pGaeaClient, m_pEngineDevice );
	m_pItemImage->CreateSub( this, "RNCOMPETITION_ITEM_IMAGE_SIZE", UI_FLAG_XSIZE | UI_FLAG_BOTTOM );
	m_pItemImage->CreateSubControl("RNCOMPETITION_ITEM_IMAGE_SIZE");
	RegisterControl( m_pItemImage );

	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub( this, "RNCOMPETITION_ITEM_IMAGE_SIZE" );
	pLineBox->CreateSubControl( "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );
	RegisterControl( pLineBox );

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
	m_pTurnNum = new CBasicTextBox(m_pEngineDevice);
	m_pTurnNum->CreateSub( this, "RNCOMPETITION_ITEM_IMAGE_SIZE", UI_FLAG_DEFAULT );
	m_pTurnNum->SetFont( pFont );		
	m_pTurnNum->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_BOTTOM );
	RegisterControl( m_pTurnNum );
}

void RnCompetitionRewardItem::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nscroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nscroll, fElapsedTime, bFirstControl);
}

void RnCompetitionRewardItem::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void RnCompetitionRewardItem::SetItemId( SNATIVEID sNativeID )
{
	m_pItemImage->SetItem( sNativeID );
}

void RnCompetitionRewardItem::SetTurnNum( const WORD wTurnNum )
{
	if ( !m_pTurnNum )
	{
		return;
	}

	//m_pItemImage->SetTurnNum( wTurnNum );

	if ( wTurnNum == 0 || m_pItemImage->GetItemID() == NATIVEID_NULL() )
	{
		m_pTurnNum->SetVisibleSingle ( FALSE );
		return;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( m_pItemImage->GetItemID() );
	if ( !pItem )
	{
		m_pTurnNum->SetVisibleSingle ( FALSE );
		return;
	}

	WORD wMaxTurnNum = pItem->sDrugOp.wPileNum;

	if ( wMaxTurnNum <= 1 )
	{
		m_pTurnNum->SetVisibleSingle ( FALSE );
		return;
	}

	CString strNumber;
	strNumber.Format ( "%d", wTurnNum );
	DWORD dwColor = NS_UITEXTCOLOR::GREENYELLOW;
	if ( wTurnNum == wMaxTurnNum ) dwColor = NS_UITEXTCOLOR::ORANGE;
	m_pTurnNum->SetOneLineText   ( strNumber, dwColor );
	m_pTurnNum->SetVisibleSingle ( TRUE );
}

