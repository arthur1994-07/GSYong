#include "pch.h"

#include "../../../RanThirdParty/PhysX_3_2/include/foundation/PxTransform.h"
#include "../../../RanThirdParty/PhysX_3_2/include/geometry/PxGeometry.h"
#include "../../../RanThirdParty/PhysX_3_2/include/cloth/PxClothCollisionData.h"
#include "../../../RanThirdParty/PhysX_3_2/include/PxRigidDynamic.h"
#include "../../../RanThirdParty/PhysX_3_2/include/PxRigidActor.h"
#include "../../../RanThirdParty/PhysX_3_2/include/PxShape.h"

#include "../../DxTools/DxMethods.h"
#include "../../DxTools/EditMeshs.h"
#include "../../Common/StlFunctions.h"
#include "../../PhysX/NSPhysX.h"

#include "./DxSkeletonMaintain.h"
#include "./DxSkeletonManager.h"

#include "DxClothColl.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern INT		g_nSELECT_COLL_BONE;

//------------------------------------------------------------------------------------------------------------
//					D	x		J	i	g	g	l	e		B	o	n	e		D	a	t	a
//------------------------------------------------------------------------------------------------------------
ClothCollData::ClothCollData()
	: m_vPosition(0.f,0.f,0.f)
	, m_fRadius(1.f)
	, m_strBoneName(_T(""))
	, m_nBoneIndex(-1)
{
}

ClothCollData::~ClothCollData()
{
	CleanUp();
}

void ClothCollData::CleanUp()
{
}

void ClothCollData::RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton, DWORD dwColor )
{
	const DxBoneTrans* pBoneTrans = pSkeleton->FindBone_Index_Bone_const( m_nBoneIndex );

	D3DXVECTOR3 vPos;
	D3DXVec3TransformCoord( &vPos, &m_vPosition, &pBoneTrans->matCombined );

	EDITMESHS::RENDERSPHERE( vPos,
							D3DXVECTOR3( m_fRadius, m_fRadius, m_fRadius ),
							FALSE, 
							NULL,
							dwColor );
}

// 게임이 진행되기 위한 데이터들을 얻어오고 만든다.
BOOL ClothCollData::Import( const ClothCollData* pSrc, float fScale, const DxSkeletonMaintain* pSkeleton )
{
	CleanUp();

	if ( !pSrc )
	{
		return FALSE;
	}

	m_vPosition = pSrc->m_vPosition;
	m_fRadius = pSrc->m_fRadius;
	m_strBoneName = pSrc->m_strBoneName;
	m_nBoneIndex = pSrc->m_nBoneIndex;

	FrameMove_MOVED( pSkeleton );

	return TRUE;
}

void ClothCollData::FrameMove_ERROR( const DxSkeletonMaintain* pSkeleton, const D3DXMATRIX& matWorld ) const
{
	memcpy ( &m_vPositionResult.x, &matWorld._41, sizeof(float)*3 );
}

void ClothCollData::FrameMove_STATIC( const DxSkeletonMaintain* pSkeleton ) const
{
	const DxBoneTransMaintain* pBoneTrans = pSkeleton->FindBone_Index( m_nBoneIndex );
	if ( !pBoneTrans )
		return;

	memcpy ( &m_vPositionResult.x, &pBoneTrans->m_matCombined._41, sizeof(float)*3 );
}

void ClothCollData::FrameMove_MOVED( const DxSkeletonMaintain* pSkeleton ) const
{
	const DxBoneTransMaintain* pBoneTrans = pSkeleton->FindBone_Index( m_nBoneIndex );
	if ( !pBoneTrans )
		return;

	D3DXVec3TransformCoord( &m_vPositionResult, &m_vPosition, &pBoneTrans->m_matCombined );
}

BOOL ClothCollData::CreateCollisionData( const DxSkeleton* pSkeleton, const TSTRING& strBoneName )
{
	m_strBoneName = strBoneName;

	m_nBoneIndex = pSkeleton->GetReferenceIndex_t( m_strBoneName );
	if ( m_nBoneIndex == -1 )
		return FALSE;

	return TRUE;
}

void ClothCollData::Save( sc::SerialFile& SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		SFile << m_vPosition;
		SFile << m_fRadius;
		SFile << m_strBoneName;
	}
	SFile.EndBlock();
}

