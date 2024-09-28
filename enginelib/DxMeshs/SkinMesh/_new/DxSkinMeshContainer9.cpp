#include "pch.h"

#include "../../../../SigmaCore/Log/LogMan.h"
#include "../../../../SigmaCore/File/SerialFile.h"

#include "../../../DxMeshs/SkinMesh/_new/DxSkinMesh9_NORMAL.h"
#include "../../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../../DxTools/MaterialSkin/DxMultiMaterialSkinMesh.h"
#include "../../../DxTools/TextureManager.h"
#include "../../../DxTools/RENDERPARAM.h"
#include "../../../DxTools/Collision.h"

#include "../DxSkeletonMaintain.h"
#include "../DxSkeletonManager.h"
#include "../DxMaterial.h"

#include "./NsSMeshSceneGraph.h"
#include "./DxSkinMesh9_HLSL.h"
#include "./DxSkinVB_ATT.h"
#include "./DxSkinMesh9.h"

#include "DxSkinMeshContainer9.h"


// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


SBoneInfluence::SBoneInfluence () :
	dwInfue(0)
{
}

void SBoneInfluence::SetInfue ( DWORD _dwInfue )
{
	dwInfue = _dwInfue;
	GASSERT(dwInfue<=MAX_BONE);
}

//------------------------------------------------------------------------------------[SVERTEXINFLU]
//
const DWORD SVERTEXINFLU::VERSION = 0x0101;

SVERTEXINFLU::SVERTEXINFLU () :
	m_dwIndex(COL_ERR),
	m_vVector(0,0,0),
	m_vNormal(0,0,0),
	
	m_dwNumBone(COL_ERR)
{
}

void SVERTEXINFLU::SetBoneNum ( DWORD _dwBone )
{
	if ( _dwBone==COL_ERR )
	{
		m_dwNumBone = _dwBone;
		return;
	}

	m_dwNumBone = _dwBone;
	GASSERT(m_dwNumBone<=MAX_BONE);
}

BOOL SVERTEXINFLU::SaveFile ( sc::SerialFile &SFile )
{
	DWORD dwSize = sizeof(DWORD) +
					sizeof(D3DXVECTOR3) +
					sizeof(D3DXVECTOR3) +
					sizeof(DWORD) +
					sizeof(DWORD)*m_dwNumBone +
					sizeof(float)*m_dwNumBone;
	
	if ( m_dwNumBone > 0 && m_dwNumBone!=COL_ERR )
	{
		GASSERT(m_pBone);
		GASSERT(m_pWeights);

        dwSize += sizeof(DWORD)*m_dwNumBone + sizeof(float)*m_dwNumBone;
	}

	SFile << VERSION;
	SFile << dwSize;

	SFile << m_dwIndex;
	SFile << m_vVector;
	SFile << m_vNormal;

	SFile << m_dwNumBone;

	if ( m_dwNumBone > 0 && m_dwNumBone!=COL_ERR )
	{
		SFile.WriteBuffer ( m_pBone, sizeof(DWORD)*m_dwNumBone );
		SFile.WriteBuffer ( m_pWeights, sizeof(float)*m_dwNumBone );
	}

	return TRUE;
}

BOOL SVERTEXINFLU::LoadFile ( sc::BaseStream &SFile )
{
	DWORD dwVer;
	DWORD dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	if ( dwVer == SVERTEXINFLU::VERSION )
	{
		SFile >> m_dwIndex;
		SFile >> m_vVector;
		SFile >> m_vNormal;

		SFile >> m_dwNumBone;

		SetBoneNum(m_dwNumBone);	//	점검 차원에서 호출.
		if ( m_dwNumBone > 0 && m_dwNumBone != COL_ERR )
		{
			SFile.ReadBuffer ( m_pBone, sizeof(DWORD)*m_dwNumBone );
			SFile.ReadBuffer ( m_pWeights, sizeof(float)*m_dwNumBone );
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwSize );
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------
//			M	E	S	H		D	A	T	A		_		H	L	S	L
//---------------------------------------------------------------------------------------
MESHDATA_HLSL::MESHDATA_HLSL()
	: m_pBoneCombinationBuf(NULL)
{
	m_MeshData.pMesh = NULL;
}

MESHDATA_HLSL::~MESHDATA_HLSL()
{
	CleanUp();
}

void MESHDATA_HLSL::CleanUp()
{
	SAFE_RELEASE( m_pBoneCombinationBuf );
	SAFE_RELEASE( m_MeshData.pMesh );
}

//--------------------------------------------------------------------------------------
// Called either by CreateMeshContainer when loading a skin mesh, or when 
// changing methods.  This function uses the pSkinInfo of the mesh 
// container to generate the desired drawable mesh and bone combination 
// table.
//-------------------------------------------------------------------------------------
DWORD SMeshContainer::g_dwStaticBoneMatrixNum = 2;

SMeshContainer::~SMeshContainer()
{
    SAFE_DELETE_ARRAY( Name );

    SAFE_RELEASE( MeshData.pMesh );

    for( UINT i=0; i<NumMaterials; ++i )
		SAFE_DELETE_ARRAY( pMaterials[ i ].pTextureFilename );
    SAFE_DELETE_ARRAY( pMaterials );
    SAFE_DELETE_ARRAY( pAdjacency );

    SAFE_RELEASE( pSkinInfo );

    SAFE_DELETE_ARRAY( m_pMaterialEx );

    SAFE_RELEASE( m_pOrigMesh );
	//SAFE_DELETE_ARRAY( pAttributeTable );

    SAFE_RELEASE( m_pBoneCombinationBuf );

    //SAFE_DELETE_ARRAY( m_ppBoneMatrixPtrs );
	SAFE_DELETE_ARRAY( m_ppStaticMatrixPtrs );
	SAFE_DELETE_ARRAY( m_arrayBoneIndex );
    SAFE_DELETE_ARRAY( m_pBoneOffsetMatrices );

	SAFE_DELETE_ARRAY ( m_arrayfInvWeight );

   // SAFE_DELETE_ARRAY( m_pVB_DATA );

	//SAFE_DELETE( m_pSlimMesh );
	//SAFE_DELETE( m_pEdgeMeshHLSL );

	//SAFE_DELETE( m_pNormalMesh );
}

LPDIRECT3DTEXTUREQ SMeshContainer::SetMaterial ( LPDIRECT3DDEVICEQ pd3dDevice, 
												DWORD nIndex, 
												BOOL& bZBiasChange, 
												SMATERIAL_PIECE *pmtrlPiece, 
												DXMATERIAL_CHAR_EFF* pmtrlSpecular, 
												LPDIRECT3DTEXTUREQ* apOverlayTex )
{
	if( nIndex >= NumMaterials )	return NULL;

	//pMaterials[nIndex].MatD3D.Emissive.a = 0.f;
	//pMaterials[nIndex].MatD3D.Emissive.r = 0.f;
	//pMaterials[nIndex].MatD3D.Emissive.g = 0.f;
	//pMaterials[nIndex].MatD3D.Emissive.b = 0.f;

	//pMaterials[nIndex].MatD3D.Power = 14.f;						// 30.f - 디폴트

	float fZBias(0.f);
	D3DMATERIALQ*		pMatD3D(NULL);
	LPDIRECT3DTEXTUREQ	pTexture(NULL);

	if( pmtrlSpecular )
	{
		if( pmtrlSpecular[nIndex].IsType( EMCET_BASE ) )
		{
			pMatD3D = &(pmtrlSpecular[nIndex].d3dMaterial);
			pTexture = pmtrlSpecular[nIndex].GetTexture();
			fZBias = 0.00002f;

			D3DCOLOR dwColor = D3DCOLOR_COLORVALUE ( pMatD3D->Diffuse.r, pMatD3D->Diffuse.g,
											pMatD3D->Diffuse.b, pMatD3D->Diffuse.a );

			pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, dwColor );	// 음. ^^;
		}
		else if ( pmtrlSpecular[nIndex].IsType( EMCET_TEXTURE_MATRIX ) )
		{
			pTexture = pmtrlSpecular[nIndex].GetTexture();
		}
	}
	else if( pmtrlPiece )
	{
		if ( pmtrlPiece[nIndex].m_textureRes.GetTexture() )
		{
			pTexture = pmtrlPiece[nIndex].m_textureRes.GetTexture();
		}
	
		fZBias = pmtrlPiece[nIndex].m_fZBias;
	}

	// WorldBattle 때문에 추가 되었다.
	// RenderTarget 으로 변경되는 Texture 들은 이걸로 하면 좋겠다.
	if ( apOverlayTex && apOverlayTex[nIndex] )
	{
		pTexture = apOverlayTex[nIndex];
	}

	// Base Texture
	if( !pTexture )
	{
		pTexture = m_pMaterialEx[nIndex].m_textureRes.GetTexture();
	}

	if( fZBias>0.f )
	{
		float fBias = -fZBias;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		bZBiasChange = TRUE;
	}
	//pd3dDevice->SetMaterial ( &pMaterials[nIndex].MatD3D );
	pd3dDevice->SetTexture ( 0, pTexture );

	return pTexture;
}

HRESULT SMeshContainer::ReSetMaterial ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	float fBias = -0.00000f;
	pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );

	return S_OK;
}

