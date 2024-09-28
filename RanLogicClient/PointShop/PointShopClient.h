#ifndef _POINT_SHOP_CLIENT_H_
#define _POINT_SHOP_CLIENT_H_

#pragma once

#include "../../SigmaCore/Database/Sqlite/SQLite.h"
#include "../../RanLogic/PointShop/PointShop.h"

#include "../Widget/GLWidgetScript.h"

namespace ticpp
{
    class Document;
    class Exception;
}

class GLGaeaClient;

namespace pointshop
{

//! Client Point Shop 관리
//! Data 는 xml 로 저장하고 로딩후에 SQLite 로 관리된다.
class PointShopClient // : public PointShop
{
public:
    enum { VERSION=1, };

public:
    PointShopClient(GLGaeaClient* pGaeaClient);
    virtual ~PointShopClient();

private:
    GLGaeaClient* m_pGaeaClient;
    __time64_t m_UpdateDate;
    std::string m_CacheFileName;
    std::string m_OptionFielName;

    std::vector<UI_OPTION> m_UiOption;

    //! 정렬조건이 까다로워서 SQLite 를 사용한다
    SQLite::Database m_SQLite;

private:
    void WriteErrorLog(ticpp::Exception& ex);
    bool MakeDbFile();
    bool LoadXml(const std::string& XmlFile);
    bool Parse(ticpp::Document* pXml);
    bool ParseTimeStamp(ticpp::Document* pXml);
    bool ParseSellItem(ticpp::Document* pXml);
    void MakeWhereQuery(
        const std::string& SearchName,
        std::string& Query,
        const std::vector<EMITEM_TYPE>& TypeList);
    void MakeOrderQuery(const std::vector<SEARCH_ORDER>& OrderList,
        std::string& Query);

    bool LoadUiOptionFile();

public:
    void SetUpdateDate(__time64_t UpdateTime);
    __time64_t UpdateDate() const { return m_UpdateDate; }

    bool Load();
    void Save();
    void Clear();

    bool InsertItem(SNATIVEID ItemId, __int64 ItemPrice);
    bool InsertItem(const SellItem& Item);
    
    void GetList(
        std::vector<SellItemClient>& ItemList,
        const std::string& SearchName,
        const std::vector<EMITEM_TYPE>& TypeList,
        const std::vector<SEARCH_ORDER>& OrderList);

    const std::vector<UI_OPTION>& GetUiOption() const { return m_UiOption; }

    //! 판매하는 아이템인가?
    //! /return 판매여부/가격
    std::tr1::tuple<bool, __int64> ExistItem(const SNATIVEID& ItemId);
    
    //! 아이템가격
    //! 결과값이 0 이면 판매하지 않는 아이템이다
    __int64 ItemPrice(const SNATIVEID& ItemId);

private: 
	enum POINTSHOPCART{MAX_CARTLIST = 10};
	std::vector<pointshop::SellItemClient>	m_vecSellItemList;
	std::vector<SellItem>					m_vecCartList;
	
private:
	void SetSellVehicleList(std::vector<SellItemClient> &vecSellItemList);
	lua_tinker::table GetVehicleList(VEHICLE_TYPE emType);
	
	//중복 적용 아이템 반환
	void GetOverlapType( OUT std::vector<EMITEM_TYPE> &vecTYPE );
	//아이템 타입에 대한 카테고리 인덱스 반환
	int TypeToCategory( const EMITEM_TYPE TYPE, std::vector<EMITEM_TYPE> &vecOverlapType);
	//카테고리 인덱스로 아이템 타입 목록 반환
	void CategoryToType( OUT std::vector<EMITEM_TYPE> &vecTYPE, const int nCategory );

public :
	//카테고리(m_UiOption) 인덱스를 받아서 해당 카테고리 아이템을 반환  
	lua_tinker::table GetCategoryItem( int nCategoryIndex );
	//아이템 검색 함수 
	lua_tinker::table GetItemList(const char *szSearchName,
								lua_tinker::table tbCartegoryList ,
								lua_tinker::table tbOrderList);
	
	lua_tinker::table GetCategoryList( ); //카테고리 정보를 반환한다
	lua_tinker::table GetCartList(); //장바구니 목록을 불러온다
	void AddCartListScaleform( const int nIndex );
	void DelCartList(); //장바구니 목록 제거
	void BuyCartList(); //장바구니 목록 구매

//Slot Event Func
	const int AddCartList( const SITEMCUSTOM &sItemCustom);
	bool DelCartItem( const SITEMCUSTOM &sItemCustom, const int nIndex);

	// 탈것 보드 리스트
	lua_tinker::table GetVehicleBoardList();
	// 탈것 바이크 리스트
	lua_tinker::table GetVehicleBikeList();
	// 탈것 오일 리스트
	lua_tinker::table GetVehicleOilList();
	// 탈것 파츠 리스트
	lua_tinker::table GetVehiclePartsList();
};

} // namespace pointshop

#endif // _POINT_SHOP_CLIENT_H_