#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Util.h"

#include "../../DxEffect/TexEff/DxTexEffSpecular.h"
#include "../../DxEffect/TexEff/DxTexEffDiffuse.h"
#include "../../DxEffect/TexEff/DxTexEffRotate.h"
#include "../../DxEffect/TexEff/DxTexEffFlowUV.h"
#include "../../DxEffect/TexEff/DxTexEffMan.h"
#include "../../DxEffect/TexEff/DxTexEff.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../../DxLand/ExportXFile/NSExportXFile.h"
#include "../../DxLand/LightMap/LightMapRenderMan.h"
#include "../../DxLand/LightMap/LightMapObjectVEC.h"
#include "../../DxLand/LightMap/LightMapBoxColor.h"
#include "../../DxLand/LightMap/LightMapObject.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../DxLand/LightMap/NSLightMap.h"
#include "../../DxLand/DxPiece/NSPieceQuickSort.h"
#include "../../DxLand/DxPiece/DxSingleTexMesh.h"
#include "../../DxLand/DxPiece/DxAABBOctree.h"
#include "../../DxLand/DxCullingMeshForThread.h"
#include "../../DxLand/NSLandThread.h"
#include "../../Common/profile.h"
#include "../../GlobalParam.h"

#include "../TriangleLinkLight/DxTriangleLinkLight_LightMap.h"
#include "../Texture/NSTexture16Bit.h"
#include "../Texture/NSTexCompress.h"
#include "../CubeMap/NSCubeMapCulling.h"
#include "../CubeMap/NSCubeMapUtil.h"
#include "../Light/NSShadowLight.h"
#include "../Light/DxShadowLight.h"
#include "../Light/DxLightMan.h"
#include "../TextureManager.h"
#include "../DxRenderStates.h"
#include "../DxEnvironment.h"
#include "../DxSurfaceTex.h"
#include "../RENDERPARAM.h"
#include "../DxViewPort.h"

#include "./NSShadowRenderForMaterial.h"
#include "./NSMaterialManager.h"
#include "./DxSingleMaterialMeshRenderOrder.h"
#include "./MaterialFieldDef.h"

#include "DxSingleMaterialMesh.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//											DxSingleMaterialMesh
// -----------------------------------------------------------------------------------------------------------------------------------------
#define MIPMAPLEVEL 3

DxSingleMaterialMesh::DxSingleMaterialMesh()
	: m_pLightMapRenderMan(NULL)
	, m_bCheckTex(FALSE)
	, m_bDefaultFX_EnableLightMap(TRUE)
	, m_pShadowMesh(NULL)
	, m_dwFlags(ENSTM_CAMCOLL)
	, m_emPS_3_0_Type_ForField(PS_3_0_TYPE_ZERO)
	//, m_emPS_3_0_Type_ForField_USED(PS_3_0_TYPE_USED_ZERO)
	, m_wwLightID_ForField(WORDWORD(-1,-1))
	, m_pNext(NULL)		// m_pNext 를 파괴 할 수는 없다.... Octree 만 파괴하라.
{
};

DxSingleMaterialMesh::~DxSingleMaterialMesh()
{
	CleanUp();

	m_dwFlags = ENSTM_CAMCOLL;
	m_pNext = NULL;
}

void DxSingleMaterialMesh::CleanUp()
{
	SAFE_DELETE( m_pLightMapRenderMan );
	SAFE_RELEASE ( m_pShadowMesh );
	m_wwLightID_ForField = WORDWORD(-1,-1);
}

void DxSingleMaterialMesh::CreateLightMap( const LPDIRECT3DDEVICEQ pd3dDevice, 
											const TCHAR* szMaterialName, 
											const TSTRING& strTextureName,
											const DWORD dwFaceNUM, 
											BYTE* pVertices,
											BOOL bCamColl,
											DWORD dwFVF,
											DWORD dwFlags,
											float fCellSize )
{
	CleanUp();

	m_dwFlags = dwFlags;
	m_strMaterialName = szMaterialName;
	m_strTextureName = strTextureName;

	LightMapRenderMan* pLightMapRenderMan = dynamic_cast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
	pLightMapRenderMan->Create
	( 
		pd3dDevice, 
		dwFaceNUM, 
		reinterpret_cast<BYTE*>(pVertices), 
		dwFVF, 
		EMLR_PIECE, 
		TRUE, 
		FALSE, 
		ABTT_MAT,
		fCellSize
	);

	//----------------------------------------------
	bool bEnableVertexColor(false);
	Import( pd3dDevice, pLightMapRenderMan, fCellSize, bEnableVertexColor );
	SAFE_DELETE( pLightMapRenderMan );

	if ( bEnableVertexColor )
	{
		m_dwFlags |= ENSTM_VERTEX_COLOR;
	}
}

void DxSingleMaterialMesh::GetVertexNum( DWORD& dwVertexNum )
{
	m_pLightMapRenderMan->GetVertexNum( dwVertexNum );
}

void DxSingleMaterialMesh::GetVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV, std::vector<BOOL>& vecReceiveShadowLM )
{
	// 메모리 단편화를 피하기 위한 작업.
	DWORD dwVertexNum(0);
	m_pLightMapRenderMan->GetVertexNum( dwVertexNum );
	if ( (dwVertexNum+vecVertices.size()) > vecVertices.capacity() )
	{
		vecVertices.reserve( vecVertices.size() + dwVertexNum );
	}
	if ( (dwVertexNum+vecLightMapUV.size()) > vecLightMapUV.capacity() )
	{
		vecLightMapUV.reserve( vecLightMapUV.size() + dwVertexNum );
	}
	if ( (dwVertexNum+vecReceiveShadowLM.size()) > vecReceiveShadowLM.capacity() )
	{
		vecReceiveShadowLM.reserve( vecReceiveShadowLM.size() + dwVertexNum );
	}

	// 메모리 단편화를 피하기 위한 작업.
	std::vector<D3DXVECTOR2> vecLightMapUVCur;
	vecLightMapUVCur.reserve( dwVertexNum );

	m_pLightMapRenderMan->GetVertices( vecVertices, vecLightMapUVCur );

	// 값이 있다면 RollBack 개념으로 되돌려주자.
	// 이렇게 하지 않으면 계속 Light를 구을시 User가 작업한 LightUV 변경된것을 사용하게 되어버린다.
	if ( !m_vecLightMapUV_BackUp.empty() )
	{
		for ( DWORD i=0; i<m_vecLightMapUV_BackUp.size(); ++i )
		{
			vecLightMapUV.push_back( m_vecLightMapUV_BackUp[i] );
		}
	}
	else
	{
		for ( DWORD i=0; i<vecLightMapUVCur.size(); ++i )
		{
			vecLightMapUV.push_back( vecLightMapUVCur[i] );
		}
	}

	//BOOL bDontReceive(FALSE);
	//if ( !NSMaterialManager::IsReceiveShadowLM( m_strMaterialName ) )
	//{
	//	bDontReceive = TRUE;
	//}

	//size_t nFaceNUM_NEW = vecVertices.size()/3;
	//for ( size_t i=nFaceNUM; i<nFaceNUM_NEW; ++i )
	//{
	//	vecReceiveShadowLM.push_back( bDontReceive );
	//}
}

void DxSingleMaterialMesh::CreateLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
													LightMapObject* pLightMapObject,
													WORDWORD wwBaseTexelXY,
													BOOL bRan_1_0,
													float fCellSize )
{
	// Texture 사이즈를 줄임.
	// UV 좌표 변경
	// DxSingleTexMesh 를 만듬.
	{
		// DxSingleTexMesh - LightMap 용으로 작업하면 될 것 같다.
		// LightMap 용 정보를 얻어온다.
		{
			// 메모리 단편화를 피하기 위한 작업.
			std::vector<VERTEXNORCOLORTEX4> vecVertices;
			std::vector<D3DXVECTOR2>		vecLightMapUV;
			DWORD dwVertexNum(0);
			m_pLightMapRenderMan->GetVertexNum( dwVertexNum );
			vecVertices.reserve( dwVertexNum );
			vecLightMapUV.reserve( dwVertexNum );

			// Vertex 얻어오기
			m_pLightMapRenderMan->GetVertices( vecVertices, vecLightMapUV );

			// LightMap 을 사용하는 녀석인지 아닌지 체크하는 코드.
			m_bDefaultFX_EnableLightMap = FALSE;
			for ( DWORD i=0; i<vecLightMapUV.size(); ++i )
			{
				if ( vecLightMapUV[i].x < -0.001f || vecLightMapUV[i].y > 0.001f )
				{
					m_bDefaultFX_EnableLightMap = TRUE;
					break;
				}
			}

			// 라이트맵을 안 만드는 녀석이라면 작업할 필요가 없다.
			if ( !m_bDefaultFX_EnableLightMap )
				return;

			// LightMapUV 를 백업하자.
			// 값이 있다면 RollBack 개념으로 되돌려주자.
			// 이렇게 하지 않으면 계속 Light를 구을시 User가 작업한 LightUV 변경된것을 사용하게 되어버린다.
			if ( m_vecLightMapUV_BackUp.empty() )
			{
				m_vecLightMapUV_BackUp = vecLightMapUV;
			}
			else
			{
				if ( vecLightMapUV.size() == m_vecLightMapUV_BackUp.size() )
				{
					vecLightMapUV = m_vecLightMapUV_BackUp;
				}
			}

			// UV 를 재계산할 필요없다.
			// Mesh가 다 만들어지면 추후 셋팅해주자.
			//{
			//	//------- UV 좌표를 재계산한다.
			//	{
			//		pLightMapObject->CaleUV( vecLightMapUV, 0, 0 );
			//		pLightMapObject->SubUV( vecLightMapUV, wwBaseTexelXY );
			//	}
			//}

			VERTEXNORCOLORTEX5* pVertices = new VERTEXNORCOLORTEX5[ vecVertices.size() ];
			for ( DWORD i=0; i<vecVertices.size(); ++i )
			{
				pVertices[i].vPos = vecVertices[i].vPos;
				pVertices[i].vNor = vecVertices[i].vNor;
				pVertices[i].dwColor = vecVertices[i].dwColor;
				pVertices[i].vTex[0] = vecVertices[i].vTex[0];
				pVertices[i].vTex[1] = vecVertices[i].vTex[1];
				pVertices[i].vTex[2] = vecVertices[i].vTex[2];
				pVertices[i].vTex[3] = vecVertices[i].vTex[3];

				if ( i < vecLightMapUV.size() )
				{
					pVertices[i].vTex[4] = vecLightMapUV[i];
				}
			}

			CreateLightMap
			( 
				pd3dDevice, 
				m_strMaterialName.c_str(), 
				m_strTextureName,
				static_cast<DWORD> (vecVertices.size()/3), 
				reinterpret_cast<BYTE*>( pVertices ), // 작업하자.
				FALSE,	//m_dwFlags & ENSTM_CAMCOLL,
				VERTEXNORCOLORTEX5::FVF,
				m_dwFlags,
				fCellSize
			);

			SAFE_DELETE_ARRAY( pVertices );
		}
	}
}

