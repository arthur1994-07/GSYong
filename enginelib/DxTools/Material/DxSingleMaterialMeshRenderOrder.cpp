#include "pch.h"

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
#include "../../Common/profile.h"
#include "../../GlobalParam.h"

#include "../Texture/NSTexture16Bit.h"
#include "../Texture/NSTexCompress.h"
#include "../CubeMap/NSCubeMapCulling.h"
#include "../CubeMap/NSCubeMapUtil.h"
#include "../Light/NSShadowLight.h"
#include "../Light/DxLightMan.h"
#include "../TextureManager.h"
#include "../DxRenderStates.h"
#include "../DxEnvironment.h"
#include "../DxSurfaceTex.h"
#include "../RENDERPARAM.h"
#include "../DxViewPort.h"

#include "./NSShadowRenderForMaterial.h"
#include "./NSMaterialManager.h"
#include "./DxSingleMaterialMesh.h"
#include "./MaterialDef.h"

#include "DxSingleMaterialMeshRenderOrder.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//										DxSingleMaterialMeshRenderOrder
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxSingleMaterialMeshRenderOrder::RenderMesh( const LPDIRECT3DDEVICEQ pd3dDevice, 
												 NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
												 LPDIRECT3DTEXTUREQ pTextureCombined, 
												 LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_1,
												 LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_2,
												 LPDIRECT3DTEXTUREQ pTextureIndirect,
												 Material* pMaterial,
												 MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
												 BOOL bMPShading,
												 UINT unPass,
												 BOOL bLightMapTest,
												 D3DXVECTOR4* pvAmbientCube,
												 D3DXVECTOR4& vVoxelColor,
												 const D3DXMATRIX& matWorld,
												 TEXTURE_ALPHA emAlphaType,
												 BOOL bDecal )
{
	BOOL bExistCullingData(FALSE);
	MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
	for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
	{
		VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
		for ( DWORD i=0; i<vecMesh.size(); ++i )
		{
			if ( vecMesh[i]->IsExistCullingData( emCullingRenderType ) )
			{
				bExistCullingData = TRUE;
				break;
			}
		}

		if ( bExistCullingData )
			break;
	}

	if ( !bExistCullingData )
		return;

	if ( RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
	{
		//  옵션 상이상 이지만, 특정한 이유(반사)로 인해 렌더링은 TnL_PS_3_0_DF 으로 동작되도록 한다.
		if ( !pMaterial->m_rFXnType_PS30_DF )
			return;

		ID3DXEffect* pFX = pMaterial->m_rFXnType_PS30_DF->m_rFX;
		if ( !pFX )
			return;

		D3DXHANDLE hTechniqueHandle(NULL);

		hTechniqueHandle = pMaterial->m_rFXnType_PS30_DF->m_hTechnique_runtime_2;
		if ( !hTechniqueHandle )
			return;

		if ( pTextureCombined )
		{
			pFX->SetTexture( "g_LightMapDayTex", pTextureCombined );
		}

		pFX->SetTechnique( hTechniqueHandle );

		RenderMesh_NEWFX
		( 
			pd3dDevice, 
			emCullingRenderType,
			pTextureCombined, 
			pMaterial,
			mapType_vecMesh,
			unPass,
			pvAmbientCube,
			vVoxelColor,
			matWorld,
			pFX,
			pMaterial->m_rFXnType_PS30_DF->m_emFxType
		);
	}
	else if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM && !bLightMapTest && !bDecal )
	{
		// 3.0 이상이면서 Shading 작업을 하는 경우.
		ID3DXEffect* pFX(NULL);
		D3DXHANDLE hTechniqueHandle(NULL);
		NSMaterialManager::FX_TYPE	emFxType(NSMaterialManager::FX_TYPE_DEFAULT);
		if ( bMPShading )
		{
			if ( !pMaterial->m_rFXnType_LBuffer )
				return;

			pFX = pMaterial->m_rFXnType_LBuffer->m_rFX;
			if ( !pFX )
				return;

			emFxType = pMaterial->m_rFXnType_LBuffer->m_emFxType;
			hTechniqueHandle = pMaterial->m_rFXnType_LBuffer->m_hTechnique_runtime_3;
		}
		else
		{
			pFX = pMaterial->m_sFXnType.m_rFX;
			if ( !pFX )
				return;

			emFxType = pMaterial->m_sFXnType.m_emFxType;
			hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_3;
		}

		if ( hTechniqueHandle )
		{
			if ( pTextureIndirect )
			{
				pFX->SetTexture( "g_LightMapDayTex", pTextureIndirect );
				pFX->SetTexture( "g_LightMapTex_DS_1st", pTextureDirect_Shadow_1 );
				pFX->SetTexture( "g_LightMapTex_DS_2nd", pTextureDirect_Shadow_2 );
			}
		}
		else
		{
			return;		
		}

		pFX->SetTechnique( hTechniqueHandle );

		NSMaterialManager::SetFX_CommonParameter_Field( pFX, emFxType, pvAmbientCube, vVoxelColor );

		NSMaterialManager::SetFX_CommonParameter_Small( pFX, 1.f, matWorld );

		//pd3dDevice->SetVertexDeclaration( pMaterial->m_sMaterialGAME.m_pDCRT );		

		// 렌더링한다.
		UINT numPasses;
		pFX->Begin( &numPasses, 0 );
		{
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
			for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
			{
				// 3.0 일경우 알맞은 Pass 를 구하도록 한다.
				size_t unPassTemp = unPass;
				//unPassTemp = unPass + (*iterA).first * 2;
				size_t unPassUsed(0);

				if ( bMPShading )
				{
					switch ( (*iterA).first )
					{
					case PS_3_0_TYPE_ZERO:
					case PS_3_0_TYPE_CSM:
						continue;	// 렌더링 할 필요가 없다.
						break;

					case PS_3_0_TYPE_PL1:
						unPassTemp = 0;
						break;

					case PS_3_0_TYPE_CSM_PL:
					case PS_3_0_TYPE_PL2:
						unPassTemp = 1;
						break;

					case PS_3_0_TYPE_PL_PL:
						unPassTemp = 2;
						break;
					};

					unPassUsed = unPassTemp;
					if ( emAlphaType == EMTT_ALPHA_HARD || emAlphaType == EMTT_ALPHA_SOFT )
					{
						unPassUsed += 3;
					}
				}
				else
				{
					switch ( (*iterA).first )
					{
					case PS_3_0_TYPE_ZERO:
					case PS_3_0_TYPE_CSM:
						unPassTemp = 0;
						break;

					case PS_3_0_TYPE_PL1:
						unPassTemp = 1;
						break;

					case PS_3_0_TYPE_CSM_PL:
					case PS_3_0_TYPE_PL2:
						unPassTemp = 2;
						break;

					case PS_3_0_TYPE_PL_PL:
						unPassTemp = 3;
						break;
					};

					unPassUsed = unPassTemp;
					if ( emAlphaType == EMTT_ALPHA_SOFT )
					{
						unPassUsed += 4;
					}
				}


				if( unPassUsed < numPasses )
				{			
					pFX->BeginPass( unPassUsed );
					{
						VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
						for ( DWORD i=0; i<vecMesh.size(); ++i )
						{
							BOOL bChangePass(FALSE);
							if ( !vecMesh[i]->IsRenderEnable_ChangeFX_Field( pFX, bMPShading, emAlphaType, unPassTemp, bChangePass ) )
								continue;

							vecMesh[i]->RenderThreadLoad( pd3dDevice, emCullingRenderType, pFX );

							if ( bChangePass )
							{
								pFX->EndPass();
								pFX->BeginPass( unPassUsed );
							}
						}
					}
					pFX->EndPass();	
				}
			}
		}
		pFX->End();
	}
	else
	{
		ID3DXEffect* pFX = pMaterial->m_sFXnType.m_rFX;
		if ( !pFX )
			return;

		D3DXHANDLE hTechniqueHandle(NULL);

		if ( bLightMapTest )
		{
			hTechniqueHandle = pFX->GetTechniqueByName("lightmaptest");

			// EMRM_TEST(EMRM_MATERIAL) 의 LMRT_CHECKER 의 경우는 다른곳에서 g_vLightMapUV_Multiply 값을 셋팅한다.
			D3DXVECTOR4 vUV_Multiply(1.f,1.f,1.f,1.f);
			pFX->SetVector( "g_vLightMapUV_Multiply", &vUV_Multiply );

			pFX->SetTexture( "g_LightMapDayTex", pTextureCombined );
		}
		else
		{
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
			{
				hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_3;
			}

			if ( !hTechniqueHandle )
			{
				if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
				{
					hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_2;

					if ( pTextureCombined )
					{
						pFX->SetTexture( "g_LightMapDayTex", pTextureCombined );
					}
				}
				if ( !hTechniqueHandle && RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
				{
					hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_1;
					if ( pTextureCombined )
					{
						pFX->SetTexture( "g_LightMapDayTex", pTextureCombined );
					}
				}
			}

			if ( bMPShading )
			{
				if ( pTextureCombined )
				{
					pFX->SetTexture( "g_LightMapDayTex", pTextureCombined );
				}
			}
		}

		if ( !hTechniqueHandle )
			return;

		pFX->SetTechnique( hTechniqueHandle );

		RenderMesh_NEWFX
		( 
			pd3dDevice, 
			emCullingRenderType,
			pTextureCombined, 
			pMaterial,
			mapType_vecMesh,
			unPass,
			pvAmbientCube,
			vVoxelColor,
			matWorld,
			pFX,
			pMaterial->m_sFXnType.m_emFxType
		);
	}
}

void DxSingleMaterialMeshRenderOrder::RenderMesh_NEWFX( const LPDIRECT3DDEVICEQ pd3dDevice, 
													   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
													   LPDIRECT3DTEXTUREQ pTextureCombined, 
													   Material* pMaterial,
													   MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
													   UINT unPass,
													   D3DXVECTOR4* pvAmbientCube,
													   D3DXVECTOR4& vVoxelColor,
													   const D3DXMATRIX& matWorld,
													   ID3DXEffect* pFX,
													   NSMaterialManager::FX_TYPE emFxType )
{
	NSMaterialManager::SetFX_CommonParameter_Field( pFX, emFxType, pvAmbientCube, vVoxelColor );

	// 이 안의 Code 는 NSMaterialManager::SetFX_CommonParameter_Field 호출 후에 불려져야한다.
	// NSMaterialManager::SetFX_CommonParameter_Field 에서 일단 g_vLightMapUV_Multiply 가 호출되기 때문이다.
	{
		D3DXVECTOR4 vUV_Multiply(1.f,1.f,1.f,1.f);
		if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
		{
			NSLightMapFX::GetLightMapCheker( pTextureCombined, vUV_Multiply );
			pFX->SetVector( "g_vLightMapUV_Multiply", &vUV_Multiply );

			pFX->SetTexture( "g_LightMapDayTex", pTextureCombined );
		}
	}

	NSMaterialManager::SetFX_CommonParameter_Small( pFX, 1.f, matWorld );

	//pd3dDevice->SetVertexDeclaration( pMaterial->m_sMaterialGAME.m_pDCRT );		

	// 렌더링한다.
	UINT numPasses;
	pFX->Begin( &numPasses, 0 );
	{
		// 3.0 일경우 알맞은 Pass 를 구하도록 한다.
		size_t unPassTemp = unPass;
		if( unPassTemp < numPasses )
		{			
			pFX->BeginPass( unPassTemp );
			{
				MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
				for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
				{
					VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
					for ( DWORD i=0; i<vecMesh.size(); ++i )
					{
						vecMesh[i]->RenderThreadLoad( pd3dDevice, emCullingRenderType, pFX );
					}
				}
				pFX->EndPass();	
			}
		}
	}
	pFX->End();
}

void DxSingleMaterialMeshRenderOrder::RenderMeshWater( const LPDIRECT3DDEVICEQ pd3dDevice, 
													 NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
													 Material* pMaterial,
													 VEC_SINGLE_MATERIAL_MESH& vecMesh,
													 float fPrevHeight, 
													 float &fHeight,
													 float &fNearLength, 
													 const D3DXMATRIX& matWorld )
{
	ID3DXEffect* pFX = pMaterial->m_sFXnType.m_rFX;
	if ( !pFX )
		return;

	D3DXHANDLE hTechniqueHandle(NULL);

	{
		if ( !hTechniqueHandle && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
		{
			hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_2;
		}
		if ( !hTechniqueHandle && RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
		{
			hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_1;
		}
	}

	if ( !hTechniqueHandle )
		return;

	pFX->SetTechnique( hTechniqueHandle );

	//// SetFX_CommonParameter 을 한번만 호출하기 위해 존재함.
	//std::set<ID3DXEffect*>::iterator iter = m_setFX.find( pFX );
	//if ( iter==m_setFX.end() )
	//{
	//	NSMaterialManager::SetFX_CommonParameter( pFX );
	//	m_setFX.insert( pFX );
	//}
	NSMaterialManager::SetFX_CommonParameter( pFX, pMaterial->m_sFXnType.m_emFxType );

	NSMaterialManager::SetFX_CommonParameter_Small( pFX, 1.f, matWorld );

	for ( DWORD i=0; i<vecMesh.size(); ++i )
	{
		vecMesh[i]->RenderStaticWater( pd3dDevice, emCullingRenderType, fPrevHeight, fHeight, fNearLength );
	}
}

void DxSingleMaterialMeshRenderOrder::RenderMeshCustomMaterial( const LPDIRECT3DDEVICEQ pd3dDevice, 
																  NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
																  Material* pMaterial,
																  VEC_SINGLE_MATERIAL_MESH& vecMesh, 
																  const D3DXMATRIX& matWorld )
{
	ID3DXEffect* pFX = pMaterial->m_sFXnType.m_rFX;
	if ( !pFX )
		return;

	D3DXHANDLE hTechniqueHandle(NULL);

	{
		if ( !hTechniqueHandle && RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
		{
			hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_1;
		}
	}

	if ( !hTechniqueHandle )
		return;

	pFX->SetTechnique( hTechniqueHandle );

	//// SetFX_CommonParameter 을 한번만 호출하기 위해 존재함.
	//std::set<ID3DXEffect*>::iterator iter = m_setFX.find( pFX );
	//if ( iter==m_setFX.end() )
	//{
	//	NSMaterialManager::SetFX_CommonParameter( pFX );
	//	m_setFX.insert( pFX );
	//}
	NSMaterialManager::SetFX_CommonParameter( pFX, pMaterial->m_sFXnType.m_emFxType );
	NSMaterialManager::SetFX_CommonParameter_Small( pFX, 1.f, matWorld );

	for ( DWORD i=0; i<vecMesh.size(); ++i )
	{
		vecMesh[i]->RenderCommonCustomMaterial( pd3dDevice, emCullingRenderType );
	}
}

void DxSingleMaterialMeshRenderOrder::RenderPiece( PieceParameter& sParameter,
													Material* pMaterial,
													MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
													BOOL bMPShading,
													UINT unPass,
													BOOL bLightMapTest,
													TEXTURE_ALPHA emAlphaType,
													BOOL bDecal )
{
	if ( RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
	{
		if ( !pMaterial->m_rFXnType_PS30_DF )
			return;

		ID3DXEffect* pFX = pMaterial->m_rFXnType_PS30_DF->m_rFX;
		if ( !pFX )
			return;

		D3DXHANDLE hTechniqueHandle(NULL);
		hTechniqueHandle = pMaterial->m_rFXnType_PS30_DF->m_hTechnique_runtime_2;
		if ( !hTechniqueHandle )
			return;

		if ( sParameter.m_pTextureCombined )
		{
			pFX->SetTexture( "g_LightMapDayTex", sParameter.m_pTextureCombined );
		}

		pFX->SetTechnique( hTechniqueHandle );

		RenderPiece_NEWFX
		( 
			sParameter,
			mapType_vecMesh,
			unPass,
			pFX,
			pMaterial->m_rFXnType_PS30_DF->m_emFxType
		);
	}
	else if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM && !bLightMapTest && (sParameter.m_nRenderMode==NSMaterialManager::EMRM_MATERIAL) && !bDecal )
	{
		ID3DXEffect* pFX(NULL);
		D3DXHANDLE hTechniqueHandle(NULL);
		NSMaterialManager::FX_TYPE	emFxType(NSMaterialManager::FX_TYPE_DEFAULT);
		if ( bMPShading )
		{
			if ( !pMaterial->m_rFXnType_LBuffer )
				return;

			pFX = pMaterial->m_rFXnType_LBuffer->m_rFX;
			if ( !pFX )
				return;

			emFxType = pMaterial->m_rFXnType_LBuffer->m_emFxType;
			hTechniqueHandle = pMaterial->m_rFXnType_LBuffer->m_hTechnique_runtime_3;
		}
		else
		{
			pFX = pMaterial->m_sFXnType.m_rFX;
			if ( !pFX )
				return;

			emFxType = pMaterial->m_sFXnType.m_emFxType;
			hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_3;
		}

		if ( hTechniqueHandle )
		{
			if ( sParameter.m_pTextureIndirect )
			{
				pFX->SetTexture( "g_LightMapDayTex", sParameter.m_pTextureIndirect );
				pFX->SetTexture( "g_LightMapTex_DS_1st", sParameter.m_pTextureDirect_Shadow_1 );
				pFX->SetTexture( "g_LightMapTex_DS_2nd", sParameter.m_pTextureDirect_Shadow_2 );
			}
		}
		else
		{
			return;		
		}

		pFX->SetTechnique( hTechniqueHandle );

		D3DXVECTOR4 vLightMapUV_Offset4( sParameter.m_vLightMapUV_Offset.x, sParameter.m_vLightMapUV_Offset.y, 1.f, 1.f );
		pFX->SetVector( "g_vLightMapUV_Offset", &vLightMapUV_Offset4 );

		NSMaterialManager::SetFX_CommonParameter( pFX, emFxType );

		NSMaterialManager::SetFX_CommonParameter_Small( pFX, sParameter.m_fAlpha, *sParameter.m_rmatWorld );

		// 렌더링한다.
		UINT numPasses;
		pFX->Begin( &numPasses, 0 );
		{
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
			for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
			{
				//// 3.0 일경우 알맞은 Pass 를 구하도록 한다.
				//size_t unPassTemp = unPass;
				////unPassTemp = unPass + sParameter.m_pTriangleLinkLight->m_emPS_3_0_Type_USED * 2;

				//size_t unPassUsed(0);
				//if ( bMPShading )
				//{
				//	switch ( (*iterA).first )
				//	{
				//	case PS_3_0_TYPE_ZERO:
				//	case PS_3_0_TYPE_CSM:
				//		continue;	// 렌더링 할 필요가 없다.
				//		break;

				//	case PS_3_0_TYPE_PL1:
				//		unPassTemp = 0;
				//		break;

				//	case PS_3_0_TYPE_CSM_PL:
				//	case PS_3_0_TYPE_PL2:
				//		unPassTemp = 1;
				//		break;

				//	case PS_3_0_TYPE_PL_PL:
				//		unPassTemp = 2;
				//		break;
				//	};

				//	unPassUsed = unPassTemp;
				//	if ( emAlphaType == EMTT_ALPHA_HARD || emAlphaType == EMTT_ALPHA_SOFT )
				//	{
				//		unPassUsed += 3;
				//	}
				//}
				//else
				//{
				//	switch ( (*iterA).first )
				//	{
				//	case PS_3_0_TYPE_ZERO:
				//	case PS_3_0_TYPE_CSM:
				//		unPassTemp = 0;
				//		break;

				//	case PS_3_0_TYPE_PL1:
				//		unPassTemp = 1;
				//		break;

				//	case PS_3_0_TYPE_CSM_PL:
				//	case PS_3_0_TYPE_PL2:
				//		unPassTemp = 2;
				//		break;

				//	case PS_3_0_TYPE_PL_PL:
				//		unPassTemp = 3;
				//		break;
				//	};

				//	unPassUsed = unPassTemp;
				//	if ( emAlphaType == EMTT_ALPHA_SOFT )
				//	{
				//		unPassUsed += 4;
				//	}
				//}


							
				VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
				for ( DWORD i=0; i<vecMesh.size(); ++i )
				{
					//BOOL bChangePass(FALSE);
					//if ( !vecMesh[i]->IsRenderEnable_ChangeFX( pFX, 
					//											bMPShading, 
					//											emAlphaType, 
					//											unPassTemp, 
					//											bChangePass, 
					//											sParameter.m_pTriangleLinkLight->m_emPS_3_0_Type,
					//											sParameter.m_pTriangleLinkLight->m_wwLightID ) )
					//	continue;

					int nNewPass(unPass);
					if ( bMPShading )
					{
						nNewPass = NSShadowLightUtil::GetPassFX_PS_3_0_TYPE_USED_MPShading( sParameter.m_pTriangleLinkLight->m_emPS_3_0_Type, sParameter.m_pTriangleLinkLight->m_wwLightID );
						if ( nNewPass == -1 )
							continue;		// 이건 렌더링 할 필요가 없다.

						if ( emAlphaType == EMTT_ALPHA_HARD || emAlphaType == EMTT_ALPHA_SOFT )
						{
							nNewPass += 3;
						}
					}
					else
					{
						nNewPass = NSShadowLightUtil::GetPassFX_PS_3_0_TYPE_USED( sParameter.m_pTriangleLinkLight->m_emPS_3_0_Type, sParameter.m_pTriangleLinkLight->m_wwLightID );

						if ( emAlphaType == EMTT_ALPHA_SOFT )
						{
							nNewPass += 4;
						}
					}

					if( static_cast<size_t>(nNewPass) < numPasses )
					{
						pFX->BeginPass( nNewPass );
						{
							vecMesh[i]->RenderPiece( sParameter, pFX );
						}
						pFX->EndPass();
					}
					//if ( bChangePass )
					//{
					//	pFX->EndPass();
					//	pFX->BeginPass( unPassUsed );
					//}
						
				}
			}
		}
		pFX->End();
	}
	else
	{
		ID3DXEffect* pFX(NULL);
		pFX = pMaterial->m_sFXnType.m_rFX;
		if ( !pFX )
			return;

		D3DXHANDLE hTechniqueHandle(NULL);

		if ( sParameter.m_nRenderMode ==  NSMaterialManager::EMRM_TOOL )	// 순서 중요함.
		{
			// [shhan][2015.09.22] 문제상황 : psf 상에서 checker 를 보면 crash 가 발생함.
			//						위치 : pFX->CommitChanges(); 에서 문제가 발생하는게 발생했다.
			//						원인 : Mesh 는 변경해주지 않고, fx 를 변경시키면서(SEMANTIC_VS_INPUT_TOOL게 변경됨) 매칭이 되지 않아서 crash 가 발생한 듯하다.
			//						해결 : Tool 일 경우 무조건 우선 순위 높게 둬서, hTechniqueHandle 값이 NULL 이 되면서 아무런 Rendering 이 되지 않도록 한다.
			//
			hTechniqueHandle = pFX->GetTechniqueByName("tool_deffered");
			if ( hTechniqueHandle )
			{
				NSMaterialManager::SetFX_SpotLightEDITMODE( pFX );
				pFX->SetFloat( "g_fVoxel", 0.f );
			}
		}
		else if ( bLightMapTest )
		{
			hTechniqueHandle = pFX->GetTechniqueByName("lightmaptest");

			pFX->SetTexture( "g_LightMapDayTex", sParameter.m_pTextureCombined );
		}
		else
		{
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
			{
				hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_3;
			}

			if ( !hTechniqueHandle )
			{
				if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
				{
					hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_2;

					if ( sParameter.m_pTextureCombined )
					{
						pFX->SetTexture( "g_LightMapDayTex", sParameter.m_pTextureCombined );
					}
				}
				if ( !hTechniqueHandle && RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
				{
					hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_1;
					if ( sParameter.m_pTextureCombined )
					{
						pFX->SetTexture( "g_LightMapDayTex", sParameter.m_pTextureCombined );
					}
				}		
			}
		}

		if ( !hTechniqueHandle )
			return;

		pFX->SetTechnique( hTechniqueHandle );

		RenderPiece_NEWFX
		( 
			sParameter,
			mapType_vecMesh,
			unPass,
			pFX,
			pMaterial->m_sFXnType.m_emFxType
		);
	}
}

void DxSingleMaterialMeshRenderOrder::RenderPiece_NEWFX( PieceParameter& sParameter,
														MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
														UINT unPass,
														ID3DXEffect* pFX,
														NSMaterialManager::FX_TYPE emFxType )
{
	D3DXVECTOR4 vLightMapUV_Offset4( sParameter.m_vLightMapUV_Offset.x, sParameter.m_vLightMapUV_Offset.y, 1.f, 1.f );
	pFX->SetVector( "g_vLightMapUV_Offset", &vLightMapUV_Offset4 );

	NSMaterialManager::SetFX_CommonParameter( pFX, emFxType );

	// 이 안의 Code 는 NSMaterialManager::SetFX_CommonParameter 호출 후에 불려져야한다.
	// NSMaterialManager::SetFX_CommonParameter 에서 일단 g_vLightMapUV_Multiply 가 호출되기 때문이다.
	{
		D3DXVECTOR4 vUV_Multiply(1.f,1.f,1.f,1.f);
		if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
		{
			LPDIRECT3DTEXTUREQ pTextureCombined = sParameter.m_pTextureCombined;

			NSLightMapFX::GetLightMapCheker( pTextureCombined, vUV_Multiply );
			pFX->SetVector( "g_vLightMapUV_Multiply", &vUV_Multiply );

			pFX->SetTexture( "g_LightMapDayTex", pTextureCombined );
		}
	}

	NSMaterialManager::SetFX_CommonParameter_Small( pFX, sParameter.m_fAlpha, *sParameter.m_rmatWorld );

	// 렌더링한다.
	UINT numPasses;
	pFX->Begin( &numPasses, 0 );
	{
		size_t unPassTemp = unPass;
		if( unPassTemp < numPasses )
		{
			pFX->BeginPass( unPassTemp );
			{
				MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
				for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
				{

					VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
					for ( DWORD i=0; i<vecMesh.size(); ++i )
					{
						vecMesh[i]->RenderPiece( sParameter, pFX );
					}
				}
				pFX->EndPass();	
			}
		}
	}
	pFX->End();
}

void DxSingleMaterialMeshRenderOrder::RenderMeshFixed( const LPDIRECT3DDEVICEQ pd3dDevice, 
														NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
														MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* pmapType_vecMesh,
														VEC_SINGLE_MATERIAL_MESH* pvecMesh )
{
	DWORD dwColorOP_1, dwAlphaOP_1;
	pd3dDevice->GetTextureStageState( 1, D3DTSS_COLOROP,	&dwColorOP_1 );
	pd3dDevice->GetTextureStageState( 1, D3DTSS_ALPHAOP,	&dwAlphaOP_1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2,	D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

	DWORD dwColorOP_2, dwAlphaOP_2;
	pd3dDevice->GetTextureStageState( 2, D3DTSS_COLOROP,	&dwColorOP_2 );
	pd3dDevice->GetTextureStageState( 2, D3DTSS_ALPHAOP,	&dwAlphaOP_2 );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2,	D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

	DWORD dwColorOP_3, dwAlphaOP_3;
	pd3dDevice->GetTextureStageState( 3, D3DTSS_COLOROP,	&dwColorOP_3 );
	pd3dDevice->GetTextureStageState( 3, D3DTSS_ALPHAOP,	&dwAlphaOP_3 );
	pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG2,	D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

	DWORD dwColorOP_4, dwAlphaOP_4;
	pd3dDevice->GetTextureStageState( 4, D3DTSS_COLOROP,	&dwColorOP_4 );
	pd3dDevice->GetTextureStageState( 4, D3DTSS_ALPHAOP,	&dwAlphaOP_4 );
	pd3dDevice->SetTextureStageState( 4, D3DTSS_COLORARG2,	D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 4, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 4, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 4, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

	DWORD dwColorOP_5, dwAlphaOP_5;
	pd3dDevice->GetTextureStageState( 5, D3DTSS_COLOROP,	&dwColorOP_5 );
	pd3dDevice->GetTextureStageState( 5, D3DTSS_ALPHAOP,	&dwAlphaOP_5 );
	pd3dDevice->SetTextureStageState( 5, D3DTSS_COLORARG2,	D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 5, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 5, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 5, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

	// 렌더링한다.
	if ( pmapType_vecMesh )
	{
		MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = (*pmapType_vecMesh).begin();
		for ( ; iter!=(*pmapType_vecMesh).end(); ++iter )
		{
			VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iter).second;
			for ( DWORD i=0; i<vecMesh.size(); ++i )
			{
				vecMesh[i]->RenderFixed( pd3dDevice, emCullingRenderType );
			}
		}
	}
	else if ( pvecMesh )
	{
		for ( DWORD i=0; i<(*pvecMesh).size(); ++i )
		{
			(*pvecMesh)[i]->RenderFixed( pd3dDevice, emCullingRenderType );
		}
	}
	
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	dwColorOP_1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,	dwAlphaOP_1 );

	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,	dwColorOP_2 );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,	dwAlphaOP_2 );

	pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,	dwColorOP_3 );
	pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,	dwAlphaOP_3 );

	pd3dDevice->SetTextureStageState( 4, D3DTSS_COLOROP,	dwColorOP_4 );
	pd3dDevice->SetTextureStageState( 4, D3DTSS_ALPHAOP,	dwAlphaOP_4 );

	pd3dDevice->SetTextureStageState( 5, D3DTSS_COLOROP,	dwColorOP_5 );
	pd3dDevice->SetTextureStageState( 5, D3DTSS_ALPHAOP,	dwAlphaOP_5 );
}

void DxSingleMaterialMeshRenderOrder::RenderPiece_Instancing( const PieceParameter& sParameter,
															  Material* pMaterial,
															  MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
															  UINT unPass,
															  BOOL bDecal )
{
	if ( RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
	{
		if ( !pMaterial->m_rFXnType_PS30_DF )
			return;

		ID3DXEffect* pFX = pMaterial->m_rFXnType_PS30_DF->m_rFX;
		if ( !pFX )
			return;

		D3DXHANDLE hTechniqueHandle(NULL);
		hTechniqueHandle = pMaterial->m_rFXnType_PS30_DF->m_hTechnique_Instancing;
		if ( !hTechniqueHandle )
			return;

		if ( sParameter.m_pTextureCombined )
		{
			pFX->SetTexture( "g_LightMapDayTex", sParameter.m_pTextureCombined );
		}

		pFX->SetTechnique( hTechniqueHandle );

		sParameter.m_pd3dDevice->SetVertexDeclaration( pMaterial->m_rFXnType_PS30_DF->m_pInstancingDCRT );

		RenderPiece_Instancing_NEWFX
		( 
			sParameter,
			mapType_vecMesh,
			unPass,
			pFX,
			pMaterial->m_rFXnType_PS30_DF->m_emFxType
		);
	}
	else if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM && !bDecal )
	{
		ID3DXEffect* pFX(NULL);
		D3DXHANDLE hTechniqueHandle(NULL);
		NSMaterialManager::FX_TYPE	emFxType(NSMaterialManager::FX_TYPE_DEFAULT);
		{
			pFX = pMaterial->m_sFXnType.m_rFX;
			if ( !pFX )
				return;

			emFxType = pMaterial->m_sFXnType.m_emFxType;
			hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_Instancing;
		}

		if ( hTechniqueHandle )
		{
			if ( sParameter.m_pTextureIndirect )
			{
				pFX->SetTexture( "g_LightMapDayTex", sParameter.m_pTextureIndirect );
				pFX->SetTexture( "g_LightMapTex_DS_1st", sParameter.m_pTextureDirect_Shadow_1 );
				//pFX->SetTexture( "g_LightMapTex_DS_2nd", sParameter.m_pTextureDirect_Shadow_2 );
			}
		}
		else
		{
			return;		
		}

		pFX->SetTechnique( hTechniqueHandle );

		sParameter.m_pd3dDevice->SetVertexDeclaration( pMaterial->m_sFXnType.m_pInstancingDCRT );

		RenderPiece_Instancing_NEWFX
		( 
			sParameter,
			mapType_vecMesh,
			unPass,
			pFX,
			pMaterial->m_sFXnType.m_emFxType
		);
	}
	else if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
	{
		ID3DXEffect* pFX(NULL);
		pFX = pMaterial->m_sFXnType.m_rFX;
		if ( !pFX )
			return;

		D3DXHANDLE hTechniqueHandle(NULL);

		hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_Instancing;

		if ( sParameter.m_pTextureCombined )
		{
			pFX->SetTexture( "g_LightMapDayTex", sParameter.m_pTextureCombined );
		}

		if ( !hTechniqueHandle )
			return;

		pFX->SetTechnique( hTechniqueHandle );

		sParameter.m_pd3dDevice->SetVertexDeclaration( pMaterial->m_sFXnType.m_pInstancingDCRT );

		RenderPiece_Instancing_NEWFX
		( 
			sParameter,
			mapType_vecMesh,
			unPass,
			pFX,
			pMaterial->m_sFXnType.m_emFxType
		);
	}
}

void DxSingleMaterialMeshRenderOrder::RenderPiece_Instancing_NEWFX( const PieceParameter& sParameter,
																	MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
																	UINT unPass,
																	ID3DXEffect* pFX,
																	NSMaterialManager::FX_TYPE emFxType )
{
	NSMaterialManager::SetFX_CommonParameter( pFX, emFxType );

	//NSMaterialManager::SetFX_CommonParameter_Small( pFX, sParameter.m_fAlpha, *sParameter.m_rmatWorld );
	NSMaterialManager::SetFX_CommonParameter_Small( pFX, 1.f, *sParameter.m_rmatWorld );

	// 렌더링한다.
	UINT numPasses;
	pFX->Begin( &numPasses, 0 );
	{
		size_t unPassTemp = unPass;
		if( unPassTemp < numPasses )
		{
			pFX->BeginPass( unPassTemp );
			{
				MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
				for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
				{

					VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
					for ( DWORD i=0; i<vecMesh.size(); ++i )
					{
						vecMesh[i]->RenderPiece_Instancing( sParameter, pFX );
					}
				}
				pFX->EndPass();	
			}
		}
	}
	pFX->End();
}

void DxSingleMaterialMeshRenderOrder::RenderOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, 
												   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
												   LPDIRECT3DTEXTUREQ pTextureCombined, 
												   LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_1,
												   LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_2,
												   LPDIRECT3DTEXTUREQ pTextureIndirect,
												   BOOL bMPShading,
												   D3DXVECTOR4* pvAmbientCube,
												   D3DXVECTOR4& vVoxelColor,
												   const D3DXMATRIX& matWorld )
{
	UINT unPass(0);

	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	switch( NSLightMapFX::g_emRenderType )
	{
	case NSLIGHTMAPDEF::LMRT_LIGHTMAP:
	case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
	case NSLIGHTMAPDEF::LMRT_CHECKER:
		{
			if ( !m_mapLightMap.empty() )
			{
				DWORD dwMagFilter, dwMinFilter;
				pd3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER, &dwMagFilter );
				pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &dwMinFilter );

				switch ( NSLightMapFX::g_emRenderType )
				{
				case NSLIGHTMAPDEF::LMRT_CHECKER:
				case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
					pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
					pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
					break;

				default:
					//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
					//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
					break;
				};

				MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapLightMap.begin();
				for ( ; iter!=m_mapLightMap.end(); ++iter )
				{
					RenderMesh
					(
						pd3dDevice, 
						emCullingRenderType,
						pTextureCombined, 
						pTextureDirect_Shadow_1,
						pTextureDirect_Shadow_2,
						pTextureIndirect,
						(*iter).first,
						(*iter).second,
						bMPShading,
						0,
						TRUE, 
						pvAmbientCube, 
						vVoxelColor,
						matWorld,
						EMTT_NORMAL,
						FALSE
					);
				}

				pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, dwMagFilter );
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
			}
		}
		break;

	default:
		{
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
			{
				MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* parrayMAP[2] = { &m_mapOpaque, &m_mapUnKnownTextureType };
				for ( DWORD i=0; i<2; ++i )
				{
					MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = parrayMAP[i]->begin();
					for ( ; iter!=parrayMAP[i]->end(); ++iter )
					{
						RenderMesh
						(
							pd3dDevice, 
							emCullingRenderType,
							pTextureCombined, 
							pTextureDirect_Shadow_1,
							pTextureDirect_Shadow_2,
							pTextureIndirect,
							(*iter).first,
							(*iter).second,
							bMPShading,
							unPass,
							FALSE, 
							pvAmbientCube, 
							vVoxelColor,
							matWorld,
							EMTT_NORMAL,
							FALSE
						);
					}
				}
			}
			else
			{
				MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* parrayMAP[2] = { &m_mapOpaque, &m_mapUnKnownTextureType };
				for ( DWORD i=0; i<2; ++i )
				{
					MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = parrayMAP[i]->begin();
					for ( ; iter!=parrayMAP[i]->end(); ++iter )
					{
						RenderMeshFixed
						(
							pd3dDevice, 
							emCullingRenderType,
							&(*iter).second,
							NULL
						);
					}
				}
			}
		}
		break;
	}

	pd3dDevice->SetFVF( dwFVF_SAVE );
}

