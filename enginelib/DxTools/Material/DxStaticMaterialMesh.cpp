#include "pch.h"

#include <algorithm>
#include "../../Common/StlFunctions.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/EBTime.h"
#include "../../DxTools/Material/NSShadowRenderForMaterial.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/Texture/NSTexture16Bit.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/Collision.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxLand/LightMap/LightMapObjectVEC.h"
#include "../../DxLand/LightMap/LightMapObject.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../DxLand/LightMap/LightMapDef.h"
#include "../../DxLand/LightMap/NSLightMap.h"
#include "../../DxLand/DxPiece/NSOctreeRenderList.h"
#include "../../DxLand/DxPiece/DxSingleTexMesh.h"
#include "../../DxLand/DxPiece/DxPieceDef.h"
#include "../../DxLand/DxLandMan.h"
#include "../../DxLand/DxLandDef.h"
#include "../../DxEffect/Mesh/DxStaticMesh.h"
#include "../../GlobalParam.h"
#include "../RENDERPARAM.h"

#include "./DxSingleMaterialMeshRenderOrder.h"
#include "./DxSingleMaterialMesh.h"

#include "DxStaticMaterialMesh.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		S	t	a	t	i	c		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMaterialMesh::DxStaticMaterialMesh()
    : m_emRenderType(EMRT_OCTREE)
    //, m_strLightMap(_T(""))
	, m_strLightMapCombinedForFieldMesh(_T(""))			// Piece 가 아닌 FieldMesh
	, m_strLightMapDirectShadow1stForFieldMesh(_T(""))	// Piece 가 아닌 FieldMesh
	, m_strLightMapDirectShadow2ndForFieldMesh(_T(""))	// Piece 가 아닌 FieldMesh
	, m_strLightMapIndirectForFieldMesh(_T(""))			// Piece 가 아닌 FieldMesh
	, m_pList(NULL)
	, m_pMeshTree(NULL)
	, m_vMin(FLT_MAX,FLT_MAX,FLT_MAX)
	, m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
	, m_spRenderOrder(new DxSingleMaterialMeshRenderOrder)
{
	m_vVoxelColorForFieldMesh = D3DXVECTOR4( 1.f, 1.f, 1.f, 1.f );

	for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
	{
		m_avAmbientCubeForFieldMesh[i] = D3DXVECTOR4( 1.f, 1.f, 1.f, 1.f );
	}
}

DxStaticMaterialMesh::~DxStaticMaterialMesh()
{
	CleanUp();

	//// 파일을 닫는다.
	//m_SFile.CloseFile();
}

void DxStaticMaterialMesh::StartThread( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//InitializeCriticalSection(&m_CSLockLoading);
	//m_sThread.StartThread( pd3dDevice, &m_SFile, &m_listLoadingData, &m_CSLockLoading );
}

void DxStaticMaterialMesh::EndThread()
{
	//DxStaticMeshColor_THREAD::GetInstance().DeleteList();	//	관련 List를 지운다.

	//m_sThread.EndThread();
	//DeleteCriticalSection(&m_CSLockLoading);
}

void DxStaticMaterialMesh::CleanUp()
{
	m_vMin = D3DXVECTOR3 (FLT_MAX,FLT_MAX,FLT_MAX);
	m_vMax = D3DXVECTOR3 (-FLT_MAX,-FLT_MAX,-FLT_MAX);

	m_pList = NULL;

	SAFE_DELETE ( m_pMeshTree );

	std::for_each ( m_mmapMesh.begin(), m_mmapMesh.end(), std_afunc::DeleteMapObject() );
	m_mmapMesh.clear();
}

void DxStaticMaterialMesh::FrameMove( const float fElapsedTime )
{
	// Note : 조각 파일 일 경우는 계산하면 안된다.
	if( m_emRenderType==EMRT_PIECE )
        return;

	DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectLight();
	const D3DLIGHT9& sLight = pLight->m_Light;

	m_vVoxelColorForFieldMesh.x = sLight.Diffuse.r + sLight.Ambient.r;
	m_vVoxelColorForFieldMesh.y = sLight.Diffuse.g + sLight.Ambient.g;
	m_vVoxelColorForFieldMesh.z = sLight.Diffuse.b + sLight.Ambient.b;
	m_vVoxelColorForFieldMesh.w = 1.f;

	D3DXVECTOR3 avNor[LightMapBoxColor::TEST_COUNT];
	avNor[0] = D3DXVECTOR3( 1.f, 0.f, 0.f );
	avNor[1] = D3DXVECTOR3( -1.f, 0.f, 0.f );
	avNor[2] = D3DXVECTOR3( 0.f, 1.f, 0.f );
	avNor[3] = D3DXVECTOR3( 0.f, -1.f, 0.f );
	avNor[4] = D3DXVECTOR3( 0.f, 0.f, 1.f );
	avNor[5] = D3DXVECTOR3( 0.f, 0.f, -1.f );


	D3DXVECTOR3 vLightDir;
	vLightDir.x = -sLight.Direction.x;
	vLightDir.y = -sLight.Direction.y;
	vLightDir.z = -sLight.Direction.z;
	D3DXVec3Normalize( &vLightDir, &vLightDir );
	for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
	{
		float fDot = D3DXVec3Dot( &avNor[i], &vLightDir );
		if ( fDot < 0.f )
			fDot = 0.f;

		m_avAmbientCubeForFieldMesh[i].x = sLight.Diffuse.r*fDot + sLight.Ambient.r;
		m_avAmbientCubeForFieldMesh[i].y = sLight.Diffuse.g*fDot + sLight.Ambient.g;
		m_avAmbientCubeForFieldMesh[i].z = sLight.Diffuse.b*fDot + sLight.Ambient.b;
		m_avAmbientCubeForFieldMesh[i].w = 1.f;
	}
}

void DxStaticMaterialMesh::RenderOrderSetting( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	//m_spRenderOrder->CleanUp();

	//DWORD dwTexType(0L);
	////COLLISION::RenderOrderSetting( pd3dDevice, sCV, m_pMeshTree, m_spRenderOrder.get(), dwTexType );
	//RenderOrderSettingPiece( pd3dDevice, FALSE, dwTexType );
}

void DxStaticMaterialMesh::Render( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld )
{
	DWORD dwAddTexType(0L);
	m_spRenderOrder->SetTextureTypeForUnKnownTexture( pd3dDevice, dwAddTexType );
	m_dwTexType |= dwAddTexType;

	if ( m_spRenderOrder->IsTextureTypeComplete() )
		m_dwTexType &= ~EMTTF_UNKNOWN;

	m_spRenderOrder->RenderOpaque
	( 
		pd3dDevice, 
		emCullingRenderType, 
		m_textureResCombinedForFieldMesh.GetTexture(), 
		m_textureResDirectShadow1stForFieldMesh.GetTexture(),
		m_textureResDirectShadow2ndForFieldMesh.GetTexture(),
		m_textureResIndirectForFieldMesh.GetTexture(),
		bMPShading, 
		&m_avAmbientCubeForFieldMesh[0], 
		m_vVoxelColorForFieldMesh,
		matWorld
	);
}

