#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "./GLSaleItem.h"

#include "../../SigmaCore/DebugInclude.h"

namespace private_market
{
	GLSaleItem::GLSaleItem()
	{

	}

	GLSaleItem::~GLSaleItem()
	{

	}

	bool GLSaleItem::InsertSaleItem( UUID& sItemGUID, const SITEMCUSTOM& sItemcustom, DWORD ChaNum, bool ConsignmentSale, int ServerGroup )
	{
		std::string strGUID = sc::string::uuidToString( sItemGUID );
		return InsertSaleItem( strGUID, sItemcustom, ChaNum, ConsignmentSale, ServerGroup );
	}

	bool GLSaleItem::InsertSaleItem( const std::string& strGUID, const SITEMCUSTOM& sItemcustom, DWORD ChaNum, bool ConsignmentSale, int ServerGroup )
	{
		bool bReturn = false;	
		spItemcustom spItem = FindSaleItem( strGUID );
		if ( NULL == spItem )
		{
			/*sc::writeLogInfo( sc::string::format( "GLSaleItem::InsertSaleItem nomal %1%", strGUID.c_str() ) );*/

			spItem = spItemcustom( new SITEMCUSTOM( sItemcustom ) );
			m_theContainer.insert( SALE_ITEM_VALUE( strGUID, spItem ) );

			CHAINFO chainfo( ChaNum, ServerGroup );
			m_ForSearch.insert( CHA_SALE_ITEM_VALUE( chainfo, strGUID ) );
			
			if ( ConsignmentSale )
			{
				/*sc::writeLogInfo( sc::string::format( "GLSaleItem::InsertSaleItem ConsignmentSale %1%", strGUID.c_str() ) );*/
				m_ConsignmentSale.insert( SALE_ITEM_VALUE( strGUID, spItem ) );
			}

			bReturn = true;
		}
		return bReturn;
	}

	bool GLSaleItem::DeleteSaleItem( UUID& sItemGUID, DWORD ChaNum, int ServerGroup )
	{
		std::string strGUID = sc::string::uuidToString( sItemGUID );
		return DeleteSaleItem( strGUID, ChaNum, ServerGroup );
	}

	bool GLSaleItem::DeleteSaleItem( const std::string& strGUID, DWORD ChaNum, int ServerGroup )
	{
		bool bReturn = false;
		SALE_ITEM_ITER iterFind = m_theContainer.find( strGUID );
		if ( iterFind != m_theContainer.end() )
		{
			/*sc::writeLogInfo( sc::string::format( "GLSaleItem::DeleteSaleItem nomal %1%", strGUID.c_str() ) );*/
			m_theContainer.erase( iterFind );
			bReturn = true;
		}

		iterFind = m_ConsignmentSale.find( strGUID );
		if ( iterFind != m_ConsignmentSale.end() )
		{
			/*sc::writeLogInfo( sc::string::format( "GLSaleItem::DeleteSaleItem ConsignmentSale %1%", strGUID.c_str() ) );*/
			m_ConsignmentSale.erase( iterFind );
			bReturn = true;
		}

		CHAINFO chainfo( ChaNum, ServerGroup );

		std::pair< CHA_SALE_ITEM_ITER, CHA_SALE_ITEM_ITER > range = m_ForSearch.equal_range( chainfo );
		CHA_SALE_ITEM_ITER it;
		for ( it = range.first; it != range.second; )
		{
			if ( strGUID == it->second )
			{
				/*sc::writeLogInfo( sc::string::format( "GLSaleItem::DeleteSaleItem equal_range %1%", strGUID.c_str() ) );*/
				it = m_ForSearch.erase( it );
			}
			else
			{
				it++;
			}
		}

		return bReturn;
	}

	void GLSaleItem::DeleteSaleItem( DWORD ChaNum, int ServerGroup )
	{
		// 캐릭터로 삭제할 때는 m_ConsignmentSale 에서는 삭제하지 않는다. 없다고 추가하지 말것
		CHAINFO chainfo( ChaNum, ServerGroup );
		std::pair< CHA_SALE_ITEM_ITER, CHA_SALE_ITEM_ITER > range = m_ForSearch.equal_range( chainfo );
		CHA_SALE_ITEM_ITER it;
		for ( it = range.first; it != range.second; )
		{
			std::string& strGUID = it->second;
			SALE_ITEM_ITER iterFind = m_theContainer.find( strGUID );
			if ( iterFind != m_theContainer.end() )
			{
				/*sc::writeLogInfo( sc::string::format( "GLSaleItem::DeleteSaleItem ChaNum nomal %1%", strGUID.c_str() ) );*/
				m_theContainer.erase( iterFind );
			}

			/*sc::writeLogInfo( sc::string::format( "GLSaleItem::DeleteSaleItem ChaNum equal_range %1%", strGUID.c_str() ) );*/
			it = m_ForSearch.erase( it );
		}
	}

	void GLSaleItem::DeleteSaleItemAll()
	{
		m_theContainer.clear();
		m_ConsignmentSale.clear();
		m_ForSearch.clear();
	}

	spItemcustom GLSaleItem::FindSaleItem( UUID& sItemGUID )
	{
		std::string strGUID = sc::string::uuidToString( sItemGUID );
		return FindSaleItem( strGUID );
	}

	spItemcustom GLSaleItem::FindSaleItem( const std::string& strGUID )
	{
		SALE_ITEM_ITER iterFind = m_theContainer.find( strGUID );
		if ( iterFind != m_theContainer.end() )
		{
			return iterFind->second;
		}
		else
		{
			iterFind = m_ConsignmentSale.find( strGUID );
			if ( iterFind != m_ConsignmentSale.end() )
			{
				return iterFind->second;
			}

			static spItemcustom spDummy;
			return spDummy;
		}
	}

}

