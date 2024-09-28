#include "../pch.h"
#include "../GLogicDataMan.h"
//#include "./GLQuest.h"
#include "./GLQuestPlay.h"
#include "./GLQuestMan.h"

//#include "ByteStream.h"
//#include "StlFunctions.h"
//#include <algorithm>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLQuestPlay::GLQuestPlay ()
{
}

GLQuestPlay::~GLQuestPlay ()
{
	DeleteAll ();
}

bool GLQuestPlay::GetReqREADING ( DWORD &dwNID )
{
	MAPQUEST_ITER iter = m_mapQuestProc.begin();
	MAPQUEST_ITER iter_end = m_mapQuestProc.end();
	for ( ; iter!=iter_end; ++iter )
	{
		GLQUESTPROG* pPROG = (*iter).second;

		if ( pPROG->IsReqREADING() )
		{
			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
			if ( pQUEST )
			{
				dwNID = pPROG->m_sNID.dwID;
				return true;
			}
		}
	}

	return false;
}

void GLQuestPlay::ResetReqREADING ( DWORD dwNID )
{
	GLQUESTPROG *pPROG = FindProc ( dwNID );
	if ( pPROG )
	{
		pPROG->ResetReqREADING();
	}

	return;
}

GLQUESTPROG* GLQuestPlay::FindProc ( DWORD dwQuestID )
{
	MAPQUEST_ITER iter = m_mapQuestProc.find ( dwQuestID );
	if ( iter!=m_mapQuestProc.end() )	
	{
		return (*iter).second;
	}
	else
	{
		return NULL;
	}
}

GLQUESTPROG* GLQuestPlay::FindEnd ( DWORD dwQuestID )
{
	MAPQUEST_ITER iter = m_mapQuestEnd.find ( dwQuestID );
	if ( iter!=m_mapQuestEnd.end() )
	{
		return (*iter).second;
	}
	else
	{
		return NULL;
	}
}
	
bool GLQuestPlay::InsertProc(const GLQUESTPROG& sQuestProg, bool bCheck)
{
    // data 가 삭제된 퀘스트 처리
    if (bCheck && !GLogicData::GetInstance().IsExistQuest(sQuestProg.m_sNID.Id()))
        return false;

	GLQUESTPROG* pQuestProg = FindProc(sQuestProg.m_sNID.dwID);
	if (pQuestProg)
	{
		*pQuestProg = sQuestProg;
		return false;
	}
	else
	{

		//pQuestProg = new GLQUESTPROG;
		pQuestProg = GLQuestMan::GetInstance().m_poolGLQuestProg.construct();
		*pQuestProg = sQuestProg;

		std::pair<MAPQUEST_ITER, bool> Success;
		Success = m_mapQuestProc.insert(std::make_pair(sQuestProg.m_sNID.dwID,pQuestProg));
		if ( Success.second == true )
		{
			InsertMobProc(sQuestProg);
			InsertItemUseProc(sQuestProg);
			return true;
		}
		else
		{
			SAFE_DELETE( pQuestProg );
			return false;
		}
	}
}

