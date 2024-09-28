#include "pch.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxTools/RENDERPARAM.h"
#include "./_new/DxSkinMeshContainer9.h"
#include "./_new/DxSkinMesh9_NORMAL.h"
#include "./DxSkeletonMaintain.h"
#include "./DxSkinAniControl.h"

#include "DxSkinAniControlThreadNeedData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



//////////////////////////////////////////////////////////////////////////
//					DxSkinAniControlThreadNeedData
//////////////////////////////////////////////////////////////////////////
DxSkinAniControlThreadNeedData::DxSkinAniControlThreadNeedData( int wCharPartNUM )
{
	m_dwCharPartNUM = wCharPartNUM;

	m_arrayKeyJiggleWeight = new float[wCharPartNUM+1];

	for ( int i=0; i<wCharPartNUM+1; ++i )
	{
		m_arrayKeyJiggleWeight[i] = 1.f;
	}

	m_wwMoveParentBoneIDForSafeModeWeaponSlot.dwData = -1;
}

DxSkinAniControlThreadNeedData::~DxSkinAniControlThreadNeedData()
{
	SAFE_DELETE_ARRAY( m_arrayKeyJiggleWeight );
}



//////////////////////////////////////////////////////////////////////////
//						CharPartBoneMatrices
//////////////////////////////////////////////////////////////////////////
CharPartBoneMatrices::CharPartBoneMatrices( const SMeshContainer& sMeshContainer, std::tr1::shared_ptr<DxSkinAniControl>& spSkinAniControlThread )
	: m_rMeshContainer( &sMeshContainer )
	, m_dwUsedIndex(0)
	, m_dwCalculateIndex(0)
	, m_nKeyTime(-1)
	, m_bGhostingON(FALSE)
	, m_nPrevKeyTimeForGhosting(-1)
	, m_fLifeTimeForGhosting(1.f)
	, m_spSkinAniControlThread(spSkinAniControlThread)
{
	m_vecarrayBoneMatrices.resize(2);

	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		m_dwBoneNum = sMeshContainer.m_dwNumAttributeGroups * sMeshContainer.m_dwNumInfl;

		for ( DWORD i=0; i<2; ++i )
		{
			D3DXMATRIXA16* arrayBoneMatrices = new D3DXMATRIXA16[ m_dwBoneNum ];
			for ( DWORD z=0; z<m_dwBoneNum; ++z )
			{
				D3DXMatrixIdentity( &arrayBoneMatrices[z] );
			}
			m_vecarrayBoneMatrices[i] = arrayBoneMatrices;
		}
	}
	else
	{
		m_dwBoneNum = sMeshContainer.m_dwNumAttributeGroups * sMeshContainer.m_dwNumPaletteEntries;

		for ( DWORD i=0; i<2; ++i )
		{
			D3DXMATRIXA16* arrayBoneMatrices = new D3DXMATRIXA16[ m_dwBoneNum ];
			for ( DWORD z=0; z<m_dwBoneNum; ++z )
			{
				D3DXMatrixIdentity( &arrayBoneMatrices[z] );
			}
			m_vecarrayBoneMatrices[i] = arrayBoneMatrices;
		}
	}

	// [shhan][2014.11.25] Skeleton 에 값이 들어 있을 때만 작업하도록 한다.
	//						아이템을 변경할 시 자신의 변경할 Part가 1frame 안보이는 문제 수정하기 위함. ( Mesh 는 있지만 Matrix 값이 없어서 Matrix 값을 넣어줌. )
	if ( (m_dwBoneNum > 1) && spSkinAniControlThread->GetSkeletonMaintain()->FindBone_Index( m_dwBoneNum-1 ) )
	{
		CaculateBoneMatrices( sMeshContainer, m_vecarrayBoneMatrices[0], *spSkinAniControlThread->GetSkeletonMaintain() );
	}
}

CharPartBoneMatrices::~CharPartBoneMatrices()
{
	for ( DWORD i=0; i<m_vecarrayBoneMatrices.size(); ++i )
	{
		SAFE_DELETE_ARRAY( m_vecarrayBoneMatrices[i] );
	}

	{
		std::list<GhostingData*>::iterator iter = m_listarrayGhostingDataBackUp.begin();
		for ( ; iter!=m_listarrayGhostingDataBackUp.end(); ++iter )
		{
			SAFE_DELETE_ARRAY( (*iter)->m_arrayBoneMatrices );
			SAFE_DELETE( (*iter) );
		}
	}
	{
		std::deque<GhostingData*>::iterator iter = m_dequearrayGhostingDataUse.begin();
		for ( ; iter!=m_dequearrayGhostingDataUse.end(); ++iter )
		{
			SAFE_DELETE_ARRAY( (*iter)->m_arrayBoneMatrices );
			SAFE_DELETE( (*iter) );
		}
	}
	
}