void DxSingleMaterialMeshRenderOrder::RenderHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
													  NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
													  LPDIRECT3DTEXTUREQ pTextureCombined, 
													  LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_1,
													  LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_2,
													  LPDIRECT3DTEXTUREQ pTextureIndirect,
													  BOOL bMPShading,
													  D3DXVECTOR4* pvAmbientCube,
													  D3DXVECTOR4& vVoxelColor,
													  const D3DXMATRIX& matWorld )
{
	UINT unPass(0);

	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapHardAlpha.begin();
		for ( ; iter!=m_mapHardAlpha.end(); ++iter )
		{
			RenderMesh
			(
				pd3dDevice, 
				emCullingRenderType,
				pTextureCombined, 
				pTextureDirect_Shadow_1,
				pTextureDirect_Shadow_2,
				pTextureIndirect,
				(*iter).first,
				(*iter).second,
				bMPShading,
				unPass,
				FALSE, 
				pvAmbientCube, 
				vVoxelColor,
				matWorld,
				EMTT_ALPHA_HARD,
				FALSE
			);
		}
	}
	else
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapHardAlpha.begin();
		for ( ; iter!=m_mapHardAlpha.end(); ++iter )
		{
			RenderMeshFixed
			(
				pd3dDevice, 
				emCullingRenderType,
				&(*iter).second,
				NULL
			);
		}
	}

	pd3dDevice->SetFVF( dwFVF_SAVE );
}

