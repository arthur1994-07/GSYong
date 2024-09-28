// Terrain0.cpp: implementation of the DxEffectTiling class.
//
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../../../VisualMaterialLib/Manager/DxVMManager.h"

#include "../../../SigmaCore/Math/Random.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxEffect/Frame/DxEffectDefine.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxTools/Collision.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/EditMeshs.h"
#include "../../DxMeshs/SkinMesh/DxMaterial.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxLand/DxPiece/DxSingleTexMesh.h"
#include "../../DxLand/DxLandMan.h"

#include "./DxEffectTiling.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//	---------------------------------------------------------------------------------------------------
//										이 함수의 초기화와 삭제 부분
//	---------------------------------------------------------------------------------------------------
DxEffectTiling::MATERIALEX::MATERIALEX( DWORD dwPointEX_NUM )
	: m_bAlpha(FALSE)
	, m_pMeshList(NULL)
{
	m_pPointUseColor = new POINT_USE_COLOR[dwPointEX_NUM];
}

DxEffectTiling::MATERIALEX::~MATERIALEX()
{
	listFace.clear();
	listTriangle.clear();

	SAFE_DELETE ( m_pPointUseColor );
	SAFE_DELETE ( m_pMeshList );
}

//	---------------------------------------------------------------------------------------------------
//											생	성	부	분
//	---------------------------------------------------------------------------------------------------
void	DxEffectTiling::SetBaseQuad ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs )
{
	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	if ( !pmsMeshs->m_pLocalMesh )	return;

    LPD3DXMESH pTempMesh;
    if ( FAILED( pmsMeshs->m_pLocalMesh->CloneMeshFVF( pmsMeshs->m_pLocalMesh->GetOptions(), VERTEXNORTEXTANGENT::FVF,
										                pd3dDevice, &pTempMesh ) ) )
		return;

    NS_VM_MANAGER::CreateTangentSkinMeshObject( pd3dDevice, &pTempMesh );


	m_dwVertices	= pTempMesh->GetNumVertices ( );
	m_dwFaces		= pTempMesh->GetNumFaces ( );

	VERTEXNORTEXTANGENT*	pSrcVertices;
	WORD*			        pSrcIndices;

	pTempMesh->LockVertexBuffer ( 0L, (VOID**)&pSrcVertices );
	pTempMesh->LockIndexBuffer ( 0L, (VOID**)&pSrcIndices );


	m_vMax = m_vMin = pSrcVertices[0].vPos;	// 초기값 셋팅

	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		m_vMax.x = (pSrcVertices[i].vPos.x > m_vMax.x) ? pSrcVertices[i].vPos.x : m_vMax.x;
		m_vMax.y = (pSrcVertices[i].vPos.y > m_vMax.y) ? pSrcVertices[i].vPos.y : m_vMax.y;
		m_vMax.z = (pSrcVertices[i].vPos.z > m_vMax.z) ? pSrcVertices[i].vPos.z : m_vMax.z;

		m_vMin.x = (pSrcVertices[i].vPos.x < m_vMin.x) ? pSrcVertices[i].vPos.x : m_vMin.x;
		m_vMin.y = (pSrcVertices[i].vPos.y < m_vMin.y) ? pSrcVertices[i].vPos.y : m_vMin.y;
		m_vMin.z = (pSrcVertices[i].vPos.z < m_vMin.z) ? pSrcVertices[i].vPos.z : m_vMin.z;
	}

	// 각 점의 정보를 얻기위한 배열을 지정한다.
	SetPointData( pSrcVertices, pSrcIndices );

    pTempMesh->UnlockVertexBuffer ();
	pTempMesh->UnlockIndexBuffer ();

	// 삼각형 정보를 얻기 위한 작업
	SetTriangleData ( pTempMesh );
    SetMaterial ( pmsMeshs );

    SAFE_RELEASE( pTempMesh );
}

void	DxEffectTiling::SetPointData ( VERTEXNORTEXTANGENT* pVertices, WORD* pIndices )
{
	SAFE_DELETE_ARRAY ( m_pPoint );
	m_pPoint = new POINT [ m_dwFaces*3 ];

	DWORD dwVertex_NUM;
	DWORD dwPoint_NUM;

	D3DXMATRIX	matWorld;
	D3DXMatrixMultiply ( &matWorld, m_pmatLocal, &m_matFrameComb );

	for ( DWORD i=0; i<m_dwFaces; ++i )
	{
		dwPoint_NUM = (i*3)+0;
		dwVertex_NUM = pIndices[ dwPoint_NUM ];
		D3DXVec3TransformCoord( &m_pPoint[dwPoint_NUM].vPos, &pVertices[dwVertex_NUM].vPos, &matWorld );
        D3DXVec3TransformNormal( &m_pPoint[dwPoint_NUM].vNor, &pVertices[dwVertex_NUM].vNor, &matWorld );
		m_pPoint [ dwPoint_NUM ].vTex		= pVertices [ dwVertex_NUM ].vTex;
        m_pPoint [ dwPoint_NUM ].vTan       = pVertices [ dwVertex_NUM ].vTan;
		m_pPoint [ dwPoint_NUM ].dwFace_NUM = i; 

		dwPoint_NUM = (i*3)+1;
		dwVertex_NUM = pIndices[ dwPoint_NUM ];
		D3DXVec3TransformCoord( &m_pPoint[dwPoint_NUM].vPos, &pVertices[dwVertex_NUM].vPos, &matWorld );
        D3DXVec3TransformNormal( &m_pPoint[dwPoint_NUM].vNor, &pVertices[dwVertex_NUM].vNor, &matWorld );
		m_pPoint [ dwPoint_NUM ].vTex		= pVertices [ dwVertex_NUM ].vTex;
		m_pPoint [ dwPoint_NUM ].vTan       = pVertices [ dwVertex_NUM ].vTan;
		m_pPoint [ dwPoint_NUM ].dwFace_NUM = i;

		dwPoint_NUM = (i*3)+2;
		dwVertex_NUM = pIndices[ dwPoint_NUM ];
		D3DXVec3TransformCoord( &m_pPoint[dwPoint_NUM].vPos, &pVertices[dwVertex_NUM].vPos, &matWorld );
        D3DXVec3TransformNormal( &m_pPoint[dwPoint_NUM].vNor, &pVertices[dwVertex_NUM].vNor, &matWorld );
		m_pPoint [ dwPoint_NUM ].vTex		= pVertices [ dwVertex_NUM ].vTex;
		m_pPoint [ dwPoint_NUM ].vTan       = pVertices [ dwVertex_NUM ].vTan;
		m_pPoint [ dwPoint_NUM ].dwFace_NUM = i;
	}

	//	통합적으로 쓰일 점을 구한다.
	//
	FINDEX	sFindEX;
	m_mapFindEX.clear();

	m_dwPointEX = 0L;
	SAFE_DELETE_ARRAY ( m_pPointEX );
	m_pPointEX = new POINTEX [ m_dwFaces*3 ];

	for ( DWORD i=0; i<m_dwFaces*3; ++i )
	{
		if ( !DxFindPoint ( m_pPoint[i].vPos, i ) )	// 값이 없다면..
		{
			m_pPointEX [ m_dwPointEX ].pPoint = new POINTA;
			m_pPointEX [ m_dwPointEX ].pPoint->dwIndex = i;
			m_pPointEX [ m_dwPointEX ].vPos = m_pPoint[i].vPos;

			m_pPoint[i].dwPointEx = m_dwPointEX;				// 부모격인 점 인덱스를 얻는다.

			sFindEX.dwEX = m_dwPointEX;
			sFindEX.vPos = m_pPointEX[m_dwPointEX].vPos;
			m_mapFindEX.insert ( std::make_pair(sFindEX.vPos.x,sFindEX) );

			++m_dwPointEX;
		}
	}

	return;
}

