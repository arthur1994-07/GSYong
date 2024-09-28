#include "pch.h"

#include "../../../RanThirdParty/PhysX_3_2/include/foundation/PxTransform.h"
#include "../../../RanThirdParty/PhysX_3_2/include/geometry/PxGeometry.h"
#include "../../../RanThirdParty/PhysX_3_2/include/PxRigidDynamic.h"
#include "../../../RanThirdParty/PhysX_3_2/include/PxRigidActor.h"
#include "../../../RanThirdParty/PhysX_3_2/include/PxShape.h"

#include "../../DxTools/DxMethods.h"
#include "../../Common/StlFunctions.h"
#include "../../PhysX/NSPhysX.h"

#include "./DxSkeletonMaintain.h"
#include "./DxSkeletonManager.h"

#include "DxJiggleBoneColl.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern INT		g_nSELECT_COLL_BONE;

//------------------------------------------------------------------------------------------------------------
//					D	x		J	i	g	g	l	e		B	o	n	e		D	a	t	a
//------------------------------------------------------------------------------------------------------------
JiggleBoneCollData::JiggleBoneCollData()
	: m_emMeshType(EMMT_BOX)
	, m_vPosition(0.f,0.f,0.f)
	, m_vHalfScale(1.f,1.f,1.f)
	, m_strBoneName(_T(""))
	, m_pActor(NULL)
	, m_nBoneIndex(0)
{
}

JiggleBoneCollData::~JiggleBoneCollData()
{
	CleanUp();
}

void JiggleBoneCollData::CleanUp()
{
	NSPhysX::DeleteActor( m_pActor );
}

void JiggleBoneCollData::FrameMove( const DxSkeletonMaintain& sSkeletonMaintain )
{
	if ( !m_pActor )
		return;

	const DxBoneTransMaintain* pBone = sSkeletonMaintain.FindBone_Index( m_nBoneIndex );
	if ( !pBone )
		return;

	const D3DXMATRIX& matParent = pBone->m_matCombined;

	D3DXMATRIX matResult;
	D3DXMatrixTranslation( &matResult, m_vPosition.x, m_vPosition.y, m_vPosition.z );
	D3DXMatrixMultiply( &matResult, &matResult, &matParent );

	D3DXVECTOR3 vScale;
	D3DXQUATERNION vQuat;
	D3DXVECTOR3 vPos;
	D3DXMatrixToSQT( vScale, vQuat, vPos, matResult );

	physx::PxTransform pose;
	pose.q.x = vQuat.x;
	pose.q.y = vQuat.y;
	pose.q.z = vQuat.z;
	pose.q.w = vQuat.w;
	pose.p.x = vPos.x;
	pose.p.y = vPos.y;
	pose.p.z = vPos.z;
	m_pActor->setGlobalPose( pose );
}

void JiggleBoneCollData::RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwColor )
{
	if ( !m_pActor )
		return;

	// 현재 Bone의 최종 Matrix를 얻는다. < PhysX 에서 만들어진것 >
	D3DXMATRIX matActor;
	NSPhysX::GetMatrix( matActor, m_pActor );

	physx::PxShape* shape; 
	m_pActor->getShapes(&shape, 1);
	physx::PxGeometryType::Enum emGeometryType = shape->getGeometryType();

	switch ( emGeometryType )
	{
	case physx::PxGeometryType::eSPHERE:
		NSPhysX::DrawSphere( shape, matActor, dwColor );
		break;

	case physx::PxGeometryType::eCAPSULE:
		NSPhysX::DrawCapsule( shape, matActor, dwColor );
		break;

	case physx::PxGeometryType::eBOX:
		NSPhysX::DrawBox( shape, matActor, dwColor );
		break;

	default:
		break;
	};
}

// 계산 Thread 가 동작되지않는 상황에서 호출된다.
// 게임이 진행되기 위한 데이터들을 얻어오고 만든다.
BOOL JiggleBoneCollData::Import( const JiggleBoneCollData* pSrc, DxSkeleton* pSkeleton, float fScale )
{
	CleanUp();

	if ( !pSrc )
	{
		return FALSE;
	}

	if ( !pSkeleton )
	{
		return FALSE;
	}

	m_emMeshType = pSrc->m_emMeshType;
	m_vPosition = pSrc->m_vPosition;
	m_vHalfScale = pSrc->m_vHalfScale;
	m_strBoneName = pSrc->m_strBoneName;

	m_nBoneIndex = pSkeleton->GetReferenceIndex_t( m_strBoneName );
	if ( m_nBoneIndex == -1 )
		return FALSE;

	const DxBoneTrans* rBoneTrans = pSkeleton->FindBone_t( m_strBoneName );
	if ( !rBoneTrans )
		return FALSE;

	D3DXVECTOR3 vPos(rBoneTrans->matCombined._41,rBoneTrans->matCombined._42,rBoneTrans->matCombined._43);
	m_vHalfScale *= fScale;

	switch ( m_emMeshType )
	{
	case EMMT_BOX:
		NSPhysX::CreateActorBox( &m_pActor, vPos, m_vHalfScale );
		break;

	case EMMT_SPHERE:
		NSPhysX::CreateActorSphere( &m_pActor, vPos, m_vHalfScale.y );
		break;

	case EMMT_CAPSULE:
		NSPhysX::CreateActorCapsule( &m_pActor, vPos, m_vHalfScale.y, m_vHalfScale.x );
		break;

	default:
		break;
	};

	return TRUE;
}

BOOL JiggleBoneCollData::CreateCollisionData( const DxSkeleton* pSkeleton, const TSTRING& strBoneName )
{
	m_strBoneName = strBoneName;

	m_nBoneIndex = pSkeleton->GetReferenceIndex_t( strBoneName );
	if ( m_nBoneIndex == -1 )
		return FALSE;

	return TRUE;
}