BOOL SMeshContainer::IsAlphaTex_HARD ( DWORD nIndex )
{
	if( nIndex >= NumMaterials )	return FALSE;
	return (m_pMaterialEx[nIndex].dwFlags&DXMATERIALEX::FLAG_ALPHA_HARD)!=NULL;
}

BOOL SMeshContainer::IsAlphaTex_SOFT ( DWORD nIndex )
{
	if( nIndex >= NumMaterials )	return FALSE;
	return (m_pMaterialEx[nIndex].dwFlags&DXMATERIALEX::FLAG_ALPHA_SOFT)!=NULL;
}
//
//BOOL SMeshContainer::IsCollision ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vDetectPos, DWORD &dwDetectFace )
//{
//	HRESULT hr = S_OK;
//	float fDistNew = FLT_MAX, fDistOld = FLT_MAX;
//	D3DXMATRIX matCombined;
//
//	dwDetectFace = COL_ERR;
//	vDetectPos = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
//
//	if ( !MeshData.pMesh && !pSkinInfo )		return FALSE;
//
//	if ( pSkinInfo )
//	{
//		if ( MeshData.pMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;
//
//		DWORD dwNumFaces = MeshData.pMesh->GetNumFaces ();
//		DWORD dwNumVertices = MeshData.pMesh->GetNumVertices ();
//		
//		DWORD dwSkinFVF = MeshData.pMesh->GetFVF ();
//		DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );
//
//		WORD *pIndexBuffer;
//		hr = MeshData.pMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndexBuffer );
//		if ( FAILED(hr) )						return FALSE;
//
//		BYTE *pbData = NULL;
//		hr = MeshData.pMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
//		if ( FAILED(hr) )
//		{
//			MeshData.pMesh->UnlockIndexBuffer ();
//			return FALSE;
//		}
//
//		DWORD dwCollisionFace = COL_ERR;
//		D3DXVECTOR3 vDetect = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
//		for ( DWORD j=0; j<dwNumFaces; j++ )
//		{
//			PWORD pwIndex = pIndexBuffer + 3*j;
//
//			D3DXVECTOR3 *pVector0 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+0) ) );
//			D3DXVECTOR3 *pVector1 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+1) ) );
//			D3DXVECTOR3 *pVector2 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+2) ) );
//
//			D3DXVECTOR3 vCollisionPos;
//			BOOL bCollision = COLLISION::IsLineTriangleCollision
//			(
//				(D3DXVECTOR3*)pVector0,
//				(D3DXVECTOR3*)pVector1,
//				(D3DXVECTOR3*)pVector2,
//				(D3DXVECTOR3*)&vPoint1,
//				(D3DXVECTOR3*)&vPoint2,
//				&vCollisionPos,
//				NULL, 
//				EMCC_CULL_CCW, 
//				TRUE
//			);
//
//			if ( bCollision )
//			{
//				D3DXVECTOR3 vDist = vPoint1 - vCollisionPos;
//				fDistNew = D3DXVec3Length ( &vDist );
//				
//				if ( fDistNew <= fDistOld )
//				{
//					dwCollisionFace = j;
//					vDetect = vCollisionPos;
//
//					fDistOld = fDistNew;
//				}
//			}
//		}
//
//		MeshData.pMesh->UnlockVertexBuffer ();
//		MeshData.pMesh->UnlockIndexBuffer ();
//
//		vDetectPos = vDetect;
//		dwDetectFace = dwCollisionFace;
//
//	}
//	//	Note : 일반 메시.
//	//
//	else
//	{
//		if ( MeshData.pMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;
//
//		DWORD dwNumFaces = MeshData.pMesh->GetNumFaces ();
//		DWORD dwNumVertices = MeshData.pMesh->GetNumVertices ();
//
//		DWORD dwFVF = MeshData.pMesh->GetFVF ();
//		DWORD dwVertSize = D3DXGetFVFVertexSize ( dwFVF );
//
//		BYTE *pbData = NULL;
//		hr = MeshData.pMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
//		if ( FAILED(hr) )	return FALSE;
//
//		WORD *pIndexBuffer;
//		hr = MeshData.pMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndexBuffer );
//		if ( FAILED(hr) )
//		{
//			MeshData.pMesh->UnlockVertexBuffer ();
//			return FALSE;
//		}
//
//		if ( m_dwNumBoneMatrix == SMeshContainer::g_dwStaticBoneMatrixNum && m_ppBoneMatrixPtrs )
//		{
//			//	특정 본에 영향을 받아야하지만 그 본이 존제 안할 경우가 있을때 대비.
//			D3DXMATRIX	matTemp;
//			if ( m_ppBoneMatrixPtrs[2] )
//			{
//				D3DXMatrixMultiply ( &matTemp, m_ppBoneMatrixPtrs[1], m_ppBoneMatrixPtrs[2] );
//				D3DXMatrixMultiply ( &matCombined, m_ppBoneMatrixPtrs[0], &matTemp );
//			}
//		}
//
//		DWORD dwCollisionFace = COL_ERR;
//		D3DXVECTOR3 vDetect = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
//		for ( DWORD j=0; j<dwNumFaces; j++ )
//		{
//			PWORD pwIndex = pIndexBuffer+0 + 3*j;
//
//			D3DXVECTOR3 Vector0 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+0)) );
//			D3DXVECTOR3 Vector1 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+1)) );
//			D3DXVECTOR3 Vector2 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+2)) );
//
//			//D3DXVec3TransformCoord ( &Vector0, &Vector0, &matCombined );
//			//D3DXVec3TransformCoord ( &Vector1, &Vector1, &matCombined );
//			//D3DXVec3TransformCoord ( &Vector2, &Vector2, &matCombined );
//
//			D3DXVECTOR3 vCollisionPos;
//			BOOL bCollision = COLLISION::IsLineTriangleCollision
//			(
//				(D3DXVECTOR3*)&Vector0,
//				(D3DXVECTOR3*)&Vector1,
//				(D3DXVECTOR3*)&Vector2,
//				(D3DXVECTOR3*)&vPoint1,
//				(D3DXVECTOR3*)&vPoint2,
//				&vCollisionPos,
//				NULL, 
//				EMCC_CULL_CCW, 
//				TRUE
//			);
//
//			if ( bCollision )
//			{
//				D3DXVECTOR3 vDist = vPoint1 - vCollisionPos;
//				fDistNew = D3DXVec3Length ( &vDist );
//				
//				if ( fDistNew <= fDistOld )
//				{
//					dwCollisionFace = j;
//					vDetect = vCollisionPos;
//
//					fDistOld = fDistNew;
//				}
//			}
//		}
//
//		vDetectPos = vDetect;
//		dwDetectFace = dwCollisionFace;
//
//		MeshData.pMesh->UnlockVertexBuffer ();
//		MeshData.pMesh->UnlockIndexBuffer ();
//	}
//
//	return (dwDetectFace!=COL_ERR);
//}

