#include "pch.h"

#include "../../../SigmaCore/Util/Util.h"
#include "../../../VisualMaterialLib/Util/DxTangentMesh.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/Material/NSMaterialSetting.h"
#include "../../DxTools/Material/MaterialMesh.h"
#include "../../DxTools/Material/MaterialFieldDef.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/DxCubeMap.h"
#include "../../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "./DxTexEff.h"

#include "DxTextureEffMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void DxTextureEffMan::FrameMove( float fElapsedTime )
{
	MAPTEXEFF_ITER iter = m_mapTexEff.begin();
	for( ; iter!=m_mapTexEff.end(); ++iter )
	{
		DxTexEffBase* pEff = (*iter).second;
		while( pEff )
		{
            pEff->FrameMove( fElapsedTime );				
			pEff = pEff->m_pNext;
		}
	}
}

void DxTextureEffMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs, const D3DXMATRIX& matWorld, BOOL bRealPL, const CMaterialSetting* pMaterialSetting )
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

			const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( pMeshs->m_vecMaterialName[dwMatIndex] );
			if ( pSetting )
			{
				if ( pSetting->m_emDecalType != NSMaterialSetting::DECAL_TYPE_NULL )
				{
					continue;
				}
			}

			TSTRING strTextureName = pMeshs->m_pTextureRes[dwMatIndex].GetFileName();
			strTextureName = sc::util::ChangeExtName( strTextureName, _T("dds") );
				MAPTEXLIST_CITER citer = m_mapTexList.find( strTextureName );
			if ( citer != m_mapTexList.end() )
			{
				switch ( citer->second.m_dwTexType )
				{
				case TEXEFF_OPAQUE:
					emMETR = MATERIAL_NORMAL;
					break;

				case TEXEFF_HARDALPHA:
					emMETR = MATERIAL_ALPHA_HARD;
					break;

				case TEXEFF_SOFTALPHA:
				case TEXEFF_SOFTALPHA_1:
				case TEXEFF_SOFTALPHA_2:
					emMETR = MATERIAL_ALPHA_SOFT;
					break;
				}
			}

			if ( emMETR != MATERIAL_NORMAL )		
                continue;

			//	Note : 
			pd3dDevice->SetMaterial( &pMeshs->m_rgMaterials[dwMatIndex] );
			pd3dDevice->SetTexture( 0, pMeshs->m_pTextureRes[dwMatIndex].GetTexture() );

			NSLightMapFX::LightMapDetail(1.f);
			NSLightMapFX::SetLightMapUV_Multiply( pMeshs->m_wwLightMapUserUV_TextureSize );

			NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
			if ( pSetting && pSetting->m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_VOXELCOLOR )
			{
				emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
			}

			// Note : NSMaterialManager ¿¡ ÀÖ´Â mat ÀÌ¶ó¸é NSMaterialManager::Render ¿¡¼­ ¸ðµç ÀÛ¾÷À» ÇÑ´Ù.
			MATERIAL_DATA sMaterial_Data;
			float fCubeMapValue = pSetting ? (pSetting->m_bCubeMap ? pSetting->m_fCubeMapValue : 0.f) : 0.f;
			if ( NSMaterialManager::Active( pd3dDevice, 
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

				if ( pMeshs->m_spMaterialMeshArray->Render( pd3dDevice, i, sMaterial_Data, 0, FALSE ) )
				{
					continue;
				}
			}

			// Note : 
			MAPTEXEFF_ITER iter = m_mapTexEff.find( pMeshs->m_strTextureFiles[dwMatIndex].GetString() );
			if( iter==m_mapTexEff.end() )
			{
                // [2010.03.31] DraSubSet °ú DrawIndexedPrimitive ´Â ´Ù¸£´Ù.
                // DraSubSet( dwMatIndex ); ·Î »ç¿ëµÇ¸é.
                // DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 
				//			            0,
				//			            sAttrib[i].VertexStart,
				//			            sAttrib[i].VertexCount,
				//			            sAttrib[i].FaceStart*3,
				//			            sAttrib[i].FaceCount );  ·Î »ç¿ëµÈ´Ù.	- m_pAttrib °¡ ±âÁØÀÓ.
                //pLocalMesh->DrawSubset( dwMatIndex ); // sAttrib[i].AttribId DrawSubset ÀÏ °æ¿ì - m_cMaterials ÀÌ ±âÁØÀÓ.
                NSVERTEXNORTEX_FX::SetBaseTexture( pMeshs->m_pTextureRes[dwMatIndex].GetTexture() );
                NSVERTEXNORTEX_FX::Render( pd3dDevice, pLocalMesh, dwMatIndex );
                
				continue;
			}

            // Note : VisualMaterialÀ» »ç¿ëÇÑ´Ù¸é, ¿øº»Àº ±×¸®Áö ¾Ê°í DxTexEffBase¿¡¼­ ±×¸°´Ù.
            BOOL bVisualMaterial = FALSE;
            if ( DxEffectMan::GetInstance().IsEnableCharPixelShader() )
            {
			    DxTexEffBase* pEff = (*iter).second;
			    while( pEff )
			    {
                    switch( pEff->GetTypeID() )
				    {
                    case DEF_TEXEFF_VISUAL_MATERIAL:
                        bVisualMaterial = TRUE;

                        if( !pMeshs->m_spTangentMesh.get() )
                        {
                            pMeshs->m_spTangentMesh = std::tr1::shared_ptr<DxTangentMesh>(new DxTangentMesh);
                            pMeshs->m_spTangentMesh->CreateColorMesh( pd3dDevice, pMeshs->m_pLocalMesh );
                        }

                        pEff->RenderColor( pd3dDevice, pMeshs->m_spTangentMesh.get(), i, bRealPL, matWorld, 1.f );

                        break;
                    };
                    pEff = pEff->m_pNext;
                };
            }

            // VisualMaterial ÀÌ ¾øÀ» °æ¿ì¸¸, °¡´ÉÇÏ´Ù.
            if ( !bVisualMaterial )
            {
			    // Note : Base Render
			    //pLocalMesh->DrawSubset( dwMatIndex );
                NSVERTEXNORTEX_FX::SetBaseTexture( pMeshs->m_pTextureRes[dwMatIndex].GetTexture() );
                NSVERTEXNORTEX_FX::Render( pd3dDevice, pLocalMesh, dwMatIndex );

			    // Note : 
			    DxTexEffBase* pEff = (*iter).second;
			    while( pEff )
			    {
				    switch( pEff->GetTypeID() )
				    {
				    case DEF_TEXEFF_DIFFUSE:
				    case DEF_TEXEFF_SPECULAR:
					    pEff->Render( pd3dDevice, pLocalMesh, dwMatIndex );
					    break;

                    case DEF_TEXEFF_FLOWUV:
                    case DEF_TEXEFF_ROTATE:
                        {
                            if( !pMeshs->m_pEffVB )	
                                pMeshs->MakeEffectMeshs( pd3dDevice, VERTEX::FVF );

                            pEff->Render( pd3dDevice, pMeshs->m_pEffVB, pMeshs->m_pEffIB, pMeshs->m_pAttrib, 
									        i, pMeshs->m_pVertSrc, pMeshs->m_dwVert );
                        }
					    break;
				    };
    					
				    pEff = pEff->m_pNext;
			    }
            }
		}
	}
}