void DxSingleMaterialMesh::GetAABBSize ( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{ 
	if ( m_pLightMapRenderMan )
	{
		m_pLightMapRenderMan->GetAABBSize( vMax, vMin );
	}
	else
	{
		vMax = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		vMin = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
	}
}

HRESULT DxSingleMaterialMesh::Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool )
{
	if ( !m_pLightMapRenderMan )
		return S_OK;

	// 데칼인 경우에는 lightmaptest 가 렌더링 되지 않도록 한다.
	if ( m_dwFlags & ENSTM_DECAL_FULL )
	{
		switch( NSLightMapFX::g_emRenderType )
		{
		case NSLIGHTMAPDEF::LMRT_LIGHTMAP:
		case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
		case NSLIGHTMAPDEF::LMRT_CHECKER:
			{
				return S_OK;
			}
			break;
		}
	}

	// shadermodel 1.1 이하 버젼에서는 Decal_Before 가 나오지 않는다.
	if ( RENDERPARAM::g_emGroundQulity <= TnL_PS_1_1 )
	{
		// 고정 파이프라인에서는 Decal 이 렌더링 되지 않도록 한다.
		if ( RENDERPARAM::g_emGroundQulity <= TnL_FIXED )
		{
			if ( m_dwFlags & ENSTM_DECAL_FULL )
			{
				return S_OK;
			}
		}
		else if ( m_dwFlags & ENSTM_DECAL_BEFORE )
		{
			return S_OK;
		}
	}


	{
		NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
		if ( m_dwFlags & ENSTM_UNLIGHTMAP_AMBIENT_CUBE )
		{
			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_AMBIENTCUBE;
		}
		else if ( m_dwFlags & ENSTM_UNLIGHTMAP_VOXEL_COLOR )
		{
			// RAD 경우는 Voxel 에 렌더링을 하지 않는다.
			if ( NSPieceQuickSort::g_bBakeRAD )
				return S_OK;

			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
		}
		else
		{
			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_LIGHTMAP;
		}

		m_sMaterial_Data.Init();
		if ( bTool )
		{
			// Edit 에서만 따로 로딩 하도록 한다.
			ThreadLoadingTexture();

			m_sMaterial_Data.m_bGameMode = FALSE;
		}

		BOOL bDecal = (m_dwFlags & ENSTM_DECAL_FULL) ? TRUE : FALSE;
		BOOL bVertexColor = (m_dwFlags & ENSTM_VERTEX_COLOR) ? TRUE : FALSE;
		BOOL bCubeMap = (m_dwFlags & ENSTM_CUBEMAP) ? TRUE : FALSE;
		BOOL bNormalMap = (m_dwFlags & ENSTM_NORMALMAP) ? TRUE : FALSE;
		NSMaterialManager::Active_MATERIAL_DATA( pd3dDevice, 
												m_strMaterialName, 
												emUnLMRenderMode,
												m_sMaterial_Data,
												bDecal,
												bVertexColor,
												bCubeMap ? 1 : 0,
												bNormalMap );
	}

	return S_OK;
}

BOOL DxSingleMaterialMesh::RenderOrderSetting( LPDIRECT3DDEVICEQ pd3dDevice, DxSingleMaterialMeshRenderOrder* pRenderOrder, DWORD& dwTexType, BOOL bTool, BOOL bUnKnownTextureMode )
{
	if ( !m_pLightMapRenderMan )
		return TRUE;

	if ( !m_sMaterial_Data.m_rMaterial )
		return TRUE;

	if ( !m_sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX )
		return TRUE;

	// 데칼인 경우에는 lightmaptest 가 렌더링 되지 않도록 한다.
	if ( m_dwFlags & ENSTM_DECAL_FULL )
	{
		switch( NSLightMapFX::g_emRenderType )
		{
		case NSLIGHTMAPDEF::LMRT_LIGHTMAP:
		case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
		case NSLIGHTMAPDEF::LMRT_CHECKER:
			{
				return TRUE;
			}
			break;
		}
	}

	// shadermodel 1.1 이하 버젼에서는 Decal_Before 가 나오지 않는다.
	if ( RENDERPARAM::g_emGroundQulity <= TnL_PS_1_1 )
	{
		// 고정 파이프라인에서는 Decal 이 렌더링 되지 않도록 한다.
		if ( RENDERPARAM::g_emGroundQulity <= TnL_FIXED )
		{
			if ( m_dwFlags & ENSTM_DECAL_FULL )
			{
				return TRUE;
			}
		}
		else if ( m_dwFlags & ENSTM_DECAL_BEFORE )
		{
			return TRUE;
		}
	}

	{
		NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
		if ( m_dwFlags & ENSTM_UNLIGHTMAP_AMBIENT_CUBE )
		{
			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_AMBIENTCUBE;
		}
		else if ( m_dwFlags & ENSTM_UNLIGHTMAP_VOXEL_COLOR )
		{
			// RAD 경우는 Voxel 에 렌더링을 하지 않는다.
			if ( NSPieceQuickSort::g_bBakeRAD )
				return TRUE;

			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
		}
		else
		{
			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_LIGHTMAP;
		}

		if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_LIGHTMAP )
		{
			switch( NSLightMapFX::g_emRenderType )
			{
			case NSLIGHTMAPDEF::LMRT_LIGHTMAP:
			case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
			case NSLIGHTMAPDEF::LMRT_CHECKER:
				{
					Insert( pRenderOrder->m_mapLightMap );
					return TRUE;
				}
				break;

			default:
				break;
			};
		}
	}

	//// Decal 인지 먼저 확인한다.
	//int nDecalOrder(0);
	//int nDecalRenderType = NSMaterialManager::IsDecalRenderType( m_strMaterialName );
	//switch ( nDecalRenderType )
	//{
	//case -1:	// User가 셋팅한 Material 파일이 없다는 뜻으로 Tool 에서 셋팅한것으로 동작되도록 한다.
	{
		if ( m_dwFlags & ENSTM_DECAL_BEFORE )
		{
			dwTexType |= EMTTF_DECAL_BEFORE;

			DWORD dwSort;
			switch ( m_dwFlags&ENSTM_DECAL_ORDER_MAX )
			{
			case ENSTM_DECAL_ORDER_3:
				dwSort = 2;
				break;
			case ENSTM_DECAL_ORDER_2:
				dwSort = 1;
				break;
			case ENSTM_DECAL_ORDER_1:
			default:
				dwSort = 0;
				break;
			};

			if ( m_dwFlags & ENSTM_DECAL_ALPHABLEND )
			{
				Insert( pRenderOrder->m_mapDecalBlend[ dwSort ] );
			}
			else if ( m_dwFlags & ENSTM_DECAL_ALPHATEST )
			{
				Insert( pRenderOrder->m_mapDecalTest[ dwSort ] );
			}
			else if ( m_dwFlags & ENSTM_DECAL_MULTIPLY )
			{
				Insert( pRenderOrder->m_mapDecalMutiply[ dwSort ] );
			}
			
			return TRUE;
		}
		else if ( m_dwFlags & ENSTM_DECAL_AFTER )
		{
			dwTexType |= EMTTF_DECAL_AFTER;

			Insert( pRenderOrder->m_mapDecalPass2 );
			return TRUE;
		}
	}

	// 물인지 확인한다.
	if ( NSMaterialManager::IsWater( m_strMaterialName ) )
	{
		Insert( pRenderOrder->m_mapWater );
		return TRUE;
	}

	// 냇물인지 확인한다.
	if ( NSMaterialManager::IsCustomSoftAlpha( m_strMaterialName ) )
	{
		Insert( pRenderOrder->m_mapCustomSoftAlpha );
		return TRUE;
	}

	// UNLIT
	if ( NSMaterialManager::IsCustomUNLIT_UNFOG( m_strMaterialName ) )
	{
		Insert( pRenderOrder->m_mapCustomUNLIT_UNFOG );
		return TRUE;
	}

	// 알파 텍스쳐인지 불투명텍스쳐인지 셋팅한다.
	if ( m_spTextureRes.IsCompletedData() )
	{
		switch ( m_spTextureRes.GetAlphaType() )
		{
		case EMTT_ALPHA_HARD:
			Insert( pRenderOrder->m_mapHardAlpha );
			dwTexType |= EMTTF_ALPHA_HARD;
			break;
		case EMTT_ALPHA_SOFT:
			Insert( pRenderOrder->m_mapSoftAlpha00 );
			dwTexType |= EMTTF_ALPHA_SOFT;
			break;
		case EMTT_ALPHA_SOFT01:
			Insert( pRenderOrder->m_mapSoftAlpha01 );
			dwTexType |= EMTTF_ALPHA_SOFT;
			break;
		case EMTT_ALPHA_SOFT02:
			Insert( pRenderOrder->m_mapSoftAlpha02 );
			dwTexType |= EMTTF_ALPHA_SOFT;
			break;
		case EMTT_NORMAL:
		default:
			Insert( pRenderOrder->m_mapOpaque );
			dwTexType |= EMTTF_OPAQUE;
			break;
		};
	}
	else
	{
		if ( bUnKnownTextureMode )
		{
			return FALSE;
		}
		else
		{
			Insert( pRenderOrder->m_mapUnKnownTextureType );
			dwTexType |= EMTTF_UNKNOWN;
		}
	}

	return TRUE;
}