// [shhan][2014.11.28] 이부분에서는 Mesh 에 Lock 을 걸 수 있는 이유는 Main Thread 에서 작업하기 때문이다.
//						MainThread 가 아닌 곳에서 이함수를 호출한다면 Mesh 들이 깜빡이면서 렌더링이 되었다 안되었다가 반복될 것이다.
//
BOOL SMeshContainer::GetClosedPoint2 ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vDetectPos, DWORD &dwDetectIndex, 
									  D3DXVECTOR3& vCLOSED_POINT, D3DXVECTOR3& vCLOSED_NORMAL, const DxSkeleton* pSkeleton, const D3DXMATRIX& matStaticLinkBone )
{
	HRESULT hr = S_OK;
	float fDistNew = FLT_MAX, fDistOld = FLT_MAX;
	D3DXMATRIX matCombined;

	dwDetectIndex = COL_ERR;

	if ( pSkinInfo )
	{
		if ( !m_pOrigMesh )								return FALSE;
		if ( m_pOrigMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;

		DWORD dwNumFaces = m_pOrigMesh->GetNumFaces ();
		DWORD dwNumVertices = m_pOrigMesh->GetNumVertices ();

		DWORD dwSkinFVF = m_pOrigMesh->GetFVF ();
		DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );

		DWORD dwFVF_VERTEX = dwSkinFVF&D3DFVF_POSITION_MASK;
		DWORD dwSize_VERTEX = D3DXGetFVFVertexSize( dwFVF_VERTEX );

		PBYTE pbData = new BYTE[dwSkinVertSize*dwNumVertices];
		hr = UpdateSkinnedMesh( pbData, dwSkinFVF, NULL, pSkeleton );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pbData);
			return FALSE;
		}

		WORD *pIndexBuffer;
		hr = m_pOrigMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			return FALSE;
		}

		DWORD dwCollisionFace = COL_ERR;
		D3DXVECTOR3 vDetect = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( DWORD j=0; j<dwNumFaces; j++ )
		{
			PWORD pwIndex = pIndexBuffer + 3*j;

			D3DXVECTOR3 *pVector0 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+0) ) );
			D3DXVECTOR3 *pVector1 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+1) ) );
			D3DXVECTOR3 *pVector2 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+2) ) );

			D3DXVECTOR3 vCollisionPos;
			BOOL bCollision = COLLISION::IsLineTriangleCollision
			(
				(D3DXVECTOR3*)pVector0,
				(D3DXVECTOR3*)pVector1,
				(D3DXVECTOR3*)pVector2,
				(D3DXVECTOR3*)&vPoint1,
				(D3DXVECTOR3*)&vPoint2,
				&vCollisionPos,
				NULL, 
				EMCC_CULL_CCW, 
				TRUE
			);

			if ( bCollision )
			{
				D3DXVECTOR3 vDist = vPoint1 - vCollisionPos;
				fDistNew = D3DXVec3Length ( &vDist );
				
				if ( fDistNew <= fDistOld )
				{
					dwCollisionFace = j;
					vDetect = vCollisionPos;

					fDistOld = fDistNew;

					/*D3DXVECTOR3 a(1.0f, 0.0f, 1.0f);
					D3DXVECTOR3 b(1.0f, 0.0f, 0.0f);
					D3DXVECTOR3 c(0.0f, 0.0f, 1.0f);*/
					D3DXVECTOR3 a  = *pVector0;
					D3DXVECTOR3 b  = *pVector1;
					D3DXVECTOR3 c  = *pVector2;
					D3DXVECTOR3 ab = a - b;
					D3DXVECTOR3 ac = a - c;
					D3DXVec3Cross( &vCLOSED_NORMAL, &ab, &ac );
					D3DXVec3Normalize( &vCLOSED_NORMAL, &vCLOSED_NORMAL );
				}
			}
		}
		m_pOrigMesh->UnlockIndexBuffer ();

		if ( dwCollisionFace == COL_ERR )
		{
			return FALSE;
		}

		vCLOSED_POINT = vDetect;	// Note : 실제 찍혀 있는 위치를 넣는다.

		DWORD dwClosed = COL_ERR;
		D3DXVECTOR3 vClosed = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		fDistNew = FLT_MAX;
		fDistOld = FLT_MAX;
		for ( DWORD i=0; i<dwNumVertices; i++ )
		{
			D3DXVECTOR3 Vector0 = *(D3DXVECTOR3*) ( pbData + dwSkinVertSize*i );

			D3DXVECTOR3 vDist = Vector0 - vDetect;
			fDistNew = D3DXVec3Length ( &vDist );
			
			if ( fDistNew < fDistOld )
			{
				dwClosed = i;
				vClosed = Vector0;

				fDistOld = fDistNew;

				//vCLOSED_NORMAL = *(D3DXVECTOR3*)( pbData + dwSkinVertSize*i + dwSize_VERTEX );			
			}
		}

		vDetectPos = vClosed;
		dwDetectIndex = dwClosed;

		SAFE_DELETE_ARRAY( pbData );
	}
	//	Note : 일반 메시.
	//
	else
	{
		if ( !MeshData.pMesh )								return FALSE;
		if ( MeshData.pMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;

		DWORD dwNumFaces = MeshData.pMesh->GetNumFaces ();
		DWORD dwNumVertices = MeshData.pMesh->GetNumVertices ();

		DWORD dwFVF = MeshData.pMesh->GetFVF ();
		DWORD dwVertSize = D3DXGetFVFVertexSize ( dwFVF );

		DWORD dwFVF_VERTEX = dwFVF&D3DFVF_POSITION_MASK;
		DWORD dwSize_VERTEX = D3DXGetFVFVertexSize( dwFVF_VERTEX );

		BYTE *pbData = NULL;
		hr = MeshData.pMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
		if ( FAILED(hr) )	return FALSE;

		WORD *pIndexBuffer;
		hr = MeshData.pMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			MeshData.pMesh->UnlockVertexBuffer ();
			return FALSE;
		}

        /*
		if ( dwNumBoneMatrix == 3 && ppBoneMatrixPtrs )
		{
			//	특정 본에 영향을 받아야하지만 그 본이 존제 안할 경우가 있을때 대비.
			D3DXMATRIX	matTemp;
			if ( ppBoneMatrixPtrs[2] )
			{
				D3DXMatrixMultiply ( &matTemp, ppBoneMatrixPtrs[1], ppBoneMatrixPtrs[2] );
				D3DXMatrixMultiply ( &matCombined, ppBoneMatrixPtrs[0], &matTemp );
			}
		}
        */

        //const D3DXMATRIX& mat = m_matStaticLinkBone;
        //D3DXMatrixMultiply( &mat, m_ppStaticMatrixPtrs[1], m_ppBoneMatrixPtrs[2] );
        //D3DXMatrixMultiply( &mat, m_ppStaticMatrixPtrs[0], &mat );

		DWORD dwCollisionFace = COL_ERR;
		D3DXVECTOR3 vDetect = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( DWORD j=0; j<dwNumFaces; j++ )
		{
			PWORD pwIndex = pIndexBuffer+0 + 3*j;

			D3DXVECTOR3 Vector0 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+0)) );
			D3DXVECTOR3 Vector1 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+1)) );
			D3DXVECTOR3 Vector2 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+2)) );

            D3DXVec3TransformCoord( &Vector0, &Vector0, &matStaticLinkBone );
            D3DXVec3TransformCoord( &Vector1, &Vector1, &matStaticLinkBone );
            D3DXVec3TransformCoord( &Vector2, &Vector2, &matStaticLinkBone );

			D3DXVECTOR3 vCollisionPos;
			BOOL bCollision = COLLISION::IsLineTriangleCollision
			(
				(D3DXVECTOR3*)&Vector0,
				(D3DXVECTOR3*)&Vector1,
				(D3DXVECTOR3*)&Vector2,
				(D3DXVECTOR3*)&vPoint1,
				(D3DXVECTOR3*)&vPoint2,
				&vCollisionPos,
				NULL, 
				EMCC_CULL_CCW, 
				TRUE
			);

			if ( bCollision )
			{
				D3DXVECTOR3 vDist = vPoint1 - vCollisionPos;
				fDistNew = D3DXVec3Length ( &vDist );
				
				if ( fDistNew <= fDistOld )
				{
					dwCollisionFace = j;
					vDetect = vCollisionPos;

					fDistOld = fDistNew;

					D3DXVECTOR3 ab = Vector0 - Vector1;
					D3DXVECTOR3 ac = Vector0 - Vector2;
					D3DXVec3Cross( &vCLOSED_NORMAL, &ab, &ac );
					D3DXVec3Normalize( &vCLOSED_NORMAL, &vCLOSED_NORMAL );
				}
			}
		}

		vCLOSED_POINT = vDetect;	// Note : 실제 찍혀 있는 위치를 넣는다.

		if ( dwCollisionFace == COL_ERR )
		{
			MeshData.pMesh->UnlockVertexBuffer ();
			MeshData.pMesh->UnlockIndexBuffer ();
			return FALSE;
		}

		DWORD dwClosed = COL_ERR;
		D3DXVECTOR3 vClosed = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		fDistNew = FLT_MAX;
		fDistOld = FLT_MAX;
		for ( DWORD i=0; i<dwNumVertices; i++ )
		{
			D3DXVECTOR3 Vector0 = *(D3DXVECTOR3*) ( pbData + dwVertSize*i );

            D3DXVec3TransformCoord( &Vector0, &Vector0, &matStaticLinkBone );

			D3DXVECTOR3 vDist = Vector0 - vDetect;
			fDistNew = D3DXVec3Length ( &vDist );
			
			if ( fDistNew < fDistOld )
			{
				dwClosed = i;
				vClosed = Vector0;

				fDistOld = fDistNew;

//				vCLOSED_NORMAL = *(D3DXVECTOR3*)( pbData + dwVertSize*i + dwSize_VERTEX );				
			}
		}

		MeshData.pMesh->UnlockVertexBuffer ();
		MeshData.pMesh->UnlockIndexBuffer ();

		vDetectPos = vClosed;
		dwDetectIndex = dwClosed;
		D3DXVec3Cross( &vCLOSED_NORMAL, &vClosed, &vDetect );
	}

	return (dwDetectIndex!=COL_ERR);

}