// ÀÌÀü µ¥ÀÌÅÍ¸¦ »ì¸®±â À§ÇØ Á¸Àç.
// HardAlpha ¿Í SoftAlpha µÑ´Ù ·»´õ¸µÇÑ´Ù.
void DxTextureEffMan::RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs, const BOOL bShadow, const D3DXMATRIX& matWorld, BOOL bRealPL, const CMaterialSetting* pMaterialSetting )
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

	DWORD	dwMatIndex = 0;
	for ( DWORD i=0; i<pMeshs->m_dwAttrib; ++i )
	{
        dwMatIndex = pMeshs->m_pAttrib[i].AttribId;
		DXMTRLTYPE emMETR = pMeshs->m_exMaterials[dwMatIndex].MtrlType;

		const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( pMeshs->m_vecMaterialName[dwMatIndex] );
		if ( pSetting )
		{
			if ( pSetting->m_emDecalType != NSMaterialSetting::DECAL_TYPE_NULL )
			{
				continue;
			}
		}

		TSTRING strTextureName = pMeshs->m_pTextureRes[dwMatIndex].GetFileName();
		strTextureName = sc::util::ChangeExtName( strTextureName, _T("dds") );
		MAPTEXLIST_CITER citer = m_mapTexList.find( strTextureName );
		if ( citer != m_mapTexList.end() )
		{
			switch ( citer->second.m_dwTexType )
			{
			case TEXEFF_OPAQUE:
				emMETR = MATERIAL_NORMAL;
				break;

			case TEXEFF_HARDALPHA:
				emMETR = MATERIAL_ALPHA_HARD;
				break;

			case TEXEFF_SOFTALPHA:
			case TEXEFF_SOFTALPHA_1:
			case TEXEFF_SOFTALPHA_2:
				emMETR = MATERIAL_ALPHA_SOFT;
				break;
			}
		}

		if ( emMETR==MATERIAL_NORMAL )
            continue;

		//	Note : ë©”í„°ë¦¬ì–¼, ?ìŠ¤ì³?ì§€??
		pd3dDevice->SetMaterial( &pMeshs->m_rgMaterials[dwMatIndex] );
		pd3dDevice->SetTexture( 0, pMeshs->m_pTextureRes[dwMatIndex].GetTexture() );

		NSLightMapFX::LightMapDetail(1.f);
		NSLightMapFX::SetLightMapUV_Multiply( pMeshs->m_wwLightMapUserUV_TextureSize );

		NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
		if ( pSetting )
		{
			if ( pSetting->m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_VOXELCOLOR )
			{
				emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
			}
			else if ( pSetting->m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_AMBIENTCUBE )
			{
				emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_AMBIENTCUBE;
			}
		}
		

		// Note : NSMaterialManager ¿¡ ÀÖ´Â mat ÀÌ¶ó¸é NSMaterialManager::Render ¿¡¼­ ¸ðµç ÀÛ¾÷À» ÇÑ´Ù.
		MATERIAL_DATA sMaterial_Data;
		float fCubeMapValue = pSetting ? (pSetting->m_bCubeMap ? pSetting->m_fCubeMapValue : 0.f) : 0.f;
		if ( NSMaterialManager::Active( pd3dDevice, 
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
			ID3DXEffect* pFX = sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX;
			if ( !pFX )
				return;

			int nTexType(0);
			DWORD dwAlphaTestEnable(0L);
			DWORD dwAlphaBlendEnable(0L);
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

				if ( emMETR == MATERIAL_ALPHA_HARD )
				{
					nTexType = 1;

					pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
				}
				else if ( emMETR == MATERIAL_ALPHA_SOFT )
				{
					nTexType = 2;

					pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				}
			}

			if ( pMeshs->m_spMaterialMeshArray->Render( pd3dDevice, i, sMaterial_Data, 0, FALSE ) )
			{
				if ( nTexType == 1 )
				{
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
				}
				else if ( nTexType == 2 )
				{
					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
				}

				continue;
			}
		}

		// Note : ê¸°ë³¸
		MAPTEXEFF_ITER iter = m_mapTexEff.find( pMeshs->m_strTextureFiles[dwMatIndex].GetString() );
		if( iter==m_mapTexEff.end() )
		{
            NSVERTEXNORTEX_FX::SetBaseTexture( pMeshs->m_pTextureRes[dwMatIndex].GetTexture() );
			RenderAlpha( pd3dDevice, pLocalMesh, dwMatIndex, emMETR, bShadow );
			continue;
		}

        // Note : VisualMaterialÀ» »ç¿ëÇÑ´Ù¸é, ¿øº»Àº ±×¸®Áö ¾Ê°í DxTexEffBase¿¡¼­ ±×¸°´Ù.
        BOOL bVisualMaterial = FALSE;
        if ( DxEffectMan::GetInstance().IsEnableCharPixelShader() )
        {
		    DxTexEffBase* pEff = (*iter).second;
		    while( pEff )
		    {
                switch( pEff->GetTypeID() )
			    {
                case DEF_TEXEFF_VISUAL_MATERIAL:
                    {
                        bVisualMaterial = TRUE;

                        if( !pMeshs->m_spTangentMesh.get() )
                        {
                            pMeshs->m_spTangentMesh = std::tr1::shared_ptr<DxTangentMesh>(new DxTangentMesh);
                            pMeshs->m_spTangentMesh->CreateColorMesh( pd3dDevice, pMeshs->m_pLocalMesh );
                        }

                        pEff->RenderColor( pd3dDevice, pMeshs->m_spTangentMesh.get(), i, bRealPL, matWorld, 1.f );
                    }
                    break;
                };
                pEff = pEff->m_pNext;
            };
        }

        // VisualMaterial ÀÌ ¾øÀ» °æ¿ì¸¸, °¡´ÉÇÏ´Ù.
        if ( !bVisualMaterial )
        {
            NSVERTEXNORTEX_FX::SetBaseTexture( pMeshs->m_pTextureRes[dwMatIndex].GetTexture() );
	        RenderAlpha( pd3dDevice, pLocalMesh, dwMatIndex, emMETR, bShadow );

	        // Note : ?¨ê³¼ ?Œë”
	        DxTexEffBase* pEff = (*iter).second;
	        while( pEff )
	        {
		        switch( pEff->GetTypeID() )
		        {
		        case DEF_TEXEFF_DIFFUSE:
		        case DEF_TEXEFF_SPECULAR:
			        pEff->Render( pd3dDevice, pLocalMesh, dwMatIndex );
			        break;
		        };
        			
		        pEff = pEff->m_pNext;
	        }
        }
	}
}