BOOL	DxEffectTiling::DxFindPoint ( D3DXVECTOR3 vPos, DWORD _dwIndex )
{
	D3DXVECTOR3 vTemp;
	FINDEXMAP_ITER iter = m_mapFindEX.lower_bound ( vPos.x-0.01f );
	for ( ; iter!=m_mapFindEX.end(); ++iter )
	{
		vTemp = (*iter).second.vPos;

		if ( ((vTemp.x+0.01f)>vPos.x) && (vTemp.x<(vPos.x+0.01f)) && 
			((vTemp.y+0.01f)>vPos.y) && (vTemp.y<(vPos.y+0.01f)) && 
			((vTemp.z+0.01f)>vPos.z) && (vTemp.z<(vPos.z+0.01f)) )
		{
			DWORD dwEX = (*iter).second.dwEX;

			POINTA* pNew = new POINTA;
			pNew->dwIndex = _dwIndex;
			pNew->pNext = m_pPointEX [ dwEX ].pPoint;
			m_pPointEX [ dwEX ].pPoint = pNew;
			
			m_pPoint[_dwIndex].dwPointEx = dwEX;			// 부모격인 점 인덱스를 얻는다.

			return TRUE;
		}
		else if ( vTemp.x > (vPos.x + 0.01f) )
		{
			return FALSE;
		}
	}

	return FALSE;
}

void DxEffectTiling::SetTriangleData ( LPD3DXMESH pSrcMesh )
{
	m_pTriangle = new TRIANGLE [ m_dwFaces ];

	DWORD	dwAttribTableSize;
	pSrcMesh->GetAttributeTable ( NULL, &dwAttribTableSize );

	D3DXATTRIBUTERANGE*	pAttribTable;
	pAttribTable = new D3DXATTRIBUTERANGE [ dwAttribTableSize ];

	pSrcMesh->GetAttributeTable( pAttribTable, &dwAttribTableSize );

	DWORD dwIndex;
	for ( DWORD i=0; i<dwAttribTableSize; ++i )
	{
		dwIndex = pAttribTable[i].AttribId;	// 매트리얼

		for ( DWORD j=pAttribTable[i].FaceStart; j<pAttribTable[i].FaceStart+pAttribTable[i].FaceCount; ++j )
		{
			m_pTriangle[j].dwMaterial = dwIndex;
		}
	}

	SAFE_DELETE_ARRAY ( pAttribTable );
}

void DxEffectTiling::SetMaterial ( DxMeshes *pmsMeshs )
{
    m_dwMaterials = pmsMeshs->m_cMaterials;
	m_pMaterials = new DXMATERIAL[m_dwMaterials];

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].strTexture = pmsMeshs->m_strTextureFiles[i].GetString();
	}
}

//	---------------------------------------------------------------------------------------------------
//									에	디	트	부	분			( Interface )
//	---------------------------------------------------------------------------------------------------
char*	DxEffectTiling::GetStrList ( DWORD i )
{
	static char szStrEdit[256];

	MATERIALMAP_ITER iter = m_mapMaterials.find ( i );
	if ( iter!=m_mapMaterials.end() )
	{
		StringCchPrintf( szStrEdit, 256, "[%d] %s", (*iter).first, (*iter).second->strTexture.c_str() );
	}

	return szStrEdit;
}

BOOL	DxEffectTiling::SetStrList ( LPDIRECT3DDEVICEQ pd3dDevice, const char* cName )
{
	MATERIALMAP_ITER iter = m_mapMaterials.begin();
	for ( ; iter!=m_mapMaterials.end(); ++iter )
	{
		if ( !strcmp ( iter->second->strTexture.c_str(), cName ) )
		{
			m_dwCurTILE = iter->first;	// 현재 쓰이는 타일로 교체
			return FALSE;
		}
	}

	std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(m_dwPointEX) );

	spMaterials->strTexture = cName;

    spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
        cName,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		false,
		FALSE );

    if( spMaterials->m_textureRes.GetTexture() )
	    spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// 알파 쓰는지 안쓰는지 체크

	m_mapMaterials.insert ( std::make_pair(m_dwUNIQUE_NUMBER,spMaterials) );

	m_dwCurTILE = m_dwUNIQUE_NUMBER;	// 현재 쓰이는 타일로 교체

	return TRUE;
}

