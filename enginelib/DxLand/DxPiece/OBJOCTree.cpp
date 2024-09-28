#include "pch.h"
#include <omp.h>
#include "../../../VisualMaterialLib/Util/DxTangentMesh.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxEffect/TexEff/DxTexEff.h"
#include "../../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../../DxTools/Occlusion/NSOcclusionOctree.h"
#include "../../DxTools/Occlusion/DxOcclusionQuery.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/Collision.h"
#include "../../G-Logic/GLPeriod.h"
#include "../LightMap/LightMapObject.h"
#include "../LightMap/NSLightMapFX.h"
#include "../LightMap/NSLightMap.h"
#include "./NSOctreeRenderList.h"
#include "./NsOCTree.h"

#include "OBJOCTree.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------------------------------------------------------
//										O	B	J		O	C	T	r	e	e
// -----------------------------------------------------------------------------------------------------------------------------------------

OBJOCTree::OBJOCTree()
    : AABBBinaryTreeBase(ABTT_OCTREE)
	, pNormalColor(NULL)
	, m_emLandType(EMLR_DAYNIGHT_ON)
	, m_dwFVF(VERTEXNORCOLORTEX::FVF)
	, m_nFVFSize(0)
	, m_dwVert(0L)
	, m_pVertSrc(NULL)
	, m_pColor(NULL)
	, m_pVB(NULL)
	, m_pVB_Checker(NULL)
	, m_pIB(NULL)
    , m_bTangentMesh_m_pVertSrc(FALSE)
	, m_pLightMapUV(NULL)
	, m_dwCopyIndex(0)
{
	//InitializeCriticalSection(&m_pCSLockVB);
}

OBJOCTree::~OBJOCTree()
{
	CleanUp();

	//DeleteCriticalSection(&m_pCSLockVB);
}

void OBJOCTree::CleanUp()
{
    AABBBinaryTreeBase::CleanUp_Base();

	m_dwVert = 0L;
	m_dwFace = 0L;
	SAFE_DELETE_ARRAY( m_pVertSrc );
	SAFE_DELETE_ARRAY( m_pColor );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pVB_Checker );
	SAFE_RELEASE( m_pIB );

	
	SAFE_DELETE_ARRAY( pNormalColor );
	SAFE_DELETE_ARRAY( m_pLightMapUV );
	

	m_bCheck = FALSE;
	m_bLoad = FALSE;
	m_dwFileCur = 0;
}

BOOL OBJOCTree::IsLoad() const
{
	if( !m_bLoad )			
		return FALSE;

	//if( !m_pVB )			
	//	return FALSE;

	//if( !m_pIB )			
	//	return FALSE;

	//if ( !m_pCollsionTREE )	return FALSE;	// Prev Version - No Data

	// [shhan][2015.02.24] VB, IB 가 있어야 m_pCollsionTREE 생성가능.
	//						현재는 무조건 m_pCollsionTREE 가 있어야 되도록 작업됨.
	if ( !m_pCollsionTREE )
		return FALSE;

	return TRUE; 
}

void OBJOCTree::DrawEff( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !NSOcclusionOctree::Visible( m_nID ) )
	{
		return;
	}

	//EnterCriticalSection(&m_pCSLockVB);

 //   pd3dDevice->SetFVF( m_dwFVF );
	//pd3dDevice->SetStreamSource( 0, m_pVB, 0, m_nFVFSize );
	//pd3dDevice->SetIndices( m_pIB );
	//pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwVert, 0, m_dwFace );

	// [shhan][2015.04.28] 타 Thread 에서 VB 에 Lock 거는 문제가 있나 싶어서 Main 으로 옮겨옴.
	//
	// [shhan][2015.08.13] Thread 에서 Color 계산하는 것까지 가지고 왔다.
	//						또한 컬러 계산은 EMLR_DAYNIGHT_ON 만 하도록 하고, EMLR_DAYNIGHT_OFF, EMLR_PIECE 는 할 필요가 없다 안하도록 한다.
	//
	if ( m_emLandType == EMLR_DAYNIGHT_ON )
	{
		DWORD dwSub = 0;
		DWORD dwAdd = 10;	//1600;
		//for( ; m_dwCopyIndex<m_dwVert; )
		if ( m_dwCopyIndex < m_dwVert )
		{
			dwSub = m_dwVert - m_dwCopyIndex;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;

			{
				ComputeCOLOR( m_dwCopyIndex, m_dwCopyIndex+dwSub );	// 계산을 한다. i부터 i+dwSub-1까지	(i,i+dwSub]

				if ( m_bTangentMesh_m_pVertSrc )
				{
					BYTE* pVertices(NULL);
					HRESULT hr = m_spTangentMesh->GetVB()->Lock( m_nFVFSize*m_dwCopyIndex, m_nFVFSize*dwSub, (VOID**)&pVertices, 0L );
					if( FAILED( hr ) )
					{
						//LeaveCriticalSection(&m_pCSLockVB);
						return;
					}
					memcpy( pVertices, &m_pVertSrc[m_dwCopyIndex], m_nFVFSize*dwSub );
					m_spTangentMesh->GetVB()->Unlock();
				}
				else
				{
					BYTE* pVertices(NULL);
					HRESULT hr = m_pVB->Lock( m_nFVFSize*m_dwCopyIndex, m_nFVFSize*dwSub, (VOID**)&pVertices, 0L );
					if( FAILED( hr ) )
					{
						//LeaveCriticalSection(&m_pCSLockVB);
						return;
					}
					memcpy( pVertices, &m_pVertSrc[m_dwCopyIndex], m_nFVFSize*dwSub );
					m_pVB->Unlock();
				}
			}

			m_dwCopyIndex += dwSub;
		}

		if ( m_dwCopyIndex >= m_dwVert )
		{
			m_dwCopyIndex = 0; 
		}
	}

	switch ( NSLightMapFX::g_emRenderType )
	{
	case NSLIGHTMAPDEF::LMRT_CHECKER:
		{
			CreateCheckerMesh( pd3dDevice );

			if ( m_pVB_Checker && !NSLightMapFX::Render( pd3dDevice, m_pVB_Checker, m_pIB, m_nFVFSize, m_dwFVF, m_dwVert, m_dwFace ) )
			{
				NSVERTEXNORTEX_FX::Render( pd3dDevice, m_pVB, m_pIB, m_nFVFSize, m_dwFVF, m_dwVert, m_dwFace );
			}
		}
		break;

	default:
		{
			NSVERTEXNORTEX_FX::Render( pd3dDevice, m_pVB, m_pIB, m_nFVFSize, m_dwFVF, m_dwVert, m_dwFace );
		}
		break;
	};
	
	//LeaveCriticalSection(&m_pCSLockVB);
}

void OBJOCTree::RenderOctreeEff( LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffBase* pEff )
{
	pd3dDevice->SetIndices ( m_pIB );			
	pEff->Render( pd3dDevice, m_dwVert, GetFaceNum(), m_pVertSrc );
}

void OBJOCTree::Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex )
{
	if ( !m_pd3dQuery )
		return;

	if ( NSOcclusionExceptIndex::IsHaveExceptIndexOctree( m_nID ) )
		return;

	if( !m_bQueryTest )
	{
		// 1차검사 안되었으면 검사하고 플래그를 바꾼다.
		m_pd3dQuery->Issue( D3DISSUE_BEGIN );
		
		DrawEff( pd3dDevice );

		m_pd3dQuery->Issue( D3DISSUE_END );

		m_bQueryTest = TRUE;
	}
}

void OBJOCTree::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecOctreeIndex )
{
	if ( !m_bQueryTest )
	{
		m_dwCullPixels = 0;
		return;
	}

	DWORD dwCullingDelay = timeGetTime();

	while ( GetDataQuery( m_dwCullPixels ) == S_FALSE )
	{
		//DrawEff( pd3dDevice );

		D3DXVECTOR3 VertBox[8];
		VertBox[0] = D3DXVECTOR3( 100.f,	100.f,	100.f );
		VertBox[1] = D3DXVECTOR3( 100.f,	100.f,	0.f );
		VertBox[2] = D3DXVECTOR3( 0.f,		100.f,	100.f );
		VertBox[3] = D3DXVECTOR3( 0.f,		100.f,	0.f );
		VertBox[4] = D3DXVECTOR3( 100.f,	0.f,	100.f );
		VertBox[5] = D3DXVECTOR3( 100.f,	0.f,	0.f );
		VertBox[6] = D3DXVECTOR3( 0.f,		0.f,	100.f );
		VertBox[7] = D3DXVECTOR3( 0.f,		0.f,	0.f );
		pd3dDevice->DrawPrimitiveUP ( D3DPT_POINTLIST, 8, VertBox, 12 );
	}

	// 딜레이를 누적한다.
	DxOcclusionQuery::g_dwCullingDelay += timeGetTime() - dwCullingDelay;

	// 1차검사 되었다고 2차검사가 제대로 완료 되었으므로 초기화 한다.
	m_bQueryTest = FALSE;

	if ( m_dwCullPixels != 0 )
	{
		vecOctreeIndex.push_back( m_nID );
	}
}

void OBJOCTree::CreateBASE( const LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwVert, const DWORD dwFaces )
{
	// Note : 로딩 되어있는 데이터 형.
	m_dwVert = dwVert;
	m_dwFace = dwFaces;

	SAFE_DELETE_ARRAY( pNormalColor );			// 계속 쓰일 것이 아니다. 나중에 삭제 될 것.
	pNormalColor = new NORMALCOLOR[m_dwVert];	// 계속 쓰일 것이 아니다. 나중에 삭제 될 것.

	SAFE_DELETE_ARRAY( m_pVertSrc );
	m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];

	SAFE_DELETE_ARRAY( m_pColor );
	m_pColor = new DIRECTPOINTCOLOR[m_dwVert];

	//// Note : Piece 일 경우만 Normal을 사용한다.
	//switch( m_emLandType )
	//{
	//case EMLR_DAYNIGHT_ON:
	//	m_dwFVF = VERTEXNORCOLORTEX::FVF;
	//	break;
	//case EMLR_DAYNIGHT_OFF:
	//	m_dwFVF = VERTEXNORCOLORTEX::FVF;
	//	break;
	//case EMLR_PIECE:
	//	m_dwFVF = VERTEXNORCOLORTEX::FVF;
	//	break;
	//};
	m_dwFVF = VERTEXNORCOLORTEX::FVF;
	m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );

	//EnterCriticalSection(&m_pCSLockVB);
	{		
		SAFE_RELEASE( m_pVB );
		pd3dDevice->CreateVertexBuffer( m_dwVert*m_nFVFSize, D3DUSAGE_DYNAMIC, m_dwFVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
		if( !m_pVB )
		{
			//// Note : 전체<->윈도우 화면으로 바뀌면서 pd3dDevice를 잃어버릴 경우가 있다. <Reset the device>
			////		추측이지만 이쪽은 Thread이므로 그 때 이곳에 들어와서 Lock 할 경우 실패가 뜨는거 같다.			
			//CDebugSet::ToLogFile( "Call Me(Sung-Hwan Han) - OBJOCTree::CreateBASE() - CreateVertexBuffer Error" );

			//LeaveCriticalSection(&m_pCSLockVB);
			return;
		}
	}
	//LeaveCriticalSection(&m_pCSLockVB);

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( !m_pIB )
	{
		//// Note : 전체<->윈도우 화면으로 바뀌면서 pd3dDevice를 잃어버릴 경우가 있다. <Reset the device>
		////		추측이지만 이쪽은 Thread이므로 그 때 이곳에 들어와서 Lock 할 경우 실패가 뜨는거 같다.
		//CDebugSet::ToLogFile( "Call Me(Sung-Hwan Han) - OBJOCTree::CreateBASE() - CreateIndexBuffer Error" );
		return;
	}
}

