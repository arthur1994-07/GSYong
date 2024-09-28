#include "pch.h"

#include <algorithm>

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Util.h"

#include "../../DxEffect/Mesh/DxStaticMesh.h"
#include "../../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxTools/Material/NSShadowRenderForMaterial.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../DxLand/LightMap/NSLightMap.h"
#include "../../DxLand/DxLandMan.h"
#include "../../Common/StlFunctions.h"
#include "../../Common/profile.h"
#include "../../DxCommon9/DXUTmisc.h"
#include "../../GlobalParam.h"

#include "../PieceInstancingThread.h"

#include "./NSPieceInstancing.h"
#include "./NSPieceQuickSort.h"
#include "./DxPieceManagerPSF.h"
#include "./DxPiececontainer.h"
#include "./DxPieceEdit.h"
#include "./NSPIECETREE.h"
#include "./DxPieceNode.h"
#include "./DxPiece.h"

#include "DxPieceAABB.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//											D	x		P	i	e	c	e		A	A	B	B
// -----------------------------------------------------------------------------------------------------------------------------------------
DxPieceAABB::DxPieceAABB()
	: m_pOldMapAndCollisionPieceTree(NULL)
	, m_pAllPieceTree(NULL)
	, m_nFrameMoveStep(-1)
{
}

DxPieceAABB::~DxPieceAABB()
{
	CleanUp();
}

void DxPieceAABB::CleanUp()
{
	m_nFrameMoveStep = -1;

	std::for_each( m_vecPiece.begin(), m_vecPiece.end(), std_afunc::DeleteObject() );
	m_vecPiece.clear();

	SAFE_DELETE( m_pOldMapAndCollisionPieceTree );
	SAFE_DELETE( m_pAllPieceTree );

	// 순수한 참조값이다.
	//m_vecPieceForFrameAniHardAlpha.clear();
	//m_vecPieceForReplace.clear();
}

void DxPieceAABB::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
}

void DxPieceAABB::OnLostDevice()
{
}

void DxPieceAABB::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = m_vecPiece[i];
		pCur->CreateQuery( pd3dDevice );
	}
}

void DxPieceAABB::DestroyQuery()
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = m_vecPiece[i];
		pCur->DestroyQuery();
	}
}

void DxPieceAABB::SetPiece_NULL()
{
	m_vecPiece.clear();
}

void DxPieceAABB::SetPiece( DxPiece* pPiece )
{
	m_vecPiece.push_back( pPiece );
}

void DxPieceAABB::AddPiece( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pSrc )
{
	DxPiece* pNew = new DxPiece(pd3dDevice);
	pNew->CloneData ( pd3dDevice, pSrc );
	m_vecPiece.push_back( pNew );
}

void DxPieceAABB::AddPieceAABB_DeleteSrc( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceAABB* pSrc )
{
	for( DWORD i=0; i<pSrc->m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = pSrc->m_vecPiece[i];
		m_vecPiece.push_back( pCur );
	}

	pSrc->m_vecPiece.clear();
}

//void DxPieceAABB::AddPieceAABB( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceAABB* pSrc )
//{
//	TSTRING strName;
//	float fLength(0.f);
//	DWORD dwTexSize(0);
//	AddPieceAABB ( pd3dDevice, pSrc, FALSE, strName, NULL, NULL, dwTexSize, fLength );
//}
//
//void DxPieceAABB::AddPieceAABB( LPDIRECT3DDEVICEQ pd3dDevice, 
//								  DxPieceAABB* pSrc, 
//								  BOOL bLightMap, 
//								  const TSTRING& strLightMapName, 
//								  DxStaticMesh* pStaticMesh, 
//								  DxPieceManagerPSF* pPieceManager,
//								  DWORD& dwStartTextureSize,
//								  float fLightMapDetail )
//{
//	for( DWORD i=0; i<pSrc->m_vecPiece.size(); ++i )
//	{
//		DxPiece* pCur = pSrc->m_vecPiece[i];
//
//		DxPiece* pNew = new DxPiece;
//		pNew->CloneData( pd3dDevice, pCur, bLightMap, strLightMapName, pStaticMesh, pPieceManager, dwStartTextureSize, fLightMapDetail );
//
//		m_vecPiece.push_back( pNew );
//	}
//}

void DxPieceAABB::FrameMove( const float fTime, const float fElapsedTime )
{
	for ( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->FrameMoveTOOL( fTime, fElapsedTime );
	}
}

//void DxPieceAABB::FrameMoveAll( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
//{
//	for ( DWORD i=0; i<m_vecPiece.size(); ++i )
//	{
//		m_vecPiece[i]->FrameMoveStep( pd3dDevice, bPSF );
//	}
//}

