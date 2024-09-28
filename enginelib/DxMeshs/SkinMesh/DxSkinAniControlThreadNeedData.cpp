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

	// [shhan][2014.11.25] Skeleton �� ���� ��� ���� ���� �۾��ϵ��� �Ѵ�.
	//						�������� ������ �� �ڽ��� ������ Part�� 1frame �Ⱥ��̴� ���� �����ϱ� ����. ( Mesh �� ������ Matrix ���� ��� Matrix ���� �־���. )
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
						//	continue;	// �̰��� ���� ������ �ȵȴ�.

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
	// Bone Matrices ���
	// Back ���� �۾��� �Ǿ� �ִ�. DxSkinAniControl::SwapBone ���̱� ������ �׷���.
	CaculateBoneMatrices( *m_rMeshContainer, m_vecarrayBoneMatrices[m_dwCalculateIndex], *m_spSkinAniControlThread->GetSkeletonMaintainBackUp() );
}

const D3DXMATRIXA16* CharPartBoneMatrices::GetBoneMatrices() const
{
	// [shhan][2014.08.08] ���⼭ Error �� �߸� Ȥ�� DxCharPart::SetPart �Ŀ� DxCharPart::FrameMove �� �ѹ��̶� ȣ���� �Ǵ��� Ȯ���غ��� �Ұ���.
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
			// Key �� ����Ǿ����Ƿ� �۾��� ���ش�.
			if ( m_nPrevKeyTimeForGhosting != m_spSkinAniControlThread->GETCURKEYTIME() )
			{
				m_nPrevKeyTimeForGhosting = m_spSkinAniControlThread->GETCURKEYTIME();

				if ( !m_listarrayGhostingDataBackUp.empty() )
				{
					// �����п��� �����´�.
					std::list<GhostingData*>::iterator iter = m_listarrayGhostingDataBackUp.begin();

					(*iter)->m_fLifeTime = m_fLifeTimeForGhosting;
					D3DXMATRIXA16* pTemp = m_vecarrayBoneMatrices[m_dwCalculateIndex];			// ������ ���Ǿ��� ���� �����Ѵ�.
					m_vecarrayBoneMatrices[m_dwCalculateIndex] = (*iter)->m_arrayBoneMatrices;
					(*iter)->m_arrayBoneMatrices = pTemp;

					m_dequearrayGhostingDataUse.push_back( (*iter) );
					m_listarrayGhostingDataBackUp.erase( iter );
				}
				else if ( !m_dequearrayGhostingDataUse.empty() )
				{
					// �������� �����Ƿ� ������ΰ��� ����.
					std::deque<GhostingData*>::iterator iter = m_dequearrayGhostingDataUse.begin();

					(*iter)->m_fLifeTime = m_fLifeTimeForGhosting;
					D3DXMATRIXA16* pTemp = m_vecarrayBoneMatrices[m_dwCalculateIndex];			// ������ ���Ǿ��� ���� �����Ѵ�.
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
		// 50 ���� �����ϵ��� �Ѵ� MagicNumber
		// Ư���� 50 ���� ������ �� �ʿ�� ����.
		// ������ �ʹ� ���ڰ� ũ�� �޸𸮸� ���� �Ἥ ������ ���°� ����.
		// 50 ���� 30fps ���� �� 1.5��
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