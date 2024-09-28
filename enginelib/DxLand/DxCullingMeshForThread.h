//////////////////////////////////////////////////////////////////////////
// [shhan][2015.02.04]	목적 - Field 에 있는 Mesh (AABBBinaryTreeBase) 들을 
//							Thread 상 돌리기 위해서 사용됨.
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
//	DxSingleMaterialMesh, 2, 3 에서 가지고 있다.
class DxCullingMeshSwap
{
private:
	int		m_nUSED;	// Render 되어지는 번호

	const AABBBinaryTreeBase*	m_rLightMapTree;		// 원본이 되는 분할된 Mesh 정보

	VEC_AABB_BINARY_TREE_MESHS	m_vecLightMapTree[2];		// Swap 되는 Data	// 하나는 m_rvecLightMapTreeUSED 에 연결되고, 하나는 Thread 상에서 push_back 작업이 된다.

public:
	void Swap( int nIndex );

	// 구형
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
	// Thread 상에서 일어남.
public:
	void CalculateTHREAD( int nIndex, const CLIPVOLUME& sCV );
	void NoCalculateTHREAD( int nIndex );

private:
	void InsertCullingMesh( const AABBBinaryTreeBase* pTree, const CLIPVOLUME& sCV, VEC_AABB_BINARY_TREE_MESHS& vecLightMapTreeOUT );
	// Thread 상에서 일어남
	//////////////////////////////////////////////////////////////////////////.



public:
	DxCullingMeshSwap( const AABBBinaryTreeBase* pLightMapTree );
	~DxCullingMeshSwap();
};





//////////////////////////////////////////////////////////////////////////
//		Thread 상에서 사용하기 위해 모아놓은 DxCullingMeshForThread
class DxFieldMeshForThreadPackage
{
private:
	BOOL		m_bEnable;	// 활성화 되었는지 확인한다. FALSE 라면 CV 를 이용해 Mesh 정보들을 모으지 않는다.
	CLIPVOLUME  m_sCV;

	// 모든 Field Mesh 의 정보를 알 수 있다.
	// DxCullingMeshForThread의 m_rLightMapTree 로 인해 모든 정보를 알 수 있다. ( Tree 라 다 알 수 있다. )
	// 모든 DxSingleMaterialMesh, 2, 3 의 모임.
	// 순수한 참조값이다.
	std::vector<boost::shared_ptr<DxCullingMeshSwap>>	m_vecFieldMeshAll;
	

public:
	// 지형 로딩시 각 단위Mesh 마다 한번씩 호출한다.
	void AddCullingMesh( boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread );

	// 지형이 삭제되면 호출
	void CleanUp();

	// 매 프레임 호출된다.
	void Swap();

	// 매프레임 Culling 작업을 위한 필수 Data 들을 저장해 놓는다.
	void InsertNeedData( const CLIPVOLUME* pCV );


	//////////////////////////////////////////////////////////////////////////
	// Thread 상에서 사용됨.
private:
	int m_nUSED;
	int m_nBackUp;

public:
	void CalculateTHREAD();
	// Thread 상에서 일어남
	//////////////////////////////////////////////////////////////////////////


public:
	DxFieldMeshForThreadPackage();
	~DxFieldMeshForThreadPackage();
};