//////////////////////////////////////////////////////////////////////////
// [shhan][2015.02.04]	���� - Field �� �ִ� Mesh (AABBBinaryTreeBase) ���� 
//							Thread �� ������ ���ؼ� ����.
//						
#pragma once

#include <vector>
#include <boost/tr1/memory.hpp>

#include "../DxTools/DxCustomTypes.h"
#include "./NSLandThreadDef.h"

struct MATERIAL_DATA;
class AABBBinaryTreeBase;
class DxTexEffBase;



typedef std::vector<const AABBBinaryTreeBase*>	VEC_AABB_BINARY_TREE_MESHS;





//////////////////////////////////////////////////////////////////////////
//	DxSingleMaterialMesh, 2, 3 ���� ������ �ִ�.
class DxCullingMeshSwap
{
private:
	int		m_nUSED;	// Render �Ǿ����� ��ȣ

	const AABBBinaryTreeBase*	m_rLightMapTree;		// ������ �Ǵ� ���ҵ� Mesh ����

	VEC_AABB_BINARY_TREE_MESHS	m_vecLightMapTree[2];		// Swap �Ǵ� Data	// �ϳ��� m_rvecLightMapTreeUSED �� ����ǰ�, �ϳ��� Thread �󿡼� push_back �۾��� �ȴ�.

public:
	void Swap( int nIndex );

	// ����
public:
	void RenderOctree( LPDIRECT3DDEVICEQ pd3dDevice );

	void RenderOctreeEff( LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffBase* pEff );

	void RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
								DxTexEffBase* pEff, 
								const D3DXMATRIX& matWorld, 
								const D3DXVECTOR2& vLightMapUV_Offset,
								float fAlpha,
								LPDIRECT3DTEXTUREQ pTextureDay, 
								LPDIRECT3DTEXTUREQ pTextureNight );

	void RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
								DxTexEffBase* pEff, 
								const D3DXMATRIX& matWorld );

	// Material System
public:
	void RenderMaterialTnL( LPDIRECT3DDEVICEQ pd3dDevice );

	void RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice, const MATERIAL_DATA& sMaterial_Data );

	void RenderMaterialWater( LPDIRECT3DDEVICEQ pd3dDevice,
								const MATERIAL_DATA& sMaterial_Data,
								float& fHeight,
								float& fNearLength );

	void RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex );

	BOOL IsExistCullingData();


	//////////////////////////////////////////////////////////////////////////
	// Thread �󿡼� �Ͼ.
public:
	void CalculateTHREAD( int nIndex, const CLIPVOLUME& sCV );
	void NoCalculateTHREAD( int nIndex );

private:
	void InsertCullingMesh( const AABBBinaryTreeBase* pTree, const CLIPVOLUME& sCV, VEC_AABB_BINARY_TREE_MESHS& vecLightMapTreeOUT );
	// Thread �󿡼� �Ͼ
	//////////////////////////////////////////////////////////////////////////.



public:
	DxCullingMeshSwap( const AABBBinaryTreeBase* pLightMapTree );
	~DxCullingMeshSwap();
};





//////////////////////////////////////////////////////////////////////////
//		Thread �󿡼� ����ϱ� ���� ��Ƴ��� DxCullingMeshForThread
class DxFieldMeshForThreadPackage
{
private:
	BOOL		m_bEnable;	// Ȱ��ȭ �Ǿ����� Ȯ���Ѵ�. FALSE ��� CV �� �̿��� Mesh �������� ������ �ʴ´�.
	CLIPVOLUME  m_sCV;

	// ��� Field Mesh �� ������ �� �� �ִ�.
	// DxCullingMeshForThread�� m_rLightMapTree �� ���� ��� ������ �� �� �ִ�. ( Tree �� �� �� �� �ִ�. )
	// ��� DxSingleMaterialMesh, 2, 3 �� ����.
	// ������ �������̴�.
	std::vector<boost::shared_ptr<DxCullingMeshSwap>>	m_vecFieldMeshAll;
	

public:
	// ���� �ε��� �� ����Mesh ���� �ѹ��� ȣ���Ѵ�.
	void AddCullingMesh( boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread );

	// ������ �����Ǹ� ȣ��
	void CleanUp();

	// �� ������ ȣ��ȴ�.
	void Swap();

	// �������� Culling �۾��� ���� �ʼ� Data ���� ������ ���´�.
	void InsertNeedData( const CLIPVOLUME* pCV );


	//////////////////////////////////////////////////////////////////////////
	// Thread �󿡼� ����.
private:
	int m_nUSED;
	int m_nBackUp;

public:
	void CalculateTHREAD();
	// Thread �󿡼� �Ͼ
	//////////////////////////////////////////////////////////////////////////


public:
	DxFieldMeshForThreadPackage();
	~DxFieldMeshForThreadPackage();
};