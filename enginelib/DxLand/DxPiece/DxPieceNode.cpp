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
#include "./DxPieceInstancing.h"
#include "./NSPieceQuickSort.h"
#include "./DxPieceStaticAni.h"
#include "./DxPieceTexEff.h"
#include "./DxPieceEdit.h"
#include "./DxPieceAABB.h"
#include "./DxPieceEff.h"
#include "./DxPiece.h"

#include "./DxPieceNode.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//											D	x		P	i	e	c	e		T	r	e	e
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxPieceNode::ReleasePIECE()
{
	//if (m_bDeletePIECE)
	//{
	//	size_t sizeVector = m_vecPIECE.size();
	//	for (size_t i=0; i<sizeVector; ++i)
	//		SAFE_DELETE(m_vecPIECE[i]);
	//	m_bDeletePIECE = FALSE;
	//}
	m_vecPIECE.clear();
	m_mapPieceInstancing.clear();
	//m_vecInstancingExceptPiece.clear();
}

void DxPieceNode::Save( sc::SerialFile& SFile )
{
    SFile << static_cast<DWORD>( VERSION );

    SFile.BeginBlock();
    {
		SFile << m_vMax;
		SFile << m_vMin;

		// Note : 개수 얻기
		int nSize = (int)m_vecPIECE.size();
		SFile << nSize;

		for( int i=0; i<nSize; ++i )
		{
			SFile << m_vecPIECE[i]->GetID();
		}

		if( m_pLeftChild )
		{
			SFile << (BOOL)TRUE;
			m_pLeftChild->Save( SFile );
		}
		else
		{
			SFile << (BOOL)FALSE;
		}
		if( m_pRightChild )	
		{
			SFile << (BOOL)TRUE;
			m_pRightChild->Save( SFile );
		}
		else
		{
			SFile << (BOOL)FALSE;
		}
    }
    SFile.EndBlock();
}

void DxPieceNode::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const VEC_PIECE& vecPiece )
{
	ReleasePIECE();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	SFile >> m_vMax;
	SFile >> m_vMin;

	m_vCenter = (m_vMax+m_vMin)*0.5f;
	m_vHalf = m_vMax-m_vCenter;
	m_fRadius = D3DXVec3Length( &m_vHalf );

	DWORD dwID(0L);
	int nCount = 0;
	SFile >> nCount;
	for( int i=0; i<nCount; ++i )
	{
		SFile >> dwID;
		m_vecPIECE.push_back( vecPiece[dwID] );
	}

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		m_pLeftChild = new DxPieceNode;
		m_pLeftChild->Load( pd3dDevice, SFile, vecPiece );
	}

	SFile >> bExist;
	if( bExist )
	{
		m_pRightChild = new DxPieceNode;
		m_pRightChild->Load( pd3dDevice, SFile, vecPiece );
	}
}

void DxPieceNode::Load_PREV2( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxPieceEdit* pSrc, VEC_PIECE& vecPiece )
{
	ReleasePIECE();

    DWORD dwVer;
    SFile >> dwVer;

    // 사이즈 저장은 하지 말자.
    if ( dwVer == 0x100 )
    {
        DWORD dwBufferSize;
        SFile >> dwBufferSize;
    }

	SFile >> m_vMax;
	SFile >> m_vMin;

	//int nCount = 0;
	//SFile >> nCount;
	//for( int i=0; i<nCount; ++i )
	//{
	//	DxPiece* pNew = new DxPiece;
	//	pNew->Load( pd3dDevice, SFile, pSrc );

	//	// Tree List 연결
	//	pNew->m_pTreeNext = m_pPiece;
	//	m_pPiece = pNew;
	//}
	int nCount = 0;
	SFile >> nCount;
	for( int i=0; i<nCount; ++i )
	{
		DxPiece* pNew = new DxPiece(pd3dDevice);
		pNew->Load_PREV2( pd3dDevice, SFile, pSrc );
		m_vecPIECE.push_back( pNew );
		vecPiece.push_back( pNew );
	}
	//m_bDeletePIECE = TRUE;

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		m_pLeftChild = new DxPieceNode;
		m_pLeftChild->Load_PREV2( pd3dDevice, SFile, pSrc, vecPiece );
	}

	SFile >> bExist;
	if( bExist )
	{
		m_pRightChild = new DxPieceNode;
		m_pRightChild->Load_PREV2( pd3dDevice, SFile, pSrc, vecPiece );
	}
}