void OBJOCTree::CovertPieceSetting( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUserLightMapUV )
{
	if( !m_pOcMesh )							
		return;

	if( m_pOcMesh->GetFVF() != VERTEXNORCOLORTEX5::FVF )	
		return;

	m_dwFVF = VERTEXNORCOLORTEX4::FVF;
	m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );
	m_dwVert = m_pOcMesh->GetNumVertices();
	m_dwFace = m_pOcMesh->GetNumFaces();

	// Note : VB IB 생성
	//EnterCriticalSection(&m_pCSLockVB);
	{		
		SAFE_RELEASE( m_pVB );
		pd3dDevice->CreateVertexBuffer( m_dwVert*m_nFVFSize, D3DUSAGE_DYNAMIC, m_dwFVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
		if( !m_pVB )
			return;
	}
	//LeaveCriticalSection(&m_pCSLockVB);

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( !m_pIB )
		return;

	// Note : 데이터 얻어오기
	//EnterCriticalSection(&m_pCSLockVB);
	{
		VERTEXNORCOLORTEX5* pVertexSRC(NULL);
		VERTEXNORCOLORTEX4* pVertexDEST(NULL);
		m_pOcMesh->LockVertexBuffer( 0L, (VOID**)&pVertexSRC );
		m_pVB->Lock( 0, 0, (VOID**)&pVertexDEST, D3DLOCK_DISCARD );
		//memcpy( pVertexDEST, pVertexSRC, sizeof(VERTEX)*m_dwVert );
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			pVertexDEST[i].vPos = pVertexSRC[i].vPos;
			pVertexDEST[i].vNor = pVertexSRC[i].vNor;
			pVertexDEST[i].dwColor = pVertexSRC[i].dwColor;
			pVertexDEST[i].vTex[0] = pVertexSRC[i].vTex[0];
			pVertexDEST[i].vTex[1] = pVertexSRC[i].vTex[1];
			pVertexDEST[i].vTex[2] = pVertexSRC[i].vTex[2];
			pVertexDEST[i].vTex[3] = pVertexSRC[i].vTex[3];
		}
		m_pVB->Unlock();
		m_pOcMesh->UnlockVertexBuffer();
	}
	//LeaveCriticalSection(&m_pCSLockVB);

	WORD* pIndexSRC(NULL);
	WORD* pIndexDEST(NULL);
	m_pOcMesh->LockIndexBuffer( 0L, (VOID**)&pIndexSRC );
	m_pIB->Lock( 0, 0, (VOID**)&pIndexDEST, 0L );
	memcpy( pIndexDEST, pIndexSRC, sizeof(WORD)*3*m_dwFace );
	m_pIB->Unlock();
	m_pOcMesh->UnlockIndexBuffer();

	// Note : User가 만든 데이터는 알맞게 저장시켜 놓는다.
	SAFE_DELETE_ARRAY( m_pLightMapUV );
	if ( bUserLightMapUV )
	{
		VERTEXNORCOLORTEX5* pVertexSRC(NULL);
		m_pOcMesh->LockVertexBuffer( 0L, (VOID**)&pVertexSRC );

		m_pLightMapUV = new D3DXVECTOR2[m_dwVert];
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			m_pLightMapUV[i] = pVertexSRC[i].vTex[4];
		}

		m_pOcMesh->UnlockVertexBuffer();
	}

	// Note : 데이터 정리
	SAFE_RELEASE( m_pOcMesh );
}

void OBJOCTree::CreateVertSrc( BYTE* pByte, const DWORD dwFVF, const DWORD dwVert )
{
	// Note : 저장방식을 풀어서 데이터형을 VERTEXNORCOLORTEX::FVF로 만든다.
	//		무조건 D3DFVF_XYZ 와 D3DFVF_NORMAL 와 D3DFVF_TEX1 는 있다고 가정한다.
	//		차이는 D3DFVF_DIFFUSE 가 있는냐 없느냐가 차이이다.
	DWORD dwFVFTemp = dwFVF & (~D3DFVF_TEXCOUNT_MASK);
	UINT nSize = D3DXGetFVFVertexSize( dwFVF );
	UINT nOffSet = D3DXGetFVFVertexSize( dwFVFTemp );

	dwFVFTemp = dwFVF & D3DFVF_POSITION_MASK;
	UINT nPositionMarkSize = D3DXGetFVFVertexSize( dwFVFTemp );

	D3DXVECTOR3* pVector(NULL);
	D3DXVECTOR3* pNormal(NULL);
	D3DXVECTOR2* pTex(NULL);
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		pVector = (D3DXVECTOR3*)( pByte + (nSize*i) );
		pNormal = (D3DXVECTOR3*)( pByte + (nSize*i) + nPositionMarkSize );
		pTex = (D3DXVECTOR2*)( pByte + (nSize*i) + nOffSet );

		m_pVertSrc[i].vPos = *pVector;
		m_pVertSrc[i].vNor = *pNormal;
		m_pVertSrc[i].dwColor = 0xffffffff;
		m_pVertSrc[i].vTex = *pTex;
	}
}

void OBJOCTree::CreateNormalColor( BYTE* pByte, const DWORD dwFVF, const DWORD dwVert )
{
	BOOL bDiffuse = (BOOL)( dwFVF&D3DFVF_DIFFUSE );
	UINT nSize = D3DXGetFVFVertexSize( dwFVF );
	D3DXVECTOR3* pVector(NULL);
	DWORD* pDiffuse(NULL);
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		pVector = (D3DXVECTOR3*)( pByte + (nSize*i) + sizeof(D3DXVECTOR3) );
		pDiffuse = (DWORD*)( pByte + (nSize*i) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3) );
		pNormalColor[i].vNor = *pVector;
		D3DXVec3Normalize( &pNormalColor[i].vNor, &pNormalColor[i].vNor );

		if( bDiffuse )
		{
			pNormalColor[i].cColor.a = (((*pDiffuse)&0xff000000)>>24) / 255.f;
			pNormalColor[i].cColor.r = (((*pDiffuse)&0xff0000)>>16) / 255.f;
			pNormalColor[i].cColor.g = (((*pDiffuse)&0xff00)>>8) / 255.f;
			pNormalColor[i].cColor.b = ((*pDiffuse)&0xff) / 255.f;
		}
		else
		{
			pNormalColor[i].cColor.a = 1.f;
			pNormalColor[i].cColor.r = 1.f;
			pNormalColor[i].cColor.g = 1.f;
			pNormalColor[i].cColor.b = 1.f;
		}
	}

	// Note : 낮밤 변화가 있을 경우와 없을 경우 2가지를 보아야 한다.
	if( m_emLandType==EMLR_DAYNIGHT_ON )
	{
		ComputeNormalColor( dwVert );
	}
	else if( m_emLandType==EMLR_DAYNIGHT_OFF )
	{
		ComputeNormalColorDAYNIGHT_OFF( dwVert );
	}
	else
	{
		//CDebugSet::ToLogFile( "OBJOCTree::CreateNormalColor에서의 컬러 셋팅 에러" );
	}

	// Note : 데이터 정리
	SAFE_DELETE_ARRAY( pNormalColor );
}

void OBJOCTree::ComputeNormalColor( const DWORD dwVert )
{
	float fColorR(0.f);
	float fColorG(0.f);
	float fColorB(0.f);
	float fDot(0.f);
	D3DXVECTOR3 vDirect(0.f,0.f,0.f);
	DWORD		dwColor(0L);

	// ... Point Light의 계산을 한다.
	//		많은 속도부하가 생기지 싶다.
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		dwColor = DxLightMan::GetInstance()->ComputeClosedPointLight( &m_pVertSrc[i].vPos, &pNormalColor[i].vNor );	// 가까운 빛들을 계산한 값을 얻는다.

		m_pColor[i].wNightR = (WORD)((dwColor&0xff0000)>>16);
		m_pColor[i].wNightG = (WORD)((dwColor&0xff00)>>8);
		m_pColor[i].wNightB = (WORD)(dwColor&0xff);
	}

	// Note : 낮일 경우의 컬러를 구한다.
	//
	DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectNoonLight();
	vDirect.x = -pLight->m_Light.Direction.x;
	vDirect.y = -pLight->m_Light.Direction.y;
	vDirect.z = -pLight->m_Light.Direction.z;

	for( DWORD i=0; i<m_dwVert; ++i )
	{
		D3DXVec3Normalize( &vDirect, &vDirect );
		D3DXVec3Normalize( &pNormalColor[i].vNor, &pNormalColor[i].vNor );
		fDot = D3DXVec3Dot( &pNormalColor[i].vNor, &vDirect );
		if( fDot > 0.f )	// 빛과의 방향을 맞춘다.
		{
			// Note : Diffuse Color를 구한다.
			fColorR = pLight->m_Light.Diffuse.r*fDot;
			fColorG = pLight->m_Light.Diffuse.g*fDot;
			fColorB = pLight->m_Light.Diffuse.b*fDot;

			// Note : Ambient Color를 더한다.
			fColorR += pLight->m_Light.Ambient.r;
			fColorG += pLight->m_Light.Ambient.g;
			fColorB += pLight->m_Light.Ambient.b;

			fColorR = (fColorR>1.f) ? 1.f : fColorR;
			fColorG = (fColorG>1.f) ? 1.f : fColorG;
			fColorB = (fColorB>1.f) ? 1.f : fColorB;

			m_pColor[i].wDayR = (WORD)(fColorR*255.f);
			m_pColor[i].wDayG = (WORD)(fColorG*255.f);
			m_pColor[i].wDayB = (WORD)(fColorB*255.f);
		}
		else
		{
			// Note : Ambient Color를 구한다.
			fColorR = pLight->m_Light.Ambient.r;
			fColorG = pLight->m_Light.Ambient.g;
			fColorB = pLight->m_Light.Ambient.b;

			m_pColor[i].wDayR = (WORD)(fColorR*255.f);
			m_pColor[i].wDayG = (WORD)(fColorG*255.f);
			m_pColor[i].wDayB = (WORD)(fColorB*255.f);
		}
	}

	// Note : 낮에도 PointLight를 끄지 않는다.
	if( !DxLightMan::GetInstance()->IsWhenDayPointOff() )
	{
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			m_pColor[i].wDayR += m_pColor[i].wNightR;
			m_pColor[i].wDayG += m_pColor[i].wNightG;
			m_pColor[i].wDayB += m_pColor[i].wNightB;
		}
	}

	// Note : 밤일 경우의 컬러를 구한다.
	//
	pLight = DxLightMan::GetInstance()->GetDirectNightLight();
	vDirect.x = -pLight->m_Light.Direction.x;
	vDirect.y = -pLight->m_Light.Direction.y;
	vDirect.z = -pLight->m_Light.Direction.z;

	for( DWORD i=0; i<m_dwVert; ++i )
	{
		D3DXVec3Normalize( &vDirect, &vDirect );
		D3DXVec3Normalize( &pNormalColor[i].vNor, &pNormalColor[i].vNor );
		fDot = D3DXVec3Dot( &pNormalColor[i].vNor, &vDirect );
		if( fDot > 0.f )	// 빛과의 방향을 맞춘다.
		{
			// Note : Diffuse Color를 구한다.
			fColorR = pLight->m_Light.Diffuse.r*fDot;
			fColorG = pLight->m_Light.Diffuse.g*fDot;
			fColorB = pLight->m_Light.Diffuse.b*fDot;

			// Note : Ambient Color를 더한다.
			fColorR += pLight->m_Light.Ambient.r;
			fColorG += pLight->m_Light.Ambient.g;
			fColorB += pLight->m_Light.Ambient.b;

			fColorR = (fColorR>1.f) ? 1.f : fColorR;
			fColorG = (fColorG>1.f) ? 1.f : fColorG;
			fColorB = (fColorB>1.f) ? 1.f : fColorB;

			m_pColor[i].wNightR += (WORD)(fColorR*255.f);
			m_pColor[i].wNightG += (WORD)(fColorG*255.f);
			m_pColor[i].wNightB += (WORD)(fColorB*255.f);
		}
		else
		{
			// Note : Ambient Color를 구한다.
			fColorR = pLight->m_Light.Ambient.r;
			fColorG = pLight->m_Light.Ambient.g;
			fColorB = pLight->m_Light.Ambient.b;

			m_pColor[i].wNightR += (WORD)(fColorR*255.f);
			m_pColor[i].wNightG += (WORD)(fColorG*255.f);
			m_pColor[i].wNightB += (WORD)(fColorB*255.f);
		}
	}

	// Note : 최종 색 , 원래 가지고 있던 Diffuse 색을 곱하는 것이다.
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		m_pColor[i].wAlpha = (WORD)( 255.f*pNormalColor[i].cColor.a );

		m_pColor[i].wDayR = (WORD)( m_pColor[i].wDayR * pNormalColor[i].cColor.r );
		m_pColor[i].wDayG = (WORD)( m_pColor[i].wDayG * pNormalColor[i].cColor.g );
		m_pColor[i].wDayB = (WORD)( m_pColor[i].wDayB * pNormalColor[i].cColor.b );

		m_pColor[i].wNightR = (WORD)( m_pColor[i].wNightR * pNormalColor[i].cColor.r );
		m_pColor[i].wNightG = (WORD)( m_pColor[i].wNightG * pNormalColor[i].cColor.g );
		m_pColor[i].wNightB = (WORD)( m_pColor[i].wNightB * pNormalColor[i].cColor.b );
	}

	// Note : 컬러를 임시로 복제
	WORD wTempColor(0);
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		dwColor = (m_pColor[i].wAlpha<<24);

		switch( GLPeriod::GetInstance().m_sFGTime )
		{
		case FGT_DAYTIME:
			wTempColor = m_pColor[i].wDayR;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += (wTempColor<<16);

			wTempColor = m_pColor[i].wDayG;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += (wTempColor<<8);

			wTempColor = m_pColor[i].wDayB;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += wTempColor;

			break;

		case FGT_DAWN:
		case FGT_SUNSET:
		case FGT_NIGHT:
			wTempColor = m_pColor[i].wNightR;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += (wTempColor<<16);

			wTempColor = m_pColor[i].wNightG;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += (wTempColor<<8);

			wTempColor = m_pColor[i].wNightB;
			if( wTempColor > 255 )	wTempColor = 255;
			dwColor += wTempColor;

			break;
		};

		m_pVertSrc[i].dwColor = dwColor;
	}
}