void JiggleBoneCollData::Save( sc::SerialFile& SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		SFile << (DWORD)m_emMeshType;
		SFile << m_vPosition;
		SFile << m_vHalfScale;
		SFile << m_strBoneName;
	}
	SFile.EndBlock();
}

void JiggleBoneCollData::Load( sc::BaseStream& SFile, const DxSkeleton* pSkeleton )
{
	CleanUp();

	DWORD dwVersion, dwBufferSize;
	SFile >> dwVersion;
	SFile >> dwBufferSize;

	if( dwVersion == VERSION )
	{
		DWORD dwType;
		SFile >> dwType;			m_emMeshType = static_cast<MESHTYPE>( dwType );
		SFile >> m_vPosition;
		SFile >> m_vHalfScale;
		SFile >> m_strBoneName;
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
		return;
	}

	if ( pSkeleton )
	{
		m_nBoneIndex = pSkeleton->GetReferenceIndex_t( m_strBoneName );
		if ( m_nBoneIndex == -1 )
			return;
	}
}

//------------------------------------------------------------------------------------------------------------
//								D	x		J	i	g	g	l	e		B	o	n	e
//------------------------------------------------------------------------------------------------------------
DxJiggleBoneColl::DxJiggleBoneColl()
{
}

DxJiggleBoneColl::~DxJiggleBoneColl()
{
	CleanUp();
}

void DxJiggleBoneColl::CleanUp()
{
	m_vecCollisionMeshData.clear();
}

void DxJiggleBoneColl::FrameMove( const DxSkeletonMaintain& sSkeletonMaintain )
{
	for ( DWORD i=0; i<m_vecCollisionMeshData.size(); ++i )
	{
		m_vecCollisionMeshData[i]->FrameMove( sSkeletonMaintain );
	}
}

void DxJiggleBoneColl::RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice )
{
	int nCount(0);
	VEC_JIGGLEBONECOLLDATA_ITER iter = m_vecCollisionMeshData.begin();
	for ( ; iter!=m_vecCollisionMeshData.end(); ++iter, ++nCount )
	{
		if ( g_nSELECT_COLL_BONE == nCount )
		{
			(*iter)->RenderEdit( pd3dDevice, 0xffff8888 );
		}
		else
		{
			(*iter)->RenderEdit( pd3dDevice, 0xffffffff );
		}
	}
}

// 계산 Thread 가 동작되지않는 상황에서 호출된다.
// 게임이 진행되기 위한 데이터들을 얻어오고 만든다.
void DxJiggleBoneColl::Import( const DxJiggleBoneColl* pSrc, DxSkeleton* pSkeleton, float fScale )
{
	CleanUp();

	if ( !pSrc )
		return;

	VEC_JIGGLEBONECOLLDATA_CITER iter = pSrc->m_vecCollisionMeshData.begin();
	for ( ; iter!=pSrc->m_vecCollisionMeshData.end(); ++iter )
	{
		std::tr1::shared_ptr<JiggleBoneCollData> spNew = std::tr1::shared_ptr<JiggleBoneCollData>(new JiggleBoneCollData);
		if ( spNew->Import( (*iter).get(), pSkeleton, fScale ) )
			m_vecCollisionMeshData.push_back( spNew );
	}
}

void DxJiggleBoneColl::Insert( const DxSkeleton* pSkeleton, const TSTRING& strBoneName )
{
	std::tr1::shared_ptr<JiggleBoneCollData> spNew = std::tr1::shared_ptr<JiggleBoneCollData>(new JiggleBoneCollData);
	if ( spNew->CreateCollisionData( pSkeleton, strBoneName ) )
	{
		m_vecCollisionMeshData.push_back( spNew );
	}
}

void DxJiggleBoneColl::Delete( int nIndex )
{
	int nCount(0);
	VEC_JIGGLEBONECOLLDATA_ITER iter = m_vecCollisionMeshData.begin();
	for ( ; iter!=m_vecCollisionMeshData.end(); ++iter, ++nCount )
	{
		if ( nCount == nIndex )
		{
			m_vecCollisionMeshData.erase( iter );
			return;
		}
	}
}

JiggleBoneCollData* DxJiggleBoneColl::GetJiggleBoneCollData( int nIndex )
{
	int nCount(0);
	VEC_JIGGLEBONECOLLDATA_ITER iter = m_vecCollisionMeshData.begin();
	for ( ; iter!=m_vecCollisionMeshData.end(); ++iter, ++nCount )
	{
		if ( nCount == nIndex )
		{
			return (*iter).get();
		}
	}

	return NULL;
}

void DxJiggleBoneColl::Save( sc::SerialFile& SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		DWORD dwSize = static_cast<DWORD>( m_vecCollisionMeshData.size() );
		SFile << dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			m_vecCollisionMeshData[i]->Save( SFile );
		}
	}
	SFile.EndBlock();
}

void DxJiggleBoneColl::Load( sc::BaseStream& SFile, const DxSkeleton* pSkeleton )
{
	CleanUp();

	DWORD dwVersion, dwBufferSize;
	SFile >> dwVersion;
	SFile >> dwBufferSize;

	if( dwVersion == VERSION )
	{
		DWORD dwSize(0);
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			std::tr1::shared_ptr<JiggleBoneCollData> spNew = std::tr1::shared_ptr<JiggleBoneCollData>(new JiggleBoneCollData);
			spNew->Load( SFile, pSkeleton );
			m_vecCollisionMeshData.push_back( spNew );
		}
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
	}
}