BOOL ClothCollData::Load( sc::BaseStream& SFile, const DxSkeleton* pSkeleton )
{
	CleanUp();

	DWORD dwVersion, dwBufferSize;
	SFile >> dwVersion;
	SFile >> dwBufferSize;

	if( dwVersion == VERSION )
	{
		SFile >> m_vPosition;
		SFile >> m_fRadius;
		SFile >> m_strBoneName;
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
		return FALSE;
	}

	m_nBoneIndex = pSkeleton->GetReferenceIndex_t( m_strBoneName );
	if ( m_nBoneIndex == -1 )
		return FALSE;

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------
//								D	x		J	i	g	g	l	e		B	o	n	e
//------------------------------------------------------------------------------------------------------------
DxClothColl::DxClothColl()
{
}

DxClothColl::~DxClothColl()
{
	CleanUp();
}

void DxClothColl::CleanUp()
{
	m_vecIndexPair.clear();
	m_vecCollisionMeshData.clear();
}

// TAG_CalculateAnimationThread_2_5
// 다른 Thread 상에서 변경되는 Data 는 외부 Data -> vecSpheres
void DxClothColl::FrameMove( std::vector<physx::PxClothCollisionSphere>& vecSpheres, 
							const DxSkeletonMaintain* pSkeleton, 
							const D3DXMATRIX& matWorld, 
							float fBaseBoneScale ) const
{
	for ( DWORD i=0; i<m_vecCollisionMesh_ERROR.size(); ++i )
	{
		m_vecCollisionMesh_ERROR[i]->FrameMove_ERROR( pSkeleton, matWorld );
	}

	// 위치 고정인 것
	for ( DWORD i=0; i<m_vecCollisionMesh_STATIC.size(); ++i )
	{
		m_vecCollisionMesh_STATIC[i]->FrameMove_STATIC( pSkeleton );
	}

	// 위치 이동된 것
	for ( DWORD i=0; i<m_vecCollisionMesh_MOVED.size(); ++i )
	{
		m_vecCollisionMesh_MOVED[i]->FrameMove_MOVED( pSkeleton );
	}

	GetAllClothCollData( vecSpheres, fBaseBoneScale );
}

//// 게임이 진행되기 위한 데이터들을 얻어오고 만든다.
//void DxClothColl::Import( const DxClothColl* pSrc, float fScale, const DxSkeletonMaintain* pSkeleton )
//{
//	CleanUp();
//
//	if ( !pSrc )
//		return;
//
//	VEC_CLOTH_COLLDATA_CITER iter = pSrc->m_vecCollisionMeshData.begin();
//	for ( ; iter!=pSrc->m_vecCollisionMeshData.end(); ++iter )
//	{
//		std::tr1::shared_ptr<ClothCollData> spNew = std::tr1::shared_ptr<ClothCollData>(new ClothCollData);
//		if ( spNew->Import( (*iter).get(), fScale, pSkeleton ) )
//			m_vecCollisionMeshData.push_back( spNew );
//	}
//}

// FrameMove 과 GetAllClothCollData 에서 사용되는 vecSpheres 는 다르다.
void DxClothColl::GetAllClothCollData( std::vector<physx::PxClothCollisionSphere>& vecSpheres, float fBaseBoneScale ) const
{
	if ( vecSpheres.size() != m_vecCollisionMeshData.size() )
	{
		vecSpheres.resize( m_vecCollisionMeshData.size() );
	}

	// 위치 고정인 것
	for ( DWORD i=0; i<m_vecCollisionMeshData.size(); ++i )
	{
		memcpy ( &vecSpheres[i].pos, &m_vecCollisionMeshData[i]->m_vPositionResult, sizeof(float)*3 );
		vecSpheres[i].radius = m_vecCollisionMeshData[i]->m_fRadius * fBaseBoneScale;
	}
}

// FrameMove 과 GetAllClothCollData 에서 사용되는 vecSpheres 는 다르다.
void DxClothColl::GetAllClothCollData( std::vector<physx::PxClothCollisionSphere>& vecSpheres, std::vector<DWORD>& vecIndexPairs, float fBaseBoneScale ) const
{
	// 기본 값을 얻도록 한다.
	GetAllClothCollData( vecSpheres, fBaseBoneScale );

	// IndexPair
	int nIndex = 0;
	vecIndexPairs.resize( m_vecIndexPair.size()*2 );
	for ( DWORD i=0; i<m_vecIndexPair.size(); ++i )
	{
		vecIndexPairs[nIndex++] = m_vecIndexPair[i].wA;
		vecIndexPairs[nIndex++] = m_vecIndexPair[i].wB;
	}
}

// Edit
void DxClothColl::RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton ) const
{
	if ( !pSkeleton )
		return;

	int nCount(0);
	VEC_CLOTH_COLLDATA_CITER citer = m_vecCollisionMeshData.begin();
	for ( ; citer!=m_vecCollisionMeshData.end(); ++citer, ++nCount )
	{
		if ( g_nSELECT_COLL_BONE == nCount )
		{
			(*citer)->RenderEdit( pd3dDevice, pSkeleton, 0xffff8888 );
		}
		else
		{
			(*citer)->RenderEdit( pd3dDevice, pSkeleton, 0xffffffff );
		}
	}

	for ( DWORD i=0; i<m_vecIndexPair.size(); ++i )
	{
		const DxBoneTrans* pBoneTransA = pSkeleton->FindBone_t( m_vecCollisionMeshData[ m_vecIndexPair[i].wA ]->m_strBoneName );
		if ( !pBoneTransA )
			continue;

		const DxBoneTrans* pBoneTransB = pSkeleton->FindBone_t( m_vecCollisionMeshData[ m_vecIndexPair[i].wB ]->m_strBoneName );
		if ( !pBoneTransB )
			continue;

		D3DXVECTOR3 vPosA;
		D3DXVec3TransformCoord( &vPosA, &m_vecCollisionMeshData[ m_vecIndexPair[i].wA ]->m_vPosition, &pBoneTransA->matCombined );

		D3DXVECTOR3 vPosB;
		D3DXVec3TransformCoord( &vPosB, &m_vecCollisionMeshData[ m_vecIndexPair[i].wB ]->m_vPosition, &pBoneTransB->matCombined );

		D3DXVECTOR3 vPos = vPosA * 0.25f + vPosB * 0.75f;
		float fRadius = m_vecCollisionMeshData[ m_vecIndexPair[i].wA ]->m_fRadius * 0.25f + m_vecCollisionMeshData[ m_vecIndexPair[i].wB ]->m_fRadius * 0.75f;

		EDITMESHS::RENDERSPHERE( vPos,
			D3DXVECTOR3( fRadius, fRadius, fRadius ),
			FALSE, 
			NULL,
			0xffffffff );

		vPos = vPosA * 0.5f + vPosB * 0.5f;
		fRadius = m_vecCollisionMeshData[ m_vecIndexPair[i].wA ]->m_fRadius * 0.5f + m_vecCollisionMeshData[ m_vecIndexPair[i].wB ]->m_fRadius * 0.5f;

		EDITMESHS::RENDERSPHERE( vPos,
			D3DXVECTOR3( fRadius, fRadius, fRadius ),
			FALSE, 
			NULL,
			0xffffffff );

		vPos = vPosA * 0.75f + vPosB * 0.25f;
		fRadius = m_vecCollisionMeshData[ m_vecIndexPair[i].wA ]->m_fRadius * 0.75f + m_vecCollisionMeshData[ m_vecIndexPair[i].wB ]->m_fRadius * 0.25f;

		EDITMESHS::RENDERSPHERE( vPos,
			D3DXVECTOR3( fRadius, fRadius, fRadius ),
			FALSE, 
			NULL,
			0xffffffff );
	}
}

