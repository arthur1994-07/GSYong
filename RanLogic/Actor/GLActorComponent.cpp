#include "pch.h"
#include "GLActor.h"

void GLActor::InsertComponent( ComponentSystem::IComponentBase* pComponent )
{
	if( !pComponent )
		return;

	const ComponentSystem::EMCOMP_ID emCompID = pComponent->GetComponentID();
	const ComponentSystem::EMFAMILY_ID emFamilyID = pComponent->GetFamilyID();
	for( COMP_MAP_ITER iter = m_mapComponet.begin();
		iter != m_mapComponet.end(); ++iter )
	{
		// 같은 Family의 Component일 경우 추가할 수 없다;
		if ( emCompID == iter->first )
		{
			sc::writeLogError( "GLActor::InsertComponent - Duplicated Component's Family ID." );
			return;
		}

		// 같은 Component가 있을 경우 추가할 수 없다;
		ComponentSystem::IComponentBase* pTempComponent = iter->second;
		if ( emFamilyID == pTempComponent->GetComponentID() )
		{
			sc::writeLogError( "GLActor::InsertComponent - Duplicated Component ID." );
			return;
		}
	}

	m_mapComponet[ emFamilyID ] = pComponent;

	/*sc::writeLogInfo( sc::string::format(
		"GLActor::InsertComponent - %1%/%2%",
		emCompID, emFamilyID ) );*/
}

ComponentSystem::IComponentBase* GLActor::GetComponent( 
	const ComponentSystem::EMFAMILY_ID& emFamilyID )
{
	COMP_MAP_ITER iter = m_mapComponet.find( emFamilyID );
	if( iter != m_mapComponet.end() )
		return iter->second;

	return NULL;
}

const ComponentSystem::IComponentBase* GLActor::GetComponent(
	const ComponentSystem::EMFAMILY_ID& emFamilyID ) const
{
	COMP_MAP_CITER iter = m_mapComponet.find( emFamilyID );
	if( iter != m_mapComponet.end() )
		return iter->second;

	return NULL;
}

void GLActor::RemoveAllComponent()
{
	for( COMP_MAP_ITER iter = m_mapComponet.begin();
		iter != m_mapComponet.end(); ++iter )
	{
		SAFE_DELETE( iter->second );
	}

	m_mapComponet.clear();
}