void OBJOCTree::ComputeNormalColorDAYNIGHT_OFF( const DWORD dwVert )
{
	float fColorR(0.f);
	float fColorG(0.f);
	float fColorB(0.f);
	float fDot(0.f);
	D3DXVECTOR3 vDirect(0.f,0.f,0.f);
	DWORD		dwColor(0L);

	// ... Point Light의 계산을 한다.
	//		많은 속도부하가 생기지 싶다.
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		dwColor = DxLightMan::GetInstance()->ComputeClosedPointLight( &m_pVertSrc[i].vPos, &pNormalColor[i].vNor );	// 가까운 빛들을 계산한 값을 얻는다.

		m_pColor[i].wNightR = (WORD)((dwColor&0xff0000)>>16);
		m_pColor[i].wNightG = (WORD)((dwColor&0xff00)>>8);
		m_pColor[i].wNightB = (WORD)(dwColor&0xff);
	}

	// Note : 디폴트는 낮 일 경우의 빛이다.
	DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectNoonLight();
	vDirect.x = -pLight->m_Light.Direction.x;
	vDirect.y = -pLight->m_Light.Direction.y;
	vDirect.z = -pLight->m_Light.Direction.z;

	for( DWORD i=0; i<m_dwVert; ++i )
	{
		D3DXVec3Normalize( &vDirect, &vDirect );
		D3DXVec3Normalize( &pNormalColor[i].vNor, &pNormalColor[i].vNor );
		fDot = D3DXVec3Dot( &pNormalColor[i].vNor, &vDirect );
		if( fDot > 0.f )	// 빛과의 방향을 맞춘다.
		{
			// Note : Diffuse Color를 구한다.
			fColorR = pLight->m_Light.Diffuse.r*fDot;
			fColorG = pLight->m_Light.Diffuse.g*fDot;
			fColorB = pLight->m_Light.Diffuse.b*fDot;

			// Note : Ambient Color를 더한다.
			fColorR += pLight->m_Light.Ambient.r;
			fColorG += pLight->m_Light.Ambient.g;
			fColorB += pLight->m_Light.Ambient.b;

			fColorR = (fColorR>1.f) ? 1.f : fColorR;
			fColorG = (fColorG>1.f) ? 1.f : fColorG;
			fColorB = (fColorB>1.f) ? 1.f : fColorB;

			m_pColor[i].wDayR = (WORD)(fColorR*255.f);
			m_pColor[i].wDayG = (WORD)(fColorG*255.f);
			m_pColor[i].wDayB = (WORD)(fColorB*255.f);
		}
		else
		{
			// Note : Ambient Color를 구한다.
			fColorR = pLight->m_Light.Ambient.r;
			fColorG = pLight->m_Light.Ambient.g;
			fColorB = pLight->m_Light.Ambient.b;

			m_pColor[i].wDayR = (WORD)(fColorR*255.f);
			m_pColor[i].wDayG = (WORD)(fColorG*255.f);
			m_pColor[i].wDayB = (WORD)(fColorB*255.f);
		}
	}

	// Note : 낮에도 PointLight를 끄지 않는다.
	if( !DxLightMan::GetInstance()->IsWhenDayPointOff() )
	{
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			m_pColor[i].wDayR += m_pColor[i].wNightR;
			m_pColor[i].wDayG += m_pColor[i].wNightG;
			m_pColor[i].wDayB += m_pColor[i].wNightB;
		}
	}

	// Note : 최종 색 
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		m_pColor[i].wAlpha = (WORD)( 255.f*pNormalColor[i].cColor.a );

		m_pColor[i].wDayR = (WORD)( m_pColor[i].wDayR * pNormalColor[i].cColor.r );
		m_pColor[i].wDayG = (WORD)( m_pColor[i].wDayG * pNormalColor[i].cColor.g );
		m_pColor[i].wDayB = (WORD)( m_pColor[i].wDayB * pNormalColor[i].cColor.b );
	}

	// Note : 컬러를 임시로 복제
	WORD wTempColor(0);
	for( DWORD i=0; i<m_dwVert; ++i )
	{
		dwColor = (m_pColor[i].wAlpha<<24);

		wTempColor = m_pColor[i].wDayR;
		if( wTempColor > 255 )	wTempColor = 255;
		dwColor += (wTempColor<<16);

		wTempColor = m_pColor[i].wDayG;
		if( wTempColor > 255 )	wTempColor = 255;
		dwColor += (wTempColor<<8);

		wTempColor = m_pColor[i].wDayB;
		if( wTempColor > 255 )	wTempColor = 255;
		dwColor += wTempColor;

		m_pVertSrc[i].dwColor = dwColor;
	}

	// Note : 복제
	//EnterCriticalSection(&m_pCSLockVB);
	{
		VERTEXNORCOLORTEX* pVertices(NULL);
		m_pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
		memcpy( pVertices, m_pVertSrc, sizeof(VERTEXNORCOLORTEX)*m_dwVert );
		m_pVB->Unlock();
	}
	//LeaveCriticalSection(&m_pCSLockVB);

	// Note : 데이터 정리
	SAFE_DELETE_ARRAY( m_pColor );
}

void OBJOCTree::CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//EnterCriticalSection(&m_pCSLockVB);
	{
		m_spTangentMesh = std::tr1::shared_ptr<DxTangentMesh>(new DxTangentMesh);
		m_spTangentMesh->CreateMesh( pd3dDevice, m_pVB, m_pIB, m_dwFVF, m_dwVert, m_dwFace );
	}
	//LeaveCriticalSection(&m_pCSLockVB);
}

void OBJOCTree::CreateTangentColorMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//EnterCriticalSection(&m_pCSLockVB);
	{
		m_spTangentMesh = std::tr1::shared_ptr<DxTangentMesh>(new DxTangentMesh);
		m_spTangentMesh->CreateColorMesh( pd3dDevice, m_pVB, m_pIB, m_dwFVF, m_dwVert, m_dwFace );
	}
	//LeaveCriticalSection(&m_pCSLockVB);
}

void OBJOCTree::CreateCheckerMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pVB_Checker )
	{
		SAFE_RELEASE( m_pVB_Checker );
		pd3dDevice->CreateVertexBuffer( m_dwVert*m_nFVFSize, D3DUSAGE_DYNAMIC, m_dwFVF, D3DPOOL_SYSTEMMEM, &m_pVB_Checker, NULL );

		if ( m_pVB && m_pVB_Checker )
		{
			if ( m_pLightMapUV )
			{
				D3DXVECTOR2* pUV_DEST;
				BYTE* pVertexSRC;
				BYTE* pVertexDEST;
				m_pVB->Lock( 0, 0, (VOID**)&pVertexSRC, D3DLOCK_READONLY );
				m_pVB_Checker->Lock( 0, 0, (VOID**)&pVertexDEST, D3DLOCK_DISCARD );
				memcpy( pVertexDEST, pVertexSRC, m_dwVert*m_nFVFSize );
				for ( DWORD i=0; i<m_dwVert; ++i )
				{
					pUV_DEST = (D3DXVECTOR2*)( pVertexDEST + ((i+1)*m_nFVFSize) - sizeof(D3DXVECTOR2) );
					*pUV_DEST = m_pLightMapUV[i];
				}
				m_pVB_Checker->Unlock();
				m_pVB->Unlock();
			}
			else
			{
				D3DXVECTOR3 vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
				D3DXVECTOR3 vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );

				D3DXVECTOR2* pUV_DEST;
				BYTE* pVertexSRC;
				BYTE* pVertexDEST;
				m_pVB->Lock( 0, 0, (VOID**)&pVertexSRC, D3DLOCK_READONLY );
				m_pVB_Checker->Lock( 0, 0, (VOID**)&pVertexDEST, D3DLOCK_DISCARD );
				memcpy( pVertexDEST, pVertexSRC, m_dwVert*m_nFVFSize );
				for ( DWORD i=0; i<m_dwVert; ++i )
				{
					D3DXVECTOR3 *pVecter = (D3DXVECTOR3*)( pVertexDEST + m_nFVFSize*i );

					if ( vMax.x < pVecter->x )	vMax.x = pVecter->x;
					if ( vMax.y < pVecter->y )	vMax.y = pVecter->y;
					if ( vMax.z < pVecter->z )	vMax.z = pVecter->z;

					if ( vMin.x > pVecter->x )	vMin.x = pVecter->x;
					if ( vMin.y > pVecter->y )	vMin.y = pVecter->y;
					if ( vMin.z > pVecter->z )	vMin.z = pVecter->z;
				}

				int nIndex(0);
				D3DXVECTOR3 vDir = vMax - vMin;
				if ( vDir.x >= vDir.y && vDir.x >= vDir.z )
				{
					if ( vDir.y >= vDir.z )	// x,y
						nIndex = 0;
					else					// x,z
						nIndex = 1;
				}
				else if ( vDir.y >= vDir.x && vDir.y >= vDir.z )
				{
					if ( vDir.x >= vDir.z )	// x,y
						nIndex = 0;
					else					// y,z
						nIndex = 2;
				}
				else
				{
					if ( vDir.x >= vDir.y )	// x,z
						nIndex = 1;
					else					// y,z
						nIndex = 2;
				}

				for ( DWORD i=0; i<m_dwVert; ++i )
				{
					D3DXVECTOR3 *pVecter = (D3DXVECTOR3*)( pVertexDEST + m_nFVFSize*i );
					pUV_DEST = (D3DXVECTOR2*)( pVertexDEST + ((i+1)*m_nFVFSize) - sizeof(D3DXVECTOR2) );
					if ( nIndex == 0 )
					{
						pUV_DEST->x = pVecter->x;
						pUV_DEST->y = pVecter->y;
					}
					else if ( nIndex == 1 )
					{
						pUV_DEST->x = pVecter->x;
						pUV_DEST->y = pVecter->z;
					}
					else
					{
						pUV_DEST->x = pVecter->y;
						pUV_DEST->y = pVecter->z;
					}

				}
				m_pVB_Checker->Unlock();
				m_pVB->Unlock();
			}
		}
	}
}