// [shhan][2014.11.28] 이부분에서는 Mesh 에 Lock 을 걸 수 있는 이유는 Main Thread 에서 작업하기 때문이다.
//						MainThread 가 아닌 곳에서 이함수를 호출한다면 Mesh 들이 깜빡이면서 렌더링이 되었다 안되었다가 반복될 것이다.
//
BOOL SMeshContainer::GetClosedPoint ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vDetectPos, DWORD &dwDetectIndex, 
									 D3DXVECTOR3& vCLOSED_POINT, D3DXVECTOR3& vCLOSED_NORMAL, const DxSkeleton* pSkeleton )
{
	HRESULT hr = S_OK;
	float fDistNew = FLT_MAX, fDistOld = FLT_MAX;
	//D3DXMATRIX matCombined;

	dwDetectIndex = COL_ERR;
	vDetectPos = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	if ( pSkinInfo )
	{
		if ( !m_pOrigMesh )								return FALSE;
		if ( m_pOrigMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;

		DWORD dwNumFaces = m_pOrigMesh->GetNumFaces ();
		DWORD dwNumVertices = m_pOrigMesh->GetNumVertices ();

		DWORD dwSkinFVF = m_pOrigMesh->GetFVF ();
		DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );

		DWORD dwFVF_VERTEX = dwSkinFVF&D3DFVF_POSITION_MASK;
		DWORD dwSize_VERTEX = D3DXGetFVFVertexSize( dwFVF_VERTEX );

		PBYTE pbData = new BYTE[dwSkinVertSize*dwNumVertices];
		hr = UpdateSkinnedMesh( pbData, dwSkinFVF, NULL, pSkeleton );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pbData);
			return FALSE;
		}

		WORD *pIndexBuffer;
		hr = m_pOrigMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pbData);
			return FALSE;
		}

		DWORD dwCollisionFace = COL_ERR;
		D3DXVECTOR3 vDetect = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( DWORD j=0; j<dwNumFaces; j++ )
		{
			PWORD pwIndex = pIndexBuffer + 3*j;

			D3DXVECTOR3 *pVector0 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+0) ) );
			D3DXVECTOR3 *pVector1 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+1) ) );
			D3DXVECTOR3 *pVector2 = (D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+2) ) );

			D3DXVECTOR3 vCollisionPos;
			BOOL bCollision = COLLISION::IsLineTriangleCollision
			(
				(D3DXVECTOR3*)pVector0,
				(D3DXVECTOR3*)pVector1,
				(D3DXVECTOR3*)pVector2,
				(D3DXVECTOR3*)&vPoint1,
				(D3DXVECTOR3*)&vPoint2,
				&vCollisionPos,
				NULL, 
				EMCC_CULL_CCW, 
				TRUE
			);

			if ( bCollision )
			{
				D3DXVECTOR3 vDist = vPoint1 - vCollisionPos;
				fDistNew = D3DXVec3Length ( &vDist );
				
				if ( fDistNew <= fDistOld )
				{
					dwCollisionFace = j;
					vDetect = vCollisionPos;

					fDistOld = fDistNew;
				}
			}
		}
		m_pOrigMesh->UnlockIndexBuffer ();

		if ( dwCollisionFace == COL_ERR )	
		{
			SAFE_DELETE_ARRAY(pbData);
			return FALSE;
		}

		vCLOSED_POINT = vDetect;	// Note : 실제 찍혀 있는 위치를 넣는다.

		DWORD dwClosed = COL_ERR;
		D3DXVECTOR3 vClosed = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		fDistNew = FLT_MAX;
		fDistOld = FLT_MAX;
		for ( DWORD i=0; i<dwNumVertices; i++ )
		{
			D3DXVECTOR3 Vector0 = *(D3DXVECTOR3*) ( pbData + dwSkinVertSize*i );

			D3DXVECTOR3 vDist = Vector0 - vDetect;
			fDistNew = D3DXVec3Length ( &vDist );
			
			if ( fDistNew < fDistOld )
			{
				dwClosed = i;
				vClosed = Vector0;

				fDistOld = fDistNew;

				vCLOSED_NORMAL = *(D3DXVECTOR3*)( pbData + dwSkinVertSize*i + dwSize_VERTEX );
			}
		}

		vDetectPos = vClosed;
		dwDetectIndex = dwClosed;

		SAFE_DELETE_ARRAY( pbData );
	}
	//	Note : 일반 메시.
	//
	else
	{
		if ( !MeshData.pMesh )								return FALSE;
		if ( MeshData.pMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;

		DWORD dwNumFaces = MeshData.pMesh->GetNumFaces ();
		DWORD dwNumVertices = MeshData.pMesh->GetNumVertices ();

		DWORD dwFVF = MeshData.pMesh->GetFVF ();
		DWORD dwVertSize = D3DXGetFVFVertexSize ( dwFVF );

		DWORD dwFVF_VERTEX = dwFVF&D3DFVF_POSITION_MASK;
		DWORD dwSize_VERTEX = D3DXGetFVFVertexSize( dwFVF_VERTEX );

		BYTE *pbData = NULL;
		hr = MeshData.pMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
		if ( FAILED(hr) )	return FALSE;

		WORD *pIndexBuffer;
		hr = MeshData.pMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			MeshData.pMesh->UnlockVertexBuffer ();
			return FALSE;
		}

		//if ( m_dwNumBoneMatrix == SMeshContainer::g_dwStaticBoneMatrixNum && m_ppStaticMatrixPtrs )
		//{
		//	//	특정 본에 영향을 받아야하지만 그 본이 존제 안할 경우가 있을때 대비.
		//	D3DXMATRIX	matTemp;
		//	if ( m_ppBoneMatrixPtrs[2] )
		//	{
		//		D3DXMatrixMultiply ( &matTemp, m_ppStaticMatrixPtrs[1], m_ppBoneMatrixPtrs[2] );
		//		D3DXMatrixMultiply ( &matCombined, m_ppStaticMatrixPtrs[0], &matTemp );
		//	}
		//}

		//const D3DXMATRIX& matCombined = m_matStaticLinkBone;

		DWORD dwCollisionFace = COL_ERR;
		D3DXVECTOR3 vDetect = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( DWORD j=0; j<dwNumFaces; j++ )
		{
			PWORD pwIndex = pIndexBuffer+0 + 3*j;

			D3DXVECTOR3 Vector0 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+0)) );
			D3DXVECTOR3 Vector1 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+1)) );
			D3DXVECTOR3 Vector2 = *(D3DXVECTOR3*) ( pbData + dwVertSize*(*(pwIndex+2)) );

			D3DXVECTOR3 vCollisionPos;
			BOOL bCollision = COLLISION::IsLineTriangleCollision
			(
				(D3DXVECTOR3*)&Vector0,
				(D3DXVECTOR3*)&Vector1,
				(D3DXVECTOR3*)&Vector2,
				(D3DXVECTOR3*)&vPoint1,
				(D3DXVECTOR3*)&vPoint2,
				&vCollisionPos,
				NULL, 
				EMCC_CULL_CCW, 
				TRUE
			);

			if ( bCollision )
			{
				D3DXVECTOR3 vDist = vPoint1 - vCollisionPos;
				fDistNew = D3DXVec3Length ( &vDist );
				
				if ( fDistNew <= fDistOld )
				{
					dwCollisionFace = j;
					vDetect = vCollisionPos;

					fDistOld = fDistNew;
				}
			}
		}

		vCLOSED_POINT = vDetect;	// Note : 실제 찍혀 있는 위치를 넣는다.

		if ( dwCollisionFace == COL_ERR )
		{
			MeshData.pMesh->UnlockVertexBuffer ();
			MeshData.pMesh->UnlockIndexBuffer ();
			return FALSE;
		}

		DWORD dwClosed = COL_ERR;
		D3DXVECTOR3 vClosed = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		fDistNew = FLT_MAX;
		fDistOld = FLT_MAX;
		for ( DWORD i=0; i<dwNumVertices; i++ )
		{
			D3DXVECTOR3 Vector0 = *(D3DXVECTOR3*) ( pbData + dwVertSize*i );

			D3DXVECTOR3 vDist = Vector0 - vDetect;
			fDistNew = D3DXVec3Length ( &vDist );
			
			if ( fDistNew < fDistOld )
			{
				dwClosed = i;
				vClosed = Vector0;

				fDistOld = fDistNew;

				vCLOSED_NORMAL = *(D3DXVECTOR3*)( pbData + dwVertSize*i + dwSize_VERTEX );
			}
		}

		MeshData.pMesh->UnlockVertexBuffer ();
		MeshData.pMesh->UnlockIndexBuffer ();

		vDetectPos = vClosed;
		dwDetectIndex = dwClosed;
	}

	return (dwDetectIndex!=COL_ERR);
}