// Edit
void DxClothColl::Insert( const DxSkeleton* pSkeleton, const TSTRING& strBoneName )
{
	// 같은 Bone 이 있는지 검사한다.
	for ( WORD i=0; i<m_vecCollisionMeshData.size(); ++i )
	{
		if ( m_vecCollisionMeshData[i]->m_strBoneName == strBoneName )
			return;
	}

	std::tr1::shared_ptr<ClothCollData> spNew = std::tr1::shared_ptr<ClothCollData>(new ClothCollData);
	if ( spNew->CreateCollisionData( pSkeleton, strBoneName ) )
	{
		m_vecCollisionMeshData.push_back( spNew );
	}

	CreateIndexPair( pSkeleton );
}

// Edit
void DxClothColl::Delete( const DxSkeleton* pSkeleton, int nIndex )
{
	int nCount(0);
	VEC_CLOTH_COLLDATA_ITER iter = m_vecCollisionMeshData.begin();
	for ( ; iter!=m_vecCollisionMeshData.end(); ++iter, ++nCount )
	{
		if ( nCount == nIndex )
		{
			m_vecCollisionMeshData.erase( iter );
			break;
		}
	}

	CreateIndexPair( pSkeleton );
}

// Edit
ClothCollData* DxClothColl::GetClothCollData( int nIndex )
{
	int nCount(0);
	VEC_CLOTH_COLLDATA_ITER iter = m_vecCollisionMeshData.begin();
	for ( ; iter!=m_vecCollisionMeshData.end(); ++iter, ++nCount )
	{
		if ( nCount == nIndex )
		{
			return (*iter).get();
		}
	}

	return NULL;
}

