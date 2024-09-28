#pragma once

#include "../../RanLogic/Product/GLProductDataType.h"

struct CHAR_PRODUCT_LOG
{
	DWORD dwChaNum;
	std::string strAction;
	DWORD dwProductType;
	DWORD dwProductID;
	std::string strStat;
	int point;
	int totalPoint;

	CHAR_PRODUCT_LOG()
		: dwChaNum( 0 )
		, dwProductType( 0 )
		, dwProductID( 0 )
		, point( 0 )
		, totalPoint( 0 )
	{

	}
};