bool GLQuestPlay::InsertProc( 
							 const GLQUESTPROG& sQuestProg, 
							 bool bCheck, 
							 std::multimap< DWORD, SINVENITEM_SAVE >& mapItems, 
							 std::vector< sc::MGUID >& vecDeleteItems )
{
	// data 가 삭제된 퀘스트 처리
	if ( bCheck && !GLogicData::GetInstance().IsExistQuest( sQuestProg.m_sNID.Id() ) )
	{
		std::pair< std::multimap< DWORD, SINVENITEM_SAVE >::iterator, std::multimap< DWORD, SINVENITEM_SAVE >::iterator > range = mapItems.equal_range( sQuestProg.m_sNID.dwID );
		std::multimap< DWORD, SINVENITEM_SAVE >::iterator it;
		for ( it = range.first; it != range.second; ++it )
		{
			SINVENITEM_SAVE& sItem = it->second;
			vecDeleteItems.push_back( sItem.sItemCustom.guid );
		}
		return false;
	}

	GLQUESTPROG* pQuestProg = FindProc( sQuestProg.m_sNID.dwID );
	if ( pQuestProg )
	{
		*pQuestProg = sQuestProg;
		std::pair< std::multimap< DWORD, SINVENITEM_SAVE >::iterator, std::multimap< DWORD, SINVENITEM_SAVE >::iterator > range = mapItems.equal_range( sQuestProg.m_sNID.dwID );
		std::multimap< DWORD, SINVENITEM_SAVE >::iterator it;
		for ( it = range.first; it != range.second; ++it )
		{
			SINVENITEM_SAVE& sItem = it->second;
			pQuestProg->m_sINVENTORY.InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}

		return false;
	}
	else
	{
		pQuestProg = GLQuestMan::GetInstance().m_poolGLQuestProg.construct();
		*pQuestProg = sQuestProg;

		std::pair< std::multimap< DWORD, SINVENITEM_SAVE >::iterator, std::multimap< DWORD, SINVENITEM_SAVE >::iterator > range = mapItems.equal_range( sQuestProg.m_sNID.dwID );
		std::multimap< DWORD, SINVENITEM_SAVE >::iterator it;
		for ( it = range.first; it != range.second; ++it )
		{
			SINVENITEM_SAVE& sItem = it->second;
			pQuestProg->m_sINVENTORY.InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}

		std::pair< MAPQUEST_ITER, bool > Success;
		Success = m_mapQuestProc.insert( std::make_pair( sQuestProg.m_sNID.dwID, pQuestProg ) );
		if ( Success.second == true )
		{
			InsertMobProc(sQuestProg);
			InsertItemUseProc(sQuestProg);
			return true;
		}
		else
		{
			GLQuestMan::GetInstance().m_poolGLQuestProg.destroy( pQuestProg );
			return false;
		}
	}
}

bool GLQuestPlay::InsertProcForMigration( const GLQUESTPROG& sQuestProg, bool bCheck )
{
	return false;

	// data 가 삭제된 퀘스트 처리
	if ( bCheck && !GLogicData::GetInstance().IsExistQuest( sQuestProg.m_sNID.Id() ) )
		return false;

	GLQUESTPROG* pQuestProg = FindProc( sQuestProg.m_sNID.dwID );
	if ( pQuestProg )
	{
		*pQuestProg = sQuestProg;
		return false;
	}
	else
	{
		//pQuestProg = new GLQUESTPROG;
		pQuestProg = GLQuestMan::GetInstance().m_poolGLQuestProg.construct();
		*pQuestProg = sQuestProg;

		std::pair< MAPQUEST_ITER, bool > Success;
		Success = m_mapQuestProc.insert( std::make_pair( sQuestProg.m_sNID.dwID, pQuestProg ) );
		InsertMobProc( sQuestProg );
		InsertItemUseProc( sQuestProg );
		return Success.second;
	}
}

bool GLQuestPlay::DeleteProc ( DWORD dwQuestID )
{	
	DeleteMobProc( dwQuestID );
	DeleteItemUseProc(dwQuestID);
	//DeleteItemGrindingProc(dwQuestID);
	MAPQUEST_ITER iter = m_mapQuestProc.find ( dwQuestID );
	if ( iter!=m_mapQuestProc.end() )
	{
		//SAFE_DELETE ( (*iter).second );
		GLQuestMan::GetInstance().m_poolGLQuestProg.destroy( (*iter).second );
		m_mapQuestProc.erase ( iter );
		return true;
	}
	else
	{
		return false;
	}
}

void GLQuestPlay::InsertMobProc( const GLQUESTPROG &sQuestProg )
{
	GLQUESTPROG* pPROG = NULL;
	GLQUEST* pQUEST = NULL;
	GLQUEST_STEP* pSTEP = NULL;

	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();
	
	pPROG = FindProc ( sQuestProg.m_sNID.dwID );
	if( pPROG )
	{
		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if( !pQUEST ) return;
		for( INT i = 0; i < ( INT ) pQUEST->m_vecProgStep.size(); i++ )
		{
			pSTEP = &( pQUEST->m_vecProgStep[ i ] );
			if ( !pSTEP )								return;

			if( pSTEP->IsNEED_MOBKILL() )
			{
				DWORD dwMobID = pSTEP->m_dwNID_MOBKILL;
				DWORD dwQuestID = pQUEST->m_sNID.dwID;

				m_mapQuestMob.insert( std::make_pair( dwMobID, dwQuestID ) );
			}

			if( pSTEP->IsNEED_QITEM() )
			{
				GENMOBITEMARRAY_ITER iter = pSTEP->m_vecMOBGEN_QITEM.begin();
				GENMOBITEMARRAY_ITER iter_end = pSTEP->m_vecMOBGEN_QITEM.end();
				for ( ; iter!=iter_end; ++iter )
				{
					SGENQUESTITEM &sGENQITEM = (*iter);	

					for( INT i = 0; i < ( INT ) sGENQITEM.vecGEN_MOB.size(); i++ )
					{
						DWORD dwMobID = sGENQITEM.vecGEN_MOB[ i ];
						DWORD dwQuestID = pQUEST->m_sNID.dwID;

						m_mapQuestMob.insert( std::make_pair( dwMobID, dwQuestID ) );
					}
				}	
			}
		}
	}
}

