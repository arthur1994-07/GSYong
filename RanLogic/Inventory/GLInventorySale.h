#pragma once

#include "./GLInventory.h"

struct stBuyInvenItem
{
	WORD wPosX, wPosY, wBuyNum;
	stBuyInvenItem(void)
		: wPosX(0), wPosY(0), wBuyNum(0){}
	stBuyInvenItem(WORD posX, WORD posY, WORD buyNum)
		: wPosX(posX), wPosY(posY), wBuyNum(buyNum){}
};
typedef std::vector<stBuyInvenItem> VECBUYINVENITEM;
typedef VECBUYINVENITEM::iterator VECBUYINVENITEM_ITR;

enum EEXTYPE
{	
	EX_ITEM_TYPE,	// 아이템으로 교환
	EX_POINT_TYPE,  // 포인트로 교환
	EX_LVP_TYPE,	// 생활점수로 교환
	EX_NONE,
	EX_MAX_TYPE = EX_NONE
};

// 필요 교환 아이템
typedef struct stExNeed
{
public :
	stExNeed() : iNeedCount(0) {}
	stExNeed(SNATIVEID sID, __int64 iCnt) { sNativeID = sID; iNeedCount = iCnt; }

	
public :
	__int64		iNeedCount;	
	SNATIVEID	sNativeID;

	stExNeed&	operator=( const stExNeed& rhs )
	{
		if( this == &rhs ) return *this;

		sNativeID  = rhs.sNativeID;
		iNeedCount = rhs.iNeedCount;

		return *this;

	}

}sExNeed, *ptExNeedItem;

// 필요 교환아이템 리스트
typedef struct stExchangeList
{
public:
	//stExchangeList() : wX(0), wY(0) { vNeedItem.clear(); }
	stExchangeList() : exType(EX_NONE), iCount(0) { vNeedItem.clear(); }

public:
	//WORD		wX;
	//WORD		wY;

	SNATIVEID	sNativeID;	// 대표아이템 ID
	BYTE		iCount;

	EEXTYPE		exType;

	std::vector<sExNeed> vNeedItem;

	stExchangeList& operator=( const stExchangeList& rhs)
	{
		if( this == &rhs ) return *this;

		//wX = rhs.wX;
		//wY = rhs.wY;
		sNativeID = rhs.sNativeID;

		exType = rhs.exType;

		iCount = rhs.iCount;

		vNeedItem.clear();

		std::vector<sExNeed>::const_iterator _iter = rhs.vNeedItem.begin();

		while( _iter != rhs.vNeedItem.end())
		{
			vNeedItem.push_back( (*_iter) );
			++_iter;
		}

		return *this;
	}
	

}sExList, *ptExList;

enum
{
	ESaleX			= 6,
	ESaleY			= 8,

	ESaleTotalSlot	= ESaleX * ESaleY,
};

#define CROW_SALE_V_INDEX(t)	(t - 1)

class GLInventorySale : public GLInventory
{
public :
	// NPC 교환 아이템 판매에 쓰임.
	enum CROW_SALE_TYPE_VERSION
	{
		CROW_SALE_NONE		  = 0, // 
		CROW_SALE_V_1		  =	1, // 아이템 머니로 거래      (.crowsale : szSaleType  )	szSaleType과 szTradeType은 같이 쓸수 없다.
		CROW_SALE_V_2		  =	2, // 아이템 교환을 통한 거래 (.crowsale : szTradeType )	szSaleType과 szTradeType은 같이 쓸수 없다.
		CROW_SALE_VERSION_MAX,
	};
	
	typedef std::map< int, sExList > map_ExList;
	typedef map_ExList::iterator	 map_ExList_iter;
	typedef map_ExList::value_type	 map_ExList_Value;

public:
	GLInventorySale()
		: GLInventory( ESaleX, ESaleY ), m_vSaleFuncType(CROW_SALE_NONE)
	{
		m_ExChangeList.clear();
	}

	GLInventorySale( WORD _sizeX, WORD _sizeY )
		: GLInventory( _sizeX, _sizeY ), m_vSaleFuncType(CROW_SALE_NONE)
	{
		m_ExChangeList.clear();
	}

	CROW_SALE_TYPE_VERSION GetSaleFuncType() const { return m_vSaleFuncType; }

	void SetSaleFuncType( CROW_SALE_TYPE_VERSION value ) { m_vSaleFuncType = value; }
	void push_exchangeData( WORD wCol, WORD wRow, const sExList* const pData );

	EEXTYPE GetExChangeType( const SNATIVEID& sId, WORD wCol, WORD wRow );
	EEXTYPE GetExchangeList( const SNATIVEID& sId, WORD wCol, WORD wRow, std::vector<sExNeed>& rList );
	bool    GetIsNeedConditionHave( std::vector<sExNeed>& rList, GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow, __int64 iRanPoint64 = 0, __int64 iLvPoint64 = 0, WORD wCount = 1 );

	bool IsNeedConditionHave( GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow, __int64 iRanPoint64 = 0, __int64 iLvPoint64 = 0, WORD wCount = 1 );

	bool IsNeedItemCondition(std::vector<sExNeed>& rList, GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow );
	bool IsNeedRanPointCondition(std::vector<sExNeed>& rList, GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow, __int64 iRanPoint64 );
	bool IsNeedLvPointCondition(std::vector<sExNeed>& rList, GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow, __int64 iLvPoint64 );

	bool IsUserExInsertEnable( GLInventory* pUserinven, SNATIVEID& sId, WORD wCol, WORD wRow, __int64 iRanPoint64 = 0, __int64 iLvPoint64 = 0, WORD wCount = 1 );

	SNATIVEID FindItemID( WORD wCol, WORD wRow );

	virtual void Assign( const GLInventory &Inven );

	// 구매 리스트; 구매자 인벤; 구매자 포인트; 구매자 생활정수;
	EEXTYPE IsBuyableItemList(IN VECBUYINVENITEM& vecItemlist, GLInventory* pUserinven
		, __int64 nRanPoint, __int64 nLvPoint);

private :
	map_ExList	m_ExChangeList;
	
	CROW_SALE_TYPE_VERSION	m_vSaleFuncType;


};