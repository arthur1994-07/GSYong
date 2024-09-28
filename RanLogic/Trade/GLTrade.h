#pragma once

#include "../Inventory/GLInventoryTrade.h"

enum
{
	EMTRADE_BLOCK_TIME = 0, // ���� 3���� 0���� ����. ��, �ð� ������ ���ݴ�. by luxes.
};

struct NET_MSG_GENERIC;

class GLTrade
{
protected:
	DWORD		m_dwTargetID;
	BOOL		m_bAgree;
	BOOL		m_bLock; // �ŷ�â ���. by luxes.

	float		m_fBlockTimer;
	GLInventoryTrade	m_sTradeBox;
	LONGLONG	m_lnMoney;

public:
	void Reset ();
	BOOL Valid ()						{	return ( m_dwTargetID != GAEAID_NULL ); }

public:
	void SetTarget ( DWORD dwTarID )	{ m_dwTargetID = dwTarID; }
	DWORD GetTarget ()					{ return m_dwTargetID; }

	// �ŷ�â ����� ���� ��. by luxes.
public:
	void SetLock( BOOL bLock );
	BOOL IsLock()						{ return m_bLock; }
	//

public:
	BOOL IsAgreeAble ()					{ return m_fBlockTimer==0.0f; }
	BOOL SetAgree ();
	void ReSetAgree ()					{ m_bAgree = FALSE; }
	BOOL GetAgree ()					{ return m_bAgree; }

public:
	void SetBlockTime ()				{ m_fBlockTimer = EMTRADE_BLOCK_TIME; }
	void BlockAgree ();

public:
	void SetMoney ( const LONGLONG lnMoney );
	LONGLONG GetMoney ()						{ return m_lnMoney; }

	//	server.
	BOOL InsertItem ( const WORD wPosX, const WORD wPosY, SINVENITEM *pInvenItem );
	
	//	client
	BOOL InsertItem ( SINVENITEM *pInvenItem );
	BOOL DeleteItem ( const WORD wPosX, const WORD wPosY );

	GLInventory& GetTradeBox ()				{ return m_sTradeBox; }

public:
	SINVENITEM* GetItem ( const WORD wPosX, const WORD wPosY );
	DWORD GetNumItems() { return m_sTradeBox.GetNumItems(); }

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );

public:
	GLTrade(void);
	~GLTrade(void);
};