//	---------------------------------------------------------------------------------------------------
//									에	디	트	부	분			( Engine )
//	---------------------------------------------------------------------------------------------------
void	DxEffectTiling::TileEdit ( D3DXVECTOR3 vPickRayOrig, D3DXVECTOR3 vPickRayDir )
{
	//	Note : 원 나타내기.
	//
	POINTA*		pCur;
	D3DXVECTOR3 vTemp;

	D3DXVECTOR3 vPos = D3DXVECTOR3 ( 0.f, 0.f, 0.f );	// 이넘을 구해라. 오케이 ?

	{
		int		nCount = 0;
		float	fDistPrev = 0.f;

		D3DXVECTOR3 v0, v1, v2;
		FLOAT fBary1, fBary2;
		FLOAT fDist;

		D3DXVECTOR3 _v0, _v1, _v2;
		FLOAT _fBary1, _fBary2;

		for( DWORD i=0; i<m_dwFaces; i++ )
		{
			v0 = m_pPoint[3*i+0].vPos;
			v1 = m_pPoint[3*i+1].vPos;
			v2 = m_pPoint[3*i+2].vPos;

			// Check if the pick ray passes through this point
			if( DxEffectMan::GetInstance().IntersectTriangle( vPickRayOrig, vPickRayDir, v0, v1, v2,
									&fDist, &fBary1, &fBary2 ) )
			{
				if( (nCount==0) || (fDist < fDistPrev) )
				{
					_v0 = v0;
					_v1 = v1;
					_v2 = v2;
					_fBary1 = fBary1;
					_fBary2 = fBary2;

					fDistPrev = fDist;
				}

				++nCount;
			}
		}

		if ( nCount )
		{
			D3DXVECTOR3 vTemp;
			float fDis0, fDis1, fDis2;
			vPos = _v0 + ((_v1 - _v0)*_fBary1) + ((_v2 - _v0)*_fBary2);

			vTemp = vPos - _v0;
			fDis0 = D3DXVec3Length ( &vTemp );

			vTemp = vPos - _v1;
			fDis1 = D3DXVec3Length ( &vTemp );

			vTemp = vPos - _v2;
			fDis2 = D3DXVec3Length ( &vTemp );

			if ( (fDis0<=fDis1) && (fDis0<=fDis2) )			vPos = _v0;
			else if ( (fDis1<=fDis0) && (fDis1<=fDis2) )	vPos = _v1;
			else if ( (fDis2<=fDis0) && (fDis2<=fDis1) )	vPos = _v2;
		}
	}

	FINDEXMAP_ITER iter = m_mapFindEX.lower_bound ( vPos.x-0.01f );
	for ( ; iter!=m_mapFindEX.end(); ++iter )
	{
		vTemp = (*iter).second.vPos;

		if ( ((vTemp.x+0.01f)>vPos.x) && (vTemp.x<(vPos.x+0.01f)) && 
			((vTemp.y+0.01f)>vPos.y) && (vTemp.y<(vPos.y+0.01f)) && 
			((vTemp.z+0.01f)>vPos.z) && (vTemp.z<(vPos.z+0.01f)) )
		{
			m_dwCurPOINT = (*iter).second.dwEX;
			goto _RETURN;
		}
		else if ( vTemp.x > (vPos.x + 0.01f) )
		{
			return;
		}
	}

_RETURN:

	DWORD		dwFaceNUM;
	pCur = m_pPointEX [ m_dwCurPOINT ].pPoint;
	while ( pCur )
	{
		dwFaceNUM = m_pPoint [ pCur->dwIndex ].dwFace_NUM;

		BOOL bNew = TRUE;
		TRIANGLELIST_ITER iter = m_listSelectTriangle.begin();
		for ( ; iter!=m_listSelectTriangle.end(); ++iter )
		{
			if ( (*iter) == dwFaceNUM )	bNew = FALSE;	// 같은거 있으면 등록 시키지 마라.
		}
		if ( bNew )	m_listSelectTriangle.push_back ( dwFaceNUM );

		pCur = pCur->pNext;
	}
}

void	DxEffectTiling::DxLeftMouseClick ()
{
//	if ( m_dwCurPOINT_PREV == m_dwCurPOINT )	return;

	if ( m_dwFlag & USE_TEX_DRAW )		BlendTileSetting ();	// 이녀석은 알파
	if ( m_dwFlag & USE_COLOR_DRAW )	BaseTileSetting ();		// 이녀석은 알파 상관 없다.
}

void	DxEffectTiling::BaseTileSetting ()
{
	D3DXCOLOR	cColor;
	cColor.r = m_cCurCOLOR.r + m_cCurRANDOM.r * sc::Random::RANDOM_POS();
	cColor.g = m_cCurCOLOR.g + m_cCurRANDOM.g * sc::Random::RANDOM_POS();
	cColor.b = m_cCurCOLOR.b + m_cCurRANDOM.b * sc::Random::RANDOM_POS();

	if ( cColor.r>1.f )	cColor.r = 1.f;	// 범위 제한
	if ( cColor.r<0.f )	cColor.r = 0.f;	// 범위 제한
	if ( cColor.g>1.f )	cColor.g = 1.f;	// 범위 제한
	if ( cColor.g<0.f )	cColor.g = 0.f;	// 범위 제한
	if ( cColor.b>1.f )	cColor.b = 1.f;	// 범위 제한
	if ( cColor.b<0.f )	cColor.b = 0.f;	// 범위 제한

	int R = (int)(cColor.r*255.f);
	int G = (int)(cColor.g*255.f);
	int B = (int)(cColor.b*255.f);

	MATERIALMAP_ITER	iter = m_mapMaterials.find ( m_dwCurTILE );
	if ( iter!=m_mapMaterials.end() )
	{
		DWORD	dwColor = (*iter).second->m_pPointUseColor [ m_dwCurPOINT ].dwColor;
		dwColor = dwColor&(0xff000000);
		dwColor = dwColor + (R<<16) + (G<<8) + B;

		// 타일 색 변화
		(*iter).second->m_pPointUseColor [ m_dwCurPOINT ].bUse = TRUE;
		(*iter).second->m_pPointUseColor [ m_dwCurPOINT ].dwColor = dwColor;
	}

	//DWORD	dwColor = m_pPointEX [ m_dwCurPOINT ].dwColor;
	//dwColor = dwColor&(0xff000000);
	//dwColor = dwColor + (R<<16) + (G<<8) + B;

	//// 타일 색 변화
	//m_pPointEX [ m_dwCurPOINT ].dwColor	= dwColor;

	//// 셋팅할 타일의 종류를 알기 위한 작업
	//BOOL			bUse = FALSE;
	//TRIANGLELIST	listTriangle;
	//DWORD			dwFace_NUM;
	//DWORD			dwMaterial;
	//POINTA*			pPoint = m_pPointEX [ m_dwCurPOINT ].pPoint;
	//while ( pPoint )
	//{
	//	dwFace_NUM = m_pPoint [ pPoint->dwIndex ].dwFace_NUM;
	//	dwMaterial = m_pTriangle [ dwFace_NUM ].dwMaterial;

	//	bUse = FALSE;
	//	TRIANGLELIST_ITER iter = listTriangle.begin();
	//	for ( ; iter!=listTriangle.end(); ++iter )
	//	{
	//		if ( (*iter) == dwMaterial )	bUse = TRUE;		
	//	}

	//	// 삼각형을 셋팅한다.
	//	if ( !bUse )
	//	{
	//		listTriangle.push_back ( dwMaterial );
	//	}

	//	pPoint = pPoint->pNext;
	//}
	//listTriangle.clear();
}

