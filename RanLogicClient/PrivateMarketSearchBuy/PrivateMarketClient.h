#ifndef _PRIVATE_MARKET_CLIENT_H_
#define _PRIVATE_MARKET_CLIENT_H_
#pragma once

#include "../../RanLogic/Market/MarketDefine.h"
//#include "../../RanLogic/Market/GLPrivateMarketItemCache.h"
#include "PrivateMarketClientDefine.h"

#include "../Widget/GLWidgetScript.h"
//#include "MarketSearchLuaLoad.h"

class GLGaeaClient;
class GLPrivateMarket;

class PrivateMarketClient
{
public:
	PrivateMarketClient(GLGaeaClient* pGaeaClient);
	~PrivateMarketClient();

	GLGaeaClient* m_pGaeaClient;
	//GLPrivateMarketItemCache m_PrivateMarketItemCache;

	private_market::sSaleItemSearchandSortBasic m_sSaleItemSearchandSortBasic;
	CString m_strSearchItemName;

	int m_nTotalItemNum; //�Ѿ����� ����
	int m_nTotalWhishItemNum; //�Ѿ����� ����
	void SetTotalItemNum(int nValue);
	void SetTotalWhishItemNum(int nValue);

	//�˻�â �˻�����
	//-- �⺻����
	void SetItemNameSearch( CString strName );
	void SetItemType(int nType);
	void SetTimeLimit( BOOL bBool );
	void SetItemTurnNum(DWORD dwNum);
	void SetWeaponType(int nType);
	void SetArmorType(int nType);
	void SetItemStatsType(int nType);// ��, ��ø, ����
	void SetItemStatsValue(int nValue);
	void SetItemLevelCategory(int nRank);
	void SetClassCategory(int nClassCategory);
	void SetSexCategory(int nSexCategory);
	//-- �Ϲݴɷ�ġ
	void SetNomalStatsCategory_1(int nType, float fValue);
	void SetNomalStatsCategory_2(int nType, float fValue);
	void SetNomalStatsCategory_3(int nType, float fValue);
	void SetNomalStatsCategory_4(int nType, float fValue);
	//-- ���� �� ���� �ɼ�
	void SetAddRandomOptionCategory_1(int nType, float fValue);
	void SetAddRandomOptionCategory_2(int nType, float fValue);
	void SetAddRandomOptionCategory_3(int nType, float fValue);
	void SetAddRandomOptionCategory_4(int nType, float fValue);

	private_market::sSaleItemSearchandSortBasic SetSearchData( CString strName  = "",
																int nItemType = NULL,
																BOOL bBool = FALSE,
																DWORD dwItemTurnNum = NULL,
																int nWeaponType = NULL,
																int nArmorType = NULL,
																int nStatsType = NULL,
																int nStatsValue = NULL,
																int nItemLRank = NULL,
																int nClassCategory = NULL,
																int nSexCategory = NULL,
																int nNomalStatsType1 = NULL,
																float fNomalStatsValue1 = NULL,
																int nNomalStatsType2 = NULL,
																float fNomalStatsValue2 = NULL,
																int nNomalStatsType3 = NULL,
																float fNomalStatsValue3 = NULL,
																int nNomalStatsType4 = NULL,
																float fNomalStatsValue4 = NULL,
																int nAddRandomType1 = NULL,
																float fAddRandomValue1 = NULL,
																int nAddRandomType2 = NULL,
																float fAddRandomValue2 = NULL,
																int nAddRandomType3 = NULL,
																float fAddRandomValue3 = NULL,
																int nAddRandomType4 = NULL,
																float fAddRandomValue4 = NULL,
																int nSearchSellType = 0 );

	//�˻�â ������
	std::vector<private_market::sSaleItem> m_vecSearchPageData;
	void SetSearchResultData(private_market::sSaleItem sSaleItem);
	std::vector<private_market::sSaleItem*> GetSearchResultData();
	int m_nSearchPageCount;	//�� ��������
	int GetSearchPageCount();
	void SetSearchPageData();
	void SetSearchPageCount();
	std::vector<SSEARCHPAGEDATA*> GetSearchPageData( int nPage );
	const private_market::sSaleItem* GetSearchPageData_sSaleItem( DWORD dwIndex );
	void ClearSearchItemdate();
	void ClearSearchPageData();
	CString GetSearchItemName();


	//��ٱ��� ������
	std::vector<private_market::sSaleItem> m_vecWhishPageData;
	int m_nWhishPageCount; //�� ��������
	int GetWhishPageCount();
	bool SetWhishPageData( private_market::sSaleItem* psSaleItem );
	std::vector<private_market::sSaleItem*> GetWhishPageData( int nPage );
	const private_market::sSaleItem* GetWishPageData_sSaleItem( DWORD dwIndex );
	void DelWhishPageData( UUID uuidID );
	void SetWhishPageCount();
	void ClearWhishPageData();

	//�ŷ����� ������
	std::vector<private_market::sSaleLog> m_sMaketLog;
	int m_nTotalLogCount; //�� �α� ��
	int m_nMaketLogPegeCount; //�� ��������
	int GetLogPageCount();
	void SetTotalLogCount(const int nTotalLogCount);
	void SetLogPageCount();
	void SetLogPageData(private_market::sSaleLog sMaketLog);
	std::vector<private_market::sSaleLog*> GetLogtData();
	BOOL m_bLogEnd;
	BOOL ISLogEnd();
	void SetLogEnd(BOOL bLogEnd);
	void ClearLogPage();