//void DxPieceAABB::FrameMoveStep( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
//{
//	if ( m_vecPiece.empty() )
//		return;
//	
//	if ( m_nFrameMoveStep == -1 )
//	{
//		for ( DWORD i=0; i<m_vecPiece.size(); ++i )
//		{
//			m_vecPiece[i]->FrameMoveStep( pd3dDevice, bPSF );
//		}
//		++m_nFrameMoveStep;
//	}
//	else
//	{
//		if ( m_nFrameMoveStep >= static_cast<int>( m_vecPiece.size() ) )
//		{
//			m_nFrameMoveStep = 0;
//		}
//		
//		m_vecPiece[m_nFrameMoveStep++]->FrameMoveStep( pd3dDevice, bPSF );
//	}
//}

//// Instancing 가능한 Piece 인지 확인 후 불가능 하면 m_pAllPieceTree 으로 넘긴다.
//void DxPieceAABB::FrameMove_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
//{
//	if ( !m_pInstancingPieceTree )
//		return;
//
//	if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP || 
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT ||
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
//		return;
//
//	if ( RENDERPARAM::g_emGroundQulity <= TnL_PS_1_1 )
//	{
//		return;
//	}
//
//	// 만약 SoftAlpha Texture 라면 Instancing 에서 제외하도록 한다.
//	VEC_PIECE vecInstancingExceptPiece;
//	NSPIECETREE::FrameMove_Instancing( m_pInstancingPieceTree, pd3dDevice, fElapsedTime, vecInstancingExceptPiece );
//
//	// m_pAllPieceTree 를 새로 만든다.
//	if ( !vecInstancingExceptPiece.empty() )
//	{
//		// vecInstancingExceptPiece 으로 정보를 옮긴다.
//		if ( m_pAllPieceTree )
//		{
//			NSPIECETREE::GetVecPiece( m_pAllPieceTree, vecInstancingExceptPiece );
//		}
//
//		// 제외된걸 찾은 후 넣어야 한다.
//		NSPIECETREE::MakeTree( m_pAllPieceTree, vecInstancingExceptPiece, 10.f );
//	}
//}