void DxSingleMaterialMesh::Insert( MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapMesh )
{
	//DWORD dwNumber = NSShadowLight::GetLightNumber();

	//switch ( m_emPS_3_0_Type_ForField )
	//{
	//case PS_3_0_TYPE_ZERO:
	//	m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_ZERO;
	//	break;

	//case PS_3_0_TYPE_CSM:
	//	m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_CSM;
	//	break;

	//case PS_3_0_TYPE_CSM_PL:
	//	{
	//		BOOL bUsedPL(FALSE);
	//		for ( DWORD z=0; z<dwNumber; ++z )
	//		{
	//			const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
	//			if ( pSpotLight )
	//			{
	//				if ( m_wwLightID_ForField.wB == pSpotLight->m_dwID )
	//				{
	//					bUsedPL = TRUE;
	//					break;
	//				}
	//			}
	//		}

	//		if ( bUsedPL )
	//		{
	//			m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_CSM_PL2_RT;
	//		}
	//		else
	//		{
	//			m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_CSM_PL2_ON;
	//		}
	//	}
	//	break;

	//case PS_3_0_TYPE_PL1:
	//	{
	//		BOOL bUsedPL(FALSE);
	//		for ( DWORD z=0; z<dwNumber; ++z )
	//		{
	//			const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
	//			if ( pSpotLight )
	//			{
	//				if ( m_wwLightID_ForField.wA == pSpotLight->m_dwID )
	//				{
	//					bUsedPL = TRUE;
	//					break;
	//				}
	//			}
	//		}

	//		if ( bUsedPL )
	//		{
	//			m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_PL1_RT;
	//		}
	//		else
	//		{
	//			m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_PL1_ON;
	//		}
	//	}
	//	break;

	//case PS_3_0_TYPE_PL2:
	//	{
	//		BOOL bUsedPL(FALSE);
	//		for ( DWORD z=0; z<dwNumber; ++z )
	//		{
	//			const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
	//			if ( pSpotLight )
	//			{
	//				if ( m_wwLightID_ForField.wB == pSpotLight->m_dwID )
	//				{
	//					bUsedPL = TRUE;
	//					break;
	//				}
	//			}
	//		}

	//		if ( bUsedPL )
	//		{
	//			m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_PL2_RT;
	//		}
	//		else
	//		{
	//			m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_PL2_ON;
	//		}
	//	}
	//	break;

	//case PS_3_0_TYPE_PL_PL:
	//	{
	//		BOOL bUsedPL(FALSE);
	//		BOOL bUsedPL2(FALSE);
	//		for ( DWORD z=0; z<dwNumber; ++z )
	//		{
	//			const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
	//			if ( pSpotLight )
	//			{
	//				if ( m_wwLightID_ForField.wA == pSpotLight->m_dwID )
	//				{
	//					bUsedPL = TRUE;
	//				}
	//				else if ( m_wwLightID_ForField.wB == pSpotLight->m_dwID )
	//				{
	//					bUsedPL2 = TRUE;
	//				}
	//			}
	//		}

	//		if ( bUsedPL && bUsedPL2 )
	//		{
	//			m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_PL1_RT_PL2_RT;
	//		}
	//		else if ( bUsedPL )
	//		{
	//			m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_PL1_RT_PL2_ON;
	//		}
	//		else if ( bUsedPL2 )
	//		{
	//			m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_PL1_ON_PL2_RT;
	//		}
	//		else
	//		{
	//			m_emPS_3_0_Type_ForField_USED = PS_3_0_TYPE_USED_PL1_ON_PL2_ON;
	//		}
	//	}
	//	break;
	//}

	// 알맞은 것을 넣어주도록 해야한다.
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = mapMesh.find( m_sMaterial_Data.m_rMaterial );
	if ( iter == mapMesh.end() )
	{
		// 새로운 것.
		MAP_TYPE_VEC_SINGLE_MATERIAL_MESH mapType_vecMesh;
		VEC_SINGLE_MATERIAL_MESH vecMesh;
		vecMesh.push_back( this );
		mapType_vecMesh.insert( std::make_pair( static_cast<DWORD>(m_emPS_3_0_Type_ForField), vecMesh ) );

		mapMesh.insert( std::make_pair( m_sMaterial_Data.m_rMaterial, mapType_vecMesh ) );
	}
	else
	{
		MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh = (*iter).second;
		MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.find ( static_cast<DWORD>(m_emPS_3_0_Type_ForField) );
		if ( iterA == mapType_vecMesh.end() )
		{
			// 새로운것
			VEC_SINGLE_MATERIAL_MESH vecMesh;
			vecMesh.push_back( this );
			mapType_vecMesh.insert( std::make_pair( static_cast<DWORD>(m_emPS_3_0_Type_ForField), vecMesh ) );			
		}
		else
		{
			// 있던거
			VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
			vecMesh.push_back( this );
		}
	}
}

void DxSingleMaterialMesh::Insert( MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH& mapMesh )
{
	MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH_ITER iter = mapMesh.find( m_sMaterial_Data.m_rMaterial );
	if ( iter == mapMesh.end() )
	{
		// 새로운 것.
		VEC_SINGLE_MATERIAL_MESH vecMesh;
		vecMesh.push_back( this );
		mapMesh.insert( std::make_pair( m_sMaterial_Data.m_rMaterial, vecMesh ) );
	}
	else
	{
		// 있던거
		(*iter).second.push_back( this );
	}
}

BOOL DxSingleMaterialMesh::IsRenderEnable_ChangeFX_Field( ID3DXEffect* pFX, 
														   BOOL bMPShading, 
														   TEXTURE_ALPHA emAlphaType, 
														   UINT unPass, 
														   BOOL& bChangePassOUT )
{
	return IsRenderEnable_ChangeFX( pFX, bMPShading, emAlphaType, unPass, bChangePassOUT, m_emPS_3_0_Type_ForField, m_wwLightID_ForField );
}

BOOL DxSingleMaterialMesh::IsRenderEnable_ChangeFX( ID3DXEffect* pFX, 
													BOOL bMPShading, 
													TEXTURE_ALPHA emAlphaType, 
													UINT unPass, 
													BOOL& bChangePassOUT,
													PS_3_0_TYPE emPS_3_0_Type,
													WORDWORD wwLightID )
{
	int nNewPass(unPass);
	if ( bMPShading )
	{
		nNewPass = NSShadowLightUtil::GetPassFX_PS_3_0_TYPE_USED_MPShading( emPS_3_0_Type, wwLightID );
		if ( nNewPass == -1 )
			return FALSE;		// 이건 렌더링 할 필요가 없다.

		if ( nNewPass != unPass )
		{
			bChangePassOUT = TRUE;
			if ( emAlphaType == EMTT_ALPHA_HARD || emAlphaType == EMTT_ALPHA_SOFT )
			{
				pFX->EndPass();
				pFX->BeginPass( nNewPass + 3 );
			}
			else
			{
				pFX->EndPass();
				pFX->BeginPass( nNewPass );
			}
		}
	}
	else
	{
		if ( unPass != 0 )
		{
			bChangePassOUT = TRUE;
			nNewPass = NSShadowLightUtil::GetPassFX_PS_3_0_TYPE_USED( emPS_3_0_Type, wwLightID );
			if ( nNewPass != unPass )
			{
				if ( emAlphaType == EMTT_ALPHA_SOFT )
				{
					pFX->EndPass();
					pFX->BeginPass( nNewPass + 4 );
				}
				else
				{
					pFX->EndPass();
					pFX->BeginPass( nNewPass );
				}
			}
		}
	}

	return TRUE;
}

HRESULT DxSingleMaterialMesh::RenderThreadLoad( const LPDIRECT3DDEVICEQ pd3dDevice, 
											   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
											   ID3DXEffect* pFX )
{
	if ( !IsExistCullingData( emCullingRenderType ) )
		return S_OK;

	NSMaterialManager::SetFX_Parameter( m_sMaterial_Data, pFX );

	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
	{
		pFX->SetFloat( "g_fSpecularPower", m_sMaterialValue.m_fSpotSpecPower );
		pFX->SetFloat( "g_fSpecularIntensity", m_sMaterialValue.m_fSpotSpecMulti );

		//if ( m_emPS_3_0_Type_ForField_USED != PS_3_0_TYPE_ZERO )
		if (  m_emPS_3_0_Type_ForField != PS_3_0_TYPE_ZERO && 
			 m_emPS_3_0_Type_ForField != PS_3_0_TYPE_CSM )
		{
			D3DXMATRIX matIdentity;
			D3DXMatrixIdentity( &matIdentity );
			NSMaterialManager::SetFX_SpotLight( m_wwLightID_ForField, matIdentity, pFX );
		}
	}


	if ( m_dwFlags & ENSTM_CUBEMAP )
	{
		if ( NSPieceQuickSort::g_bBakeRAD )
		{
			// RAD 작업시에는 CubeMap 이 작동하지 않도록 한다.
			pFX->SetFloat( "g_fCubeMapValue", 0.f );
		}
		else
		{
			pFX->SetFloat( "g_fCubeMapValue", m_sMaterialValue.m_fCubeMapValue );
		}
	}
	else
	{
		pFX->SetFloat( "g_fCubeMapValue", 0.f );
	}

	if ( m_dwFlags & ENSTM_DECAL_BEFORE )
	{
		if ( m_dwFlags & ENSTM_DECAL_NORMAL )
		{
			pFX->SetFloat( "g_fDecalNormal", 1.f );
		}
		else
		{
			pFX->SetFloat( "g_fDecalNormal", 0.f );
		}
	}

	int nTexType(0);
	DWORD dwAlphaTestEnable(0L);
	DWORD dwAlphaBlendEnable(0L);
	if ( m_sMaterial_Data.m_bDefaultMaterial )
	{
		//ThreadLoadingTexture();

		LPDIRECT3DTEXTUREQ pTexture = m_spTextureRes.GetTexture();
		pFX->SetTexture( "g_BaseTexture", pTexture );

		if ( m_dwFlags & ENSTM_MASK_MAP )
		{
			pTexture = m_spMaskTexRes.GetTexture();
			pFX->SetTexture( "g_MaskTexture", pTexture );
		}
		else
		{
			pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
		}
	}

	// FX 마무리
	pFX->CommitChanges();

	// 내부에서 m_sMaterial_Data.m_rMaterial->m_pFX 데이터를 사용하지 않도록 한다.
	//m_pLightMapRenderMan->RenderMaterial( pd3dDevice, &sCV, m_sMaterial_Data );
	m_spCullingMeshSwap[emCullingRenderType]->RenderMaterial( pd3dDevice, m_sMaterial_Data );

	return S_OK;
}

