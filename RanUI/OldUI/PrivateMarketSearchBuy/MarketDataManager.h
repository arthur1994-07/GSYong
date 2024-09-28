//���λ��� �ý��� ������ �Ŵ���

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

	// ������ ���� (�̹���, �̸�, ����)
	SITEMCUSTOM m_sItemcustom;
	// �ɸ��� ���� (�̸�, ��ġ)
	struct  
	{
		std::string srtCharName;
		D3DXVECTOR3 vecCharPos;
	};
	 
	// �˻�â ������
	// ��ٱ��� ������
	// �ŷ����� ������
	std::vector<UI_OPTION> m_UiOption;
	std::string m_OptionFielName;

	bool LoadUiOptionFile();
};

#endif 