void DxStaticMaterialMesh::RenderHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld )
{
	DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
	m_spRenderOrder->RenderHardAlpha
	( 
		pd3dDevice, 
		emCullingRenderType, 
		m_textureResCombinedForFieldMesh.GetTexture(), 
		m_textureResDirectShadow1stForFieldMesh.GetTexture(),
		m_textureResDirectShadow2ndForFieldMesh.GetTexture(),
		m_textureResIndirectForFieldMesh.GetTexture(),
		bMPShading, 
		&m_avAmbientCubeForFieldMesh[0],
		m_vVoxelColorForFieldMesh,
		matWorld
	);
	DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
}

void DxStaticMaterialMesh::RenderSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld )
{
	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );

	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	m_spRenderOrder->RenderSoftAlpha
	( 
		pd3dDevice, 
		emCullingRenderType, 
		m_textureResCombinedForFieldMesh.GetTexture(), 
		m_textureResDirectShadow1stForFieldMesh.GetTexture(),
		m_textureResDirectShadow2ndForFieldMesh.GetTexture(),
		m_textureResIndirectForFieldMesh.GetTexture(),
		bMPShading, 
		&m_avAmbientCubeForFieldMesh[0], 
		m_vVoxelColorForFieldMesh,
		matWorld
	);

	float fBias = 0.0000f;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
}

void DxStaticMaterialMesh::RenderSoftAlpha_Deffered( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const D3DXMATRIX& matWorld )
{
	m_spRenderOrder->RenderSoftAlpha
	( 
		pd3dDevice, 
		emCullingRenderType, 
		m_textureResCombinedForFieldMesh.GetTexture(), 
		m_textureResDirectShadow1stForFieldMesh.GetTexture(),
		m_textureResDirectShadow2ndForFieldMesh.GetTexture(),
		m_textureResIndirectForFieldMesh.GetTexture(),
		FALSE, 
		&m_avAmbientCubeForFieldMesh[0], 
		m_vVoxelColorForFieldMesh,
		matWorld
	);
}

void DxStaticMaterialMesh::RenderDecal( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld )
{
	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
	m_spRenderOrder->RenderDecal( pd3dDevice, emCullingRenderType, bMPShading, &m_avAmbientCubeForFieldMesh[0], m_vVoxelColorForFieldMesh, matWorld );
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
}

void DxStaticMaterialMesh::RenderDecalPass2( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const D3DXMATRIX& matWorld )
{
	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );

	m_spRenderOrder->RenderDecalPass2( pd3dDevice, emCullingRenderType, &m_avAmbientCubeForFieldMesh[0], m_vVoxelColorForFieldMesh, matWorld );

	float fBias = 0.0000f;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
}

// DxStaticMaterialMeshLightMap::RenderWater_CustomSoftAlpha 에서 renderstate 조절
// UNLIT
void DxStaticMaterialMesh::RenderCustomMaterialUNLIT_UNFOG( const LPDIRECT3DDEVICEQ pd3dDevice, 
																NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	m_spRenderOrder->RenderCustomUNLIT_UNFOG( pd3dDevice, emCullingRenderType, matIdentity );
}

// DxStaticMaterialMeshLightMap::RenderWater_CustomSoftAlpha 에서 renderstate 조절
// UNLIT
void DxStaticMaterialMesh::RenderCustomMaterialUNLIT_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
															   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
															   float fPrevHeight, 
															   float &fHeight,
															   float &fNearLength )
{
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	m_spRenderOrder->RenderWater( pd3dDevice, emCullingRenderType, fPrevHeight, fHeight, fNearLength, matIdentity );
	m_spRenderOrder->RenderCustomSoftAlpha( pd3dDevice, emCullingRenderType, matIdentity );
}

void DxStaticMaterialMesh::RenderCascadeShadowOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	//m_spRenderOrder->CleanUp();
	//DWORD dwTexType(0L);
	//COLLISION::RenderOrderSetting( pd3dDevice, sCV, m_pMeshTree, m_scpRenderOrder.get(), dwTexType );

	// Opaque
	if ( emCullingRenderType == NSLandThread::TYPE_NO_FIELDMESH )
	{
		pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_ONLYPOS );
		//NSShadowRenderForMaterial::CommitChanges();

		if ( NSShadowRenderForMaterial::FX_BeginPass( 0 ) )
		{
			m_spRenderOrder->RenderCascadeShadowOpaque( pd3dDevice, emCullingRenderType );

			NSShadowRenderForMaterial::FX_EndPass();
		}
	}
	else
	{
		m_spRenderOrder->RenderCascadeShadowOpaque( pd3dDevice, emCullingRenderType );
	}
}

void DxStaticMaterialMesh::RenderCascadeShadowHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	// HardAlpha
	if ( emCullingRenderType == NSLandThread::TYPE_NO_FIELDMESH )
	{
		pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_POSTEX );
		//NSShadowRenderForMaterial::CommitChanges();

		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );

		if ( NSShadowRenderForMaterial::FX_BeginPass( 1 ) )
		{
			m_spRenderOrder->RenderCascadeShadowHardAlpha( pd3dDevice, emCullingRenderType );

			NSShadowRenderForMaterial::FX_EndPass();
		}

		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
	}
	else
	{
		m_spRenderOrder->RenderCascadeShadowHardAlpha( pd3dDevice, emCullingRenderType );
	}
}

void DxStaticMaterialMesh::Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV )
{
	m_spRenderOrder->Render_Query( pd3dDevice, pCV );
}

VOID DxStaticMaterialMesh::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxSingleMaterialMesh* pCur = m_pList;
	while( pCur )
	{
		pCur->CreateQuery( pd3dDevice );
		pCur = pCur->m_pNext;
	}
}

VOID DxStaticMaterialMesh::DestroyQuery()
{
	DxSingleMaterialMesh* pCur = m_pList;
	while( pCur )
	{
		pCur->DestroyQuery();
		pCur = pCur->m_pNext;
	}
}

void DxStaticMaterialMesh::ResetOcclusionQuery()
{
	DxSingleMaterialMesh* pCur = m_pList;
	while( pCur )
	{
		pCur->ResetOcclusionQuery();
		pCur = pCur->m_pNext;
	}
}

void DxStaticMaterialMesh::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex )
{
	DxSingleMaterialMesh* pCur = m_pList;
	while( pCur )
	{
		pCur->FindOcclusionRenderIndex( pd3dDevice, sCV, vecOctreeIndex );
		pCur = pCur->m_pNext;
	}
}

// Note : vEnd 이 값은 계속 줄어든다.
//		그래서 제일 적은 위치에 있는것을 찾을 수 있다.
BOOL DxStaticMaterialMesh::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
									LPCSTR& szName, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	BOOL		bUse		= FALSE;

	MMAPSINGLEMATERIALMESH_CITER iter = m_mmapMesh.begin();
	for( ; iter!=m_mmapMesh.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, szName, emCullMode, bOnlyCamColl ) )	bUse = TRUE;
	}

	return bUse;
}