void	DxEffectTiling::BlendTileSetting ()
{
	if ( !m_dwCurTILE )	
		return;

	POINTA*		pCur;
	DWORD		dwFaceNUM;

	D3DXCOLOR	cColor;
	cColor.a = m_cCurCOLOR.a + m_cCurRANDOM.a * sc::Random::RANDOM_POS();
	if ( cColor.a>1.f )	cColor.a = 1.f;	// 범위 제한
	if ( cColor.a<0.f )	cColor.a = 0.f;	// 범위 제한
	int A = (int)(cColor.a*255.f);

	MATERIALMAP_ITER	iter = m_mapMaterials.find ( m_dwCurTILE );
	if ( iter!=m_mapMaterials.end() )
	{
		DWORD	dwColor = (*iter).second->m_pPointUseColor [ m_dwCurPOINT ].dwColor;
		dwColor = (dwColor & 0xffffff);
		dwColor = dwColor + (A<<24);

		// 타일 색 변화
		(*iter).second->m_pPointUseColor [ m_dwCurPOINT ].bUse = TRUE;
		(*iter).second->m_pPointUseColor [ m_dwCurPOINT ].dwColor = dwColor;

		pCur = m_pPointEX [ m_dwCurPOINT ].pPoint;
		while ( pCur )
		{
			dwFaceNUM = m_pPoint [ pCur->dwIndex ].dwFace_NUM;

			//	삼각형이 중복으로 들어가지 않도록 한다.
			if ( !CheckTriangleUSE ( iter, dwFaceNUM ) )
			{
				(*iter).second->listTriangle.push_back ( dwFaceNUM );
			}

			pCur = pCur->pNext;
		}
	}

	//DWORD	dwColor = m_pPointEX [ m_dwCurPOINT ].dwColor;
	//dwColor = (dwColor & 0xffffff);
	//dwColor = dwColor + (A<<24);

	//// 타일 셋팅, 알파
	//m_pPointEX [ m_dwCurPOINT ].dwMaterial	= m_dwCurTILE;
	//m_pPointEX [ m_dwCurPOINT ].dwColor		= dwColor;

	//// 매트리얼에 대한 삼각형의 활성화
	//MATERIALMAP_ITER	iter = m_mapMaterials.find ( m_dwCurTILE );
	//if ( iter!=m_mapMaterials.end() )
	//{
	//	pCur = m_pPointEX [ m_dwCurPOINT ].pPoint;
	//	while ( pCur )
	//	{
	//		dwFaceNUM = m_pPoint [ pCur->dwIndex ].dwFace_NUM;

	//		//	삼각형이 중복으로 들어가지 않도록 한다.
	//		if ( !CheckTriangleUSE ( iter, dwFaceNUM ) )
	//		{
	//			(*iter).second->listTriangle.push_back ( dwFaceNUM );
	//		}

	//		pCur = pCur->pNext;
	//	}
	//}
}

BOOL	DxEffectTiling::CheckTriangleUSE ( MATERIALMAP_ITER& iter, DWORD dwFaceNUM )
{
	TRIANGLELIST_ITER iterTriangle = (*iter).second->listTriangle.begin();
	for ( ; iterTriangle!=(*iter).second->listTriangle.end(); ++iterTriangle )
	{
		if ( (*iterTriangle) == dwFaceNUM )		return TRUE;
	}

	return FALSE;
}

void	DxEffectTiling::DeleteClick ()
{
	DWORD		dwFaceNUM;
	//DWORD		dwMaterial;
	POINTA* pCur = m_pPointEX [ m_dwCurPOINT ].pPoint;
	while ( pCur )
	{
		dwFaceNUM = m_pPoint [ pCur->dwIndex ].dwFace_NUM;

		MATERIALMAP_ITER iter = m_mapMaterials.find ( m_dwCurTILE );
		if ( iter != m_mapMaterials.end() )
		{
			BOOL bNew = TRUE;
			TRIANGLELIST_ITER iter_Del;
			TRIANGLELIST_ITER iter2 = (*iter).second->listTriangle.begin();
			for ( ; iter2!=(*iter).second->listTriangle.end(); )
			{
				if ( (*iter2) == dwFaceNUM )
				{
					iter_Del = iter2;
					++iter2;
					(*iter).second->listTriangle.erase ( iter_Del );
				}
				else
				{
					++iter2;
				}
			}

			// 사용을 끔.
			(*iter).second->m_pPointUseColor[ m_pPoint [ pCur->dwIndex ].dwPointEx ].bUse = FALSE;
		}

		pCur = pCur->pNext;
	}
}