void OBJOCTree::RenderVisualMaterial_Color( LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffBase* pEff, BOOL bRealPL, const D3DXMATRIX& matWorld, float fAlpha )
{
	//EnterCriticalSection(&m_pCSLockVB);
	{
		switch ( NSLightMapFX::g_emRenderType )
		{
		case NSLIGHTMAPDEF::LMRT_CHECKER:
			{
				CreateCheckerMesh( pd3dDevice );
				
				if ( m_pVB_Checker && !NSLightMapFX::Render( pd3dDevice, m_pVB_Checker, m_pIB, m_nFVFSize, m_dwFVF, m_dwVert, m_dwFace ) )
				{
					pEff->RenderColor( pd3dDevice, m_spTangentMesh.get(), 0, bRealPL, matWorld, fAlpha );
				}
			}
			break;

		default:
			{
				pEff->RenderColor( pd3dDevice, m_spTangentMesh.get(), 0, bRealPL, matWorld, fAlpha );
			}
			break;
		};
	}
	//LeaveCriticalSection(&m_pCSLockVB);
}

void OBJOCTree::CopyVertSrcToVB( BYTE* pByte, VERTEXNORCOLORTEX* pVertSrc, LPDIRECT3DVERTEXBUFFERQ pVB, const DWORD dwVert, DWORD dwFVF )
{
	if( !pVertSrc )	return;
	if( !pVB )		return;

	if( VERTEX::FVF == dwFVF )
	{
		//EnterCriticalSection(&m_pCSLockVB);
		{
			VERTEX* pVertSRC = reinterpret_cast<VERTEX*>( pByte );
			VERTEXNORCOLORTEX* pVertices;
			pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<dwVert; ++i )
			{
				pVertices[i].vPos = pVertSRC[i].vPos;
				pVertices[i].vNor = pVertSRC[i].vNor;
				pVertices[i].dwColor = 0xffffffff;
				pVertices[i].vTex = pVertSRC[i].vTex;
			}
			pVB->Unlock();
		}
		//LeaveCriticalSection(&m_pCSLockVB);
	}
	else if ( VERTEXNORCOLORTEX::FVF == dwFVF )
	{
		//EnterCriticalSection(&m_pCSLockVB);
		{
			VERTEXNORCOLORTEX* pVertices;
			pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
			memcpy( pVertices, pVertSrc, sizeof(VERTEXNORCOLORTEX)*dwVert );
			pVB->Unlock();
		}
		//LeaveCriticalSection(&m_pCSLockVB);
	}
}

void OBJOCTree::CopyMeshToIB( WORD* pSrc, LPDIRECT3DINDEXBUFFERQ pIB, const DWORD dwFaces )
{
	if( !pSrc )		return;
	if( !pIB )		return;

	WORD* pIndices(NULL);
	pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
	memcpy( pIndices, pSrc, sizeof(WORD)*3*dwFaces );
	pIB->Unlock();
}

//void OBJOCTree::InsertColorList(LOADINGDATALIST& listColorData)
//{
//	// Note : Loading이 되어 있지 않으면 VB 도 없기 때문에 작업할 수가 없다.
//	if (!IsLoad())
//        return;
//	if (!m_pColor)
//        return;
//
//	DWORD_PTR dwData = (DWORD_PTR) (this);
//	listColorData.push_back(dwData);
//}

//void OBJOCTree::FrameMoveCOLOR()	// Thread로 진행 된다.
//{
//	DWORD dwTick = timeGetTime();
//
//	//EnterCriticalSection(&m_pCSLockVB);
//	{
//		// TangentMesh 일 경우 m_spTangentMesh 의 Data를 m_pVertSrc 으로 복제한다.
//		// VERTEXNORCOLORTEX 과 vm::VERTEXNORCOLORTEXTANGENT_COMPRESS 의 사이즈는 같다.
//		if ( m_spTangentMesh.get() )
//		{
//			if ( !m_bTangentMesh_m_pVertSrc )
//			{
//				if ( m_spTangentMesh->GetVB() )
//				{
//					BYTE* pVertices(NULL);
//					HRESULT hr = m_spTangentMesh->GetVB()->Lock( 0, m_nFVFSize*m_dwVert, (VOID**)&pVertices, D3DLOCK_READONLY );
//					if( FAILED( hr ) )
//					{
//						//LeaveCriticalSection(&m_pCSLockVB);
//						return;
//					}
//					memcpy( m_pVertSrc, pVertices, m_nFVFSize*m_dwVert );
//					m_spTangentMesh->GetVB()->Unlock();
//
//					// 작업이 성공하면 TRUE 를 주도록 한다.
//					m_bTangentMesh_m_pVertSrc = TRUE;
//				}
//			}
//		}
//	}
//	//LeaveCriticalSection(&m_pCSLockVB);
//
//	DWORD dwSub = 0;
//	DWORD dwAdd = 200;
//	for( DWORD i=0; i<m_dwVert; )
//	{
//		dwSub = m_dwVert - i;
//		dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
//
//		ComputeCOLOR( i, i+dwSub );	// 계산을 한다. i부터 i+dwSub-1까지	(i,i+dwSub]
//
//		i += dwSub;
//
//		//dwTick = timeGetTime() - dwTick;
//
//		//// Log에 남겨본다.
//		//if ( dwTick > 10 )
//		//	sc::writeLogInfo( sc::string::format("DxStaticMeshColor_THREAD::LoadThread #1 ------------- %1%", dwTick ) );
//
//		Sleep(0);
//
//		dwTick = timeGetTime();
//	}
//
//	{
//		// [shhan][2015.04.14] Crash 가 자주와서 코드변경함
//		//
//		//dwSub = 0;
//		//dwAdd = 1600;	//1600;
//		//for( DWORD i=0; i<m_dwVert; )
//		//{
//		//	dwSub = m_dwVert - i;
//		//	dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
//
//		//	//EnterCriticalSection(&m_pCSLockVB);
//		//	{
//		//		dwTick = timeGetTime();
//
//		//		if ( m_bTangentMesh_m_pVertSrc )
//		//		{
//		//			BYTE* pVertices(NULL);
//		//			HRESULT hr = m_spTangentMesh->GetVB()->Lock( m_nFVFSize*i, m_nFVFSize*dwSub, (VOID**)&pVertices, 0L );
//		//			if( FAILED( hr ) )
//		//			{
//		//				//// Note : 전체<->윈도우 화면으로 바뀌면서 pd3dDevice를 잃어버릴 경우가 있다. <Reset the device>
//		//				////		추측이지만 이쪽은 Thread이므로 그 때 이곳에 들어와서 Lock 할 경우 실패가 뜨는거 같다.
//		//				//CDebugSet::ToLogFile( "Call Me(Sung-Hwan Han) - OBJOCTree::FrameMoveCOLOR() - Lock Error" );
//
//		//				//LeaveCriticalSection(&m_pCSLockVB);
//		//				return;
//		//			}
//		//			memcpy( pVertices, &m_pVertSrc[i], m_nFVFSize*dwSub );
//		//			m_spTangentMesh->GetVB()->Unlock();
//		//		}
//		//		else
//		//		{
//		//			BYTE* pVertices(NULL);
//		//			HRESULT hr = m_pVB->Lock( m_nFVFSize*i, m_nFVFSize*dwSub, (VOID**)&pVertices, 0L );
//		//			if( FAILED( hr ) )
//		//			{
//		//				//// Note : 전체<->윈도우 화면으로 바뀌면서 pd3dDevice를 잃어버릴 경우가 있다. <Reset the device>
//		//				////		추측이지만 이쪽은 Thread이므로 그 때 이곳에 들어와서 Lock 할 경우 실패가 뜨는거 같다.
//		//				//CDebugSet::ToLogFile( "Call Me(Sung-Hwan Han) - OBJOCTree::FrameMoveCOLOR() - Lock Error" );
//
//		//				//LeaveCriticalSection(&m_pCSLockVB);
//		//				return;
//		//			}
//		//			memcpy( pVertices, &m_pVertSrc[i], m_nFVFSize*dwSub );
//		//			m_pVB->Unlock();
//		//		}
//
//		//		dwTick = timeGetTime() - dwTick;
//
//		//		//// Log에 남겨본다.
//		//		//if ( dwTick > 10 )
//		//		//	sc::writeLogInfo( sc::string::format("DxStaticMeshColor_THREAD::LoadThread #2 ------------- %1%", dwTick ) );
//		//	}
//		//	//LeaveCriticalSection(&m_pCSLockVB);
//
//		//	i += dwSub;
//
//		//	Sleep(0);
//		//}
//
//
//		//EnterCriticalSection(&m_pCSLockVB);
//		//{
//		//	//dwTick = timeGetTime();
//
//		//	if ( m_bTangentMesh_m_pVertSrc )
//		//	{
//		//		BYTE* pVertices(NULL);
//		//		HRESULT hr = m_spTangentMesh->GetVB()->Lock( 0, 0, (VOID**)&pVertices, 0L );
//		//		if( FAILED( hr ) )
//		//		{
//		//			//LeaveCriticalSection(&m_pCSLockVB);
//		//			return;
//		//		}
//		//		memcpy( pVertices, m_pVertSrc, m_nFVFSize*m_dwVert );
//		//		m_spTangentMesh->GetVB()->Unlock();
//		//	}
//		//	else
//		//	{
//		//		BYTE* pVertices(NULL);
//		//		HRESULT hr = m_pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
//		//		if( FAILED( hr ) )
//		//		{
//		//			//LeaveCriticalSection(&m_pCSLockVB);
//		//			return;
//		//		}
//		//		memcpy( pVertices, m_pVertSrc, m_nFVFSize*m_dwVert );
//		//		m_pVB->Unlock();
//		//	}
//
//		//	//dwTick = timeGetTime() - dwTick;
//
//		//	//// Log에 남겨본다.
//		//	//if ( dwTick > 10 )
//		//	//	sc::writeLogInfo( sc::string::format("DxStaticMeshColor_THREAD::LoadThread #2 ------------- %1%", dwTick ) );
//		//}
//		//LeaveCriticalSection(&m_pCSLockVB);
//
//		Sleep(0);
//	}
//}