void DxSingleMaterialMeshRenderOrder::RenderSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
													  NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
													  LPDIRECT3DTEXTUREQ pTextureCombined, 
													  LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_1,
													  LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_2,
													  LPDIRECT3DTEXTUREQ pTextureIndirect,
													  BOOL bMPShading,
													  D3DXVECTOR4* pvAmbientCube,
													  D3DXVECTOR4& vVoxelColor,
													  const D3DXMATRIX& matWorld )
{
	UINT unPass(0);

	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapSoftAlpha00.begin();
		for ( ; iter!=m_mapSoftAlpha00.end(); ++iter )
		{
			RenderMesh
			(
				pd3dDevice, 
				emCullingRenderType,
				pTextureCombined, 
				pTextureDirect_Shadow_1,
				pTextureDirect_Shadow_2,
				pTextureIndirect,
				(*iter).first,
				(*iter).second,
				bMPShading,
				unPass,
				FALSE, 
				pvAmbientCube, 
				vVoxelColor,
				matWorld,
				EMTT_ALPHA_SOFT,
				FALSE
			);
		}

		iter = m_mapSoftAlpha01.begin();
		for ( ; iter!=m_mapSoftAlpha01.end(); ++iter )
		{
			RenderMesh
			(
				pd3dDevice, 
				emCullingRenderType,
				pTextureCombined, 
				pTextureDirect_Shadow_1,
				pTextureDirect_Shadow_2,
				pTextureIndirect,
				(*iter).first,
				(*iter).second,
				bMPShading,
				unPass,
				FALSE, 
				pvAmbientCube, 
				vVoxelColor,
				matWorld,
				EMTT_ALPHA_SOFT,
				FALSE
			);
		}

		iter = m_mapSoftAlpha02.begin();
		for ( ; iter!=m_mapSoftAlpha02.end(); ++iter )
		{
			RenderMesh
			(
				pd3dDevice, 
				emCullingRenderType,
				pTextureCombined, 
				pTextureDirect_Shadow_1,
				pTextureDirect_Shadow_2,
				pTextureIndirect,
				(*iter).first,
				(*iter).second,
				bMPShading,
				unPass,
				FALSE, 
				pvAmbientCube, 
				vVoxelColor,
				matWorld,
				EMTT_ALPHA_SOFT,
				FALSE
			);
		}
	}
	else
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapSoftAlpha00.begin();
		for ( ; iter!=m_mapSoftAlpha00.end(); ++iter )
		{
			RenderMeshFixed
			(
				pd3dDevice, 
				emCullingRenderType,
				&(*iter).second,
				NULL
			);
		}

		iter = m_mapSoftAlpha01.begin();
		for ( ; iter!=m_mapSoftAlpha01.end(); ++iter )
		{
			RenderMeshFixed
			(
				pd3dDevice, 
				emCullingRenderType,
				&(*iter).second,
				NULL
			);
		}

		iter = m_mapSoftAlpha02.begin();
		for ( ; iter!=m_mapSoftAlpha02.end(); ++iter )
		{
			RenderMeshFixed
			(
				pd3dDevice, 
				emCullingRenderType,
				&(*iter).second,
				NULL
			);
		}
	}

	pd3dDevice->SetFVF( dwFVF_SAVE );
}

