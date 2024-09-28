#ifndef _PRIVATE_MARKET_CLIENT_DEFINE_H_
#define _PRIVATE_MARKET_CLIENT_DEFINE_H_

#pragma once

#include <vector>
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../RanLogic/Item/GLItemDef.h"

const int MARKET_ROW = 8;
const int LOG_ROW = 12;


struct SSEARCHPAGEDATA
{
	//sc::MGUID Guid;			// 아이템 고유 ID  
	CString	cstrSellerName; // 판매자 이름
	SNATIVEID	sNaTiveID_Map;// 판매자 위치 맵
	D3DXVECTOR3	m_vPosition;// 판매자 위치 좌표
	SITEMCUSTOM	sItemcustom;// 아이템 데이터
	LONGLONG	lnItemPrice;
};

struct SWHISHLISTDATA: public SSEARCHPAGEDATA
{

};

struct SBUYHISTORYDATA
{

};

#endif 