HRESULT SMeshContainer::UpdateSkinnedMesh ( PBYTE pbData, DWORD dwFVF, LPD3DXMATRIX pBoneMatrices, const DxSkeleton* pSkeleton )
{
	HRESULT hr = S_OK;

	if (!pbData)
        CDebugSet::ToLogFile(std::string("SMeshContainer::UpdateSkinnedMesh() -- pbData == NULL"));

	DWORD dwVertSize = D3DXGetFVFVertexSize ( dwFVF );
	DWORD dwNumVertices = m_pOrigMesh->GetNumVertices();

	BYTE *pbSkinData = NULL;
	hr = m_pOrigMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbSkinData );
	if ( FAILED(hr) )	return E_FAIL;

	DWORD dwSkinFVF = m_pOrigMesh->GetFVF();
	DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );

	D3DXVECTOR3 *pVector = NULL;
	D3DXVECTOR3 *pSkinVector = NULL;

	DWORD i=0;

	//	Note : 값을 초기화 한다.
	//
	for ( i=0; i<dwNumVertices; i++ )
	{
		pVector = (D3DXVECTOR3*) ( pbData + dwVertSize*i );
		pVector->x = pVector->y = pVector->z = 0;
	}

	//	set up bone transforms
	//
	DWORD cBones  = pSkinInfo->GetNumBones();

	BOOL bBoneMatNew = FALSE;
	if ( !pBoneMatrices )
	{
		bBoneMatNew = TRUE;
		pBoneMatrices = new D3DXMATRIX[cBones];

		for ( DWORD iBone = 0; iBone < cBones; ++iBone )
		{
			//D3DXMatrixMultiply( &pBoneMatrices[iBone], &m_pBoneOffsetMatrices[iBone], &pSkeleton->FindBone_Index_t(iBone)->matCombined );
			D3DXMatrixMultiply( &pBoneMatrices[iBone], &m_pBoneOffsetMatrices[iBone], &pSkeleton->FindBone_Index_Skin( m_arrayBoneIndex, iBone )->matCombined );
		}
	}

	DWORD dwInfue(0L);
	DWORD* pVertices(NULL);
	float* pWeights(NULL);

	D3DXVECTOR3 vVecWeight;

	SET_DWORD setIndex;
	for( i=0; i<cBones; ++i )
	{
		dwInfue = pSkinInfo->GetNumBoneInfluences (i);

		pVertices = new DWORD[dwInfue];
		pWeights = new float[dwInfue];
		pSkinInfo->GetBoneInfluence ( i, pVertices, pWeights );

		for ( DWORD j=0; j<dwInfue; j++ )
		{
			if( pVertices[j] >= dwNumVertices )	// 최대 점 갯수 보다 크면 작업을 안 한다.
			{
				CDebugSet::ToLogFile(
                    std::string("SMeshContainer::UpdateSkinnedMesh() -- pSkinInfo의 최대 점 갯수가 제한을 넘었습니다."));
				continue;
			}

			pVector = (D3DXVECTOR3*) ( pbData + dwVertSize*pVertices[j] );
			pSkinVector = (D3DXVECTOR3*) ( pbSkinData + dwSkinVertSize*pVertices[j] );

			D3DXVec3TransformCoord( &vVecWeight, pSkinVector, &pBoneMatrices[i] );

			*pVector += vVecWeight*pWeights[j];

			setIndex.insert( pVertices[j] );
		}

		SAFE_DELETE_ARRAY(pVertices);
		SAFE_DELETE_ARRAY(pWeights);
	}

	if ( bBoneMatNew )	
		SAFE_DELETE_ARRAY(pBoneMatrices);

	m_pOrigMesh->UnlockVertexBuffer ();

	return S_OK;
}

