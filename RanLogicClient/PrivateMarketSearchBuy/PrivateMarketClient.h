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

	int m_nTotalItemNum; //총아이템 개수
	int m_nTotalWhishItemNum; //총아이템 개수
	void SetTotalItemNum(int nValue);
	void SetTotalWhishItemNum(int nValue);

	//검색창 검색조건
	//-- 기본조건
	void SetItemNameSearch( CString strName );
	void SetItemType(int nType);
	void SetTimeLimit( BOOL bBool );
	void SetItemTurnNum(DWORD dwNum);
	void SetWeaponType(int nType);
	void SetArmorType(int nType);
	void SetItemStatsType(int nType);// 힘, 민첩, 정신
	void SetItemStatsValue(int nValue);
	void SetItemLevelCategory(int nRank);
	void SetClassCategory(int nClassCategory);
	void SetSexCategory(int nSexCategory);
	//-- 일반능력치
	void SetNomalStatsCategory_1(int nType, float fValue);
	void SetNomalStatsCategory_2(int nType, float fValue);
	void SetNomalStatsCategory_3(int nType, float fValue);
	void SetNomalStatsCategory_4(int nType, float fValue);
	//-- 가산 및 랜덤 옵션
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

	//검색창 데이터
	std::vector<private_market::sSaleItem> m_vecSearchPageData;
	void SetSearchResultData(private_market::sSaleItem sSaleItem);
	std::vector<private_market::sSaleItem*> GetSearchResultData();
	int m_nSearchPageCount;	//총 페이지수
	int GetSearchPageCount();
	void SetSearchPageData();
	void SetSearchPageCount();
	std::vector<SSEARCHPAGEDATA*> GetSearchPageData( int nPage );
	const private_market::sSaleItem* GetSearchPageData_sSaleItem( DWORD dwIndex );
	void ClearSearchItemdate();
	void ClearSearchPageData();
	CString GetSearchItemName();


	//찜바구니 데이터
	std::vector<private_market::sSaleItem> m_vecWhishPageData;
	int m_nWhishPageCount; //총 페이지수
	int GetWhishPageCount();
	bool SetWhishPageData( private_market::sSaleItem* psSaleItem );
	std::vector<private_market::sSaleItem*> GetWhishPageData( int nPage );
	const private_market::sSaleItem* GetWishPageData_sSaleItem( DWORD dwIndex );
	void DelWhishPageData( UUID uuidID );
	void SetWhishPageCount();
	void ClearWhishPageData();

	//거래내역 데이터
	std::vector<private_market::sSaleLog> m_sMaketLog;
	int m_nTotalLogCount; //총 로그 수
	int m_nMaketLogPegeCount; //총 페이지수
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
	// 아이템 리스트 루아 테이블 생성
	void GenItemLuaTable( const std::vector< private_market::sSaleItem >& vecSaleItem, LuaTable& tb );

public :
	// 아이템 구매 요청
	void ReqBuySearchItem( LuaTable tbPurchase );
	// 아이템 검색 요청
	bool SearchItem( WORD wPage, LuaTable tbSearchInfo );

	// 검색 - 아이템 리스트
	LuaTable GetSearchItemList();

	// 검색 - 검색 대상이 될 수 있는 아이템 이름 리스트;
	LuaTable GetSearchAbleItemNameList();

	// 찜바구니 - 아이템 리스트
	LuaTable GetWishItemList();
	// 찜바구니 - 아이템 추가
	bool InsertWishItem( WORD wSearchIndex );
	// 찜바구니 - 아이템 삭제
	bool DeleteWishItem( WORD wWishIndex );

	// 거래 내역 - 리스트 갱신
	bool RefreshLog();
	// 거래 내역 - 아이템 리스트
	LuaTable GetLogList();
	
	// 포인트 거래 - 리스트 요청
	void ReqPointTradeList();
	// 포인트 거래 - 등록된 아이템 정보
	LuaTable GetPointItemList();
	// 포인트 거래 - 판매 가능한 금화 아이템 정보
	LuaTable GetSellingMoneyList();
	// 포인트 거래 - 판매 기간 정보
	LuaTable GetSellPeriodList();
	// 포인트 거래 - 확장 슬롯관련 정보
	LuaTable GetExpansionSlotInfo();
	// 포인트 거래 - 판매 수수료 비율
	float GetSalesCompletionFee();
	// 포인트 거래 - 아이템 등록 요청
	bool ReqRegistItem( int nPosX, int nPosY, WORD wCount, LONGLONG llItemPrice, 
		WORD wSellType, WORD wMoneyIndex, WORD wPeriodIndex );
	// 포인트 거래 - 아이템 등록 취소 요청
	bool ReqUnregistItem( int nSelectedIndex );
	// 포인트 거래 - 아이템에 대한 판매 수수료
	DWORD CalcInsertionFee( int nPeriodIndex, WORD wMID, WORD wSID );
	// 포인트 거래 - 최소 가격
	WORD GetPointTradeMinPrice();

	// 포인트 거래 - 판매 등록가능 여부 확인
	int PointTradeCheckRegItem( WORD wPosX, WORD wPosY );
	
	// 개인상점개설관련
protected:
	bool m_bOpener;
	DWORD m_dwOpenerID;

public:
	// 개인상점 타이틀 설정, 아이템등록UI 열기
	void ReqPrivateMarketRegItemOpen( const char* cMarketName );

	// 아이템등록 메시지박스 열기
	void ReqPrivateMarketRegItemMsgBoxOpen( int nPosX, int nPosY );

	// 아이템등록 요청
	void ReqPrivateMarketRegItem( WORD wPosX, WORD wPosY, LONGLONG llMoney, DWORD dwNum );

	// 아이템삭제 요청
	void ReqPrivateMarketDelItem( WORD wPosX, WORD wPosY );

	// 개인상점 개설요청
	void ReqPrivateMarketOpen( bool bOnlyUI );

	// 개인상점 닫기
	void ReqPrivateMarketClose();

	// 다른 사람의 개인상점 정보요청
	void ReqPMarketInfo( DWORD dwGaeaID );	

	// 다른 사람의 개인상점 정보응답
	// void MsgPMarketInfo();

	// 개인상점 개설응답
	void MsgPrivateMarketOpenFb();

	// 아이템 구입팝업
	void ReqPMarketBuy( WORD wPosX, WORD wPosY );

	// 아이템 구입
	void ReqPMarketBuyItem( WORD wPosX, WORD wPosY, DWORD dwNum = 1 );

	// 아이템목록 갱신
	void UpdatePrivateMarketItemList( GLPrivateMarket& sPMarket, const DWORD dwChaDBNum, const bool bMe = true );

	const bool& IsOpener() { return m_bOpener; }

	// 개인상점 개설유저 GaeaID
	const DWORD& GetOpenerID() { return m_dwOpenerID; }

	// 등록된 아이템인가?
	bool IsRegInven( WORD wPosX, WORD wPosY );
	// 판매 완료된 아이템인가?
	bool IsSold( WORD wPosX, WORD wPosY );
};

#endif