void GLQuestPlay::DeleteMobProc( DWORD dwQuestID )
{
	QUESTMOB_MAP_ITER iter = m_mapQuestMob.begin();
	QUESTMOB_MAP_ITER iter_end = m_mapQuestMob.end();

	for( ; iter != iter_end ; )
	{
		if( iter->second == dwQuestID )
		{
			m_mapQuestMob.erase ( iter++ );
		}
		else
		{
			++iter;
		}
	}
}

void GLQuestPlay::InsertItemUseProc( const GLQUESTPROG &sQuestProg )
{
	GLQUESTPROG* pPROG = NULL;
	GLQUEST* pQUEST = NULL;
	GLQUEST_STEP* pSTEP = NULL;

	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();

	pPROG = FindProc ( sQuestProg.m_sNID.dwID );
	if( pPROG )
	{
		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
		if( !pQUEST ) return;
		for( INT i = 0; i < ( INT ) pQUEST->m_vecProgStep.size(); i++ )
		{
			pSTEP = &( pQUEST->m_vecProgStep[ i ] );
			if ( !pSTEP )								return;

			if( pSTEP->IsNEED_ITEMUSE() )
			{
				DWORD dwItemUseID = pSTEP->m_dwNID_ITEMUSE;
				DWORD dwQuestID = pQUEST->m_sNID.dwID;

				m_mapQuestItemUse.insert( std::make_pair( dwItemUseID, dwQuestID ) );
			}
		}
	}
}

void GLQuestPlay::DeleteItemUseProc( DWORD dwQuestID )
{
	QUESTITEMUSE_MAP_ITER iter = m_mapQuestItemUse.begin();
	QUESTITEMUSE_MAP_ITER iter_end = m_mapQuestItemUse.end();

	for( ; iter != iter_end ; )
	{
		if( iter->second == dwQuestID )
		{
			m_mapQuestItemUse.erase ( iter++ );
		}
		else
		{
			++iter;
		}
	}
}

//void GLQuestPlay::InsertItemGrindingProc( const GLQUESTPROG &sQuestProg )
//{
//	GLQUESTPROG* pPROG = NULL;
//	GLQUEST* pQUEST = NULL;
//	GLQUEST_STEP* pSTEP = NULL;
//
//	GLQuestMan & glQuestMan = GLQuestMan::GetInstance();
//
//	pPROG = FindProc ( sQuestProg.m_sNID.dwID );
//	if( pPROG )
//	{
//		pQUEST = glQuestMan.Find ( pPROG->m_sNID.dwID );
//		if( !pQUEST ) return;
//		for( INT i = 0; i < ( INT ) pQUEST->m_vecProgStep.size(); i++ )
//		{
//			pSTEP = &( pQUEST->m_vecProgStep[ i ] );
//			if ( !pSTEP )								return;
//
//			if( pSTEP->IsNEED_ITEMGRINDING() )
//			{
//				DWORD dwItemGrindingID = pSTEP->m_dwNID_ITEMGRINDING;
//				DWORD dwQuestID = pQUEST->m_sNID.dwID;
//
//				m_mapQuestItemGrinding.insert( std::make_pair( dwItemGrindingID, dwQuestID ) );
//			}
//		}
//	}
//}
//
//void GLQuestPlay::DeleteItemGrindingProc( DWORD dwQuestID )
//{
//	QUESTITEMGRINDING_MAP_ITER iter = m_mapQuestItemGrinding.begin();
//	QUESTITEMGRINDING_MAP_ITER iter_end = m_mapQuestItemGrinding.end();
//
//	for( ; iter != iter_end ; )
//	{
//		if( iter->second == dwQuestID )
//		{
//			m_mapQuestItemGrinding.erase ( iter++ );
//		}
//		else
//		{
//			++iter;
//		}
//	}
//}