HRESULT DxSingleMaterialMesh::RenderStaticWater( const LPDIRECT3DDEVICEQ pd3dDevice, 
												   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
												   float fPrevHeight, 
												   float &fHeight,
												   float &fNearLength )
{
	if ( !IsExistCullingData( emCullingRenderType ) )
		return S_OK;

	LPD3DXEFFECT pFX = m_sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX;

	{
		NSMaterialManager::SetFX_Parameter( m_sMaterial_Data, pFX );

		pd3dDevice->SetVertexDeclaration( m_sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pDCRT );

		D3DXVECTOR3 vMax;
		D3DXVECTOR3 vMin;
		GetAABBSize ( vMax, vMin );
		D3DXVECTOR3 vCenter = (vMax + vMin) * 0.5f;

		UINT unPass(0);
		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && DxEnvironment::GetInstance().IsRealTimeReflection() )
		{
			if ( (fPrevHeight + 0.01f) >= vCenter.y && fPrevHeight <= (vCenter.y + 0.01f) )	// 작은 차이는 인정해 주자.
			{
				unPass = 1;
				pFX->SetTexture( "g_ReflectTexRT", DxSurfaceTex::GetInstance().m_pReflectTex );
			}
		}

		UINT numPasses;
		pFX->Begin( &numPasses, 0 );
		if( unPass < numPasses )
		{
			pFX->BeginPass( unPass );
			{
				// FX 마무리
				pFX->CommitChanges();

				// 내부에서 m_sMaterial_Data.m_rMaterial->m_pFX 데이터를 사용하지 않도록 한다.
				//m_pLightMapRenderMan->RenderMaterialWater( pd3dDevice, pCV, m_sMaterial_Data, fHeight, fNearLength );
				m_spCullingMeshSwap[emCullingRenderType]->RenderMaterialWater( pd3dDevice, m_sMaterial_Data, fHeight, fNearLength );
			}
			pFX->EndPass();
		}
		pFX->End();
	}

	return S_OK;
}

HRESULT DxSingleMaterialMesh::RenderCommonCustomMaterial( const LPDIRECT3DDEVICEQ pd3dDevice, 
															NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	if ( emCullingRenderType != NSLandThread::TYPE_NO_FIELDMESH )
	{
		if ( !IsExistCullingData( emCullingRenderType ) )
			return S_OK;
	}	

	LPD3DXEFFECT pFX = m_sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX;

	{
		NSMaterialManager::SetFX_Parameter( m_sMaterial_Data, pFX );

		pd3dDevice->SetVertexDeclaration( m_sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pDCRT );

		UINT unPass(0);
		UINT numPasses;
		pFX->Begin( &numPasses, 0 );
		if( unPass < numPasses )
		{
			pFX->BeginPass( unPass );
			{
				// FX 마무리
				pFX->CommitChanges();

				// 내부에서 m_sMaterial_Data.m_rMaterial->m_pFX 데이터를 사용하지 않도록 한다.
				//m_pLightMapRenderMan->RenderMaterial( pd3dDevice, pCV, m_sMaterial_Data );
				if ( emCullingRenderType == NSLandThread::TYPE_NO_FIELDMESH )
				{
					m_pLightMapRenderMan->RenderMaterial( pd3dDevice, NULL, m_sMaterial_Data );
				}
				else
				{
					m_spCullingMeshSwap[emCullingRenderType]->RenderMaterial( pd3dDevice, m_sMaterial_Data );
				}
			}
			pFX->EndPass();
		}
		pFX->End();
	}

	return S_OK;
}

HRESULT DxSingleMaterialMesh::RenderDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice,
											 NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
											 const CLIPVOLUME &sCV, 
											 sc::SerialFile* const SFile )
{
	if ( !m_pLightMapRenderMan )	return S_OK;

	// DynaminLoad
	m_pLightMapRenderMan->DynamicLoad( pd3dDevice, sCV, SFile );

	return S_OK;
}

void DxSingleMaterialMesh::RenderPiece( PieceParameter& sParameter, ID3DXEffect* pFX )
{
	LPDIRECT3DDEVICEQ pd3dDevice = sParameter.m_pd3dDevice;
	const LightMapBoxColorData* pLMBCData = &sParameter.m_sLMBCData;

	NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
	if ( !pLMBCData )
	{
		emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_NULL;
	}
	else if ( m_dwFlags & ENSTM_UNLIGHTMAP_AMBIENT_CUBE )
	{
		emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_AMBIENTCUBE;
	}
	else if ( m_dwFlags & ENSTM_UNLIGHTMAP_VOXEL_COLOR )
	{
		emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
	}
	else
	{
		emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_LIGHTMAP;
	}

	{
		NSMaterialManager::SetFX_Parameter( m_sMaterial_Data, pFX );

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
		{
			//if ( sParameter.m_pTriangleLinkLight->m_emPS_3_0_Type_USED != PS_3_0_TYPE_ZERO )
			if ( sParameter.m_pTriangleLinkLight->m_emPS_3_0_Type != PS_3_0_TYPE_ZERO && 
				sParameter.m_pTriangleLinkLight->m_emPS_3_0_Type != PS_3_0_TYPE_CSM )
			{
				NSMaterialManager::SetFX_SpotLight( sParameter.m_pTriangleLinkLight->m_wwLightID, *sParameter.m_rmatWorld, pFX );
			}

			pFX->SetFloat( "g_fSpecularPower", m_sMaterialValue.m_fSpotSpecPower );
			pFX->SetFloat( "g_fSpecularIntensity", m_sMaterialValue.m_fSpotSpecMulti );	

			if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_AMBIENTCUBE )
			{
				pFX->SetVectorArray( "g_vAmbientCube", pLMBCData->m_pAmbientCube, LightMapBoxColor::TEST_COUNT );
				pFX->SetFloatArray( "g_arrayfAmbientCubePL1", pLMBCData->m_fAmbientCubeSpot1, LightMapBoxColor::TEST_COUNT );
				pFX->SetFloatArray( "g_arrayfAmbientCubePL2", pLMBCData->m_fAmbientCubeSpot2, LightMapBoxColor::TEST_COUNT );
			}
			else if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_VOXELCOLOR )
			{
				pFX->SetFloat( "g_fVoxel", 1.f );
				pFX->SetVector( "g_vVoxelColor", pLMBCData->m_pVoxelColor );
				pFX->SetFloat( "g_fSpotShadowLight1_VoxelColorPower", pLMBCData->m_pSpot1_Spot2->x );
				pFX->SetFloat( "g_fSpotShadowLight2_VoxelColorPower", pLMBCData->m_pSpot1_Spot2->y );
			}
		}
		else
		{
			if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_AMBIENTCUBE )
			{
				pFX->SetVectorArray( "g_vAmbientCube", pLMBCData->m_pAmbientCube, LightMapBoxColor::TEST_COUNT );
			}
			else if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_VOXELCOLOR )
			{
				pFX->SetFloat( "g_fVoxel", 1.f );
				pFX->SetVector( "g_vVoxelColor", pLMBCData->m_pVoxelColor );
			}
		}

		if ( m_dwFlags & ENSTM_CUBEMAP )
		{
			if ( NSPieceQuickSort::g_bBakeRAD )
			{
				// RAD 작업시에는 CubeMap 이 작동하지 않도록 한다.
				pFX->SetFloat( "g_fCubeMapValue", 0.f );
			}
			else
			{
				pFX->SetFloat( "g_fCubeMapValue", m_sMaterialValue.m_fCubeMapValue );
			}
		}
		else
		{
			pFX->SetFloat( "g_fCubeMapValue", 0.f );
		}

		if ( m_dwFlags & ENSTM_DECAL_BEFORE )
		{
			if ( m_dwFlags & ENSTM_DECAL_NORMAL )
			{
				pFX->SetFloat( "g_fDecalNormal", 1.f );
			}
			else
			{
				pFX->SetFloat( "g_fDecalNormal", 0.f );
			}
		}

		int nTexType(0);
		DWORD dwAlphaTestEnable(0L);
		DWORD dwAlphaBlendEnable(0L);
		if ( m_sMaterial_Data.m_bDefaultMaterial )
		{
			// Note : Texture Loading
			//ThreadLoadingTexture();

			LPDIRECT3DTEXTUREQ pTexture = m_spTextureRes.GetTexture();
			pFX->SetTexture( "g_BaseTexture", pTexture );

			if ( m_dwFlags & ENSTM_MASK_MAP )
			{
				pTexture = m_spMaskTexRes.GetTexture();
				pFX->SetTexture( "g_MaskTexture", pTexture );
			}
			else
			{
				pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
			}
		}

		//if ( sParameter.m_nRenderMode == NSMaterialManager::EMRM_TOOL )
		//{
		//	m_sMaterial_Data.m_bGameMode = FALSE;
		//	pd3dDevice->SetVertexDeclaration( m_sMaterial_Data.m_rMaterial->m_sMaterialTOOL.m_pDCRT );
		//}
		//else
		//{
		//	m_sMaterial_Data.m_bGameMode = TRUE;
		//	pd3dDevice->SetVertexDeclaration( m_sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pDCRT );
		//}

		// FX 마무리
		pFX->CommitChanges();

		// 내부에서 m_sMaterial_Data.m_rMaterial->m_pFX 데이터를 사용하지 않도록 한다.
		m_pLightMapRenderMan->RenderMaterial( pd3dDevice, NULL, m_sMaterial_Data );
	}
}

void DxSingleMaterialMesh::RenderPiece_Instancing( const PieceParameter& sParameter, ID3DXEffect* pFX )
{
	LPDIRECT3DDEVICEQ pd3dDevice = sParameter.m_pd3dDevice;
	const LightMapBoxColorData* pLMBCData = &sParameter.m_sLMBCData;

	NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
	if ( !pLMBCData )
	{
		emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_NULL;
	}
	else if ( m_dwFlags & ENSTM_UNLIGHTMAP_AMBIENT_CUBE )
	{
		emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_AMBIENTCUBE;
	}
	else if ( m_dwFlags & ENSTM_UNLIGHTMAP_VOXEL_COLOR )
	{
		emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
	}
	else
	{
		emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_LIGHTMAP;
	}

	{
		NSMaterialManager::SetFX_Parameter( m_sMaterial_Data, pFX );

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
		{
			pFX->SetFloat( "g_fSpecularPower", m_sMaterialValue.m_fSpotSpecPower );
			pFX->SetFloat( "g_fSpecularIntensity", m_sMaterialValue.m_fSpotSpecMulti );	
		}
		
		if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_VOXELCOLOR )
		{
			pFX->SetFloat( "g_fVoxel", 1.f );
		}

		if ( m_dwFlags & ENSTM_CUBEMAP )
		{
			if ( NSPieceQuickSort::g_bBakeRAD )
			{
				// RAD 작업시에는 CubeMap 이 작동하지 않도록 한다.
				pFX->SetFloat( "g_fCubeMapValue", 0.f );
			}
			else
			{
				pFX->SetFloat( "g_fCubeMapValue", m_sMaterialValue.m_fCubeMapValue );
			}
		}
		else
		{
			pFX->SetFloat( "g_fCubeMapValue", 0.f );
		}

		if ( m_dwFlags & ENSTM_DECAL_BEFORE )
		{
			if ( m_dwFlags & ENSTM_DECAL_NORMAL )
			{
				pFX->SetFloat( "g_fDecalNormal", 1.f );
			}
			else
			{
				pFX->SetFloat( "g_fDecalNormal", 0.f );
			}
		}

		int nTexType(0);
		DWORD dwAlphaTestEnable(0L);
		DWORD dwAlphaBlendEnable(0L);
		if ( m_sMaterial_Data.m_bDefaultMaterial )
		{
			// Note : Texture Loading
			//ThreadLoadingTexture();

			LPDIRECT3DTEXTUREQ pTexture = m_spTextureRes.GetTexture();
			pFX->SetTexture( "g_BaseTexture", pTexture );

			if ( m_dwFlags & ENSTM_MASK_MAP )
			{
				pTexture = m_spMaskTexRes.GetTexture();
				pFX->SetTexture( "g_MaskTexture", pTexture );
			}
			else
			{
				pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
			}
		}

		// FX 마무리
		//pFX->CommitChanges(); 더 안쪽에서 호출된다.

		// 내부에서 m_sMaterial_Data.m_rMaterial->m_pFX 데이터를 사용하지 않도록 한다.
		m_pLightMapRenderMan->RenderPieceMaterial_Instancing( pd3dDevice, pFX, m_sMaterial_Data );
	}
}

