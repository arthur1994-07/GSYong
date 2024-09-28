//개인상점 시스템 데이터 매니저

#ifndef _MARKET_DATA_MANAGER_H_
#define _MARKET_DATA_MANAGER_H_

#include "PrivateMarketSearchBuyDefine.h"

#pragma once
class GLGaeaClient;

class MaketDataMng
{
	MaketDataMng(GLGaeaClient* pGaeaClient);
	virtual ~MaketDataMng();

private:
	GLGaeaClient* m_pGaeaClient;

	// 아이템 정보 (이미지, 이름, 가격)
	SITEMCUSTOM m_sItemcustom;
	// 케릭터 정보 (이름, 위치)
	struct  
	{
		std::string srtCharName;
		D3DXVECTOR3 vecCharPos;
	};
	 
	// 검색창 데이터
	// 찜바구니 데이터
	// 거래내역 데이터
	std::vector<UI_OPTION> m_UiOption;
	std::string m_OptionFielName;

	bool LoadUiOptionFile();
};

#endif 