void CharPartBoneMatrices::CaculateBoneMatrices( const SMeshContainer& sMeshContainer, 
												  D3DXMATRIXA16* arrayBoneMatrices,
												  const DxSkeletonMaintain& sSkeleton )
{
	UINT iAttrib;
	UINT iMatrixIndex;
	UINT iPaletteEntry;
	LPD3DXBONECOMBINATION pBoneComb;

	// first check for skinning
	if ( sMeshContainer.pSkinInfo )
	{
		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		{
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(sMeshContainer.m_pBoneCombinationBuf->GetBufferPointer());
			for (iAttrib = 0; iAttrib < sMeshContainer.m_dwNumAttributeGroups; ++iAttrib)
			{ 
				LPD3DXBONECOMBINATION pBoneCur = &pBoneComb[iAttrib];

				UINT NumBlend = 0;
				DWORD dwNumInfl = sMeshContainer.m_dwNumInfl;
				for (DWORD i = 0; i < dwNumInfl; ++i)
				{
					if (pBoneCur->BoneId[i] != UINT_MAX)
					{
						NumBlend = i;
					}
				}

				if( DxSkinMesh9_NORMAL::m_dwMaxVertexBlendMatrices >= NumBlend + 1 )
				{
					for (DWORD i = 0; i < dwNumInfl; ++i)
					{
						iMatrixIndex = pBoneCur->BoneId[i];
						if (iMatrixIndex != UINT_MAX)
						{
							D3DXMatrixMultiply( &arrayBoneMatrices[ (iAttrib*sMeshContainer.m_dwNumInfl) + i], 
												&sMeshContainer.m_pBoneOffsetMatrices[iMatrixIndex], 
												&sSkeleton.FindBone_Index( sMeshContainer.m_arrayBoneIndex[iMatrixIndex] )->m_matCombined );
						}
					}
				}
			}
		}
		else
		{
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(sMeshContainer.m_pBoneCombinationBuf->GetBufferPointer());
			for (iAttrib = 0; iAttrib < sMeshContainer.m_dwNumAttributeGroups; ++iAttrib)
			{ 
				// first calculate all the world matrices
				for (iPaletteEntry = 0; iPaletteEntry < sMeshContainer.m_dwNumPaletteEntries; ++iPaletteEntry)
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
					if (iMatrixIndex != UINT_MAX)
					{
						//if ( !sSkeleton.FindBone( sMeshContainer.m_arrayBoneIndex[iMatrixIndex] ) )
						//	continue;	// 이곳에 절대 들어오면 안된다.

						D3DXMatrixMultiply( &arrayBoneMatrices[ (iAttrib*sMeshContainer.m_dwNumPaletteEntries) + iPaletteEntry], 
											&sMeshContainer.m_pBoneOffsetMatrices[iMatrixIndex], 
											&sSkeleton.FindBone_Index( sMeshContainer.m_arrayBoneIndex[iMatrixIndex] )->m_matCombined );
					}
				}
			}
		}
	}
}

void CharPartBoneMatrices::FrameMove_PureThread()
{
	// Bone Matrices 계산
	// Back 값에 작업이 되어 있다. DxSkinAniControl::SwapBone 전이기 때문에 그렇다.
	CaculateBoneMatrices( *m_rMeshContainer, m_vecarrayBoneMatrices[m_dwCalculateIndex], *m_spSkinAniControlThread->GetSkeletonMaintainBackUp() );
}

const D3DXMATRIXA16* CharPartBoneMatrices::GetBoneMatrices() const
{
	// [shhan][2014.08.08] 여기서 Error 가 뜨면 혹시 DxCharPart::SetPart 후에 DxCharPart::FrameMove 가 한번이라도 호출이 되는지 확인해봐야 할것임.
	return m_vecarrayBoneMatrices[m_dwUsedIndex];
}