void DxSingleMaterialMesh::RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	// Decal 은 TnL 렌더 안하도록 한다.
	if ( m_dwFlags & ENSTM_DECAL_FULL )
		return;

	// Note : Texture Loading
	//ThreadLoadingTexture();

	//int nTexType(0);
	//DWORD dwAlphaTestEnable(0L);
	//DWORD dwAlphaBlendEnable(0L);
	LPDIRECT3DTEXTUREQ pTexture = m_spTextureRes.GetTexture();
	//if ( pTexture )
	//{
	//	const D3DXIMAGE_INFO& sInfo = m_spTextureRes.GetInfo();
	//	if ( sInfo.Format == D3DFMT_DXT3 )
	//	{
	//		nTexType = 1;

	//		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );
	//		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
	//	}
	//	else if ( sInfo.Format == D3DFMT_DXT5 )
	//	{
	//		nTexType = 2;

	//		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	//		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	//		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	//		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	//	}
	//}

	// 텍스쳐가 없을 경우는, NSMaterialManager::Active 에서 설정된 Texture 를 사용한다.
	{
		if ( pTexture )
		{
			pd3dDevice->SetTexture( 0, pTexture );
		}

	//	DWORD dwNormalizeNormals(FALSE);
	//	pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
	//	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

		// 현재 pCV 가 있는 것은 Field 없는 것은 Piece 이다.
		// Field 쪽은 Thread 로 Culling 계산된 걸 사용하고,
		// Piece 는 원본 그대로를 전부 렌더링 한다.
		if ( emCullingRenderType == NSLandThread::TYPE_NO_FIELDMESH )
		{
			m_pLightMapRenderMan->RenderMaterialTnL( pd3dDevice, NULL );
		}
		else
		{
			m_spCullingMeshSwap[emCullingRenderType]->RenderMaterialTnL( pd3dDevice );
		}

	//	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );
	}

	//if ( nTexType == 1 )
	//{
	//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
	//}
	//else if ( nTexType == 2 )
	//{
	//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	//}
}

void DxSingleMaterialMesh::ThreadLoadingTexture()
{
	// Note : Texture Loading
	if( !m_spTextureRes.IsValid() )
	{
		if( !m_bCheckTex )
		{
			if ( !m_strTextureName.empty() )
			{
				m_spTextureRes = TextureManager::GetInstance().LoadTexture(
					m_strTextureName,
					false,
					TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
					0,
					D3DFMT_UNKNOWN,
					TEXTURE_RESIZE_NONE,
					TEXTURE_LAND,
					true,
					FALSE );
			}

			if ( m_dwFlags & ENSTM_MASK_MAP )
			{
				TSTRING strMaskTexName = sc::util::GetSpecularName( m_strTextureName, _T("_mask") );

				m_spMaskTexRes = TextureManager::GetInstance().LoadTexture(
					strMaskTexName,
					false,
					TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
					0,
					D3DFMT_UNKNOWN,
					TEXTURE_RESIZE_NONE,
					TEXTURE_LAND,
					true,
					FALSE );
			}

			m_bCheckTex = TRUE;
		}
	}
}

void DxSingleMaterialMesh::RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bAlphaTex )
{
	// Cast Shadow 가 아니라면 렌더할 필요가 없다.
	if ( m_dwFlags & ENSTM_DONT_CAST_SHADOW_LM )
		return;

	if ( bAlphaTex )
	{
		if ( m_sMaterial_Data.m_bDefaultMaterial )
		{
			//ThreadLoadingTexture();

			LPDIRECT3DTEXTUREQ pTexture = m_spTextureRes.GetTexture();
			NSShadowRenderForMaterial::SetTexture( pTexture );
		}
		else
		{
			if ( m_sMaterial_Data.m_rMaterial->m_vecTextures.size() > 0 )
			{
				if ( m_sMaterial_Data.m_rMaterial->m_vecTextures[0].m_textureRes.Update() )
				{
					NSShadowRenderForMaterial::SetTexture( m_sMaterial_Data.m_rMaterial->m_vecTextures[0].m_textureRes.GetTexture() );
				}
			}
		}

		NSShadowRenderForMaterial::CommitChanges();
	}

	//m_pLightMapRenderMan->RenderMaterial_OtherFX( pd3dDevice, m_sMaterial_Data );
	//m_pLightMapRenderMan->RenderCascadeShadow( pd3dDevice, pCV, bAlphaTex );
	if ( emCullingRenderType == NSLandThread::TYPE_NO_FIELDMESH )
	{
		m_pLightMapRenderMan->RenderCascadeShadow( pd3dDevice, NULL, bAlphaTex );
	}
	else
	{
		m_spCullingMeshSwap[emCullingRenderType]->RenderCascadeShadow( pd3dDevice, bAlphaTex );
	}
	



	//m_sMaterial_Data.m_bGameMode = TRUE;
	//pd3dDevice->SetVertexDeclaration( m_sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pDCRT );

	//if ( bAlphaTex )
	//{
	//	if ( m_sMaterial_Data.m_bDefaultMaterial )
	//	{
	//		ThreadLoadingTexture();

	//		LPDIRECT3DTEXTUREQ pTexture = m_spTextureRes.GetTexture();
	//		NSShadowRenderForMaterial::SetTexture( pTexture );
	//	}
	//	else
	//	{
	//		for ( DWORD i=0; i<m_sMaterial_Data.m_rMaterial->m_vecTextures.size(); ++i )
	//		{
	//			if ( m_sMaterial_Data.m_rMaterial->m_vecTextures[i].m_textureRes.Update() )
	//			{
	//				NSShadowRenderForMaterial::SetTexture( m_sMaterial_Data.m_rMaterial->m_vecTextures[i].m_textureRes.GetTexture() );
	//				break;
	//			}
	//		}
	//	}
	//}
	//else
	//{
	//	NSShadowRenderForMaterial::SetTexture( NSMaterialManager::GetWhiteTexture() );
	//}

	//NSShadowRenderForMaterial::CommitChanges();
}

void DxSingleMaterialMesh::RenderCascadeShadow_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex )
{
	// Cast Shadow 가 아니라면 렌더할 필요가 없다.
	if ( m_dwFlags & ENSTM_DONT_CAST_SHADOW_LM )
		return;

	if ( bAlphaTex )
	{
		if ( m_sMaterial_Data.m_bDefaultMaterial )
		{
			//ThreadLoadingTexture();

			LPDIRECT3DTEXTUREQ pTexture = m_spTextureRes.GetTexture();
			NSShadowRenderForMaterial::SetTexture( pTexture );
		}
		else
		{
			if ( m_sMaterial_Data.m_rMaterial->m_vecTextures.size() > 0 )
			{
				if ( m_sMaterial_Data.m_rMaterial->m_vecTextures[0].m_textureRes.Update() )
				{
					NSShadowRenderForMaterial::SetTexture( m_sMaterial_Data.m_rMaterial->m_vecTextures[0].m_textureRes.GetTexture() );
				}
			}
		}
	}

	//m_pLightMapRenderMan->RenderMaterial_OtherFX( pd3dDevice, m_sMaterial_Data );
	m_pLightMapRenderMan->RenderCascadeShadow_Instancing( pd3dDevice, bAlphaTex );
}

void DxSingleMaterialMesh::RenderFixedCV( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV )
{
	// Decal 은 TnL 렌더 안하도록 한다.
	if ( m_dwFlags & ENSTM_DECAL_FULL )
		return;

	LPDIRECT3DTEXTUREQ pTexture = m_spTextureRes.GetTexture();
	if ( pTexture )
	{
		pd3dDevice->SetTexture( 0, pTexture );
	}

	m_pLightMapRenderMan->RenderMaterialTnL( pd3dDevice, pCV );
}

HRESULT DxSingleMaterialMesh::Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	if ( !m_pLightMapRenderMan )
		return S_OK;

	if ( !m_spTextureRes.IsValid() )
		return S_OK;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	BOOL bAlphaTex(FALSE);
	if ( m_spTextureRes.GetAlphaType() == EMTT_ALPHA_HARD )
		bAlphaTex = TRUE;

	m_pLightMapRenderMan->Render_Query( pd3dDevice, sCV, bAlphaTex );

	return S_OK;
}

VOID DxSingleMaterialMesh::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if (m_pLightMapRenderMan)
		m_pLightMapRenderMan->CreateQuery( pd3dDevice );
}

VOID DxSingleMaterialMesh::DestroyQuery()
{
	if (m_pLightMapRenderMan)
		m_pLightMapRenderMan->DestroyQuery();
}

void DxSingleMaterialMesh::ResetOcclusionQuery()
{
	if (m_pLightMapRenderMan)
		m_pLightMapRenderMan->ResetOcclusionQuery();
}

void DxSingleMaterialMesh::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex )
{
	if (m_pLightMapRenderMan)
		m_pLightMapRenderMan->FindOcclusionRenderIndex( pd3dDevice, sCV, vecOctreeIndex );
}

//void DxSingleMaterialMesh::InsertColorList(LOADINGDATALIST& listColorData)
//{
//	if (m_pLightMapRenderMan)
//		m_pLightMapRenderMan->InsertColorList(listColorData);
//}