void DxTextureEffMan::RenderDecal( LPDIRECT3DDEVICEQ pd3dDevice, 
								  DxMeshes* pMeshs, 
								  const D3DXMATRIX& matWorld, 
								  const CMaterialSetting* pMaterialSetting, 
								  int nRenderOrder )
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

			const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( pMeshs->m_vecMaterialName[dwMatIndex] );
			if ( pSetting )
			{
				// ·»´õ Order
				if ( pSetting->m_nDecalOrder != nRenderOrder )
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
					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
					pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
					pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
				}
				else if ( pSetting->m_emDecalType == NSMaterialSetting::DECAL_TYPE_ADD )
				{
					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
					pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
					pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
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
			if ( NSMaterialManager::Active( pd3dDevice, 
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
					pMeshs->m_spMaterialMeshArray->Render( pd3dDevice, i, sMaterial_Data, 0, FALSE );
					pMeshs->m_spMaterialMeshArray->Render( pd3dDevice, i, sMaterial_Data, 0, TRUE );
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

void DxTextureEffMan::RenderWaterCustomSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
													  DxMeshes* pMeshs, 
													  const D3DXMATRIX& matWorld, 
													  const CMaterialSetting* pMaterialSetting )
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

			const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( pMeshs->m_vecMaterialName[dwMatIndex] );

			NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
			if ( pSetting && pSetting->m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_VOXELCOLOR )
			{
				emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
			}

			// Note : NSMaterialManager ¿¡ ÀÖ´Â mat ÀÌ¶ó¸é NSMaterialManager::Render ¿¡¼­ ¸ðµç ÀÛ¾÷À» ÇÑ´Ù.
			MATERIAL_DATA sMaterial_Data;
			float fCubeMapValue = pSetting ? (pSetting->m_bCubeMap ? pSetting->m_fCubeMapValue : 0.f) : 0.f;
			if ( NSMaterialManager::Active( pd3dDevice, 
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
					pMeshs->m_spMaterialMeshArray->Render( pd3dDevice, i, sMaterial_Data, 0, TRUE );
				}

				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

				fBias = 0.f;
				pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS, *((DWORD*)&fBias) );
				pd3dDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&fBias) );
			}
		}
	}
}

