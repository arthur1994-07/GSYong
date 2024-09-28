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
	//sc::MGUID Guid;			// ������ ���� ID  
	CString	cstrSellerName; // �Ǹ��� �̸�
	SNATIVEID	sNaTiveID_Map;// �Ǹ��� ��ġ ��
	D3DXVECTOR3	m_vPosition;// �Ǹ��� ��ġ ��ǥ
	SITEMCUSTOM	sItemcustom;// ������ ������
	LONGLONG	lnItemPrice;
};

struct SWHISHLISTDATA: public SSEARCHPAGEDATA
{

};

struct SBUYHISTORYDATA
{

};

#endif 