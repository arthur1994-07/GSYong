#pragma once

#include "../../RanLogic/Trade/GLTrade.h"

class GLGaeaClient;

class GLTradeClient
{
public:
    GLTradeClient(GLGaeaClient* pGaeaClient);
    virtual ~GLTradeClient(void);

protected:
    GLGaeaClient* m_pGaeaClient;
	BOOL			m_bTrade;
	GLTrade			m_sMyTrade;
	GLTrade			m_sTarTrade;
	SINVEN_POS		m_sPreTradeItem;

public:
	BOOL Start ( DWORD dwTargetID );
	BOOL Reset ();

	BOOL Valid ();

public:
	const SINVEN_POS& GetPreItem ()		{ return m_sPreTradeItem; }
	void SetPreItem ( const SINVEN_POS& sPreItem );

	BOOL ValidPreItem ()				{ m_sPreTradeItem.VALID(); }
	void ReSetPreItem ()				{ m_sPreTradeItem.RESET(); }

public:
	SINVENITEM* FindPosItem ( const WORD wPosX, const WORD wPosY );
	SINVENITEM* FindItemBack ( const WORD wBackX, const WORD wBackY );
	BOOL IsInsertable ( const WORD wPosX, const WORD wPosY );
	BOOL IsExInsertable ( const WORD wPosX, const WORD wPosY );

public:
	BOOL IsAgreeAble ();
	void BlockAgree ();

public:
	GLTrade& GetMyTrade()	{ return m_sMyTrade; }
	GLTrade& GetTarTrade()	{ return m_sTarTrade; }

	GLInventory& GetMyTradeBox () { return m_sMyTrade.GetTradeBox(); }
	GLInventory& GetTarTradeBox () { return m_sTarTrade.GetTradeBox(); }

public:
	BOOL ReqTradeOpen();
	BOOL ReqTradeMoney ( LONGLONG lnMoney );
	BOOL ReqTradeLock();
	BOOL ReqTradeAgree();
	BOOL ReqTradeCancel();
	void ReqTradeAns(bool OKnCancel);

protected:
	BOOL MsgTradeFB(NET_MSG_GENERIC* nmg);
	BOOL MsgTradeLockTar(NET_MSG_GENERIC* nmg); // by luxes.
	BOOL MsgTradeAgreeTar(NET_MSG_GENERIC* nmg);
	BOOL MsgTradeItemResistTar(NET_MSG_GENERIC* nmg);
	BOOL MsgTradeItemReMoveTar(NET_MSG_GENERIC* nmg);
	BOOL MsgTradeMoneyTar(NET_MSG_GENERIC* nmg);
	BOOL MsgTradeCompleteTar(NET_MSG_GENERIC* nmg);
	BOOL MsgTradeCancelTar(NET_MSG_GENERIC* nmg);

public:
	void MsgProcess(NET_MSG_GENERIC* nmg);

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
};