// Note : vEnd 이 값은 계속 줄어든다.
//		그래서 제일 적은 위치에 있는것을 찾을 수 있다.
BOOL DxStaticMaterialMesh::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	BOOL		bUse		= FALSE;

	MMAPSINGLEMATERIALMESH_CITER iter = m_mmapMesh.begin();
	for( ; iter!=m_mmapMesh.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, emCullMode, bOnlyCamColl) )	bUse = TRUE;
	}

	return bUse;
}

BOOL DxStaticMaterialMesh::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
{
	MMAPSINGLEMATERIALMESH_CITER iter = m_mmapMesh.begin();
	for( ; iter!=m_mmapMesh.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
			return TRUE;
	}

	return FALSE;
}

// Note : 충돌이 되면 바로 리턴한다.
BOOL DxStaticMaterialMesh::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV )
{
	MMAPSINGLEMATERIALMESH_CITER iter = m_mmapMesh.begin();
	for( ; iter!=m_mmapMesh.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, nIndexUV ) )
            return TRUE;
	}

    return FALSE;
}

void DxStaticMaterialMesh::GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) // 주병진 추가 메소드
{
	BOOL bUse = FALSE;

	//Check
  	MMAPSINGLEMATERIALMESH_ITER iter = m_mmapMesh.begin();
	for( ; iter!=m_mmapMesh.end(); ++iter )
	{
		(*iter).second->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
	}
}

void DxStaticMaterialMesh::GetCollisionLineInTriListExceptYReplacePiece ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) // 주병진 추가 메소드
{

}

BOOL DxStaticMaterialMesh::GetLightMapColorForFieldMesh( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor )
{
	// Note : UV로 좌표를 구한다.
	NSTexture16Bit::DATA* pDataDay(NULL);
	if ( m_textureResCombinedForFieldMesh.GetTexture() )
	{
		pDataDay = NSTexture16Bit::Insert( pd3dDevice, m_textureResCombinedForFieldMesh.GetFileName(), m_textureResCombinedForFieldMesh.GetTexture() );
	}

	return GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor, pDataDay );
}

BOOL DxStaticMaterialMesh::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor, const NSTexture16Bit::DATA* pDataDay )
{
	MMAPSINGLEMATERIALMESH* mapMesh_ARRAY[1] = { &m_mmapMesh };

	BOOL bExist(FALSE);
	for ( DWORD i=0; i<1; ++i )
	{
		if ( mapMesh_ARRAY[i]->empty() )
			continue;

		MMAPSINGLEMATERIALMESH_ITER iter = mapMesh_ARRAY[i]->begin();
		for ( ; iter!=mapMesh_ARRAY[i]->end(); ++iter )
		{
			if ( (*iter).second->GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor, pDataDay ) )
			{
				bExist = TRUE;
			}
		}
	}
	return bExist;
}

void DxStaticMaterialMesh::RefreshLightMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_strLightMapCombinedForFieldMesh.empty() )
	{
		TextureManager::GetInstance().DeleteData( m_strLightMapCombinedForFieldMesh );
	}

	if ( !m_strLightMapDirectShadow1stForFieldMesh.empty() )
	{
		TextureManager::GetInstance().DeleteData( m_strLightMapDirectShadow1stForFieldMesh );
	}

	if ( !m_strLightMapDirectShadow2ndForFieldMesh.empty() )
	{
		TextureManager::GetInstance().DeleteData( m_strLightMapDirectShadow2ndForFieldMesh );
	}

	if ( !m_strLightMapIndirectForFieldMesh.empty() )
	{
		TextureManager::GetInstance().DeleteData( m_strLightMapIndirectForFieldMesh );
	}

	LoadLightMapForFieldMesh( pd3dDevice );
}

void DxStaticMaterialMesh::SetAABBMaxMin ()
{
	m_vMax = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	m_vMin = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
	D3DXVECTOR3	vMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	D3DXVECTOR3	vMin( FLT_MAX, FLT_MAX, FLT_MAX );

	MMAPSINGLEMATERIALMESH_ITER iter;
	for( iter=m_mmapMesh.begin(); iter!=m_mmapMesh.end(); ++iter )
	{
		(*iter).second->GetAABBSize( vMax, vMin );

		if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
		if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
		if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

		if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
		if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
		if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
	}
}

void DxStaticMaterialMesh::BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, sc::SerialFile* const pSFile )
{
	float fDis = DxFogMan::GetInstance().GetFogRangeFar();
	D3DXVECTOR3 vMax = vPos + D3DXVECTOR3( fDis, fDis, fDis );
	D3DXVECTOR3 vMin = vPos - D3DXVECTOR3( fDis, fDis, fDis );

	BaseLoad( pd3dDevice, vMax, vMin, pSFile );
}

void DxStaticMaterialMesh::BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const pSFile )
{
	COLLISION::LoadAABBTreeBOX( pd3dDevice, vMax, vMin, m_pMeshTree, pSFile );

	//if ( m_spRenderOrder->IsValidData() )
	//{
	//	if ( pCV )
	//	{
	//		RenderOrderSetting( pd3dDevice, *pCV );
	//	}
	//	else
	//	{
			// Piece 는 아니지만, 임시로 정보를 가지고 있어야 되어서 셋팅된다.
	//		RenderOrderSettingPiece( pd3dDevice, FALSE, m_dwTexType );
	//	}
	//}
}

void DxStaticMaterialMesh::BaseLoadPiece( LPDIRECT3DDEVICEQ pd3dDevice )
{
	MMAPSINGLEMATERIALMESH_ITER iter;
	for( iter=m_mmapMesh.begin(); iter!=m_mmapMesh.end(); ++iter )
	{
		(*iter).second->LoadTexture_NonThread( pd3dDevice );
	}
}

void DxStaticMaterialMesh::Save ( sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	DWORD	dwSize = 0;

	SFile << static_cast<DWORD>( VERSION );

	SFile.BeginBlock();
	{
		SFile << m_vMax;
		SFile << m_vMin;
		SFile << m_strLightMapCombinedForFieldMesh;			// Piece 가 아닌 FieldMesh
		SFile << m_strLightMapDirectShadow1stForFieldMesh;	// Piece 가 아닌 FieldMesh
		SFile << m_strLightMapDirectShadow2ndForFieldMesh;	// Piece 가 아닌 FieldMesh
		SFile << m_strLightMapIndirectForFieldMesh;			// Piece 가 아닌 FieldMesh
		SFile << m_wwLightMapUserUV_TEXEL.dwData;
	}
	SFile.EndBlock();

	SingleTexMesh_Save( m_mmapMesh, SFile, bPiece, pArchive );
}