void OBJOCTree::ComputeCOLOR( const DWORD& dwStart, const DWORD& dwEnd )
{
    if (!m_pColor)
        return;


    DWORD dwColor(0L);
    DWORD dwColorR(0);
    DWORD dwColorG(0);
    DWORD dwColorB(0);
    // float 이었는데 최적화 하기 위해서 2^16을 곱했다.
    // 아래에서 다시 shift 연산을 해서 복원한다.
    // GLPeriod::GetInstance().GetBlendFact() + GLPeriod::GetInstance().GetNightFact() = 1 이다.
    DWORD dwBlendFact = (DWORD) (GLPeriod::GetInstance().GetBlendFact() * 65536);
    DWORD dwNightFact = (DWORD) (GLPeriod::GetInstance().GetNightFact() * 65536);
    float fBlueFact = GLPeriod::GetInstance().GetBlueFact();
    float fRedFact = GLPeriod::GetInstance().GetRedFact();

    int OmpStart = static_cast<int> (dwStart);
    int OmpEnd   = static_cast<int> (dwEnd);
    // 혹시 omp 를 사용하고 싶으면 테스트 해보고 시도해 보시오.
//  #pragma omp parallel for
    for (int i=OmpStart; i<OmpEnd; ++i)
    {
        //switch( GLPeriod::GetInstance().m_sFGTime )
        //{
        //case FGT_DAWN:
        //	wColorR = (WORD)(m_pColor[i].wDayR*fBlendFact) + (WORD)(m_pColor[i].wNightR*fNightFact);
        //	wColorG = (WORD)(m_pColor[i].wDayG*fBlendFact) + (WORD)(m_pColor[i].wNightG*fNightFact);
        //	wColorB = (WORD)(m_pColor[i].wDayB*fBlendFact) + (WORD)(m_pColor[i].wNightB*fNightFact);
        //	break;
        //case FGT_DAYTIME:
        //	wColorR = (WORD)(m_pColor[i].wDayR*fBlendFact);
        //	wColorG = (WORD)(m_pColor[i].wDayG*fBlendFact);
        //	wColorB = (WORD)(m_pColor[i].wDayB*fBlendFact);
        //	break;
        //case FGT_SUNSET:
        //	wColorR = (WORD)(m_pColor[i].wDayR*fBlendFact) + (WORD)(m_pColor[i].wNightR*fNightFact);
        //	wColorG = (WORD)(m_pColor[i].wDayG*fBlendFact) + (WORD)(m_pColor[i].wNightG*fNightFact);
        //	wColorB = (WORD)(m_pColor[i].wDayB*fBlendFact) + (WORD)(m_pColor[i].wNightB*fNightFact);
        //	break;
        //case FGT_NIGHT:
        //	wColorR = (WORD)(m_pColor[i].wNightR*fNightFact);
        //	wColorG = (WORD)(m_pColor[i].wNightG*fNightFact);
        //	wColorB = (WORD)(m_pColor[i].wNightB*fNightFact);
        //	break;
        //};



        dwColor = (m_pColor[i].wAlpha<<24);

        dwColorR = m_pColor[i].wDayR*dwBlendFact;
        dwColorG = m_pColor[i].wDayG*dwBlendFact;
        dwColorB = m_pColor[i].wDayB*dwBlendFact;

        dwColorR += m_pColor[i].wNightR*dwNightFact;
        dwColorG += m_pColor[i].wNightG*dwNightFact;
        dwColorB += m_pColor[i].wNightB*dwNightFact;

        // 위에서  2^16을 곱했다.
        // 다시 shift 연산을 해서 복원한다.
        dwColorR = dwColorR >> 16;
        dwColorG = dwColorG >> 16;
        dwColorB = dwColorB >> 16;

        if( dwColorR > 255 )
            dwColorR = 255;
        if( dwColorG > 255 )
            dwColorG = 255;
        if( dwColorB > 255 )
            dwColorB = 255;

        dwColor += (dwColorR<<16);
        dwColor += (dwColorG<<8);
        dwColor += dwColorB;

        m_pVertSrc[i].dwColor = dwColor;
    }
}

void OBJOCTree::GetVertexNum( DWORD& dwVertexNum )
{
	dwVertexNum += m_dwFace * 3;
}

void OBJOCTree::GetVectorVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV )
{
    //if ( m_emLandType != EMLR_PIECE )
    //    return;

    if ( !m_pVB || !m_pIB )
        return;

	
    VERTEXNORCOLORTEX4 sVertex;
    BYTE* pVertices(NULL);
    WORD* pIndices(NULL);
    m_pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_READONLY );
    m_pIB->Lock( 0, 0, (VOID**)&pIndices, D3DLOCK_READONLY );
   
	//if ( m_pLightMapUV )
	//{
	//	for ( DWORD i=0; i<m_dwFace; ++i )
	//	{
	//		for ( DWORD j=0; j<3; ++j )
	//		{
	//			pPos = (D3DXVECTOR3*)( pVertices + (pwIndex[i*3+j]*m_nFVFSize) );
	//			pNor = (D3DXVECTOR3*)( pVertices + (pwIndex[i*3+j]*m_nFVFSize) + sizeof(D3DXVECTOR3) );
	//			pUV = (D3DXVECTOR2*)( pVertices + ((pwIndex[i*3+j]+1)*m_nFVFSize) - sizeof(D3DXVECTOR2) );

	//			sVertex.vPos = *pPos;
	//			sVertex.vNor = *pNor;
	//			sVertex.dwColor = 0xffffffff;
	//			sVertex.vTex[0] = *pUV;
	//			sVertex.vTex[1] = m_pLightMapUV[ pwIndex[i*3+j] ];
	//			sVertex.vTex[2] = D3DXVECTOR2(0.f,0.f);
	//			sVertex.vTex[3] = D3DXVECTOR2(0.f,0.f);
	//			vecVertices.push_back( sVertex );
	//		}
	//	}
	//}
	//else
	//{
	//	for ( DWORD i=0; i<m_dwFace; ++i )
	//	{
	//		for ( DWORD j=0; j<3; ++j )
	//		{
	//			pPos = (D3DXVECTOR3*)( pVertices + (pwIndex[i*3+j]*m_nFVFSize) );
	//			pNor = (D3DXVECTOR3*)( pVertices + (pwIndex[i*3+j]*m_nFVFSize) + sizeof(D3DXVECTOR3) );
	//			pUV = (D3DXVECTOR2*)( pVertices + ((pwIndex[i*3+j]+1)*m_nFVFSize) - sizeof(D3DXVECTOR2) );

	//			sVertex.vPos = *pPos;
	//			sVertex.vNor = *pNor;
	//			sVertex.dwColor = 0xffffffff;
	//			sVertex.vTex[0] = *pUV;
	//			sVertex.vTex[1] = D3DXVECTOR2(0.f,0.f);
	//			sVertex.vTex[2] = D3DXVECTOR2(0.f,0.f);
	//			sVertex.vTex[3] = D3DXVECTOR2(0.f,0.f);
	//			vecVertices.push_back( sVertex );
	//		}
	//	}
	//}


	D3DXVECTOR3* pVectorPos;
	D3DXVECTOR3* pVectorNor;
	D3DXVECTOR2* pVector2;
	D3DXVECTOR2* pVectorTex2;
	D3DXVECTOR2* pVectorTex3;
	D3DXVECTOR2* pVectorTex4;
	DWORD*       pColor;
	
	UINT nSIZE = D3DXGetFVFVertexSize ( m_dwFVF );

	for ( DWORD i=0; i<m_dwFace; ++i )
	{
		for ( DWORD j=0; j<3; ++j )
		{
			DWORD dwSizeADD(0);

			WORD nIndex = pIndices[i*3+j];

			if ( m_dwFVF & D3DFVF_XYZ )
			{
				pVectorPos = reinterpret_cast<D3DXVECTOR3*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vPos = *pVectorPos;

				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			if ( m_dwFVF & D3DFVF_NORMAL )
			{
				pVectorNor = reinterpret_cast<D3DXVECTOR3*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vNor = *pVectorNor;

				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			if ( m_dwFVF & D3DFVF_DIFFUSE )
			{
				pColor = reinterpret_cast<DWORD*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.dwColor = *pColor;

				dwSizeADD += sizeof(DWORD);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX1 )
			{
				pVector2 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[0] = *pVector2;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX2 )
			{
				pVectorTex2 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[1] = *pVectorTex2;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX3 )
			{
				pVectorTex3 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[2] = *pVectorTex3;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX4 )
			{
				pVectorTex4 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[3] = *pVectorTex4;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}

			vecVertices.push_back( sVertex );

			if ( m_pLightMapUV )
			{
				vecLightMapUV.push_back( m_pLightMapUV[ nIndex ] );
			}
		}
	}
	
    m_pIB->Unlock();
    m_pVB->Unlock();
}

void OBJOCTree::GetVectorVertices( std::vector<VERTEXNORCOLORTEX5>& vecVertices )
{
	if ( !m_pVB || !m_pIB )
		return;

	VERTEXNORCOLORTEX5 sVertex;
	BYTE* pVertices(NULL);
	WORD* pIndices(NULL);
	m_pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_READONLY );
	m_pIB->Lock( 0, 0, (VOID**)&pIndices, D3DLOCK_READONLY );

	D3DXVECTOR3* pVectorPos;
	D3DXVECTOR3* pVectorNor;
	D3DXVECTOR2* pVector2;
	D3DXVECTOR2* pVectorTex2;
	D3DXVECTOR2* pVectorTex3;
	D3DXVECTOR2* pVectorTex4;
	D3DXVECTOR2* pVectorTex5;
	DWORD*       pColor;

	UINT nSIZE = D3DXGetFVFVertexSize ( m_dwFVF );

	for ( DWORD i=0; i<m_dwFace; ++i )
	{
		for ( DWORD j=0; j<3; ++j )
		{
			DWORD dwSizeADD(0);

			WORD nIndex = pIndices[i*3+j];

			if ( m_dwFVF & D3DFVF_XYZ )
			{
				pVectorPos = reinterpret_cast<D3DXVECTOR3*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vPos = *pVectorPos;

				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			if ( m_dwFVF & D3DFVF_NORMAL )
			{
				pVectorNor = reinterpret_cast<D3DXVECTOR3*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vNor = *pVectorNor;

				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			if ( m_dwFVF & D3DFVF_DIFFUSE )
			{
				pColor = reinterpret_cast<DWORD*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.dwColor = *pColor;

				dwSizeADD += sizeof(DWORD);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX1 )
			{
				pVector2 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[0] = *pVector2;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX2 )
			{
				pVectorTex2 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[1] = *pVectorTex2;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX3 )
			{
				pVectorTex3 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[2] = *pVectorTex3;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX4 )
			{
				pVectorTex4 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[3] = *pVectorTex4;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}

			if ( m_pLightMapUV )
			{
				sVertex.vTex[4] = m_pLightMapUV[ nIndex ];
			}
			else if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX5 )
			{
				pVectorTex5 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[4] = *pVectorTex5;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}

			vecVertices.push_back( sVertex );
		}
	}

	m_pIB->Unlock();
	m_pVB->Unlock();
}

OBJOCTree* OBJOCTree::GetLeftChild_OBJOCTree()
{
    return MIN_BOOST::polymorphic_downcast<OBJOCTree*>( m_pLeftChild ); // dynamic_cast 해야 하지만 속도 때문에 static_cast 으로 한다.
}

OBJOCTree* OBJOCTree::GetRightChild_OBJOCTree()
{
    return MIN_BOOST::polymorphic_downcast<OBJOCTree*>( m_pRightChild ); // dynamic_cast 해야 하지만 속도 때문에 static_cast 으로 한다.
}

void OBJOCTree::Save ( sc::SerialFile& SFile, BOOL bPiece )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		SFile << m_vMax;
		SFile << m_vMin;

		// Note : Object의 타입을 설정한다.
		if( bPiece )
		{
			m_emLandType = EMLR_PIECE;
		}
		else
		{
			if( DxLightMan::GetInstance()->IsNightAndDay() )	m_emLandType = EMLR_DAYNIGHT_ON;
			else												m_emLandType = EMLR_DAYNIGHT_OFF;		
		}

		SFile.BeginBlock();
		{
			BOOL bExist(FALSE);
			if( m_pVB && m_pIB )
			{
				bExist = TRUE;
			}

			SFile << bExist;
			if ( bExist )
			{
				SFile << VERSION_OCTREE;	// 버젼 저장.

				SFile << m_dwFVF;
				SFile << m_dwVert;
				SFile << m_dwFace;
				SFile << (DWORD)m_emLandType;

				m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );

				//EnterCriticalSection(&m_pCSLockVB);
				{
					BYTE* pVertices;
					m_pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_READONLY );
					SFile.WriteBuffer ( pVertices, m_nFVFSize*m_dwVert );
					m_pVB->Unlock();
				}
				//LeaveCriticalSection(&m_pCSLockVB);

				WORD* pIndices;
				m_pIB->Lock( 0, 0, (VOID**)&pIndices, D3DLOCK_READONLY );
				SFile.WriteBuffer ( pIndices, sizeof(WORD)*m_dwFace*3 );
				m_pIB->Unlock();

				switch( m_emLandType )
				{
				case EMLR_DAYNIGHT_ON:
					SFile.WriteBuffer( m_pColor, sizeof(DIRECTPOINTCOLOR)*m_dwVert );
					break;
				};
			}

			// 충돌 Mesh
			{
				bExist = m_pCollsionTREE ? TRUE : FALSE;
				SFile << bExist;
				if ( m_pCollsionTREE )
				{
					m_pCollsionTREE->SaveFile( SFile );
				}
			}
		}
		SFile.EndBlock();

		SFile.BeginBlock();
		{
			// Piece LightMap 용 UV Data
			{
				BOOL bExist(FALSE);
				if ( bPiece && m_dwVert > 0 && m_pLightMapUV )
				{
					bExist = TRUE;
				}
				else
					bExist = FALSE;

				SFile << bExist;
				if ( bExist )
				{
					SFile << m_dwVert;
					SFile.WriteBuffer( m_pLightMapUV, sizeof(D3DXVECTOR2)*m_dwVert );
				}
			}
		}
		SFile.EndBlock();
	}
	SFile.EndBlock();
}