BOOL	DxEffectTiling::IsTriangle ( D3DXVECTOR3 vPickRayOrig, D3DXVECTOR3 vPickRayDir )
{
	int		nCount = 0;
	float	fDistPrev = 0.f;
	BOOL	bUse = FALSE;
	BOOL	bNew = TRUE;		// 새로운 것 

	// Not using D3DX
	D3DXVECTOR3 v0, v1, v2;
	FLOAT fBary1, fBary2;
	FLOAT fDist;
	for( DWORD i=0; i<m_dwFaces; i++ )
	{
		v0 = m_pPoint[3*i+0].vPos;
		v1 = m_pPoint[3*i+1].vPos;
		v2 = m_pPoint[3*i+2].vPos;

		// Check if the pick ray passes through this point
		if( DxEffectMan::GetInstance().IntersectTriangle( vPickRayOrig, vPickRayDir, v0, v1, v2,
								&fDist, &fBary1, &fBary2 ) )
		{
			if( (nCount==0) || (fDist < fDistPrev) )
			{
				if ( nCount == 0 )	
                    fDistPrev = fDist;

				m_dwCurTILE = m_pTriangle[i].dwMaterial;
				++nCount;

				TRIANGLELIST_ITER iter = m_listSelectTriangle.begin();
				for ( ; iter!=m_listSelectTriangle.end(); ++iter )
				{
					if ( (*iter) == i )	
                        bNew = FALSE;	// 같은거 있으면 등록 시키지 마라.
				}

				if ( bNew )	
                    m_listSelectTriangle.push_back ( i );
			}
			bUse = TRUE;
		}
	}

	if ( bUse )	return TRUE;
	else		return FALSE;
}

void	DxEffectTiling::SetListBox ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( SetStrList ( pd3dDevice, m_pMaterials [ m_dwCurTILE ].strTexture.c_str() ) )
	{
		++m_dwUNIQUE_NUMBER;
	}
}

void DxEffectTiling::DeleteColorALL()
{
	MATERIALMAP_ITER iter = m_mapMaterials.find( m_dwCurTILE );

	VERTEXDATA sVertex;

	if( iter!=m_mapMaterials.end() )
	{
		MATERIALEX* pMaterialEX = (*iter).second.get();

		TRIANGLELIST_ITER iter_S = pMaterialEX->listTriangle.begin();

		for( ; iter_S!=pMaterialEX->listTriangle.end(); ++iter_S )
		{
			DWORD dwFacesNUM = (*iter_S);

			for ( DWORD i=0; i<3; ++i )
			{
				sVertex.dwNUM = (dwFacesNUM*3) + i;
				sVertex.dwNUM_EX = m_pPoint[sVertex.dwNUM].dwPointEx;

				(*iter).second->m_pPointUseColor[ sVertex.dwNUM_EX ].dwColor = 0xffffffff;
				//m_pPointEX [ sVertex.dwNUM_EX ].dwColor = 0xffffffff;
			}
		}
	}
}

//	---------------------------------------------------------------------------------------------------
//									삼	각	형	생	성	부	분
//	---------------------------------------------------------------------------------------------------
void	DxEffectTiling::CreateTiles ( LPDIRECT3DDEVICEQ pd3dDevice, LIGHTMAPUV* pLightMapUV )
{
	MATERIALMAP_ITER iter = m_mapMaterials.begin();
	for ( ; iter!=m_mapMaterials.end(); ++iter )
	{
		CreateTile ( pd3dDevice, iter, pLightMapUV );	
	}
}

void	DxEffectTiling::CreateTile ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALMAP_ITER& iterMaterial, LIGHTMAPUV* pLightMapUV )
{
	(*iterMaterial).second->listFace.clear();

	DWORD	Numbering = 0;
	DWORD	dwFacesNUM;
	int		nCount;
	DWORD	dwMaterial;

	VERTEXDATA sVertex[3];

	dwMaterial = (*iterMaterial).first;
	SEPARATEOBJ* pSeparateRoot = NULL;

	TRIANGLELIST_ITER iter_DEL;
	TRIANGLELIST_ITER iter = (*iterMaterial).second->listTriangle.begin();
	for ( ; iter!=(*iterMaterial).second->listTriangle.end(); )
	{
		MATERIALEX* pMaterialEX = (*iterMaterial).second.get();

		dwFacesNUM = (*iter);

		nCount = 0;

		for ( DWORD i=0; i<3; ++i )
		{
			sVertex[i].dwNUM = (dwFacesNUM*3) + i;
			sVertex[i].bEqual = FALSE;

			sVertex[i].dwNUM_EX = m_pPoint[sVertex[i].dwNUM].dwPointEx;
			//if ( m_pPointEX [ sVertex[i].dwNUM_EX ].dwMaterial == dwMaterial )
			//{
			//	sVertex[i].bEqual = TRUE;
			//	++nCount;
			//}

			if ( pMaterialEX->m_pPointUseColor[ sVertex[i].dwNUM_EX ].bUse )
			{
				sVertex[i].bEqual = TRUE;
				++nCount;
			}
		}

		m_pTriangle[dwFacesNUM].bBase = TRUE;

		if ( nCount==1 )		OneSrcTriangle ( sVertex, pMaterialEX, pMaterialEX->listFace, pLightMapUV );
		else if ( nCount==2 )	TwoSrcTriangle ( sVertex, pMaterialEX, pMaterialEX->listFace, pLightMapUV );
		else if ( nCount==3 )
		{
			m_pTriangle[dwFacesNUM].bBase = FALSE;
			ThrSrcTriangle ( sVertex, pMaterialEX, pMaterialEX->listFace, pLightMapUV );
		}
		else	// Delete
		{
			iter_DEL = iter;
			++iter;
			pMaterialEX->listTriangle.erase ( iter_DEL );	// erase

			continue;
		}

		++iter;
	}
}