//////////////////////////////////////////////////////////////////////////
// 전체 다 호출된다.
void DxPieceAABB::InsertQuickSort( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend, BOOL bReflect )
{
	if ( !m_pOldMapAndCollisionPieceTree || !m_pAllPieceTree )
		return;

	PROFILE_BEGIN2("DxPieceAABB::Render");

	D3DXVECTOR3 vDir = vFromPt - vLookatPt;
	float fLength = D3DXVec3Length( &vDir );

	// Note : Quick Sort 를 한다.		이넘을 저기서도 쓰려면... 데이터를 공유하면 안될 듯 하네.
	//			Sort 정보만 
	if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP || 
		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT ||
		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
	{
		NSPIECETREE::InsertQuickSortALL( m_pAllPieceTree, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
	}
	else if ( bReflect )
	{
		// 반사 - LowOption 되어있는 건 렌더링에서 제외한다.
		NSPIECETREE::InsertQuickSortALL( m_pOldMapAndCollisionPieceTree, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
	}
	else if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
	{
		// TnL_PS_3_0_DF 이상 버젼은 Instancing 이 동작되기 때문에, Instancing 에 포함 안되는 것들을 Insert 하도록 한다.
		NSPIECETREE::InsertQuickSortALL( m_pAllPieceTree, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
	}
	else
	{
		NSPIECETREE::InsertQuickSortALL( m_pOldMapAndCollisionPieceTree, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
	}

	PROFILE_END2("DxPieceAABB::Render");
}

void DxPieceAABB::InsertQuickSortForShadow( const CLIPVOLUME &sCV )
{
	if ( !m_pAllPieceTree )
		return;

	NSPIECETREE::InsertQuickSortForShadow( m_pAllPieceTree, sCV );
}

void DxPieceAABB::InsertQuickSortPSF( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend )
{
	if ( !m_pAllPieceTree || !m_pOldMapAndCollisionPieceTree )
		return;

	PROFILE_BEGIN2("DxPieceAABB::Render");

	D3DXVECTOR3 vDir = vFromPt - vLookatPt;
	float fLength = D3DXVec3Length( &vDir );

	// Note : Quick Sort 를 한다.		이넘을 저기서도 쓰려면... 데이터를 공유하면 안될 듯 하네.
	//			Sort 정보만 
	//
	// RENDERPARAM::g_emTnLMode 를 쓰는 건 PSF 상에서 확인되야 하기 때문이다.
	// PSF 상 RENDERPARAM::g_emGroundQulity 값은 최고값이 셋팅된다.
	if ( RENDERPARAM::g_emTnLMode >= TnL_PS_3_0_DF )
	{
		NSPIECETREE::InsertQuickSortALL( m_pAllPieceTree, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
	}
	else
	{
		NSPIECETREE::InsertQuickSortALL( m_pOldMapAndCollisionPieceTree, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
	}
	

	PROFILE_END2("DxPieceAABB::Render");
}
// 전체 다 호출된다.
//////////////////////////////////////////////////////////////////////////



void DxPieceAABB::RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	//if ( !m_pInstancingPieceTree )
	//	return;



	////////////////////////////////////////////////////////////////////////////
	////					Instancing 초기화 작업
	//spPieceInstancing->Reset();



	//m_vecPieceForFrameAniHardAlpha.clear();
	//m_vecPieceForReplace.clear();

	//// Opaque
	//{
	//	pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_ONLYPOS );
	//	if ( NSShadowRenderForMaterial::FX_BeginPass( 0 ) )
	//	{
	//		NSPIECETREE::RenderCascadeShadowOpaque( m_pInstancingPieceTree, pd3dDevice, sCV, m_vecPieceForFrameAniHardAlpha, m_vecPieceForReplace, spPieceInstancing );

	//		spPieceInstancing->InsertDataToVB();

	//		spPieceInstancing->RenderCascadeShadowOpaque( pd3dDevice );
	//		NSShadowRenderForMaterial::FX_EndPass();
	//	}

	//	pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_ONLYPOS_INSTANCING );
	//	if ( NSShadowRenderForMaterial::FX_BeginPass( 2 ) )
	//	{
	//		spPieceInstancing->RenderCascadeShadowOpaque_Instancing( pd3dDevice );
	//		NSShadowRenderForMaterial::FX_EndPass();
	//	}
	//}

	//{
	//	DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );

	//	pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_POSTEX );
	//	if ( NSShadowRenderForMaterial::FX_BeginPass( 1 ) )
	//	{
	//		spPieceInstancing->RenderCascadeShadowHardAlpha( pd3dDevice );

	//		for ( DWORD i=0; i<m_vecPieceForFrameAniHardAlpha.size(); ++i )
	//		{
	//			m_vecPieceForFrameAniHardAlpha[i]->RenderCascadeShadowHardAlpha_Animation();
	//		}

	//		NSShadowRenderForMaterial::FX_EndPass();
	//	}

	//	pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_POSTEX_INSTANCING );
	//	if ( NSShadowRenderForMaterial::FX_BeginPass( 3 ) )
	//	{
	//		spPieceInstancing->RenderCascadeShadowHardAlpha_Instancing( pd3dDevice );

	//		NSShadowRenderForMaterial::FX_EndPass();
	//	}

	//	DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );	
	//}

	//if ( !m_vecPieceForReplace.empty() )
	//{
	//	// Replace
	//	pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_POSTEX );
	//	if ( NSShadowRenderForMaterial::FX_BeginPass( 1 ) )
	//	{
	//		for ( DWORD i=0; i<m_vecPieceForReplace.size(); ++i )
	//		{
	//			m_vecPieceForReplace[i]->RenderCascadeShadowReplace();
	//		}

	//		NSShadowRenderForMaterial::FX_EndPass();
	//	}
	//}
}
//
//void DxPieceAABB::RenderOpaque_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
//{
//	if ( !m_pInstancingPieceTree )
//		return;
//
//	if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP || 
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT ||
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
//		return;
//
//	NSPIECETREE::RenderOpaque_Instancing( m_pInstancingPieceTree, pd3dDevice, sCV );
//
//	pd3dDevice->SetStreamSourceFreq( 0, 1 );
//	pd3dDevice->SetStreamSourceFreq( 1, 1 );
//}
//
//void DxPieceAABB::RenderHardAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
//{
//	if ( !m_pInstancingPieceTree )
//		return;
//
//	if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP || 
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT ||
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
//		return;
//
//	DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
//
//	NSPIECETREE::RenderHardAlpha_Instancing( m_pInstancingPieceTree, pd3dDevice, sCV );
//
//	DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
//
//	pd3dDevice->SetStreamSourceFreq( 0, 1 );
//	pd3dDevice->SetStreamSourceFreq( 1, 1 );
//}
//
//void DxPieceAABB::RenderEffFrame_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bLBuffer )
//{
//	if ( !m_pInstancingPieceTree )
//		return;
//
//	if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP || 
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT ||
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
//		return;
//
//	NSPIECETREE::RenderEffFrame_Instancing( m_pInstancingPieceTree, pd3dDevice, sCV, bLBuffer );
//}
//
//void DxPieceAABB::RenderDecal_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
//{
//	if ( !m_pInstancingPieceTree )
//		return;
//
//	if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP || 
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT ||
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
//		return;
//
//	NSPIECETREE::RenderDecal_Instancing( m_pInstancingPieceTree, pd3dDevice, sCV );
//
//	pd3dDevice->SetStreamSourceFreq( 0, 1 );
//	pd3dDevice->SetStreamSourceFreq( 1, 1 );
//}
//
//void DxPieceAABB::RenderDecalPass2_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
//{
//	if ( !m_pInstancingPieceTree )
//		return;
//
//	if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP || 
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT ||
//		NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
//		return;
//
//	NSPIECETREE::RenderDecalPass2_Instancing( m_pInstancingPieceTree, pd3dDevice, sCV );
//
//	pd3dDevice->SetStreamSourceFreq( 0, 1 );
//	pd3dDevice->SetStreamSourceFreq( 1, 1 );
//}
//
//void DxPieceAABB::RenderWaterCustomSoftAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
//{
//	if ( !m_pInstancingPieceTree )
//		return;
//
//	NSPIECETREE::RenderWaterCustomSoftAlpha_Instancing( m_pInstancingPieceTree, pd3dDevice, sCV );
//}
//
//void DxPieceAABB::RenderEff_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxStaticMeshLightMap* pStaticMeshLightMap )
//{
//	if ( !m_pInstancingPieceTree )
//		return;
//
//	NSPIECETREE::RenderEff_Instancing( m_pInstancingPieceTree, pd3dDevice, sCV, pStaticMeshLightMap );
//}

void DxPieceAABB::ResetOcclusionQuery()
{
	for ( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->ResetOcclusionQuery();
	}
}

void DxPieceAABB::FindOcclusionRenderPieceIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecPieceIndex )
{
	for ( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		// 성공할때까지 돈다.
		DWORD dwCullPixels(0);
		if ( m_vecPiece[i]->VisibleOcclusionQueryTest( pd3dDevice, dwCullPixels ) )
		{
			if ( dwCullPixels != 0 )
			{
				vecPieceIndex.push_back( i );
			}
		}
	}
}

//void DxPieceAABB::MakeTree()
//{
//	// Note : 현재 조각들의 AABB Box를 구한다.
//	DxPiece* pCur = m_pPieceHead;
//	while( pCur )
//	{
//		pCur->SetAABBBox();
//		pCur = pCur->m_pNext;
//	}
//
//	// Note : 현재 조각파일의 전체 갯수
//	DWORD dwCount = 0;
//	pCur = m_pPieceHead;
//	while( pCur )
//	{
//		++dwCount;
//		pCur = pCur->m_pNext;
//	}
//
//	m_dwPieceNUM = dwCount;
//
//	// Note : 전체 조각들이 모두 사용 할수 있도록 최대 배열을 잡을수 있게 해 준다.
//	m_pQuickSort->SetArrayMaxSize( dwCount );
//
//	// Note : Tree 를 만든다.
//	SAFE_DELETE( m_pPieceAABBTree );
//	NSPIECETREE::MakeTree( m_pPieceAABBTree, m_pPieceHead );
//}

void DxPieceAABB::MakeTree()
{
	if ( m_vecPiece.empty() )
		return;

	// Note : 현재 조각들의 AABB Box를 구한다.
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = m_vecPiece[i];
		pCur->SetAABBBox();
	}

	// Note : 전체 조각들이 모두 사용 할수 있도록 최대 배열을 잡을수 있게 해 준다.
	NSPieceQuickSort::SetArrayMaxSize( (DWORD) m_vecPiece.size() );

	// 다 만듬.
	NSPIECETREE::MakeTree( m_pAllPieceTree, m_vecPiece, 10.f );

	// Note : Tree 를 만든다. 작은 사이즈의 Tree. 각각의 Piece 개인 플레이.
	{
		// 저사양을 위해 필수적으로 렌더링되야 하는 것 들만 얻어오도록 한다.
		// 충돌처리도 이걸 바탕으로 한다.
		VEC_PIECE vecInstancingExceptPiece;
		for ( DWORD i=0; i<m_vecPiece.size(); ++i )
		{
			if ( !m_vecPiece[i]->IsExceptLowOption() )
				vecInstancingExceptPiece.push_back( m_vecPiece[i] );
		}
		NSPIECETREE::MakeTree( m_pOldMapAndCollisionPieceTree, vecInstancingExceptPiece, 10.f );
	}
}
//
//void DxPieceAABB::MakeTree( BOOL bWLD, 
//						   float fInstancingCellSize, 
//						   BOOL bInstancing, 
//						   boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>& spPieceInstancingBase,
//						   boost::shared_ptr<PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>>& spPieceInstancingReflection )
//{
//	MakeTree( bWLD );
//
//	//SAFE_DELETE ( m_pInstancingPieceTree );
//	SAFE_DELETE ( m_pAllPieceTree );
//	//if ( bInstancing )
//	//{
//	//	// Instancing 용 Tree 를 만든다. 큰사이즈의 Tree. Piece 들이 다수가 들어가서 Instancing 그룹을 이룰수 있게 한다.
//	//	NSPIECETREE::MakeTree( m_pInstancingPieceTree, m_vecPiece, fInstancingCellSize );
//
//	//	// 
//	//	{
//	//		// m_pInstancingPieceTree 에서 적응 못하는 것을 찾도록 한다. & m_mapPieceInstancing 셋팅.
//	//		VEC_PIECE vecInstancingExceptPiece;
//	//		NSPIECETREE::InitInstancing( m_pInstancingPieceTree, vecInstancingExceptPiece );
//
//	//		// 제외된걸 찾은 후 넣어야 한다.
//	//		NSPIECETREE::MakeTree( m_pAllPieceTree, vecInstancingExceptPiece, 10.f );
//	//	}
//
//	//	//spPieceInstancingBase->SetInstancingPieceTree( m_pInstancingPieceTree );
//	//	//spPieceInstancingReflection->SetInstancingPieceTree( m_pInstancingPieceTree );
//	//}
//}

void DxPieceAABB::MakeTree( const D3DXMATRIX& matWorld )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = m_vecPiece[i];
		pCur->SetPrefabMatrix( matWorld );
	}

	MakeTree();
}

void DxPieceAABB::InitInstancing( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//// 그림자용 Instancing이다.
	//{
	//	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	//	{
	//		DxPiece* pCur = m_vecPiece[i];
	//		pCur->InitInstancing( pd3dDevice );
	//	}

	//	spPieceInstancing->CreateVB( pd3dDevice );
	//}

	//// 일반적인 Instancing 이다.
	//NSPIECETREE::CreateVB_ForInstancing( m_pInstancingPieceTree, pd3dDevice );
}

void DxPieceAABB::ResetPieceParameter()
{
	// DxPiece::PieceParameter 정보를 재 셋팅해준다. -1이면 다 재 셋팅한다.
	m_nFrameMoveStep = -1;
}

void DxPieceAABB::CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, BOOL bRan_1_0, float fCellSize )
{
	for ( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->CreateLightMapPiece( pd3dDevice, strLightMapName );
	}

	// 공유되는것을 작업하기 때문에 하나만 건들도록 한다.
	if ( m_vecPiece.size() >= 1 )
	{
		m_vecPiece[0]->CreateLightMapPieceObject( pd3dDevice, strLightMapName, bRan_1_0, fCellSize );
	}
}