bool GLQuestPlay::InsertEnd(const GLQUESTPROG& sQuestProg, bool bCheck)
{
    // data 가 삭제된 퀘스트 처리
    if (bCheck && !GLogicData::GetInstance().IsExistQuest(sQuestProg.m_sNID.Id()))
        return false;

	GLQUESTPROG* pQuestProg = FindEnd(sQuestProg.m_sNID.dwID);
	if (pQuestProg)
	{
		*pQuestProg = sQuestProg;
		return false;
	}
	else
	{
		std::pair< MAPQUEST_ITER, bool > Success;
		//pQuestProg = new GLQUESTPROG;
		pQuestProg = GLQuestMan::GetInstance().m_poolGLQuestProg.construct();
		*pQuestProg = sQuestProg;
		Success = m_mapQuestEnd.insert ( std::make_pair(sQuestProg.m_sNID.dwID, pQuestProg) );
		if (Success.second == true)
		{
			return true;
		}
		else
		{
			//SAFE_DELETE(pQuestProg);
			GLQuestMan::GetInstance().m_poolGLQuestProg.destroy(pQuestProg);
			return false;
		}
	}
}

bool GLQuestPlay::InsertEnd( 
							const GLQUESTPROG& sQuestProg, 
							bool bCheck, 
							std::multimap< DWORD, SINVENITEM_SAVE >& mapItems, 
							std::vector< sc::MGUID >& vecDeleteItems )
{
	// data 가 삭제된 퀘스트 처리
	if ( bCheck && !GLogicData::GetInstance().IsExistQuest( sQuestProg.m_sNID.Id() ) )
	{
		std::pair< std::multimap< DWORD, SINVENITEM_SAVE >::iterator, std::multimap< DWORD, SINVENITEM_SAVE >::iterator > range = mapItems.equal_range( sQuestProg.m_sNID.dwID );
		std::multimap< DWORD, SINVENITEM_SAVE >::iterator it;
		for ( it = range.first; it != range.second; ++it )
		{
			SINVENITEM_SAVE& sItem = it->second;
			vecDeleteItems.push_back( sItem.sItemCustom.guid );
		}
		return false;
	}

	GLQUESTPROG* pQuestProg = FindEnd( sQuestProg.m_sNID.dwID );
	if ( pQuestProg )
	{
		*pQuestProg = sQuestProg;

		std::pair< std::multimap< DWORD, SINVENITEM_SAVE >::iterator, std::multimap< DWORD, SINVENITEM_SAVE >::iterator > range = mapItems.equal_range( sQuestProg.m_sNID.dwID );
		std::multimap< DWORD, SINVENITEM_SAVE >::iterator it;
		for ( it = range.first; it != range.second; ++it )
		{
			SINVENITEM_SAVE& sItem = it->second;
			pQuestProg->m_sINVENTORY.InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}

		return false;
	}
	else
	{
		std::pair< MAPQUEST_ITER, bool > Success;
		pQuestProg = GLQuestMan::GetInstance().m_poolGLQuestProg.construct();
		*pQuestProg = sQuestProg;

		std::pair< std::multimap< DWORD, SINVENITEM_SAVE >::iterator, std::multimap< DWORD, SINVENITEM_SAVE >::iterator > range = mapItems.equal_range( sQuestProg.m_sNID.dwID );
		std::multimap< DWORD, SINVENITEM_SAVE >::iterator it;
		for ( it = range.first; it != range.second; ++it )
		{
			SINVENITEM_SAVE& sItem = it->second;
			pQuestProg->m_sINVENTORY.InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}

		Success = m_mapQuestEnd.insert ( std::make_pair( sQuestProg.m_sNID.dwID, pQuestProg ) );
		if ( Success.second == true )
		{
			return true;
		}
		else
		{
			GLQuestMan::GetInstance().m_poolGLQuestProg.destroy( pQuestProg );
			return false;
		}
	}
}

bool GLQuestPlay::DeleteEnd ( DWORD dwQuestID )
{
	MAPQUEST_ITER iter = m_mapQuestEnd.find ( dwQuestID );
	if ( iter!=m_mapQuestEnd.end() )
	{
		//SAFE_DELETE ( (*iter).second );
		GLQuestMan::GetInstance().m_poolGLQuestProg.destroy( (*iter).second );
		m_mapQuestEnd.erase ( iter );
		return true;
	}
	else
	{
		return false;
	}
}