void CharPartBoneMatrices::SwapBoneMatrices( float fElapsedTime )
{
	m_dwUsedIndex = m_dwCalculateIndex;

	++m_dwCalculateIndex;
	if ( m_dwCalculateIndex >= m_vecarrayBoneMatrices.size() )
		m_dwCalculateIndex = 0;

	//////////////////////////////////////////////////////////////////////////
	//							Ghosting
	{
		if ( m_bGhostingON )
		{
			// Key 가 변경되었으므로 작업을 해준다.
			if ( m_nPrevKeyTimeForGhosting != m_spSkinAniControlThread->GETCURKEYTIME() )
			{
				m_nPrevKeyTimeForGhosting = m_spSkinAniControlThread->GETCURKEYTIME();

				if ( !m_listarrayGhostingDataBackUp.empty() )
				{
					// 여유분에서 가져온다.
					std::list<GhostingData*>::iterator iter = m_listarrayGhostingDataBackUp.begin();

					(*iter)->m_fLifeTime = m_fLifeTimeForGhosting;
					D3DXMATRIXA16* pTemp = m_vecarrayBoneMatrices[m_dwCalculateIndex];			// 이전에 사용되었던 것을 스왑한다.
					m_vecarrayBoneMatrices[m_dwCalculateIndex] = (*iter)->m_arrayBoneMatrices;
					(*iter)->m_arrayBoneMatrices = pTemp;

					m_dequearrayGhostingDataUse.push_back( (*iter) );
					m_listarrayGhostingDataBackUp.erase( iter );
				}
				else if ( !m_dequearrayGhostingDataUse.empty() )
				{
					// 여유분이 없으므로 사용중인것을 쓴다.
					std::deque<GhostingData*>::iterator iter = m_dequearrayGhostingDataUse.begin();

					(*iter)->m_fLifeTime = m_fLifeTimeForGhosting;
					D3DXMATRIXA16* pTemp = m_vecarrayBoneMatrices[m_dwCalculateIndex];			// 이전에 사용되었던 것을 스왑한다.
					m_vecarrayBoneMatrices[m_dwCalculateIndex] = (*iter)->m_arrayBoneMatrices;
					(*iter)->m_arrayBoneMatrices = pTemp;

					m_dequearrayGhostingDataUse.push_back( (*iter) );
					m_dequearrayGhostingDataUse.pop_front();
				}
			}
		}
		else
		{
			m_nPrevKeyTimeForGhosting = -1;
		}

		std::deque<GhostingData*>::iterator iter = m_dequearrayGhostingDataUse.begin();
		for ( ; iter!=m_dequearrayGhostingDataUse.end();  )
		{
			(*iter)->m_fLifeTime -= fElapsedTime;

			if ( (*iter)->m_fLifeTime < 0.f )
			{
				m_listarrayGhostingDataBackUp.push_back( (*iter) );
				iter = m_dequearrayGhostingDataUse.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}
}

void CharPartBoneMatrices::GhostingON( BOOL bGhosting, float fLifeTime )
{
	m_bGhostingON = bGhosting;
	m_fLifeTimeForGhosting = fLifeTime;

	if ( m_listarrayGhostingDataBackUp.empty() && m_dequearrayGhostingDataUse.empty() )
	{
		// 50 개만 생성하도록 한다 MagicNumber
		// 특별히 50 개에 제한을 둘 필요는 없다.
		// 하지만 너무 숫자가 크면 메모리를 많이 써서 적당히 쓰는게 좋다.
		// 50 개는 30fps 봤을 때 1.5초
		for ( DWORD i=0; i<50; ++i )
		{
			GhostingData* pNew = new GhostingData;
			pNew->m_arrayBoneMatrices = new D3DXMATRIXA16[m_dwBoneNum];
			m_listarrayGhostingDataBackUp.push_back( pNew );
		}
	}
}

const D3DXMATRIXA16* CharPartBoneMatrices::GetBoneMatricesForGhosting( size_t nIndex ) const
{
	if ( nIndex >= m_dequearrayGhostingDataUse.size() )
		return NULL;

	return m_dequearrayGhostingDataUse[m_dequearrayGhostingDataUse.size()-1-nIndex]->m_arrayBoneMatrices;
}

void CharPartBoneMatrices::SetPosOffset( const D3DXVECTOR3& vPosOffset )
{
	for ( DWORD i=0; i<m_dwBoneNum; ++i )
	{
		m_vecarrayBoneMatrices[m_dwUsedIndex][i]._41 += vPosOffset.x;
		m_vecarrayBoneMatrices[m_dwUsedIndex][i]._42 += vPosOffset.y;
		m_vecarrayBoneMatrices[m_dwUsedIndex][i]._43 += vPosOffset.z;
	}
}