void DxTextureEffMan::RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pLocalMesh, const DWORD dwAttrib, const int nMETR, const BOOL bShadow )
{
	if ( !bShadow )
	{
		switch ( nMETR )
		{
		case MATERIAL_ALPHA:
			DxRenderStates::GetInstance().SetAlphaLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_NOZWRI:
			DxRenderStates::GetInstance().SetAlphaNoZWriLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_NOTEST:
			DxRenderStates::GetInstance().SetAlphaNoTestLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_HARD:
			DxRenderStates::GetInstance().SetOnAlphaMap( pd3dDevice );
			break;
		case MATERIAL_ALPHA_SOFT:
			DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
			break;
		};
	}

    NSVERTEXNORTEX_FX::Render( pd3dDevice, pLocalMesh, dwAttrib );
	//pLocalMesh->DrawSubset( dwAttrib );

	if ( !bShadow )
	{
		switch ( nMETR )
		{
		case MATERIAL_ALPHA:
			DxRenderStates::GetInstance().ReSetAlphaLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_NOZWRI:
			DxRenderStates::GetInstance().ReSetAlphaNoZWriLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_NOTEST:
			DxRenderStates::GetInstance().ReSetAlphaNoTestLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_HARD:
			DxRenderStates::GetInstance().ReSetOnAlphaMap( pd3dDevice );
			break;
		case MATERIAL_ALPHA_SOFT:
			DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
			break;
		};
	}
}