// Edit
// FrameMove 를 위한 정보를 Refresh 한다.
void DxClothColl::RefreshCollisionMeshForFrameMove()
{
	//////////////////////////////////////////////////////////////////////////
	// Edit 상황에 맞게 정보를 변경한다.
	m_vecCollisionMesh_ERROR.clear();
	m_vecCollisionMesh_STATIC.clear();
	m_vecCollisionMesh_MOVED.clear();

	VEC_CLOTH_COLLDATA_CITER iter = m_vecCollisionMeshData.begin();
	for ( ; iter!=m_vecCollisionMeshData.end(); ++iter )
	{
		if ( (*iter)->m_nBoneIndex == -1 )
		{
			m_vecCollisionMesh_ERROR.push_back( (*iter) );
		}
		else if ( (*iter)->m_vPosition.x == (*iter)->m_vPosition.y &&
			(*iter)->m_vPosition.y == (*iter)->m_vPosition.z &&
			(*iter)->m_vPosition.z == 0.f )
		{
			m_vecCollisionMesh_STATIC.push_back( (*iter) );
		}
		else
		{
			m_vecCollisionMesh_MOVED.push_back( (*iter) );
		}
	}
}

// Edit
void DxClothColl::CreateIndexPair( const DxSkeleton* pSkeleton )
{
	m_vecIndexPair.clear();
	for ( WORD i=0; i<m_vecCollisionMeshData.size(); ++i )
	{
		const DxBoneTrans* pBoneTransA = pSkeleton->FindBone_t( m_vecCollisionMeshData[i]->m_strBoneName.c_str() );
		if ( pBoneTransA == NULL )
			continue;

		const DxBoneTrans* pParent = pBoneTransA;

		BOOL bOK(FALSE);
		while ( !bOK )
		{
			pParent = pParent->pBoneTransParent;
			if ( !pParent )
				break;

			for ( WORD j=0; j<m_vecCollisionMeshData.size(); ++j )
			{
				// 같은건 할 필요없다.
				if ( i == j )
					continue;

				const DxBoneTrans* pBoneTransB = pSkeleton->FindBone_t( m_vecCollisionMeshData[j]->m_strBoneName.c_str() );
				if ( pBoneTransB == NULL )
					continue;

				if ( pParent == pBoneTransB )
				{
					// wA 자식, wB 부모
					bOK = TRUE;
					WORDWORD wwIndexPair(i,j);
					m_vecIndexPair.push_back( wwIndexPair );
					break;
				}
			}
		}
	}
}

void DxClothColl::Save( sc::SerialFile& SFile )
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

void DxClothColl::Load( sc::BaseStream& SFile, const DxSkeleton* pSkeleton )
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
			std::tr1::shared_ptr<ClothCollData> spNew = std::tr1::shared_ptr<ClothCollData>(new ClothCollData);
			spNew->Load( SFile, pSkeleton );
			m_vecCollisionMeshData.push_back( spNew );
		}

		RefreshCollisionMeshForFrameMove();
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
	}

	CreateIndexPair( pSkeleton );
}