void DxSingleMaterialMeshRenderOrder::RenderDecal( const LPDIRECT3DDEVICEQ pd3dDevice, 
												  NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
												  BOOL bMPShading,
												  D3DXVECTOR4* pvAmbientCube,
												  D3DXVECTOR4& vVoxelColor,
												  const D3DXMATRIX& matWorld )
{
	// Decal은 Shading 을 하지 않는다.
	if ( bMPShading )
		return;

	BOOL bExistDecal(FALSE);
	for ( DWORD i=0; i<DECAL_LAYER_NUM; ++i )
	{
		if ( !m_mapDecalTest[i].empty() )
		{
			bExistDecal = TRUE;
			break;
		}

		if ( !m_mapDecalBlend[i].empty() )
		{
			bExistDecal = TRUE;
			break;
		}

		if ( !m_mapDecalMutiply[i].empty() )
		{
			bExistDecal = TRUE;
			break;
		}
	}

	if ( !bExistDecal )
		return;

	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	// 뒷판에 영향을 안주는 가장 조금 띄우는 수치들
	//D3DRS_DEPTHBIAS = -0.0000002f;
	//D3DRS_SLOPESCALEDEPTHBIAS = -0.02f;
	// 0.3f 만큼 띄워져 있는 Object 가 있을 경우 4600 정도부터 지글거림이 생긴다.
	// [20123.05.15] Geforce 8500 Ti 에서 지글거림이 생겨서 5배를 곱하게 되었다.
	// 이제 어느정도에서 지글거림이 생기는지 테스트를 다시 해봐야 하겠다.

	float fBias = 0.f;
	fBias = -0.0000002f * 5.f;
	//fBias = DxFogMan::GetInstance().m_fFogStart;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) ); // 먼거에 강함
	fBias = -0.02f*5.f;
	//fBias = DxFogMan::GetInstance().m_fFogEnd;
	pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) ); // 가까운거에 강함.

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	// Decal 의 Multiply 기능이 혹 LOD가 되야 될 때 사용되기 위함.
	DWORD dwAlphaTestEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );

	for ( DWORD i=0; i<DECAL_LAYER_NUM; ++i )
	{
		if ( !m_mapDecalTest[i].empty() )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

			MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalTest[i].begin();
			for ( ; iter!=m_mapDecalTest[i].end(); ++iter )
			{
				RenderMesh
				(
					pd3dDevice, 
					emCullingRenderType,
					NULL, 
					NULL,
					NULL,
					NULL,
					(*iter).first,
					(*iter).second,
					bMPShading,
					0,			// Fog 색이 필요없다.
					FALSE, 
					pvAmbientCube, 
					vVoxelColor,
					matWorld,
					EMTT_ALPHA_SOFT,
					TRUE
				);
			}

			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		}

		if ( !m_mapDecalBlend[i].empty() )
		{
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalBlend[i].begin();
			for ( ; iter!=m_mapDecalBlend[i].end(); ++iter )
			{
				RenderMesh
				(
					pd3dDevice, 
					emCullingRenderType,
					NULL, 
					NULL,
					NULL,
					NULL,
					(*iter).first,
					(*iter).second,
					bMPShading,
					0,			// Fog 색이 필요없다.
					FALSE, 
					pvAmbientCube, 
					vVoxelColor,
					matWorld,
					EMTT_ALPHA_SOFT,
					TRUE
				);
			}
		}

		if ( !m_mapDecalMutiply[i].empty() )
		{
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );

			MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalMutiply[i].begin();
			for ( ; iter!=m_mapDecalMutiply[i].end(); ++iter )
			{
				RenderMesh
				(
					pd3dDevice, 
					emCullingRenderType,
					NULL, 
					NULL,
					NULL,
					NULL,
					(*iter).first,
					(*iter).second,
					bMPShading,
					1,			// Fog 색이 필요없다.
					FALSE, 
					pvAmbientCube, 
					vVoxelColor,
					matWorld,
					EMTT_ALPHA_SOFT,
					TRUE
				);
			}
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	fBias = 0.f;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) );
	pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) );

	pd3dDevice->SetFVF( dwFVF_SAVE );
}