void GLQuestPlay::DeleteAll ()
{
	//std::for_each ( m_mapQuestProc.begin(), m_mapQuestProc.end(), std_afunc::DeleteMapObject() );
	for ( MAPQUEST_ITER iter = m_mapQuestProc.begin(); iter!=m_mapQuestProc.end(); ++iter )
	{
		GLQuestMan::GetInstance().m_poolGLQuestProg.destroy( (*iter).second );
	}
	m_mapQuestProc.clear();

	//std::for_each ( m_mapQuestEnd.begin(), m_mapQuestEnd.end(), std_afunc::DeleteMapObject() );
	for ( MAPQUEST_ITER iter = m_mapQuestEnd.begin(); iter!=m_mapQuestEnd.end(); ++iter )
	{
		GLQuestMan::GetInstance().m_poolGLQuestProg.destroy( (*iter).second );
	}
	m_mapQuestEnd.clear();

	m_mapQuestMob.clear();
	m_mapQuestItemUse.clear();
}

GLQuestPlay& GLQuestPlay::operator= ( GLQuestPlay& value )
{
	DeleteAll ();

	{
		MAPQUEST_ITER iter = value.GetQuestProc().begin();
		MAPQUEST_ITER iter_end = value.GetQuestProc().end();
		for ( ; iter!=iter_end; ++iter )
		{
			InsertProc(*(*iter).second, false);
		}
	}

	{
		MAPQUEST_ITER iter = value.GetQuestEnd().begin();
		MAPQUEST_ITER iter_end = value.GetQuestEnd().end();
		for ( ; iter!=iter_end; ++iter )
		{
			InsertEnd(*(*iter).second, false);
		}
	}

	return *this;
}

bool GLQuestPlay::GET_BYBUFFER(se::ByteStream& ByteStream, bool bCheck)
{
	DeleteAll();
	if (ByteStream.IsEmpty())
        return true;

	DWORD dwVer(0);
	DWORD dwSize(0);

	ByteStream >> dwVer;

	ByteStream >> dwSize;
	for ( DWORD i=0 ; i<dwSize; ++i )
	{
		GLQUESTPROG sQuestProg;
		sQuestProg.GET_BYBUFFER(ByteStream);
		InsertProc(sQuestProg, bCheck);
	}

	ByteStream >> dwSize;
	for ( DWORD i=0 ; i<dwSize; ++i )
	{
		GLQUESTPROG sQuestEnd;
		sQuestEnd.GET_BYBUFFER(ByteStream);
		InsertEnd(sQuestEnd, bCheck);
	}

	return true;
}

bool GLQuestPlay::GET_BYBUFFER_FOR_MIGRATION( se::ByteStream& ByteStream, bool bCheck )
{
	DeleteAll();
	if ( ByteStream.IsEmpty() )
		return true;

	DWORD dwVer( 0 );
	DWORD dwSize( 0 );

	ByteStream >> dwVer;

	ByteStream >> dwSize;
	for ( DWORD i = 0 ; i < dwSize; ++i )
	{
		GLQUESTPROG sQuestProg;
		sQuestProg.GET_BYBUFFER( ByteStream );
		InsertProcForMigration( sQuestProg, bCheck );
	}

	ByteStream >> dwSize;
	for ( DWORD i = 0 ; i < dwSize; ++i )
	{
		GLQUESTPROG sQuestEnd;
		sQuestEnd.GET_BYBUFFER( ByteStream );
		InsertEnd( sQuestEnd, bCheck );
	}

	return true;
}

bool GLQuestPlay::GET_BYBUFFER_NEW( 
								   se::ByteStream& ByteStream, 
								   bool bCheck, 
								   std::multimap< DWORD, SINVENITEM_SAVE >& mapProcItems, 
								   std::multimap< DWORD, SINVENITEM_SAVE >& mapEndItems, 
								   std::vector< sc::MGUID >& vecDeleteItems )
{
	DeleteAll();
	if ( ByteStream.IsEmpty() )
		return true;

	DWORD dwVer( 0 );
	DWORD dwSize( 0 );

	ByteStream >> dwVer;

	ByteStream >> dwSize;
	for ( DWORD i = 0 ; i < dwSize; ++i )
	{
		GLQUESTPROG sQuestProg;
		sQuestProg.GET_BYBUFFER( ByteStream );
		InsertProc( sQuestProg, bCheck, mapProcItems, vecDeleteItems );
	}

	ByteStream >> dwSize;
	for ( DWORD i = 0 ; i < dwSize; ++i )
	{
		GLQUESTPROG sQuestEnd;
		sQuestEnd.GET_BYBUFFER( ByteStream );
		InsertEnd( sQuestEnd, bCheck, mapEndItems, vecDeleteItems );
	}

	return true;
}

