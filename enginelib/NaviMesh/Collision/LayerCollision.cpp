#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "./SimpleCollision.h"


#include "LayerCollision.h"



//////////////////////////////////////////////////////////////////////////
//						LayerCollision
LayerCollision::LayerCollision( BOOL bActive )
: m_bActiveForClient(bActive)
{
}

LayerCollision::~LayerCollision()
{
}

void LayerCollision::SetActiveLayer()
{
	m_bActiveForClient = TRUE;
}

void LayerCollision::SetDeActiveLayer()
{
	m_bActiveForClient = FALSE;
}

BOOL LayerCollision::IsActiveLayer() const
{
	return m_bActiveForClient;
}

BOOL LayerCollision::IsExistSimpleCollision() const
{
	return m_vecSimpleCollision.empty() ? FALSE : TRUE;
}

SIMPLE_COLLISION_RESULT LayerCollision::IsCollision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset ) const
{
	if ( !m_bActiveForClient )
		return COLL_FALSE;

	SIMPLE_COLLISION_RESULT emResult;
	for ( DWORD i=0; i<m_vecSimpleCollision.size(); ++i )
	{
		emResult = m_vecSimpleCollision[i]->IsCollision( vStart, vEnd, fOffset );

		// 충돌이 되었다면 return
		if ( emResult != COLL_FALSE )
			return emResult;
	}
	return COLL_FALSE;
}

const SimpleCollisionBase* LayerCollision::FindCollision( const char* szName ) const
{
	for ( DWORD i=0; i<m_vecSimpleCollision.size(); ++i )
	{
		if ( !strcmp(m_vecSimpleCollision[i]->GetName(),szName) )
			return m_vecSimpleCollision[i].get();
	}

	return NULL;
}

void LayerCollision::AddCollision( boost::shared_ptr<SimpleCollisionBase> pNew )
{
	m_vecSimpleCollision.push_back( pNew );
}

void LayerCollision::ChangeCollision( UINT unIndex, boost::shared_ptr<SimpleCollisionBase> spNew )
{
	if ( unIndex >= m_vecSimpleCollision.size() )
		return;

	*(m_vecSimpleCollision.begin()+unIndex) = spNew;
}

void LayerCollision::DelCollision( UINT unIndex )
{
	if ( unIndex >= m_vecSimpleCollision.size() )
		return;

	m_vecSimpleCollision.erase( m_vecSimpleCollision.begin()+unIndex );
}

void LayerCollision::SetObjRotate( float fAddAngle )
{
	for ( DWORD i=0; i<m_vecSimpleCollision.size(); ++i )
	{
		m_vecSimpleCollision[i]->SetObjRotate( fAddAngle );
	}
}

const std::vector<boost::shared_ptr<SimpleCollisionBase>>& LayerCollision::GetvecSimpleCollision() const
{ 
	return m_vecSimpleCollision; 
}

void LayerCollision::Save( sc::SerialFile &SFile )
{
	SFile << static_cast<DWORD>( VERSION );

	SFile.BeginBlock();
	{
		SFile << static_cast<DWORD>( m_vecSimpleCollision.size() );

		for ( DWORD i=0; i<m_vecSimpleCollision.size(); ++i )
		{
			SIMPLE_COLLISION_TYPE emType = m_vecSimpleCollision[i]->GetType();
			SFile << static_cast<DWORD>( emType );

			m_vecSimpleCollision[i]->Save( SFile );
		}
	}
	SFile.EndBlock();
}

void LayerCollision::Load( sc::BaseStream &SFile )
{
	m_vecSimpleCollision.clear();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	DWORD dwSFile_Check = SFile.GetfTell();

	if ( dwVer == 0x100 )
	{
		DWORD dwSize(0);
		DWORD dwType(0);
		SFile >> dwSize;

		for ( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> dwType;

			SimpleCollisionBase* pNew;
			switch ( dwType )
			{
			case EMSCT_PLANE:
				pNew = new SimpleCollisionPlane;
				break;
			case EMSCT_BOX:
				pNew = new SimpleCollisionBox;
				break;
			case EMSCT_CYLINDER:
				pNew = new SimpleCollisionCylinder;
				break;
			default:
				SFile.SetOffSet ( dwSFile_Check + dwBufferSize );
				return;
			};

			pNew->Load( SFile );

			m_vecSimpleCollision.push_back( boost::shared_ptr<SimpleCollisionBase>( pNew ) );
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBufferSize );
	}
}