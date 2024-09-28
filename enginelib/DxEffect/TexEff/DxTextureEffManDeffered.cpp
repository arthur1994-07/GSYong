#include "pch.h"

#include "../../../SigmaCore/Util/Util.h"

#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/Material/NSMaterialSetting.h"
#include "../../DxTools/Material/MaterialMesh.h"
#include "../../DxTools/Material/MaterialFieldDef.h"

#include "DxTextureEffMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void DxTextureEffMan::Render_Deffered_GPass( LPDIRECT3DDEVICEQ pd3dDevice, 
											DxMeshes* pMeshs, 
											const D3DXMATRIX& matWorld, 
											const CMaterialSetting* pMaterialSetting, 
											DWORD dwPass, 
											BOOL bAlpha,
											BOOL bForwardPass )	// bForwardPass : TRUE - Pass2,Water,CustomAlpha µî Deffered ·Î ·»´õ¸µ ¾ÈÇÏ´Â °Íµé..
{
	LPD3DXMESH	pLocalMesh = pMeshs->m_pLocalMesh;
	if( !pLocalMesh )	
		return;

	// Attrib ÀÌ ¾øÀ¸¸é ¸¸µé¾îÁØ´Ù.
	if( !pMeshs->m_pAttrib )
	{
		pLocalMesh->GetAttributeTable( NULL, &pMeshs->m_dwAttrib );

		SAFE_DELETE_ARRAY( pMeshs->m_pAttrib );
		pMeshs->m_pAttrib = new D3DXATTRIBUTERANGE[ pMeshs->m_dwAttrib ];
		pLocalMesh->GetAttributeTable( pMeshs->m_pAttrib, &pMeshs->m_dwAttrib );
	}

	if( pMeshs->m_pAttrib )
	{
		DWORD	dwMatIndex = 0;
		for ( DWORD i=0; i<pMeshs->m_dwAttrib; ++i )
		{
			dwMatIndex = pMeshs->m_pAttrib[i].AttribId;
			DXMTRLTYPE emMETR = pMeshs->m_exMaterials[dwMatIndex].MtrlType;

			if ( bAlpha )
			{
				if ( emMETR == MATERIAL_NORMAL || emMETR == MATERIAL_ALPHA_HARD )		
					continue;
			}
			else
			{
				if ( !(emMETR == MATERIAL_NORMAL || emMETR == MATERIAL_ALPHA_HARD) )		
					continue;
			}

			const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( pMeshs->m_vecMaterialName[dwMatIndex] );
			if ( pSetting )
			{
				if ( pSetting->m_emDecalType != NSMaterialSetting::DECAL_TYPE_NULL )
				{
					continue;
				}
			}

			//	Note : 
			pd3dDevice->SetMaterial( &pMeshs->m_rgMaterials[dwMatIndex] );
			pd3dDevice->SetTexture( 0, pMeshs->m_pTextureRes[dwMatIndex].GetTexture() );

			NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
			if ( pSetting && pSetting->m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_VOXELCOLOR )
			{
				emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
			}

			// Note : NSMaterialManager ¿¡ ÀÖ´Â mat ÀÌ¶ó¸é NSMaterialManager::Render ¿¡¼­ ¸ðµç ÀÛ¾÷À» ÇÑ´Ù.
			MATERIAL_DATA sMaterial_Data;
			float fCubeMapValue = pSetting ? (pSetting->m_bCubeMap ? pSetting->m_fCubeMapValue : 0.f) : 0.f;
			if ( NSMaterialManager::ActiveDefferedForTool( pd3dDevice, 
															pMeshs->m_vecMaterialName[dwMatIndex], 
															NSMaterialManager::EMRM_TOOL, 
															emUnLMRenderMode,
															sMaterial_Data,
															FALSE,
															matWorld, 
															1.f,
															pMeshs->m_bVertexColor,
															MaterialValue( fCubeMapValue, pSetting->m_fSpotSpecPower, pSetting->m_fSpotSpecMulti ),
															pSetting->m_bNormalMap ) )
			{
				if ( sMaterial_Data.m_bDefaultMaterial )
				{
					ID3DXEffect* pFX = sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX;
					if ( !pFX )
						return;

					pFX->SetTexture( "g_BaseTexture", pMeshs->m_pTextureRes[dwMatIndex].GetTexture() );

					if ( pSetting ? pSetting->m_bMaskCubeMapSpecularMap || pSetting->m_bNormalMap : FALSE )
					{
						// ¾ø´Ù¸é ·ÎµùÇØº»´Ù.
						if ( !pMeshs->m_pMaskTexRes[dwMatIndex].GetTexture() )
						{
							TSTRING strName = pMeshs->m_pTextureRes[dwMatIndex].GetFileName();

							strName = sc::util::GetSpecularName( strName, _T("_mask") );

							pMeshs->m_pMaskTexRes[dwMatIndex] = TextureManager::GetInstance().LoadTexture(
								strName,
								false,
								TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
								0,
								D3DFMT_UNKNOWN,
								TEXTURE_RESIZE_1024,
								TEXTURE_LAND,
								false,
								FALSE );
						}

						// ÀÖ´Ù¸é Mask ¸ÊÀ» ¾ø´Ù¸é Èò»öÀ¸·Î ÀÛµ¿µÈ´Ù.
						if ( pMeshs->m_pMaskTexRes[dwMatIndex].GetTexture() )
						{
							pFX->SetTexture( "g_MaskTexture", pMeshs->m_pMaskTexRes[dwMatIndex].GetTexture() );
						}
						else
						{
							pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
						}
					}
					else
					{
						pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
					}

					switch ( pMeshs->m_pTextureRes[dwMatIndex].GetAlphaType() )
					{
					case EMTT_NORMAL:
						pMeshs->m_exMaterials[dwMatIndex].MtrlType = MATERIAL_NORMAL;
						break;
					case EMTT_ALPHA_HARD:
						pMeshs->m_exMaterials[dwMatIndex].MtrlType = MATERIAL_ALPHA_HARD;
						break;
					case EMTT_ALPHA_SOFT:
					case EMTT_ALPHA_SOFT01:
					case EMTT_ALPHA_SOFT02:
						pMeshs->m_exMaterials[dwMatIndex].MtrlType = MATERIAL_ALPHA_SOFT;
						break;
					};

					pFX->SetFloat( "g_fDecalNormal", 1.f );
				}

				int nTexType(0);
				DWORD dwAlphaTestEnable(0L);
				DWORD dwAlphaBlendEnable(0L);
				if ( emMETR == MATERIAL_ALPHA_HARD )
				{
					nTexType = 1;

					pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
				}

				pMeshs->m_spMaterialMeshArray->Render( pd3dDevice, i, sMaterial_Data, dwPass, bForwardPass );

				if ( nTexType == 1 )
				{
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
				}
			}
		}
	}
}