void DxSingleMaterialMeshRenderOrder::RenderDecalPass2( const LPDIRECT3DDEVICEQ pd3dDevice, 
													   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
													   D3DXVECTOR4* pvAmbientCube,
													   D3DXVECTOR4& vVoxelColor,
													   const D3DXMATRIX& matWorld )
{
	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	// 뒷판에 영향을 안주는 가장 조금 띄우는 수치들
	//D3DRS_DEPTHBIAS = -0.0000002f;
	//D3DRS_SLOPESCALEDEPTHBIAS = -0.02f;
	// 0.3f 만큼 띄워져 있는 Object 가 있을 경우 4600 정도부터 지글거림이 생긴다.
	// [20123.05.15] Geforce 8500 Ti 에서 지글거림이 생겨서 5배를 곱하게 되었다.
	// 이제 어느정도에서 지글거림이 생기는지 테스트를 다시 해봐야 하겠다.

	float fBias = 0.f;
	fBias = -0.0000002f * 5.f;
	//fBias = DxFogMan::GetInstance().m_fFogStart;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) ); // 먼거에 강함
	fBias = -0.02f*5.f;
	//fBias = DxFogMan::GetInstance().m_fFogEnd;
	pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) ); // 가까운거에 강함.

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	DWORD dwFogColor(0L);
	pd3dDevice->GetRenderState( D3DRS_FOGCOLOR, &dwFogColor );

	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// Fog는 검은색
	pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, 0L );
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalPass2.begin();
		for ( ; iter!=m_mapDecalPass2.end(); ++iter )
		{
			// 데칼은 Shading 을 안하므로 m_pFX 를 바로 사용해도 괜찮다.
			D3DXVECTOR4 vDecalBelndColor(0.f,0.f,0.f,0.f);
			Material* pMaterial = (*iter).first;
			ID3DXEffect* pFX = pMaterial->m_sFXnType.m_rFX;
			pFX->SetVector( "g_vDecalBelndColor", &vDecalBelndColor );

			RenderMesh
			(
				pd3dDevice, 
				emCullingRenderType,
				NULL, 
				NULL,
				NULL,
				NULL,
				pMaterial,
				(*iter).second,
				FALSE,
				2,			// Fog 색이 필요하다.
				FALSE, 
				pvAmbientCube, 
				vVoxelColor,
				matWorld,
				EMTT_ALPHA_SOFT,
				TRUE
			);
		}
	}
	pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, dwFogColor );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	fBias = 0.f;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) );
	pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) );

	pd3dDevice->SetFVF( dwFVF_SAVE );
}

void DxSingleMaterialMeshRenderOrder::RenderWater( const LPDIRECT3DDEVICEQ pd3dDevice, 
												  NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
												  float fPrevHeight, 
												  float &fHeight, 
												  float &fNearLength, 
												  const D3DXMATRIX& matWorld )
{
	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	
	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && !DxEnvironment::GetInstance().IsEditModeDiffuseTexture() )
	{
		MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapWater.begin();
		for ( ; iter!=m_mapWater.end(); ++iter )
		{
			RenderMeshWater
			(
				pd3dDevice, 
				emCullingRenderType,
				(*iter).first,
				(*iter).second,
				fPrevHeight,
				fHeight,
				fNearLength,
				matWorld
			);
		}
	}
	else
	{
		MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapWater.begin();
		for ( ; iter!=m_mapWater.end(); ++iter )
		{
			RenderMeshFixed
			(
				pd3dDevice, 
				emCullingRenderType,
				NULL,
				&(*iter).second
			);
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	pd3dDevice->SetFVF( dwFVF_SAVE );
}

// DxSingleMaterialMeshRenderOrder::RenderPieceWaterCustomSoftAlpha 에서 renderstate 조절
// DxStaticMaterialMeshLightMap::RenderWater_CustomSoftAlpha 에서 renderstate 조절
void DxSingleMaterialMeshRenderOrder::RenderCustomUNLIT_UNFOG( const LPDIRECT3DDEVICEQ pd3dDevice, 
																NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
																const D3DXMATRIX& matWorld )
{
	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 && !DxEnvironment::GetInstance().IsEditModeDiffuseTexture() )
	{
		MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapCustomUNLIT_UNFOG.begin();
		for ( ; iter!=m_mapCustomUNLIT_UNFOG.end(); ++iter )
		{
			RenderMeshCustomMaterial
			(
				pd3dDevice, 
				emCullingRenderType,
				(*iter).first,
				(*iter).second,
				matWorld
			);
		}
	}
	else
	{
		DWORD dwLighting(0L);
		pd3dDevice->GetRenderState( D3DRS_LIGHTING, &dwLighting );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );	

		MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapCustomUNLIT_UNFOG.begin();
		for ( ; iter!=m_mapCustomUNLIT_UNFOG.end(); ++iter )
		{
			RenderMeshFixed
			(
				pd3dDevice, 
				emCullingRenderType,
				NULL,
				&(*iter).second
			);
		}

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, dwLighting );
	}

	pd3dDevice->SetFVF( dwFVF_SAVE );
}

