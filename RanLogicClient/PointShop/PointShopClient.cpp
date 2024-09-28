#include "pch.h"
#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/File/TxtFile.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../EngineLib/Common/SUBPATH.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../GLGaeaClient.h"
#include "./PointShopClient.h"

#include "../../RanLogicClient/Widget/GLWidgetScript.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace pointshop
{
    
PointShopClient::PointShopClient(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient(pGaeaClient)
    , m_UpdateDate(0)
{
    std::string CacheFileName(sc::getAppPath());
    const SUBPATH* pPath = m_pGaeaClient->GetSubPath();
    CacheFileName.append(pPath->Cache()); //SUBPATH::CACHE);
    
    // Cache directory check and make
    if (!sc::file::isExist(CacheFileName))
        sc::file::createDirectory(CacheFileName);

    const std::string PointShopCacheFileName("PointShopCache.xml");
    CacheFileName.append(PointShopCacheFileName);

    m_CacheFileName = CacheFileName;

    MakeDbFile();    
	m_vecCartList.clear();
}

PointShopClient::~PointShopClient()
{
    if (m_SQLite.IsOpen())
        m_SQLite.Close(); 
}

bool PointShopClient::LoadUiOptionFile()
{
    m_UiOption.clear();

    try
    {
        std::string OptionFile(sc::getAppPath());
        const SUBPATH* pPath = m_pGaeaClient->GetSubPath();
        OptionFile.append(pPath->GLogicPath()); //SUBPATH::GLogicPath());
        OptionFile.append(GLogicData::GetInstance().GetPointShopUiOptionFileName());
        m_OptionFielName = OptionFile;

        sc::lua_init();
        if (!sc::lua_doFile(m_OptionFielName.c_str()))
        {        
            ::MessageBoxA(
                NULL,
                sc::lua_getError().c_str(),
                OptionFile.c_str(),
                MB_OK);
            return false;
        }

        LuaPlus::LuaObject OptionList = sc::lua_getGlobalFromName("PointShopOption");
		int nOptionCount = 0;
        for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next(), nOptionCount++)
        {
            UI_OPTION UiOption;
            for (LuaPlus::LuaTableIterator Typeiter(OptionIter.GetValue()); Typeiter; Typeiter.Next())
            {
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					std::wstring OptionNameW((const wchar_t*) Typeiter.GetValue().GetWString());
					std::string strOptionName;
					strOptionName = CGameTextMan::GetInstance().GetText(
						"POINTSHOP_OPTIONNAME", nOptionCount, CGameTextMan::EM_GAME_IN_TEXT).GetString();
					UiOption.m_OptionName = strOptionName;
				}
				else
				{
					std::wstring OptionNameW((const wchar_t*) Typeiter.GetValue().GetWString());
					UiOption.m_OptionName = sc::string::unicodeToAnsi(OptionNameW);
				}

                Typeiter.Next();

                for (LuaPlus::LuaTableIterator IntIter(Typeiter.GetValue()); IntIter; IntIter.Next())
                {
                    int IntType = IntIter.GetValue().GetInteger();
                    if (IntType < 0 || IntType >= ITEM_NSIZE)
                    {
                        std::string ErrorMsg(
                            sc::string::format(
                                "PointShopOption unknown option type %1%", IntType));
                        AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
                    }
                    else
                    {
                        UiOption.m_TypeVec.push_back(EMITEM_TYPE(IntType));
                    }
                }
                m_UiOption.push_back(UiOption);
            }            
        }
        return true;
    }
    catch (const LuaPlus::LuaException& e)
    {        
        ::MessageBoxA(
            NULL,
            e.GetErrorMessage(),
            m_OptionFielName.c_str(),
            MB_OK);
        return false;
    }
}