void OBJOCTree::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )
{
	CleanUp();

	if ( !bPiece )
	{
		m_nID = NSOctreeRenderList::g_nRenderIndex++;
	}

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	if ( dwVer == VERSION )
	{
		// Note : Object의 타입을 설정한다.
		if( bPiece )
		{
			m_emLandType = EMLR_PIECE;
		}
		else
		{
			if( DxLightMan::GetInstance()->IsNightAndDay() )	m_emLandType = EMLR_DAYNIGHT_ON;
			else												m_emLandType = EMLR_DAYNIGHT_OFF;		
		}

		SFile >> m_vMax;
		SFile >> m_vMin;

		m_vMax += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		m_vMin -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

		DWORD dwBufferSize;
		SFile >> dwBufferSize;

		m_dwFileCur = SFile.GetfTell();
		if( bDynamicLoad && NSOCTREE::IsDynamicLoad() )	SFile.SetOffSet( m_dwFileCur+dwBufferSize );
		else											DynamicLoad( pd3dDevice, SFile, FALSE );

		SFile >> dwBufferSize;

		DWORD dwVert(0L);
		BOOL bExist(FALSE);
		SFile >> bExist;
		if ( bExist )
		{
			SFile >> dwVert;

			SAFE_DELETE_ARRAY( m_pLightMapUV );
			m_pLightMapUV = new D3DXVECTOR2[dwVert];
			SFile.ReadBuffer( m_pLightMapUV, sizeof(D3DXVECTOR2)*dwVert );

			// 만약 데이터가 맞지 않는다면 지워버리자.
			if ( dwVert != m_dwVert )
			{
				SAFE_DELETE_ARRAY( m_pLightMapUV );
			}
		}
	}
	else if ( dwVer == 0x101 || dwVer == 0x100 )
	{
		// Note : Object의 타입을 설정한다.
		if( bPiece )
		{
			m_emLandType = EMLR_PIECE;
		}
		else
		{
			if( DxLightMan::GetInstance()->IsNightAndDay() )	m_emLandType = EMLR_DAYNIGHT_ON;
			else												m_emLandType = EMLR_DAYNIGHT_OFF;		
		}

		SFile >> m_vMax;
		SFile >> m_vMin;

		m_vMax += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		m_vMin -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

		DWORD dwBufferSize;
		SFile >> dwBufferSize;

		m_dwFileCur = SFile.GetfTell();
		if( bDynamicLoad && NSOCTREE::IsDynamicLoad() )	
		{
			SFile.SetOffSet( m_dwFileCur+dwBufferSize );
		}
		else											
		{
			DynamicLoad( pd3dDevice, SFile, FALSE );

			// 101에서 작업을 잘 못하여서 이런식으로 작업됨.
			DWORD dwVert(0L);
			BOOL bExist(FALSE);
			SFile >> bExist;
			if ( bExist )
			{
				SFile >> dwVert;

				SAFE_DELETE_ARRAY( m_pLightMapUV );
				m_pLightMapUV = new D3DXVECTOR2[dwVert];
				SFile.ReadBuffer( m_pLightMapUV, sizeof(D3DXVECTOR2)*dwVert );

				// 만약 데이터가 맞지 않는다면 지워버리자.
				if ( dwVert != m_dwVert )
				{
					SAFE_DELETE_ARRAY( m_pLightMapUV );
				}
			}
		}
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
	}
}

void OBJOCTree::Load_Prev( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )
{
	CleanUp();

	if ( !bPiece )
	{
		m_nID = NSOctreeRenderList::g_nRenderIndex++;
	}

	// Note : Object의 타입을 설정한다.
	if( bPiece )
	{
		m_emLandType = EMLR_PIECE;
	}
	else
	{
		if( DxLightMan::GetInstance()->IsNightAndDay() )	m_emLandType = EMLR_DAYNIGHT_ON;
		else												m_emLandType = EMLR_DAYNIGHT_OFF;		
	}

	SFile >> m_vMax;
	SFile >> m_vMin;

	m_vMax += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
	m_vMin -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
	
	DWORD dwBufferSize;
	SFile >> dwBufferSize;

	m_dwFileCur = SFile.GetfTell();
	if( bDynamicLoad && NSOCTREE::IsDynamicLoad() )	SFile.SetOffSet( m_dwFileCur+dwBufferSize );
	else											DynamicLoad( pd3dDevice, SFile, FALSE );
}
//
//void OBJOCTree::ThreadLoad(LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading)
//{
//	//if (m_bCheck)
// //       return;	// 다시 안 들어오도록 체크해야 한다.
//	//m_bCheck = TRUE;
//
//	//DWORD_PTR dwData = (DWORD_PTR) (this);
//
//	//// Note : 로딩 데이터에 삽입해 넣는다.
//	//EnterCriticalSection(&CSLockLoading);
//	//{
//	//	listLoadingData.push_back( dwData );
//	//}
//	//LeaveCriticalSection(&CSLockLoading);
//}

BOOL OBJOCTree::DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread )
{
	BOOL	bUse = TRUE;

	// Note : Loading Complete
	if ( IsLoad() )		return TRUE;

	if ( !m_dwFileCur )	
	{
		bUse = FALSE;
		goto _RETURN;
	}

	SFile.SetOffSet( m_dwFileCur );

	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		DWORD dwVer;
		SFile >> dwVer;
		if( dwVer < 0x10000 )
		{
			// Note : 예전 버젼으로 되돌린다. 이때는 Normal을 사용했었다.
			//			dwVer는 이때 당시 FVF로 저장되었었다.	그래서 넘겨준 후 로딩을 해야한다.
			if( !OctreeLoadOLD( pd3dDevice, SFile, dwVer, bThread ) )
			{
				bUse = FALSE;
				goto _RETURN;
			}
		}
		else if( dwVer==0x10000 )
		{
			if( !OctreeLoad_10000( pd3dDevice, SFile, bThread ) )
			{
				bUse = FALSE;
				goto _RETURN;
			}
		}
		else if( dwVer==0x10001 )
		{
			if( !OctreeLoad_10001( pd3dDevice, SFile, bThread ) )
			{
				bUse = FALSE;
				goto _RETURN;
			}
		}
		else if( dwVer==VERSION_OCTREE )
		{
			if( !OctreeLoad( pd3dDevice, SFile, bThread ) )
			{
				bUse = FALSE;
				goto _RETURN;
			}
		}
		else
		{
			// Note : 없다고 생각하고 로딩을 하지 않는다.
			bUse = FALSE;
			goto _RETURN;
		}
	}

	SFile >> bExist;
	if ( bExist )
	{
		if( bThread )	Sleep(0);

		m_pCollsionTREE = new DxAABBNode;
		if ( m_pCollsionTREE )
		{
			m_pCollsionTREE->LoadFile_PREV( SFile );

			SAFE_DELETE( m_pCollsionTREE );
		}
	}

	if ( m_pVB && m_pIB )
	{
		NSOCTREE::MakeCollisionData_VBIB( m_pVB, m_pIB, m_dwFVF, m_dwFace, &m_pCollsionTREE );
	}

	m_bLoad = TRUE;		// 로딩 완료
	return bUse;

_RETURN:
	m_bLoad = FALSE;	// 로딩 실패
	return bUse;
}

