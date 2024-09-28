#include "pch.h"

#include "../../../VisualMaterialLib/Util/NSNearPointLight.h"

#include "../../../SigmaCore/Util/Util.h"
#include "../../../SigmaCore/Math/Random.h"

#include "../../DxCommon9/DXUTmisc.h"
#include "../../DxEffect/Mesh/DxStaticMesh.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../../DxTools/Occlusion/NSOcclusionQueryFastRender.h"
#include "../../DxTools/Occlusion/NSOcclusionOctree.h"
#include "../../DxTools/Occlusion/DxOcclusionQuery.h"
#include "../../DxTools/Material/NSShadowRenderForMaterial.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/Texture/NSTexture16Bit.h"
#include "../../DxTools/Light/NSShadowLight.h"
#include "../../DxTools/Light/DxShadowLight.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxTools/Collision.h"
#include "../../DxLand/LightMap/LightMapObjectVEC.h"
#include "../../DxLand/LightMap/LightMapBoxColor.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../DxLand/LightMap/NSLightMap.h"
#include "../../DxLand/DxSetLandMan.h"
#include "../LightMap/LightMapObject.h"

#include "./NSPieceInstancing.h"
#include "./NSPieceQuickSort.h"
#include "./DxPieceStaticAni.h"
#include "./DxPieceTexEff.h"
#include "./DxPieceEdit.h"
#include "./DxPieceAABB.h"
#include "./DxPieceEff.h"
#include "./DxPieceNode.h"
#include "./DxPiece.h"

