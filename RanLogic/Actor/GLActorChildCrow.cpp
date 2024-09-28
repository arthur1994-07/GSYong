#include "pch.h"
#include "GLActor.h"

void GLActor::InsertChildCrow( const Faction::GLActorID& sID )
{
	// 이미 등록 되었다면 등록하지 않는다;
	for( CROWIDLIST_ITER iter = m_listCrowID.begin();
		iter != m_listCrowID.end(); ++iter )
	{
		Faction::GLActorID sTempID = *iter;
		if ( sTempID == sID )
		{
			sc::writeLogError( sc::string::format(
				"GLActor::InsertChildCrow - Duplicated ChildCrow's ID %1%/%2%",
				sID.actorType, sID.actorID_Num ) );
			return;
		}
	}

	m_listCrowID.insert( m_listCrowID.end(), sID );

	/*sc::writeLogInfo( sc::string::format(
		"GLActor::InsertChildCrow - %1%/%2%",
		sID.actorType, sID.actorID_Num ) );*/
}

void GLActor::RemoveChildCrow( const Faction::GLActorID& sID )
{
	for( CROWIDLIST_ITER iter = m_listCrowID.begin();
		iter != m_listCrowID.end(); )
	{
		Faction::GLActorID sTempID = *iter;
		if ( sTempID == sID )
		{
			/*sc::writeLogInfo ( sc::string::format(
				"GLActor::RemoveChildCrow - %1%/%2%",
				sTempID.actorType, sTempID.actorID_Num ) );*/
			iter = m_listCrowID.erase( iter );
		}
		else
			++iter;
	}
}

bool GLActor::IsEmptyChildCrow()
{
	return m_listCrowID.empty();
}

const DWORD GLActor::GetChildCrowCount() const
{
	return m_listCrowID.size();
}