BOOL OBJOCTree::OctreeLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread )
{
	DWORD dwTemp(0L);
	SFile >> m_dwFVF;
	SFile >> m_dwVert;
	SFile >> m_dwFace;
	SFile >> dwTemp;
	m_emLandType = (EM_LAND_TYPE)dwTemp;

	m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );

	DWORD dwTick(0L);
	if( bThread )	
	{
		Sleep(0);

		dwTick = timeGetTime();
	}

	// Note : 
	BYTE*	pTempVB(NULL);
	pTempVB = new BYTE[m_dwVert*m_nFVFSize];

	//EnterCriticalSection(&m_pCSLockVB);
	{		
		SAFE_RELEASE( m_pVB );
		pd3dDevice->CreateVertexBuffer( m_dwVert*m_nFVFSize, D3DUSAGE_DYNAMIC, m_dwFVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
		if( !m_pVB )
		{
			// Note : 임시로 만든것을 지워줌.
			SAFE_DELETE_ARRAY( pTempVB );

			//LeaveCriticalSection(&m_pCSLockVB);
			return FALSE;
		}
	}
	//LeaveCriticalSection(&m_pCSLockVB);

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( !m_pIB )
	{
		// Note : 임시로 만든것을 지워줌.
		SAFE_DELETE_ARRAY( pTempVB );

		//EnterCriticalSection(&m_pCSLockVB);
		SAFE_RELEASE( m_pVB );
		//LeaveCriticalSection(&m_pCSLockVB);

		return FALSE;
	}

	if( bThread )	
	{
		//dwTick = timeGetTime() - dwTick;

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #1 %1%", dwTick ) );

		Sleep(0);

		dwTick = timeGetTime();
	}

	// Note : 저장 되었던 VB를 로드한다.
	if( bThread )
	{
		// 기본 데이터를 로드한다.
		DWORD dwSub = 0;
		DWORD dwAdd = 20;	// ??byte * 20 = ????byte
		//DWORD dwAdd = 60;	// ??byte * 60 = ????byte
		for( DWORD i=0; i<m_dwVert; )
		{
			dwSub = m_dwVert - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pTempVB+(m_nFVFSize*i), m_nFVFSize*dwSub );
			i += dwSub;
			
			{
				//dwTick = timeGetTime() - dwTick;

				//// Log에 남겨본다.
				//if ( dwTick > 10 )
				//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #2 %1%", dwTick ) );

				Sleep(0);

				dwTick = timeGetTime();
			}
		}
	}
	else
	{
		SFile.ReadBuffer( pTempVB, m_dwVert*m_nFVFSize );
	}

	// Note : 저장 되었던 IB를 로드한다.
	if( bThread )
	{
		DWORD dwSub = 0;
		//DWORD dwAdd = 40;	// 6byte * 40 = 240byte
		DWORD dwAdd = 160;	// 6byte * 160 = 960byte
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		for( DWORD i=0; i<m_dwFace; )
		{
			dwSub = m_dwFace - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pIndices+(i*3), sizeof(WORD)*dwSub*3 );
			i += dwSub;
			
			{
				//dwTick = timeGetTime() - dwTick;

				//// Log에 남겨본다.
				//if ( dwTick > 10 )
				//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #3 %1%", dwTick ) );

				Sleep(0);

				dwTick = timeGetTime();
			}
		}
		m_pIB->Unlock();
	}
	else
	{
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		SFile.ReadBuffer( pIndices, sizeof(WORD)*m_dwFace*3 );
		m_pIB->Unlock();
	}

	// Note : 
	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
		m_pColor = new DIRECTPOINTCOLOR[m_dwVert];
		SFile.ReadBuffer( m_pColor, sizeof(DIRECTPOINTCOLOR)*m_dwVert );
		break;
	};

	if( bThread )
	{
		//dwTick = timeGetTime() - dwTick;

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #4 %1%", dwTick ) );

		Sleep(0);

		dwTick = timeGetTime();
	}

	// Note : VertexBuffer 생성후 데이터 삽입.
	//EnterCriticalSection(&m_pCSLockVB);
	{
		BYTE* pVertices(NULL);
		m_pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
		memcpy( pVertices, pTempVB, m_dwVert*m_nFVFSize );
		m_pVB->Unlock();
	}
	//LeaveCriticalSection(&m_pCSLockVB);

	if( bThread )
	{
		//dwTick = timeGetTime() - dwTick;

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #5 %1%", dwTick ) );

		Sleep(0);

		dwTick = timeGetTime();
	}

	// Note : 
	SAFE_DELETE_ARRAY( m_pVertSrc );
	m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];
	memcpy( m_pVertSrc, pTempVB, sizeof(VERTEXNORCOLORTEX)*m_dwVert );

	if( bThread )
	{
		//dwTick = timeGetTime() - dwTick;

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #6 %1%", dwTick ) );

		Sleep(0);

		dwTick = timeGetTime();
	}

	// Note : 임시로 만든것을 지워줌.
	SAFE_DELETE_ARRAY( pTempVB );

	return TRUE;
}

BOOL OBJOCTree::OctreeLoad_10001( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread )
{
	DWORD dwTemp(0L);
	DWORD dwBaseFVF(0L);
	SFile >> dwBaseFVF;
	SFile >> m_dwVert;
	SFile >> m_dwFace;
	SFile >> dwTemp;
	m_emLandType = (EM_LAND_TYPE)dwTemp;

	DWORD dwSaveFVFSize = D3DXGetFVFVertexSize( dwBaseFVF );
	DWORD dwSaveFVFSize_Except_UV = D3DXGetFVFVertexSize( dwBaseFVF & ~( D3DFVF_TEXCOUNT_MASK | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_LASTBETA_D3DCOLOR ) );

	m_dwFVF = VERTEXNORCOLORTEX::FVF;
	m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );

	DWORD dwTick(0L);
	if( bThread )	
	{
		Sleep(0);

		dwTick = timeGetTime();
	}

	// Note : 
	BYTE*	pTempVB(NULL);
	pTempVB = new BYTE[m_dwVert*dwSaveFVFSize];

	//EnterCriticalSection(&m_pCSLockVB);
	{		
		SAFE_RELEASE( m_pVB );
		pd3dDevice->CreateVertexBuffer( m_dwVert*m_nFVFSize, D3DUSAGE_DYNAMIC, m_dwFVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
		if( !m_pVB )
		{
			// Note : 임시로 만든것을 지워줌.
			SAFE_DELETE_ARRAY( pTempVB );

			//LeaveCriticalSection(&m_pCSLockVB);
			return FALSE;
		}
	}
	//LeaveCriticalSection(&m_pCSLockVB);

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( !m_pIB )
	{
		// Note : 임시로 만든것을 지워줌.
		SAFE_DELETE_ARRAY( pTempVB );

		//EnterCriticalSection(&m_pCSLockVB);
		SAFE_RELEASE( m_pVB );
		//LeaveCriticalSection(&m_pCSLockVB);

		return FALSE;
	}

	if( bThread )	
	{
		//dwTick = timeGetTime() - dwTick;

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #1 %1%", dwTick ) );

		Sleep(0);

		dwTick = timeGetTime();
	}

	// Note : 저장 되었던 VB를 로드한다.
	if( bThread )
	{
		// 기본 데이터를 로드한다.
		DWORD dwSub = 0;
		DWORD dwAdd = 20;	// ??byte * 20 = ????byte
		//DWORD dwAdd = 60;	// ??byte * 60 = ????byte
		for( DWORD i=0; i<m_dwVert; )
		{
			dwSub = m_dwVert - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pTempVB+(dwSaveFVFSize*i), dwSaveFVFSize*dwSub );
			i += dwSub;

			{
				//dwTick = timeGetTime() - dwTick;

				//// Log에 남겨본다.
				//if ( dwTick > 10 )
				//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #2 %1%", dwTick ) );

				Sleep(0);

				dwTick = timeGetTime();
			}
		}
	}
	else
	{
		SFile.ReadBuffer( pTempVB, m_dwVert*dwSaveFVFSize );
	}

	// Note : 저장 되었던 IB를 로드한다.
	if( bThread )
	{
		DWORD dwSub = 0;
		//DWORD dwAdd = 40;	// 6byte * 40 = 240byte
		DWORD dwAdd = 160;	// 6byte * 160 = 960byte
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		for( DWORD i=0; i<m_dwFace; )
		{
			dwSub = m_dwFace - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pIndices+(i*3), sizeof(WORD)*dwSub*3 );
			i += dwSub;

			{
				//dwTick = timeGetTime() - dwTick;

				//// Log에 남겨본다.
				//if ( dwTick > 10 )
				//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #3 %1%", dwTick ) );

				Sleep(0);

				dwTick = timeGetTime();
			}
		}
		m_pIB->Unlock();
	}
	else
	{
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		SFile.ReadBuffer( pIndices, sizeof(WORD)*m_dwFace*3 );
		m_pIB->Unlock();
	}

	// Note : 
	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
		m_pColor = new DIRECTPOINTCOLOR[m_dwVert];
		SFile.ReadBuffer( m_pColor, sizeof(DIRECTPOINTCOLOR)*m_dwVert );
		break;
	};

	if( bThread )
	{
		//dwTick = timeGetTime() - dwTick;

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #4 %1%", dwTick ) );

		Sleep(0);

		dwTick = timeGetTime();
	}

	if( bThread )
	{
		//dwTick = timeGetTime() - dwTick;

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #5 %1%", dwTick ) );

		Sleep(0);

		dwTick = timeGetTime();
	}

	// Note : 
	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
	case EMLR_DAYNIGHT_OFF:
		SAFE_DELETE_ARRAY( m_pVertSrc );
		m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];
		memcpy( m_pVertSrc, pTempVB, sizeof(VERTEXNORCOLORTEX)*m_dwVert );

		if( bThread )
		{
			//dwTick = timeGetTime() - dwTick;

			//// Log에 남겨본다.
			//if ( dwTick > 10 )
			//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #6 %1%", dwTick ) );

			Sleep(0);

			dwTick = timeGetTime();
		}

		break;
	case EMLR_PIECE:
		D3DXVECTOR3* pPos(NULL);
		D3DXVECTOR3* pNor(NULL);
		D3DXVECTOR2* pTex(NULL);
		SAFE_DELETE_ARRAY( m_pVertSrc );
		m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			pPos = (D3DXVECTOR3*)( pTempVB + i*dwSaveFVFSize );
			pNor = (D3DXVECTOR3*)( pTempVB + i*dwSaveFVFSize + 4*3 );						// 4바이트 * (위치)
			pTex = (D3DXVECTOR2*)( pTempVB + i*dwSaveFVFSize + dwSaveFVFSize_Except_UV );	// 4바이트 * 

			m_pVertSrc[i].vPos = *pPos;
			m_pVertSrc[i].vNor = *pNor;
			m_pVertSrc[i].dwColor = 0xffffffff;
			m_pVertSrc[i].vTex = *pTex;
		}

		if( bThread )
		{
			//dwTick = timeGetTime() - dwTick;

			//// Log에 남겨본다.
			//if ( dwTick > 10 )
			//	sc::writeLogInfo( sc::string::format("DxStaticMesh_THREAD::LoadThread ------------- #6 %1%", dwTick ) );

			Sleep(0);

			dwTick = timeGetTime();
		}

		break;
	};

	//EnterCriticalSection(&m_pCSLockVB);
	{
		BYTE* pVertices(NULL);
		m_pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
		memcpy( pVertices, m_pVertSrc, m_dwVert*m_nFVFSize );
		m_pVB->Unlock();
	}
	//LeaveCriticalSection(&m_pCSLockVB);

	// Note : 임시로 만든것을 지워줌.
	SAFE_DELETE_ARRAY( pTempVB );

	return TRUE;
}