void DxPieceNode::Load_PREV( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxPieceEdit* pSrc, VEC_PIECE& vecPiece )
{
	ReleasePIECE();

	SFile >> m_vMax;
	SFile >> m_vMin;

	//int nCount = 0;
	//SFile >> nCount;
	//for( int i=0; i<nCount; ++i )
	//{
	//	DxPiece* pNew = new DxPiece;
	//	pNew->Load_PREV( pd3dDevice, SFile, pSrc );

	//	// Tree List 연결
	//	pNew->m_pTreeNext = m_pPiece;
	//	m_pPiece = pNew;
	//}
	int nCount = 0;
	SFile >> nCount;
	for( int i=0; i<nCount; ++i )
	{
		DxPiece* pNew = new DxPiece(pd3dDevice);
		pNew->Load_PREV( pd3dDevice, SFile, pSrc );
		m_vecPIECE.push_back( pNew );
		vecPiece.push_back( pNew );
	}
	//m_bDeletePIECE = TRUE;

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		m_pLeftChild = new DxPieceNode;
		m_pLeftChild->Load_PREV( pd3dDevice, SFile, pSrc, vecPiece );
	}

	SFile >> bExist;
	if( bExist )
	{
		m_pRightChild = new DxPieceNode;
		m_pRightChild->Load_PREV( pd3dDevice, SFile, pSrc, vecPiece );
	}
}

void DxPieceNode::InitInstancing( VEC_PIECE& vecInstancingExceptPiece )
{
	//m_mapPieceInstancing.clear();

	//VEC_PIECE_ITER iterA = m_vecPIECE.begin();
	//for ( ; iterA != m_vecPIECE.end() ; ++iterA )
	//{
	//	DxStaticAniFrame* pStaticAniFrame = (*iterA)->GetStaticAniFrame();
	//	if ( !pStaticAniFrame )
	//	{
	//		GASSERT( 0 && _T("특정 Piece 파일이 존재하지 않습니다. log를 참조해주세요.") );
	//		return;
	//	}

	//	PIECE_INSTANCING_FIRST sFirst( pStaticAniFrame, (*iterA)->GetLightMapCombined() );

	//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.find( sFirst );
	//	if ( iter == m_mapPieceInstancing.end() )
	//	{
	//		std::tr1::shared_ptr<DxPieceInstancing> spNew = std::tr1::shared_ptr<DxPieceInstancing>( new DxPieceInstancing(pStaticAniFrame) );
	//		if ( spNew->Insert( (*iterA) ) )
	//		{
	//			m_mapPieceInstancing.insert( std::make_pair( sFirst, spNew ) );
	//		}
	//		else
	//		{
	//			vecInstancingExceptPiece.push_back( (*iterA) );
	//		}
	//	}
	//	else
	//	{
	//		if ( !(*iter).second->Insert( (*iterA) ) )
	//		{
	//			vecInstancingExceptPiece.push_back( (*iterA) );
	//		}
	//	}
	//}

	//// 너무 갯수가 적은건 m_vecPIECE 로 다시 넣어준다.
	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
	//for ( ; iter!=m_mapPieceInstancing.end(); )
	//{
	//	// 4 개보다 적을 경우는 굳이 Instancing 을 하지 않도록 한다.
	//	if ( (*iter).second->GetSize() < 4 )
	//	{
	//		(*iter).second->ExportPiece( vecInstancingExceptPiece );

	//		iter = m_mapPieceInstancing.erase( iter );
	//	}
	//	else
	//	{
	//		++iter;
	//	}
	//}
}