// DxSingleMaterialMeshRenderOrder::RenderPieceWaterCustomSoftAlpha 에서 renderstate 조절
// DxStaticMaterialMeshLightMap::RenderWater_CustomSoftAlpha 에서 renderstate 조절
void DxSingleMaterialMeshRenderOrder::RenderCustomSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
															NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
															const D3DXMATRIX& matWorld )
{
	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );


	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 && !DxEnvironment::GetInstance().IsEditModeDiffuseTexture() )
	{
		MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapCustomSoftAlpha.begin();
		for ( ; iter!=m_mapCustomSoftAlpha.end(); ++iter )
		{
			RenderMeshCustomMaterial
			(
				pd3dDevice, 
				emCullingRenderType,
				(*iter).first,
				(*iter).second,
				matWorld
			);
		}
	}
	else
	{
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapCustomSoftAlpha.begin();
		for ( ; iter!=m_mapCustomSoftAlpha.end(); ++iter )
		{
			RenderMeshFixed
			(
				pd3dDevice, 
				emCullingRenderType,
				NULL,
				&(*iter).second
			);
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	pd3dDevice->SetFVF( dwFVF_SAVE );
}

void DxSingleMaterialMeshRenderOrder::RenderPieceOpaque( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend )
{
	if ( m_mapOpaque.empty() && m_mapLightMap.empty() && m_mapUnKnownTextureType.empty() )
		return;

	LPDIRECT3DDEVICEQ pd3dDevice = sParameter.m_pd3dDevice;

	switch( NSLightMapFX::g_emRenderType )
	{
	case NSLIGHTMAPDEF::LMRT_LIGHTMAP:
	case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
	case NSLIGHTMAPDEF::LMRT_CHECKER:
		{
			if ( !m_mapLightMap.empty() )
			{
				DWORD dwMagFilter, dwMinFilter;
				pd3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER, &dwMagFilter );
				pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &dwMinFilter );

				switch ( NSLightMapFX::g_emRenderType )
				{
				case NSLIGHTMAPDEF::LMRT_CHECKER:
				case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
					pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
					pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
					break;

				default:
					//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
					//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
					break;
				};

				MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapLightMap.begin();
				for ( ; iter!=m_mapLightMap.end(); ++iter )
				{
					RenderPiece
						(
						sParameter,
						(*iter).first,
						(*iter).second,
						bMPShading,
						0,
						TRUE,
						EMTT_NORMAL,
						FALSE
						);
				}

				pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, dwMagFilter );
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
			}
		}
		break;

	default:
		{
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
			{
				MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* parrayMAP[2] = { &m_mapOpaque, &m_mapUnKnownTextureType };
				for ( DWORD i=0; i<2; ++i )
				{
					MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = parrayMAP[i]->begin();
					for ( ; iter!=parrayMAP[i]->end(); ++iter )
					{
						RenderPiece
						(
							sParameter,
							(*iter).first,
							(*iter).second,
							bMPShading,
							0,
							FALSE,
							bBlend ? EMTT_ALPHA_SOFT : EMTT_NORMAL,
							FALSE
						);
					}
				}
			}
			else
			{
				// Fixed Pipeline 을 위함.
				pd3dDevice->SetTransform( D3DTS_WORLD, sParameter.m_rmatWorld );

				MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* parrayMAP[2] = { &m_mapOpaque, &m_mapUnKnownTextureType };
				for ( DWORD i=0; i<2; ++i )
				{
					MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = parrayMAP[i]->begin();
					for ( ; iter!=parrayMAP[i]->end(); ++iter )
					{
						RenderMeshFixed
						(
							pd3dDevice, 
							NSLandThread::TYPE_NO_FIELDMESH,
							&(*iter).second,
							NULL
						);
					}
				}
			}
		}
		break;
	}
}

void DxSingleMaterialMeshRenderOrder::RenderPieceHardAlpha( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend )
{
	if ( m_mapHardAlpha.empty() )
		return;

	UINT unPass(0);

	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapHardAlpha.begin();
		for ( ; iter!=m_mapHardAlpha.end(); ++iter )
		{
			RenderPiece
			(
				sParameter,
				(*iter).first,
				(*iter).second,
				bMPShading,
				unPass,
				FALSE,
				bBlend ? EMTT_ALPHA_SOFT : EMTT_ALPHA_HARD,
				FALSE
			);
		}
	}
	else
	{
		// Fixed Pipeline 을 위함.
		sParameter.m_pd3dDevice->SetTransform( D3DTS_WORLD, sParameter.m_rmatWorld );

		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapHardAlpha.begin();
		for ( ; iter!=m_mapHardAlpha.end(); ++iter )
		{
			RenderMeshFixed
			(
				sParameter.m_pd3dDevice, 
				NSLandThread::TYPE_NO_FIELDMESH,
				&(*iter).second,
				NULL
			);
		}
	}
}

void DxSingleMaterialMeshRenderOrder::RenderPieceSoftAlpha( PieceParameter& sParameter, BOOL bMPShading )
{
	if ( m_mapSoftAlpha00.empty() && m_mapSoftAlpha01.empty() && m_mapSoftAlpha02.empty() )
		return;

	UINT unPass(0);

	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapSoftAlpha00.begin();
		for ( ; iter!=m_mapSoftAlpha00.end(); ++iter )
		{
			RenderPiece
			(
				sParameter,
				(*iter).first,
				(*iter).second,
				bMPShading,
				unPass,
				FALSE,
				EMTT_ALPHA_SOFT,
				FALSE
			);
		}

		iter = m_mapSoftAlpha01.begin();
		for ( ; iter!=m_mapSoftAlpha01.end(); ++iter )
		{
			RenderPiece
			(
				sParameter,
				(*iter).first,
				(*iter).second,
				bMPShading,
				unPass,
				FALSE,
				EMTT_ALPHA_SOFT,
				FALSE
			);
		}

		iter = m_mapSoftAlpha02.begin();
		for ( ; iter!=m_mapSoftAlpha02.end(); ++iter )
		{
			RenderPiece
			(
				sParameter,
				(*iter).first,
				(*iter).second,
				bMPShading,
				unPass,
				FALSE,
				EMTT_ALPHA_SOFT,
				FALSE
			);
		}
	}
	else
	{
		// Fixed Pipeline 을 위함.
		sParameter.m_pd3dDevice->SetTransform( D3DTS_WORLD, sParameter.m_rmatWorld );

		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapSoftAlpha00.begin();
		for ( ; iter!=m_mapSoftAlpha00.end(); ++iter )
		{
			RenderMeshFixed
			(
				sParameter.m_pd3dDevice, 
				NSLandThread::TYPE_NO_FIELDMESH,
				&(*iter).second,
				NULL
			);
		}

		iter = m_mapSoftAlpha01.begin();
		for ( ; iter!=m_mapSoftAlpha01.end(); ++iter )
		{
			RenderMeshFixed
			(
				sParameter.m_pd3dDevice, 
				NSLandThread::TYPE_NO_FIELDMESH,
				&(*iter).second,
				NULL
			);
		}

		iter = m_mapSoftAlpha02.begin();
		for ( ; iter!=m_mapSoftAlpha02.end(); ++iter )
		{
			RenderMeshFixed
			(
				sParameter.m_pd3dDevice, 
				NSLandThread::TYPE_NO_FIELDMESH,
				&(*iter).second,
				NULL
			);
		}
	}
}

void DxSingleMaterialMeshRenderOrder::RenderPieceSoftAlphaForPSF( PieceParameter& sParameter )
{
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapSoftAlpha00.begin();
	for ( ; iter!=m_mapSoftAlpha00.end(); ++iter )
	{
		RenderPiece
		(
			sParameter,
			(*iter).first,
			(*iter).second,
			FALSE,
			0,
			FALSE,
			EMTT_ALPHA_SOFT,
			FALSE
		);
	}

	iter = m_mapSoftAlpha01.begin();
	for ( ; iter!=m_mapSoftAlpha01.end(); ++iter )
	{
		RenderPiece
		(
			sParameter,
			(*iter).first,
			(*iter).second,
			FALSE,
			0,
			FALSE,
			EMTT_ALPHA_SOFT,
			FALSE
		);
	}

	iter = m_mapSoftAlpha02.begin();
	for ( ; iter!=m_mapSoftAlpha02.end(); ++iter )
	{
		RenderPiece
		(
			sParameter,
			(*iter).first,
			(*iter).second,
			FALSE,
			0,
			FALSE,
			EMTT_ALPHA_SOFT,
			FALSE
		);
	}
}