void	DxEffectTiling::OneSrcTriangle ( VERTEXDATA* pData, const MATERIALEX* pMaterialEX, FACELIST& pRoot, LIGHTMAPUV* pLightMapUV )
{
	BOOL bUse=FALSE;
	DWORD dwCenter;
	DWORD dwTemp_1;
	DWORD dwTemp_2;

	VERTEX sCenter;
	VERTEX sTemp_1;
	VERTEX sTemp_2;

	D3DXVECTOR2 avLightMapUV[3];
	D3DXVECTOR2 avLightMapUV_TEMP[3];

	// 임시 데이터에 값 넣기
	for ( DWORD i=0; i<3; ++i )
	{
		if ( pData[i].bEqual )			dwCenter = pData[i].dwNUM;
		else
		{
			if ( !bUse )
			{
				dwTemp_1 = pData[i].dwNUM;
				bUse = TRUE;
			}
			else	dwTemp_2 = pData[i].dwNUM;
		}
	}

	// 데이터 변화 시키기
	sCenter.vPos = m_pPoint[dwCenter].vPos;
	sCenter.vTex = m_pPoint[dwCenter].vTex;
	sTemp_1.vPos = m_pPoint[dwTemp_1].vPos;
	sTemp_1.vTex = m_pPoint[dwTemp_1].vTex;
	sTemp_2.vPos = m_pPoint[dwTemp_2].vPos;
	sTemp_2.vTex = m_pPoint[dwTemp_2].vTex;

	sTemp_1.vPos = (sTemp_1.vPos-sCenter.vPos)*0.5f;
	sTemp_1.vPos = sCenter.vPos + sTemp_1.vPos;
	sTemp_2.vPos = (sTemp_2.vPos-sCenter.vPos)*0.5f;
	sTemp_2.vPos = sCenter.vPos + sTemp_2.vPos;

	sTemp_1.vTex = (sTemp_1.vTex-sCenter.vTex)*0.5f;
	sTemp_1.vTex = sCenter.vTex + sTemp_1.vTex;
	sTemp_2.vTex = (sTemp_2.vTex-sCenter.vTex)*0.5f;
	sTemp_2.vTex = sCenter.vTex + sTemp_2.vTex;

	if ( pLightMapUV )
	{
		avLightMapUV_TEMP[0] = pLightMapUV->m_vecUV[dwCenter];	// 메인
		avLightMapUV_TEMP[1] = (pLightMapUV->m_vecUV[dwCenter] + pLightMapUV->m_vecUV[dwTemp_1])*0.5f;	// 1
		avLightMapUV_TEMP[2] = (pLightMapUV->m_vecUV[dwCenter] + pLightMapUV->m_vecUV[dwTemp_2])*0.5f;	// 2
	}


	// 데이터 삽입
	DWORD Numbering = 0;
    vm::VERTEXNORCOLORTEXTANGENT sVertex[3];

	DWORD	dwVertexNUM;
	//DWORD	dwColor = m_pPointEX [ m_pPoint[dwCenter].dwPointEx ].dwColor;
	DWORD	dwColor = pMaterialEX->m_pPointUseColor[ m_pPoint[dwCenter].dwPointEx ].dwColor;
	DWORD	dwColor_NOALPHA = (dwColor&0xffffff);

	for ( DWORD i=0; i<3; ++i )
	{
		dwVertexNUM = pData[i].dwNUM;
		sVertex[Numbering].vPos = m_pPoint[dwVertexNUM].vPos;
		sVertex[Numbering].vNor = m_pPoint[dwVertexNUM].vNor;
		sVertex[Numbering].cColor = dwColor;
		sVertex[Numbering].vTex = m_pPoint[dwVertexNUM].vTex;
        sVertex[Numbering].vTan = m_pPoint[dwVertexNUM].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[0];

		if ( dwVertexNUM==dwTemp_1 )
		{
			sVertex[Numbering].cColor = dwColor_NOALPHA;
			sVertex[Numbering].vPos = sTemp_1.vPos;
			sVertex[Numbering].vTex = sTemp_1.vTex;
			avLightMapUV[Numbering] = avLightMapUV_TEMP[1];
		}
		else if ( dwVertexNUM==dwTemp_2 )
		{
			sVertex[Numbering].cColor = dwColor_NOALPHA;
			sVertex[Numbering].vPos = sTemp_2.vPos;
			sVertex[Numbering].vTex = sTemp_2.vTex;
			avLightMapUV[Numbering] = avLightMapUV_TEMP[2];
		}

		++Numbering;
	}

	// List 에 값을 넣는다.
	SetSeparateList ( pRoot, sVertex[0], sVertex[1], sVertex[2], &avLightMapUV[0] );
}

