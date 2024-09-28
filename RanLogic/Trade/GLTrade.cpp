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

BOOL GLTrade::InsertItem ( const WORD wPosX, const WORD wPosY, SINVENITEM *pInvenItem /*�κ�*/ )
{
	GASSERT(pInvenItem);

	//	Note : �̹� ��ϵ� ���������� �˻�.
	SINVENITEM* pBackItem = m_sTradeBox.FindItemBack ( pInvenItem->wPosX, pInvenItem->wPosY );
	if ( pBackItem )
		return FALSE;

	//	������ ��ġ�� ���� �ִ��� �˻�.
	const SITEM *pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return FALSE;

	BOOL bOk = m_sTradeBox.IsInsertable ( wPosX, wPosY );
	if ( !bOk )
		return FALSE;

	//	������ ��ġ�� ����.
	m_sTradeBox.InsertItem ( pInvenItem->sItemCustom, wPosX, wPosY, pInvenItem->wPosX, pInvenItem->wPosY );

	return TRUE;
}

BOOL GLTrade::InsertItem ( SINVENITEM *pInvenItem /*�ŷ��ڽ�*/ )
{
	GASSERT(pInvenItem);

	//	Note : �̹� ��ϵ� ���������� �˻�.
	SINVENITEM* pBackItem = m_sTradeBox.FindItemBack ( pInvenItem->wBackX, pInvenItem->wBackY );
	if ( pBackItem )
		return FALSE;

	//	������ ��ġ�� ���� �ִ��� �˻�.
	const SITEM *pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return FALSE;

	BOOL bOk = m_sTradeBox.IsInsertable ( pInvenItem->wPosX, pInvenItem->wPosY );
	if ( !bOk )
		return FALSE;

	//	������ ��ġ�� ����.
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
        return S_FALSE; // �ŷ� â �� ��� ������ �ŷ� �Ұ�. by luxes.

	m_fBlockTimer -= fElapsedTime;
	if ( m_fBlockTimer < 0.0f )
        m_fBlockTimer = 0.0f;

	return S_OK;
}


