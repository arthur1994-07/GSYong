#include "pch.h"
#include "./s_COdbcManager.h"
#include "./OdbcStmt.h"
#include "../DBAction/DbAction.h"

#include "../../../SigmaCore/DebugInclude.h"

int COdbcManager::AddProductType( const DWORD dwChaNum, const WORD nProductType )
{
    sc::writeLogError("COdbcManager::AddProductType. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::GetProductType(const DWORD dwChaNum, std::vector<WORD>& vecProductType OUT)
{
    sc::writeLogError("COdbcManager::GetProductTypeAndPoint. Use ADO");
    return sc::db::DB_ERROR;
}


int COdbcManager::DelProductType( const DWORD dwChaNum, const WORD nProductID )
{
    sc::writeLogError("COdbcManager::DelProductType. Use ADO");
    return sc::db::DB_ERROR;
}


int COdbcManager::AddProductIsGetExp( const DWORD dwChaNum, const DWORD dwProductID )
{
    sc::writeLogError("COdbcManager::AddProductIsGetExp. Use ADO");
    return sc::db::DB_ERROR;
}


int COdbcManager::GetProductIsGetExp( const DWORD dwChaNum, std::vector<WORD>& vecProductID OUT)
{
    sc::writeLogError("COdbcManager::GetProductIsGetExp. Use ADO");
    return sc::db::DB_ERROR;
}


int COdbcManager::DelProductIsGetExp( const DWORD dwChaNum, const PRODUCT_ITEM& productItem )
{
    sc::writeLogError("COdbcManager::DelProductIsGetExp. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::SaveProductBook( DWORD nChaNum, const std::vector<DWORD>& setLearnProductBook )
{
    sc::writeLogError("COdbcManager::SaveProductBook. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::LoadProductBook( DWORD nChaNum, std::vector<DWORD>& setLearnProductBook OUT )
{
    sc::writeLogError("COdbcManager::LoadProductBook. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::DeleteProductBook( DWORD nChaNum, const std::vector<PRODUCT_ITEM>& vecBook )
{
    sc::writeLogError("COdbcManager::DeleteProductBook. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::CharProductLog( const CHAR_PRODUCT_LOG& productLog )
{
	sc::writeLogError("COdbcManager::CharProductLog. Use ADO");
	return sc::db::DB_ERROR;
}

int COdbcManager::ProductExchangeLog( const PRODUCT_EXCHANGE_LOG& productLog )
{
	sc::writeLogError("COdbcManager::ProductExchangeLog. Use ADO");
	return sc::db::DB_ERROR;
}