void	DxEffectTiling::TwoSrcTriangle ( VERTEXDATA* pData, const MATERIALEX* pMaterialEX, FACELIST& pRoot, LIGHTMAPUV* pLightMapUV )
{
	BOOL bCCW=TRUE;
	BOOL bUse=FALSE;
	DWORD dwCenter;
	DWORD dwTemp_1;
	DWORD dwTemp_2;

	VERTEX sCenter;
	VERTEX sTemp_1;
	VERTEX sTemp_2;

	D3DXVECTOR2 avLightMapUV[6];
	D3DXVECTOR2 avLightMapUV_TEMP[4];

	// 임시 데이터에 값 넣기
	for ( DWORD i=0; i<3; ++i )
	{
		if ( !pData[i].bEqual )
		{
			dwCenter = pData[i].dwNUM;
			if ( i==1 )	bCCW=FALSE;
		}
		else
		{
			if ( !bUse )
			{
				dwTemp_1 = pData[i].dwNUM;
				bUse = TRUE;
			}
			else	dwTemp_2 = pData[i].dwNUM;
		}
	}

	// 데이터 변화 시키기
	sCenter.vPos = m_pPoint[dwCenter].vPos;
	sCenter.vTex = m_pPoint[dwCenter].vTex;
	sTemp_1.vPos = m_pPoint[dwTemp_1].vPos;
	sTemp_1.vTex = m_pPoint[dwTemp_1].vTex;
	sTemp_2.vPos = m_pPoint[dwTemp_2].vPos;
	sTemp_2.vTex = m_pPoint[dwTemp_2].vTex;

	sTemp_1.vPos = (sTemp_1.vPos-sCenter.vPos)*0.5f;
	sTemp_1.vPos = sCenter.vPos + sTemp_1.vPos;
	sTemp_2.vPos = (sTemp_2.vPos-sCenter.vPos)*0.5f;
	sTemp_2.vPos = sCenter.vPos + sTemp_2.vPos;

	sTemp_1.vTex = (sTemp_1.vTex-sCenter.vTex)*0.5f;
	sTemp_1.vTex = sCenter.vTex + sTemp_1.vTex;
	sTemp_2.vTex = (sTemp_2.vTex-sCenter.vTex)*0.5f;
	sTemp_2.vTex = sCenter.vTex + sTemp_2.vTex;

	if ( pLightMapUV )
	{
		avLightMapUV_TEMP[0] = pLightMapUV->m_vecUV[dwTemp_1];	// 메인 1
		avLightMapUV_TEMP[1] = pLightMapUV->m_vecUV[dwTemp_2];	// 메인 2
		avLightMapUV_TEMP[2] = (pLightMapUV->m_vecUV[dwCenter] + pLightMapUV->m_vecUV[dwTemp_1])*0.5f;	// 보조 1
		avLightMapUV_TEMP[3] = (pLightMapUV->m_vecUV[dwCenter] + pLightMapUV->m_vecUV[dwTemp_2])*0.5f;	// 보조 2
	}

	//DWORD	dwColor1 = m_pPointEX [ m_pPoint[dwTemp_1].dwPointEx ].dwColor;
	//DWORD	dwColor2 = m_pPointEX [ m_pPoint[dwTemp_2].dwPointEx ].dwColor;
	DWORD	dwColor1 = pMaterialEX->m_pPointUseColor[ m_pPoint[dwTemp_1].dwPointEx ].dwColor;
	DWORD	dwColor2 = pMaterialEX->m_pPointUseColor[ m_pPoint[dwTemp_2].dwPointEx ].dwColor;
	dwColor1 = (dwColor1&0xffffff);
	dwColor2 = (dwColor2&0xffffff);

	DWORD Numbering = 0;
	vm::VERTEXNORCOLORTEXTANGENT sVertex[6];

	// 데이터 삽입
	if ( bCCW )
	{
		sVertex[Numbering].vPos = sTemp_1.vPos;
		sVertex[Numbering].vNor = m_pPoint[dwCenter].vNor;
		sVertex[Numbering].cColor = dwColor1;
		sVertex[Numbering].vTex = sTemp_1.vTex;
        sVertex[Numbering].vTan = m_pPoint[dwCenter].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[2];
		++Numbering;

		sVertex[Numbering].vPos = m_pPoint[dwTemp_1].vPos;
		sVertex[Numbering].vNor = m_pPoint[dwTemp_1].vNor;
		//sVertex[Numbering].cColor = m_pPointEX [ m_pPoint[dwTemp_1].dwPointEx ].dwColor;
		sVertex[Numbering].cColor = pMaterialEX->m_pPointUseColor[ m_pPoint[dwTemp_1].dwPointEx ].dwColor;
		sVertex[Numbering].vTex = m_pPoint[dwTemp_1].vTex;
        sVertex[Numbering].vTan = m_pPoint[dwTemp_1].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[0];
		++Numbering;

		sVertex[Numbering].vPos = sTemp_2.vPos;
		sVertex[Numbering].vNor = m_pPoint[dwCenter].vNor;
		sVertex[Numbering].cColor = dwColor2;
		sVertex[Numbering].vTex = sTemp_2.vTex;
        sVertex[Numbering].vTan = m_pPoint[dwCenter].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[3];
		++Numbering;



		sVertex[Numbering].vPos = m_pPoint[dwTemp_1].vPos;
		sVertex[Numbering].vNor = m_pPoint[dwTemp_1].vNor;
		//sVertex[Numbering].cColor = m_pPointEX [ m_pPoint[dwTemp_1].dwPointEx ].dwColor;
		sVertex[Numbering].cColor = pMaterialEX->m_pPointUseColor[ m_pPoint[dwTemp_1].dwPointEx ].dwColor;
		sVertex[Numbering].vTex = m_pPoint[dwTemp_1].vTex;
        sVertex[Numbering].vTan = m_pPoint[dwTemp_1].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[0];
		++Numbering;

		sVertex[Numbering].vPos = m_pPoint[dwTemp_2].vPos;
		sVertex[Numbering].vNor = m_pPoint[dwTemp_2].vNor;
		//sVertex[Numbering].cColor = m_pPointEX [ m_pPoint[dwTemp_2].dwPointEx ].dwColor;
		sVertex[Numbering].cColor = pMaterialEX->m_pPointUseColor[ m_pPoint[dwTemp_2].dwPointEx ].dwColor;
		sVertex[Numbering].vTex = m_pPoint[dwTemp_2].vTex;
        sVertex[Numbering].vTan = m_pPoint[dwTemp_2].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[1];
		++Numbering;

		sVertex[Numbering].vPos = sTemp_2.vPos;
		sVertex[Numbering].vNor = m_pPoint[dwCenter].vNor;
		sVertex[Numbering].cColor = dwColor2;
		sVertex[Numbering].vTex = sTemp_2.vTex;
        sVertex[Numbering].vTan = m_pPoint[dwCenter].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[3];
		++Numbering;
	}
	else
	{
		sVertex[Numbering].vPos = sTemp_2.vPos;
		sVertex[Numbering].vNor = m_pPoint[dwCenter].vNor;
		sVertex[Numbering].cColor = dwColor2;
		sVertex[Numbering].vTex = sTemp_2.vTex;
        sVertex[Numbering].vTan = m_pPoint[dwCenter].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[3];
		++Numbering;

		sVertex[Numbering].vPos = m_pPoint[dwTemp_1].vPos;
		sVertex[Numbering].vNor = m_pPoint[dwTemp_1].vNor;
		//sVertex[Numbering].cColor = m_pPointEX [ m_pPoint[dwTemp_1].dwPointEx ].dwColor;
		sVertex[Numbering].cColor = pMaterialEX->m_pPointUseColor[ m_pPoint[dwTemp_1].dwPointEx ].dwColor;
		sVertex[Numbering].vTex = m_pPoint[dwTemp_1].vTex;
        sVertex[Numbering].vTan = m_pPoint[dwTemp_1].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[0];
		++Numbering;

		sVertex[Numbering].vPos = sTemp_1.vPos;
		sVertex[Numbering].vNor = m_pPoint[dwCenter].vNor;
		sVertex[Numbering].cColor = dwColor1;
		sVertex[Numbering].vTex = sTemp_1.vTex;
        sVertex[Numbering].vTan = m_pPoint[dwCenter].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[2];
		++Numbering;



		sVertex[Numbering].vPos = m_pPoint[dwTemp_2].vPos;
		sVertex[Numbering].vNor = m_pPoint[dwTemp_2].vNor;
		//sVertex[Numbering].cColor = m_pPointEX [ m_pPoint[dwTemp_2].dwPointEx ].dwColor;
		sVertex[Numbering].cColor = pMaterialEX->m_pPointUseColor[ m_pPoint[dwTemp_2].dwPointEx ].dwColor;
		sVertex[Numbering].vTex = m_pPoint[dwTemp_2].vTex;
        sVertex[Numbering].vTan = m_pPoint[dwTemp_2].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[1];
		++Numbering;

		sVertex[Numbering].vPos = m_pPoint[dwTemp_1].vPos;
		sVertex[Numbering].vNor = m_pPoint[dwTemp_1].vNor;
		//sVertex[Numbering].cColor = m_pPointEX [ m_pPoint[dwTemp_1].dwPointEx ].dwColor;
		sVertex[Numbering].cColor = pMaterialEX->m_pPointUseColor[ m_pPoint[dwTemp_1].dwPointEx ].dwColor;
		sVertex[Numbering].vTex = m_pPoint[dwTemp_1].vTex;
        sVertex[Numbering].vTan = m_pPoint[dwTemp_1].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[0];
		++Numbering;

		sVertex[Numbering].vPos = sTemp_2.vPos;
		sVertex[Numbering].vNor = m_pPoint[dwCenter].vNor;
		sVertex[Numbering].cColor = dwColor2;
		sVertex[Numbering].vTex = sTemp_2.vTex;
        sVertex[Numbering].vTan = m_pPoint[dwCenter].vTan;
		avLightMapUV[Numbering] = avLightMapUV_TEMP[3];
		++Numbering;
	}

	// List 에 값을 넣는다.
	SetSeparateList ( pRoot, sVertex[0], sVertex[1], sVertex[2], &avLightMapUV[0] );
	SetSeparateList ( pRoot, sVertex[3], sVertex[4], sVertex[5], &avLightMapUV[3] );
}