BOOL SMeshContainer::GetStaticVertexData( std::vector<VERTEXNORCOLORTEX4>& vecVertices, const DxSkeleton* pSkeleton, const D3DXMATRIX& matStaticLinkBone )
{
	HRESULT hr = S_OK;

	if ( pSkinInfo )
	{
		if ( !m_pOrigMesh )								return FALSE;
		if ( m_pOrigMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;

		DWORD dwNumFaces = m_pOrigMesh->GetNumFaces ();
		DWORD dwNumVertices = m_pOrigMesh->GetNumVertices ();

		DWORD dwSkinFVF = m_pOrigMesh->GetFVF ();
		DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );

		DWORD dwFVF_VERTEX = dwSkinFVF&D3DFVF_POSITION_MASK;
		DWORD dwSize_VERTEX = D3DXGetFVFVertexSize( dwFVF_VERTEX );

		PBYTE pbData = new BYTE[dwSkinVertSize*dwNumVertices];
		hr = UpdateSkinnedMesh( pbData, dwSkinFVF, NULL, pSkeleton );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pbData);
			return FALSE;
		}
		VERTEX* pVerticesA = reinterpret_cast<VERTEX*>( pbData );

		WORD *pIndexBuffer;
		hr = m_pOrigMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			return FALSE;
		}

		for ( DWORD j=0; j<dwNumFaces; j++ )
		{
			PWORD pwIndex = pIndexBuffer + 3*j;

			D3DXVECTOR3 sPos[3];
			sPos[0] = *(D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+0) ) );
			sPos[1] = *(D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+1) ) );
			sPos[2] = *(D3DXVECTOR3*) ( pbData + dwSkinVertSize * ( *(pwIndex+2) ) );

			if ( sPos[0].x == 0.f || sPos[0].y == 0.f || sPos[0].z == 0.f ||
				sPos[1].x == 0.f || sPos[1].y == 0.f || sPos[1].z == 0.f ||
				sPos[2].x == 0.f || sPos[2].y == 0.f || sPos[2].z == 0.f )
			{
				continue;
			}

			for ( DWORD x=0; x<3; ++x )
			{
				VERTEXNORCOLORTEX4 sVertex;
				sVertex.vPos = sPos[x];
				sVertex.vNor = D3DXVECTOR3(0.f,1.f,0.f);
				sVertex.dwColor = 0xffffffff;
				sVertex.vTex[0] = D3DXVECTOR2(0.f,0.f);
				sVertex.vTex[1] = D3DXVECTOR2(0.f,0.f);
				sVertex.vTex[2] = D3DXVECTOR2(0.f,0.f);
				sVertex.vTex[3] = D3DXVECTOR2(0.f,0.f);
				vecVertices.push_back( sVertex );
			}
		}
		m_pOrigMesh->UnlockIndexBuffer ();

		SAFE_DELETE_ARRAY( pbData );
	}
	//	Note : 일반 메시.
	//
	else
	{
		if ( !MeshData.pMesh )								return FALSE;
		if ( MeshData.pMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;

		DWORD dwNumFaces = MeshData.pMesh->GetNumFaces ();
		DWORD dwNumVertices = MeshData.pMesh->GetNumVertices ();

		DWORD dwFVF = MeshData.pMesh->GetFVF ();
		DWORD dwVertSize = D3DXGetFVFVertexSize ( dwFVF );

		DWORD dwFVF_VERTEX = dwFVF&D3DFVF_POSITION_MASK;
		DWORD dwSize_VERTEX = D3DXGetFVFVertexSize( dwFVF_VERTEX );

		BYTE *pbData = NULL;
		hr = MeshData.pMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
		if ( FAILED(hr) )	return FALSE;

		WORD *pIndexBuffer;
		hr = MeshData.pMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndexBuffer );
		if ( FAILED(hr) )
		{
			MeshData.pMesh->UnlockVertexBuffer ();
			return FALSE;
		}

		//D3DXMATRIX matCombined;
		//if ( m_dwNumBoneMatrix == SMeshContainer::g_dwStaticBoneMatrixNum && m_ppBoneMatrixPtrs )
		//{
		//	//	특정 본에 영향을 받아야하지만 그 본이 존제 안할 경우가 있을때 대비.
		//	D3DXMATRIX	matTemp;
		//	if ( m_ppBoneMatrixPtrs[2] )
		//	{
		//		D3DXMatrixMultiply ( &matTemp, m_ppBoneMatrixPtrs[1], m_ppBoneMatrixPtrs[2] );
		//		D3DXMatrixMultiply ( &matCombined, m_ppBoneMatrixPtrs[0], &matTemp );
		//	}
		//}

		//const D3DXMATRIX& matCombined = m_matStaticLinkBone;

		for ( DWORD j=0; j<dwNumFaces; j++ )
		{
			PWORD pwIndex = pIndexBuffer+0 + 3*j;

			for ( DWORD x=0; x<3; ++x )
			{
				VERTEXNORCOLORTEX4 sVertex;
				sVertex.vPos = *(D3DXVECTOR3*) ( pbData + dwVertSize * ( *(pwIndex+x) ) );
				sVertex.vNor = D3DXVECTOR3(0.f,1.f,0.f);
				sVertex.dwColor = 0xffffffff;
				sVertex.vTex[0] = D3DXVECTOR2(0.f,0.f);
				sVertex.vTex[1] = D3DXVECTOR2(0.f,0.f);
				sVertex.vTex[2] = D3DXVECTOR2(0.f,0.f);
				sVertex.vTex[3] = D3DXVECTOR2(0.f,0.f);
				D3DXVec3TransformCoord( &sVertex.vPos, &sVertex.vPos, &matStaticLinkBone );
				vecVertices.push_back( sVertex );
			}
		}

		MeshData.pMesh->UnlockVertexBuffer ();
		MeshData.pMesh->UnlockIndexBuffer ();
	}

	return TRUE;
}

