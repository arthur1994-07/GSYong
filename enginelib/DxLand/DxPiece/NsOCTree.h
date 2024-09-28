// NsOcTree.h: interface for the CCollision class.
//
//	class DxAABBOctree : 정점 데이터와 면 갯수를 넣으면 Octree 형식의 최적화된 OptimizeMesh 를 만든다.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../../DxTools/Collision/CollisionDef.h"
#include "../../DxTools/DxCustomTypes.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../LightMap/LightMapDef.h"

class DxTexEffBase;
class DxTexEffFlowUV;
class DxTexEffRotate;
class AABBBinaryTreeBase;
class OBJOCTree;
class LightMapRender;
class DxDecal;
struct MATERIAL_DATA;

namespace sc
{
	class SerialFile;
};

namespace NSTexture16Bit
{
	struct DATA;
};

namespace NSOCTREE
{
    //--------------------------------------------------------------------------------------------------------------------
    // 공용
	void OneTimeSceneInit();
	void CleanUp();

	void EnableDynamicLoad();
	BOOL IsDynamicLoad();

	extern float g_fCOLL_LENGTH;


    //--------------------------------------------------------------------------------------------------------------------
    // OBJOCTree 관련
    void MakeVBIBAndDelMesh( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree );	// Tree 안에 Mesh를 삭제하고 VB, IB, Normal을 빼낸다.
	void DeleteNormalColor( OBJOCTree* pTree );										    // Normal을 삭제
	void CovertPieceSetting( LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, BOOL bUserLightMapUV );			// Mesh를 Piece에 맞게 셋팅해준다.
	//void InsertColorList( OBJOCTree* pTree, LOADINGDATALIST &listColorData );
	void RenderOctree_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree, DxTexEffFlowUV* pEff );
	void RenderOctree_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree, DxTexEffRotate* pEff );
    void RenderOctree_VisualMaterialColor( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree, DxTexEffBase* pEff, const D3DXMATRIX& matWorld );
	void RenderList_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, DxTexEffFlowUV* pEff );
	void RenderList_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, DxTexEffRotate* pEff );
    void RenderList_VisualMaterialColor( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, DxTexEffBase* pEff, BOOL bRealPL, const D3DXMATRIX& matWorld, float fAlpha );
	void AddNormalMesh( OBJOCTree* pTree );
    void ThreadLoad( OBJOCTree* pTree, const CLIPVOLUME &sCV, LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading );
    void GetCollisionLineInTriListExceptY( OBJOCTree* pTree, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ); // 주병진 추가메소드
	void GetVertexNum( AABBBinaryTreeBase* pTree, DWORD& dwVertexNum );
    void GetVertices( AABBBinaryTreeBase* pTree, std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV );
	void GetVertices( AABBBinaryTreeBase* pTree, std::vector<VERTEXNORCOLORTEX5>& vecVertices );
    void GetClosedPoint( OBJOCTree* pTree, const D3DXVECTOR3 vBasePos, D3DXVECTOR3& vCollisionOut, float& fDistance );
	void HaveLightMapUV( OBJOCTree* pTree, BOOL& bHaveLightMapUV );

    //--------------------------------------------------------------------------------------------------------------------
    // LightMapRender, LightMapRenderMat 관련
    void InitRenderOctree( LPDIRECT3DDEVICEQ pd3dDevice, 
                            const CLIPVOLUME* pCV, 
                            const D3DXMATRIX* pmatWorld, 
							DxTexEffBase* pEff,				// LightMapRender 에서만 쓰임
							const MATERIAL_DATA& sMaterial_Data,	// LightMapRenderMat 에서만 쓰임
                            LPDIRECT3DTEXTUREQ pTextureDay, 
                            LPDIRECT3DTEXTUREQ pTextureNight,
                            const D3DXVECTOR2& vLightMapUV_Offset,
                            float fAlpha,
							BOOL bShadowRender );			// LightMapRenderMat 에서만 쓰임

	void MakeMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree );

    void RenderOctree_VisualMaterial( AABBBinaryTreeBase* pTree );																			// LightMapRender
    void RenderOctree_VisualMaterial_NoneClipVolume( AABBBinaryTreeBase* pTree );															// LightMapRender
	void RenderMaterial( AABBBinaryTreeBase* pTree );																						// LightMapRenderMat
	void RenderPieceMaterial_Instancing( AABBBinaryTreeBase* pTree, 
										LPDIRECT3DDEVICEQ pd3dDevice, 
										ID3DXEffect* pFX, 
										const MATERIAL_DATA& sMaterial_Data );																// LightMapRenderMat
	void RenderMaterialWater( AABBBinaryTreeBase* pTree, float& fHeight, float& fNearLength );												// LightMapRenderMat
	void RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV, AABBBinaryTreeBase* pTree, BOOL bAlphaTex );				// LightMapRenderMat
	void RenderCascadeShadow_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree, BOOL bAlphaTex );							// LightMapRenderMat
	void SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate, AABBBinaryTreeBase* pTree );												// LightMapRenderMat

    //--------------------------------------------------------------------------------------------------------------------
    // AABBBinaryTreeBase 공용
    void MakeOcMesh ( const LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree, const DWORD dwFVF );
	void SetFVF ( LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree, DWORD dwFVF );
	void MakeOptimizeMesh ( const LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree, BOOL bIndexNoMove=FALSE );
	void MakeCollisionData ( const LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree );
	void MakeCollisionData2( LPD3DXMESH pMesh, DxAABBNode** pTree );
	void MakeCollisionData_VBIB( const LPDIRECT3DVERTEXBUFFERQ pVB, const LPDIRECT3DINDEXBUFFERQ pIB, DWORD dwFVF, DWORD dwFaceNUM, DxAABBNode** pTree );
	void Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, AABBBinaryTreeBase* pTree, BOOL bAlphaTex );
	void MakeLoadCheckOK( AABBBinaryTreeBase* pTree );
    void RenderOctree ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, AABBBinaryTreeBase* pTree );
    void RenderList( const LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree );
    void DivideFace ( const BYTE* pSrc, BYTE* pLeft, BYTE* pRight, DWORD dwFace, DWORD& dwLeft, DWORD& dwRight, const D3DXPLANE& sPlane, const UINT nSIZE );
	void RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree );
	void RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV, AABBBinaryTreeBase* pTree );

	BOOL IsCollisionLineSimple( AABBBinaryTreeBase* pTree, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode );

    void CollisionLine( AABBBinaryTreeBase* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
						BOOL& bColl, EMCOLLMODE emCullMode, float& fCOLL_LENGTH );

    // Note : 충돌이 되면 바로 리턴한다.
    BOOL IsCollisionLinePixel( AABBBinaryTreeBase* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, EMCOLLMODE emCullMode, const NSTexture16Bit::DATA* pData16Bit, int nIndexUV );

	BOOL IsCollisionPixel2( D3DXVECTOR3 vPos[], D3DXVECTOR2 vUV[], D3DXVECTOR3 &vCollision, LPDIRECT3DTEXTUREQ pTexture );

	BOOL GetLightMapColor( AABBBinaryTreeBase* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR2& vUV );

	void FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, AABBBinaryTreeBase* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );
	void DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, AABBBinaryTreeBase* pTree, const CLIPVOLUME &sCV );
    void SaveOctree ( sc::SerialFile& SFile, AABBBinaryTreeBase* pTree, BOOL bPiece );
	void LoadOctreeVER_100( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, OBJOCTree** ppTree );
	void LoadOctree_Prev( const LPDIRECT3DDEVICEQ pd3dDevice, 
						sc::SerialFile& SFile, 
						OBJOCTree** ppTree, 
						const BOOL bDynamicLoad, 
						BOOL bPiece );

	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree );
	VOID DestroyQuery( AABBBinaryTreeBase* pTree );
	void ResetOcclusionQuery( AABBBinaryTreeBase* pTree );
	void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, AABBBinaryTreeBase* pTree, VEC_DWORD& vecOctreeIndex );

	void ClipMesh( AABBBinaryTreeBase* pTree, DxDecal* pDecal );


    //--------------------------------------------------------------------------------------------------------------------
    // 템플릿
    extern float DISMAX;
	extern DWORD FACEMAX;

    template<class TYPE>
	void MakeOctree1( TYPE* pTree, const UINT nSIZE )
	{
		// Note : 이건 완료된 데이터
		float fDisX = pTree->GetMax().x - pTree->GetMin().x;
		float fDisY = pTree->GetMax().y - pTree->GetMin().y;
		float fDisZ = pTree->GetMax().z - pTree->GetMin().z;

		if ( (fDisX<DISMAX) && (fDisY<DISMAX) && (fDisZ<DISMAX) && (pTree->GetFaceNum()<FACEMAX) )
		{
			return;
		}

		// 순서 구하기
		VEC_DWORD vecDWORD;
		if ( (fDisX>fDisY) && (fDisX>fDisZ) )	// X 길다.
		{
			vecDWORD.push_back(0);
			if (fDisY>fDisZ)
			{
				vecDWORD.push_back(1);	
				vecDWORD.push_back(2);
			}
			else
			{
				vecDWORD.push_back(2);
				vecDWORD.push_back(1);
			}
		}
		else if (fDisY>fDisZ)									// Y 길다.
		{
			vecDWORD.push_back(1);
			if (fDisX>fDisZ)
			{
				vecDWORD.push_back(0);	
				vecDWORD.push_back(2);
			}
			else
			{
				vecDWORD.push_back(2);
				vecDWORD.push_back(0);
			}
		}								
		else 
		{
			vecDWORD.push_back(2);
			if (fDisX>fDisY)
			{
				vecDWORD.push_back(0);	
				vecDWORD.push_back(1);
			}
			else
			{
				vecDWORD.push_back(1);
				vecDWORD.push_back(0);
			}
		}

		// Note : 분할을 하자.
		DWORD	dwLeftFace = 0;
		DWORD	dwRightFace = 0;
		BYTE* pLeftVertex = new BYTE[nSIZE*pTree->GetFaceNum()*3];
		BYTE* pRightVertex = new BYTE[nSIZE*pTree->GetFaceNum()*3];

		D3DXPLANE sPlane;
		D3DXVECTOR3 vCenter = pTree->GetMin();
		vCenter.x += (fDisX*0.5f);
		vCenter.y += (fDisY*0.5f);
		vCenter.z += (fDisZ*0.5f);


		for ( DWORD i=0; i<vecDWORD.size(); ++i )
		{
			dwLeftFace = 0;
			dwRightFace = 0;

			D3DXVECTOR3	v0 = vCenter;
			D3DXVECTOR3	v1 = vCenter;
			D3DXVECTOR3	v2 = vCenter;

			switch ( vecDWORD[i] )
			{
			case 0:
				{
					v1.z += 1.f;
					v2.y += 1.f;
					D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );
				}
				break;
			case 1:
				{
					v1.x += 1.f;
					v2.z += 1.f;
					D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );	
				}
				break;
			case 2:
				{
					v1.x += 1.f;
					v2.y += 1.f;
					D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );
				}
				break;
			};

			DivideFace ( pTree->GetVertices(), pLeftVertex, pRightVertex, pTree->GetFaceNum(), dwLeftFace, dwRightFace, sPlane, nSIZE );

			if ( dwLeftFace && dwRightFace )
			{
				// 분할이 잘 되었다.
				break;
			}
		}

		// 분할이 안되었으면 밑으로 넘어가지 않는다.
		if ( !dwLeftFace || !dwRightFace )
		{
			if( pTree->GetFaceNum()>=FACEMAX )	MessageBox( NULL, "Static Mesh (Octree Action) pTree->dwFace>=FACEMAX ", "ERROR", MB_OK );

			SAFE_DELETE_ARRAY ( pLeftVertex );
			SAFE_DELETE_ARRAY ( pRightVertex );
			return;
		}

		// Note : 필요 없어진것 삭제
        pTree->DeleteVertexArray();
        pTree->SetFaceNum_Zero();

		// Note : Left 생성
		if ( dwLeftFace )
		{
			D3DXVECTOR3 vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
			D3DXVECTOR3 vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
			for ( DWORD i=0; i<dwLeftFace*3; ++i )
			{
				D3DXVECTOR3 *pVecter = (D3DXVECTOR3*)( pLeftVertex + nSIZE*i );

				if ( vMax.x < pVecter->x )	vMax.x = pVecter->x;
				if ( vMax.y < pVecter->y )	vMax.y = pVecter->y;
				if ( vMax.z < pVecter->z )	vMax.z = pVecter->z;

				if ( vMin.x > pVecter->x )	vMin.x = pVecter->x;
				if ( vMin.y > pVecter->y )	vMin.y = pVecter->y;
				if ( vMin.z > pVecter->z )	vMin.z = pVecter->z;
			}

			// 충돌을 위해서
			vMax += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
			vMin -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

			pTree->m_pLeftChild = new TYPE;
            pTree->m_pLeftChild->InitAABBBBinaryTreeBase(vMax,vMin,pLeftVertex,dwLeftFace);

			//pTree->m_pLeftChild->vMax = vMax;
			//pTree->m_pLeftChild->vMin = vMin;
			//pTree->m_pLeftChild->pVertex = pLeftVertex;
			//pTree->m_pLeftChild->dwFace = dwLeftFace;

            TYPE* pChild = MIN_BOOST::polymorphic_downcast<TYPE*>(pTree->m_pLeftChild);     // dynamic_cast 를 사용해야 하지만 속도 때문에 static_cast를 사용함.
			MakeOctree1 ( pChild, nSIZE );
		}
		else
		{
			SAFE_DELETE_ARRAY ( pLeftVertex );
		}

		// Note : Right 생성
		if ( dwRightFace )
		{
			D3DXVECTOR3 vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
			D3DXVECTOR3 vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
			for ( DWORD i=0; i<dwRightFace*3; ++i )
			{
				D3DXVECTOR3 *pVecter = (D3DXVECTOR3*)( pRightVertex + nSIZE*i );

				if ( vMax.x < pVecter->x )	vMax.x = pVecter->x;
				if ( vMax.y < pVecter->y )	vMax.y = pVecter->y;
				if ( vMax.z < pVecter->z )	vMax.z = pVecter->z;

				if ( vMin.x > pVecter->x )	vMin.x = pVecter->x;
				if ( vMin.y > pVecter->y )	vMin.y = pVecter->y;
				if ( vMin.z > pVecter->z )	vMin.z = pVecter->z;
			}

			// 충돌을 위해서
			vMax += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
			vMin -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

			pTree->m_pRightChild = new TYPE;
            pTree->m_pRightChild->InitAABBBBinaryTreeBase(vMax,vMin,pRightVertex,dwRightFace);

			//pTree->m_pRightChild->vMax = vMax;
			//pTree->m_pRightChild->vMin = vMin;
			//pTree->m_pRightChild->pVertex = pRightVertex;
			//pTree->m_pRightChild->dwFace = dwRightFace;

            TYPE* pChild = MIN_BOOST::polymorphic_downcast<TYPE*>(pTree->m_pRightChild);     // dynamic_cast 를 사용해야 하지만 속도 때문에 static_cast를 사용함.
			MakeOctree1 ( pChild, nSIZE );
		}
		else
		{
			SAFE_DELETE_ARRAY ( pRightVertex );
		}
	}

    template<class TYPE>
	void MakeOctree ( TYPE** ppTree, BYTE* pVertex, const DWORD dwFace, float fDisMAX, DWORD dwFaceMAX, const UINT nSIZE )
	{
		DISMAX = fDisMAX;
		FACEMAX = dwFaceMAX;

		D3DXVECTOR3 vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );

		for ( DWORD i=0; i<dwFace*3; ++i )
		{
			D3DXVECTOR3 *pVecter = (D3DXVECTOR3*)( pVertex + nSIZE*i );

			if ( vMax.x < pVecter->x )	vMax.x = pVecter->x;
			if ( vMax.y < pVecter->y )	vMax.y = pVecter->y;
			if ( vMax.z < pVecter->z )	vMax.z = pVecter->z;

			if ( vMin.x > pVecter->x )	vMin.x = pVecter->x;
			if ( vMin.y > pVecter->y )	vMin.y = pVecter->y;
			if ( vMin.z > pVecter->z )	vMin.z = pVecter->z;
		}

		// 충돌을 위해서
		vMax += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		vMin -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

        BYTE* pVerticesDest = new BYTE[nSIZE*dwFace*3];
        memcpy( pVerticesDest, pVertex, nSIZE*dwFace*3 );

		(*ppTree) = new TYPE;
        (*ppTree)->InitAABBBBinaryTreeBase(vMax,vMin,pVerticesDest,dwFace);

		//(*ppTree)->vMax = vMax;
		//(*ppTree)->vMin = vMin;
		//(*ppTree)->pVertex = new BYTE[nSIZE*dwFace*3];
		//memcpy ( (*ppTree)->pVertex, pVertex, nSIZE*dwFace*3 );
		//(*ppTree)->dwFace = dwFace;

		MakeOctree1 ( (*ppTree), nSIZE );
	}

 //       template<class TYPE>
	//void LoadOctree ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, TYPE** ppTree, const BOOL bDynamicLoad, BOOL bPiece, DWORD dwType )
	//{
	//	BOOL bUse = TRUE;
	//	SFile >> bUse;

	//	if ( bUse )
	//	{
	//		(*ppTree) = new TYPE;
	//		(*ppTree)->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
	//	}
	//	else
	//	{
	//		return;
	//	}

 //       TYPE* pLeftChild = static_cast<TYPE*>( (*ppTree)->m_pLeftChild );   // dynamic_cast 로 해야 하지만 속도상 static_cast 로 변경함.
 //       TYPE* pRightChild = static_cast<TYPE*>( (*ppTree)->m_pRightChild ); // dynamic_cast 로 해야 하지만 속도상 static_cast 로 변경함.
	//	LoadOctree( pd3dDevice, SFile, &pLeftChild, bDynamicLoad, bPiece, dwType );
	//	LoadOctree( pd3dDevice, SFile, &pRightChild, bDynamicLoad, bPiece, dwType );
	//}

    template<class TYPE>
	void LoadOctree ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, TYPE** ppTree, const BOOL bDynamicLoad, BOOL bPiece, DWORD dwType )
	{
		BOOL bUse = TRUE;
		SFile >> bUse;

		if ( bUse )
		{
			(*ppTree) = new TYPE;
			(*ppTree)->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
		else
		{
			return;
		}

		LoadOctree2( pd3dDevice, SFile, &(*ppTree)->m_pLeftChild, bDynamicLoad, bPiece, dwType );
		LoadOctree2( pd3dDevice, SFile, &(*ppTree)->m_pRightChild, bDynamicLoad, bPiece, dwType );
	}

    void LoadOctree2( const LPDIRECT3DDEVICEQ pd3dDevice, 
                        sc::SerialFile& SFile, 
                        AABBBinaryTreeBase** ppTree, 
                        const BOOL bDynamicLoad, 
                        BOOL bPiece, 
                        DWORD dwType ); // emType - AABB_BINARY_TREE_TYPE


	// ---------------------------------------- LoadOctree 의 이전버젼
	template<class TYPE>
	void LoadOctree_Prev( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, TYPE** ppTree, const BOOL bDynamicLoad, BOOL bPiece, DWORD dwType )
	{
		BOOL bUse = TRUE;
		SFile >> bUse;

		if ( bUse )
		{
			(*ppTree) = new TYPE;
			(*ppTree)->Load_Prev( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
		else
		{
			return;
		}

		LoadOctree_Prev2( pd3dDevice, SFile, &(*ppTree)->m_pLeftChild, bDynamicLoad, bPiece, dwType );
		LoadOctree_Prev2( pd3dDevice, SFile, &(*ppTree)->m_pRightChild, bDynamicLoad, bPiece, dwType );
	}

	void LoadOctree_Prev2( const LPDIRECT3DDEVICEQ pd3dDevice, 
		sc::SerialFile& SFile, 
		AABBBinaryTreeBase** ppTree, 
		const BOOL bDynamicLoad, 
		BOOL bPiece, 
		DWORD dwType ); // emType - AABB_BINARY_TREE_TYPE
}