void DxTextureEffMan::Render_Deffered_GPass_Decal( LPDIRECT3DDEVICEQ pd3dDevice, 
												  DxMeshes* pMeshs, 
												  const D3DXMATRIX& matWorld, 
												  const CMaterialSetting* pMaterialSetting, 
												  DWORD dwOrder,
												  BOOL bForwardPass )	// bForwardPass : TRUE - Pass2,Water,CustomAlpha µî Deffered ·Î ·»´õ¸µ ¾ÈÇÏ´Â °Íµé..
{
	LPD3DXMESH	pLocalMesh = pMeshs->m_pLocalMesh;
	if( !pLocalMesh )	
		return;

	DWORD dwAlphaBlendEnable;
	DWORD dwAlphaTestEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );

	// Attrib ÀÌ ¾øÀ¸¸é ¸¸µé¾îÁØ´Ù.
	if( !pMeshs->m_pAttrib )
	{
		pLocalMesh->GetAttributeTable( NULL, &pMeshs->m_dwAttrib );

		SAFE_DELETE_ARRAY( pMeshs->m_pAttrib );
		pMeshs->m_pAttrib = new D3DXATTRIBUTERANGE[ pMeshs->m_dwAttrib ];
		pLocalMesh->GetAttributeTable( pMeshs->m_pAttrib, &pMeshs->m_dwAttrib );
	}

	if( pMeshs->m_pAttrib )
	{
		DWORD	dwMatIndex = 0;
		for ( DWORD i=0; i<pMeshs->m_dwAttrib; ++i )
		{
			dwMatIndex = pMeshs->m_pAttrib[i].AttribId;

			DWORD dwDecalType = 0;
			BOOL bDecalNormal(FALSE);
			const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( pMeshs->m_vecMaterialName[dwMatIndex] );
			if ( pSetting )
			{
				// ·»´õ Order
				if ( pSetting->m_nDecalOrder != static_cast<int>(dwOrder) )
					continue;

				// Blend Option
				if ( pSetting->m_emDecalType == NSMaterialSetting::DECAL_TYPE_NULL )
				{
					continue;
				}
				else if ( pSetting->m_emDecalType == NSMaterialSetting::DECAL_TYPE_ALPHABLEND )
				{
					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
					pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				}
				else if ( pSetting->m_emDecalType == NSMaterialSetting::DECAL_TYPE_ALPHATEST )
				{
					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				}
				else if ( pSetting->m_emDecalType == NSMaterialSetting::DECAL_TYPE_MULTIPLY )
				{
					dwDecalType = 1;
					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
					pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
					pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
				}
				else if ( pSetting->m_emDecalType == NSMaterialSetting::DECAL_TYPE_ADD )
				{
					dwDecalType = 2;
					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
					pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
					pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				}

				if ( pSetting->m_bDecalNormal )
				{
					bDecalNormal = TRUE;
				}
			}

			NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
			if ( pSetting && pSetting->m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_VOXELCOLOR )
			{
				emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
			}

			// Note : NSMaterialManager ¿¡ ÀÖ´Â mat ÀÌ¶ó¸é NSMaterialManager::Render ¿¡¼­ ¸ðµç ÀÛ¾÷À» ÇÑ´Ù.
			MATERIAL_DATA sMaterial_Data;
			float fCubeMapValue = pSetting ? (pSetting->m_bCubeMap ? pSetting->m_fCubeMapValue : 0.f) : 0.f;
			if ( NSMaterialManager::ActiveDefferedForTool( pd3dDevice, 
															pMeshs->m_vecMaterialName[dwMatIndex], 
															NSMaterialManager::EMRM_TOOL, 
															emUnLMRenderMode,
															sMaterial_Data,
															TRUE,
															matWorld, 
															1.f,
															pMeshs->m_bVertexColor,
															MaterialValue( fCubeMapValue, pSetting->m_fSpotSpecPower, pSetting->m_fSpotSpecMulti ),
															pSetting->m_bNormalMap ) )
			{
				ID3DXEffect* pFX = sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX;
				if ( !pFX )
					return;

				if ( sMaterial_Data.m_bDefaultMaterial )
				{
					pFX->SetTexture( "g_BaseTexture", pMeshs->m_pTextureRes[dwMatIndex].GetTexture() );

					if ( pSetting ? pSetting->m_bMaskCubeMapSpecularMap || pSetting->m_bNormalMap : FALSE )
					{
						// ¾ø´Ù¸é ·ÎµùÇØº»´Ù.
						if ( !pMeshs->m_pMaskTexRes[dwMatIndex].GetTexture() )
						{
							TSTRING strName = pMeshs->m_pTextureRes[dwMatIndex].GetFileName();

							strName = sc::util::GetSpecularName( strName, _T("_mask") );

							pMeshs->m_pMaskTexRes[dwMatIndex] = TextureManager::GetInstance().LoadTexture(
								strName,
								false,
								TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
								0,
								D3DFMT_UNKNOWN,
								TEXTURE_RESIZE_1024,
								TEXTURE_LAND,
								false,
								FALSE );
						}

						// ÀÖ´Ù¸é Mask ¸ÊÀ» ¾ø´Ù¸é Èò»öÀ¸·Î ÀÛµ¿µÈ´Ù.
						if ( pMeshs->m_pMaskTexRes[dwMatIndex].GetTexture() )
						{
							pFX->SetTexture( "g_MaskTexture", pMeshs->m_pMaskTexRes[dwMatIndex].GetTexture() );
						}
						else
						{
							pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
						}
					}
					else
					{
						pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
					}
				}

				if ( bDecalNormal )
				{
					pFX->SetFloat( "g_fDecalNormal", 1.f );
				}
				else
				{
					pFX->SetFloat( "g_fDecalNormal", 0.f );
				}

				// µÞÆÇ¿¡ ¿µÇâÀ» ¾ÈÁÖ´Â °¡Àå Á¶±Ý ¶ç¿ì´Â ¼öÄ¡µé
				//D3DRS_DEPTHBIAS = -0.0000002f;
				//D3DRS_SLOPESCALEDEPTHBIAS = -0.02f;
				// 0.3f ¸¸Å­ ¶ç¿öÁ® ÀÖ´Â Object °¡ ÀÖÀ» °æ¿ì 4600 Á¤µµºÎÅÍ Áö±Û°Å¸²ÀÌ »ý±ä´Ù.

				float fBias = 0.f;
				fBias = -0.0000002f;
				//fBias = DxFogMan::GetInstance().m_fFogStart;
				pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) ); // ¸Õ°Å¿¡ °­ÇÔ
				fBias = -0.02f;
				//fBias = DxFogMan::GetInstance().m_fFogEnd;
				pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) ); // °¡±î¿î°Å¿¡ °­ÇÔ.

				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

				{
					pMeshs->m_spMaterialMeshArray->Render( pd3dDevice, i, sMaterial_Data, dwDecalType, bForwardPass );
				}

				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

				fBias = 0.f;
				pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) );
				pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) );
			}
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
}