bool PointShopClient::MakeDbFile()
{    
    // memory db 로 관리한다.
    if (SQLITE_OK != m_SQLite.Open(":memory:"))
    {
        //ASSERT(0 && m_SQLite.GetLastError());
        return false;
    }

    // Table 생성
    const char* CreateTable = "\
CREATE TABLE PointShopItem( \
ItemMid   INTEGER NOT NULL, \
ItemSid   INTEGER NOT NULL, \
ItemPrice INTEGER NOT NULL, \
ItemType  INTEGER NOT NULL, \
ItemSuit  INTEGER NOT NULL, \
ItemName  TEXT    NOT NULL, \
PRIMARY KEY(ItemMid, ItemSid)); \
CREATE UNIQUE INDEX Idx_ItemId ON PointShopItem (ItemMid, ItemSid); \
CREATE INDEX Idx_ItemPrice ON PointShopItem (ItemPrice); \
CREATE INDEX Idx_ItemSuit  ON PointShopItem (ItemSuit); \
CREATE INDEX Idx_ItemName  ON PointShopItem (ItemName); \
CREATE INDEX Idx_ItemType  ON PointShopItem (ItemType); ";

    if (SQLITE_OK != m_SQLite.ExecuteSQL(CreateTable))
        return false;
    else
        return true;
}

bool PointShopClient::Load()
{
    LoadUiOptionFile();

    // xml check
    if (sc::file::isExist(m_CacheFileName))
        return LoadXml(m_CacheFileName);
    else
        return false;
}

void PointShopClient::Save()
{
    sc::file::remove(m_CacheFileName);
    sc::TxtFile XmlFile(m_CacheFileName, true);
    // Head
    XmlFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    XmlFile.write("<RAN_POINT_SHOP>");

    // Timestamp
    XmlFile.write(
        sc::string::format("  <TIMESTAMP>%1%</TIMESTAMP>", m_UpdateDate));
    
    // Sell Item
    XmlFile.write("  <SELL_ITEM>");

    std::vector<SellItemClient> ItemList;
    std::string SearchName("");
    std::vector<EMITEM_TYPE> TypeList;
    std::vector<SEARCH_ORDER> OrderList;
    GetList(ItemList, SearchName, TypeList, OrderList);

    for (size_t i=0; i<ItemList.size(); ++i)
    {
        const SellItemClient& Item = ItemList[i];
        XmlFile.write(
            sc::string::format(
                "    <ITEM M=\"%1%\" S=\"%2%\" P=\"%3%\" />",
                Item.ItemMid(),
                Item.ItemSid(),
                Item.Price()));
    }

    XmlFile.write("  </SELL_ITEM>");

    // Footer
    XmlFile.write("</RAN_POINT_SHOP>");
}

void PointShopClient::WriteErrorLog(ticpp::Exception& ex)
{
    sc::writeLogError(ex.m_details);
}

void PointShopClient::SetUpdateDate(__time64_t UpdateTime)
{
    m_UpdateDate = UpdateTime;
}

void PointShopClient::Clear()
{
    m_UpdateDate = 0;
    // Delete table data
    std::string Query= "DELETE FROM PointShopItem;";
    m_SQLite.ExecuteSQL(Query);
}

bool PointShopClient::LoadXml(const std::string& XmlFile)
{
    ticpp::Document* pXml = new ticpp::Document;
    try
    {        
        pXml->LoadFile(XmlFile, TIXML_ENCODING_UTF8);        
        bool bReturn = Parse(pXml);
        SAFE_DELETE(pXml);
        return bReturn;
    }
    catch (ticpp::Exception& ex)
    {
        WriteErrorLog(ex);
        SAFE_DELETE(pXml);
        return false;
    }
}

bool PointShopClient::Parse(ticpp::Document* pXml)
{
    if (!ParseTimeStamp(pXml) ||
        !ParseSellItem(pXml))
    {
        Clear();
        return false;
    }
    else
    {
        return true;
    }
}

bool PointShopClient::ParseTimeStamp(ticpp::Document* pXml)
{
    try
    {        
        ticpp::Element* pElem = (ticpp::Element*) pXml->FirstChild("RAN_POINT_SHOP")->FirstChild("TIMESTAMP");
        pElem->GetTextOrDefault<__time64_t, __time64_t> (&m_UpdateDate, 0);        
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        WriteErrorLog(ex);        
        sc::writeLogError(
            sc::string::format(
                "RAN_POINT_SHOP/TIMESTAMP value not define %1%", m_UpdateDate));
        return false;
    }
}