void	DxEffectTiling::ThrSrcTriangle ( VERTEXDATA* pData, const MATERIALEX* pMaterialEX, FACELIST& pRoot, LIGHTMAPUV* pLightMapUV )
{
	DWORD	dwVertexNUM;

	DWORD Numbering = 0;
	vm::VERTEXNORCOLORTEXTANGENT sVertex[3];
	D3DXVECTOR2 avLightMapUV[3];

	for ( DWORD i=0; i<3; ++i )
	{
		dwVertexNUM = pData[i].dwNUM;
		sVertex[Numbering].vPos = m_pPoint[dwVertexNUM].vPos;
		sVertex[Numbering].vNor = m_pPoint[dwVertexNUM].vNor;
		//sVertex[Numbering].cColor = m_pPointEX [ pData[i].dwNUM_EX ].dwColor;
		sVertex[Numbering].cColor = pMaterialEX->m_pPointUseColor[ pData[i].dwNUM_EX ].dwColor;
		sVertex[Numbering].vTex = m_pPoint[dwVertexNUM].vTex;
        sVertex[Numbering].vTan = m_pPoint[dwVertexNUM].vTan;
		if ( pLightMapUV )
		{
			avLightMapUV[Numbering] = pLightMapUV->m_vecUV[dwVertexNUM];
		}
		++Numbering;
	}

	// List 에 값을 넣는다.
	SetSeparateList ( pRoot, sVertex[0], sVertex[1], sVertex[2], &avLightMapUV[0] );
}

void	DxEffectTiling::SetSeparateList ( FACELIST& pRoot, const vm::VERTEXNORCOLORTEXTANGENT& sVertex_0, 
											const vm::VERTEXNORCOLORTEXTANGENT& sVertex_1, const vm::VERTEXNORCOLORTEXTANGENT& sVertex_2, D3DXVECTOR2 avLightMapUV[] )
{
	SEPARATEOBJ sObj;
	sObj.m_sVertex[0].vPos      = sVertex_0.vPos;
    sObj.m_sVertex[0].vNor      = sVertex_0.vNor;
    sObj.m_sVertex[0].dwColor   = sVertex_0.cColor;
    sObj.m_sVertex[0].vTex      = sVertex_0.vTex;

    sObj.m_sVertex[1].vPos      = sVertex_1.vPos;
    sObj.m_sVertex[1].vNor      = sVertex_1.vNor;
    sObj.m_sVertex[1].dwColor   = sVertex_1.cColor;
    sObj.m_sVertex[1].vTex      = sVertex_1.vTex;

    sObj.m_sVertex[2].vPos      = sVertex_2.vPos;
    sObj.m_sVertex[2].vNor      = sVertex_2.vNor;
    sObj.m_sVertex[2].dwColor   = sVertex_2.cColor;
    sObj.m_sVertex[2].vTex      = sVertex_2.vTex;

    sObj.m_vTangetArray[0] = sVertex_0.vTan;
	sObj.m_vTangetArray[1] = sVertex_1.vTan;
	sObj.m_vTangetArray[2] = sVertex_2.vTan;

	sObj.m_avLightMapUV[0] = avLightMapUV[0];
	sObj.m_avLightMapUV[1] = avLightMapUV[1];
	sObj.m_avLightMapUV[2] = avLightMapUV[2];

	pRoot.push_back ( sObj );
}