void DxStaticMaterialMesh::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	CleanUp();	// 모두 초기화

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	switch ( dwVer )
	{
	case VERSION:
		{
			Load_105( pd3dDevice, SFile, bDynamicLoad, bPiece, pArchive );
		}
		break;

	case 0x0104:
		{
			Load_104( pd3dDevice, SFile, bDynamicLoad, bPiece, pArchive );
		}
		break;

	case 0x0103:
		{
			Load_102_103( pd3dDevice, SFile, bDynamicLoad, bPiece, pArchive );
		}
		break;

		// 이하 버전은 밤 LightMap 정보를 없애도록 한다.
	case 0x0102:
		{
			Load_102_103( pd3dDevice, SFile, bDynamicLoad, bPiece, pArchive );
		}
		break;

	case 0x0101:
		{
			Load_101( pd3dDevice, SFile, bDynamicLoad, bPiece, pArchive );
		}
		break;

	case 0x0100:
		{
			Load_100( pd3dDevice, SFile, bDynamicLoad, bPiece, pArchive );
		}
		break;

	default:
		{
			DWORD dwCur = SFile.GetfTell();
			SFile.SetOffSet( dwCur+dwBufferSize );
		}
		break;
	};	

	SetAABBMaxMin();

	LoadLightMapForFieldMesh( pd3dDevice );


	return;
}

void DxStaticMaterialMesh::SingleTexMesh_Save( MMAPSINGLEMATERIALMESH& mapMesh, sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	DWORD	dwSize = 0;

	dwSize = (DWORD)mapMesh.size();
	SFile << dwSize;

	MMAPSINGLEMATERIALMESH_ITER iter;
	for ( iter=mapMesh.begin(); iter!=mapMesh.end(); ++iter )
	{
		SFile << (*iter).first;
		(*iter).second->Save( SFile, bPiece, pArchive );
	}
}

void DxStaticMaterialMesh::SingleTexMesh_Load_Insert( LPDIRECT3DDEVICEQ pd3dDevice, MMAPSINGLEMATERIALMESH& mapMesh, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	DWORD	dwSize = 0;

	if( bPiece )	m_emRenderType = EMRT_PIECE;
	else			m_emRenderType = EMRT_OCTREE;

	std::string szName;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> szName;
		DxSingleMaterialMesh* pNew = new DxSingleMaterialMesh;
		pNew->Load( pd3dDevice, SFile, bDynamicLoad, bPiece, pArchive );

		mapMesh.insert( std::make_pair ( szName, pNew ) );
	}
}

void DxStaticMaterialMesh::Load_105( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	SFile >> m_vMax;
	SFile >> m_vMin;
	SFile >> m_strLightMapCombinedForFieldMesh;			// Piece 가 아닌 FieldMesh
	SFile >> m_strLightMapDirectShadow1stForFieldMesh;	// Piece 가 아닌 FieldMesh
	SFile >> m_strLightMapDirectShadow2ndForFieldMesh;	// Piece 가 아닌 FieldMesh
	SFile >> m_strLightMapIndirectForFieldMesh;			// Piece 가 아닌 FieldMesh
	SFile >> m_wwLightMapUserUV_TEXEL.dwData;

#ifndef  NDEBUG
	static float s_fStartTime = 0.f;
	static float s_fEndTime = 0.f;

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Note : DxSingleMaterialMesh 내부에서 Version 체크가 일어난다.
	SingleTexMesh_Load_Insert( pd3dDevice, m_mmapMesh, SFile, bDynamicLoad, bPiece, pArchive );


#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "Static Load : %f", s_fEndTime );

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Tree 를 만들면서 그와 관련된 작업들을 정리한다.
	CreateTree_AddWork();

#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "MakeAABBTree : %f", s_fEndTime );
#endif
}

void DxStaticMaterialMesh::Load_104( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	SFile >> m_vMax;
	SFile >> m_vMin;
	SFile >> m_strLightMapCombinedForFieldMesh;			// Piece 가 아닌 FieldMesh
	SFile >> m_strLightMapDirectShadow1stForFieldMesh;	// Piece 가 아닌 FieldMesh
	SFile >> m_strLightMapIndirectForFieldMesh;			// Piece 가 아닌 FieldMesh
	SFile >> m_wwLightMapUserUV_TEXEL.dwData;

	m_strLightMapDirectShadow2ndForFieldMesh = m_strLightMapDirectShadow1stForFieldMesh;

#ifndef  NDEBUG
	static float s_fStartTime = 0.f;
	static float s_fEndTime = 0.f;

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Note : DxSingleMaterialMesh 내부에서 Version 체크가 일어난다.
	SingleTexMesh_Load_Insert( pd3dDevice, m_mmapMesh, SFile, bDynamicLoad, bPiece, pArchive );


#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "Static Load : %f", s_fEndTime );

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Tree 를 만들면서 그와 관련된 작업들을 정리한다.
	CreateTree_AddWork();

#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "MakeAABBTree : %f", s_fEndTime );
#endif
}

void DxStaticMaterialMesh::Load_102_103( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	SFile >> m_vMax;
	SFile >> m_vMin;
	SFile >> m_strLightMapCombinedForFieldMesh;			// Piece 가 아닌 FieldMesh
	SFile >> m_strLightMapDirectShadow1stForFieldMesh;	// Piece 가 아닌 FieldMesh
	SFile >> m_wwLightMapUserUV_TEXEL.dwData;

	m_strLightMapDirectShadow2ndForFieldMesh	= m_strLightMapDirectShadow1stForFieldMesh;
	m_strLightMapIndirectForFieldMesh			= m_strLightMapDirectShadow1stForFieldMesh;

#ifndef  NDEBUG
	static float s_fStartTime = 0.f;
	static float s_fEndTime = 0.f;

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Note : DxSingleMaterialMesh 내부에서 Version 체크가 일어난다.
	SingleTexMesh_Load_Insert( pd3dDevice, m_mmapMesh, SFile, bDynamicLoad, bPiece, pArchive );


#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "Static Load : %f", s_fEndTime );

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Tree 를 만들면서 그와 관련된 작업들을 정리한다.
	CreateTree_AddWork();

#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "MakeAABBTree : %f", s_fEndTime );
#endif
}

void DxStaticMaterialMesh::Load_101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	SFile >> m_vMax;
	SFile >> m_vMin;
	SFile >> m_wwLightMapUserUV_TEXEL.dwData;

#ifndef  NDEBUG
	static float s_fStartTime = 0.f;
	static float s_fEndTime = 0.f;

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Note : DxSingleMaterialMesh 내부에서 Version 체크가 일어난다.
	SingleTexMesh_Load_Insert( pd3dDevice, m_mmapMesh, SFile, bDynamicLoad, bPiece, pArchive );


#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "Static Load : %f", s_fEndTime );

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Tree 를 만들면서 그와 관련된 작업들을 정리한다.
	CreateTree_AddWork();

#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "MakeAABBTree : %f", s_fEndTime );
#endif
}

void DxStaticMaterialMesh::Load_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	SFile >> m_vMax;
	SFile >> m_vMin;
	SFile >> m_wwLightMapUserUV_TEXEL.dwData;

#ifndef  NDEBUG
	static float s_fStartTime = 0.f;
	static float s_fEndTime = 0.f;

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Note : DxSingleMaterialMesh 내부에서 Version 체크가 일어난다.
	SingleTexMesh_Load_Insert( pd3dDevice, m_mmapMesh, SFile, bDynamicLoad, bPiece, pArchive );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mmapMesh, SFile, bDynamicLoad, bPiece, pArchive );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mmapMesh, SFile, bDynamicLoad, bPiece, pArchive );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mmapMesh, SFile, bDynamicLoad, bPiece, pArchive );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mmapMesh, SFile, bDynamicLoad, bPiece, pArchive );


