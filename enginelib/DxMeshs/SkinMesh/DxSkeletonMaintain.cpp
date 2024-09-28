#include "pch.h"

#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/EditMeshs.h"

#include "./DxSkeletonManager.h"

#include "DxSkeletonMaintain.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//						BoneTransMapMaintain
void BoneTransMapMaintain::CleanUp()
{
	m_mapBoneMap.clear();
}

void BoneTransMapMaintain::Import_PureThread( const BoneTransMap* pBoneTransMap, const std::vector<DxBoneTrans*> vecReferenceBoneTransSRC, std::vector<DxBoneTransMaintain*>& vecReferenceBoneTrans )
{
	if ( vecReferenceBoneTransSRC.size() == vecReferenceBoneTrans.size() )
	{
		// 처음 외는 무조건 이곳으로 와야한다.
		for ( DWORD i=0; i<vecReferenceBoneTransSRC.size(); ++i )
		{
			vecReferenceBoneTrans[i]->m_matCombined = vecReferenceBoneTransSRC[i]->matCombined;
		}
	}
	else
	{
		// 처음 들어오면 이곳으로온다.
		CleanUp();

		vecReferenceBoneTrans.clear();
		vecReferenceBoneTrans.resize( vecReferenceBoneTransSRC.size() );

		int nCount(0);
		BoneTransMap::BONE_TRANS_MAP_CITER citer = pBoneTransMap->cbegin();
		for ( ; citer!=pBoneTransMap->cend(); ++citer )
		{
			std::tr1::shared_ptr<DxBoneTransMaintain> spBoneTransMaintain = 
				std::tr1::shared_ptr<DxBoneTransMaintain>( new DxBoneTransMaintain( nCount, (*citer).second->matCombined) );

			m_mapBoneMap.insert( std::make_pair( (*citer).first, spBoneTransMaintain ) );

			vecReferenceBoneTrans[nCount++] = spBoneTransMaintain.get();
		}
	}
}



//////////////////////////////////////////////////////////////////////////
//						DxSkeletonMaintain
DxSkeletonMaintain::DxSkeletonMaintain()
{
}

DxSkeletonMaintain::~DxSkeletonMaintain()
{
	CleanUp();
}

void DxSkeletonMaintain::CleanUp()
{
	m_sBoneMap.CleanUp();
	m_vecReferenceBoneTrans.clear();
}

const DxBoneTransMaintain* DxSkeletonMaintain::FindBone( const TSTRING& strName) const
{
	return m_sBoneMap.findData(strName);
}

const DxBoneTransMaintain* DxSkeletonMaintain::FindBone_Index( int nIndex ) const
{
	if ( nIndex >= static_cast<int>( m_vecReferenceBoneTrans.size() ) )
		return NULL;

	return m_vecReferenceBoneTrans[nIndex];
}

void DxSkeletonMaintain::CheckSphere( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName ) const
{
	const DxBoneTransMaintain* pBoneTran = FindBone( strName );
	if( !pBoneTran )
		return;

	D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
	vPos.x = pBoneTran->m_matCombined._41;
	vPos.y = pBoneTran->m_matCombined._42;
	vPos.z = pBoneTran->m_matCombined._43;

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

	EDITMESHS::RENDERSPHERE( pd3dDevice, vPos, 0.006f*DxViewPort::GetInstance().GetDistance(), NULL, 0xffff0000 );

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );
}

BOOL DxSkeletonMaintain::IsActiveBone() const
{
	return m_vecReferenceBoneTrans.empty() ? FALSE : TRUE;
}

// TAG_CalculateAnimationThread_2_4
// Import
void DxSkeletonMaintain::Import_PureThread( const DxSkeleton* pSkeleton )
{
	m_sBoneMap.Import_PureThread( &pSkeleton->m_skeletonPart.BoneMap, pSkeleton->m_vecReferenceBoneTrans, m_vecReferenceBoneTrans );
	
	//////////////////////////////////////////////////////////////////////////
	// [shhan][2014.05.29] m_sBoneMap.Import 안의 pSkeleton->m_skeletonPart.BoneMap 주가 아닌 여기서 m_sBoneMap 기준으로 하니 순서가 꼬이는거 같다.
	//////////////////////////////////////////////////////////////////////////
//	// 여기서 vector 형식으로도 참조 할 수 있도록 작업한다.
//	m_vecReferenceBoneTrans.clear();
//	BoneTransMapMaintain::BONE_TRANS_MAP_Maintain_CITER citer = m_sBoneMap.begin();
//	for ( ; citer!=m_sBoneMap.end(); ++citer )
//	{
//		m_vecReferenceBoneTrans.push_back( (*citer).second.get() );
//	}
}