void DxSingleMaterialMeshRenderOrder::RenderPieceDecal( PieceParameter& sParameter )
{
	BOOL bExistDecal(FALSE);
	for ( DWORD i=0; i<DECAL_LAYER_NUM; ++i )
	{
		if ( !m_mapDecalTest[i].empty() )
		{
			bExistDecal = TRUE;
			break;
		}

		if ( !m_mapDecalBlend[i].empty() )
		{
			bExistDecal = TRUE;
			break;
		}

		if ( !m_mapDecalMutiply[i].empty() )
		{
			bExistDecal = TRUE;
			break;
		}
	}

	if ( !bExistDecal )
		return;

	LPDIRECT3DDEVICEQ pd3dDevice = sParameter.m_pd3dDevice;

	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
	//DWORD dwAlphaBlendEnable;
	//pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	//DWORD dwAlphaOP_0;
	//pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP_0 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
	{
		// 뒷판에 영향을 안주는 가장 조금 띄우는 수치들
		//D3DRS_DEPTHBIAS = -0.0000002f;
		//D3DRS_SLOPESCALEDEPTHBIAS = -0.02f;
		// 0.3f 만큼 띄워져 있는 Object 가 있을 경우 4600 정도부터 지글거림이 생긴다.
		// [20123.05.15] Geforce 8500 Ti 에서 지글거림이 생겨서 5배를 곱하게 되었다.
		// 이제 어느정도에서 지글거림이 생기는지 테스트를 다시 해봐야 하겠다.

		float fBias = 0.f;
		fBias = -0.0000002f * 5.f;
		//fBias = DxFogMan::GetInstance().m_fFogStart;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) ); // 먼거에 강함
		fBias = -0.02f*5.f;
		//fBias = DxFogMan::GetInstance().m_fFogEnd;
		pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) ); // 가까운거에 강함.

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		// Decal 의 Multiply 기능이 혹 LOD가 되야 될 때 사용되기 위함.
		DWORD dwAlphaTestEnable;
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );

		for ( DWORD i=0; i<DECAL_LAYER_NUM; ++i )
		{
			if ( !m_mapDecalTest[i].empty() )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

				MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalTest[i].begin();
				for ( ; iter!=m_mapDecalTest[i].end(); ++iter )
				{
					RenderPiece
					(
						sParameter,
						(*iter).first,
						(*iter).second,
						FALSE,
						0,
						FALSE,
						EMTT_ALPHA_SOFT,
						TRUE
					);
				}

				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			}

			if ( !m_mapDecalBlend[i].empty() )
			{
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

				MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalBlend[i].begin();
				for ( ; iter!=m_mapDecalBlend[i].end(); ++iter )
				{
					RenderPiece
					(
						sParameter,
						(*iter).first,
						(*iter).second,
						FALSE,
						0,
						FALSE,
						EMTT_ALPHA_SOFT,
						TRUE
					);
				}
			}

			if ( !m_mapDecalMutiply[i].empty() )
			{
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );

				// (Game,Tool) Mode 로 음영을 없애는 것
				//if ( sParameter.m_nRenderMode == NSMaterialManager::EMRM_MATERIAL )
				//{
					// GameMode
					MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalMutiply[i].begin();
					for ( ; iter!=m_mapDecalMutiply[i].end(); ++iter )
					{
						RenderPiece
						(
							sParameter,
							(*iter).first,
							(*iter).second,
							FALSE,
							1,
							FALSE,
							EMTT_ALPHA_SOFT,
							TRUE
						);
					}
				//}
				//else
				//{
				//	// ToolMode
				//	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalMutiply[i].begin();
				//	for ( ; iter!=m_mapDecalMutiply[i].end(); ++iter )
				//	{
				//		// 데칼은 Shading 을 안하므로 m_pFX 를 바로 사용해도 괜찮다.
				//		D3DXVECTOR4 vDecalBelndColor(1.f,1.f,1.f,1.f);
				//		Material* pMaterial = (*iter).first;
				//		ID3DXEffect* pFX = pMaterial->m_pFX;
				//		pFX->SetVector( "g_vDecalBelndColor", &vDecalBelndColor );

				//		RenderPiece
				//		(
				//			sParameter,
				//			(*iter).first,
				//			(*iter).second,
				//			FALSE,
				//			2,
				//			FALSE
				//		);
				//	}
				//}
			}
		}

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		fBias = 0.f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) );
	}
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		dwAlphaBlendEnable );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwAlphaOP_0 );
}

void DxSingleMaterialMeshRenderOrder::RenderPieceDecalPass2( PieceParameter& sParameter )
{
	if ( m_mapDecalPass2.empty() )
		return;

	LPDIRECT3DDEVICEQ pd3dDevice = sParameter.m_pd3dDevice;

	DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
	{
		DWORD dwFVF_SAVE(0L);
		pd3dDevice->GetFVF( &dwFVF_SAVE );
		pd3dDevice->SetFVF( 0 );

		// 뒷판에 영향을 안주는 가장 조금 띄우는 수치들
		//D3DRS_DEPTHBIAS = -0.0000002f;
		//D3DRS_SLOPESCALEDEPTHBIAS = -0.02f;
		// 0.3f 만큼 띄워져 있는 Object 가 있을 경우 4600 정도부터 지글거림이 생긴다.
		// [20123.05.15] Geforce 8500 Ti 에서 지글거림이 생겨서 5배를 곱하게 되었다.
		// 이제 어느정도에서 지글거림이 생기는지 테스트를 다시 해봐야 하겠다.

		float fBias = 0.f;
		fBias = -0.0000002f * 5.f;
		//fBias = DxFogMan::GetInstance().m_fFogStart;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) ); // 먼거에 강함
		fBias = -0.02f*5.f;
		//fBias = DxFogMan::GetInstance().m_fFogEnd;
		pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) ); // 가까운거에 강함.

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		DWORD dwFogColor(0L);
		pd3dDevice->GetRenderState( D3DRS_FOGCOLOR, &dwFogColor );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		// Fog는 검은색
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, 0L );
		{
			int nPass(0);
			if ( sParameter.m_nRenderMode == NSMaterialManager::EMRM_MATERIAL )
			{
				nPass = 2;
			}
			else
			{
				nPass = 3;
			}

			MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalPass2.begin();
			for ( ; iter!=m_mapDecalPass2.end(); ++iter )
			{
				// 데칼은 Shading 을 안하므로 m_pFX 를 바로 사용해도 괜찮다.
				D3DXVECTOR4 vDecalBelndColor(0.f,0.f,0.f,0.f);
				Material* pMaterial = (*iter).first;
				ID3DXEffect* pFX = pMaterial->m_sFXnType.m_rFX;
				pFX->SetVector( "g_vDecalBelndColor", &vDecalBelndColor );

				RenderPiece
				(
					sParameter,
					(*iter).first,
					(*iter).second,
					FALSE,
					nPass,
					FALSE,
					EMTT_ALPHA_SOFT,
					TRUE
				);
			}
		}
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, dwFogColor );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		fBias = 0.f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) );

		pd3dDevice->SetFVF( dwFVF_SAVE );
	}
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
}

void DxSingleMaterialMeshRenderOrder::RenderPieceCustomUNLIT_UNFOG( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	if ( false == m_mapCustomUNLIT_UNFOG.empty() )
	{
		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );

		DWORD dwFogEnable(0L);
		pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

		RenderCustomUNLIT_UNFOG( pd3dDevice, NSLandThread::TYPE_NO_FIELDMESH, matWorld );

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
	}
}

void DxSingleMaterialMeshRenderOrder::RenderPieceWaterCustomSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	if ( false == m_mapCustomSoftAlpha.empty() )
	{
		float fHeight(0.f);
		float fNearLength(1.f);

		DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
		RenderWater( pd3dDevice, NSLandThread::TYPE_NO_FIELDMESH, 0.f, fHeight, fNearLength, matWorld );
		RenderCustomSoftAlpha( pd3dDevice, NSLandThread::TYPE_NO_FIELDMESH, matWorld );
		DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
	}
}

void DxSingleMaterialMeshRenderOrder::RenderPieceOpaque_Instancing( const PieceParameter& sParameter )
{
	if ( m_mapOpaque.empty() && m_mapUnKnownTextureType.empty() )
		return;

	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* parrayMAP[2] = { &m_mapOpaque, &m_mapUnKnownTextureType };
	for ( DWORD i=0; i<2; ++i )
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = parrayMAP[i]->begin();
		for ( ; iter!=parrayMAP[i]->end(); ++iter )
		{
			RenderPiece_Instancing
			(
				sParameter,
				(*iter).first,
				(*iter).second,
				0,
				FALSE
			);
		}
	}
}

void DxSingleMaterialMeshRenderOrder::RenderPieceHardAlpha_Instancing( const PieceParameter& sParameter )
{
	if ( m_mapHardAlpha.empty() )
		return;

	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapHardAlpha.begin();
	for ( ; iter!=m_mapHardAlpha.end(); ++iter )
	{
		RenderPiece_Instancing
		(
			sParameter,
			(*iter).first,
			(*iter).second,
			0,
			FALSE
		);
	}
}

void DxSingleMaterialMeshRenderOrder::RenderPieceDecal_Instancing( const PieceParameter& sParameter )
{
	BOOL bExistDecal(FALSE);
	for ( DWORD i=0; i<DECAL_LAYER_NUM; ++i )
	{
		if ( !m_mapDecalTest[i].empty() )
		{
			bExistDecal = TRUE;
			break;
		}

		if ( !m_mapDecalBlend[i].empty() )
		{
			bExistDecal = TRUE;
			break;
		}

		if ( !m_mapDecalMutiply[i].empty() )
		{
			bExistDecal = TRUE;
			break;
		}
	}

	if ( !bExistDecal )
		return;

	LPDIRECT3DDEVICEQ pd3dDevice = sParameter.m_pd3dDevice;

	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
	{
		// 뒷판에 영향을 안주는 가장 조금 띄우는 수치들
		//D3DRS_DEPTHBIAS = -0.0000002f;
		//D3DRS_SLOPESCALEDEPTHBIAS = -0.02f;
		// 0.3f 만큼 띄워져 있는 Object 가 있을 경우 4600 정도부터 지글거림이 생긴다.
		// [20123.05.15] Geforce 8500 Ti 에서 지글거림이 생겨서 5배를 곱하게 되었다.
		// 이제 어느정도에서 지글거림이 생기는지 테스트를 다시 해봐야 하겠다.

		float fBias = 0.f;
		fBias = -0.0000002f * 5.f;
		//fBias = DxFogMan::GetInstance().m_fFogStart;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) ); // 먼거에 강함
		fBias = -0.02f*5.f;
		//fBias = DxFogMan::GetInstance().m_fFogEnd;
		pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) ); // 가까운거에 강함.

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		// Decal 의 Multiply 기능이 혹 LOD가 되야 될 때 사용되기 위함.
		DWORD dwAlphaTestEnable;
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );

		for ( DWORD i=0; i<DECAL_LAYER_NUM; ++i )
		{
			if ( !m_mapDecalTest[i].empty() )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

				MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalTest[i].begin();
				for ( ; iter!=m_mapDecalTest[i].end(); ++iter )
				{
					RenderPiece_Instancing
					(
						sParameter,
						(*iter).first,
						(*iter).second,
						0,
						TRUE
					);
				}

				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			}

			if ( !m_mapDecalBlend[i].empty() )
			{
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

				MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalBlend[i].begin();
				for ( ; iter!=m_mapDecalBlend[i].end(); ++iter )
				{
					RenderPiece_Instancing
					(
						sParameter,
						(*iter).first,
						(*iter).second,
						0,
						TRUE
					);
				}
			}

			if ( !m_mapDecalMutiply[i].empty() )
			{
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );

				// GameMode
				MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalMutiply[i].begin();
				for ( ; iter!=m_mapDecalMutiply[i].end(); ++iter )
				{
					RenderPiece_Instancing
					(
						sParameter,
						(*iter).first,
						(*iter).second,
						1,
						TRUE
					);
				}
			}
		}

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		fBias = 0.f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) );
	}
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
}

