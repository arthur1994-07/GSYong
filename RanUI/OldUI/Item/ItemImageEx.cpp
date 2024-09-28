#include "StdAfx.h"

#include "../Item/ItemImageEx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CItemImageEx::CItemImageEx(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
	: CItemImage(pGaeaClient, pEngineDevice)
    , m_wTurnNum(0)
	, m_pNumberBox(NULL)
{
}

CItemImageEx::~CItemImageEx ()
{
}

void CItemImageEx::CreateSubControl (const char* szControlKeywordImageSize )
{
	CItemImage::CreateSubControl ("" ,D3DCOLOR_XRGB(255,255,255), szControlKeywordImageSize);

	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "ITEM_TURNNUM" );
	pTextBox->SetFont ( pFont8 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_RIGHT );
	RegisterControl ( pTextBox );

	m_pNumberBox = pTextBox;
	m_pNumberBox->SetVisibleSingle ( FALSE );
}

void CItemImageEx::ResetItem ()
{
	CItemImage::ResetItem ();

	if ( m_pNumberBox )
	{
		m_wTurnNum = 0;
		m_pNumberBox->SetVisibleSingle( FALSE );
	}
}

void CItemImageEx::SetItemProcess( SNATIVEID sNativeID )
{
	CItemImage::SetItemProcess ( sNativeID );
	
	SetTurnNum ( 0 );
}

void CItemImageEx::ReSetItemProcess()
{
	CItemImage::ReSetItemProcess();
	
	SetTurnNum ( 0 );
}

void CItemImageEx::SetTurnNum ( const WORD wTurnNum )
{
	if ( !m_pNumberBox )
	{
		return;
	}

	m_wTurnNum = wTurnNum;

	if ( wTurnNum == 0 || m_sNativeID == NATIVEID_NULL() )
	{
		m_pNumberBox->SetVisibleSingle ( FALSE );
		return;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( m_sNativeID );
	if ( !pItem )
	{
		m_pNumberBox->SetVisibleSingle ( FALSE );
		return;
	}

	WORD wMaxTurnNum = pItem->sDrugOp.wPileNum;

	if ( wMaxTurnNum <= 1 )
	{
		m_pNumberBox->SetVisibleSingle ( FALSE );
		return;
	}

	CString strNumber;
	strNumber.Format ( "%d", wTurnNum );
	DWORD dwColor = NS_UITEXTCOLOR::GREENYELLOW;
	if ( wTurnNum == wMaxTurnNum ) dwColor = NS_UITEXTCOLOR::ORANGE;
	m_pNumberBox->SetOneLineText   ( strNumber, dwColor );
	m_pNumberBox->SetVisibleSingle ( TRUE );

}