void DxPieceAABB::LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 텍스쳐 로딩을 했으니, DxPiece::PieceParameter 정보를 재 셋팅해준다. -1이면 다 재 셋팅한다.
	m_nFrameMoveStep = -1;

	for ( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->LoadLightMapForRAD( pd3dDevice );
	}
}

void DxPieceAABB::ReleaseLightMap()
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = m_vecPiece[i];
		pCur->ReleaseLightMap();
	}
}

void DxPieceAABB::ResetAnimationTime_DisableLoopType()
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = m_vecPiece[i];
		pCur->ResetAnimationTime_DisableLoopType();
	}
}

void DxPieceAABB::CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = m_vecPiece[i];
		pCur->CreateAlphaTexture32Bit( pd3dDevice );
	}
}
//
//BOOL DxPieceAABB::IsUserLightMapUV()	// User가 만든 LightMapUV를 사용하는가~?
//{
//	DxPiece* pCur = m_pPieceHead;
//	if ( pCur )
//	{
//		if ( !(pCur->m_dwFlags & EMPO_UNSUPPORT_RAD) )
//			return TRUE;
//	}
//
//	return FALSE;
//}

BOOL DxPieceAABB::Clone_MouseShift( LPD3DXMATRIX& pMatrix )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = m_vecPiece[i];
		DWORD_PTR dwBuffer = (DWORD_PTR) &pCur->m_matWorld;
		if( dwBuffer == (DWORD_PTR) pMatrix )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void DxPieceAABB::Save( sc::SerialFile& SFile )
{
    SFile << static_cast<DWORD>( VERSION );

    SFile.BeginBlock();
    {
		SFile << (DWORD)m_vecPiece.size();

		for( DWORD i=0; i<m_vecPiece.size(); ++i )
		{
			m_vecPiece[i]->Save( SFile );
		}

		// LightMap 정보를 저장하는 구간
		{
			std::map<TSTRING,DWORD>	mapPieceName;
			for( DWORD i=0; i<m_vecPiece.size(); ++i )
			{
				mapPieceName.insert ( std::make_pair( m_vecPiece[i]->m_strName, i ) );
			}

			SFile << (DWORD)mapPieceName.size();

			std::map<TSTRING,DWORD>::iterator iter = mapPieceName.begin();
			for( ; iter!=mapPieceName.end(); ++iter )
			{
				SFile << (*iter).second;
			}
		}
    }
    SFile.EndBlock();
}

