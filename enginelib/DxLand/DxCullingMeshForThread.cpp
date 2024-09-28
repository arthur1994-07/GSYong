#include "pch.h"

#include "../DxTools/Collision.h"
#include "./AABBBinaryTree/AABBBinaryTreeBase.h"
#include "./DxPiece/OBJOCTree.h"

#include "DxCullingMeshForThread.h"






//////////////////////////////////////////////////////////////////////////
//						DxCullingMeshForThread
//////////////////////////////////////////////////////////////////////////
DxCullingMeshSwap::DxCullingMeshSwap( const AABBBinaryTreeBase* pLightMapTree )
	: m_rLightMapTree( pLightMapTree )
	, m_nUSED(0)
{
}

DxCullingMeshSwap::~DxCullingMeshSwap()
{
}

void DxCullingMeshSwap::Swap( int nIndex )
{
	m_nUSED = nIndex;
}

void DxCullingMeshSwap::RenderOctree( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecLightMapTree[m_nUSED].size(); ++i )
	{
		// const Ǯ�� ���� ������ ��������� �� �˷�����.
		// �̰������� ����ȴٴ� �ǹ̷� ������ �� �ϴ�.
		AABBBinaryTreeBase* pCullMesh = const_cast<AABBBinaryTreeBase*>(m_vecLightMapTree[m_nUSED][i]);
		pCullMesh->DrawEff( pd3dDevice );
	}
}

void DxCullingMeshSwap::RenderOctreeEff( LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffBase* pEff )
{
	for ( DWORD i=0; i<m_vecLightMapTree[m_nUSED].size(); ++i )
	{
		// const Ǯ�� ���� ������ ��������� �� �˷�����.
		// �̰������� ����ȴٴ� �ǹ̷� ������ �� �ϴ�.
		AABBBinaryTreeBase* pCullMesh = const_cast<AABBBinaryTreeBase*>(m_vecLightMapTree[m_nUSED][i]);
		pCullMesh->RenderOctreeEff( pd3dDevice, pEff );
	}
}

void DxCullingMeshSwap::RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
											  DxTexEffBase* pEff, 
											  const D3DXMATRIX& matWorld, 
											  const D3DXVECTOR2& vLightMapUV_Offset,
											  float fAlpha,
											  LPDIRECT3DTEXTUREQ pTextureDay, 
											  LPDIRECT3DTEXTUREQ pTextureNight )
{
	for ( DWORD i=0; i<m_vecLightMapTree[m_nUSED].size(); ++i )
	{
		// const Ǯ�� ���� ������ ��������� �� �˷�����.
		// �̰������� ����ȴٴ� �ǹ̷� ������ �� �ϴ�.
		AABBBinaryTreeBase* pCullMesh = const_cast<AABBBinaryTreeBase*>(m_vecLightMapTree[m_nUSED][i]);
		if ( pCullMesh->IsLoad() )
		{
			pCullMesh->RenderVisualMaterial
			( 
				pd3dDevice, 
				pEff, 
				matWorld, 
				vLightMapUV_Offset,
				fAlpha,
				pTextureDay, 
				pTextureNight
			);
		}
	}
}

void DxCullingMeshSwap::RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
											 DxTexEffBase* pEff, 
											 const D3DXMATRIX& matWorld )
{
	for ( DWORD i=0; i<m_vecLightMapTree[m_nUSED].size(); ++i )
	{
		// const Ǯ�� ���� ������ ��������� �� �˷�����.
		// �̰������� ����ȴٴ� �ǹ̷� ������ �� �ϴ�.
		AABBBinaryTreeBase* pCullMesh = const_cast<AABBBinaryTreeBase*>(m_vecLightMapTree[m_nUSED][i]);
		OBJOCTree* pOBJOCTree = dynamic_cast<OBJOCTree*>(pCullMesh);

		if ( pOBJOCTree && pOBJOCTree->IsLoad() )
		{
			if ( !pOBJOCTree->m_spTangentMesh.get() )
			{
				pOBJOCTree->CreateTangentColorMesh( pd3dDevice );
			}
			pOBJOCTree->RenderVisualMaterial_Color( pd3dDevice, pEff, FALSE, matWorld, 1.f );
		}
	}
}

void DxCullingMeshSwap::RenderMaterialTnL( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecLightMapTree[m_nUSED].size(); ++i )
	{
		m_vecLightMapTree[m_nUSED][i]->Render( pd3dDevice );
	}
}

void DxCullingMeshSwap::RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice, const MATERIAL_DATA& sMaterial_Data )
{
	for ( DWORD i=0; i<m_vecLightMapTree[m_nUSED].size(); ++i )
	{
		// const Ǯ�� ���� ������ ��������� �� �˷�����.
		// �̰������� ����ȴٴ� �ǹ̷� ������ �� �ϴ�.
		AABBBinaryTreeBase* pCullMesh = const_cast<AABBBinaryTreeBase*>(m_vecLightMapTree[m_nUSED][i]);
		pCullMesh->RenderMaterial( pd3dDevice, sMaterial_Data );
	}
}