BOOL DxSingleMaterialMesh::IsCollisionLine(
									  const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
									  LPCSTR& szName, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	if ( !m_pLightMapRenderMan )
		return FALSE;

	// 데칼은 충돌처리를 하지 않는다.
	if ( m_dwFlags & ENSTM_DECAL_FULL )
		return FALSE;

	if ( bOnlyCamColl )
	{
		if ( !(m_dwFlags&ENSTM_CAMCOLL) ) 
			return FALSE;
	}

	// 벽만 충돌 처리를 한다.
	if ( emCullMode & EMCC_ONLY_WALL )
	{
		// 알파를 벽이라고 할 수는 없다.
		// 충돌처리를 하지 않도록 한다.
		if ( m_spTextureRes.GetAlphaType_const() != EMTT_NORMAL )
			return FALSE;
	}

	if ( emCullMode & EMCC_EXCEPT_ALPHATEX )
	{
		if ( m_spTextureRes.GetAlphaType_const() != EMTT_NORMAL )
			return FALSE;
	}

	if ( emCullMode & EMCC_EXCEPT_WATER )
	{
		if ( m_sMaterial_Data.m_rMaterial && m_sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_WATER )
			return FALSE;
	}

	if ( m_pLightMapRenderMan->IsCollisionLine( vStart, vEnd, vColl, vNor, emCullMode ) )
	{
		return TRUE;
	}
	else	return FALSE;
}

BOOL DxSingleMaterialMesh::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	if ( !m_pLightMapRenderMan )
		return FALSE;

	// 데칼은 충돌처리를 하지 않는다.
	if ( m_dwFlags & ENSTM_DECAL_FULL )
		return FALSE;

	if ( bOnlyCamColl )
	{
		if ( !(m_dwFlags&ENSTM_CAMCOLL) ) 
			return FALSE;
	}

	// 벽만 충돌 처리를 한다.
	if ( emCullMode & EMCC_ONLY_WALL )
	{
		// 알파를 벽이라고 할 수는 없다.
		// 충돌처리를 하지 않도록 한다.
		if ( m_spTextureRes.GetAlphaType_const() != EMTT_NORMAL )
			return FALSE;
	}

	if ( emCullMode & EMCC_EXCEPT_ALPHATEX )
	{
		if ( m_spTextureRes.GetAlphaType_const() != EMTT_NORMAL )
			return FALSE;
	}

	if ( emCullMode & EMCC_EXCEPT_WATER )
	{
		if ( m_sMaterial_Data.m_rMaterial && m_sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_WATER )
			return FALSE;
	}

	if ( m_pLightMapRenderMan->IsCollisionLine( vStart, vEnd, vColl, vNor, emCullMode ) )	return TRUE;
	else																			        return FALSE;
}

BOOL DxSingleMaterialMesh::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
{
	if ( !m_pLightMapRenderMan )
		return FALSE;

	// 데칼은 충돌처리를 하지 않는다.
	if ( m_dwFlags & ENSTM_DECAL_FULL )
		return FALSE;

	// 벽만 충돌 처리를 한다.
	if ( emCullMode & EMCC_ONLY_WALL )
	{
		// 알파를 벽이라고 할 수는 없다.
		// 충돌처리를 하지 않도록 한다.
		if ( m_spTextureRes.GetAlphaType_const() != EMTT_NORMAL )
			return FALSE;
	}

	if ( emCullMode & EMCC_EXCEPT_ALPHATEX )
	{
		if ( m_spTextureRes.GetAlphaType_const() != EMTT_NORMAL )
			return FALSE;
	}

	if ( emCullMode & EMCC_EXCEPT_WATER )
	{
		if ( m_sMaterial_Data.m_rMaterial && m_sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_WATER )
			return FALSE;
	}

	if ( m_pLightMapRenderMan->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )	
		return TRUE;
	else																			
		return FALSE;
}

// Note : 충돌이 되면 바로 리턴한다.
sc::DefaultLock g_sDefaultLock;
BOOL DxSingleMaterialMesh::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV )
{
	if ( !m_pLightMapRenderMan )	
		return FALSE;

	//// Matrerial 이 있다면 그것에 따른다. 그외는 Tool 에서 셋팅한 것을 따른다.
	//int nCastShadow = NSMaterialManager::IsCastShadowLightMap( m_strMaterialName );
	//if ( nCastShadow == 0 )
	//{
	//	return FALSE;
	//}
	//else if ( nCastShadow == -1 )
	//{
	//	if ( m_dwFlags & ENSTM_DONT_CAST_SHADOW_LM )
	//		return FALSE;
	//	if ( m_dwFlags & ENSTM_DECAL_FULL )
	//		return FALSE;
	//}

	// Matrerial 이 있다면 그것에 따른다. 그외는 Tool 에서 셋팅한 것을 따른다.
	// 바뀌었다.. Tool 을 기준으로 한다.
	{
		if ( m_dwFlags & ENSTM_DONT_CAST_SHADOW_LM )
			return FALSE;
		if ( m_dwFlags & ENSTM_DECAL_FULL )
			return FALSE;
	}

	if ( emCullMode & EMCC_EXCEPT_ALPHATEX )
	{
		if ( m_spTextureRes.GetAlphaType_const() != EMTT_NORMAL )
			return FALSE;

		//if ( NSMaterialManager::GetBlendType(m_strMaterialName) != NSMaterialManager::EMBLEND_OPAQUE )
		//	return FALSE;
	}

	if ( emCullMode & EMCC_EXCEPT_WATER )
	{
		if ( m_sMaterial_Data.m_rMaterial && m_sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_WATER )
			return FALSE;
	}

	g_sDefaultLock.lockOn();

	LoadTexture_NonThread( pd3dDevice );

	NSTexture16Bit::DATA* pData16Bit(NULL);
	if ( m_spTextureRes.GetAlphaType_const() != EMTT_NORMAL )
	{
		// Note : UV로 좌표를 구한다.
		
		if ( m_spTextureRes.GetTexture_const() )
		{
			pData16Bit = NSTexture16Bit::Insert( pd3dDevice, m_spTextureRes.GetFileName(), m_spTextureRes.GetTexture_const() );
		}
	}

	g_sDefaultLock.lockOff();

	if ( m_pLightMapRenderMan->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, pData16Bit, nIndexUV ) )	return TRUE;
	else																											return FALSE;
}

void DxSingleMaterialMesh::GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) // 주병진 추가 메소드
{
	if ( !m_pLightMapRenderMan ) return;

	m_pLightMapRenderMan->GetCollisionLineInTriListExceptY(vStart, vEnd, TriList);
}

BOOL DxSingleMaterialMesh::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor, const NSTexture16Bit::DATA* pDataDay )
{
	if ( !m_pLightMapRenderMan ) 
		return FALSE;

	if ( m_dwFlags & ENSTM_DECAL_FULL )
		return FALSE;

	if ( m_sMaterial_Data.m_rMaterial )
	{
		if ( m_sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_WATER )
			return FALSE;

		if ( m_sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_CUSTOM_SOFT_ALPHA )
			return FALSE;

		if ( m_sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_CUSTOM_UNLIT_UNFOG )
			return FALSE;
	}	

	return m_pLightMapRenderMan->GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor, pDataDay );
}

void DxSingleMaterialMesh::ClipStaticMesh( DxDecal* pDecal )
{
	m_pLightMapRenderMan->ClipMesh( pDecal );
}

void DxSingleMaterialMesh::Save( sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	SFile << static_cast<DWORD>( VERSION );

	SFile.BeginBlock();
	{
		SFile << m_strMaterialName;
		SFile << m_strTextureName;

		SFile << m_dwFlags;
		SFile << m_bDefaultFX_EnableLightMap;

		SFile << m_sMaterialValue.m_fCubeMapValue;
		SFile << m_sMaterialValue.m_fSpotSpecPower;
		SFile << m_sMaterialValue.m_fSpotSpecMulti;

		SFile << static_cast<DWORD>( m_emPS_3_0_Type_ForField );
		SFile << m_wwLightID_ForField.dwData;

		BOOL bExist;
		bExist = m_pLightMapRenderMan ? TRUE : FALSE;
		SFile << bExist;
		if ( m_pLightMapRenderMan )
		{
			m_pLightMapRenderMan->Save( SFile, bPiece );
		}
	}
	SFile.EndBlock();

	if ( pArchive )
	{
		if ( m_dwFlags & ENSTM_MASK_MAP )
		{
			TSTRING strMaskTexName = sc::util::GetSpecularName( m_strTextureName, _T("_mask") );
			pArchive( strMaskTexName.c_str() );
		}

		NSMaterialManager::Archive( m_strMaterialName, pArchive );
	}
}

