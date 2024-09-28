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
	EX_ITEM_TYPE,	// ���������� ��ȯ
	EX_POINT_TYPE,  // ����Ʈ�� ��ȯ
	EX_LVP_TYPE,	// ��Ȱ������ ��ȯ
	EX_NONE,
	EX_MAX_TYPE = EX_NONE
};

// �ʿ� ��ȯ ������
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

// �ʿ� ��ȯ������ ����Ʈ
typedef struct stExchangeList
{
public:
	//stExchangeList() : wX(0), wY(0) { vNeedItem.clear(); }
	stExchangeList() : exType(EX_NONE), iCount(0) { vNeedItem.clear(); }

public:
	//WORD		wX;
	//WORD		wY;

	SNATIVEID	sNativeID;	// ��ǥ������ ID
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
	// NPC ��ȯ ������ �Ǹſ� ����.
	enum CROW_SALE_TYPE_VERSION
	{
		CROW_SALE_NONE		  = 0, // 
		CROW_SALE_V_1		  =	1, // ������ �ӴϷ� �ŷ�      (.crowsale : szSaleType  )	szSaleType�� szTradeType�� ���� ���� ����.
		CROW_SALE_V_2		  =	2, // ������ ��ȯ�� ���� �ŷ� (.crowsale : szTradeType )	szSaleType�� szTradeType�� ���� ���� ����.
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

	// ���� ����Ʈ; ������ �κ�; ������ ����Ʈ; ������ ��Ȱ����;
	EEXTYPE IsBuyableItemList(IN VECBUYINVENITEM& vecItemlist, GLInventory* pUserinven
		, __int64 nRanPoint, __int64 nLvPoint);

private :
	map_ExList	m_ExChangeList;
	
	CROW_SALE_TYPE_VERSION	m_vSaleFuncType;


};