void DxSingleMaterialMeshRenderOrder::RenderPieceDecalPass2_Instancing( const PieceParameter& sParameter )
{
	if ( m_mapDecalPass2.empty() )
		return;

	LPDIRECT3DDEVICEQ pd3dDevice = sParameter.m_pd3dDevice;

	DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
	{
		DWORD dwFVF_SAVE(0L);
		pd3dDevice->GetFVF( &dwFVF_SAVE );
		pd3dDevice->SetFVF( 0 );

		// 뒷판에 영향을 안주는 가장 조금 띄우는 수치들
		//D3DRS_DEPTHBIAS = -0.0000002f;
		//D3DRS_SLOPESCALEDEPTHBIAS = -0.02f;
		// 0.3f 만큼 띄워져 있는 Object 가 있을 경우 4600 정도부터 지글거림이 생긴다.
		// [20123.05.15] Geforce 8500 Ti 에서 지글거림이 생겨서 5배를 곱하게 되었다.
		// 이제 어느정도에서 지글거림이 생기는지 테스트를 다시 해봐야 하겠다.

		float fBias = 0.f;
		fBias = -0.0000002f * 5.f;
		//fBias = DxFogMan::GetInstance().m_fFogStart;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) ); // 먼거에 강함
		fBias = -0.02f*5.f;
		//fBias = DxFogMan::GetInstance().m_fFogEnd;
		pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) ); // 가까운거에 강함.

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		DWORD dwFogColor(0L);
		pd3dDevice->GetRenderState( D3DRS_FOGCOLOR, &dwFogColor );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		// Fog는 검은색
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, 0L );
		{
			MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapDecalPass2.begin();
			for ( ; iter!=m_mapDecalPass2.end(); ++iter )
			{
				// 데칼은 Shading 을 안하므로 m_pFX 를 바로 사용해도 괜찮다.
				D3DXVECTOR4 vDecalBelndColor(0.f,0.f,0.f,0.f);
				Material* pMaterial = (*iter).first;
				ID3DXEffect* pFX = pMaterial->m_sFXnType.m_rFX;
				pFX->SetVector( "g_vDecalBelndColor", &vDecalBelndColor );

				RenderPiece_Instancing
				(
					sParameter,
					(*iter).first,
					(*iter).second,
					2,
					TRUE
				);
			}
		}
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, dwFogColor );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		fBias = 0.f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) );

		pd3dDevice->SetFVF( dwFVF_SAVE );
	}
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
}

void DxSingleMaterialMeshRenderOrder::RenderCascadeShadowOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	if ( m_mapOpaque.empty() && m_mapUnKnownTextureType.empty() )
		return;

	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* parrayMAP[2] = { &m_mapOpaque, &m_mapUnKnownTextureType };
	for ( DWORD i=0; i<2; ++i )
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = parrayMAP[i]->begin();
		for ( ; iter!=parrayMAP[i]->end(); ++iter )
		{
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh = (*iter).second;
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
			for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
			{
				VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
				for ( DWORD i=0; i<vecMesh.size(); ++i )
				{
					vecMesh[i]->RenderCascadeShadow( pd3dDevice, emCullingRenderType, FALSE );
				}
			}
		}
	}
}

void DxSingleMaterialMeshRenderOrder::RenderCascadeShadowHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	if ( !m_mapHardAlpha.empty() )
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapHardAlpha.begin();
		for ( ; iter!=m_mapHardAlpha.end(); ++iter )
		{
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh = (*iter).second;
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
			for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
			{
				VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
				for ( DWORD i=0; i<vecMesh.size(); ++i )
				{
					vecMesh[i]->RenderCascadeShadow( pd3dDevice, emCullingRenderType, TRUE );
				}
			}
		}
	}
}

void DxSingleMaterialMeshRenderOrder::RenderCascadeShadowOpaque_Instancing( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_mapOpaque.empty() && m_mapUnKnownTextureType.empty() )
		return;

	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* parrayMAP[2] = { &m_mapOpaque, &m_mapUnKnownTextureType };
	for ( DWORD i=0; i<2; ++i )
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = parrayMAP[i]->begin();
		for ( ; iter!=parrayMAP[i]->end(); ++iter )
		{
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh = (*iter).second;
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
			for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
			{
				VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
				for ( DWORD i=0; i<vecMesh.size(); ++i )
				{
					vecMesh[i]->RenderCascadeShadow_Instancing( pd3dDevice, FALSE );
				}
			}
		}
	}
}

void DxSingleMaterialMeshRenderOrder::RenderCascadeShadowHardAlpha_Instancing( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_mapHardAlpha.empty() )
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = m_mapHardAlpha.begin();
		for ( ; iter!=m_mapHardAlpha.end(); ++iter )
		{
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh = (*iter).second;
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
			for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
			{
				VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
				for ( DWORD i=0; i<vecMesh.size(); ++i )
				{
					vecMesh[i]->RenderCascadeShadow_Instancing( pd3dDevice, TRUE );
				}
			}
		}
	}
}

void DxSingleMaterialMeshRenderOrder::Render_CharShadow( const LPDIRECT3DDEVICEQ pd3dDevice, 
													   const CLIPVOLUME* pCV )
{	
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* parrayMAP[2] = { &m_mapOpaque, &m_mapUnKnownTextureType };
	for ( DWORD i=0; i<2; ++i )
	{
		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = parrayMAP[i]->begin();
		for ( ; iter!=parrayMAP[i]->end(); ++iter )
		{
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh = (*iter).second;
			MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
			for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
			{
				VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
				for ( DWORD i=0; i<vecMesh.size(); ++i )
				{
					vecMesh[i]->RenderFixedCV( pd3dDevice, pCV );
				}
			}
		}
	}
}

void DxSingleMaterialMeshRenderOrder::Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, 
													const CLIPVOLUME* pCV )
{
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* parrayMAP[5] = { &m_mapOpaque, &m_mapHardAlpha, &m_mapSoftAlpha00, &m_mapSoftAlpha01, &m_mapSoftAlpha02 };

	if ( pCV )
	{
		// 기본 Base 지형임.
		// 애네들은 내부에서 Query 작업과 렌더링이 필요함.
		for ( DWORD z=0; z<5; ++z )
		{
			MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = parrayMAP[z]->begin();
			for ( ; iter!=parrayMAP[z]->end(); ++iter )
			{
				MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh = (*iter).second;
				MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
				for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
				{
					VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
					for ( DWORD i=0; i<vecMesh.size(); ++i )
					{
						vecMesh[i]->Render_Query( pd3dDevice, *pCV );
					}
				}
			}
		}
	}
	else
	{
		// Piece 들임.
		// 애네들은 Query 를 위한 렌더링만 하면 됨.
		for ( DWORD z=0; z<5; ++z )
		{
			MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iter = parrayMAP[z]->begin();
			for ( ; iter!=parrayMAP[z]->end(); ++iter )
			{
				MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh = (*iter).second;
				MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER iterA = mapType_vecMesh.begin();
				for ( ; iterA!=mapType_vecMesh.end(); ++iterA )
				{
					VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
					for ( DWORD i=0; i<vecMesh.size(); ++i )
					{
						vecMesh[i]->RenderFixed( pd3dDevice, NSLandThread::TYPE_NO_FIELDMESH );
					}
				}
			}
		}
	}
}

void DxSingleMaterialMeshRenderOrder::SetTextureTypeForUnKnownTexture( LPDIRECT3DDEVICEQ pd3dDevice, DWORD& dwAddTexType )
{
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH::iterator iter = m_mapUnKnownTextureType.begin();
	for ( ; iter!=m_mapUnKnownTextureType.end(); )
	{
		MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& sData = (*iter).second;
		MAP_TYPE_VEC_SINGLE_MATERIAL_MESH::iterator iterA = sData.begin();
		for ( ; iterA!=sData.end(); )
		{
			VEC_SINGLE_MATERIAL_MESH& vecMesh = (*iterA).second;
			VEC_SINGLE_MATERIAL_MESH::iterator iterB = vecMesh.begin();
			for ( ; iterB!=vecMesh.end(); )
			{
				if ( (*iterB)->IsTextureLoadingComplete() )
				{
					if ( (*iterB)->RenderOrderSetting( pd3dDevice, this, dwAddTexType, FALSE, TRUE ) )
					{
						iterB = vecMesh.erase( iterB );
					}
					else
					{
						++iterB;
					}
				}
				else
				{
					++iterB;
				}
			}

			if ( vecMesh.empty() )
			{
				iterA = sData.erase( iterA );
			}
			else
			{
				++iterA;
			}
		}

		if ( sData.empty() )
		{
			iter = m_mapUnKnownTextureType.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

BOOL DxSingleMaterialMeshRenderOrder::IsTextureTypeComplete()
{
	if ( m_mapUnKnownTextureType.empty() )
		return TRUE;

	return FALSE;
}

//BOOL DxSingleMaterialMeshRenderOrder::IsValidData()
//{
//	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* parrayMAP[5] = { &m_mapOpaque, &m_mapHardAlpha, &m_mapSoftAlpha00, &m_mapSoftAlpha01, &m_mapSoftAlpha02 };
//	for ( DWORD i=0; i<5; ++i )
//	{
//		if ( !parrayMAP[i]->empty() )
//			return TRUE;
//	}
//
//	for ( DWORD i=0; i<DECAL_LAYER_NUM; ++i )
//	{
//		if ( !m_mapDecalTest[i].empty() )
//			return TRUE;
//
//		if ( !m_mapDecalBlend[i].empty() )
//			return TRUE;
//
//		if ( !m_mapDecalMutiply[i].empty() )
//			return TRUE;
//	}
//
//	if ( !m_mapDecalPass2.empty() )
//		return TRUE;
//
//	if ( !m_mapWater.empty() )
//		return TRUE;
//
//	if ( !m_mapCustomSoftAlpha.empty() )
//		return TRUE;
//
//	return FALSE;
//}