#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "Static Load : %f", s_fEndTime );

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Tree 를 만들면서 그와 관련된 작업들을 정리한다.
	CreateTree_AddWork();

#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "MakeAABBTree : %f", s_fEndTime );
#endif
}

BOOL DxStaticMaterialMesh::IsLightMapPiece()
{
    MMAPSINGLEMATERIALMESH mapMesh[1] = { m_mmapMesh };

    for ( DWORD i=0; i<1; ++i )
    {
        MMAPSINGLEMATERIALMESH_ITER iter = mapMesh[i].begin();
        for ( ; iter!=mapMesh[i].end(); ++iter )
        {
            if ( (*iter).second->IsLightMapPiece() )
                return TRUE;
        }
    }
    return FALSE;
}

void DxStaticMaterialMesh::SingleTexMesh_SaveLightMapPiece( MMAPSINGLEMATERIALMESH& mapMesh, sc::SerialFile& SFile )
{
	DWORD	dwSize = 0;

	dwSize = (DWORD)mapMesh.size();
	SFile << dwSize;

	MMAPSINGLEMATERIALMESH_ITER iter;
	for ( iter=mapMesh.begin(); iter!=mapMesh.end(); ++iter )
	{
		SFile << (*iter).first;
		(*iter).second->SaveLightMapPiece( SFile );
	}
}

void DxStaticMaterialMesh::SingleTexMesh_LoadLightMapPiece_Insert( LPDIRECT3DDEVICEQ pd3dDevice, MMAPSINGLEMATERIALMESH& mapMesh, sc::SerialFile& SFile )
{
	DWORD	dwSize = 0;

	std::string szName;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> szName;
		DxSingleMaterialMesh* pNew = new DxSingleMaterialMesh;
		pNew->LoadLightMapPiece( pd3dDevice, SFile );

        mapMesh.insert( std::make_pair ( szName, pNew ) );
	}
}

void DxStaticMaterialMesh::SaveLightMapPiece( sc::SerialFile& SFile )
{
    SFile << static_cast<DWORD>( VERSION_LM );

    SFile.BeginBlock();
    {
        // Note : DxSingleMaterialMesh 내부에서 Version 체크가 일어난다.
	    SingleTexMesh_SaveLightMapPiece( m_mmapMesh,			SFile );
    }
    SFile.EndBlock();
}

void DxStaticMaterialMesh::LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
    DWORD dwVer, dwBufferSize;
    SFile >> dwVer;
    SFile >> dwBufferSize;
    
	if ( VERSION_LM == dwVer)
	{
		// Note : DxSingleMaterialMesh 내부에서 Version 체크가 일어난다.
		SingleTexMesh_LoadLightMapPiece_Insert( pd3dDevice, m_mmapMesh, SFile );
	}
	else if ( 0x0100 == dwVer)
	{
		// Note : DxSingleMaterialMesh 내부에서 Version 체크가 일어난다.
		SingleTexMesh_LoadLightMapPiece_Insert( pd3dDevice, m_mmapMesh, SFile );
		SingleTexMesh_LoadLightMapPiece_Insert( pd3dDevice, m_mmapMesh, SFile );
		SingleTexMesh_LoadLightMapPiece_Insert( pd3dDevice, m_mmapMesh, SFile );
		SingleTexMesh_LoadLightMapPiece_Insert( pd3dDevice, m_mmapMesh, SFile );
		SingleTexMesh_LoadLightMapPiece_Insert( pd3dDevice, m_mmapMesh, SFile );
	}

	SetAABBMaxMin();
}

void DxStaticMaterialMesh::FlipLightMapRenderMan( DxStaticMeshBase* pStaticMeshBase )
{
	DxStaticMaterialMesh* pStaticMesh = MIN_BOOST::polymorphic_downcast<DxStaticMaterialMesh*>( pStaticMeshBase );

    MMAPSINGLEMATERIALMESH mapMeshSRC[1] = { pStaticMesh->m_mmapMesh };
    MMAPSINGLEMATERIALMESH mapMeshDEST[1] = { m_mmapMesh };

    // 데이터 이동
    for ( DWORD i=0; i<1; ++i )
    {
        if ( mapMeshDEST[i].empty() )
            continue;

        if ( mapMeshDEST[i].size() != mapMeshSRC[i].size() )
            continue;

        MMAPSINGLEMATERIALMESH_ITER iter_DEST = mapMeshDEST[i].begin();
        MMAPSINGLEMATERIALMESH_ITER iter_SRC = mapMeshSRC[i].begin();
        for ( ; iter_DEST!=mapMeshDEST[i].end(); ++iter_DEST, ++iter_SRC )
        {
            if ( (*iter_DEST).first != (*iter_SRC).first )
                continue;

            (*iter_DEST).second->FlipLightMapRenderMan( (*iter_SRC).second );
        }
    }

	SetAABBMaxMin();
}

void DxStaticMaterialMesh::CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice )
{
    MMAPSINGLEMATERIALMESH_ITER iter = m_mmapMesh.begin();
    for ( ; iter!=m_mmapMesh.end(); ++iter )
    {
        (*iter).second->CreateAlphaTexture32Bit( pd3dDevice );
    }
}

void DxStaticMaterialMesh::GetClosedPoint( D3DXVECTOR3& vCollisionInOut )
{
    MMAPSINGLEMATERIALMESH mapMesh_ARRAY[1] = { m_mmapMesh };

    float fDistance(FLT_MAX);

    D3DXVECTOR3 vBasePos(vCollisionInOut);

    for ( DWORD i=0; i<1; ++i )
    {
        if ( mapMesh_ARRAY[i].empty() )
            continue;

        MMAPSINGLEMATERIALMESH_ITER iter = mapMesh_ARRAY[i].begin();
        for ( ; iter!=mapMesh_ARRAY[i].end(); ++iter )
        {
            (*iter).second->GetClosedPoint( vBasePos, vCollisionInOut, fDistance );
        }
    }
}

void DxStaticMaterialMesh::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld )
{
	MMAPSINGLEMATERIALMESH* mapMesh_ARRAY[1] = { &m_mmapMesh };

	for ( DWORD i=0; i<1; ++i )
	{
		if ( mapMesh_ARRAY[i]->empty() )
			continue;

		MMAPSINGLEMATERIALMESH_ITER iter = mapMesh_ARRAY[i]->begin();
		for ( ; iter!=mapMesh_ARRAY[i]->end(); ++iter )
		{
			(*iter).second->ExportXFile( pxofsave, pDataObjectRoot, strPieceName, matWorld );
		}
	}
}

void DxStaticMaterialMesh::ConvertMAPtoLIST ( MMAPSINGLEMATERIALMESH& mapData, DxSingleMaterialMesh** ppList )
{
	MMAPSINGLEMATERIALMESH_ITER iter = mapData.begin();
	for ( ; iter!=mapData.end(); ++iter )
	{
		(*iter).second->m_pNext = (*ppList);
		(*ppList) = (*iter).second;
	}
}