void DxPieceAABB::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bWLD )
{
	SAFE_DELETE( m_pOldMapAndCollisionPieceTree );
	//SAFE_DELETE( m_pInstancingPieceTree );
	SAFE_DELETE( m_pAllPieceTree );

    DWORD dwVer, dwBufferSize;
    SFile >> dwVer;
    SFile >> dwBufferSize;

	if ( dwVer == 0x102 )
	{
		DWORD dwPieceNUM(0L);
		SFile >> dwPieceNUM;

		// Note : 전체 조각들이 모두 사용 할수 있도록 최대 배열을 잡을수 있게 해 준다.
		NSPieceQuickSort::SetArrayMaxSize( dwPieceNUM );											// 음 문제.. ~!!

		m_vecPiece.reserve( dwPieceNUM );
		for( DWORD i=0; i<dwPieceNUM; ++i )
		{
			DxPiece* pNew = new DxPiece(pd3dDevice);
			pNew->Load( pd3dDevice, SFile, i, bWLD );
			m_vecPiece.push_back( pNew );
		}

		// LightMap 정보를 로드하는 구간
		{
			DWORD dwPieceEditNUM(0L);
			SFile >> dwPieceEditNUM;

			TSTRING strName;
			DWORD dwID;
			for( DWORD i=0; i<dwPieceEditNUM; ++i )
			{
				SFile >> dwID;

				if ( dwID < m_vecPiece.size() )
				{
					// LightMap UV 를 디폴트 값에서 현재 맵의 LightMap Texture 에 맞춘 Size 로 변경함
					m_vecPiece[dwID]->SetLightMapUV_Rate();
				}
			}
		}
	}
	else if ( dwVer == 0x101 )
	{
		DWORD dwPieceNUM(0L);
		SFile >> dwPieceNUM;

		// Note : 전체 조각들이 모두 사용 할수 있도록 최대 배열을 잡을수 있게 해 준다.
		NSPieceQuickSort::SetArrayMaxSize( dwPieceNUM );											// 음 문제.. ~!!

		m_vecPiece.reserve( dwPieceNUM );
		for( DWORD i=0; i<dwPieceNUM; ++i )
		{
			DxPiece* pNew = new DxPiece(pd3dDevice);
			pNew->Load( pd3dDevice, SFile, i, bWLD );
			m_vecPiece.push_back( pNew );
		}

		// LightMap 정보를 로드하는 구간
		{
			DWORD dwPieceEditNUM(0L);
			SFile >> dwPieceEditNUM;

			TSTRING strName;
			DWORD dwID;
			for( DWORD i=0; i<dwPieceEditNUM; ++i )
			{
				SFile >> dwID;

				if ( dwID < m_vecPiece.size() )
				{
					// LightMap UV 를 디폴트 값에서 현재 맵의 LightMap Texture 에 맞춘 Size 로 변경함
					m_vecPiece[dwID]->SetLightMapUV_Rate();
				}
			}
		}

		NSPIECETREE::Load( pd3dDevice, m_pAllPieceTree, SFile, m_vecPiece );	// 참조값을 주기 위해 m_vecPiece을 넘겨줌.
	}
	else if ( dwVer == 0x100 )
	{
		DWORD dwPieceNUM(0L);
		SFile >> dwPieceNUM;

		//// Note : 전체 조각들이 모두 사용 할수 있도록 최대 배열을 잡을수 있게 해 준다.
		//NSPieceQuickSort::SetArrayMaxSize( dwPieceNUM );

		//DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, pPieceName );

		//   BOOL bLightMapPiece(FALSE);
		//   SFile >> bLightMapPiece;
		//   if ( bLightMapPiece )
		//   {
		//       DWORD dwBufferSize_2;
		//       SFile >> dwBufferSize_2;
		//       if ( pPieceEdit )
		//       {
		//           pPieceEdit->LoadLightMapPiece( pd3dDevice, SFile );
		//       }
		//       else
		//       {
		//           SFile.SetOffSet( SFile.GetfTell() + dwBufferSize_2 );
		//       }
		//   }

		// Note : 전체 조각들이 모두 사용 할수 있도록 최대 배열을 잡을수 있게 해 준다.
		NSPieceQuickSort::SetArrayMaxSize( dwPieceNUM );											// 음 문제.. ~!!

		m_vecPiece.reserve( dwPieceNUM );
		for( DWORD i=0; i<dwPieceNUM; ++i )
		{
			DxPiece* pNew = new DxPiece(pd3dDevice);
			pNew->Load( pd3dDevice, SFile, i, bWLD );
			m_vecPiece.push_back( pNew );
		}

		// LightMap 정보를 로드하는 구간
		{
			DWORD dwPieceEditNUM(0L);
			SFile >> dwPieceEditNUM;

			TSTRING strName;
			for( DWORD i=0; i<dwPieceEditNUM; ++i )
			{
				SFile >> strName;

				DxPieceEdit* pPieceEdit(NULL);
				if ( bWLD )
				{
					pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, strName.c_str() );
				}
				else
				{
					pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, strName.c_str() );
				}
				BOOL bLightMapPiece(FALSE);
				SFile >> bLightMapPiece;
				if ( bLightMapPiece )
				{
					DWORD dwBufferSize_2;
					SFile >> dwBufferSize_2;
					if ( pPieceEdit )
					{
						pPieceEdit->LoadLightMapPiece( pd3dDevice, SFile );
					}
					else
					{
						SFile.SetOffSet( SFile.GetfTell() + dwBufferSize_2 );
					}
				}
			}
		}

		NSPIECETREE::Load( pd3dDevice, m_pAllPieceTree, SFile, m_vecPiece );	// 참조값을 주기 위해 m_vecPiece을 넘겨줌.
	}

	//RegistPresetList();
	MakeTree();
}