bool PointShopClient::ParseSellItem(ticpp::Document* pXml)
{
    try
    {    
        ticpp::Element* pParent = NULL;
        for (pParent = (ticpp::Element*) pXml->FirstChild("RAN_POINT_SHOP")->FirstChild("SELL_ITEM");
             pParent;
             pParent = (ticpp::Element*) pParent->NextSiblingElement(false))
        {
            ticpp::Element* pChild = NULL;
            for (pChild = (ticpp::Element*) pParent->FirstChild(false);
                 pChild;
                 pChild = (ticpp::Element*) pChild->NextSiblingElement(false))
            {
                std::string ValueName = pChild->Value();
                if (ValueName == "ITEM")
                {
                    WORD ItemMid = 0;
                    WORD ItemSid = 0;
                    __int64 Price = 0;
                    pChild->GetAttribute("M", &ItemMid);
                    pChild->GetAttribute("S", &ItemSid);
                    pChild->GetAttribute("P", &Price);
                    InsertItem(SNATIVEID(ItemMid, ItemSid), Price);
                }
            }
        }
    }
    catch (ticpp::Exception& ex)
    {
        WriteErrorLog(ex);
        return false;
    }
    return true;
}

bool PointShopClient::InsertItem(const SellItem& Item)
{
    return InsertItem(Item.ItemId(), Item.Price());
}

bool PointShopClient::InsertItem(SNATIVEID ItemId, __int64 ItemPrice)
{
    SITEM* pItem = GLogicData::GetInstance().GetItem(ItemId);
    if (!pItem)
    {
        // Item Data 체크
        sc::writeLogError(
            sc::string::format(
                "PointShopClient::InsertItem (%1%/%2%)",
                ItemId.Mid(),
                ItemId.Sid()));
        return false;
    }
    
    if (ItemPrice <= 0 || ItemPrice >= 2147483647)
    {
        // 가격체크
        sc::writeLogError(
            sc::string::format(
            "PointShopClient::InsertItem Price %1%",
            ItemPrice));
        return false;
    }
        
    std::string ItemName(pItem->GetName());
    sc::string::replaceForDb(ItemName);
    std::string Query(
        sc::string::format(
        "INSERT INTO PointShopItem(ItemMid, ItemSid, ItemPrice, ItemType, ItemSuit, ItemName) VALUES (%1%, %2%, %3%, %4%, %5%, '%6%');",
        ItemId.Mid(),
        ItemId.Sid(),
        ItemPrice,
        pItem->BasicType(),
        pItem->Suit(),
        ItemName));

    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
        return false;
    else
        return true;
}

void PointShopClient::GetList(
    std::vector<SellItemClient>& ItemList,
    const std::string& SearchName,
    const std::vector<EMITEM_TYPE>& TypeList,
    const std::vector<SEARCH_ORDER>& OrderList)
{    
    std::string Query("SELECT ItemMid, ItemSid, ItemPrice, ItemType, ItemSuit, ItemName FROM PointShopItem");
    
    // WHERE ------------------------------------------------------------------
    MakeWhereQuery(SearchName, Query, TypeList);

    // ORDER BY ---------------------------------------------------------------
    MakeOrderQuery(OrderList, Query);

    // Query ------------------------------------------------------------------
    SQLite::TablePtr pTable(m_SQLite.QuerySQL2(Query));
    if (pTable.m_pTable)
    {
        SQLite::Table& rTable = pTable();
        int nRowCount = rTable.GetRowCount();
        if (nRowCount <= 0)
            return;

        for (int loop=0; loop<nRowCount; ++loop)
        {
            WORD Mid         = rTable.GetValue<WORD>(0);
            WORD Sid         = rTable.GetValue<WORD>(1);
            __int64 Price    = rTable.GetValue<__int64>(2);
            EMITEM_TYPE Type = static_cast<EMITEM_TYPE> (rTable.GetValue<int>(3));
            EMSUIT Suit      = static_cast<EMSUIT> (rTable.GetValue<int>(4));
            std::string Name = rTable.GetValue<std::string>(5);

            SellItemClient Item(Mid, Sid, Price, Type, Suit, Name);
            ItemList.push_back(Item);
            rTable.GoNext();
        }
    }
}