HRESULT SMeshContainer::GetVertexInfluences ( const DWORD dwIndex, SVERTEXINFLU *pVertInflu )
{
	HRESULT hr = S_OK;
	if ( !MeshData.pMesh && !pSkinInfo )		return E_FAIL;
	if ( dwIndex == COL_ERR )					return E_FAIL;

	//	Note : 스킨 메시의 경우.
	//
	if ( pSkinInfo )
	{
		if ( !m_pOrigMesh )								return FALSE;
		if ( m_pOrigMesh->GetOptions()&D3DXMESH_32BIT )	return FALSE;

		DWORD dwNumBone = pSkinInfo->GetNumBones ();

		DWORD dwMaxVerInflu = 0;
		hr = pSkinInfo->GetMaxVertexInfluences ( &dwMaxVerInflu );
		if ( FAILED(hr) )	return E_FAIL;

		DWORD dwCount0 = 0;
		PDWORD pdwBone0 = new DWORD[dwMaxVerInflu];
		PFLOAT pfWeights0 = new FLOAT[dwMaxVerInflu];

		for ( DWORD i=0; i<dwNumBone; i++ )
		{
			DWORD dwInfue = pSkinInfo->GetNumBoneInfluences (i);
			if ( dwInfue==0 )	continue;

			DWORD* pVertices = new DWORD[dwInfue];
			float* pWeights = new float[dwInfue];

			pSkinInfo->GetBoneInfluence ( i, pVertices, pWeights );
			for ( DWORD j=0; j<dwInfue; j++ )
			{
				if ( dwIndex == pVertices[j] )
				{
					GASSERT(dwMaxVerInflu>dwCount0);

					pdwBone0[dwCount0] = i;
					pfWeights0[dwCount0] = pWeights[j];
					++dwCount0;
				}
			}

			SAFE_DELETE_ARRAY(pVertices);
			SAFE_DELETE_ARRAY(pWeights);
		}

		DWORD dwFVFMesh = m_pOrigMesh->GetFVF ();
		DWORD dwVertexSize = D3DXGetFVFVertexSize ( dwFVFMesh );
		DWORD dwNumVertices = m_pOrigMesh->GetNumVertices ();

		if ( dwIndex >= dwNumVertices )
		{
			SAFE_DELETE_ARRAY(pdwBone0);
			SAFE_DELETE_ARRAY(pfWeights0);
			return E_FAIL;
		}

		BYTE *pbData = NULL;
		hr = m_pOrigMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
		if ( FAILED(hr) )
		{
			SAFE_DELETE_ARRAY(pdwBone0);
			SAFE_DELETE_ARRAY(pfWeights0);
			return FALSE;
		}

		D3DXVECTOR3 vVector, vNormal;
		
		vVector = *( (D3DXVECTOR3*) ( pbData+dwVertexSize*dwIndex ) );
		
		if ( dwFVFMesh&D3DFVF_NORMAL )
			vNormal = *( (D3DXVECTOR3*) ( pbData+dwVertexSize*dwIndex + sizeof(D3DXVECTOR3) ) );

		m_pOrigMesh->UnlockVertexBuffer ();

		//	Note : 버텍스 정보.
		pVertInflu->m_dwIndex = dwIndex;
		pVertInflu->m_vVector = vVector;
		pVertInflu->m_vNormal = vNormal;

		pVertInflu->SetBoneNum ( dwCount0 );
		memcpy ( pVertInflu->m_pBone, pdwBone0, sizeof(DWORD)*dwCount0 );
		memcpy ( pVertInflu->m_pWeights, pfWeights0, sizeof(float)*dwCount0 );


		SAFE_DELETE_ARRAY(pdwBone0);
		SAFE_DELETE_ARRAY(pfWeights0);
	}
	//	Note : 일반 메시의 경우.
	//
	else
	{
		DWORD dwFVFMesh = MeshData.pMesh->GetFVF ();
		DWORD dwVertexSize = D3DXGetFVFVertexSize ( dwFVFMesh );
		DWORD dwNumVertices = MeshData.pMesh->GetNumVertices ();
		if ( dwIndex >= dwNumVertices )		return E_FAIL;

		BYTE *pbData = NULL;
		hr = MeshData.pMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
		if ( FAILED(hr) )	return FALSE;

		D3DXVECTOR3 vNormal;
		D3DXVECTOR3 vVector = *( (D3DXVECTOR3*) (pbData+dwVertexSize*dwIndex) );
		
		if ( dwFVFMesh&D3DFVF_NORMAL )
			vNormal = *( (D3DXVECTOR3*) ( pbData+dwVertexSize*dwIndex + sizeof(D3DXVECTOR3) ) );

		MeshData.pMesh->UnlockVertexBuffer ();

		//	Note : 버텍스 정보.
		//
		pVertInflu->m_dwIndex = dwIndex;
		pVertInflu->m_vVector = vVector;
		pVertInflu->m_vNormal = vNormal;
		pVertInflu->m_dwNumBone = COL_ERR;
	}

	return FALSE;
}

