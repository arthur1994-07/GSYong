#pragma once

#include <boost/tr1/unordered_map.hpp>
#include "MarketDefine.h"
#include "../Inventory/GLInventoryPrivateMarket.h"

class GLPrivateMarket
{
public:
    typedef std::map<DWORD, SSALEITEM> MAPITEM;
	typedef MAPITEM::iterator		   MAPITEM_ITER;
    typedef MAPITEM::const_iterator	   MAPITEM_CITER;
    typedef MAPITEM::value_type        MAPITEM_VALUE;

public:
    GLPrivateMarket();
    virtual ~GLPrivateMarket();

protected:
	bool		m_bOPEN;
	BOOL		m_bOnlyUI;		// 단순 UI 만 렌더링 할 것인가~? ( 메인캐릭터용 )
	std::string	m_strTITLE;
	MAPITEM		m_mapSALE;
	GLInventoryPrivateMarket m_invenSALE;

	MAPSEARCH	m_mapSearchItem;

public:
	bool FindInsertPos(const SNATIVEID& nidITEM, SNATIVEID& sSALEPOS);

public:
	void SetOnlyUI( BOOL bOnlyUI )					{ m_bOnlyUI = bOnlyUI; }
	void SetTITLE(const std::string& strTITLE);
	bool SetSaleState(const SNATIVEID& sSALEPOS, DWORD dwNUM, bool bSOLD);

	bool DisItem(const SNATIVEID& sSALEPOS, bool bSearchMarket=FALSE);
	bool RegItem(const SINVENITEM& sInvenItem, LONGLONG llPRICE, DWORD dwNUM, SNATIVEID sSALEPOS=SNATIVEID(false), bool bSearchMarket=FALSE);

public:
	bool IsOpen() const { return m_bOPEN; }
	bool IsRegItem(SNATIVEID nidITEM);
	bool IsRegInvenPos(SNATIVEID sINVENPOS);
	bool IsSoldPos( SNATIVEID sInvenPos );

public:
	const std::string& GetTitle() const  { return m_strTITLE; }
	DWORD GetItemNum() const { return static_cast<DWORD>(m_mapSALE.size()); }
	GLInventory& GetInven()	{ return m_invenSALE; }

	DWORD GetItemTurnNum(SNATIVEID nidITEM);

	GLPrivateMarket::MAPITEM& GetItemList() { return m_mapSALE; }
	SSALEITEM* GetItem(SNATIVEID sSALEPOS);
	SSALEITEM* GetItem( const UUID& sGUID );

public:
	virtual bool DoMarketOpen();
	virtual void DoMarketClose();

	virtual void DoMarketInfoRelease();
};
