#include "stdafx.h"

#include "./CDataManagerClubInven.h"

#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace dm
{
	CDataManagerClubInven::CDataManagerClubInven()
	: m_pDBMan( NULL )
	, m_bStorage( false )
	, m_nChaNum( 0 )
	, m_clubInvenMoney( 0 )
	, m_GuNum( 0 )
	{
	}

	CDataManagerClubInven::~CDataManagerClubInven() { }

	void CDataManagerClubInven::Initialize()
	{
		if( m_pDBMan->GetClubInven( m_nChaNum ) == DB_ERROR )
			return;
	}

	void CDataManagerClubInven::SetValidStorage( bool bStorage )
	{
		m_bStorage = bStorage;
	}

	bool CDataManagerClubInven::SETSTORAGE_BYBUF( se::ByteStream& ByteStream )
	{
		if( ByteStream.IsEmpty() )
			return true;

		DWORD dwChannel = 0;
		ByteStream >> dwChannel;
		if( dwChannel > MAX_CLUBSTORAGE )
			dwChannel = MAX_CLUBSTORAGE;

		for( DWORD i=0; i<dwChannel; ++i )
		{
			BOOL bOk = m_cStorage[i].SETITEM_BYBUFFER( ByteStream );
			if (!bOk)
				return true;
		}

		return true;
	}

	bool CDataManagerClubInven::GETSTORAGE_BYBUF( se::ByteStream& ByteStream ) const
	{
		ByteStream.ClearBuffer();
		ByteStream << DWORD( MAX_CLUBSTORAGE );
		for( DWORD i=0; i<MAX_CLUBSTORAGE; ++i )
			m_cStorage[ i ].GETITEM_BYBUFFER_FORSTORAGE( ByteStream );

		return true;
	}

	bool CDataManagerClubInven::SETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems )
	{
		size_t ItemSize = vecItems.size();
		for ( size_t loop = 0; loop < ItemSize; loop++ )
		{
			SINVENITEM_SAVE& sItem = vecItems[loop];

			if ( sItem.wPosX >= 0 && sItem.wPosX < EStorageX )
			{
				m_cStorage[0].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
			}
			else if ( sItem.wPosX >= EStorageX && sItem.wPosX < EStorageX*2 )
			{
				sItem.wPosX = sItem.wPosX - EStorageX;
				m_cStorage[1].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
			}
			else if ( sItem.wPosX >= EStorageX*2 && sItem.wPosX < EStorageX*3 )
			{
				sItem.wPosX = sItem.wPosX - ( EStorageX*2 );
				m_cStorage[2].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
			}
			else if ( sItem.wPosX >= EStorageX*3 && sItem.wPosX < EStorageX*4 )
			{
				sItem.wPosX = sItem.wPosX - ( EStorageX*3 );
				m_cStorage[3].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
			}
			else if ( sItem.wPosX >= EStorageX*4 && sItem.wPosX < EStorageX*5 )
			{
				sItem.wPosX = sItem.wPosX - ( EStorageX*4 );
				m_cStorage[4].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
			}
			else if ( sItem.wPosX >= EStorageX*5 && sItem.wPosX < EStorageX*6 )
			{
				sItem.wPosX = sItem.wPosX - ( EStorageX*5 );
				m_cStorage[5].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
			}
		}

		return true;
	}

	bool CDataManagerClubInven::GETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems )
	{
		for ( size_t x = 0; x < MAX_CLUBSTORAGE; x++ )
		{
			std::vector< SINVENITEM_SAVE > vecItemsTemp;
			GLInventoryStorage& Inven = m_cStorage[x];
			Inven.GET_ITEM_BY_VECTOR( vecItemsTemp );

			size_t ItemSize = vecItemsTemp.size();
			for ( size_t loop = 0; loop < ItemSize; loop++ )
			{
				SINVENITEM_SAVE& sItem = vecItemsTemp[loop];
				if ( 1 == x )
				{
					sItem.wPosX = sItem.wPosX + EStorageX;
				}
				else if ( 2 == x )
				{
					sItem.wPosX = sItem.wPosX + ( EStorageX*2 );
				}
				else if ( 3 == x )
				{
					sItem.wPosX = sItem.wPosX + ( EStorageX*3 );
				}
				else if ( 4 == x )
				{
					sItem.wPosX = sItem.wPosX + ( EStorageX*4 );
				}
				else if ( 5 == x )
				{
					sItem.wPosX = sItem.wPosX + ( EStorageX*5 );
				}
				vecItems.push_back( vecItemsTemp[loop] );
			}
		}

		return true;
	}

	bool CDataManagerClubInven::DeleteItem( unsigned int nChannel, WORD wX, WORD wY )
	{
		if ( nChannel >= 0 && nChannel < MAX_CLUBSTORAGE )
		{
			SINVENITEM* pInvenItem = m_cStorage[nChannel].GetItem( wX, wY );
			if ( pInvenItem )
			{
				m_pDBMan->ItemDelete( pInvenItem->GetGuid(), pInvenItem->GetDbState(), 2999 );
				if ( m_cStorage[ nChannel ].DeleteItem( wX, wY ) )
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		else
			return false;

		return true;
	}

	void CDataManagerClubInven::SaveClubInvenItemToDB()
	{
		// 창고 인벤 저장;
		std::vector< SINVENITEM_SAVE > vecItems;
		bool bOK = GETSTORAGE_BYVECTOR( vecItems );
		if( bOK )
		{
			if( m_pDBMan->SaveClubInven( m_nChaNum, m_GuNum, vecItems )
				== DB_ERROR )
				return;
		}

		// Money Db에 저장.
		/*m_pServer->AddGameDbJob(
			db::DbActionPtr(
			new db::ClubMoneySet(m_DbNum, GetStorageMoney())));

		m_pServer->AddGameDbJob(
			db::DbActionPtr(
			new db::ClubIncomeMoneySet(m_DbNum, GetIncomeMoney())));*/

		//sc::writeLogInfo(sc::string::format("SaveStorageToDB ClubDbNum %1%", m_DbNum));
		//m_bStorageUpdate = false;
	}

	void CDataManagerClubInven::Clear()
	{
		for (int i=0; i<MAX_CLUBSTORAGE; ++i)
			m_cStorage[i].DeleteItemAll();
	}

}