void PointShopClient::MakeOrderQuery(const std::vector<SEARCH_ORDER>& OrderList, std::string& Query) 
{
    if (OrderList.size() > 0)
    {
        Query.append(" ORDER BY ");
        for (size_t i=0; i<OrderList.size(); ++i)
        {
            bool bType = false;
            const SEARCH_ORDER& Order = OrderList[i];
            switch (Order.Type())
            {
            case ITEM_MID:
                Query.append("ItemMid ");
                bType = true;
                break;
            case ITEM_SID:
                Query.append("ItemSid ");
                bType = true;
                break;
            case ITEM_PRICE:
                Query.append("ItemPrice ");
                bType = true;
                break;
            case ITEM_TYPE:
                Query.append("ItemType ");
                bType = true;
                break;
            case ITEM_SUIT:
                Query.append("ItemSuit ");
                bType = true;
                break;
            case ITEM_NAME:
                Query.append("ItemName ");
                bType = true;
                break;
            default:
                break;
            }

            if (bType)
            {
                if (Order.Degree() == SQL_ASC)
                    Query.append("ASC");
                else
                    Query.append("DESC");

                if (i != (OrderList.size()-1))
                    Query.append(", ");
            }
        }
    }
}

void PointShopClient::MakeWhereQuery( const std::string &SearchName, std::string &Query, const std::vector<EMITEM_TYPE> &TypeList ) 
{
    bool bSearchName = false;
    if (!SearchName.empty())
    {
        // 아이템 이름 포함 검색
        std::string SearchItemName(SearchName);
        sc::string::replaceForDb(SearchItemName);
        Query.append(
            sc::string::format(
            " WHERE ItemName LIKE '%%%1%%%' ",
            SearchItemName));
        bSearchName = true;
    }

    if (TypeList.size() > 0)
    {
        // 해당 타입 검색
        if (bSearchName)
            Query.append(" AND ");
        else
            Query.append(" WHERE ");

        Query.append( "( " );
        for (size_t i=0; i<TypeList.size(); ++i)
        {
            Query.append(
                sc::string::format(
                "ItemType=%1% ", TypeList[i]));
            if (i != (TypeList.size()-1))
                Query.append("OR ");
        }
        Query.append( " )" );
    }
}

//! 아이템가격
//! 결과값이 0 이면 판매하지 않는 아이템이다
__int64 PointShopClient::ItemPrice(const SNATIVEID& ItemId)
{
    std::tr1::tuple<bool, __int64> Result = ExistItem(ItemId);    
    return std::tr1::get<1>(Result);
}

//! 판매하는 아이템인가?
std::tr1::tuple<bool, __int64> PointShopClient::ExistItem(const SNATIVEID& ItemId)
{
    std::string Query(
        sc::string::format(
            "SELECT ItemPrice FROM PointShopItem WHERE ItemMid=%1% AND ItemSid=%2%",
            ItemId.Mid(),
            ItemId.Sid()));

    std::tr1::tuple<bool, __int64> Result;
    // Query ------------------------------------------------------------------
    SQLite::TablePtr pTable(m_SQLite.QuerySQL2(Query));
    if (pTable.m_pTable)
    {
        SQLite::Table& rTable = pTable();
        int nRowCount = rTable.GetRowCount();
        if (nRowCount <= 0)
            Result = std::tr1::make_tuple(false, 0);
        else
            Result = std::tr1::make_tuple(true, rTable.GetValue<__int64>(0));
    }

    return Result;
}

void PointShopClient::SetSellVehicleList( std::vector<SellItemClient> &vecSellItemList )
{
	std::string SearchName("");
	std::vector<EMITEM_TYPE> TypeList;
	std::vector<pointshop::SEARCH_ORDER> OrderList;
	GetList( vecSellItemList, SearchName, TypeList, OrderList );
}

lua_tinker::table PointShopClient::GetVehicleList( VEHICLE_TYPE emType )
{
	std::vector<pointshop::SellItemClient> vecSellItemList;
	SetSellVehicleList(vecSellItemList);

	lua_tinker::table tbItems( GLWidgetScript::GetInstance().GetLuaState() );

	int j = 0;
	for (size_t i=0; i < vecSellItemList.size(); ++i)
	{
		if( vecSellItemList[i].m_Type == ITEM_VEHICLE )
		{
			const SITEM* pItemData = GLogicData::GetInstance().GetItem ( vecSellItemList[i].ItemId() );
			if( pItemData->VehicleType() == emType )
			{
				tbItems.set( j + 1, vecSellItemList[i].ItemId().Id() );
				j++;
			}
		}
	}
	
	return tbItems;
}

lua_tinker::table PointShopClient::GetVehicleBoardList()
{
	return GetVehicleList(VEHICLE_TYPE_BOARD);
}