void DxPieceAABB::Load_PREV2( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const char* pPieceName, BOOL bWLD )
{
	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	DWORD dwPieceNUM(0L);
	SFile >> dwPieceNUM;

	// Note : 전체 조각들이 모두 사용 할수 있도록 최대 배열을 잡을수 있게 해 준다.
	NSPieceQuickSort::SetArrayMaxSize( dwPieceNUM );

	DxPieceEdit* pPieceEdit(NULL);
	if ( bWLD )
	{
		pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, pPieceName );
	}
	else
	{
		pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, pPieceName );
	}

	BOOL bLightMapPiece(FALSE);
	SFile >> bLightMapPiece;
	if ( bLightMapPiece )
	{
		DWORD dwBufferSize_2;
		SFile >> dwBufferSize_2;
		if ( pPieceEdit )
		{
			pPieceEdit->LoadLightMapPiece( pd3dDevice, SFile );
		}
		else
		{
			SFile.SetOffSet( SFile.GetfTell() + dwBufferSize_2 );
		}
	}

	NSPIECETREE::Load_PREV2( pd3dDevice, m_pAllPieceTree, SFile, pPieceEdit, m_vecPiece );

	//RegistPresetList();
	MakeTree();
}

void DxPieceAABB::Load_PREV( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const char* pPieceName, BOOL bWLD )
{
	DWORD dwPieceNUM(0L);
	SFile >> dwPieceNUM;

	// Note : 전체 조각들이 모두 사용 할수 있도록 최대 배열을 잡을수 있게 해 준다.
	NSPieceQuickSort::SetArrayMaxSize( dwPieceNUM );

	DxPieceEdit* pPieceEdit(NULL);
	if ( bWLD )
	{
		pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, pPieceName );
	}
	else
	{
		pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, pPieceName );
	};

	NSPIECETREE::Load_PREV( pd3dDevice, m_pAllPieceTree, SFile, pPieceEdit, m_vecPiece );	

	//RegistPresetList();
	MakeTree();
}

