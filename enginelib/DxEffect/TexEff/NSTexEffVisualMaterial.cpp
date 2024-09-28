#include "pch.h"

#include "../../../VisualMaterialLib/Manager/NSVMRender.h"
#include "./DxTexEffVisualMaterial.h"

#include "NSTexEffVisualMaterial.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSTexEffVisualMaterial
{
    void Render( LPDIRECT3DDEVICEQ pd3dDevice, NSVMRender::DxVMStack* pVMStack )
    {
		DWORD dwSize = pVMStack->GetTranslucentNum();
		for( DWORD i=0; i<dwSize; ++i )
		{
			const NSVMRender::DATA* pData = pVMStack->GetTranslucentData( i );
			if ( !pData )
				continue;

			DxTexEffVisualMaterial* pTexEff = reinterpret_cast<DxTexEffVisualMaterial*>( pData->m_pPointer );
			if ( !pTexEff )
				continue;

			switch ( pData->m_emLightMapMode )
			{
			case NSVMRender::RT_LIGHTMAP:
				{
					pTexEff->RenderDetail( pd3dDevice, 
											pData->m_pTangentMesh, 
											pData->m_dwAttrib, 
											pData->m_matWorld,
											pData->m_pTextureDay,
											pData->m_pTextureNight,
											pData->m_pTextureAvgDay,
											pData->m_pTextureAvgNight,
											pData->m_vLightMapUV_Offset,
											pData->m_fAlpha,
											(&NSVMRender::g_sStarGlow==pVMStack) ? TRUE : FALSE );
				}
				break;


			case NSVMRender::RT_COLOR:
				{
					pTexEff->RenderColorDetail( pd3dDevice, 
												pData->m_pTangentMesh, 
												pData->m_dwAttrib, 
												pData->m_bRealPL, 
												pData->m_matWorld,
												pData->m_fAlpha,
												(&NSVMRender::g_sStarGlow==pVMStack) ? TRUE : FALSE );
				}
				break;


			case NSVMRender::RT_COLOR_LIGHTMAP:
				{
					pTexEff->RenderColorDetailLM( pd3dDevice, 
													pData->m_pTangentMesh, 
													pData->m_dwAttrib, 
													pData->m_matWorld,
													pData->m_pTextureDay,
													pData->m_pTextureNight,
													pData->m_pTextureAvgDay,
													pData->m_pTextureAvgNight,
													pData->m_vLightMapUV_Offset,
													pData->m_fAlpha,
													(&NSVMRender::g_sStarGlow==pVMStack) ? TRUE : FALSE );
				}
				break;
			};
		}

        pVMStack->CleanUp();
    }
}