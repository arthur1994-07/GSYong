#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Lock.h"

#include "../../DxEffect/TexEff/DxTexEffFlowUV.h"
#include "../../DxEffect/TexEff/DxTexEffRotate.h"
#include "../../DxTools/NsOptimizeMesh.h"
#include "../../DxTools/Material/MaterialFieldDef.h"
#include "../../DxTools/Texture/NSTexture16Bit.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxTools/Collision.h"
#include "../../DxMeshs/DxDecal.h"
#include "../AABBBinaryTree/AABBBinaryTreeBase.h"
#include "../LightMap/LightMapRender.h"
#include "./NSOctreeRenderList.h"
#include "./OBJOCTree.h"

#include "NsOCTree.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSOCTREE
{
	BOOL				g_bDynamicLoad = FALSE;	// Tool 에 따라 변함. Game 사용, Level Tool 미 사용.

	void OneTimeSceneInit()
	{
	}

	void CleanUp()
	{
	}

	BOOL IsDynamicLoad()
	{
		return g_bDynamicLoad;
	}

	void EnableDynamicLoad()
	{
		g_bDynamicLoad = TRUE; 
	}

	BOOL IsFaceFront ( const BYTE* pSrc, const D3DXPLANE& sPlane, const UINT nSIZE )
	{
		float fResult;
		D3DXVECTOR3 *pVecter;
		for ( DWORD i=0; i<3; ++i )
		{
			pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*i );
			fResult = D3DXPlaneDotCoord ( &sPlane, pVecter );
			if ( fResult >= 0.f )	return TRUE;
		}
		return FALSE;
	}

	void DivideFace ( const BYTE* pSrc, BYTE* pLeft, BYTE* pRight, DWORD dwFace, DWORD& dwLeft, DWORD& dwRight, const D3DXPLANE& sPlane, const UINT nSIZE )
	{
		D3DXVECTOR3 *pVecter;
		D3DXVECTOR3 *pDest;
		for ( DWORD i=0; i<dwFace; ++i )
		{
			// Note : 면이 앞에 있다.		 면 단위 계산
			if ( IsFaceFront ( &pSrc[nSIZE*i*3], sPlane, nSIZE ) )
			{
				pDest = (D3DXVECTOR3*)( pLeft + nSIZE*(dwLeft*3+0) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+0) );
				*pDest = *pVecter;

				pDest = (D3DXVECTOR3*)( pLeft + nSIZE*(dwLeft*3+1) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+1) );
				*pDest = *pVecter;

				pDest = (D3DXVECTOR3*)( pLeft + nSIZE*(dwLeft*3+2) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+2) );
				*pDest = *pVecter;

				memcpy ( pLeft + (nSIZE*3)*dwLeft, pSrc + (nSIZE*3)*i, nSIZE*3 );

				++dwLeft;
			}
			else
			{
				pDest = (D3DXVECTOR3*)( pRight + nSIZE*(dwRight*3+0) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+0) );
				*pDest = *pVecter;

				pDest = (D3DXVECTOR3*)( pRight + nSIZE*(dwRight*3+1) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+1) );
				*pDest = *pVecter;

				pDest = (D3DXVECTOR3*)( pRight + nSIZE*(dwRight*3+2) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+2) );
				*pDest = *pVecter;

				memcpy ( pRight + (nSIZE*3)*dwRight, pSrc + (nSIZE*3)*i, nSIZE*3 );

				++dwRight;
			}
		}
	}

	float DISMAX = 0.f;
	DWORD FACEMAX = 0;


    // 내부에서 쓰이는 데이터 끈
    //--------------------------------------------------------------------------------------------------------------------



	void MakeVBIBAndDelMesh( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree )
	{
		if( !pTree )	return;

        LPD3DXMESH* ppOcMesh = pTree->GetppOcMesh();

		// Note : VertexBuffer, IndexBuffer, pNormal을 생성하고 Mesh는 삭제한다.
		//		Mesh는 Normal을 포함하고 있는 경우일 것이다. 분해 후 넣어야 한다.
		if( (*ppOcMesh) )
		{
			// Note : FVF를 얻는다.
			DWORD dwFVF = (*ppOcMesh)->GetFVF();
			DWORD dwVert = (*ppOcMesh)->GetNumVertices();
			DWORD dwFaces = (*ppOcMesh)->GetNumFaces();

			// Note : 기본적인 VB, IB, DIRECTPOINTCOLOR, 등을 생성한다.
			pTree->CreateBASE( pd3dDevice, dwVert, dwFaces );

			// Note : 데이터를 옮긴다.
			BYTE*	pVertices(NULL);
			(*ppOcMesh)->LockVertexBuffer( 0L, (VOID**)&pVertices );
			pTree->CreateVertSrc( pVertices, dwFVF, dwVert );				// VertSrc를 생성한다.
			pTree->CreateNormalColor( pVertices, dwFVF, dwVert );			// NormalColor를 생성한다.

			// Note : 렌더에 쓰일 VB를 셋팅한다.
			pTree->CopyVertSrcToVB( pVertices, pTree->m_pVertSrc, pTree->m_pVB, dwVert, dwFVF );
			(*ppOcMesh)->UnlockVertexBuffer();

			// Note : 렌더에 쓰일 IB를 셋팅한다.
			WORD* pIndexSRC(NULL);
			(*ppOcMesh)->LockIndexBuffer( 0L, (VOID**)&pIndexSRC );
			pTree->CopyMeshToIB( pIndexSRC, pTree->m_pIB, dwFaces );
			(*ppOcMesh)->UnlockIndexBuffer();

			// Note : 마지막으로 삭제한다.
			SAFE_RELEASE( (*ppOcMesh) );

			return;
		}

		if( pTree->m_pLeftChild )	
            MakeVBIBAndDelMesh( pd3dDevice, pTree->GetLeftChild_OBJOCTree() );
		if( pTree->m_pRightChild )	
            MakeVBIBAndDelMesh( pd3dDevice, pTree->GetRightChild_OBJOCTree() );
	}

	void DeleteNormalColor( OBJOCTree* pTree )
	{
		if( !pTree )	return;

		SAFE_DELETE_ARRAY( pTree->m_pColor );
		
		if( pTree->m_pLeftChild )	DeleteNormalColor( pTree->GetLeftChild_OBJOCTree() );
		if( pTree->m_pRightChild )	DeleteNormalColor( dynamic_cast<OBJOCTree*>( pTree->m_pRightChild  ));
	}

	void CovertPieceSetting( LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, BOOL bUserLightMapUV )
	{
		if( !pTree )	return;

		pTree->CovertPieceSetting( pd3dDevice, bUserLightMapUV );
		
		if( pTree->m_pLeftChild )	CovertPieceSetting( pd3dDevice, pTree->GetLeftChild_OBJOCTree(), bUserLightMapUV );
		if( pTree->m_pRightChild )	CovertPieceSetting( pd3dDevice, pTree->GetRightChild_OBJOCTree(), bUserLightMapUV );
	}

	//void InsertColorList( OBJOCTree* pTree, LOADINGDATALIST& listColorData )
	//{
	//	if (!pTree)
 //           return;

	//	pTree->InsertColorList(listColorData);

	//	if( pTree->m_pLeftChild )	
	//		InsertColorList( pTree->GetLeftChild_OBJOCTree(), listColorData );

	//	if( pTree->m_pRightChild )	
	//		InsertColorList( pTree->GetRightChild_OBJOCTree(), listColorData );
	//}

    //LPD3DXMESH* GetppOcMesh();

	void MakeOcMesh ( const LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree, const DWORD dwFVF )
	{
		if ( pTree->GetVertices() )
		{
            LPD3DXMESH* ppOcMesh = pTree->GetppOcMesh();

			SAFE_RELEASE ( (*ppOcMesh) );
			D3DXCreateMeshFVF ( pTree->GetFaceNum(), pTree->GetFaceNum()*3, D3DXMESH_MANAGED, dwFVF, pd3dDevice, &(*ppOcMesh) );
			if ( !(*ppOcMesh) )	return;

			if ( !pTree->GetVertices() )	return;

			UINT nSIZE = D3DXGetFVFVertexSize ( dwFVF );

			BYTE *pBuffer(NULL);
			(*ppOcMesh)->LockVertexBuffer ( 0L, (VOID**)&pBuffer );
			memcpy ( pBuffer, pTree->GetVertices(), nSIZE*pTree->GetFaceNum()*3 );
			(*ppOcMesh)->UnlockVertexBuffer();

			WORD* pIndices;
			(*ppOcMesh)->LockIndexBuffer ( 0L, (VOID**)&pIndices );
			for ( DWORD i=0; i<pTree->GetFaceNum()*3; ++i )
			{
				pIndices[i] = (WORD)i;
			}
			(*ppOcMesh)->UnlockIndexBuffer();

            pTree->DeleteVertexArray();
			//SAFE_DELETE_ARRAY ( pTree->GetVertices() );
			//pTree->GetFaceNum() = 0;

			return;
		}

		if ( pTree->m_pLeftChild )	MakeOcMesh ( pd3dDevice, pTree->m_pLeftChild, dwFVF );
		if ( pTree->m_pRightChild )	MakeOcMesh ( pd3dDevice, pTree->m_pRightChild, dwFVF );
	}

	// Note : 원하는 FVF 로 변경
	void SetFVF ( LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree, DWORD dwFVF )
	{
		pTree->SetFVF( pd3dDevice, dwFVF );

		if ( pTree->m_pLeftChild )	SetFVF ( pd3dDevice, pTree->m_pLeftChild, dwFVF );
		if ( pTree->m_pRightChild )	SetFVF ( pd3dDevice, pTree->m_pRightChild, dwFVF );
	}

	void MakeOptimizeMesh ( const LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree, BOOL bIndexNoMove )
	{
        LPD3DXMESH* ppOcMesh = pTree->GetppOcMesh();

		if ( (*ppOcMesh) )
		{
			// Note : 메쉬의 최적화
			NSOPTIMIZEMESH::MakeOptimizeMesh( pd3dDevice, (*ppOcMesh), bIndexNoMove );
			return;
		}

		if ( pTree->m_pLeftChild )	MakeOptimizeMesh ( pd3dDevice, pTree->m_pLeftChild, bIndexNoMove );
		if ( pTree->m_pRightChild )	MakeOptimizeMesh ( pd3dDevice, pTree->m_pRightChild, bIndexNoMove );
	}

	void MakeMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree )
	{
		pTree->MakeMaterialMesh( pd3dDevice );

		if ( pTree->m_pLeftChild )	MakeMaterialMesh ( pd3dDevice, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	MakeMaterialMesh ( pd3dDevice, pTree->m_pRightChild );
	}

    void MakeCollisionData2( LPD3DXMESH pMesh, DxAABBNode** pTree )
	{
		DWORD dwFaceNUM = pMesh->GetNumFaces();

		WORD *pLockIndices=NULL;
		PBYTE pbLockPoints = NULL;
		pMesh->LockIndexBuffer( 0L, (VOID**)&pLockIndices );
		pMesh->LockVertexBuffer( 0L, (VOID**)&pbLockPoints );

		WORD *pFaces;
		pFaces = new WORD[dwFaceNUM];
		if ( pFaces == NULL )
		{
			pMesh->UnlockIndexBuffer();
			pMesh->UnlockVertexBuffer();
			return;// E_OUTOFMEMORY;
		}

		for ( WORD i=0; i<dwFaceNUM; i++ )
			pFaces[i] = i;

		D3DXMATRIX	matComb;
		D3DXMatrixIdentity ( &matComb );

		D3DXVECTOR3 vMax, vMin;
		COLLISION::GetSizeNode ( matComb, pLockIndices, pbLockPoints, pMesh->GetFVF(), pFaces, dwFaceNUM, vMax, vMin );

		DxAABBNode *pNode = new DxAABBNode;
		pNode->fMaxX = vMax.x;
		pNode->fMaxY = vMax.y;
		pNode->fMaxZ = vMax.z;
		pNode->fMinX = vMin.x;
		pNode->fMinY = vMin.y;
		pNode->fMinZ = vMin.z;

		COLLISION::MakeAABBNode ( pNode, matComb, pLockIndices, pbLockPoints, pMesh->GetFVF(), pFaces, dwFaceNUM, vMax, vMin );

		(*pTree) = pNode;
		
		pMesh->UnlockIndexBuffer();
		pMesh->UnlockVertexBuffer();

		SAFE_DELETE_ARRAY( pFaces );
	}

	void MakeCollisionData_VBIB( const LPDIRECT3DVERTEXBUFFERQ pVB, const LPDIRECT3DINDEXBUFFERQ pIB, DWORD dwFVF, DWORD dwFaceNUM, DxAABBNode** pTree )
	{
		WORD *pLockIndices=NULL;
		PBYTE pbLockPoints = NULL;
		pIB->Lock( 0, 0, (VOID**)&pLockIndices, 0L );
		pVB->Lock( 0, 0, (VOID**)&pbLockPoints, 0L );

		WORD *pFaces;
		pFaces = new WORD[dwFaceNUM];
		if ( pFaces == NULL )
		{
			pIB->Unlock();
			pVB->Unlock();
			return;// E_OUTOFMEMORY;
		}

		for ( WORD i=0; i<dwFaceNUM; i++ )
			pFaces[i] = i;

		D3DXMATRIX	matComb;
		D3DXMatrixIdentity ( &matComb );

		D3DXVECTOR3 vMax, vMin;
		COLLISION::GetSizeNode ( matComb, pLockIndices, pbLockPoints, dwFVF, pFaces, dwFaceNUM, vMax, vMin );

		DxAABBNode *pNode = new DxAABBNode;
		pNode->fMaxX = vMax.x;
		pNode->fMaxY = vMax.y;
		pNode->fMaxZ = vMax.z;
		pNode->fMinX = vMin.x;
		pNode->fMinY = vMin.y;
		pNode->fMinZ = vMin.z;

		COLLISION::MakeAABBNode ( pNode, matComb, pLockIndices, pbLockPoints, dwFVF, pFaces, dwFaceNUM, vMax, vMin );

		(*pTree) = pNode;

		pIB->Unlock();
		pVB->Unlock();

		SAFE_DELETE_ARRAY( pFaces );
	}

	void MakeCollisionData ( const LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree )
	{
		if( !pTree )	return;

        LPD3DXMESH* ppOcMesh = pTree->GetppOcMesh();

		if ( (*ppOcMesh) )
		{
            DxAABBNode** ppAABBNode = pTree->GetppCollisionTree();
			MakeCollisionData2 ( (*ppOcMesh), ppAABBNode );
			return;
		}

		if ( pTree->m_pLeftChild )	MakeCollisionData ( pd3dDevice, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	MakeCollisionData ( pd3dDevice, pTree->m_pRightChild );
	}

	void MakeLoadCheckOK( AABBBinaryTreeBase* pTree )
    {
        pTree->SetLoad();
		//pTree->m_bLoad = TRUE;

		if ( pTree->m_pLeftChild )	MakeLoadCheckOK( pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	MakeLoadCheckOK( pTree->m_pRightChild );
    }

    void RenderOctree ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, AABBBinaryTreeBase* pTree )
	{
		if ( !pTree )	return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->GetMax(), pTree->GetMin() ) )	
            return;

		if ( pTree->IsLoad() )
		{
			//(*ppOcMesh)->DrawSubset(0);	// Modify
		    pTree->DrawEff( pd3dDevice );
			return;
		}

		if ( pTree->m_pLeftChild )	RenderOctree ( pd3dDevice, sCV, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	RenderOctree ( pd3dDevice, sCV, pTree->m_pRightChild );
    }

	void RenderOctree_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree, DxTexEffFlowUV* pEff )
	{
		if ( !pTree )	return;

		if ( !COLLISION::IsCollisionVolume ( sCV, pTree->GetMax(), pTree->GetMin() ) )	return;

		if ( pTree->IsLoad() )
		{
			pd3dDevice->SetIndices ( pTree->m_pIB );			
			pEff->Render( pd3dDevice, pTree->m_dwVert, pTree->GetFaceNum(), pTree->m_pVertSrc );
			return;
		}

		if ( pTree->m_pLeftChild )	RenderOctree_FlowUV( pd3dDevice, sCV, pTree->GetLeftChild_OBJOCTree(), pEff );
		if ( pTree->m_pRightChild )	RenderOctree_FlowUV( pd3dDevice, sCV, pTree->GetRightChild_OBJOCTree(), pEff );
	}

	void RenderOctree_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree, DxTexEffRotate* pEff )
	{
		if ( !pTree )	return;

		if ( !COLLISION::IsCollisionVolume ( sCV, pTree->GetMax(), pTree->GetMin() ) )	return;

		if ( pTree->IsLoad() )
		{
			pd3dDevice->SetIndices ( pTree->m_pIB );
			pEff->Render( pd3dDevice, pTree->m_dwVert, pTree->GetFaceNum(), pTree->m_pVertSrc );
			return;
		}

		if ( pTree->m_pLeftChild )	RenderOctree_Rotate( pd3dDevice, sCV, pTree->GetLeftChild_OBJOCTree(), pEff );
		if ( pTree->m_pRightChild )	RenderOctree_Rotate( pd3dDevice, sCV, pTree->GetRightChild_OBJOCTree(), pEff );
	}

    void RenderOctree_VisualMaterialColor( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree, DxTexEffBase* pEff, const D3DXMATRIX& matWorld )
    {
        if ( !pTree )	return;

        if ( !COLLISION::IsCollisionVolume ( sCV, pTree->GetMax(), pTree->GetMin() ) )	return;

		if ( pTree->IsLoad() )
		{
            if ( !pTree->m_spTangentMesh.get() )
            {
                pTree->CreateTangentColorMesh( pd3dDevice );
            }
			pTree->RenderVisualMaterial_Color( pd3dDevice, pEff, FALSE, matWorld, 1.f );
			return;
		}

		if ( pTree->m_pLeftChild )	RenderOctree_VisualMaterialColor( pd3dDevice, sCV, pTree->GetLeftChild_OBJOCTree(), pEff, matWorld );
		if ( pTree->m_pRightChild )	RenderOctree_VisualMaterialColor( pd3dDevice, sCV, pTree->GetRightChild_OBJOCTree(), pEff, matWorld );
    }


	void RenderList ( const LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree )
	{
		if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
			pTree->DrawEff( pd3dDevice );
			return;
		}

		if ( pTree->m_pLeftChild )	RenderList( pd3dDevice, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	RenderList( pd3dDevice, pTree->m_pRightChild );
	}

	void RenderList_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, DxTexEffFlowUV* pEff )
	{
		if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
			pd3dDevice->SetIndices ( pTree->m_pIB );
			pEff->Render( pd3dDevice, pTree->m_dwVert, pTree->GetFaceNum(), pTree->m_pVertSrc );
			return;
		}

		if ( pTree->m_pLeftChild )	RenderList_FlowUV( pd3dDevice, pTree->GetLeftChild_OBJOCTree(), pEff );
		if ( pTree->m_pRightChild )	RenderList_FlowUV( pd3dDevice, pTree->GetRightChild_OBJOCTree(), pEff );
	}

	void RenderList_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, DxTexEffRotate* pEff )
	{
		if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
			pd3dDevice->SetIndices ( pTree->m_pIB );
			pEff->Render( pd3dDevice, pTree->m_dwVert, pTree->GetFaceNum(), pTree->m_pVertSrc );
			return;
		}

		if ( pTree->m_pLeftChild )	RenderList_Rotate( pd3dDevice, pTree->GetLeftChild_OBJOCTree(), pEff );
		if ( pTree->m_pRightChild )	RenderList_Rotate( pd3dDevice, pTree->GetRightChild_OBJOCTree(), pEff );
	}

    void RenderList_VisualMaterialColor( LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, DxTexEffBase* pEff, BOOL bRealPL, const D3DXMATRIX& matWorld, float fAlpha )
    {
        if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
            if ( !pTree->m_spTangentMesh.get() )
            {
                pTree->CreateTangentColorMesh( pd3dDevice );
            }
			pTree->RenderVisualMaterial_Color( pd3dDevice, pEff, bRealPL, matWorld, fAlpha );
			return;
		}

		if ( pTree->m_pLeftChild )	RenderList_VisualMaterialColor( pd3dDevice, pTree->GetLeftChild_OBJOCTree(), pEff, bRealPL, matWorld, fAlpha );
		if ( pTree->m_pRightChild )	RenderList_VisualMaterialColor( pd3dDevice, pTree->GetRightChild_OBJOCTree(), pEff, bRealPL, matWorld, fAlpha );
    }

	void Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, AABBBinaryTreeBase* pTree, BOOL bAlphaTex )
	{
		if ( !pTree )	return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->GetMax(), pTree->GetMin() ) )	
			return;

		if ( pTree->IsLoad() )
		{
			pTree->Render_Query( pd3dDevice, bAlphaTex );
			return;
		}

		if ( pTree->m_pLeftChild )	Render_Query( pd3dDevice, sCV, pTree->m_pLeftChild, bAlphaTex );
		if ( pTree->m_pRightChild )	Render_Query( pd3dDevice, sCV, pTree->m_pRightChild, bAlphaTex );
	}

	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree )
	{
		if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
			pTree->CreateQuery( pd3dDevice );
			return;
		}

		if ( pTree->m_pLeftChild )	CreateQuery( pd3dDevice, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	CreateQuery( pd3dDevice, pTree->m_pRightChild );
	}

	VOID DestroyQuery( AABBBinaryTreeBase* pTree )
	{
		if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
			pTree->DestroyQuery();
			return;
		}

		if ( pTree->m_pLeftChild )	DestroyQuery( pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	DestroyQuery( pTree->m_pRightChild );
	}

	void ResetOcclusionQuery( AABBBinaryTreeBase* pTree )
	{
		if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
			pTree->ResetOcclusionQuery();
			return;
		}

		if ( pTree->m_pLeftChild )	ResetOcclusionQuery( pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	ResetOcclusionQuery( pTree->m_pRightChild );
	}

	void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, AABBBinaryTreeBase* pTree, VEC_DWORD& vecOctreeIndex )
	{
		if ( !pTree )	return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->GetMax(), pTree->GetMin() ) )	
			return;

		if ( pTree->IsLoad() )
		{
			pTree->FindOcclusionRenderIndex( pd3dDevice, vecOctreeIndex );
			return;
		}

		if ( pTree->m_pLeftChild )	FindOcclusionRenderIndex( pd3dDevice, sCV, pTree->m_pLeftChild, vecOctreeIndex );
		if ( pTree->m_pRightChild )	FindOcclusionRenderIndex( pd3dDevice, sCV, pTree->m_pRightChild, vecOctreeIndex );
	}

	void IsCollision ( const DxAABBNode *pAABBCur, AABBBinaryTreeBase* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, 
						D3DXVECTOR3 &vCollision, D3DXVECTOR3 &vNormal, BOOL& bColl, EMCOLLMODE emCullMode, float& fCOLL_LENGTH )
	{
		D3DXVECTOR3 vNewP1(vStart);
        D3DXVECTOR3 vNewP2(vEnd);

		if ( pAABBCur->IsCollision(vNewP1,vNewP2) )
		{
			if ( pAABBCur->IsEnableFace() )
			{
				const D3DXVECTOR3* pvT0 = pAABBCur->GetPosition(0);
				const D3DXVECTOR3* pvT1 = pAABBCur->GetPosition(1);
				const D3DXVECTOR3* pvT2 = pAABBCur->GetPosition(2);

			    D3DXVECTOR3 vNewCollision;
			    D3DXVECTOR3 vNewNormal;
			    if ( COLLISION::IsLineTriangleCollision( pvT0, pvT1, pvT2, &vStart, &vEnd, &vNewCollision, &vNewNormal, emCullMode, TRUE ) )
			    {
				    bColl = TRUE;		// 충돌 되었다는 것 확인.

				    D3DXVECTOR3 vDxVect;
				    vDxVect = vStart - vNewCollision;
				    float fNewLength = D3DXVec3LengthSq ( &vDxVect );

				    if ( fNewLength < fCOLL_LENGTH )
				    {
					    fCOLL_LENGTH = fNewLength;
					    vCollision	= vNewCollision;
					    vNormal		= vNewNormal;

					    vEnd		= vCollision;
				    }
			    }
			}
			else
			{
				if ( pAABBCur->pLeftChild )
					IsCollision ( pAABBCur->pLeftChild, pTree, vStart, vEnd, vCollision, vNormal, bColl, emCullMode, fCOLL_LENGTH );

				if ( pAABBCur->pRightChild )
					IsCollision ( pAABBCur->pRightChild, pTree, vStart, vEnd, vCollision, vNormal, bColl, emCullMode, fCOLL_LENGTH );
			}
		}
	}

	BOOL IsCollisionSimple ( const DxAABBNode *pAABBCur, AABBBinaryTreeBase* pTree, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, EMCOLLMODE emCullMode )
	{
		D3DXVECTOR3 vNewP1(vStart);
		D3DXVECTOR3 vNewP2(vEnd);

		if ( pAABBCur->IsCollision(vNewP1,vNewP2) )
		{
			if ( pAABBCur->IsEnableFace() )
			{
				const D3DXVECTOR3* pvT0 = pAABBCur->GetPosition(0);
				const D3DXVECTOR3* pvT1 = pAABBCur->GetPosition(1);
				const D3DXVECTOR3* pvT2 = pAABBCur->GetPosition(2);

				D3DXVECTOR3 vNewCollision;
				D3DXVECTOR3 vNewNormal;
				if ( COLLISION::IsLineTriangleCollision( pvT0, pvT1, pvT2, &vStart, &vEnd, &vNewCollision, &vNewNormal, emCullMode, TRUE ) )
				{
					return TRUE;
				}
			}
			else
			{
				if ( pAABBCur->pLeftChild )
				{
					if ( IsCollisionSimple ( pAABBCur->pLeftChild, pTree, vStart, vEnd, emCullMode ) )
						return TRUE;
				}

				if ( pAABBCur->pRightChild )
				{
					if ( IsCollisionSimple ( pAABBCur->pRightChild, pTree, vStart, vEnd, emCullMode ) )
						return TRUE;
				}
			}
		}

		return FALSE;
	}

 //   void IsCollisionPixel2( D3DXVECTOR2 vUV[], float u, float v, NSTexture16Bit::DATA* pData16Bit, DWORD& dwColor )
	//{
	//	// Note : 비율로써 UV 값을 구한다.
	//	D3DXVECTOR2 vDirUV[2];
	//	vDirUV[0] = vUV[1] - vUV[0];
	//	vDirUV[1] = vUV[2] - vUV[0];

	//	D3DXVECTOR2 vUVSRC;
	//	vUVSRC = vUV[0];
	//	vUVSRC += vDirUV[0]*u;
	//	vUVSRC += vDirUV[1]*v;

	//	// Note : UV로 좌표를 구한다.
	//	int nPixel[2];
	//	nPixel[0] = static_cast<int>(pData16Bit->m_wwColorSize.wA * vUVSRC.x);
	//	nPixel[1] = static_cast<int>(pData16Bit->m_wwColorSize.wB * vUVSRC.y);

	//	while( nPixel[0] < 0 )
	//	{
	//		nPixel[0] += pData16Bit->m_wwColorSize.wA;
	//	}
	//	while( nPixel[0] >= static_cast<int>(pData16Bit->m_wwColorSize.wA) )
	//	{
	//		nPixel[0] -= pData16Bit->m_wwColorSize.wA;
	//	}

	//	while( nPixel[1] < 0 )
	//	{
	//		nPixel[1] += pData16Bit->m_wwColorSize.wB;
	//	}
	//	while( nPixel[1] >= static_cast<int>(pData16Bit->m_wwColorSize.wB) )
	//	{
	//		nPixel[1] -= pData16Bit->m_wwColorSize.wB;
	//	}

	//	dwColor = pData16Bit->m_pwColorARRAY[ nPixel[0] + (nPixel[1]*pData16Bit->m_wwColorSize.wA) ];
	//}


	BOOL IsCollisionLinePixel ( const DxAABBNode *pAABBCur, AABBBinaryTreeBase* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, 
						EMCOLLMODE emCullMode, const NSTexture16Bit::DATA* pData16Bit, int nIndexUV )
	{
		D3DXVECTOR3 vNewP1=vStart, vNewP2=vEnd;

		if ( pAABBCur->IsCollision(vNewP1,vNewP2) )
		{
			if ( pAABBCur->IsEnableFace() )
			{
				const D3DXVECTOR3* pvT0 = pAABBCur->GetPosition(0);
				const D3DXVECTOR3* pvT1 = pAABBCur->GetPosition(1);
				const D3DXVECTOR3* pvT2 = pAABBCur->GetPosition(2);

				D3DXVECTOR2 vUV0(0.f,0.f);
				D3DXVECTOR2 vUV1(0.f,0.f);
				D3DXVECTOR2 vUV2(0.f,0.f);

				if ( pData16Bit )
				{
					static sc::DefaultLock g_sDefaultLock;
					g_sDefaultLock.lockOn();

					PBYTE pbDataVB;
					PWORD pwIndexB;

					LPD3DXMESH pOcMesh = pTree->GetOcMesh();
					if ( pOcMesh )
					{
						if( FAILED( pOcMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pbDataVB ) ) )
						{
							g_sDefaultLock.lockOff();
							return FALSE;
						}
						if( FAILED( pOcMesh->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pwIndexB ) ) )
						{
							pOcMesh->UnlockVertexBuffer();
							g_sDefaultLock.lockOff();
							return FALSE;
						}

						DWORD fvfsize = D3DXGetFVFVertexSize( pOcMesh->GetFVF() );
						DWORD fvfsizeNoTex = D3DXGetFVFVertexSize( pOcMesh->GetFVF()&0xf0ff );
						PWORD pwIndex = pwIndexB + pAABBCur->GetFaceID()*3;

						DWORD dwTexUVSize(0);
						if ( nIndexUV == -1 )
						{
							dwTexUVSize = fvfsize - fvfsizeNoTex - sizeof(D3DXVECTOR2);
						}

						vUV0 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex)) + fvfsizeNoTex + dwTexUVSize );
						vUV1 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex+1)) + fvfsizeNoTex + dwTexUVSize );
						vUV2 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex+2)) + fvfsizeNoTex + dwTexUVSize );

						pOcMesh->UnlockVertexBuffer();
						pOcMesh->UnlockIndexBuffer();
					}
					else if ( pTree->GetType() == ABTT_OCTREE )
					{
						// dynamic_cast 대신 static_cast를 사용하였다.
						OBJOCTree* pOBJOctree = MIN_BOOST::polymorphic_downcast<OBJOCTree*>( pTree );

						pbDataVB = (PBYTE)pOBJOctree->m_pVertSrc;
						if( FAILED( pOBJOctree->m_pIB->Lock( 0, 0, (VOID**)&pwIndexB, D3DLOCK_READONLY ) ) )
						{
							g_sDefaultLock.lockOff();
							return FALSE;
						}

						DWORD fvfsize = sizeof(VERTEXNORCOLORTEX);		// CAUTION : pTree->m_pVertSrc의 자료형이 바뀌었을 시 안 바꿔주면 큰 문제가 생긴다.
						PWORD pwIndex = pwIndexB + pAABBCur->GetFaceID()*3;

						vUV0 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex)) + fvfsize - sizeof(D3DXVECTOR2) );
						vUV1 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex+1)) + fvfsize - sizeof(D3DXVECTOR2) );
						vUV2 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex+2)) + fvfsize - sizeof(D3DXVECTOR2) );

						pOBJOctree->m_pIB->Unlock();
					}
					else
					{
						g_sDefaultLock.lockOff();
						return FALSE;
					}
					g_sDefaultLock.lockOff();
				}

				float t,u,v;
				D3DXVECTOR3 vNewCollision;
				if ( COLLISION::IsLineTriangleCollision( pvT0, pvT1, pvT2, &vStart, &vEnd, &vNewCollision, t, u, v, NULL, emCullMode, TRUE ) )
				{
					// Note : 픽셀쪽에서 충돌 되었는지 확인한다.
					if ( pData16Bit )
					{
						D3DXVECTOR2 vUV[3];
						vUV[0] = vUV0;	vUV[1] = vUV1;	vUV[2] = vUV2;

						// Note : 비율로써 UV 값을 구한다.
						D3DXVECTOR2 vDirUV[2];
						vDirUV[0] = vUV[1] - vUV[0];
						vDirUV[1] = vUV[2] - vUV[0];

						D3DXVECTOR2 vUVSRC;
						vUVSRC = vUV[0];
						vUVSRC += vDirUV[0]*u;
						vUVSRC += vDirUV[1]*v;

						if ( NSTexture16Bit::GetColor( pData16Bit, vUVSRC ).w > 125 )
						{
							return TRUE;
						}
						else
						{
							return FALSE;
						}
					}

					return TRUE;
				}

                return FALSE;
			}
			else
			{
				if ( pAABBCur->pLeftChild )
                {
					if ( IsCollisionLinePixel ( pAABBCur->pLeftChild, pTree, vStart, vEnd, emCullMode, pData16Bit, nIndexUV ) )
                        return TRUE;
                }

				if ( pAABBCur->pRightChild )
                {
					if ( IsCollisionLinePixel ( pAABBCur->pRightChild, pTree, vStart, vEnd, emCullMode, pData16Bit, nIndexUV ) )
                        return TRUE;
                }
			}
		}

        return FALSE;
	}

	BOOL IsCollisionLinePixel ( const DxAABBNode *pAABBCur, AABBBinaryTreeBase* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, 
								EMCOLLMODE emCullMode, D3DXVECTOR2& vUV, int nIndexUV )
	{
		D3DXVECTOR3 vNewP1=vStart, vNewP2=vEnd;

		if ( pAABBCur->IsCollision(vNewP1,vNewP2) )
		{
			if ( pAABBCur->IsEnableFace() )
			{
				const D3DXVECTOR3* pvT0 = pAABBCur->GetPosition(0);
				const D3DXVECTOR3* pvT1 = pAABBCur->GetPosition(1);
				const D3DXVECTOR3* pvT2 = pAABBCur->GetPosition(2);

				D3DXVECTOR2 vUV0(0.f,0.f);
				D3DXVECTOR2 vUV1(0.f,0.f);
				D3DXVECTOR2 vUV2(0.f,0.f);

				{
					static sc::DefaultLock g_sDefaultLock;
					g_sDefaultLock.lockOn();

					PBYTE pbDataVB;
					PWORD pwIndexB;

					LPD3DXMESH pOcMesh = pTree->GetOcMesh();
					if ( pOcMesh )
					{
						if( FAILED( pOcMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pbDataVB ) ) )
						{
							g_sDefaultLock.lockOff();
							return FALSE;
						}
						if( FAILED( pOcMesh->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pwIndexB ) ) )
						{
							pOcMesh->UnlockVertexBuffer();
							g_sDefaultLock.lockOff();
							return FALSE;
						}

						DWORD fvfsize = D3DXGetFVFVertexSize( pOcMesh->GetFVF() );
						DWORD fvfsizeNoTex = D3DXGetFVFVertexSize( pOcMesh->GetFVF()&0xf0ff );
						PWORD pwIndex = pwIndexB + pAABBCur->GetFaceID()*3;

						DWORD dwTexUVSize(0);
						if ( nIndexUV == -1 )
						{
							dwTexUVSize = fvfsize - fvfsizeNoTex - sizeof(D3DXVECTOR2);
						}

						vUV0 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex)) + fvfsizeNoTex + dwTexUVSize );
						vUV1 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex+1)) + fvfsizeNoTex + dwTexUVSize );
						vUV2 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex+2)) + fvfsizeNoTex + dwTexUVSize );

						pOcMesh->UnlockVertexBuffer();
						pOcMesh->UnlockIndexBuffer();
					}
					else if ( pTree->GetType() == ABTT_OCTREE )
					{
						// dynamic_cast 대신 static_cast를 사용하였다.
						OBJOCTree* pOBJOctree = MIN_BOOST::polymorphic_downcast<OBJOCTree*>( pTree );

						pbDataVB = (PBYTE)pOBJOctree->m_pVertSrc;
						if( FAILED( pOBJOctree->m_pIB->Lock( 0, 0, (VOID**)&pwIndexB, D3DLOCK_READONLY ) ) )
						{
							g_sDefaultLock.lockOff();
							return FALSE;
						}

						DWORD fvfsize = sizeof(VERTEXNORCOLORTEX);		// CAUTION : pTree->m_pVertSrc의 자료형이 바뀌었을 시 안 바꿔주면 큰 문제가 생긴다.
						PWORD pwIndex = pwIndexB + pAABBCur->GetFaceID()*3;

						vUV0 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex)) + fvfsize - sizeof(D3DXVECTOR2) );
						vUV1 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex+1)) + fvfsize - sizeof(D3DXVECTOR2) );
						vUV2 = *(D3DXVECTOR2*) (pbDataVB+fvfsize*(*(pwIndex+2)) + fvfsize - sizeof(D3DXVECTOR2) );

						pOBJOctree->m_pIB->Unlock();
					}
					else
					{
						g_sDefaultLock.lockOff();
						return FALSE;
					}
					g_sDefaultLock.lockOff();
				}

				float t,u,v;
				D3DXVECTOR3 vNewCollision;
				if ( COLLISION::IsLineTriangleCollision( pvT0, pvT1, pvT2, &vStart, &vEnd, &vNewCollision, t, u, v, NULL, emCullMode, TRUE ) )
				{
					D3DXVECTOR2 vDir[2];
					vDir[0] = vUV1 - vUV0;
					vDir[1] = vUV2 - vUV0;
					vUV = vUV0;
					vUV += vDir[0] * u;
					vUV += vDir[1] * v;

					return TRUE;
				}

				return FALSE;
			}
			else
			{
				if ( pAABBCur->pLeftChild )
				{
					if ( IsCollisionLinePixel ( pAABBCur->pLeftChild, pTree, vStart, vEnd, emCullMode, vUV, nIndexUV ) )
						return TRUE;
				}

				if ( pAABBCur->pRightChild )
				{
					if ( IsCollisionLinePixel ( pAABBCur->pRightChild, pTree, vStart, vEnd, emCullMode, vUV, nIndexUV ) )
						return TRUE;
				}
			}
		}

		return FALSE;
	}

	void GetCollisionLineInTriListExceptY2( const DxAABBNode *pAABBCur, OBJOCTree* pTree, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) //주병진 추가 메소드
	{
  		D3DXVECTOR3 vNewP1=vStart, vNewP2=vEnd;

		if ( pAABBCur->IsCollisionExceptY(vNewP1,vNewP2) )
		{
 			if ( pAABBCur->IsEnableFace() )
			{
				const D3DXVECTOR3* pvT0 = pAABBCur->GetPosition(0);
				const D3DXVECTOR3* pvT1 = pAABBCur->GetPosition(1);
				const D3DXVECTOR3* pvT2 = pAABBCur->GetPosition(2);

				/////////////////////////////////////////////////////////

 				D3DXVECTOR2 v2Cur1   = D3DXVECTOR2( pvT0->x, pvT0->z );
				D3DXVECTOR2 v2Cur2   = D3DXVECTOR2( pvT1->x, pvT1->z );
				D3DXVECTOR2 v2Cur3   = D3DXVECTOR2( pvT2->x, pvT2->z );
				D3DXVECTOR2 v2Start  = D3DXVECTOR2( vStart.x, vStart.z );
				D3DXVECTOR2 v2End    = D3DXVECTOR2( vEnd.x,   vEnd.z   );

				D3DXVECTOR2 v2Out1,v2Out2,v2Out3;
				BOOL        bPick1,bPick2,bPick3;

				bPick1 = COLLISION::IsCollision2DLineToLine(v2Start, v2End, v2Cur1, v2Cur2, v2Out1);
				bPick2 = COLLISION::IsCollision2DLineToLine(v2Start, v2End, v2Cur1, v2Cur3, v2Out2);
				bPick3 = COLLISION::IsCollision2DLineToLine(v2Start, v2End, v2Cur2, v2Cur3, v2Out3);

				D3DXVECTOR3 vCollisionPoint;

				FLOAT       fMaxDistance = -FLT_MAX;
				BOOL        IsPick       = FALSE;

				if ( bPick1 )
				{
					FLOAT fDistance = D3DXVec2Length(&D3DXVECTOR2(v2Out1 - v2Start));

					if ( fMaxDistance < fDistance )
					{
						vCollisionPoint = D3DXVECTOR3(v2Out1.x, 0.0f, v2Out1.y);
						fMaxDistance    = fDistance;
					}

					IsPick = TRUE;
				}

				if ( bPick2 )
				{
					FLOAT fDistance = D3DXVec2Length(&D3DXVECTOR2(v2Out2 - v2Start));

					if ( fMaxDistance < fDistance )
					{
						vCollisionPoint = D3DXVECTOR3(v2Out2.x, 0.0f, v2Out2.y);
						fMaxDistance    = fDistance;
					}

					IsPick = TRUE;
				}

				if ( bPick3 )
				{
					FLOAT fDistance = D3DXVec2Length(&D3DXVECTOR2(v2Out3 - v2Start));

					if ( fMaxDistance < fDistance )
					{
						vCollisionPoint = D3DXVECTOR3(v2Out3.x, 0.0f, v2Out3.y);
						fMaxDistance    = fDistance;
					}

					IsPick = TRUE;
				}

				if ( IsPick )
				{
					FLOAT fHeight = -FLT_MAX;

					if ( pvT0->y > fHeight ) fHeight = pvT0->y;
					if ( pvT1->y > fHeight ) fHeight = pvT1->y;
					if ( pvT2->y > fHeight ) fHeight = pvT2->y; 

					COLLISION::TRIANGLE TriangleColl;

					TriangleColl.vTri1 = *pvT0;
					TriangleColl.vTri2 = *pvT1;
					TriangleColl.vTri3 = *pvT2;

					TriangleColl.vCollision = D3DXVECTOR3(vCollisionPoint.x, fHeight, vCollisionPoint.z);

					TriList.push_back(TriangleColl);
				}

				/////////////////////////////////////////////////////////

				//pTree->m_pIB->Unlock();
			}
			else
			{
				if ( pAABBCur->pLeftChild )
					GetCollisionLineInTriListExceptY2 ( pAABBCur->pLeftChild, pTree, vStart, vEnd, TriList );

				if ( pAABBCur->pRightChild )
					GetCollisionLineInTriListExceptY2 ( pAABBCur->pRightChild, pTree, vStart, vEnd, TriList );
			}
		}
	}

	BOOL IsCollisionLineSimple( AABBBinaryTreeBase* pTree, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode )
	{
		if (!pTree)
			return FALSE;

		if ( !COLLISION::IsCollisionLineToAABB( vStart, vEnd, pTree->GetMax(), pTree->GetMin() ) )
			return FALSE;

		if ( pTree->GetCollisionTree() )
		{
			if ( IsCollisionSimple( pTree->GetCollisionTree(), pTree, vStart, vEnd, emCullMode ) )
				return TRUE;
		}

		if ( pTree->m_pLeftChild )	
		{
			if ( IsCollisionLineSimple( pTree->m_pLeftChild, vStart, vEnd, emCullMode ) )
			{
				return TRUE;
			}
		}
		if ( pTree->m_pRightChild )	
		{
			if ( IsCollisionLineSimple( pTree->m_pRightChild, vStart, vEnd, emCullMode ) )
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	void CollisionLine2(
        AABBBinaryTreeBase* pTree,
        const D3DXVECTOR3& vStart,
        D3DXVECTOR3& vEnd,
        D3DXVECTOR3& vColl,
        D3DXVECTOR3& vNor, 
        BOOL& bColl,
        EMCOLLMODE emCullMode,
		float& fCOLL_LENGTH )
	{
		if (!pTree)
            return;

		if ( !COLLISION::IsCollisionLineToAABB( vStart, vEnd, pTree->GetMax(), pTree->GetMin() ) )
            return;

		if ( pTree->GetCollisionTree() )
		{
			IsCollision( pTree->GetCollisionTree(), pTree, vStart, vEnd, vColl, vNor, bColl, emCullMode, fCOLL_LENGTH );
		}

        if ( pTree->m_pLeftChild )	CollisionLine2( pTree->m_pLeftChild, vStart, vEnd, vColl, vNor, bColl, emCullMode, fCOLL_LENGTH );
		if ( pTree->m_pRightChild )	CollisionLine2( pTree->m_pRightChild, vStart, vEnd, vColl, vNor, bColl, emCullMode, fCOLL_LENGTH );
	}

	void CollisionLine ( AABBBinaryTreeBase* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor,
		BOOL& bColl, EMCOLLMODE emCullMode, float& fCOLL_LENGTH )
	{
		CollisionLine2 ( pTree, vStart, vEnd, vColl, vNor, bColl, emCullMode, fCOLL_LENGTH );
	}

    // Note : 충돌이 되면 바로 리턴한다.
    BOOL IsCollisionLinePixel( AABBBinaryTreeBase* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, EMCOLLMODE emCullMode, const NSTexture16Bit::DATA* pData16Bit, int nIndexUV )
    {
        if ( !pTree )	
            return FALSE;

		if ( !COLLISION::IsCollisionLineToAABB( vStart, vEnd, pTree->GetMax(), pTree->GetMin() ) )	
            return FALSE;

		if ( pTree->GetCollisionTree() )
		{
			DWORD dwColor(0L);
			if ( IsCollisionLinePixel( pTree->GetCollisionTree(), pTree, vStart, vEnd, emCullMode, pData16Bit, nIndexUV ) )
			{
				return TRUE;
			}
		}

        if ( pTree->m_pLeftChild )	
        {
            if ( IsCollisionLinePixel( pTree->m_pLeftChild, vStart, vEnd, emCullMode, pData16Bit, nIndexUV ) )
            {
                return TRUE;
            }
        }   
		if ( pTree->m_pRightChild )	
        {
            if ( IsCollisionLinePixel( pTree->m_pRightChild, vStart, vEnd, emCullMode, pData16Bit, nIndexUV ) )
            {
                return TRUE;
            }
        }

        return FALSE;
    }

	void GetCollisionLineInTriListExceptY( OBJOCTree* pTree, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) //주병진 추가 메소드
	{
		if ( !pTree )	return;

		D3DXVECTOR3 vCollMax = pTree->GetMax();
		D3DXVECTOR3 vCollMin = pTree->GetMin();

		vCollMax.y = FLT_MAX;
		vCollMin.y = -FLT_MAX;

		if ( !COLLISION::IsCollisionLineToAABB( vStart, vEnd, vCollMax, vCollMin ) )	return;

		if ( pTree->GetCollisionTree() )
		{
			GetCollisionLineInTriListExceptY2( pTree->GetCollisionTree(), pTree, vStart, vEnd, TriList );
		}

		if ( pTree->m_pLeftChild )	GetCollisionLineInTriListExceptY( pTree->GetLeftChild_OBJOCTree(), vStart, vEnd, TriList );
		if ( pTree->m_pRightChild )	GetCollisionLineInTriListExceptY( pTree->GetRightChild_OBJOCTree(), vStart, vEnd, TriList );
	}

	BOOL GetLightMapColor( AABBBinaryTreeBase* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR2& vUV )
	{
		if ( !pTree )	
			return FALSE;

		if ( !COLLISION::IsCollisionLineToAABB( vStart, vEnd, pTree->GetMax(), pTree->GetMin() ) )	
			return FALSE;

		if ( pTree->GetCollisionTree() )
		{
			return IsCollisionLinePixel( pTree->GetCollisionTree(), pTree, vStart, vEnd, EMCC_CULL_CCW, vUV, -1 );
		}

		if ( pTree->m_pLeftChild )	
		{
			if ( GetLightMapColor( pTree->m_pLeftChild, vStart, vEnd, vUV ) )
			{
				return TRUE;
			}
		}   
		if ( pTree->m_pRightChild )	
		{
			if ( GetLightMapColor( pTree->m_pRightChild, vStart, vEnd, vUV ) )
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	void GetVertexNum( AABBBinaryTreeBase* pTree, DWORD& dwVertexNum )
	{
		if ( !pTree )	return;

		pTree->GetVertexNum( dwVertexNum );

		if ( pTree->m_pLeftChild )	GetVertexNum( pTree->m_pLeftChild, dwVertexNum );
		if ( pTree->m_pRightChild )	GetVertexNum( pTree->m_pRightChild, dwVertexNum );
	}

    void GetVertices( AABBBinaryTreeBase* pTree, std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV )
    {
        if ( !pTree )	return;

        pTree->GetVectorVertices( vecVertices, vecLightMapUV );

        if ( pTree->m_pLeftChild )	GetVertices( pTree->m_pLeftChild, vecVertices, vecLightMapUV );
		if ( pTree->m_pRightChild )	GetVertices( pTree->m_pRightChild, vecVertices, vecLightMapUV );
    }

	void GetVertices( AABBBinaryTreeBase* pTree, std::vector<VERTEXNORCOLORTEX5>& vecVertices )
	{
		if ( !pTree )	return;

		pTree->GetVectorVertices( vecVertices );

		if ( pTree->m_pLeftChild )	GetVertices( pTree->m_pLeftChild, vecVertices );
		if ( pTree->m_pRightChild )	GetVertices( pTree->m_pRightChild, vecVertices );
	}

	// [shhan][2014.11.28] 이부분에서는 Mesh 에 Lock 을 걸 수 있는 이유는 Main Thread 에서 작업하기 때문이다.
	//						MainThread 가 아닌 곳에서 이함수를 호출한다면 Mesh 들이 깜빡이면서 렌더링이 되었다 안되었다가 반복될 것이다.
	//
	void ClipMesh( AABBBinaryTreeBase* pTree, DxDecal* pDecal )
	{
		if ( !pTree )	return;

		PBYTE pbDataVB;
		PWORD pwIndexB;

		if ( pTree->IsLoad() && pTree->GetCollisionTree() )
		{
			LPD3DXMESH pOcMesh = pTree->GetOcMesh();
			if ( pOcMesh )
			{
				if( FAILED( pOcMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pbDataVB ) ) )
					return;
				if( FAILED( pOcMesh->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pwIndexB ) ) )
				{
					pOcMesh->UnlockVertexBuffer();
					return;
				}

				pDecal->ClipOBJOCTree ( pTree->GetCollisionTree(), pOcMesh->GetFVF(), pbDataVB, pwIndexB );

				pOcMesh->UnlockVertexBuffer();
				pOcMesh->UnlockIndexBuffer();
			}
			else if ( pTree->GetType() == ABTT_OCTREE )
			{
				// dynamic_cast 대신 static_cast를 사용하였다.
				OBJOCTree* pOBJOctree = MIN_BOOST::polymorphic_downcast<OBJOCTree*>( pTree );

				//if( FAILED( pTree->m_pVB->Lock( 0, 0, (VOID**)&pbDataVB, D3DLOCK_READONLY ) ) )
				//	return;
				pbDataVB = (PBYTE)pOBJOctree->m_pVertSrc;
				if( FAILED( pOBJOctree->m_pIB->Lock( 0, 0, (VOID**)&pwIndexB, D3DLOCK_READONLY ) ) )
				{
					//pTree->m_pVB->Unlock();
					return;
				}

				pDecal->ClipOBJOCTree ( pTree->GetCollisionTree(), VERTEXNORCOLORTEX::FVF, pbDataVB, pwIndexB );

				//pTree->m_pVB->Unlock();
				pOBJOctree->m_pIB->Unlock();
			}
		}

		if ( pTree->m_pLeftChild )	ClipMesh( pTree->m_pLeftChild, pDecal );
		if ( pTree->m_pRightChild )	ClipMesh( pTree->m_pRightChild, pDecal );
	}

	void FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, AABBBinaryTreeBase* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
	{
		if( !pTree )	return;

		if ( !COLLISION::IsCollisionAABBToAABB( vMax, vMin, pTree->GetMax(), pTree->GetMin() ) )	return;

		pTree->DynamicLoad( pd3dDevice, SFile, FALSE );

		FirstLoad( pd3dDevice, SFile, pTree->m_pLeftChild, vMax, vMin );
		FirstLoad( pd3dDevice, SFile, pTree->m_pRightChild, vMax, vMin );
	}

	void DynamicLoad ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, AABBBinaryTreeBase* pTree, const CLIPVOLUME &sCV )
	{
		if( !pTree )	return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->GetMax(), pTree->GetMin() ) )	return;

		pTree->DynamicLoad( pd3dDevice, SFile, FALSE );

		DynamicLoad ( pd3dDevice, SFile, pTree->m_pLeftChild, sCV );
		DynamicLoad ( pd3dDevice, SFile, pTree->m_pRightChild, sCV );
	}

	void ThreadLoad ( OBJOCTree* pTree, const CLIPVOLUME &sCV, LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading )
	{
		//if (!pTree)
  //          return;

		//if (!COLLISION::IsCollisionVolume(sCV, pTree->GetMax(), pTree->GetMin()))
  //          return;

  //      if ( !pTree->GetLeftChild_OBJOCTree() && !pTree->GetRightChild_OBJOCTree() )
		//    pTree->ThreadLoad(listLoadingData, CSLockLoading);

		//ThreadLoad(pTree->GetLeftChild_OBJOCTree(), sCV, listLoadingData, CSLockLoading);
		//ThreadLoad(pTree->GetRightChild_OBJOCTree(), sCV, listLoadingData, CSLockLoading);
	}

	void SaveOctree ( sc::SerialFile& SFile, AABBBinaryTreeBase* pTree, BOOL bPiece )
	{
		if ( pTree )
		{
			SFile << (BOOL)TRUE;
			pTree->Save ( SFile, bPiece );
		}
		else
		{
			SFile << (BOOL)FALSE;
			return;
		}

		SaveOctree ( SFile, pTree->m_pLeftChild, bPiece );
		SaveOctree ( SFile, pTree->m_pRightChild, bPiece );
	}

	void LoadOctree2( const LPDIRECT3DDEVICEQ pd3dDevice, 
                    sc::SerialFile& SFile, 
                    AABBBinaryTreeBase** ppTree, 
                    const BOOL bDynamicLoad, 
                    BOOL bPiece, 
                    DWORD dwType )   // emType - AABB_BINARY_TREE_TYPE
	{
		BOOL bUse = TRUE;
		SFile >> bUse;

		if ( bUse )
		{
			(*ppTree) = AABBBinaryTreeBase::CreateFactory( static_cast<AABB_BINARY_TREE_TYPE>( dwType ) );
			(*ppTree)->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
		else
		{
			return;
		}

		LoadOctree2( pd3dDevice, SFile, &(*ppTree)->m_pLeftChild, bDynamicLoad, bPiece, dwType );
		LoadOctree2( pd3dDevice, SFile, &(*ppTree)->m_pRightChild, bDynamicLoad, bPiece, dwType );
	}

	void LoadOctree_Prev2( const LPDIRECT3DDEVICEQ pd3dDevice, 
						sc::SerialFile& SFile, 
						AABBBinaryTreeBase** ppTree, 
						const BOOL bDynamicLoad, 
						BOOL bPiece,
						DWORD dwType )   // emType - AABB_BINARY_TREE_TYPE
	{
		BOOL bUse = TRUE;
		SFile >> bUse;

		if ( bUse )
		{
			(*ppTree) = AABBBinaryTreeBase::CreateFactory( static_cast<AABB_BINARY_TREE_TYPE>( dwType ) );
			(*ppTree)->Load_Prev( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
		else
		{
			return;
		}

		LoadOctree_Prev2( pd3dDevice, SFile, &(*ppTree)->m_pLeftChild, bDynamicLoad, bPiece, dwType );
		LoadOctree_Prev2( pd3dDevice, SFile, &(*ppTree)->m_pRightChild, bDynamicLoad, bPiece, dwType );
	}

	void LoadOctreeVER_100 ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, OBJOCTree** ppTree )
	{
		BOOL bUse = TRUE;
		SFile >> bUse;

		if ( bUse )
		{
			(*ppTree) = new OBJOCTree;
			(*ppTree)->Load_VER100 ( pd3dDevice, SFile );
		}
		else
		{
			return;
		}

        OBJOCTree* pLeftChild = MIN_BOOST::polymorphic_downcast<OBJOCTree*>( (*ppTree)->m_pLeftChild );
        OBJOCTree* pRightChild = MIN_BOOST::polymorphic_downcast<OBJOCTree*>( (*ppTree)->m_pRightChild );
		LoadOctreeVER_100 ( pd3dDevice, SFile, &pLeftChild );
		LoadOctreeVER_100 ( pd3dDevice, SFile, &pRightChild );
	}

    //--------------------------------------------------------------------------------------------------------------------
    // LightMapRender, LightMapRenderMat 관련
    LPDIRECT3DDEVICEQ   g_pd3dDevice(NULL);
    const CLIPVOLUME*   g_pCV(NULL);
    const D3DXMATRIX*   g_pmatWorld(NULL);
    DxTexEffBase*       g_pEff(NULL);				// LightMapRender
	LPD3DXEFFECT		g_pFX(NULL);				// LightMapRenderMat
	DWORD	 			g_dwFVF(0L);				// LightMapRenderMat
	int					g_nLightMapIndex(-1);		// LightMapRenderMat
    LPDIRECT3DTEXTUREQ	g_pTextureDay(NULL);
    LPDIRECT3DTEXTUREQ  g_pTextureNight(NULL);
    D3DXVECTOR2         g_vLightMapUV_Offset(0.f,0.f);
    float               g_fAlpha(1.f);
	BOOL				g_bShadowRender(FALSE);	// LightMapRenderMat
	MATERIAL_DATA		g_sMaterial_Data;

    void InitRenderOctree( LPDIRECT3DDEVICEQ pd3dDevice, 
                            const CLIPVOLUME* pCV, 
                            const D3DXMATRIX* pmatWorld, 
                            DxTexEffBase* pEff,						// LightMapRender 에서만 쓰임
							const MATERIAL_DATA& sMaterial_Data,	// LightMapRenderMat 에서만 쓰임
                            LPDIRECT3DTEXTUREQ pTextureDay, 
                            LPDIRECT3DTEXTUREQ pTextureNight,
                            const D3DXVECTOR2& vLightMapUV_Offset,
                            float fAlpha,
							BOOL bShadowRender )
    {
        g_pd3dDevice        = pd3dDevice;
        g_pCV               = pCV;
        g_pmatWorld         = pmatWorld;
        g_pEff              = pEff;
		g_sMaterial_Data	= sMaterial_Data;
        g_pTextureDay       = pTextureDay;
        g_pTextureNight     = pTextureNight;
        g_vLightMapUV_Offset = vLightMapUV_Offset;
        g_fAlpha            = fAlpha;
		g_bShadowRender		= bShadowRender;
    }

    void RenderOctree_VisualMaterial( AABBBinaryTreeBase* pTree )
    {
        if ( !pTree )	return;

        if ( g_pCV && !COLLISION::IsCollisionVolume ( *g_pCV, pTree->GetMax(), pTree->GetMin() ) )	
            return;

		if ( pTree->IsLoad() )
		{
            pTree->RenderVisualMaterial
            ( 
                g_pd3dDevice, 
                g_pEff, 
                *g_pmatWorld, 
                D3DXVECTOR2(0.f,0.f),
                g_fAlpha,
				g_pTextureDay, 
				g_pTextureNight
            );

			//return;
		}

		if ( pTree->m_pLeftChild )	RenderOctree_VisualMaterial( pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	RenderOctree_VisualMaterial( pTree->m_pRightChild );
    }

    void RenderOctree_VisualMaterial_NoneClipVolume( AABBBinaryTreeBase* pTree )
    {
        if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
			pTree->RenderVisualMaterial
            ( 
                g_pd3dDevice, 
                g_pEff, 
                *g_pmatWorld, 
                g_vLightMapUV_Offset,
                g_fAlpha,
				g_pTextureDay, 
				g_pTextureNight
            );

			//return;
		}

		if ( pTree->m_pLeftChild )	RenderOctree_VisualMaterial_NoneClipVolume( pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	RenderOctree_VisualMaterial_NoneClipVolume( pTree->m_pRightChild );
    }

	void RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree )
	{
		if ( !pTree )	return;

		//if ( pTree->IsLoad() )
		// 현재 유효한 것은 CollisionMesh 로만 알 수 있다.
		if ( pTree->GetCollisionTree() )
		{
			pTree->Render( pd3dDevice );
		}

		if ( pTree->m_pLeftChild )	RenderMaterial ( pd3dDevice, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	RenderMaterial ( pd3dDevice, pTree->m_pRightChild );
	}

	void RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV, AABBBinaryTreeBase* pTree )
	{
		if ( !pTree )	return;

		if ( pCV )
		{
			if ( !COLLISION::IsCollisionVolume( *pCV, pTree->GetMax(), pTree->GetMin() ) )	
				return;
		}

		//if ( pTree->IsLoad() )
		// 현재 유효한 것은 CollisionMesh 로만 알 수 있다.
		if ( pTree->GetCollisionTree() )
		{
			pTree->Render( pd3dDevice );
		}

		if ( pTree->m_pLeftChild )	RenderMaterial ( pd3dDevice, pCV, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	RenderMaterial ( pd3dDevice, pCV, pTree->m_pRightChild );
	}

	void RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV, AABBBinaryTreeBase* pTree, BOOL bAlphaTex )
	{
		if ( !pTree )	return;

		if ( pCV )
		{
			if ( !COLLISION::IsCollisionVolume( *pCV, pTree->GetMax(), pTree->GetMin() ) )	
				return;
		}

		if ( pTree->IsLoad() )
		{
			pTree->RenderCascadeShadow( pd3dDevice, bAlphaTex );
		}

		if ( pTree->m_pLeftChild )	RenderCascadeShadow ( pd3dDevice, pCV, pTree->m_pLeftChild, bAlphaTex );
		if ( pTree->m_pRightChild )	RenderCascadeShadow ( pd3dDevice, pCV, pTree->m_pRightChild, bAlphaTex );
	}

	void RenderCascadeShadow_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, AABBBinaryTreeBase* pTree, BOOL bAlphaTex )
	{
		if ( pTree->IsLoad() )
		{
			pTree->RenderCascadeShadow_Instancing( pd3dDevice, bAlphaTex );
		}

		if ( pTree->m_pLeftChild )	RenderCascadeShadow_Instancing ( pd3dDevice, pTree->m_pLeftChild, bAlphaTex );
		if ( pTree->m_pRightChild )	RenderCascadeShadow_Instancing ( pd3dDevice, pTree->m_pRightChild, bAlphaTex );
	}

	void SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate, AABBBinaryTreeBase* pTree )
	{
		if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
			pTree->SetLightMapUV_Rate( vLightMapUV_Rate );
		}

		if ( pTree->m_pLeftChild )	SetLightMapUV_Rate ( vLightMapUV_Rate, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	SetLightMapUV_Rate ( vLightMapUV_Rate, pTree->m_pRightChild );
	}

	void RenderMaterial( AABBBinaryTreeBase* pTree )
	{
		if ( !pTree )	return;

		if ( g_pCV )
		{
			if ( !COLLISION::IsCollisionVolume( *g_pCV, pTree->GetMax(), pTree->GetMin() ) )	
				return;
		}

		//if ( pTree->IsLoad() )
		// 현재 유효한 것은 CollisionMesh 로만 알 수 있다.
		if ( pTree->GetCollisionTree() )
		{
			pTree->RenderMaterial
			( 
				g_pd3dDevice, 
				g_sMaterial_Data
			);

			return;
		}

		if ( pTree->m_pLeftChild )	RenderMaterial( pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	RenderMaterial( pTree->m_pRightChild );
	}

	void RenderPieceMaterial_Instancing( AABBBinaryTreeBase* pTree, LPDIRECT3DDEVICEQ pd3dDevice, ID3DXEffect* pFX, const MATERIAL_DATA& sMaterial_Data )
	{
		// 현재 유효한 것은 CollisionMesh 로만 알 수 있다.
		if ( pTree->GetCollisionTree() )
		{
			pTree->RenderPieceMaterial_Instancing
			( 
				pd3dDevice, 
				pFX,
				sMaterial_Data
			);

			return;
		}

		if ( pTree->m_pLeftChild )	RenderPieceMaterial_Instancing( pTree->m_pLeftChild, pd3dDevice, pFX, sMaterial_Data );
		if ( pTree->m_pRightChild )	RenderPieceMaterial_Instancing( pTree->m_pRightChild, pd3dDevice, pFX, sMaterial_Data );
	}

	void RenderMaterialWater( AABBBinaryTreeBase* pTree, float& fHeight, float& fNearLength )
	{
		if ( !pTree )	return;

		if ( g_pCV )
		{
			if ( !COLLISION::IsCollisionVolume( *g_pCV, pTree->GetMax(), pTree->GetMin() ) )	
				return;
		}

		//if ( pTree->IsLoad() )
		// 현재 유효한 것은 CollisionMesh 로만 알 수 있다.
		if ( pTree->GetCollisionTree() )
		{
			pTree->RenderMaterialWater
			( 
				g_pd3dDevice, 
				g_sMaterial_Data,
				fHeight,
				fNearLength
			);

			return;
		}

		if ( pTree->m_pLeftChild )	RenderMaterialWater( pTree->m_pLeftChild, fHeight, fNearLength );
		if ( pTree->m_pRightChild )	RenderMaterialWater( pTree->m_pRightChild, fHeight, fNearLength );
	}

    void GetClosedPoint( OBJOCTree* pTree, const D3DXVECTOR3 vBasePos, D3DXVECTOR3& vCollisionOut, float& fDistance )
    {
        if ( !pTree )	return;

		if ( pTree->GetCollisionTree() )
		{
            float fLength(0.f);
            D3DXVECTOR3 vDir(0.f,0.f,0.f);
		    for( DWORD i=0; i<pTree->m_dwVert; ++i )
		    {
                vDir =  pTree->m_pVertSrc[i].vPos - vBasePos;
                fLength = D3DXVec3Length( &vDir );

                if ( fLength < fDistance )
                {  
                    fDistance = fLength;
                    vCollisionOut = pTree->m_pVertSrc[i].vPos;
                }
		    }
		}

		if ( pTree->m_pLeftChild )	GetClosedPoint( pTree->GetLeftChild_OBJOCTree(), vBasePos, vCollisionOut, fDistance );
		if ( pTree->m_pRightChild )	GetClosedPoint( pTree->GetRightChild_OBJOCTree(), vBasePos, vCollisionOut, fDistance );
    }

	void HaveLightMapUV( OBJOCTree* pTree, BOOL& bHaveLightMapUV )
	{
		if ( !pTree )
			return;

		if ( pTree->m_pLightMapUV )
			bHaveLightMapUV = TRUE;

		if ( pTree->m_pLeftChild )	HaveLightMapUV( pTree->GetLeftChild_OBJOCTree(), bHaveLightMapUV );
		if ( pTree->m_pRightChild )	HaveLightMapUV( pTree->GetRightChild_OBJOCTree(), bHaveLightMapUV );
	}
}