HRESULT SMeshContainer::CalculateVertexInfluences( const SVERTEXINFLU& sVertInflu, 
												  D3DXVECTOR3 &vVecOutput, 
												  D3DXVECTOR3 &vNorOutput,
												  const LPD3DXMATRIX pBoneMatrice, 
												  BOOL bIdentity, 
												  const DxSkeletonMaintain& sSkeleton, 
												  const D3DXMATRIX& matStaticLinkBone )
{
	D3DXVECTOR3 vVecComb(0,0,0);
	D3DXVECTOR3 vVecNormal(0,0,0);
	//D3DXMATRIX matCombined;

	if ( sVertInflu.m_dwNumBone==COL_ERR )
	{
		if( !pSkinInfo && m_dwNumBoneMatrix==SMeshContainer::g_dwStaticBoneMatrixNum )	//&& m_ppBoneMatrixPtrs )
		{
			////	특정 본에 영향을 받아야하지만 그 본이 존제 안할 경우가 있을때 대비.
			//D3DXMATRIX	matTemp;
			//if ( m_ppBoneMatrixPtrs[2] )
			//{
			//	D3DXMatrixMultiply ( &matTemp, m_ppBoneMatrixPtrs[1], m_ppBoneMatrixPtrs[2] );
			//	D3DXMatrixMultiply ( &matCombined, m_ppBoneMatrixPtrs[0], &matTemp );
			//}

			//const D3DXMATRIX& matCombined = m_matStaticLinkBone;

			D3DXVec3TransformCoord ( &vVecComb, &sVertInflu.m_vVector, &matStaticLinkBone );
			D3DXVec3TransformNormal ( &vVecNormal, &sVertInflu.m_vNormal, &matStaticLinkBone );

			//	Note : 출력값.
			//
			if ( pBoneMatrice )	
				*pBoneMatrice = matStaticLinkBone;

			// ... Edit 시에는 Matrix에 영향을 받지 않으므로 이렇게 나눠놓았다.
			if( bIdentity )
			{
				vVecOutput = sVertInflu.m_vVector;
				vNorOutput = sVertInflu.m_vNormal;
			}
			else
			{
				vVecOutput = vVecComb;
				vNorOutput = vVecNormal;
			}

			return S_OK;
		}

		return E_FAIL;
	}
	else
	{
		if ( !pSkinInfo )
			return E_FAIL;

		DWORD dwNumBone = pSkinInfo->GetNumBones ();

		//	Note : 영향력을 주는 트랜스폼을 모두 합산하여 버텍스 값을 구한다.
		//
		D3DXMATRIX matCombined;
		for ( DWORD i=0; i<sVertInflu.m_dwNumBone; i++ )
		{
			DWORD iBone = sVertInflu.m_pBone[i];

			if ( dwNumBone <= iBone )
				return E_FAIL;

			//D3DXMatrixMultiply ( &matCombined, &m_pBoneOffsetMatrices[iBone], m_ppBoneMatrixPtrs[iBone] );
			D3DXMatrixMultiply ( &matCombined, &m_pBoneOffsetMatrices[iBone], &sSkeleton.FindBone_Index(m_arrayBoneIndex[iBone])->m_matCombined );

			D3DXVECTOR3 vVecWeight,vVecWeightNor;
			D3DXVec3TransformCoord ( &vVecWeight, &sVertInflu.m_vVector, &matCombined );
			vVecComb += vVecWeight*sVertInflu.m_pWeights[i];
			D3DXVec3TransformNormal ( &vVecWeightNor, &sVertInflu.m_vNormal, &matCombined );
			vVecNormal += vVecWeightNor*sVertInflu.m_pWeights[i];
		}

		//	Note : 출력값.
		//
		vVecOutput = vVecComb;
		D3DXVec3Normalize ( &vVecNormal, &vVecNormal );
		vNorOutput = vVecNormal;

		//	(주의) 영향을 받는 본의 갯수가 여러개일 경우에 문제가 생김.
		if ( pBoneMatrice )		
			*pBoneMatrice = matCombined;
	}

	return S_OK;
}

HRESULT SMeshContainer::CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	HRESULT hr;
	if ( !MeshData.pMesh && !pSkinInfo )		return E_FAIL;

	if ( MeshData.pMesh )
	{
		if ( MeshData.pMesh->GetOptions()&D3DXMESH_32BIT )	return E_FAIL;

		DWORD dwNumFaces = MeshData.pMesh->GetNumFaces ();
		DWORD dwNumVertices = MeshData.pMesh->GetNumVertices ();
		
		DWORD dwSkinFVF = MeshData.pMesh->GetFVF ();
		DWORD dwSkinVertSize = D3DXGetFVFVertexSize ( dwSkinFVF );

		vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		BYTE *pbData = NULL;
		hr = MeshData.pMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
		if ( FAILED(hr) )	return FALSE;

		LPD3DXVECTOR3 pVector = (LPD3DXVECTOR3) pbData;
		for ( DWORD i=0; i<dwNumVertices; ++i )
		{
			if ( pVector[i].x > vMax.x )	vMax.x = pVector[i].x;
			if ( pVector[i].y > vMax.y )	vMax.y = pVector[i].y;
			if ( pVector[i].z > vMax.z )	vMax.z = pVector[i].z;

			if ( pVector[i].x < vMin.x )	vMin.x = pVector[i].x;
			if ( pVector[i].y < vMin.y )	vMin.y = pVector[i].y;
			if ( pVector[i].z < vMin.z )	vMin.z = pVector[i].z;
		}

		MeshData.pMesh->UnlockVertexBuffer ();
	}

	return S_OK;
}