lua_tinker::table PointShopClient::GetVehicleBikeList()
{
	return GetVehicleList(VEHICLE_TYPE_BIKE);
}

lua_tinker::table PointShopClient::GetVehicleOilList()
{
	std::vector<pointshop::SellItemClient> vecSellItemList;
	SetSellVehicleList(vecSellItemList);

	lua_tinker::table tbItems( GLWidgetScript::GetInstance().GetLuaState() );

	int j = 0;
	for (size_t i=0; i < vecSellItemList.size(); ++i)
	{
		if( vecSellItemList[i].m_Type == ITEM_VEHICLE_OIL )
		{
			tbItems.set( j + 1, vecSellItemList[i].ItemId().Id() );
			j++;
		}
	}	

	return tbItems;
}

lua_tinker::table PointShopClient::GetVehiclePartsList() 
{
	std::vector<pointshop::SellItemClient> vecSellItemList;
	SetSellVehicleList(vecSellItemList);

	lua_tinker::table tbItems( GLWidgetScript::GetInstance().GetLuaState() );

	int j = 0;
	for (size_t i=0; i < vecSellItemList.size(); ++i)
	{
		if( vecSellItemList[i].m_Type == EMITEM_TYPE::ITEM_SUIT )
		{
			if( vecSellItemList[i].m_Suit >= SUIT_VEHICLE_PARTS_A && 
				vecSellItemList[i].m_Suit <= SUIT_VEHICLE_PARTS_F )
			{
				tbItems.set( j + 1, vecSellItemList[i].ItemId().Id() );
				j++;
			}
		}
	}

	return tbItems;
}

//카테고리 항목에 포함된 타입을 반환
void PointShopClient::CategoryToType( OUT std::vector<EMITEM_TYPE> &vecTYPE, const int nCategory )
{
	//카테고리 항목 밖
	if( nCategory > m_UiOption.size() )
	{
		return;
	}
	//스크립트로 관리 되지 않는 카테고리 항목이지만 아이템 목록에 존재하는 기타 항목(예외 항목)
	else if( nCategory == m_UiOption.size() ) 
	{
		bool bTypeInCategory[ ITEM_NSIZE ] = { false, };
		for(int i = 0; i < m_UiOption.size(); ++i)
		{
			for( int j = 0; j < m_UiOption[i].m_TypeVec.size(); ++j )
			{
				int nTYPE = m_UiOption[i].m_TypeVec[j];
				if ( ITEM_NSIZE > nTYPE )
					bTypeInCategory[nTYPE] = true;
			}
		}
		

		for( int i = 0; i < ITEM_NSIZE; ++i )
		{
			if( !bTypeInCategory[i] )
				vecTYPE.push_back( static_cast<EMITEM_TYPE>(i) );
		}

		return;
	}

	//스크립트로 관리되는 카테고리 항목
	for(int i = 0; i < m_UiOption[nCategory].m_TypeVec.size(); ++i)
		vecTYPE.push_back(m_UiOption[nCategory].m_TypeVec[i]);

}