void DxStaticMaterialMesh::Import( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, float fCellSize )
{
	CleanUp();

	if ( !pStaticMesh )
		return;

	//m_strLightMap = pStaticMesh->GetLightMapName();
	m_wwLightMapUserUV_TEXEL = pStaticMesh->GetLightMapUserUV_TEXEL();

	MAPSINGLETEXMESH* mapMesh_ARRAY_SRC[1] = { &pStaticMesh->m_mapMesh };

	for ( DWORD i=0; i<1; ++i )
	{
		MAPSINGLETEXMESH_ITER iter = mapMesh_ARRAY_SRC[i]->begin();
		for ( ; iter!=mapMesh_ARRAY_SRC[i]->end(); ++iter )
		{
			(*iter).second->Create_DxSingleMaterialMesh( pd3dDevice, m_mmapMesh, fCellSize );

			//DxSingleMaterialMesh* pNew = new DxSingleMaterialMesh;
			//pNew->Import( pd3dDevice, (*iter).second );

			//mapMesh_ARRAY[i]->insert( std::make_pair ( (*iter).first.m_strMaterialName, pNew ) );
		}
	}

	// Tree 를 만들면서 그와 관련된 작업들을 정리한다.
	CreateTree_AddWork();

	Active_MATERIAL_DATA( pd3dDevice, FALSE );

	SetAABBMaxMin();
}

void DxStaticMaterialMesh::SetLightMapName( const TSTRING& strLightMapName, const TSTRING& strAddName, DWORD dwTextureID )
{
	int nTexCount = dwTextureID;

	int n100 = nTexCount/100;
	int n10 = (nTexCount-n100*100) / 10;
	int n1 = nTexCount%10;
	TCHAR c100 = (TCHAR)(n100+48);
	TCHAR c10 = (TCHAR)(n10+48);
	TCHAR c1 = (TCHAR)(n1+48);

	m_strLightMapCombinedForFieldMesh = strLightMapName + strAddName + _T("_C_") + c100 + c10 + c1 + _T(".dds");

	// [CascadeShadow] 작업보류중
	m_strLightMapDirectShadow1stForFieldMesh = strLightMapName + strAddName + _T("_DS1_") + c100 + c10 + c1 + _T(".dds");
	m_strLightMapDirectShadow2ndForFieldMesh = strLightMapName + strAddName + _T("_DS2_") + c100 + c10 + c1 + _T(".dds");
	m_strLightMapIndirectForFieldMesh = strLightMapName + strAddName + _T("_I_") + c100 + c10 + c1 + _T(".dds");
}

BOOL DxStaticMaterialMesh::ChangeDxStaticMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, float fCellSize )
{
	CleanUp();

	if ( !pStaticMesh )
		return FALSE;

	//m_strLightMap = _T("");
	m_wwLightMapUserUV_TEXEL = pStaticMesh->GetLightMapUserUV_TEXEL();

	MAPSINGLETEXMESH* mapMesh_ARRAY_SRC[5] = { &pStaticMesh->m_mapMesh,
												&pStaticMesh->m_mapMeshALPHA,
												&pStaticMesh->m_mapMeshSOFTALPHA,
												&pStaticMesh->m_mapMeshSOFTALPHA01,
												&pStaticMesh->m_mapMeshSOFTALPHA02 };

	for ( DWORD i=0; i<5; ++i )
	{
		MAPSINGLETEXMESH_ITER iter = mapMesh_ARRAY_SRC[i]->begin();
		for ( ; iter!=mapMesh_ARRAY_SRC[i]->end(); ++iter )
		{
			DxSingleMaterialMesh* pNew = new DxSingleMaterialMesh;
			pNew->ChangeDxStaticMaterialMesh( pd3dDevice, (*iter).second, fCellSize );

			m_mmapMesh.insert( std::make_pair ( (*iter).first.m_strMaterialName, pNew ) );
		}
	}

	// Tree 를 만들면서 그와 관련된 작업들을 정리한다.
	CreateTree_AddWork();

	Active_MATERIAL_DATA( pd3dDevice, FALSE );

	SetAABBMaxMin ();

	return TRUE;
}

void DxStaticMaterialMesh::MergeLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
												  const TSTRING& strLightMapName, 
												  DxStaticMesh* pStaticMesh, 
												  DxPieceManagerPSF* pPieceManager, 
												  const D3DXMATRIX& matWorld, 
												  VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
												  DWORD& dwStartTextureSize,
												  WORDWORD& wwBaseTexelXY,
												  BOOL& bEnoughUsedPixelLightMap,
												  const WORDWORD& wwLightID4Shadow )
{
	DxSingleMaterialMesh* pListArray[1] = { m_pList };

	std::vector<BOOL>				vecReceiveShadowLM;
	std::vector<VERTEXNORCOLORTEX4>	vecVertices;
	std::vector<D3DXVECTOR2>		vecLightMapUV;
	for ( DWORD i=0; i<1; ++i )
	{
		DxSingleMaterialMesh* pCur = pListArray[i];
		while( pCur )
		{
			pCur->GetVertices( vecVertices, vecLightMapUV, vecReceiveShadowLM );
			pCur = pCur->m_pNext;
		}
	}

	if ( vecVertices.empty() )
		return;

	// 버텍스 정보를 얻는다.
	VERTEX_WORLD* pVerticesVERTEX = new VERTEX_WORLD[vecVertices.size()];
	for ( DWORD i=0; i<vecVertices.size(); ++i )
	{
		pVerticesVERTEX[i].vPos = vecVertices[i].vPos;
		pVerticesVERTEX[i].vNor = vecVertices[i].vNor;
		pVerticesVERTEX[i].dwColor = vecVertices[i].dwColor;
		pVerticesVERTEX[i].vTex[0] = vecVertices[i].vTex[0];
		pVerticesVERTEX[i].vTex[1] = vecVertices[i].vTex[1];
		pVerticesVERTEX[i].vTex[2] = vecVertices[i].vTex[2];
		pVerticesVERTEX[i].vTex[3] = vecVertices[i].vTex[3];
	}


	// LightMap 정보를 모으고 Color 도 계산함 
	{
		std::tr1::shared_ptr<SVEC_LIGHTMAPOBJ> spNew( std::tr1::shared_ptr<SVEC_LIGHTMAPOBJ>(new SVEC_LIGHTMAPOBJ) );
		spNew->m_dwLightMapFaceNUM = static_cast<DWORD>( vecVertices.size()/3 );

		NSLightMap::CreatePieceLightMapUserUV
		( 
			pd3dDevice,
			strLightMapName, 
			_T("_P"), 
			FALSE,
			TRUE,
			spNew->m_vecLightMapObject,
			spNew->m_dwLightMapFaceNUM, 
			reinterpret_cast<BYTE*>(pVerticesVERTEX),
			VERTEX_WORLD::FVF,
			pStaticMesh,
			pPieceManager, 
			matWorld,
			dwStartTextureSize,
			bEnoughUsedPixelLightMap,
			vecReceiveShadowLM,
			m_wwLightMapUserUV_TEXEL,
			vecLightMapUV,
			wwLightID4Shadow
		);

		vecsvecLightMapObj.push_back( spNew );
	}

	SAFE_DELETE_ARRAY( pVerticesVERTEX );

	//------- 최소 Base UV 를 구한다.
	wwBaseTexelXY = WORDWORD(4095,4095);
	for ( DWORD i=0; i<vecsvecLightMapObj.size(); ++i )
	{
		for ( DWORD j=0; j<vecsvecLightMapObj[i]->m_vecLightMapObject.size(); ++j )
		{
			LightMapObject* pLightMapObject = vecsvecLightMapObj[i]->m_vecLightMapObject[j].get();

			if ( wwBaseTexelXY.wA > pLightMapObject->GetBaseTexelXY().wA )
				wwBaseTexelXY.wA = pLightMapObject->GetBaseTexelXY().wA;
			if ( wwBaseTexelXY.wB > pLightMapObject->GetBaseTexelXY().wB )
				wwBaseTexelXY.wB = pLightMapObject->GetBaseTexelXY().wB;
		}
	}

	// 셋팅해 놓는다.
	for ( DWORD i=0; i<vecsvecLightMapObj.size(); ++i )
	{
		for ( DWORD j=0; j<vecsvecLightMapObj[i]->m_vecLightMapObject.size(); ++j )
		{
			LightMapObject* pLightMapObject = vecsvecLightMapObj[i]->m_vecLightMapObject[j].get();
			pLightMapObject->SetBaseTexelPieceXY( wwBaseTexelXY );
		}
	}
}