#include "NSPIECETREE.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//											N	S		P	I	E	C	E		T	R	E	E
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSPIECETREE
{
	BOOL SizeCheck( DxPieceNode* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
	{
		if ( pTree->m_vecPIECE.size() <= 1 )
			return FALSE;

		D3DXVECTOR3 vMaxAABB = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMinAABB = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );

		int nRight(0);
		int nLeft(0);
		BOOL bFullSize(FALSE);
		for( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		{
			DxPiece* pCur = pTree->m_vecPIECE[i];

			vMaxAABB = pCur->m_vMax;
			vMinAABB = pCur->m_vMin;
			D3DXMatrixAABBTransform( vMaxAABB, vMinAABB, pCur->m_matWorld );

			// 한 변이라도 최대 길이와 같으면 Right로 보낸다.
			// 아니면 Left
			if ( vMaxAABB.x <= (vMax.x+0.01f) && vMaxAABB.x >= (vMax.x-0.01f) &&
				vMinAABB.x <= (vMin.x+0.01f) && vMinAABB.x >= (vMin.x-0.01f) )
			{
				++nRight;
			}
			else if ( vMaxAABB.y <= (vMax.y+0.01f) && vMaxAABB.y >= (vMax.y-0.01f) &&
						vMinAABB.y <= (vMin.y+0.01f) && vMinAABB.y >= (vMin.y-0.01f) )
			{
				++nRight;
			}
			else if ( vMaxAABB.z <= (vMax.z+0.01f) && vMaxAABB.z >= (vMax.z-0.01f) &&
						vMinAABB.z <= (vMin.z+0.01f) && vMinAABB.z >= (vMin.z-0.01f) )
			{
				++nRight;
			}
			else
			{
				++nLeft;
			}
		}

		if ( !nRight || !nLeft )
		{
			return FALSE;
		}

		// Note : 작업
		pTree->m_pLeftChild = new DxPieceNode;
		pTree->m_pRightChild = new DxPieceNode;
		for (size_t i=0; i<pTree->m_vecPIECE.size(); ++i)
		{
			DxPiece* pCur = pTree->m_vecPIECE[i];

			vMaxAABB = pCur->m_vMax;
			vMinAABB = pCur->m_vMin;
			D3DXMatrixAABBTransform( vMaxAABB, vMinAABB, pCur->m_matWorld );

			if ( vMaxAABB.x <= (vMax.x+0.01f) && vMaxAABB.x >= (vMax.x-0.01f) &&
				vMinAABB.x <= (vMin.x+0.01f) && vMinAABB.x >= (vMin.x-0.01f) )
			{
				pTree->m_pRightChild->m_vecPIECE.push_back( pCur );
			}
			else if ( vMaxAABB.y <= (vMax.y+0.01f) && vMaxAABB.y >= (vMax.y-0.01f) &&
						vMinAABB.y <= (vMin.y+0.01f) && vMinAABB.y >= (vMin.y-0.01f) )
			{
				pTree->m_pRightChild->m_vecPIECE.push_back( pCur );
			}
			else if ( vMaxAABB.z <= (vMax.z+0.01f) && vMaxAABB.z >= (vMax.z-0.01f) &&
						vMinAABB.z <= (vMin.z+0.01f) && vMinAABB.z >= (vMin.z-0.01f) )
			{
				pTree->m_pRightChild->m_vecPIECE.push_back( pCur );
			}
			else
			{
				pTree->m_pLeftChild->m_vecPIECE.push_back( pCur );
			}
		}

		// Note : 옮겨가서 지워준다.
		pTree->m_vecPIECE.clear();

		return TRUE;
	}

	BOOL OptimizeDivide2( DxPieceNode* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, int nDivide, int& nRight, int& nLeft )
	{
		nLeft = 0;
		nRight = 0;
		for (size_t i=0; i<pTree->m_vecPIECE.size(); ++i )
		{
			DxPiece* pCur = pTree->m_vecPIECE[i];

			switch ( nDivide )
			{
			case 1:
				//if( vMinAABB.x > fHalfX )	++nRight;
				if( pCur->m_matWorld._41 > pTree->m_vCenterPoint.x )	++nRight;
				else						++nLeft;
				break;
			case 2:
				//if( vMinAABB.y > fHalfY )	++nRight;
				if( pCur->m_matWorld._42 > pTree->m_vCenterPoint.y )	++nRight;
				else						++nLeft;
				break;
			case 3:
				//if( vMinAABB.z > fHalfZ )	++nRight;
				if( pCur->m_matWorld._43 > pTree->m_vCenterPoint.z )	++nRight;
				else						++nLeft;
				break;
			};
		}

		return TRUE;
	}

	BOOL OptimizeDivide( DxPieceNode* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, BOOL bCollision, float fCellSize, int& nDivide )
	{
		// 초기화 한다.
		nDivide = 0;

		// 너무 큰게 있으면 큰것들과 작은것들을 분리한다.
		if ( SizeCheck( pTree, vMax, vMin ) )
		{
			return TRUE;
		}

		int nRight, nLeft;
		if ( OptimizeDivide2( pTree, vMax, vMin, 1, nRight, nLeft ) && (pTree->m_vHalf.x*2.f>fCellSize) )
		{
			nDivide = 1;
		}

		int nRight2, nLeft2;
		if ( OptimizeDivide2( pTree, vMax, vMin, 2, nRight2, nLeft2 ) && (pTree->m_vHalf.y*2.f>fCellSize) && !nDivide )
		{
			if ( abs(nRight-nLeft) > abs(nRight2-nLeft2) )
			{
				nRight = nRight2;
				nLeft = nLeft2;
				nDivide = 2;
			}
		}

		if ( OptimizeDivide2( pTree, vMax, vMin, 3, nRight2, nLeft2 ) && (pTree->m_vHalf.z*2.f>fCellSize) && !nDivide )
		{
			if ( abs(nRight-nLeft) > abs(nRight2-nLeft2) )
			{
				nRight = nRight2;
				nLeft = nLeft2;
				nDivide = 3;
			}
		}

		if ( !nRight || !nLeft || !nDivide )
			return FALSE;

		// Note : 작업
		pTree->m_pLeftChild = new DxPieceNode;
		pTree->m_pRightChild = new DxPieceNode;
		for (size_t i=0; i<pTree->m_vecPIECE.size(); ++i)
		{
			DxPiece* pCur = pTree->m_vecPIECE[i];

			switch( nDivide )
			{
			case 1:
				if( pCur->m_matWorld._41 > pTree->m_vCenterPoint.x )
				//if( vMinAABB.x > fHalfX )
				{
					pTree->m_pRightChild->m_vecPIECE.push_back( pCur );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPIECE.push_back( pCur );
				}
				break;
			case 2:
				if( pCur->m_matWorld._42 > pTree->m_vCenterPoint.y )
				//if( vMinAABB.y > fHalfY )
				{
					pTree->m_pRightChild->m_vecPIECE.push_back( pCur );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPIECE.push_back( pCur );
				}
				break;
			case 3:
				if( pCur->m_matWorld._43 > pTree->m_vCenterPoint.z )
				//if( vMinAABB.z > fHalfZ )
				{
					pTree->m_pRightChild->m_vecPIECE.push_back( pCur );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPIECE.push_back( pCur );
				}
				break;
			};
		}

		// Note : 옮겨가서 지워준다.
		pTree->m_vecPIECE.clear();

		return TRUE;
	}

	void MakeTreeChild( DxPieceNode* pTree, BOOL bCollision, float fCellSize )
	{
		// 설정된 Cell Size 에 들어왔기 때문에 그만 분할 하도록 한다.
		if ( (fCellSize > pTree->m_vHalf.x*2.f) && (fCellSize > pTree->m_vHalf.y*2.f) && (fCellSize > pTree->m_vHalf.z*2.f) )
		{
			return;
		}

		// FALSE 일 경우 더 이상 분할 불가
		int nDivide(0);
		if( !OptimizeDivide( pTree, pTree->m_vMax, pTree->m_vMin, bCollision, fCellSize, nDivide ) )
			return;

		DxPiece* pCur = NULL;
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vMax;
		D3DXVECTOR3 vMin;
		D3DXVECTOR3 vMaxPoint;
		D3DXVECTOR3 vMinPoint;
		D3DXVECTOR3 vMaxAABB;
		D3DXVECTOR3 vMinAABB;
		D3DXVECTOR3 vCenter;
		D3DXVECTOR3 vHalf;

		// Left
		vMaxPoint = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		vMinPoint = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		vMaxAABB = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		vMinAABB = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for (size_t i=0; i<pTree->m_pLeftChild->m_vecPIECE.size(); ++i)
		{
			DxPiece* pCur = pTree->m_pLeftChild->m_vecPIECE[i];

			if( vMaxPoint.x < pCur->m_matWorld._41 )	vMaxPoint.x = pCur->m_matWorld._41;
			if( vMaxPoint.y < pCur->m_matWorld._42 )	vMaxPoint.y = pCur->m_matWorld._42;
			if( vMaxPoint.z < pCur->m_matWorld._43 )	vMaxPoint.z = pCur->m_matWorld._43;

			if( vMinPoint.x > pCur->m_matWorld._41 )	vMinPoint.x = pCur->m_matWorld._41;
			if( vMinPoint.y > pCur->m_matWorld._42 )	vMinPoint.y = pCur->m_matWorld._42;
			if( vMinPoint.z > pCur->m_matWorld._43 )	vMinPoint.z = pCur->m_matWorld._43;

			//if (bCollision)
			//{
			//	vMax = pCur->m_vCollMax;
			//	vMin = pCur->m_vCollMin;
			//}
			//else
			{
				vMax = pCur->m_vMax;
				vMin = pCur->m_vMin;
			}
			D3DXMatrixAABBTransform( vMax, vMin, pCur->m_matWorld );


			if( vMaxAABB.x < vMax.x )	vMaxAABB.x = vMax.x;
			if( vMaxAABB.y < vMax.y )	vMaxAABB.y = vMax.y;
			if( vMaxAABB.z < vMax.z )	vMaxAABB.z = vMax.z;

			if( vMinAABB.x > vMin.x )	vMinAABB.x = vMin.x;
			if( vMinAABB.y > vMin.y )	vMinAABB.y = vMin.y;
			if( vMinAABB.z > vMin.z )	vMinAABB.z = vMin.z;
		}

		vMaxAABB += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		vMinAABB -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		vCenter = (vMaxAABB+vMinAABB)*0.5f;
		vHalf = vMaxAABB - vCenter;
		vHalf.x = fabsf( vHalf.x );
		vHalf.y = fabsf( vHalf.y );
		vHalf.z = fabsf( vHalf.z );

		pTree->m_pLeftChild->m_vMax			= vMaxAABB;
		pTree->m_pLeftChild->m_vMin			= vMinAABB;
		//pTree->m_pLeftChild->m_vMaxN		= vMaxAABB;
		//pTree->m_pLeftChild->m_vMinN		= vMinAABB;
		pTree->m_pLeftChild->m_vCenter		= vCenter;
		pTree->m_pLeftChild->m_vCenterPoint	= (vMaxPoint+vMinPoint)*0.5f;
		pTree->m_pLeftChild->m_vHalf		= vHalf;
		pTree->m_pLeftChild->m_fRadius	= D3DXVec3Length( &vHalf );

		MakeTreeChild( pTree->m_pLeftChild, bCollision, fCellSize );


		// Right
		vMaxPoint = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		vMinPoint = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		vMaxAABB = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		vMinAABB = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for (size_t i=0; i<pTree->m_pRightChild->m_vecPIECE.size(); ++i)
		{
			DxPiece* pCur = pTree->m_pRightChild->m_vecPIECE[i];

			if( vMaxPoint.x < pCur->m_matWorld._41 )	vMaxPoint.x = pCur->m_matWorld._41;
			if( vMaxPoint.y < pCur->m_matWorld._42 )	vMaxPoint.y = pCur->m_matWorld._42;
			if( vMaxPoint.z < pCur->m_matWorld._43 )	vMaxPoint.z = pCur->m_matWorld._43;

			if( vMinPoint.x > pCur->m_matWorld._41 )	vMinPoint.x = pCur->m_matWorld._41;
			if( vMinPoint.y > pCur->m_matWorld._42 )	vMinPoint.y = pCur->m_matWorld._42;
			if( vMinPoint.z > pCur->m_matWorld._43 )	vMinPoint.z = pCur->m_matWorld._43;

			//if (bCollision)
			//{
			//	vMax = pCur->m_vCollMax;
			//	vMin = pCur->m_vCollMin;
			//}
			//else
			{
				vMax = pCur->m_vMax;
				vMin = pCur->m_vMin;
			}
			D3DXMatrixAABBTransform( vMax, vMin, pCur->m_matWorld );

			if( vMaxAABB.x < vMax.x )	vMaxAABB.x = vMax.x;
			if( vMaxAABB.y < vMax.y )	vMaxAABB.y = vMax.y;
			if( vMaxAABB.z < vMax.z )	vMaxAABB.z = vMax.z;

			if( vMinAABB.x > vMin.x )	vMinAABB.x = vMin.x;
			if( vMinAABB.y > vMin.y )	vMinAABB.y = vMin.y;
			if( vMinAABB.z > vMin.z )	vMinAABB.z = vMin.z;
		}

		vMaxAABB += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		vMinAABB -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		vCenter = (vMaxAABB+vMinAABB)*0.5f;
		vHalf = vMaxAABB - vCenter;
		vHalf.x = fabsf( vHalf.x );
		vHalf.y = fabsf( vHalf.y );
		vHalf.z = fabsf( vHalf.z );

		pTree->m_pRightChild->m_vMax		= vMaxAABB;
		pTree->m_pRightChild->m_vMin		= vMinAABB;
		//pTree->m_pRightChild->m_vMaxN		= vMaxAABB;
		//pTree->m_pRightChild->m_vMinN		= vMinAABB;
		pTree->m_pRightChild->m_vCenter		= vCenter;
		pTree->m_pRightChild->m_vCenterPoint	= (vMaxPoint+vMinPoint)*0.5f;
		pTree->m_pRightChild->m_vHalf		= vHalf;
		pTree->m_pRightChild->m_fRadius	= D3DXVec3Length( &vHalf );

		// 만약 nDivide 가 0 이라면, CellSize 보다 큰 Object 가 존재한다는 의미이고,
		// 그 Object 들은 m_pRightChild 에 들어 있기 때문에 작업 할 필요가 없다.
		if ( nDivide != 0 )
		{
			MakeTreeChild( pTree->m_pRightChild, bCollision, fCellSize );
		}
	}

	void MakeTree ( PDXPIECENODE& pTree, const VEC_PIECE& vecPiece, float fCellSize )
	{
		SAFE_DELETE( pTree );
		pTree = new DxPieceNode;

		D3DXVECTOR3 vMax;
		D3DXVECTOR3 vMin;
		D3DXVECTOR3 vCenter;
		D3DXVECTOR3 vHalf;
		D3DXVECTOR3 vMaxPoint = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMinPoint = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		D3DXVECTOR3 vMaxAABB = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMinAABB = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );

		D3DXVECTOR3 vPos;
		for( DWORD i=0; i<vecPiece.size(); ++i )
		{
			DxPiece* pCur = vecPiece[i];

			if( vMaxPoint.x < pCur->m_matWorld._41 )	vMaxPoint.x = pCur->m_matWorld._41;
			if( vMaxPoint.y < pCur->m_matWorld._42 )	vMaxPoint.y = pCur->m_matWorld._42;
			if( vMaxPoint.z < pCur->m_matWorld._43 )	vMaxPoint.z = pCur->m_matWorld._43;

			if( vMinPoint.x > pCur->m_matWorld._41 )	vMinPoint.x = pCur->m_matWorld._41;
			if( vMinPoint.y > pCur->m_matWorld._42 )	vMinPoint.y = pCur->m_matWorld._42;
			if( vMinPoint.z > pCur->m_matWorld._43 )	vMinPoint.z = pCur->m_matWorld._43;

			pCur->SetID( i );		// 아이디 등록.

			vMax = pCur->m_vMax;
			vMin = pCur->m_vMin;
			D3DXMatrixAABBTransform( vMax, vMin, pCur->m_matWorld );

			if( vMaxAABB.x < vMax.x )	vMaxAABB.x = vMax.x;
			if( vMaxAABB.y < vMax.y )	vMaxAABB.y = vMax.y;
			if( vMaxAABB.z < vMax.z )	vMaxAABB.z = vMax.z;

			if( vMinAABB.x > vMin.x )	vMinAABB.x = vMin.x;
			if( vMinAABB.y > vMin.y )	vMinAABB.y = vMin.y;
			if( vMinAABB.z > vMin.z )	vMinAABB.z = vMin.z;

			pTree->m_vecPIECE.push_back( pCur );
		}

		// Note : 충돌을 위해서
		vMaxAABB += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		vMinAABB -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		vCenter = (vMaxAABB+vMinAABB)*0.5f;
		vHalf = vMaxAABB - vCenter;
		vHalf.x = fabsf( vHalf.x );
		vHalf.y = fabsf( vHalf.y );
		vHalf.z = fabsf( vHalf.z );

		pTree->m_vMax		= vMaxAABB;
		pTree->m_vMin		= vMinAABB;
		pTree->m_vCenter	= vCenter;
		pTree->m_vCenterPoint	= (vMaxPoint+vMinPoint)*0.5f;
		pTree->m_vHalf		= vHalf;
		pTree->m_fRadius	= D3DXVec3Length( &vHalf );

		MakeTreeChild( pTree, FALSE, fCellSize );
	}

	void OnResetDevice( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if ( !pTree )
			return;

		if ( !pTree->m_vecPIECE.empty() )
		{
			pTree->OnResetDevice( pd3dDevice );
		}

		OnResetDevice( pTree->m_pLeftChild, pd3dDevice );
		OnResetDevice( pTree->m_pRightChild, pd3dDevice );
	}

	void OnLostDevice( PDXPIECENODE& pTree )
	{
		if ( !pTree )
			return;

		if ( !pTree->m_vecPIECE.empty() )
		{
			pTree->OnLostDevice();
		}

		OnLostDevice( pTree->m_pLeftChild );
		OnLostDevice( pTree->m_pRightChild );
	}

	void InitInstancing( PDXPIECENODE& pTree, VEC_PIECE& vecInstancingExceptPiece )
	{
		if ( !pTree )
			return;

		// Instancing 작업하도록 한다.
		if ( !pTree->m_vecPIECE.empty() )
		{
			pTree->InitInstancing( vecInstancingExceptPiece );
		}

		InitInstancing( pTree->m_pLeftChild, vecInstancingExceptPiece );
		InitInstancing( pTree->m_pRightChild, vecInstancingExceptPiece );
	}

	void CreateVB_ForInstancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if ( !pTree )
			return;

		// Instancing 작업하도록 한다.
		if ( !pTree->m_vecPIECE.empty() )
		{
			pTree->CreateVB_ForInstancing( pd3dDevice );
		}

		CreateVB_ForInstancing( pTree->m_pLeftChild, pd3dDevice );
		CreateVB_ForInstancing( pTree->m_pRightChild, pd3dDevice );
	}

	void GetVecPiece( PDXPIECENODE& pTree, IN OUT VEC_PIECE& vecPiece )
	{
		if ( !pTree )
			return;

		for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		{
			vecPiece.push_back( pTree->m_vecPIECE[i] );
		}

		GetVecPiece( pTree->m_pLeftChild, vecPiece );
		GetVecPiece( pTree->m_pRightChild, vecPiece );
	}
	

	//void CreateUnderShader_2_0( PDXPIECENODE& pTree )
	//{
	//	if ( !pTree )
	//		return;

	//	// Instancing 작업하도록 한다.
	//	if ( !pTree->m_vecPIECE.empty() )
	//	{
	//		pTree->CreateUnderShader_2_0();
	//	}

	//	CreateUnderShader_2_0( pTree->m_pLeftChild );
	//	CreateUnderShader_2_0( pTree->m_pRightChild );
	//}

	//void FrameMove_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime, VEC_PIECE& vecInstancingExceptPiece )
	//{
	//	if ( !pTree )
	//		return;

	//	// Instancing 작업하도록 한다.
	//	if ( !pTree->m_vecPIECE.empty() )
	//	{
	//		pTree->FrameMove_Instancing( pd3dDevice, fElapsedTime, vecInstancingExceptPiece );
	//		return;
	//	}

	//	FrameMove_Instancing( pTree->m_pLeftChild, pd3dDevice, fElapsedTime, vecInstancingExceptPiece );
	//	FrameMove_Instancing( pTree->m_pRightChild, pd3dDevice, fElapsedTime, vecInstancingExceptPiece );
	//}

	void RenderOpaque_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
	{
		if ( !pTree )
			return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )
			return;

		if ( !pTree->m_vecPIECE.empty() )
		{
			pTree->RenderOpaque_Instancing( pd3dDevice, sCV );
			return;
		}

		RenderOpaque_Instancing( pTree->m_pLeftChild, pd3dDevice, sCV );
		RenderOpaque_Instancing( pTree->m_pRightChild, pd3dDevice, sCV );
	}

	void RenderHardAlpha_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
	{
		if ( !pTree )
			return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )
			return;

		if ( !pTree->m_vecPIECE.empty() )
		{
			pTree->RenderHardAlpha_Instancing( pd3dDevice, sCV );
			return;
		}

		RenderHardAlpha_Instancing( pTree->m_pLeftChild, pd3dDevice, sCV );
		RenderHardAlpha_Instancing( pTree->m_pRightChild, pd3dDevice, sCV );
	}

	void RenderEffFrame_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bLBuffer )
	{
		if ( !pTree )
			return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )
			return;

		if ( !pTree->m_vecPIECE.empty() )
		{
			pTree->RenderEffFrame_Instancing( pd3dDevice, sCV, bLBuffer );
			return;
		}

		RenderEffFrame_Instancing( pTree->m_pLeftChild, pd3dDevice, sCV, bLBuffer );
		RenderEffFrame_Instancing( pTree->m_pRightChild, pd3dDevice, sCV, bLBuffer );
	}

	void RenderDecal_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
	{
		if ( !pTree )
			return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )
			return;

		if ( !pTree->m_vecPIECE.empty() )
		{
			pTree->RenderDecal_Instancing( pd3dDevice, sCV );
			return;
		}

		RenderDecal_Instancing( pTree->m_pLeftChild, pd3dDevice, sCV );
		RenderDecal_Instancing( pTree->m_pRightChild, pd3dDevice, sCV );
	}

	void RenderDecalPass2_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
	{
		if ( !pTree )
			return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )
			return;

		if ( !pTree->m_vecPIECE.empty() )
		{
			pTree->RenderDecalPass2_Instancing( pd3dDevice, sCV );
			return;
		}

		RenderDecalPass2_Instancing( pTree->m_pLeftChild, pd3dDevice, sCV );
		RenderDecalPass2_Instancing( pTree->m_pRightChild, pd3dDevice, sCV );
	}

	void RenderWaterCustomSoftAlpha_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
	{
		if ( !pTree )
			return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )
			return;

		if ( !pTree->m_vecPIECE.empty() )
		{
			pTree->RenderWaterCustomSoftAlpha_Instancing( pd3dDevice, sCV );
			return;
		}

		RenderWaterCustomSoftAlpha_Instancing( pTree->m_pLeftChild, pd3dDevice, sCV );
		RenderWaterCustomSoftAlpha_Instancing( pTree->m_pRightChild, pd3dDevice, sCV );
	}

	void RenderEff_Instancing( PDXPIECENODE& pTree, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxStaticMeshLightMap* pStaticMeshLightMap )
	{
		if ( !pTree )
			return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )
			return;

		if ( !pTree->m_vecPIECE.empty() )
		{
			pTree->RenderEff_Instancing( pd3dDevice, sCV, pStaticMeshLightMap );
			return;
		}

		RenderEff_Instancing( pTree->m_pLeftChild, pd3dDevice, sCV, pStaticMeshLightMap );
		RenderEff_Instancing( pTree->m_pRightChild, pd3dDevice, sCV, pStaticMeshLightMap );
	}

	void InsertQuickSortALL( DxPieceNode* pTree,
		const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength, BOOL bLodBlending, BOOL bTimeBlend )
	{
		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

		for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		{
			if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vecPIECE[i]->GetCenter(), pTree->m_vecPIECE[i]->GetRadius() ) )
				continue;

			NSPieceQuickSort::InsertData( pTree->m_vecPIECE[i], vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
		}

		if( pTree->m_pLeftChild )	InsertQuickSortALL( pTree->m_pLeftChild, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
		if( pTree->m_pRightChild )	InsertQuickSortALL( pTree->m_pRightChild, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
	}

	void InsertQuickSortForShadow( DxPieceNode* pTree, const CLIPVOLUME &sCV )
	{
		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )
			return;

		for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		{
			if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vecPIECE[i]->GetCenter(), pTree->m_vecPIECE[i]->GetRadius() ) )
				continue;

			NSPieceQuickSort::InsertDataForShadow( pTree->m_vecPIECE[i] );
		}

		if( pTree->m_pLeftChild )	InsertQuickSortForShadow( pTree->m_pLeftChild, sCV );
		if( pTree->m_pRightChild )	InsertQuickSortForShadow( pTree->m_pRightChild, sCV );
	}

	//void InsertQuickSortForInstancing( DxPieceNode* pTree,
	//	const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength, BOOL bLodBlending, BOOL bTimeBlend )
	//{
	//	if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

	//	for ( DWORD i=0; i<pTree->m_vecInstancingExceptPiece.size(); ++i )
	//	{
	//		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vecInstancingExceptPiece[i]->GetCenter(), pTree->m_vecInstancingExceptPiece[i]->GetRadius() ) )
	//			continue;

	//		NSPieceQuickSort::InsertData( pTree->m_vecInstancingExceptPiece[i], vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
	//	}

	//	if( pTree->m_pLeftChild )	InsertQuickSortForInstancing( pTree->m_pLeftChild, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
	//	if( pTree->m_pRightChild )	InsertQuickSortForInstancing( pTree->m_pRightChild, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
	//}

	//void InsertQuickSortForUnderShader_2_0( DxPieceNode* pTree,
	//	const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength, BOOL bLodBlending )
	//{
	//	if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

	//	for ( DWORD i=0; i<pTree->m_vecUnderShader_2_0.size(); ++i )
	//	{
	//		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vecUnderShader_2_0[i]->GetCenter(), pTree->m_vecUnderShader_2_0[i]->GetRadius() ) )
	//			continue;

	//		NSPieceQuickSort::InsertData( pTree->m_vecUnderShader_2_0[i], vFromPt, vLookatPt, fLength, bLodBlending, FALSE );
	//	}

	//	if( pTree->m_pLeftChild )	InsertQuickSortForUnderShader_2_0( pTree->m_pLeftChild, sCV, vFromPt, vLookatPt, fLength, bLodBlending );
	//	if( pTree->m_pRightChild )	InsertQuickSortForUnderShader_2_0( pTree->m_pRightChild, sCV, vFromPt, vLookatPt, fLength, bLodBlending );
	//}

	void CollisionLine( DxPieceNode* pTree, const CLIPVOLUME &sCV, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3* pNor, D3DXVECTOR3& vCollision, BOOL& bColl, EMCOLLMODE emCullMode, BOOL bOnlyCamColl )
	{
		if( !COLLISION::IsCollisionLineToAABB( vStart, vEnd, pTree->m_vMax, pTree->m_vMin ) )
			return;

		for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		{
			if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vecPIECE[i]->GetCenter(), pTree->m_vecPIECE[i]->GetRadius() ) )
				continue;

			if( pTree->m_vecPIECE[i]->IsCollisionLine( vStart, vEnd, pNor, vCollision, emCullMode, bOnlyCamColl ) )	
				bColl = TRUE;
		}

		if( pTree->m_pLeftChild )	CollisionLine( pTree->m_pLeftChild, sCV, vStart, vEnd, pNor, vCollision, bColl, emCullMode, bOnlyCamColl );
		if( pTree->m_pRightChild )	CollisionLine( pTree->m_pRightChild, sCV, vStart, vEnd, pNor, vCollision, bColl, emCullMode, bOnlyCamColl );
	}

    void CollisionLine2( DxPieceNode* pTree,
						const D3DXVECTOR3& vStart, 
						D3DXVECTOR3& vEnd, 
						D3DXVECTOR3* pNor, 
						D3DXVECTOR3& vCollision, 
						BOOL& bColl, 
						PDXPIECE& rPiece,
                        EMCOLLMODE emCullMode, 
						BOOL bOnlyCamColl )
	{
		if( !COLLISION::IsCollisionLineToAABB( vStart, vEnd, pTree->m_vMax, pTree->m_vMin ) )	return;

		for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		{
			// 같은 Piece 는 충돌처리 안하도록 한다.
			// slope 기능에서 필요하다. DxPiece::CalculateSlopeMatrix
			if ( rPiece == pTree->m_vecPIECE[i] )
				continue;

		    //if( pCur->IsCollisionLine( vStart, vEnd, vCollision, FALSE, emCullMode ) )
            if( pTree->m_vecPIECE[i]->IsCollisionLine( vStart, vEnd, pNor, vCollision, emCullMode, bOnlyCamColl ) )
			{
				rPiece = pTree->m_vecPIECE[i];
				bColl = TRUE;
			}
	    }

		if( pTree->m_pLeftChild )	CollisionLine2( pTree->m_pLeftChild, vStart, vEnd, pNor, vCollision, bColl, rPiece, emCullMode, bOnlyCamColl );
		if( pTree->m_pRightChild )	CollisionLine2( pTree->m_pRightChild, vStart, vEnd, pNor, vCollision, bColl, rPiece, emCullMode, bOnlyCamColl );
	}

	BOOL IsCollisionLineSimple( DxPieceNode* pTree, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode )
	{
		if( !COLLISION::IsCollisionLineToAABB( vStart, vEnd, pTree->m_vMax, pTree->m_vMin ) )	
			return FALSE;

		for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		{
			if( pTree->m_vecPIECE[i]->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
				return TRUE;
		}

		if( pTree->m_pLeftChild )	
		{
			if ( IsCollisionLineSimple( pTree->m_pLeftChild, vStart, vEnd, emCullMode ) )
				return TRUE;
		}
		if( pTree->m_pRightChild )
		{
			if ( IsCollisionLineSimple( pTree->m_pRightChild, vStart, vEnd, emCullMode ) )
				return TRUE;
		}

		return FALSE;
	}

    // Note : 충돌이 되면 바로 리턴한다.
    BOOL IsCollisionLinePixel( DxPieceNode* pTree, LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV )
    {
        if( !COLLISION::IsCollisionLineToAABB( vStart, vEnd, pTree->m_vMax, pTree->m_vMin ) )	
            return FALSE;

		for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		{
			if( pTree->m_vecPIECE[i]->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, nIndexUV ) )
                return TRUE;
		}

		if( pTree->m_pLeftChild )	
        {
            if ( IsCollisionLinePixel( pTree->m_pLeftChild, pd3dDevice, vStart, vEnd, emCullMode, nIndexUV ) )
                return TRUE;
        }
		if( pTree->m_pRightChild )
        {
            if ( IsCollisionLinePixel( pTree->m_pRightChild, pd3dDevice, vStart, vEnd, emCullMode, nIndexUV ) )
                return TRUE;
        }

        return FALSE;
    }

	void GetLightMapColor( DxPieceNode* pTree, LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor, BOOL& bColl )
	{
		if( !COLLISION::IsCollisionLineToAABB( vStart, vEnd, pTree->m_vMax, pTree->m_vMin ) )	
			return;

		for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		{
			if ( pTree->m_vecPIECE[i]->GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor ) )
			{
				bColl = TRUE;
			}
		}

		if( pTree->m_pLeftChild )	
		{
			GetLightMapColor( pTree->m_pLeftChild, pd3dDevice, vStart, vEnd, dwColor, bColl );
		}
		if( pTree->m_pRightChild )
		{
			GetLightMapColor( pTree->m_pRightChild, pd3dDevice, vStart, vEnd, dwColor, bColl );
		}
	}

	void RenderCascadeShadowOpaque( DxPieceNode* pTree, 
									LPDIRECT3DDEVICEQ pd3dDevice, 
									const CLIPVOLUME &sCV, 
									std::vector<DxPiece*>& vecPieceForFrameAniHardAlpha, 
									std::vector<DxPiece*>& vecPieceForReplace )
	{
		//if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )
		//	return;

		//for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		//{
		//	pTree->m_vecPIECE[i]->RenderCascadeShadowOpaque( pd3dDevice, &sCV, vecPieceForFrameAniHardAlpha, vecPieceForReplace );
		//}

		//if( pTree->m_pLeftChild )	RenderCascadeShadowOpaque( pTree->m_pLeftChild, pd3dDevice, sCV, vecPieceForFrameAniHardAlpha, vecPieceForReplace );
		//if( pTree->m_pRightChild )	RenderCascadeShadowOpaque( pTree->m_pRightChild, pd3dDevice, sCV, vecPieceForFrameAniHardAlpha, vecPieceForReplace );
	}

	void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, const CLIPVOLUME &sCV, BOOL bPSF )
	{
		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

		for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		{
			if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vecPIECE[i]->GetCenter(), pTree->m_vecPIECE[i]->GetRadius() ) )
				continue;

			pTree->m_vecPIECE[i]->Render_Reflect( pd3dDevice, bPSF );
		}

		if( pTree->m_pLeftChild )	Render_Reflect( pd3dDevice, pTree->m_pLeftChild, sCV, bPSF);
		if( pTree->m_pRightChild )	Render_Reflect( pd3dDevice, pTree->m_pRightChild, sCV, bPSF );
	}

  //  void CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, const TSTRING& strLightMapName )
  //  {
		//for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		//{
		//	pTree->m_vecPIECE[i]->CreateLightMapPiece( pd3dDevice, strLightMapName );
		//}

  //      if( pTree->m_pLeftChild )	CreateLightMapPiece( pd3dDevice, pTree->m_pLeftChild, strLightMapName );
		//if( pTree->m_pRightChild )	CreateLightMapPiece( pd3dDevice, pTree->m_pRightChild, strLightMapName );
  //  }

  //  void LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree )
  //  {
		//for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
		//{
		//	pTree->m_vecPIECE[i]->LoadLightMapForRAD( pd3dDevice );
		//}

  //      if( pTree->m_pLeftChild )	LoadLightMap( pd3dDevice, pTree->m_pLeftChild );
		//if( pTree->m_pRightChild )	LoadLightMap( pd3dDevice, pTree->m_pRightChild );
  //  }

	void Save( DxPieceNode* pNode, sc::SerialFile& SFile )
	{
		BOOL bExist = FALSE;
		if( pNode )
		{
			bExist = TRUE;
			SFile << bExist;
		}
		else
		{
			bExist = FALSE;
			SFile << bExist;

			return;
		}

		pNode->Save( SFile );
	}

	void Load( LPDIRECT3DDEVICEQ pd3dDevice, PDXPIECENODE& pNode, sc::SerialFile& SFile, const VEC_PIECE& vecPiece )
	{
		BOOL bExist = FALSE;
		SFile >> bExist;
		if( !bExist )	return;

		pNode = new DxPieceNode;
		pNode->Load( pd3dDevice, SFile, vecPiece );
	}

    void Load_PREV( LPDIRECT3DDEVICEQ pd3dDevice, PDXPIECENODE& pNode, sc::SerialFile& SFile, DxPieceEdit* pSrc, VEC_PIECE& vecPiece )
	{
		BOOL bExist = FALSE;
		SFile >> bExist;
		if( !bExist )	return;

		pNode = new DxPieceNode;
		pNode->Load_PREV( pd3dDevice, SFile, pSrc, vecPiece );
	}

	void Load_PREV2( LPDIRECT3DDEVICEQ pd3dDevice, PDXPIECENODE& pNode, sc::SerialFile& SFile, DxPieceEdit* pSrc, VEC_PIECE& vecPiece )
	{
		BOOL bExist = FALSE;
		SFile >> bExist;
		if( !bExist )	return;

		pNode = new DxPieceNode;
		pNode->Load_PREV2( pd3dDevice, SFile, pSrc, vecPiece );
	}

	void LoadBasicAABB( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
	{
		if ( !COLLISION::IsCollisionAABBToAABB( vMax, vMin, pTree->m_vMax, pTree->m_vMin ) )
			return;

		DxPiece* pCur(NULL);
		size_t dwSize = pTree->m_vecPIECE.size();
		for (size_t i=0; i<dwSize; ++i)
		{
			pCur = pTree->m_vecPIECE[i];
			pCur->LoadBasicAABB( pd3dDevice );
		}

		if( pTree->m_pLeftChild )	LoadBasicAABB( pd3dDevice, pTree->m_pLeftChild, vMax, vMin );
		if( pTree->m_pRightChild )	LoadBasicAABB( pd3dDevice, pTree->m_pRightChild, vMax, vMin );
	}

	//void ExportXFile( DxPieceNode* pTree, LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot )
	//{
	//	for ( DWORD i=0; i<pTree->m_vecPIECE.size(); ++i )
	//	{
	//		pTree->m_vecPIECE[i]->ExportXFile( pxofsave, pDataObjectRoot );
	//	}

	//	if( pTree->m_pLeftChild )	ExportXFile( pTree->m_pLeftChild, pxofsave, pDataObjectRoot );
	//	if( pTree->m_pRightChild )	ExportXFile( pTree->m_pRightChild, pxofsave, pDataObjectRoot );
	//}
}
