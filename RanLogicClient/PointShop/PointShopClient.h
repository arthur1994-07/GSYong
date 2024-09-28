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

//! Client Point Shop ����
//! Data �� xml �� �����ϰ� �ε��Ŀ� SQLite �� �����ȴ�.
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

    //! ���������� ��ٷο��� SQLite �� ����Ѵ�
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

    //! �Ǹ��ϴ� �������ΰ�?
    //! /return �Ǹſ���/����
    std::tr1::tuple<bool, __int64> ExistItem(const SNATIVEID& ItemId);
    
    //! �����۰���
    //! ������� 0 �̸� �Ǹ����� �ʴ� �������̴�
    __int64 ItemPrice(const SNATIVEID& ItemId);

private: 
	enum POINTSHOPCART{MAX_CARTLIST = 10};
	std::vector<pointshop::SellItemClient>	m_vecSellItemList;
	std::vector<SellItem>					m_vecCartList;
	
private:
	void SetSellVehicleList(std::vector<SellItemClient> &vecSellItemList);
	lua_tinker::table GetVehicleList(VEHICLE_TYPE emType);
	
	//�ߺ� ���� ������ ��ȯ
	void GetOverlapType( OUT std::vector<EMITEM_TYPE> &vecTYPE );
	//������ Ÿ�Կ� ���� ī�װ� �ε��� ��ȯ
	int TypeToCategory( const EMITEM_TYPE TYPE, std::vector<EMITEM_TYPE> &vecOverlapType);
	//ī�װ� �ε����� ������ Ÿ�� ��� ��ȯ
	void CategoryToType( OUT std::vector<EMITEM_TYPE> &vecTYPE, const int nCategory );

public :
	//ī�װ�(m_UiOption) �ε����� �޾Ƽ� �ش� ī�װ� �������� ��ȯ  
	lua_tinker::table GetCategoryItem( int nCategoryIndex );
	//������ �˻� �Լ� 
	lua_tinker::table GetItemList(const char *szSearchName,
								lua_tinker::table tbCartegoryList ,
								lua_tinker::table tbOrderList);
	
	lua_tinker::table GetCategoryList( ); //ī�װ� ������ ��ȯ�Ѵ�
	lua_tinker::table GetCartList(); //��ٱ��� ����� �ҷ��´�
	void AddCartListScaleform( const int nIndex );
	void DelCartList(); //��ٱ��� ��� ����
	void BuyCartList(); //��ٱ��� ��� ����

//Slot Event Func
	const int AddCartList( const SITEMCUSTOM &sItemCustom);
	bool DelCartItem( const SITEMCUSTOM &sItemCustom, const int nIndex);

	// Ż�� ���� ����Ʈ
	lua_tinker::table GetVehicleBoardList();
	// Ż�� ����ũ ����Ʈ
	lua_tinker::table GetVehicleBikeList();
	// Ż�� ���� ����Ʈ
	lua_tinker::table GetVehicleOilList();
	// Ż�� ���� ����Ʈ
	lua_tinker::table GetVehiclePartsList();
};

} // namespace pointshop

#endif // _POINT_SHOP_CLIENT_H_