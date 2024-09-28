#include "pch.h"
#include "../Item/GLItemMan.h"
#include "../GLogicDataMan.h"
#include "./GLTrade.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLTrade::GLTrade(void)
	: m_dwTargetID( GAEAID_NULL )
	, m_bAgree( FALSE )
	, m_fBlockTimer( EMTRADE_BLOCK_TIME )
	, m_lnMoney( 0 )
	, m_bLock(FALSE)
{
}

GLTrade::~GLTrade(void)
{
}

void GLTrade::Reset ()
{
	m_dwTargetID = GAEAID_NULL;
	m_bAgree = FALSE;

	m_fBlockTimer = EMTRADE_BLOCK_TIME;
	m_sTradeBox.DeleteItemAll ();
	m_lnMoney = 0;

	m_bLock = FALSE;
}

void GLTrade::SetLock( BOOL bLock )
{
	m_bLock = bLock;
}

BOOL GLTrade::SetAgree ()
{
	if ( !IsAgreeAble() )
        return FALSE;

	m_bAgree = TRUE;
	return TRUE;
}

void GLTrade::BlockAgree ()
{
	ReSetAgree ();
	SetBlockTime ();
}

void GLTrade::SetMoney ( const LONGLONG lnMoney )
{
	if ( lnMoney >= 0 )
	{
		m_lnMoney = lnMoney;
	}

	if ( m_lnMoney < 0 )
	{
		m_lnMoney = 0;
	}
}

BOOL GLTrade::InsertItem ( const WORD wPosX, const WORD wPosY, SINVENITEM *pInvenItem /*인벤*/ )
{
	GASSERT(pInvenItem);

	//	Note : 이미 등록된 아이템인지 검사.
	SINVENITEM* pBackItem = m_sTradeBox.FindItemBack ( pInvenItem->wPosX, pInvenItem->wPosY );
	if ( pBackItem )
		return FALSE;

	//	지정된 위치에 들어갈수 있는지 검사.
	const SITEM *pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return FALSE;

	BOOL bOk = m_sTradeBox.IsInsertable ( wPosX, wPosY );
	if ( !bOk )
		return FALSE;

	//	지정된 위치에 넣음.
	m_sTradeBox.InsertItem ( pInvenItem->sItemCustom, wPosX, wPosY, pInvenItem->wPosX, pInvenItem->wPosY );

	return TRUE;
}

BOOL GLTrade::InsertItem ( SINVENITEM *pInvenItem /*거래박스*/ )
{
	GASSERT(pInvenItem);

	//	Note : 이미 등록된 아이템인지 검사.
	SINVENITEM* pBackItem = m_sTradeBox.FindItemBack ( pInvenItem->wBackX, pInvenItem->wBackY );
	if ( pBackItem )
		return FALSE;

	//	지정된 위치에 들어갈수 있는지 검사.
	const SITEM *pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return FALSE;

	BOOL bOk = m_sTradeBox.IsInsertable ( pInvenItem->wPosX, pInvenItem->wPosY );
	if ( !bOk )
		return FALSE;

	//	지정된 위치에 넣음.
	m_sTradeBox.InsertItem ( pInvenItem->sItemCustom, pInvenItem->wPosX, pInvenItem->wPosY, pInvenItem->wBackX, pInvenItem->wBackY );

	return TRUE;
}

SINVENITEM* GLTrade::GetItem ( const WORD wPosX, const WORD wPosY )
{
	return m_sTradeBox.GetItem ( wPosX, wPosY );
}

BOOL GLTrade::DeleteItem ( const WORD wPosX, const WORD wPosY )
{
	return m_sTradeBox.DeleteItem ( wPosX, wPosY );
}

HRESULT GLTrade::FrameMove ( float fTime, float fElapsedTime )
{
	if ( !Valid() )
        return S_FALSE;
	if ( !IsLock() )
        return S_FALSE; // 거래 창 안 잠겨 있으면 거래 불가. by luxes.

	m_fBlockTimer -= fElapsedTime;
	if ( m_fBlockTimer < 0.0f )
        m_fBlockTimer = 0.0f;

	return S_OK;
}