BOOL OBJOCTree::OctreeLoad_10000( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread )
{
	DWORD dwBaseFVF;

	DWORD dwTemp(0L);
	SFile >> dwBaseFVF;
	SFile >> m_dwVert;
	SFile >> m_dwFace;
	SFile >> dwTemp;
	m_emLandType = (EM_LAND_TYPE)dwTemp;

	DWORD dwSaveFVFSize = D3DXGetFVFVertexSize( dwBaseFVF );
	DWORD dwSaveFVFSize_Except_UV = D3DXGetFVFVertexSize( dwBaseFVF & ~( D3DFVF_TEXCOUNT_MASK | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_LASTBETA_D3DCOLOR ) );
	DWORD dwFVF = (~D3DFVF_TEXCOUNT_MASK) & dwBaseFVF;

	//switch( m_emLandType )
	//{
	//case EMLR_DAYNIGHT_ON:
	//case EMLR_DAYNIGHT_OFF:
	//	m_dwFVF = VERTEXNORCOLORTEX::FVF;
	//	m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );
	//	break;

	//case EMLR_PIECE:
	//	m_dwFVF = VERTEXNORCOLORTEX::FVF;
	//	m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );
	//	break;
	//};

	m_dwFVF = VERTEXNORCOLORTEX::FVF;
	m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );
	

	if( bThread )	Sleep(0);

	// Note : 
	BYTE*	pTempVB(NULL);
	pTempVB = new BYTE[m_dwVert*dwSaveFVFSize];

	//EnterCriticalSection(&m_pCSLockVB);
	{		
		SAFE_RELEASE( m_pVB );
		pd3dDevice->CreateVertexBuffer( m_dwVert*m_nFVFSize, D3DUSAGE_DYNAMIC, m_dwFVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
		if( !m_pVB )
		{
			// Note : 임시로 만든것을 지워줌.
			SAFE_DELETE_ARRAY( pTempVB );

			//LeaveCriticalSection(&m_pCSLockVB);
			return FALSE;
		}
	}
	//LeaveCriticalSection(&m_pCSLockVB);

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( !m_pIB )
	{
		// Note : 임시로 만든것을 지워줌.
		SAFE_DELETE_ARRAY( pTempVB );

		//EnterCriticalSection(&m_pCSLockVB);
		SAFE_RELEASE( m_pVB );
		//LeaveCriticalSection(&m_pCSLockVB);

		return FALSE;
	}

	if( bThread )	Sleep(0);

	// Note : 저장 되었던 VB를 로드한다.
	if( bThread )
	{
		// 기본 데이터를 로드한다.
		DWORD dwSub = 0;
		DWORD dwAdd = 60;	// ??byte * 60 = ????byte
		for( DWORD i=0; i<m_dwVert; )
		{
			dwSub = m_dwVert - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pTempVB+(dwSaveFVFSize*i), dwSaveFVFSize*dwSub );
			i += dwSub;
			Sleep(0);
		}
	}
	else
	{
		SFile.ReadBuffer( pTempVB, m_dwVert*dwSaveFVFSize );
	}

	// Note : 저장 되었던 IB를 로드한다.
	if( bThread )
	{
		DWORD dwSub = 0;
		DWORD dwAdd = 160;	// 12byte * 160 = 1920byte
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		for( DWORD i=0; i<m_dwFace; )
		{
			dwSub = m_dwFace - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pIndices+(i*3), sizeof(WORD)*dwSub*3 );
			i += dwSub;
			Sleep(0);
		}
		m_pIB->Unlock();
	}
	else
	{
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		SFile.ReadBuffer( pIndices, sizeof(WORD)*m_dwFace*3 );
		m_pIB->Unlock();
	}

	// Note : 
	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
		m_pColor = new DIRECTPOINTCOLOR[m_dwVert];
		SFile.ReadBuffer( m_pColor, sizeof(DIRECTPOINTCOLOR)*m_dwVert );
		break;
	};

	Sleep(0);

	// Note : 
	D3DXVECTOR3* pPos(NULL);
	D3DXVECTOR3* pNor(NULL);
	D3DXVECTOR2* pTex(NULL);
	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
	case EMLR_DAYNIGHT_OFF:
		SAFE_DELETE_ARRAY( m_pVertSrc );
		m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			pPos = (D3DXVECTOR3*)( pTempVB + i*dwSaveFVFSize );
			pTex = (D3DXVECTOR2*)( pTempVB + i*dwSaveFVFSize + dwSaveFVFSize_Except_UV );	// 4바이트 * (위치+디퓨즈)

			m_pVertSrc[i].vPos = *pPos;
			m_pVertSrc[i].vNor = D3DXVECTOR3(0.f,1.f,0.f);
			m_pVertSrc[i].dwColor = 0xffffffff;
			m_pVertSrc[i].vTex = *pTex;
		}

		//memcpy( m_pVertSrc, pTempVB, sizeof(VERTEXNORCOLORTEX)*m_dwVert );

		Sleep(0);

		break;


	case EMLR_PIECE:
		SAFE_DELETE_ARRAY( m_pVertSrc );
		m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			pPos = (D3DXVECTOR3*)( pTempVB + i*dwSaveFVFSize );
			pNor = (D3DXVECTOR3*)( pTempVB + i*dwSaveFVFSize + 4*3 );	// 4바이트 * (위치)
			pTex = (D3DXVECTOR2*)( pTempVB + i*dwSaveFVFSize + dwSaveFVFSize_Except_UV );	// 4바이트 * (위치+노멀)

			m_pVertSrc[i].vPos = *pPos;
			m_pVertSrc[i].vNor = *pNor;
			m_pVertSrc[i].dwColor = 0xffffffff;
			m_pVertSrc[i].vTex = *pTex;
		}

		Sleep(0);

		break;
	};

	//EnterCriticalSection(&m_pCSLockVB);
	{
		BYTE* pVertices(NULL);
		m_pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
		memcpy( pVertices, m_pVertSrc, m_dwVert*m_nFVFSize );
		m_pVB->Unlock();
	}
	//LeaveCriticalSection(&m_pCSLockVB);

	//switch( m_emLandType )
	//{
	//case EMLR_DAYNIGHT_ON:
	//case EMLR_DAYNIGHT_OFF:
	//	// Note : VertexBuffer 생성후 데이터 삽입.
	//	//EnterCriticalSection(&m_pCSLockVB);
	//	{
	//		BYTE* pVertices(NULL);
	//		m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
	//		memcpy( pVertices, m_pVertSrc, m_dwVert*m_nFVFSize );
	//		m_pVB->Unlock();
	//	}
	//	//LeaveCriticalSection(&m_pCSLockVB);
	//	break;

	//case EMLR_PIECE:
	//	// Note : VertexBuffer 생성후 데이터 삽입.
	//	//EnterCriticalSection(&m_pCSLockVB);
	//	{
	//		BYTE* pVertices(NULL);
	//		m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
	//		memcpy( pVertices, pTempVB, m_dwVert*m_nFVFSize );
	//		m_pVB->Unlock();
	//	}
	//	//LeaveCriticalSection(&m_pCSLockVB);
	//	break;
	//};
	//

	Sleep(0);

	// Note : 임시로 만든것을 지워줌.
	SAFE_DELETE_ARRAY( pTempVB );

	return TRUE;
}

BOOL OBJOCTree::OctreeLoadOLD( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DWORD dwFVF, const BOOL bThread )
{
	HRESULT hr(S_OK);

	SFile >> m_dwVert;
	SFile >> m_dwFace;

	UINT nSIZE = D3DXGetFVFVertexSize( dwFVF );	// dwFVF은 정해지지 않는 것이 들어 오기 대문에 지역 변수로 넣었다.

	if( bThread )	Sleep(0);

	// Note : 로딩 되어있는 데이터 형.
	BYTE*	pTempVB(NULL);
	pTempVB = new BYTE[m_dwVert*nSIZE];

	// Note : 기본적인 VB, IB, DIRECTPOINTCOLOR, 등을 생성한다.
	CreateBASE( pd3dDevice, m_dwVert, m_dwFace );
	if( !m_pVB )	return FALSE;
	if( !m_pIB )	return FALSE;

	if( bThread )	Sleep(0);

	// Note : 저장 되었던 VB를 로드한다.
	if( bThread )
	{
		// 기본 데이터를 로드한다.
		DWORD dwSub = 0;
		DWORD dwAdd = 60;	// 32byte * 60 = 1920byte
		for( DWORD i=0; i<m_dwVert; )
		{
			dwSub = m_dwVert - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pTempVB+(nSIZE*i), nSIZE*dwSub );
			i += dwSub;
			Sleep(0);
		}
	}
	else
	{
		SFile.ReadBuffer( pTempVB, m_dwVert*nSIZE );
	}

	// Note : 저장 되었던 IB를 로드한다.
	if( bThread )
	{
		DWORD dwSub = 0;
		DWORD dwAdd = 160;	// 12byte * 160 = 1920byte
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		for( DWORD i=0; i<m_dwFace; )
		{
			dwSub = m_dwFace - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pIndices+(i*3), sizeof(WORD)*dwSub*3 );
			i += dwSub;
			Sleep(0);
		}
		m_pIB->Unlock();
	}
	else
	{
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		SFile.ReadBuffer( pIndices, sizeof(WORD)*m_dwFace*3 );
		m_pIB->Unlock();
	}

	// Note : VertSrc를 생성한다.
	CreateVertSrc( pTempVB, dwFVF, m_dwVert );

	// Note : Normal과 컬러를 가져온다.
	CreateNormalColor( pTempVB, dwFVF, m_dwVert );

	if( bThread )	Sleep(0);
	
	// Note : 렌더에 쓰일 VB를 셋팅한다.
	CopyVertSrcToVB( pTempVB, m_pVertSrc, m_pVB, m_dwVert, dwFVF );

	// Note : 임시로 만든것을 지워줌.
	SAFE_DELETE_ARRAY( pTempVB );

	return TRUE;
}

void OBJOCTree::Load_VER100 ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	HRESULT hr(S_OK);
	CleanUp();

	// Note : 이때는 무조건 Piece가 아니었다. Object의 타입을 설정한다.
	if( DxLightMan::GetInstance()->IsNightAndDay() )	m_emLandType = EMLR_DAYNIGHT_ON;
	else												m_emLandType = EMLR_DAYNIGHT_OFF;

	SFile >> m_vMax;
	SFile >> m_vMin;

	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		DWORD dwFVF;
		SFile >> dwFVF;
		SFile >> m_dwVert;
		SFile >> m_dwFace;

		UINT nSIZE = D3DXGetFVFVertexSize ( dwFVF );

		// Note : 로딩 되어있는 데이터 형.
		BYTE*	pTempVB(NULL);
		pTempVB = new BYTE[m_dwVert*nSIZE];

		// Note : 기본적인 VB, IB, DIRECTPOINTCOLOR, 등을 생성한다.
		CreateBASE( pd3dDevice, m_dwVert, m_dwFace );

		// Note : 저장 되었던 VB를 로드한다.
		SFile.ReadBuffer( pTempVB, m_dwVert*nSIZE );

		// Note : 저장 되었던 IB를 로드한다.
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		SFile.ReadBuffer( pIndices, sizeof(WORD)*m_dwFace*3 );
		m_pIB->Unlock();

		// Note : VertSrc를 생성한다.
		CreateVertSrc( pTempVB, dwFVF, m_dwVert );

		// Note : Normal과 컬러를 가져온다.
		CreateNormalColor( pTempVB, dwFVF, m_dwVert );
			
		// Note : 렌더에 쓰일 VB를 셋팅한다.
		CopyVertSrcToVB( pTempVB, m_pVertSrc, m_pVB, m_dwVert, dwFVF );

		SAFE_DELETE_ARRAY( pTempVB );
	}

	if ( m_pVB && m_pIB )
	{
		NSOCTREE::MakeCollisionData_VBIB( m_pVB, m_pIB, m_dwFVF, m_dwFace, &m_pCollsionTREE );
	}

	m_bLoad = TRUE;		// 로딩 완료
}