void DxCullingMeshSwap::RenderMaterialWater( LPDIRECT3DDEVICEQ pd3dDevice,
												const MATERIAL_DATA& sMaterial_Data,
												float& fHeight,
												float& fNearLength )
{
	for ( DWORD i=0; i<m_vecLightMapTree[m_nUSED].size(); ++i )
	{
		// const Ǯ�� ���� ������ ��������� �� �˷�����.
		// �̰������� ����ȴٴ� �ǹ̷� ������ �� �ϴ�.
		AABBBinaryTreeBase* pCullMesh = const_cast<AABBBinaryTreeBase*>(m_vecLightMapTree[m_nUSED][i]);
		pCullMesh->RenderMaterialWater( pd3dDevice, sMaterial_Data, fHeight, fNearLength );
	}
}

void DxCullingMeshSwap::RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex )
{
	for ( DWORD i=0; i<m_vecLightMapTree[m_nUSED].size(); ++i )
	{
		// const Ǯ�� ���� ������ ��������� �� �˷�����.
		// �̰������� ����ȴٴ� �ǹ̷� ������ �� �ϴ�.
		AABBBinaryTreeBase* pCullMesh = const_cast<AABBBinaryTreeBase*>(m_vecLightMapTree[m_nUSED][i]);
		pCullMesh->RenderCascadeShadow( pd3dDevice, bAlphaTex );
	}
}

BOOL DxCullingMeshSwap::IsExistCullingData()
{
	return m_vecLightMapTree[m_nUSED].empty() ? FALSE : TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Thread �󿡼� ����.
void DxCullingMeshSwap::CalculateTHREAD( int nIndex, const CLIPVOLUME& sCV )
{
	m_vecLightMapTree[nIndex].clear();

	// ������ ������ �͵鸸 m_vecLightMapTree[nIndex] �� �㵵�� �Ѵ�.
	InsertCullingMesh( m_rLightMapTree, sCV, m_vecLightMapTree[nIndex] );
}

void DxCullingMeshSwap::NoCalculateTHREAD( int nIndex )
{
	m_vecLightMapTree[nIndex].clear();
}

void DxCullingMeshSwap::InsertCullingMesh( const AABBBinaryTreeBase* pTree, const CLIPVOLUME& sCV, VEC_AABB_BINARY_TREE_MESHS& vecLightMapTreeOUT )
{
	if ( !pTree )	
		return;

	if ( !COLLISION::IsCollisionVolume ( sCV, pTree->GetMax(), pTree->GetMin() ) )	
		return;

	//if ( pTree->IsLoad() )
	// ���� ��ȿ�� ���� CollisionMesh �θ� �� �� �ִ�.
	if ( pTree->GetCollisionTree() )
	{
		vecLightMapTreeOUT.push_back( pTree );
		return;
	}

	if ( pTree->m_pLeftChild )	
		InsertCullingMesh( pTree->m_pLeftChild, sCV, vecLightMapTreeOUT );

	if ( pTree->m_pRightChild )	
		InsertCullingMesh( pTree->m_pRightChild, sCV, vecLightMapTreeOUT );
}
// Thread �󿡼� ����.
//////////////////////////////////////////////////////////////////////////













//////////////////////////////////////////////////////////////////////////
//						DxFieldMeshForThreadPackage
//////////////////////////////////////////////////////////////////////////
DxFieldMeshForThreadPackage::DxFieldMeshForThreadPackage()
	: m_nUSED(0)
	, m_nBackUp(1)
	, m_bEnable(FALSE)
{
}

DxFieldMeshForThreadPackage::~DxFieldMeshForThreadPackage()
{
}

// ���� �ε��� �� ����Mesh ���� �ѹ��� ȣ���Ѵ�.
void DxFieldMeshForThreadPackage::AddCullingMesh( boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread )
{
	m_vecFieldMeshAll.push_back( spCullingMeshForThread );
}

// ������ �����Ǹ� ȣ��
void DxFieldMeshForThreadPackage::CleanUp()
{
	m_vecFieldMeshAll.clear();
}

// �� ������ ȣ��ȴ�.
void DxFieldMeshForThreadPackage::Swap()
{
	if ( m_nUSED == 0 )
	{
		m_nUSED = 1;
		m_nBackUp = 0;
	}
	else
	{
		m_nUSED = 0;
		m_nBackUp = 1;
	}

	for ( DWORD i=0; i<m_vecFieldMeshAll.size(); ++i )
	{
		m_vecFieldMeshAll[i]->Swap( m_nUSED );
	}
}

// Culling �۾��� ���� �ʼ� Data ���� ������ ���´�.
void DxFieldMeshForThreadPackage::InsertNeedData( const CLIPVOLUME* pCV )
{
	if ( pCV )
	{
		m_sCV = *pCV;
		m_bEnable = TRUE;
	}
	else
	{
		m_bEnable = FALSE;
	}
}


//////////////////////////////////////////////////////////////////////////
// Thread �󿡼� ����.
void DxFieldMeshForThreadPackage::CalculateTHREAD()
{
	if ( m_bEnable )
	{
		for ( DWORD i=0; i<m_vecFieldMeshAll.size(); ++i )
		{
			m_vecFieldMeshAll[i]->CalculateTHREAD( m_nBackUp, m_sCV );
		}
	}
	else
	{
		for ( DWORD i=0; i<m_vecFieldMeshAll.size(); ++i )
		{
			m_vecFieldMeshAll[i]->NoCalculateTHREAD( m_nBackUp );
		}
	}
}
// Thread �󿡼� ����.
//////////////////////////////////////////////////////////////////////////