lua_tinker::table PointShopClient::GetItemList( const char *szSearchName,
												lua_tinker::table tbCartegoryList ,
												lua_tinker::table tbOrderList)
{
	
	std::string SearchName(szSearchName);
	std::vector<EMITEM_TYPE> TypeList;
	std::vector<pointshop::SEARCH_ORDER> OrderList;

	//루아 테이블 데이터를 사용가능 하도록 변환
	for( int i = 1; i <= tbCartegoryList.TableLen(); ++i )
	{
		lua_tinker::table tbCategory= tbCartegoryList.get< lua_tinker::table >( i );
		
		int nCategory = tbCategory.get< EMITEM_TYPE >( "nCategory" );

		CategoryToType(TypeList, nCategory);
	}

	for( int i = 1; i <= tbOrderList.TableLen(); ++i )
	{
		lua_tinker::table tbOrder= tbOrderList.get< lua_tinker::table >( i );
		
		int nOrder = tbOrder.get<int>( "nOrder" );
		
		switch ( nOrder )
		{
		case 0:
			{
				pointshop::SEARCH_ORDER sOrder( pointshop::ITEM_PRICE, pointshop::SQL_DESC );
				OrderList.push_back( sOrder );
			}
			break;

		case 1:
			{
				pointshop::SEARCH_ORDER sOrder( pointshop::ITEM_PRICE, pointshop::SQL_ASC );
				OrderList.push_back( sOrder );
			}
			break;

		default:
			{
				pointshop::SEARCH_ORDER sOrder( pointshop::ITEM_NAME, pointshop::SQL_ASC );
				OrderList.push_back( sOrder );
			}
			break;
		}
	}
	
	m_vecSellItemList.clear();
	GetList( m_vecSellItemList, SearchName, TypeList, OrderList );

	//중복 타입 확인
	std::vector<EMITEM_TYPE> vecOverlapType;
	GetOverlapType(vecOverlapType);

	//결과 입력
	int nTableCount = 1;
	lua_tinker::table tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	std::vector<pointshop::SellItemClient>::iterator iter = m_vecSellItemList.begin();
	
	for( ; iter != m_vecSellItemList.end(); ++iter)
	{	
		SITEMCUSTOM sItem( iter->m_ItemId );

		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( "wMID", iter->m_ItemId.Mid() );
		tbItem.set( "wSID", iter->m_ItemId.Sid() );
		tbItem.set( "TYPE", iter->m_Type );
		tbItem.set( "nCategory", TypeToCategory(iter->m_Type, vecOverlapType) );
		tbItem.set( "SUIT", iter->m_Suit );
		tbItem.set( "strName", iter->m_ItemName );
		tbItem.set( "nPrice", iter->m_Price);
		tbItem.set( "nCount", sItem.wTurnNum );

		tbItemList.set( nTableCount++, tbItem );
	}

	return tbItemList;
}

// 카트 목록 입력에 따른 에러 메시지 -1, 0, 1
const int PointShopClient::AddCartList( const SITEMCUSTOM &sItemCustom )
{
	//카트 목록은 10개를 넘지 못한다
	if( MAX_CARTLIST <= m_vecCartList.size() ) 
		return 0;
		
	__int64 nItemPrice = ItemPrice(sItemCustom.GetNativeID());
	
	//가격이 0인 경우는 판매하지 않는 아이템
	if( 0 == nItemPrice) 
		return -1;

	m_vecCartList.push_back( SellItem( sItemCustom.GetNativeID(), nItemPrice) );

	return 1;
}

void PointShopClient::AddCartListScaleform( const int nIndex )
{
	if( (0 > nIndex) || ( nIndex > m_vecSellItemList.size() ) )
		return;
	
	SITEMCUSTOM sCustomItem(m_vecSellItemList[nIndex].ItemId());
	int nResult = AddCartList(sCustomItem);
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PointShop_UpdateCart, "-n", nResult);
}

bool PointShopClient::DelCartItem( const SITEMCUSTOM &sItemCustom , const int nIndex)
{
	if( (nIndex < 0) || (nIndex >= m_vecCartList.size()) )
		return false;

	std::vector<pointshop::SellItem>::iterator iter = m_vecCartList.begin() + nIndex;
	
	if( iter->m_ItemId == sItemCustom.GetNativeID() )
		m_vecCartList.erase(iter);

	return true;
}

void PointShopClient::DelCartList()
{
	m_vecCartList.clear();
}

void PointShopClient::BuyCartList()
{
	lua_tinker::table tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	std::vector<pointshop::SellItem>::iterator iter = m_vecCartList.begin();

	for( ; iter != m_vecCartList.end(); ++iter)
		m_pGaeaClient->PointShopBuyItem( iter->m_ItemId );
}

lua_tinker::table PointShopClient::GetCartList()
{
	int nTableCount = 1;
	lua_tinker::table tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	std::vector<pointshop::SellItem>::iterator iter = m_vecCartList.begin();

	for( ; iter != m_vecCartList.end(); ++iter)
	{	
		const SITEM *sItem = GLogicData::GetInstance().GetItem( iter->m_ItemId );
		if( NULL == sItem) continue;

		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( "wMID", iter->m_ItemId.Mid() );
		tbItem.set( "wSID", iter->m_ItemId.Sid() );
		tbItem.set( "strName", sItem->GetName() );
		tbItem.set( "nPrice", iter->m_Price);

		tbItemList.set( nTableCount++, tbItem );

	}

	return tbItemList;
}

