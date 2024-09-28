#pragma once

#include "../Item/GLItemCustom.h"

struct PRODUCT_EXCHANGE_LOG
{
	SITEMCUSTOM sITEM;
	//EMITEM_ROUTE routeType;
	int nNum;
	DWORD dwDbNum;

	DWORD dwProductID;
	DWORD dwProductType;
	int point;
	int totalPoint;
	LONGLONG tracePrice;

	PRODUCT_EXCHANGE_LOG()
		: nNum( 0 )
		, dwDbNum( 0 )
		, dwProductType( 0 )
		, dwProductID( 0 )
		, point( 0 )
		, totalPoint( 0 )
		, tracePrice( 0 )
	{

	}
};

struct PRODUCT_ITEM
{
	DWORD dwProductType;
	DWORD dwProductID;
	int point;

	PRODUCT_ITEM()
		: dwProductType( 0 )
		, dwProductID( 0 )
		, point( 0 )
	{

	}
};