void DxSingleMaterialMesh::Load ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bDynamicLoad, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	CleanUp();

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer==VERSION )
	{
		SFile >> m_strMaterialName;
		SFile >> m_strTextureName;

		SFile >> m_dwFlags;
		SFile >> m_bDefaultFX_EnableLightMap;

		SFile >> m_sMaterialValue.m_fCubeMapValue;
		SFile >> m_sMaterialValue.m_fSpotSpecPower;
		SFile >> m_sMaterialValue.m_fSpotSpecMulti;

		DWORD dwTemp;
		SFile >> dwTemp;	m_emPS_3_0_Type_ForField = static_cast<PS_3_0_TYPE>( dwTemp );
		SFile >> m_wwLightID_ForField.dwData;

		// Note : m_pLightMapRenderMan에서 Version 체크가 일어난다.
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
	}
	else if( dwVer==0x0107 )
	{
		SFile >> m_strMaterialName;
		SFile >> m_strTextureName;

		SFile >> m_dwFlags;
		SFile >> m_bDefaultFX_EnableLightMap;

		SFile >> m_sMaterialValue.m_fCubeMapValue;
		SFile >> m_sMaterialValue.m_fSpotSpecPower;
		SFile >> m_sMaterialValue.m_fSpotSpecMulti;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		DWORD dwTemp;
		SFile >> dwTemp;	m_emPS_3_0_Type_ForField = static_cast<PS_3_0_TYPE>( dwTemp );
		SFile >> m_wwLightID_ForField.dwData;

		// Note : m_pLightMapRenderMan에서 Version 체크가 일어난다.
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
	}
	else if( dwVer==0x0106 )
	{
		SFile >> m_strMaterialName;
		SFile >> m_strTextureName;

		SFile >> m_dwFlags;
		SFile >> m_bDefaultFX_EnableLightMap;

		SFile >> m_sMaterialValue.m_fCubeMapValue;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		DWORD dwTemp;
		SFile >> dwTemp;	m_emPS_3_0_Type_ForField = static_cast<PS_3_0_TYPE>( dwTemp );
		SFile >> m_wwLightID_ForField.dwData;

		// Note : m_pLightMapRenderMan에서 Version 체크가 일어난다.
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
	}
	else if( dwVer==0x0105 )
	{
		SFile >> m_strMaterialName;
		SFile >> m_strTextureName;

		SFile >> m_dwFlags;
		SFile >> m_bDefaultFX_EnableLightMap;

		SFile >> m_sMaterialValue.m_fCubeMapValue;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		// Note : m_pLightMapRenderMan에서 Version 체크가 일어난다.
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
	}
	else if( dwVer==0x0104 )
	{
		SFile >> m_strMaterialName;
		SFile >> m_strTextureName;

		SFile >> m_dwFlags;
		SFile >> m_bDefaultFX_EnableLightMap;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		// Note : m_pLightMapRenderMan에서 Version 체크가 일어난다.
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
	}
	else if( dwVer==0x0103 )
	{
		SFile >> m_strMaterialName;
		SFile >> m_strTextureName;

		SFile >> m_dwFlags;
		SFile >> m_bDefaultFX_EnableLightMap;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		m_dwFlags |= ENSTM_CAMCOLL;

		//if ( !NSMaterialManager::ExistMaterial( m_strMaterialName ) )
		//{
		//	// Material이 없다면 Msg로 기록해 놓는다.
		//	CDebugSet::ToLogFile( sc::string::format( _T("Create Mat File : %1% \n"), m_strMaterialName ) );
		//}

		// Note : m_pLightMapRenderMan에서 Version 체크가 일어난다.
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
	}
	else if( dwVer==0x0102 )
	{
		SFile >> m_strMaterialName;
		SFile >> m_strTextureName;

		SFile >> m_bDefaultFX_EnableLightMap;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		//if ( !NSMaterialManager::ExistMaterial( m_strMaterialName ) )
		//{
		//	// Material이 없다면 Msg로 기록해 놓는다.
		//	CDebugSet::ToLogFile( sc::string::format( _T("Create Mat File : %1% \n"), m_strMaterialName ) );
		//}

		// Note : m_pLightMapRenderMan에서 Version 체크가 일어난다.
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
	}
	else if( dwVer==0x0101 )
	{
		SFile >> m_strMaterialName;
		SFile >> m_strTextureName;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		//if ( !NSMaterialManager::ExistMaterial( m_strMaterialName ) )
		//{
		//	// Material이 없다면 Msg로 기록해 놓는다.
		//	CDebugSet::ToLogFile( sc::string::format( _T("Create Mat File : %1% \n"), m_strMaterialName ) );
		//}

		// Note : m_pLightMapRenderMan에서 Version 체크가 일어난다.
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
	}
	else if( dwVer==0x0100 )
	{
		SFile >> m_strMaterialName;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		//if ( !NSMaterialManager::ExistMaterial( m_strMaterialName ) )
		//{
		//	// Material이 없다면 Msg로 기록해 놓는다.
		//	CDebugSet::ToLogFile( sc::string::format( _T("Create Mat File : %1% \n"), m_strMaterialName ) );
		//}

		// Note : m_pLightMapRenderMan에서 Version 체크가 일어난다.
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );

		return;
	}	

	if ( pArchive )
	{
		if ( m_dwFlags & ENSTM_MASK_MAP )
		{
			TSTRING strMaskTexName = sc::util::GetSpecularName( m_strTextureName, _T("_mask") );
			pArchive( strMaskTexName.c_str() );
		}

		NSMaterialManager::Archive( m_strMaterialName, pArchive );
	}

	// Field는 무조건 한번은 해야한다. Piece 는 할 필요 없다.
	// Thread 에 DxCullingMeshForThread 정보를 넘겨주도록 한다.
	CreateDxCullingMeshSwap();
}

void DxSingleMaterialMesh::CreateDxCullingMeshSwap()
{
	// Field는 무조건 한번은 해야한다. Piece 는 할 필요 없다.
	// Thread 에 DxCullingMeshForThread 정보를 넘겨주도록 한다.
	if ( m_pLightMapRenderMan )
	{
		for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
		{
			m_spCullingMeshSwap[i] = boost::shared_ptr<DxCullingMeshSwap>(new DxCullingMeshSwap(m_pLightMapRenderMan->GetAABBBinaryBase()));
			NSLandThread::AddCullingMesh( static_cast<NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE>(i), m_spCullingMeshSwap[i] );
		}
	}
}

BOOL DxSingleMaterialMesh::IsLightMapPiece()
{
	if ( m_pLightMapRenderMan && (m_pLightMapRenderMan->GetTypeRender() == AABBRenderStrategy::TYPE_LIGHTMAP) )
	{
		return TRUE;
	}

	return FALSE;
}

void DxSingleMaterialMesh::SaveLightMapPiece( sc::SerialFile& SFile )
{
	SFile << static_cast<DWORD>( VERSION_LM );

	SFile.BeginBlock();
	{
		SFile << m_dwFlags;
		SFile << m_bDefaultFX_EnableLightMap;

		SFile << m_sMaterialValue.m_fCubeMapValue;
		SFile << m_sMaterialValue.m_fSpotSpecPower;
		SFile << m_sMaterialValue.m_fSpotSpecMulti;

		if ( m_pLightMapRenderMan )
		{
			SFile << (BOOL)TRUE;
			m_pLightMapRenderMan->Save( SFile, FALSE );
		}
		else
		{
			SFile << (BOOL)FALSE;
		}
	}
	SFile.EndBlock();
}

void DxSingleMaterialMesh::LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	if ( dwVer == VERSION_LM )
	{
		SFile >> m_dwFlags;
		SFile >> m_bDefaultFX_EnableLightMap;

		SFile >> m_sMaterialValue.m_fCubeMapValue;
		SFile >> m_sMaterialValue.m_fSpotSpecPower;
		SFile >> m_sMaterialValue.m_fSpotSpecMulti;

		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, FALSE, FALSE );
		}
	}
	else if ( dwVer == 0x0105 )
	{
		SFile >> m_dwFlags;
		SFile >> m_bDefaultFX_EnableLightMap;

		SFile >> m_sMaterialValue.m_fCubeMapValue;
		SFile >> m_sMaterialValue.m_fSpotSpecPower;
		SFile >> m_sMaterialValue.m_fSpotSpecMulti;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, FALSE, FALSE );
		}
	}
	else if ( dwVer == 0x0104 )
	{
		SFile >> m_dwFlags;
		SFile >> m_bDefaultFX_EnableLightMap;

		SFile >> m_sMaterialValue.m_fCubeMapValue;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, FALSE, FALSE );
		}
	}
	else if ( dwVer == 0x0103 )
	{
		SFile >> m_dwFlags;
		SFile >> m_bDefaultFX_EnableLightMap;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, FALSE, FALSE );
		}
	}
	else if ( dwVer == 0x0102 )
	{
		SFile >> m_dwFlags;
		SFile >> m_bDefaultFX_EnableLightMap;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		m_dwFlags |= ENSTM_CAMCOLL;

		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, FALSE, FALSE );
		}
	}
	else if ( dwVer == 0x0101 )
	{
		SFile >> m_bDefaultFX_EnableLightMap;

		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, FALSE, FALSE );
		}
	}
	else if ( dwVer == 0x0100 )
	{
		m_sMaterialValue.m_fSpotSpecMulti = 0.f;

		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pLightMapRenderMan = MIN_BOOST::polymorphic_downcast<LightMapRenderMan*>( AABBRenderStrategy::CreateFactory( AABBRenderStrategy::TYPE_LIGHTMAP ) );
			m_pLightMapRenderMan->Load( pd3dDevice, SFile, FALSE, FALSE );
		}
	}
}

void DxSingleMaterialMesh::FlipLightMapRenderMan( DxSingleMaterialMesh* pSingleTexMesh )
{
	if ( pSingleTexMesh->m_pLightMapRenderMan )
	{
		SAFE_DELETE( m_pLightMapRenderMan );
		m_pLightMapRenderMan = pSingleTexMesh->m_pLightMapRenderMan;
		m_bDefaultFX_EnableLightMap = pSingleTexMesh->m_bDefaultFX_EnableLightMap;
		pSingleTexMesh->m_pLightMapRenderMan = NULL;
	}
}

void DxSingleMaterialMesh::CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice )
{
}

void DxSingleMaterialMesh::GetClosedPoint( const D3DXVECTOR3& vBasePos, D3DXVECTOR3& vCollisionOut, float& fDistance )
{
	if ( !m_pLightMapRenderMan )	
	{
		return;
	}

	m_pLightMapRenderMan->GetClosedPoint( vBasePos, vCollisionOut, fDistance );
}

void DxSingleMaterialMesh::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld )
{
	// Mesh 정보를 얻어온다.
	std::vector<VERTEXNORCOLORTEX4>	vecVertices;
	std::vector<D3DXVECTOR2>		vecLightMapUV;
	std::vector<BOOL>				vecReceiveShadowLM;
	GetVertices( vecVertices, vecLightMapUV, vecReceiveShadowLM );

	NSExportXFile::ExportDxStaticMesh_SingleTexMesh
	( 
		pxofsave, 
		pDataObjectRoot, 
		vecVertices,
		strPieceName, 
		m_strTextureName,
		matWorld 
	);
}

// 단순히 DxStaticMesh 만들어 진 것을 DxSingleMaterialMesh 으로 변환하는 작업. 당연히 lightmap uv 는 DxStaticMesh 에 있는것을 복사한다.
// Field	- World Export 할 때 호출된다.
// Piece	- pis->pie 파일 만들 때 호출된다.
void DxSingleMaterialMesh::Import( LPDIRECT3DDEVICEQ pd3dDevice, const DxSingleTexMesh* pSingleTexMesh, float fCellSize )
{
	CleanUp();

	m_dwFlags = pSingleTexMesh->GetFlags();
	m_sMaterialValue = pSingleTexMesh->GetMaterialValue();

	m_strMaterialName = pSingleTexMesh->GetMaterialName();
	m_strTextureName = pSingleTexMesh->GetTextureName();
	m_bDefaultFX_EnableLightMap = pSingleTexMesh->GetDefaultFX_EnableLightMap();

	//if ( !NSMaterialManager::ExistMaterial( m_strMaterialName ) )
	//{
	//	// Material이 없다면 Msg로 기록해 놓는다.
	//	CDebugSet::ToLogFile( sc::string::format( _T("Create Mat File : %1% \n"), m_strMaterialName ) );
	//}

	LightMapRenderMan* pLightMapRenderMan = dynamic_cast<LightMapRenderMan*>( pSingleTexMesh->GetAABBRenderStrategy() );
	if ( pLightMapRenderMan )
	{
		bool bEnableVertexColor(false);
		Import( pd3dDevice, pLightMapRenderMan, fCellSize, bEnableVertexColor );

		if ( bEnableVertexColor )
		{
			m_dwFlags |= ENSTM_VERTEX_COLOR;
		}

		return;
	}

	//DxTriangleLinkLight_LightMap* pTriangleLinkLight_LightMap = dynamic_cast<DxTriangleLinkLight_LightMap*>( pSingleTexMesh->GetAABBRenderStrategy() );
	//if ( pTriangleLinkLight_LightMap )
	//{
	//	Import( pd3dDevice, pTriangleLinkLight_LightMap );
	//	return;
	//}

	// 그외.
	{
		// 이곳으로 들어온것은 Piece를 Export를 한다는 것이다.
		// Field 일 경우는 이곳으로 들어오지 않는다.
		DxAABBOctree* pAABBOctree = dynamic_cast<DxAABBOctree*>( pSingleTexMesh->GetAABBRenderStrategy() );
		if ( !pAABBOctree )
			return;

		// 메모리 단편화를 피하기 위한 작업.
		std::vector<VERTEXNORCOLORTEX5> vecVertices;
		DWORD dwVertexNum(0);
		pAABBOctree->GetVertexNum( dwVertexNum );
		vecVertices.reserve( dwVertexNum );
		
		pAABBOctree->GetVertices( vecVertices );

		if ( vecVertices.empty() )
			return;

		VERTEXNORCOLORTEX5* pVertices = new VERTEXNORCOLORTEX5[ vecVertices.size() ];
		for ( DWORD i=0; i<vecVertices.size(); ++i )
		{
			pVertices[i] = vecVertices[i];

			if ( vecVertices[i].dwColor != 0xffffffff )
			{
				m_dwFlags |= ENSTM_VERTEX_COLOR;
			}
		}

		CreateLightMap
		( 
			pd3dDevice, 
			pSingleTexMesh->GetMaterialName().c_str(),
			pSingleTexMesh->GetTextureName(),
			static_cast<DWORD> (vecVertices.size()/3), 
			reinterpret_cast<BYTE*>(pVertices), 
			FALSE,
			VERTEXNORCOLORTEX5::FVF,
			m_dwFlags,
			fCellSize
		);
	}
	
}