//카테고리(m_UiOption) 인덱스를 받아서 해당 카테고리 아이템을 반환  
lua_tinker::table PointShopClient::GetCategoryItem( int nCategoryIndex )
{
	
	 // 스크립트상 관리되는 목록 외 기타 항목 1개가 존재함
	if( static_cast<unsigned int>(nCategoryIndex) > (m_UiOption.size() + 1) )
	{
		lua_tinker::table tbTemp( GLWidgetScript::GetInstance().GetLuaState() );
		return tbTemp;
	}

	std::vector<pointshop::SellItemClient>vecSellItemList;
	std::string SearchName("");
	std::vector<EMITEM_TYPE> TypeList;
	std::vector<pointshop::SEARCH_ORDER> OrderList;
	
	CategoryToType(TypeList, nCategoryIndex);
	GetList( vecSellItemList, SearchName, TypeList, OrderList );
	
	//결과 입력
	int nTableCount = 1;
	lua_tinker::table tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	std::vector<pointshop::SellItemClient>::iterator iter = vecSellItemList.begin();
	
	for( ; iter != vecSellItemList.end(); ++iter)
	{	
		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );

		tbItem.set( "wMID", iter->m_ItemId.Mid() );
		tbItem.set( "wSID", iter->m_ItemId.Sid() );
		tbItem.set( "TYPE", iter->m_Type );
		tbItem.set( "SUIT", iter->m_Suit );
		tbItem.set( "strName", iter->m_ItemName );
		tbItem.set( "nPrice", iter->m_Price);
		
		tbItemList.set( nTableCount++, tbItem );
	}

	return tbItemList;
}

/*중복 적용되는 타입 반환*/
void PointShopClient::GetOverlapType( OUT std::vector<EMITEM_TYPE> &vecTYPE )
{
	BYTE bTypeInCategory[ ITEM_NSIZE ] = { 0, };
	std::vector<UI_OPTION>::iterator iter = m_UiOption.begin();
	for( ; iter != m_UiOption.end(); ++iter)
	{
		for( size_t i = 0; i < iter->m_TypeVec.size(); ++i)
		{
			if( ITEM_NSIZE > iter->m_TypeVec[i] )
				bTypeInCategory[iter->m_TypeVec[i]]++;
		}
	}
	
	//중복 출력되는 타입 입력
	for( int i = 0; i < ITEM_NSIZE; ++i )
	{
		if( 1 < bTypeInCategory[i] )
			vecTYPE.push_back( static_cast<EMITEM_TYPE>(i) );
	}

}
/*
아이템 타입에 대한 카테고리 인덱스 반환
*/
int PointShopClient::TypeToCategory( const EMITEM_TYPE TYPE, std::vector<EMITEM_TYPE> &vecOverlapType)
{
	
	//다른 카테고리에 포함된 타입이라면 0을 반환한다
	std::vector<EMITEM_TYPE>::iterator iterType = vecOverlapType.begin();
	for( ; iterType != vecOverlapType.end(); ++iterType)
	{
		if( TYPE == *iterType ) return 0;
	}

	//결과 입력
	int nCategoryCount = 1;		//Lua 1번 부터 실제 카테고리 명칭이 시작된다 펫, 복장 등등
	
	std::vector<UI_OPTION>::iterator iter = m_UiOption.begin();
	for( ; iter != m_UiOption.end(); ++iter)
	{

		bool	bIsCategory( false );
		for( size_t i = 0; i < iter->m_TypeVec.size(); ++i)
		{
			if( TYPE == iter->m_TypeVec[i] ) bIsCategory = true;
		}
		
		if( bIsCategory ) 
			return nCategoryCount;
		else
			++nCategoryCount;

	}
	
	return nCategoryCount;	//카테고리 외 아이템(m_UiOption 변수에는 기타 항목이 포함되지 않습니다)
}

lua_tinker::table PointShopClient::GetCategoryList( )
{
	lua_tinker::table tbCategoryList( GLWidgetScript::GetInstance().GetLuaState() );
	std::vector<UI_OPTION>::iterator iter = m_UiOption.begin();
	
	//결과 입력
	int nTableCount = 1;
	for( ; iter != m_UiOption.end(); ++iter)
	{
		tbCategoryList.set( nTableCount++, iter->m_OptionName );
	}

	return tbCategoryList;
}

} // namespace pointshop