void DxPieceAABB::LoadBasicAABB( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
{
	if( !m_pAllPieceTree )
	{
		return;
	}
	NSPIECETREE::LoadBasicAABB(	pd3dDevice, m_pAllPieceTree, vMax, vMin );
}

void DxPieceAABB::CloneData( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxPieceAABB* pSrc,
							float fCellSize,
							BOOL bWLD )
{
	BOOL bEnoughUsedPixelLightMap(TRUE);
	TSTRING strLightMapName;
	DWORD dwStartTextureSize;
	CloneData ( pd3dDevice, pSrc, FALSE, strLightMapName, NULL, NULL, dwStartTextureSize, bEnoughUsedPixelLightMap, fCellSize, bWLD );
}

void DxPieceAABB::CloneData( LPDIRECT3DDEVICEQ pd3dDevice, 
                            DxPieceAABB* pSrc, 
                            BOOL bLightMap, 
                            const TSTRING& strLightMapName, 
                            DxStaticMesh* pStaticMesh, 
                            DxPieceManagerPSF* pPieceManager,
                            DWORD& dwStartTextureSize,
							BOOL& bEnoughUsedPixelLightMap,
							float fCellSize,
							BOOL bWLD )
{
	// 데이터 정리.
	CleanUp();

	for( DWORD i=0; i<pSrc->m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = pSrc->m_vecPiece[i];

		DxPiece* pNew = new DxPiece(pd3dDevice);
		pNew->CloneData( pd3dDevice, pCur, bLightMap, strLightMapName, pStaticMesh, pPieceManager, dwStartTextureSize, bEnoughUsedPixelLightMap );

		m_vecPiece.push_back( pNew );
	}

	// 모든 Piece 의 BoxColor 를 계산하도록 한다.
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = m_vecPiece[i];
		pCur->LightMapCalculationBoxColor( pd3dDevice, pStaticMesh, pPieceManager );
	}

	//RegistPresetList();
	MakeTree();

	Active_MATERIAL_DATA( pd3dDevice, bWLD ? FALSE : TRUE );
}

BOOL DxPieceAABB::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3* pNor, D3DXVECTOR3& vCollision, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	BOOL bCollision(FALSE);
	if ( m_pOldMapAndCollisionPieceTree )
	{
		const CLIPVOLUME& cv = DxViewPort::GetInstance().GetClipVolume();
		NSPIECETREE::CollisionLine( m_pOldMapAndCollisionPieceTree, cv, vStart, vEnd, pNor, vCollision, bCollision, emCullMode, bOnlyCamColl );
	}

	return bCollision;
}

BOOL DxPieceAABB::IsCollisionLine2( const D3DXVECTOR3& vStart, 
									D3DXVECTOR3& vEnd, 
									D3DXVECTOR3* vNor, 
									D3DXVECTOR3& vCollision, 
									DxPiece** rrPiece,
                                    EMCOLLMODE emCullMode, 
									BOOL bOnlyCamColl ) const
{
	BOOL bCollision(FALSE);
	if( m_pOldMapAndCollisionPieceTree )
	{
		NSPIECETREE::CollisionLine2( m_pOldMapAndCollisionPieceTree, vStart, vEnd, vNor, vCollision, bCollision, *rrPiece, emCullMode, bOnlyCamColl );
	}

	return bCollision;
}

BOOL DxPieceAABB::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
{
	if( m_pOldMapAndCollisionPieceTree )
	{
		if ( NSPIECETREE::IsCollisionLineSimple( m_pOldMapAndCollisionPieceTree, vStart, vEnd, emCullMode ) )
			return TRUE;
	}

	return FALSE;
}

// Note : 충돌이 되면 바로 리턴한다.
BOOL DxPieceAABB::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV )
{
	return NSPIECETREE::IsCollisionLinePixel( m_pOldMapAndCollisionPieceTree, pd3dDevice, vStart, vEnd, emCullMode, nIndexUV );
}