BOOL GLQuestPlay::GET_ITEM_BY_VECTOR( std::vector< SINVENITEM_SAVE >& vecItem )
{/*
	{
		MAPQUEST_CITER iter = m_mapQuestProc.begin();
		MAPQUEST_CITER iter_end = m_mapQuestProc.end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG* pQuestProg = (*iter).second;
			if ( pQuestProg )
			{
				pQuestProg->m_sINVENTORY.GET_ITEM_BY_VECTOR( vecItem );
			}
		}
	}

	{
		MAPQUEST_CITER iter = m_mapQuestEnd.begin();
		MAPQUEST_CITER iter_end = m_mapQuestEnd.end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG* pQuestEnd = (*iter).second;
			pQuestEnd->SET_BYBUFFER ( ByteStream );
		}
	}*/
	return TRUE;
}

DWORD GLQuestPlay::GetDataSize()
{
	DWORD		dwDataSize = 0;

	dwDataSize += sizeof( DWORD );				// version

	dwDataSize += sizeof( DWORD );				// m_mapQuestProc.size()
	GLQUESTPROG*	pQuest;
	MAPQUEST_CITER	iter	 = m_mapQuestProc.begin();
	MAPQUEST_CITER	iter_end = m_mapQuestProc.end();
	for( ;iter != iter_end; iter++ )
	{
		pQuest = iter->second;
		dwDataSize += pQuest->GetDataSize();
	}

	dwDataSize += sizeof( DWORD );				// m_mapQuestEnd.size();
	iter	 = m_mapQuestEnd.begin();
	iter_end = m_mapQuestEnd.end();
	for( ;iter != iter_end; iter++ )
	{
		pQuest = iter->second;
		dwDataSize += pQuest->GetDataSize();
	}

	return dwDataSize;
}

/**
 * 함수 수정시 연관성 있는 GetItemDataSize도 수정필요!!!
 */
BOOL GLQuestPlay::GetData( se::SerializeWritableData& data )
{
	data << (DWORD)VERSION;

	GLQUESTPROG*	pQuest;
	MAPQUEST_CITER	iter	 = m_mapQuestProc.begin();
	MAPQUEST_CITER	iter_end = m_mapQuestProc.end();
	data << (DWORD)m_mapQuestProc.size();
	for( ;iter != iter_end; iter++ )
	{
		pQuest = iter->second;
		pQuest->GetData( data );
	}

	iter	 = m_mapQuestEnd.begin();
	iter_end = m_mapQuestEnd.end();
	data << (DWORD)m_mapQuestEnd.size();
	for( ;iter != iter_end; iter++ )
	{
		pQuest = iter->second;
		pQuest->GetData( data );
	}

	return TRUE;
}

/**
 * 함수 수정시 연관성 있는 GetDataSize도 수정필요!!!
 */
bool GLQuestPlay::SET_BYBUFFER(se::ByteStream &ByteStream) const
{
	ByteStream << (DWORD) VERSION;

	ByteStream << (DWORD) m_mapQuestProc.size();
	{
		MAPQUEST_CITER iter = m_mapQuestProc.begin();
		MAPQUEST_CITER iter_end = m_mapQuestProc.end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG* pQuestProg = (*iter).second;
			pQuestProg->SET_BYBUFFER ( ByteStream );
		}
	}

	ByteStream << (DWORD) m_mapQuestEnd.size();
	{
		MAPQUEST_CITER iter = m_mapQuestEnd.begin();
		MAPQUEST_CITER iter_end = m_mapQuestEnd.end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG* pQuestEnd = (*iter).second;
			pQuestEnd->SET_BYBUFFER ( ByteStream );
		}
	}

	return true;
}

HRESULT GLQuestPlay::FrameMove ( float fTime, float fElapsedTime )
{
	MAPQUEST_ITER iter = m_mapQuestProc.begin();
	MAPQUEST_ITER iter_end = m_mapQuestProc.end();
	for ( ; iter!=iter_end; ++iter )
	{
		GLQUESTPROG* pQuestProg = (*iter).second;

		pQuestProg->FrameMove ( fTime, fElapsedTime );
	}

	return S_OK;
}