void DxPieceNode::CreateVB_ForInstancing( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//// InstancingVB 를 만든다.
	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
	//for ( ; iter!=m_mapPieceInstancing.end(); ++iter )
	//{
	//	(*iter).second->CreateVB( pd3dDevice );
	//	(*iter).second->CalculateAABB();
	//}
}

//// 2.0 이하에서만 동작되는 것들 생성
//void DxPieceNode::CreateUnderShader_2_0()
//{
//	m_vecUnderShader_2_0.clear();
//
//	VEC_PIECE_ITER iterA = m_vecPIECE.begin();
//	for ( ; iterA != m_vecPIECE.end() ; ++iterA )
//	{
//		if ( !(*iterA)->IsShader_3_0() )
//		{
//			m_vecUnderShader_2_0.push_back( (*iterA) );
//		}
//	}
//}

//void DxPieceNode::FrameMove_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime, VEC_PIECE& vecInstancingExceptPiece )
//{
//	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
//	//for ( ; iter!=m_mapPieceInstancing.end(); )
//	//{
//	//	if ( (*iter).second->FrameMove_Instancing( pd3dDevice, fElapsedTime ) )
//	//	{
//	//		++iter;
//	//	}
//	//	else	
//	//	{
//	//		// Instancing 조건에 만족이 안되는 상황이다.
//	//		(*iter).second->ExportPiece( vecInstancingExceptPiece );
//
//	//		// 삭제
//	//		iter = m_mapPieceInstancing.erase( iter );
//	//	}
//	//}
//}

void DxPieceNode::RenderOpaque_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
	//for ( ; iter!=m_mapPieceInstancing.end(); ++iter )
	//{
	//	(*iter).second->RenderOpaque_Instancing( pd3dDevice, sCV );
	//}
}

void DxPieceNode::RenderHardAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
	//for ( ; iter!=m_mapPieceInstancing.end(); ++iter )
	//{
	//	(*iter).second->RenderHardAlpha_Instancing( pd3dDevice, sCV );
	//}
}

void DxPieceNode::RenderEffFrame_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bLBuffer )
{
	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
	//for ( ; iter!=m_mapPieceInstancing.end(); ++iter )
	//{
	//	(*iter).second->RenderEffFrame_Instancing( pd3dDevice, bLBuffer );
	//}
}

void DxPieceNode::RenderDecal_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
	//for ( ; iter!=m_mapPieceInstancing.end(); ++iter )
	//{
	//	(*iter).second->RenderDecal_Instancing( pd3dDevice );
	//}
}

void DxPieceNode::RenderDecalPass2_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
	//for ( ; iter!=m_mapPieceInstancing.end(); ++iter )
	//{
	//	(*iter).second->RenderDecalPass2_Instancing( pd3dDevice );
	//}
}

void DxPieceNode::RenderWaterCustomSoftAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
	//for ( ; iter!=m_mapPieceInstancing.end(); ++iter )
	//{
	//	(*iter).second->RenderWaterCustomSoftAlpha_Instancing( pd3dDevice );
	//}
}

void DxPieceNode::RenderEff_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxStaticMeshLightMap* pStaticMeshLightMap )
{
	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
	//for ( ; iter!=m_mapPieceInstancing.end(); ++iter )
	//{
	//	(*iter).second->RenderEff_Instancing( pd3dDevice, sCV, pStaticMeshLightMap );
	//}
}

void DxPieceNode::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
	//for ( ; iter!=m_mapPieceInstancing.end(); ++iter )
	//{
	//	(*iter).second->OnResetDevice( pd3dDevice );
	//}
}

void DxPieceNode::OnLostDevice()
{
	//MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
	//for ( ; iter!=m_mapPieceInstancing.end(); ++iter )
	//{
	//	(*iter).second->OnLostDevice();
	//}
}