//void DxTextureEffMan::Render_Forward_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs, const D3DXMATRIX& matWorld, const CMaterialSetting* pMaterialSetting )
//{
//	LPD3DXMESH	pLocalMesh = pMeshs->m_pLocalMesh;
//	if( !pLocalMesh )	
//		return;
//
//	// Attrib ÀÌ ¾øÀ¸¸é ¸¸µé¾îÁØ´Ù.
//	if( !pMeshs->m_pAttrib )
//	{
//		pLocalMesh->GetAttributeTable( NULL, &pMeshs->m_dwAttrib );
//
//		SAFE_DELETE_ARRAY( pMeshs->m_pAttrib );
//		pMeshs->m_pAttrib = new D3DXATTRIBUTERANGE[ pMeshs->m_dwAttrib ];
//		pLocalMesh->GetAttributeTable( pMeshs->m_pAttrib, &pMeshs->m_dwAttrib );
//	}
//
//	DWORD	dwMatIndex = 0;
//	for ( DWORD i=0; i<pMeshs->m_dwAttrib; ++i )
//	{
//		dwMatIndex = pMeshs->m_pAttrib[i].AttribId;
//		DXMTRLTYPE emMETR = pMeshs->m_exMaterials[dwMatIndex].MtrlType;
//
//		const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( pMeshs->m_vecMaterialName[dwMatIndex] );
//		if ( pSetting )
//		{
//			if ( pSetting->m_emDecalType != NSMaterialSetting::DECAL_TYPE_NULL )
//			{
//				continue;
//			}
//		}
//
//		//TSTRING strTextureName = pMeshs->m_pTextureRes[dwMatIndex].GetFileName();
//		//strTextureName = sc::util::ChangeExtName( strTextureName, _T("dds") );
//		//MAPTEXLIST_CITER citer = m_mapTexList.find( strTextureName );
//		//if ( citer != m_mapTexList.end() )
//		//{
//		//	switch ( citer->second.m_dwTexType )
//		//	{
//		//	case TEXEFF_OPAQUE:
//		//		emMETR = MATERIAL_NORMAL;
//		//		break;
//
//		//	case TEXEFF_HARDALPHA:
//		//		emMETR = MATERIAL_ALPHA_HARD;
//		//		break;
//
//		//	case TEXEFF_SOFTALPHA:
//		//	case TEXEFF_SOFTALPHA_1:
//		//	case TEXEFF_SOFTALPHA_2:
//		//		emMETR = MATERIAL_ALPHA_SOFT;
//		//		break;
//		//	}
//		//}
//
//		if ( emMETR==MATERIAL_NORMAL || emMETR==MATERIAL_ALPHA_HARD )
//			continue;
//
//		//	Note : ë©”í„°ë¦¬ì–¼, ?ìŠ¤ì³?ì§€??
//		pd3dDevice->SetMaterial( &pMeshs->m_rgMaterials[dwMatIndex] );
//		pd3dDevice->SetTexture( 0, pMeshs->m_pTextureRes[dwMatIndex].GetTexture() );
//
//		NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
//		if ( pSetting && pSetting->m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_VOXELCOLOR )
//		{
//			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
//		}
//
//		// Note : NSMaterialManager ¿¡ ÀÖ´Â mat ÀÌ¶ó¸é NSMaterialManager::Render ¿¡¼­ ¸ðµç ÀÛ¾÷À» ÇÑ´Ù.
//		MATERIAL_DATA sMaterial_Data;
//		float fCubeMapValue = pSetting ? (pSetting->m_bCubeMap ? pSetting->m_fCubeMapValue : 0.f) : 0.f;
//		if ( NSMaterialManager::Active( pd3dDevice, 
//										pMeshs->m_vecMaterialName[dwMatIndex], 
//										NSMaterialManager::EMRM_TOOL, 
//										emUnLMRenderMode,
//										sMaterial_Data,
//										FALSE,
//										matWorld, 
//										1.f,
//										pMeshs->m_bVertexColor,
//										MaterialValue( fCubeMapValue, pSetting->m_fSpotSpecPower, pSetting->m_fSpotSpecMulti ),
//										pSetting->m_bNormalMap ) )
//		{
//			ID3DXEffect* pFX = sMaterial_Data.m_rMaterial->m_sFXnType.m_pFX;
//			if ( !pFX )
//				return;
//
//			if ( sMaterial_Data.m_bDefaultMaterial )
//			{
//				pFX->SetTexture( "g_BaseTexture", pMeshs->m_pTextureRes[dwMatIndex].GetTexture() );
//
//				if ( pSetting ? pSetting->m_bMaskCubeMapSpecularMap || pSetting->m_bNormalMap : FALSE )
//				{
//					// ¾ø´Ù¸é ·ÎµùÇØº»´Ù.
//					if ( !pMeshs->m_pMaskTexRes[dwMatIndex].GetTexture() )
//					{
//						TSTRING strName = pMeshs->m_pTextureRes[dwMatIndex].GetFileName();
//
//						strName = sc::util::GetSpecularName( strName, _T("_mask") );
//
//						pMeshs->m_pMaskTexRes[dwMatIndex] = TextureManager::GetInstance().LoadTexture(
//							strName,
//							false,
//							TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
//							0,
//							D3DFMT_UNKNOWN,
//							TEXTURE_RESIZE_1024,
//							TEXTURE_LAND,
//							false,
//							FALSE );
//					}
//
//					// ÀÖ´Ù¸é Mask ¸ÊÀ» ¾ø´Ù¸é Èò»öÀ¸·Î ÀÛµ¿µÈ´Ù.
//					if ( pMeshs->m_pMaskTexRes[dwMatIndex].GetTexture() )
//					{
//						pFX->SetTexture( "g_MaskTexture", pMeshs->m_pMaskTexRes[dwMatIndex].GetTexture() );
//					}
//					else
//					{
//						pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
//					}
//				}
//				else
//				{
//					pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
//				}
//			}
//
//			pMeshs->m_spMaterialMeshArray->Render( pd3dDevice, i, sMaterial_Data, 0 );
//		}
//	}
//}