//void DxTextureEffMan::RenderSpecular( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : Base Render
//	pMesh->DrawSubset( dwAttrib );
//
//	// Note : GetTexture
//	if( !pProp->pSpecularTex )
//	{
//		TextureManager::GetTexture( pProp->strSpecularTex.c_str(), pProp->pSpecularTex );
//		if( !pProp->pSpecularTex )	return;
//	}
//
//	// Note : Setting
//	pd3dDevice->SetTexture( 0, pProp->pSpecularTex );
//	pd3dDevice->SetTexture( 1, DxCubeMap::GetInstance().GetCubeTexTEST() );
//
//	
//	m_pSpecularSavedSB->Capture();
//	m_pSpecularDrawSB->Apply();
//	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xffffffff );
//
//	// Note : Specular Render
//	pMesh->DrawSubset( dwAttrib );
//
//	m_pSpecularSavedSB->Apply();
//
//	// Note : SAFE
//	pd3dDevice->SetTexture( 1, NULL );
//}
//
//void DxTextureEffMan::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : Base Render
//	pMesh->DrawSubset( dwAttrib );
//
//	// Note : GetTexture
//	if( !pProp->pTex )
//	{
//		TextureManager::GetTexture( pProp->szTex, pProp->pTex );
//		if( !pProp->pTex )	return;
//	}
//	
//	// Note : Setting
//	pd3dDevice->SetTexture( 0, pProp->pTex );
//	pd3dDevice->SetTexture( 1, pProp->pCubeTex );
//
//	m_pReflectSavedSB->Capture();
//	m_pReflectDrawSB->Apply();
//	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, pProp->dwColor );
//
//	// Note : Reflect Render
//	pMesh->DrawSubset( dwAttrib );
//
//	m_pReflectSavedSB->Apply();
//
//	// Note : SAFE
//	pd3dDevice->SetTexture( 1, NULL );
//}
//
//void DxTextureEffMan::RenderBump( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : Base Render
//	pMesh->DrawSubset( dwAttrib );
//}
//
//void DxTextureEffMan::RenderGlow( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : Base Render
//	pMesh->DrawSubset( dwAttrib );
//}
//
//void DxTextureEffMan::RenderMultiTex( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : GetTexture
//	if( !pProp->pMultiBaseTex )
//	{
//		TextureManager::GetTexture( pProp->strMultiBaseTex.c_str(), pProp->pMultiBaseTex );
//		if( !pProp->pMultiBaseTex )	return;
//	}
//	if( !pProp->pMultiFlowTex )
//	{
//		TextureManager::GetTexture( pProp->strMultiFlowTex.c_str(), pProp->pMultiFlowTex );
//		if( !pProp->pMultiFlowTex )	return;
//	}
//
//	// Note : ?ìŠ¤ì³ë? ?˜ë¦°??
//	if( pProp->bfMultiTexNEWUV )
//	{
//		FrameMoveMultiTex( pProp->fMultiTexSpeed, pProp->fMultiTexUVScale, pProp->fMultiTexAddUV, pMesh );
//	}
//	else
//	{
//		FrameMoveMultiTex( pProp->fMultiTexSpeed, pMesh );
//	}
//
//	// Note : Setting
//	pd3dDevice->SetTexture( 0, pProp->pMultiBaseTex );
//	pd3dDevice->SetTexture( 1, pProp->pMultiFlowTex  );
//
//	
//	m_pMultiTexSavedSB->Capture();
//	m_pMultiTexDrawSB->Apply();
//
//	// Note : Specular Render
//	pMesh->DrawSubset( dwAttrib );
//
//	m_pMultiTexSavedSB->Apply();
//
//	// Note : SAFE
//	pd3dDevice->SetTexture( 1, NULL );
//}
//
//void DxTextureEffMan::FrameMoveMultiTex( float fBaseSpeed, LPD3DXMESH pMesh )
//{
//	float fSpeed = fBaseSpeed * 0.01f;
//
//	struct VERTEXTEX2 { D3DXVECTOR3 vPos, vNor; D3DXVECTOR2 vTex1, vTex2; };
//
//	VERTEXTEX2* pVert;
//	pMesh->LockVertexBuffer( 0L, (VOID**)&pVert );
//	for( DWORD i=0; i<pMesh->GetNumVertices(); ++i )
//	{
//		pVert[i].vTex2 += D3DXVECTOR2( fSpeed, fSpeed );
//	}
//	pMesh->UnlockVertexBuffer();
//}
//
//void DxTextureEffMan::FrameMoveMultiTex( float fBaseSpeed, float UVScale, float& UVAdd, LPD3DXMESH pMesh )
//{
//	UVAdd += fBaseSpeed * 0.01f;
//	float fScale = UVScale * 0.01f;
//
//	struct VERTEXTEX2 { D3DXVECTOR3 vPos, vNor; D3DXVECTOR2 vTex1, vTex2; };
//
//	VERTEXTEX2* pVert;
//	pMesh->LockVertexBuffer( 0L, (VOID**)&pVert );
//	for( DWORD i=0; i<pMesh->GetNumVertices(); ++i )
//	{
//		pVert[i].vTex2.x = pVert[i].vPos.x*fScale + UVAdd;
//		pVert[i].vTex2.y = pVert[i].vPos.z*fScale + UVAdd;
//	}
//	pMesh->UnlockVertexBuffer();
//}