void DxStaticMaterialMesh::CreateLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
												   const TSTRING& strLightMapName,
												   TSTRING& strTextureCombined, 
												   TSTRING& strTextureDirect_Shadow_1,
												   TSTRING& strTextureDirect_Shadow_2,
												   TSTRING& strTextureIndirect,
												   VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
												   WORDWORD wwBaseTexelXY )
{
	if ( vecsvecLightMapObj.empty() )
		return;

	// NSLightMap 텍스쳐(m_vecTextureData) 에 기입한다.
	// 마지막 정리작업.
	NSLightMap::CreateLightMap2_PIECE( vecsvecLightMapObj[0]->m_vecLightMapObject, 
										strLightMapName, 
										_T("_P"), 
										FALSE,
										TRUE,
										strTextureCombined, 
										strTextureDirect_Shadow_1,
										strTextureDirect_Shadow_2,
										strTextureIndirect,
										wwBaseTexelXY,
										TRUE,
										TRUE );
}

void DxStaticMaterialMesh::CreateLightMapUVPieceObject( LPDIRECT3DDEVICEQ pd3dDevice, 
														const TSTRING& strLightMapName,
														TSTRING& strTextureCombined, 
														TSTRING& strTextureDirect_Shadow_1,
														TSTRING& strTextureDirect_Shadow_2,
														TSTRING& strTextureIndirect,
														VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
														WORDWORD wwBaseTexelXY,
														BOOL bRan_1_0, 
														float fCellSize )
{
	if ( vecsvecLightMapObj.empty() )
		return;

	DxSingleMaterialMesh* pListArray[1] = { m_pList };

	int nCount(0);
	for ( DWORD i=0; i<1; ++i )
	{
		DxSingleMaterialMesh* pCur = pListArray[i];
		while( pCur )
		{
			pCur->CreateLightMapUVPiece( pd3dDevice, 
										vecsvecLightMapObj[0]->m_vecLightMapObject[0].get(),
										wwBaseTexelXY,
										bRan_1_0,
										fCellSize );
			pCur = pCur->m_pNext;
		}
	}
}

void DxStaticMaterialMesh::LoadLightMapForFieldMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_textureResCombinedForFieldMesh = TextureManager::GetInstance().LoadTexture(
		m_strLightMapCombinedForFieldMesh,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		1,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_LAND,
		false,
		FALSE );

	m_textureResDirectShadow1stForFieldMesh = TextureManager::GetInstance().LoadTexture(
		m_strLightMapDirectShadow1stForFieldMesh,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		1,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_LAND,
		false,
		FALSE );

	m_textureResDirectShadow2ndForFieldMesh = TextureManager::GetInstance().LoadTexture(
		m_strLightMapDirectShadow2ndForFieldMesh,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		1,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_LAND,
		false,
		FALSE );

	m_textureResIndirectForFieldMesh = TextureManager::GetInstance().LoadTexture(
		m_strLightMapIndirectForFieldMesh,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		1,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_LAND,
		false,
		FALSE );
}

void DxStaticMaterialMesh::SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate )
{
	DxSingleMaterialMesh* pCur = m_pList;
	while( pCur )
	{
		pCur->SetLightMapUV_Rate( vLightMapUV_Rate );
		pCur = pCur->m_pNext;
	}
}

void DxStaticMaterialMesh::BakeRadiosityForPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
										 DxLandMan* pLandMan,
										 VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
										 const D3DXMATRIX& matWorld,
										 LPDIRECT3DTEXTUREQ pTextureSystem,
										 float fReflectance, 
										 float fFogBlend,
										 DWORD dwFogLength,
										 DWORD dwRadiosityIndex )
{
	NSLightMap::BakeRadiosityForPiece( pd3dDevice,
										pLandMan,
										vecsvecLightMapObj[0]->m_vecLightMapObject, 
										matWorld,
										pTextureSystem,
										fReflectance, 
										fFogBlend,
										dwFogLength,
										dwRadiosityIndex );
}

void DxStaticMaterialMesh::RenderOrderSettingPiece( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool, DWORD& dwTexType )
{
	dwTexType = m_dwTexType;

	Active_MATERIAL_DATA( pd3dDevice, bTool );

	//m_spRenderOrder->CleanUp();
	//DxSingleMaterialMesh* pCur = m_pList;
	//while( pCur )
	//{
	//	pCur->RenderOrderSetting( pd3dDevice, m_spRenderOrder.get(), dwTexType, bTool );
	//	pCur = pCur->m_pNext;
	//}
}

void DxStaticMaterialMesh::Render_PIECE_( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend )
{
	DWORD dwAddTexType(0L);
	m_spRenderOrder->SetTextureTypeForUnKnownTexture( sParameter.m_pd3dDevice, dwAddTexType );
	m_dwTexType |= dwAddTexType;

	if ( m_spRenderOrder->IsTextureTypeComplete() )
		m_dwTexType &= ~EMTTF_UNKNOWN;

	m_spRenderOrder->RenderPieceOpaque( sParameter, bMPShading, bBlend );
}

void DxStaticMaterialMesh::Render_PIECE_Alpha( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend )
{
	m_spRenderOrder->RenderPieceHardAlpha( sParameter, bMPShading, bBlend );
}

void DxStaticMaterialMesh::Render_PIECE_Decal( PieceParameter& sParameter )
{
	m_spRenderOrder->RenderPieceDecal( sParameter );
}

void DxStaticMaterialMesh::Render_PIECE_SoftAlpha( PieceParameter& sParameter, BOOL bMPShading )
{
	m_spRenderOrder->RenderPieceSoftAlpha( sParameter, bMPShading );
}