	void ClearMarketSearchBuyData();

private:
	std::list<CString> m_listItemName;
	bool IsUseItem( const SITEMCUSTOM& sItemCustom );

public :
	void LoadSearchFileFromITEM();

private :
	// ������ ����Ʈ ��� ���̺� ����
	void GenItemLuaTable( const std::vector< private_market::sSaleItem >& vecSaleItem, LuaTable& tb );

public :
	// ������ ���� ��û
	void ReqBuySearchItem( LuaTable tbPurchase );
	// ������ �˻� ��û
	bool SearchItem( WORD wPage, LuaTable tbSearchInfo );

	// �˻� - ������ ����Ʈ
	LuaTable GetSearchItemList();

	// �˻� - �˻� ����� �� �� �ִ� ������ �̸� ����Ʈ;
	LuaTable GetSearchAbleItemNameList();

	// ��ٱ��� - ������ ����Ʈ
	LuaTable GetWishItemList();
	// ��ٱ��� - ������ �߰�
	bool InsertWishItem( WORD wSearchIndex );
	// ��ٱ��� - ������ ����
	bool DeleteWishItem( WORD wWishIndex );

	// �ŷ� ���� - ����Ʈ ����
	bool RefreshLog();
	// �ŷ� ���� - ������ ����Ʈ
	LuaTable GetLogList();
	
	// ����Ʈ �ŷ� - ����Ʈ ��û
	void ReqPointTradeList();
	// ����Ʈ �ŷ� - ��ϵ� ������ ����
	LuaTable GetPointItemList();
	// ����Ʈ �ŷ� - �Ǹ� ������ ��ȭ ������ ����
	LuaTable GetSellingMoneyList();
	// ����Ʈ �ŷ� - �Ǹ� �Ⱓ ����
	LuaTable GetSellPeriodList();
	// ����Ʈ �ŷ� - Ȯ�� ���԰��� ����
	LuaTable GetExpansionSlotInfo();
	// ����Ʈ �ŷ� - �Ǹ� ������ ����
	float GetSalesCompletionFee();
	// ����Ʈ �ŷ� - ������ ��� ��û
	bool ReqRegistItem( int nPosX, int nPosY, WORD wCount, LONGLONG llItemPrice, 
		WORD wSellType, WORD wMoneyIndex, WORD wPeriodIndex );
	// ����Ʈ �ŷ� - ������ ��� ��� ��û
	bool ReqUnregistItem( int nSelectedIndex );
	// ����Ʈ �ŷ� - �����ۿ� ���� �Ǹ� ������
	DWORD CalcInsertionFee( int nPeriodIndex, WORD wMID, WORD wSID );
	// ����Ʈ �ŷ� - �ּ� ����
	WORD GetPointTradeMinPrice();

	// ����Ʈ �ŷ� - �Ǹ� ��ϰ��� ���� Ȯ��
	int PointTradeCheckRegItem( WORD wPosX, WORD wPosY );
	
	// ���λ�����������
protected:
	bool m_bOpener;
	DWORD m_dwOpenerID;

public:
	// ���λ��� Ÿ��Ʋ ����, �����۵��UI ����
	void ReqPrivateMarketRegItemOpen( const char* cMarketName );

	// �����۵�� �޽����ڽ� ����
	void ReqPrivateMarketRegItemMsgBoxOpen( int nPosX, int nPosY );

	// �����۵�� ��û
	void ReqPrivateMarketRegItem( WORD wPosX, WORD wPosY, LONGLONG llMoney, DWORD dwNum );

	// �����ۻ��� ��û
	void ReqPrivateMarketDelItem( WORD wPosX, WORD wPosY );

	// ���λ��� ������û
	void ReqPrivateMarketOpen( bool bOnlyUI );

	// ���λ��� �ݱ�
	void ReqPrivateMarketClose();

	// �ٸ� ����� ���λ��� ������û
	void ReqPMarketInfo( DWORD dwGaeaID );	

	// �ٸ� ����� ���λ��� ��������
	// void MsgPMarketInfo();

	// ���λ��� ��������
	void MsgPrivateMarketOpenFb();

	// ������ �����˾�
	void ReqPMarketBuy( WORD wPosX, WORD wPosY );

	// ������ ����
	void ReqPMarketBuyItem( WORD wPosX, WORD wPosY, DWORD dwNum = 1 );

	// �����۸�� ����
	void UpdatePrivateMarketItemList( GLPrivateMarket& sPMarket, const DWORD dwChaDBNum, const bool bMe = true );

	const bool& IsOpener() { return m_bOpener; }

	// ���λ��� �������� GaeaID
	const DWORD& GetOpenerID() { return m_dwOpenerID; }

	// ��ϵ� �������ΰ�?
	bool IsRegInven( WORD wPosX, WORD wPosY );
	// �Ǹ� �Ϸ�� �������ΰ�?
	bool IsSold( WORD wPosX, WORD wPosY );
};

#endif