BOOL DxPieceAABB::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor )
{
	BOOL bColl(FALSE);
	NSPIECETREE::GetLightMapColor( m_pOldMapAndCollisionPieceTree, pd3dDevice, vStart, vEnd, dwColor, bColl );
	return bColl;
}

void DxPieceAABB::ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = m_vecPiece[i];
		pCur->ReloadTexEffVMFX( pd3dDevice );
	}
}

void DxPieceAABB::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot )
{
	for ( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->ExportXFile( pxofsave, pDataObjectRoot );
	}
}

void DxPieceAABB::Archive( boost::function<void (const TCHAR*)> fun )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		DxPiece* pCur = m_vecPiece[i];
		fun( (TSTRING(DxPieceContainer::GetInstance().GetPath()) + pCur->m_strName).c_str() );
	}

	//BOOST_FOREACH(MAPCHANGEPIS_VALUE& it, m_mapChangePis)
	//{
	//    fun( (TSTRING(it.second->m_pPisName) + DxPieceContainer::GetInstance().GetPath()).c_str() );
	//}

	//BOOST_FOREACH(VECPOINTPIS::value_type& it, m_vecPointPis)
	//{
	//    fun( (TSTRING(it->m_pPisName) + DxPieceContainer::GetInstance().GetPath()).c_str() );
	//}
}

void DxPieceAABB::SetPrefabMatrix( const D3DXMATRIX& matPrefab )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->SetPrefabMatrix( matPrefab );
	}
}

void DxPieceAABB::ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece, float fCellSize )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->ExportMAPNAMEPIECE( pd3dDevice, mapNamePiece );
	}

	// Note : Tree를 만든다.
	MakeTree();
}

// Radiosity
void DxPieceAABB::BakeRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
								DxLandMan* pLandMan, 
								LPDIRECT3DTEXTUREQ pTextureSystem, 
								float fReflectance, 
								float fFogBlend, 
								DWORD dwFogLength,
								DWORD dwRadiosityIndex )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->BakeRadiosity( pd3dDevice, pLandMan, pTextureSystem, fReflectance, fFogBlend, dwFogLength, dwRadiosityIndex );
	}
}

void DxPieceAABB::AddRadiosityColorForVoxel( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, LPDIRECT3DTEXTUREQ pTextureSystem, float fReflectance, float fFogBlend, DWORD dwFogLength )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->AddRadiosityColorForVoxel( pd3dDevice, pLandMan, pTextureSystem, fReflectance, fFogBlend, dwFogLength );
	}
}

BOOL DxPieceAABB::IsSupportRAD()
{
	if ( m_vecPiece.empty() )
		return TRUE;

	return m_vecPiece[0]->IsSupportRAD();
}

// Piece의 TriangleLinkLight 를 생성하도록 한다.
void DxPieceAABB::CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
										  DxStaticMeshCollBase* pStaticMesh, 
										  DxPieceManager* pPieceManager,
										  BOOL bDirectionLight )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->CreateTriangleLinkLight( pd3dDevice, pStaticMesh, pPieceManager, bDirectionLight );
	}
}

//  현재 라이트와 연관된 삼각형들의 특정 슬롯에 자리가 남는지 확인.
BOOL DxPieceAABB::IsLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot ) const
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		if ( !m_vecPiece[i]->IsLightID_InsertSlot( dwLightID, dwSlot ) )
			return FALSE;
	}
	return TRUE;
}

// 현재 라이트와 연관된 삼각형들의 특정 슬롯에 현재 LightID 를 셋팅.
void DxPieceAABB::InsertSlotLightID( DWORD dwLightID, DWORD dwSlot )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->InsertSlotLightID( dwLightID, dwSlot );
	}
}

// 한 삼각형에서 Light를 3개 이상 쓰고 있는 것을 찾는다.
BOOL DxPieceAABB::FindTriangleLimitLightNumOver3( DWORD& dwLightID ) const
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		if ( m_vecPiece[i]->FindTriangleLimitLightNumOver3( dwLightID ) )
			return TRUE;
	}

	return FALSE;
}

// 특정 Light를 줄여주도록 한다.
void DxPieceAABB::EraseLightID( DWORD dwLightID )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->EraseLightID( dwLightID );
	}
}

// Piece 의 PS_3_0의 렌더타입을 설정하도록 한다.
void DxPieceAABB::SetType_PS_3_0()
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->SetType_PS_3_0();
	}
}

// Material 을 구한다.
void DxPieceAABB::Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool )
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->Active_MATERIAL_DATA( pd3dDevice, bTool );
	}
}

// 강제로 DF 로 렌더된다면 그에 맞는 옵션값들을 변화시킨다.
void DxPieceAABB::SetData_Force_TnL_PS_3_0_DF()
{
	for( DWORD i=0; i<m_vecPiece.size(); ++i )
	{
		m_vecPiece[i]->SetData_Force_TnL_PS_3_0_DF();
	}

}