void DxSingleMaterialMesh::Import( LPDIRECT3DDEVICEQ pd3dDevice, LightMapRenderMan* pLightMapRenderMan, float fCellSize, bool &bEnableVertexColor )
{
	CleanUp();

	if ( pLightMapRenderMan->GetTypeRender() != AABBRenderStrategy::TYPE_LIGHTMAP )
		return;

	SAFE_DELETE( m_pLightMapRenderMan );
	m_pLightMapRenderMan = new LightMapRenderMan;
	m_pLightMapRenderMan->Import( pd3dDevice, pLightMapRenderMan, fCellSize, bEnableVertexColor );

	// Field는 무조건 한번은 해야한다. Piece 는 할 필요 없다.
	// Thread 에 DxCullingMeshForThread 정보를 넘겨주도록 한다.
	CreateDxCullingMeshSwap();
}
//
//void DxSingleMaterialMesh::Import( LPDIRECT3DDEVICEQ pd3dDevice, DxTriangleLinkLight_LightMap* pTriangleLinkLight_LightMap )
//{
//	CleanUp();
//
//	if ( pTriangleLinkLight_LightMap->GetTypeRender() != AABBRenderStrategy::TYPE_TRIANGLE_LINK_LIGHT_LIGHTMAP )
//		return;
//
//	SAFE_DELETE( m_pLightMapRenderMan );
//	m_pLightMapRenderMan = new LightMapRenderMan;
//	m_pLightMapRenderMan->Import( pd3dDevice, pTriangleLinkLight_LightMap );
//}

void DxSingleMaterialMesh::ChangeDxStaticMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxSingleTexMesh* pSingleTexMesh, float fCellSize )
{
	CleanUp();

	m_dwFlags = pSingleTexMesh->GetFlags();
	m_sMaterialValue = pSingleTexMesh->GetMaterialValue();
	m_strMaterialName = _T("PrevMaterials");	// Material 이름을 텍스쳐랑 같게한다.
	m_strTextureName = pSingleTexMesh->GetTextureName();
	m_bDefaultFX_EnableLightMap = FALSE;

	// 이곳으로 들어온것은 Piece를 Export를 한다는 것이다.
	DxAABBOctree* pAABBOctree = dynamic_cast<DxAABBOctree*>( pSingleTexMesh->GetAABBRenderStrategy() );
	if ( !pAABBOctree )
		return;

	// LightMap UV 를 가지고 있는지 체크하게 된다.
	m_bDefaultFX_EnableLightMap = pAABBOctree->IsHaveLightMapUV();

	// 메모리 단편화를 피하기 위한 작업.
	std::vector<VERTEXNORCOLORTEX5> vecVertices;
	DWORD dwVertexNum(0);
	pAABBOctree->GetVertexNum( dwVertexNum );
	vecVertices.reserve( dwVertexNum );

	pAABBOctree->GetVertices( vecVertices );

	if ( vecVertices.empty() )
		return;

	VERTEXNORCOLORTEX5* pVertices = new VERTEXNORCOLORTEX5[ vecVertices.size() ];
	for ( DWORD i=0; i<vecVertices.size(); ++i )
	{
		pVertices[i] = vecVertices[i];
	}

	CreateLightMap
	( 
		pd3dDevice, 
		pSingleTexMesh->GetMaterialName().c_str(),
		pSingleTexMesh->GetTextureName(),
		static_cast<DWORD> (vecVertices.size()/3), 
		reinterpret_cast<BYTE*>(pVertices), 
		FALSE,
		VERTEXNORCOLORTEX5::FVF,
		m_dwFlags,
		fCellSize
	);
}

void DxSingleMaterialMesh::SetLightID( const WORDWORD& wwLightID )
{
	m_wwLightID_ForField = wwLightID;

	if ( m_wwLightID_ForField.wA == 0 )
	{
		if ( m_wwLightID_ForField.wB == WORD(-1) )
		{
			m_emPS_3_0_Type_ForField = PS_3_0_TYPE_CSM;
		}
		else
		{
			m_emPS_3_0_Type_ForField = PS_3_0_TYPE_CSM_PL;
		}
	}
	else if ( m_wwLightID_ForField.wA != WORD(-1) )
	{
		if ( m_wwLightID_ForField.wB != WORD(-1) )
		{
			m_emPS_3_0_Type_ForField = PS_3_0_TYPE_PL_PL;
		}
		else
		{
			m_emPS_3_0_Type_ForField = PS_3_0_TYPE_PL1;
		}
	}
	else if ( m_wwLightID_ForField.wB != WORD(-1) )
	{
		m_emPS_3_0_Type_ForField = PS_3_0_TYPE_PL2;
	}
	else
	{
		m_emPS_3_0_Type_ForField = PS_3_0_TYPE_ZERO;
	}
}

void DxSingleMaterialMesh::SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate )
{
	if ( !m_pLightMapRenderMan )	
	{
		return;
	}

	LightMapRenderMan* pDynamic = dynamic_cast<LightMapRenderMan*>( m_pLightMapRenderMan );
	if ( !pDynamic )
		return;

	pDynamic->SetLightMapUV_Rate( vLightMapUV_Rate );
}

BOOL DxSingleMaterialMesh::CheckOnlyVoxel()
{
	if ( m_dwFlags & ENSTM_UNLIGHTMAP_VOXEL_COLOR )
	{
		return TRUE;
	}
	return FALSE;
}

HRESULT DxSingleMaterialMesh::FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const SFile )
{
	LoadTexture_NonThread( pd3dDevice );
	return S_OK;
}

void DxSingleMaterialMesh::LoadTexture_NonThread( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : Texture Loading
	if( !m_spTextureRes.Update() )
	{
		//if( !m_bCheckTex )
		//{
			if ( m_strTextureName.size() )
			{
				m_spTextureRes = TextureManager::GetInstance().LoadTexture(
					m_strTextureName,
					false,
					TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
					0,
					D3DFMT_UNKNOWN,
					TEXTURE_RESIZE_NONE,
					TEXTURE_LAND,
					false,
					FALSE );
			}

			if ( m_dwFlags & ENSTM_MASK_MAP )
			{
				TSTRING strMaskTexName = sc::util::GetSpecularName( m_strTextureName, _T("_mask") );

				m_spMaskTexRes = TextureManager::GetInstance().LoadTexture(
					strMaskTexName,
					false,
					TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
					0,
					D3DFMT_UNKNOWN,
					TEXTURE_RESIZE_NONE,
					TEXTURE_LAND,
					false,
					FALSE );
			}

			m_bCheckTex = TRUE;
		//}
	}

	NSMaterialManager::LoadTexture_NonThread( m_strMaterialName );
}

BOOL DxSingleMaterialMesh::IsTextureLoadingComplete()
{
	if ( !m_spTextureRes.GetTexture() && !m_spTextureRes.GetFileName().empty() )
	{
		// 파일이 있지만 로드가 안된상태라 FALSE 를 넘겨준다.
		if ( TextureManager::GetInstance().IsPathFile( m_spTextureRes.GetFileName()) )
			return FALSE;
	}

	if ( m_dwFlags & ENSTM_MASK_MAP )
	{
		if ( !m_spMaskTexRes.GetTexture() )
		{
			// 파일이 있지만 로드가 안된상태라 FALSE 를 넘겨준다.
			if ( TextureManager::GetInstance().IsPathFile( m_spMaskTexRes.GetFileName()) )
				return FALSE;
		}
	}

	return TRUE;
}

BOOL DxSingleMaterialMesh::IsExistCullingData( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	return m_spCullingMeshSwap[emCullingRenderType]->IsExistCullingData();
}

//// 알파 텍스쳐인지 불투명텍스쳐인지 셋팅한다.
//if ( m_spTextureRes.IsCompletedData() )
//{
//	switch ( m_spTextureRes.GetAlphaType() )
//	{
//	case EMTT_ALPHA_HARD:
//		Insert( pRenderOrder->m_mapHardAlpha );
//		dwTexType |= EMTTF_ALPHA_HARD;
//		break;
//	case EMTT_ALPHA_SOFT:
//		Insert( pRenderOrder->m_mapSoftAlpha00 );
//		dwTexType |= EMTTF_ALPHA_SOFT;
//		break;
//	case EMTT_ALPHA_SOFT01:
//		Insert( pRenderOrder->m_mapSoftAlpha01 );
//		dwTexType |= EMTTF_ALPHA_SOFT;
//		break;
//	case EMTT_ALPHA_SOFT02:
//		Insert( pRenderOrder->m_mapSoftAlpha02 );
//		dwTexType |= EMTTF_ALPHA_SOFT;
//		break;
//	case EMTT_NORMAL:
//	default:
//		Insert( pRenderOrder->m_mapOpaque );
//		dwTexType |= EMTTF_OPAQUE;
//		break;
//	};
//}
//else
//{
//	Insert( pRenderOrder->m_mapUnKnownTextureType );
//	dwTexType |= EMTTF_OPAQUE;
//}
//
//return S_OK;
//}