void DxTextureEffMan::MakeEffectMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs )
{
	//if( pMeshs->m_pEffVB )	return;

	//pMeshs->MakeEffectMeshs( pd3dDevice, D3DFVF_XYZ|D3DFVF_TEX3|D3DFVF_TEXCOORDSIZE1(2) );

	//struct VERTEXTEX1 { D3DXVECTOR3 vPos, vNor; D3DXVECTOR2 vTex1; };
	//struct VERTEXTEX2 { D3DXVECTOR3 vPos; D3DXVECTOR2 vTex1, vTex2; float fTex3; };

	//VERTEXTEX1* pSrc;
	//VERTEXTEX2* pDest;
	//pMeshs->m_pLocalMesh->LockVertexBuffer( 0L, (VOID**)&pSrc );
	//pMeshs->m_pEffectMesh->LockVertexBuffer( 0L, (VOID**)&pDest );
	//for( DWORD i=0; i<pMeshs->m_pLocalMesh->GetNumVertices(); ++i )
	//{
	//	pDest[i].vPos =  pSrc[i].vPos;
	//	pDest[i].vTex1 =  pSrc[i].vTex1;
	//	pDest[i].vTex2 =  pSrc[i].vTex1;
	//	pDest[i].fTex3 = 0.f;
	//}
	//pMeshs->m_pEffectMesh->UnlockVertexBuffer();
	//pMeshs->m_pLocalMesh->UnlockVertexBuffer();


	//if( !pMeshs->m_pLocalMesh )		return;

	//{
	//	m_dwVert = m_pLocalMesh->GetNumVertices();

	//	SAFE_DELETE_ARRAY( m_pVertSrc );
	//	m_pVertSrc = new VERTEX[ m_dwVert ];

	//	SAFE_RELEASE( m_pEffVB );
	//	pd3dDevice->CreateVertexBuffer( m_dwVert*sizeof(VERTEX), D3DUSAGE_WRITEONLY,
	//									VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pEffVB );

	//	VERTEX *pSrc, *Dest;
	//	m_pLocalMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pSrc );
	//	m_pEffVB->Lock( 0, 0, (VOID**)&Dest, 0L );
	//	memcpy( m_pVertSrc, pSrc, m_dwVert*sizeof(VERTEX) );
	//	memcpy( Dest, pSrc, m_dwVert*sizeof(VERTEX) );
	//	m_pEffVB->Unlock();
	//	m_pLocalMesh->UnlockVertexBuffer();
	//}

	//{
	//	m_dwFace = m_pLocalMesh->GetNumFaces();

	//	SAFE_RELEASE( m_pEffIB );
	//	pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L,
	//									D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pEffIB );

	//	WORD *pSrc, *Dest;
	//	m_pLocalMesh->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pSrc );
	//	m_pEffIB->Lock( 0, 0, (VOID**)&Dest, 0L );
	//	memcpy( Dest, pSrc, m_dwFace*3*sizeof(WORD) );
	//	m_pEffIB->Unlock();
	//	m_pLocalMesh->UnlockIndexBuffer();
	//}

	//{
	//	m_pLocalMesh->GetAttributeTable( NULL, &m_dwAttrib );

	//	SAFE_DELETE_ARRAY( m_pAttrib );
	//	m_pAttrib = new D3DXATTRIBUTERANGE[ m_dwAttrib ];
	//	m_pLocalMesh->GetAttributeTable( m_pAttrib, &m_dwAttrib );
	//}
}