void DxStaticMaterialMesh::Render_PIECE_SoftAlphaForPSF( PieceParameter& sParameter )
{
	m_spRenderOrder->RenderPieceSoftAlphaForPSF( sParameter );
}

void DxStaticMaterialMesh::Render_PIECE_DecalPass2( PieceParameter& sParameter )
{
	m_spRenderOrder->RenderPieceDecalPass2( sParameter );
}

void DxStaticMaterialMesh::Render_PIECE_Custom_UNLIT_UNFOG( PieceParameter& sParameter )
{
	m_spRenderOrder->RenderPieceCustomUNLIT_UNFOG( sParameter.m_pd3dDevice, *sParameter.m_rmatWorld );
}

void DxStaticMaterialMesh::Render_PIECE_Water_CustomSoftAlpha( PieceParameter& sParameter )
{
	m_spRenderOrder->RenderPieceWaterCustomSoftAlpha( sParameter.m_pd3dDevice, *sParameter.m_rmatWorld );
}

void DxStaticMaterialMesh::Render_PIECE_Opaque_Instancing( const PieceParameter& sParameter )
{
	m_spRenderOrder->RenderPieceOpaque_Instancing( sParameter );
}

void DxStaticMaterialMesh::Render_PIECE_HardAlpha_Instancing( const PieceParameter& sParameter )
{
	m_spRenderOrder->RenderPieceHardAlpha_Instancing( sParameter );
}

void DxStaticMaterialMesh::Render_PIECE_Decal_Instancing( const PieceParameter& sParameter )
{
	m_spRenderOrder->RenderPieceDecal_Instancing( sParameter );
}

void DxStaticMaterialMesh::Render_PIECE_DecalPass2_Instancing( const PieceParameter& sParameter )
{
	m_spRenderOrder->RenderPieceDecalPass2_Instancing( sParameter );
}

void DxStaticMaterialMesh::Render_PIECE_CascadeShadowOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_spRenderOrder->RenderCascadeShadowOpaque( pd3dDevice, NSLandThread::TYPE_NO_FIELDMESH );
}

void DxStaticMaterialMesh::Render_PIECE_CascadeShadowHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_spRenderOrder->RenderCascadeShadowHardAlpha( pd3dDevice, NSLandThread::TYPE_NO_FIELDMESH );
}

void DxStaticMaterialMesh::Render_PIECE_CascadeShadowOpaque_Instancing( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_spRenderOrder->RenderCascadeShadowOpaque_Instancing( pd3dDevice );
}

void DxStaticMaterialMesh::Render_PIECE_CascadeShadowHardAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_spRenderOrder->RenderCascadeShadowHardAlpha_Instancing( pd3dDevice );
}

void DxStaticMaterialMesh::ClipStaticMesh( DxDecal* pDecal )
{
	MMAPSINGLEMATERIALMESH* mapMesh_ARRAY[1] = { &m_mmapMesh };

	for ( DWORD i=0; i<1; ++i )
	{
		MMAPSINGLEMATERIALMESH_ITER iter = mapMesh_ARRAY[i]->begin();
		for ( ; iter!=mapMesh_ARRAY[i]->end(); ++iter )
		{
			(*iter).second->ClipStaticMesh( pDecal );
		}
	}
}

BOOL DxStaticMaterialMesh::CheckOnlyVoxel()
{
	MMAPSINGLEMATERIALMESH_ITER iter = m_mmapMesh.begin();
	for ( ; iter!=m_mmapMesh.end(); ++iter )
	{
		if ( !(*iter).second->CheckOnlyVoxel() )
			return FALSE;
	}

	return TRUE;
}

void DxStaticMaterialMesh::Render_CharShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV )
{
	m_spRenderOrder->Render_CharShadow( pd3dDevice, pCV );

	//MMAPSINGLEMATERIALMESH* mapMesh_ARRAY[1] = { &m_mmapMesh };

	//for ( DWORD i=0; i<1; ++i )
	//{
	//	MMAPSINGLEMATERIALMESH_ITER iter = mapMesh_ARRAY[i]->begin();
	//	for ( ; iter!=mapMesh_ARRAY[i]->end(); ++iter )
	//	{
	//		(*iter).second->Render_CharShadow( pd3dDevice, pCV );
	//	}
	//}
}

// Piece의 TriangleLinkLight 를 생성하도록 한다.
BOOL DxStaticMaterialMesh::CreateTriangleLinkLightForPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
															 DxStaticMeshCollBase* pStaticMesh, 
															 DxPieceManager* pPieceManager, 
															 std::vector<DWORD>& vecLightID, 
															 const D3DXMATRIX& matWorld,
															 const D3DLIGHTQ& sLight,
															 float fAttEndLineValue ) const
{
	std::vector<VERTEXNORCOLORTEX4> vecVerticesS;
	std::vector<D3DXVECTOR2>		vecLightMapUV;
	std::vector<BOOL>				vecRceiveShadowLM;

	MMAPSINGLEMATERIALMESH_CITER iter = m_mmapMesh.begin();
	for ( ; iter!=m_mmapMesh.end(); ++iter )
	{
		vecVerticesS.clear();
		vecLightMapUV.clear();
		vecRceiveShadowLM.clear();

		(*iter).second->GetVertices( vecVerticesS, vecLightMapUV, vecRceiveShadowLM );

		BOOL bOK = CreateTriangleLinkLightForPieceDetail
		( 
			pd3dDevice, 
			pStaticMesh, 
			pPieceManager, 
			vecLightID, 
			matWorld,
			vecVerticesS,
			sLight,
			fAttEndLineValue
		);

		if ( bOK )
			return TRUE;
	}

	return FALSE;
}

void DxStaticMaterialMesh::Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool )
{
	m_spRenderOrder->CleanUp();

	//DWORD dwTexType(0L);

	DxSingleMaterialMesh* pCur = m_pList;
	while( pCur )
	{
		// 로딩 다시 시도.
		pCur->ThreadLoadingTexture();

		pCur->Active_MATERIAL_DATA( pd3dDevice, bTool );

		pCur->RenderOrderSetting( pd3dDevice, m_spRenderOrder.get(), m_dwTexType, bTool, FALSE );

		pCur = pCur->m_pNext;
	}
}

// Tree 를 만들면서 그와 관련된 작업들을 정리한다.
void DxStaticMaterialMesh::CreateTree_AddWork()
{
	// Tree를 만들기 위해 std::map 형식을 일반적인 list 형식으로 참조한다.
	ConvertMAPtoLIST( m_mmapMesh,			&m_pList );

	// SingleMehs Tree를 생성한다.
	COLLISION::MakeAABBTree( m_pMeshTree,	m_pList );
}

// 로그인 화면 같은 경우 다중으로 맵이 로딩이 되어 있고, 맵 전환이 되기 때문에 NSLandThread 정보값도 변경이 되야한다.
void DxStaticMaterialMesh::SetNSLandThread()
{
	DxSingleMaterialMesh* pCur = m_pList;
	while( pCur )
	{
		pCur->CreateDxCullingMeshSwap();

		pCur = pCur->m_pNext;
	}
}