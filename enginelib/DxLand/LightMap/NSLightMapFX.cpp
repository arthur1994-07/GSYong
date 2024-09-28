#include "pch.h"

#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxFogMan.h"
#include "../../G-Logic/GLPeriod.h"
#include "LightMapFX_General.h"
#include "LightMapFX_Color.h"
#include "LightMapFX_Test.h"
#include "LightMapFX_VERTEX.h"
#include "LightMapFX_VERTEXNCT.h"

#include "NSLightMapFX.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSLightMapFX
{
	LightMapFX_General					g_sLightMapFX_General;
	LightMapFX_Color					g_sLightMapFX_Color;
	LightMapFX_Test						g_sLightMapFX_Test;
	LightMapFX_VERTEX					g_sLightMapFX_VERTEX;
	LightMapFX_VERTEXNCT				g_sLightMapFX_VERTEXNCT;
    RENDER_MODE							g_emMode(ENRM_ZERO);
	NSLIGHTMAPDEF::LIGHTMAP_RENDERTYPE	g_emRenderType(NSLIGHTMAPDEF::LMRT_TEXTURE_LIGHTMAP);
	TSTRING								g_strCheckerName;
	TextureResource						g_textureResChecker;
	LPDIRECT3DTEXTUREQ					g_pTextureChecker(NULL);
	float								g_fLightMapDetail_MAIN(1.f);
	D3DXVECTOR4							g_vUV_Multiply(1.f,1.f,1.f,1.f);

    void CleanUp()
    {
        g_sLightMapFX_General.CleanUp();
		g_sLightMapFX_Color.CleanUp();
		g_sLightMapFX_Test.CleanUp();
		g_sLightMapFX_VERTEX.CleanUp();
		g_sLightMapFX_VERTEXNCT.CleanUp();
    }

    HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
    {
        HRESULT hr(S_OK);

		g_sLightMapFX_General.OnResetDevice( pd3dDevice );
		g_sLightMapFX_Color.OnResetDevice( pd3dDevice );
		g_sLightMapFX_Test.OnResetDevice( pd3dDevice );
		g_sLightMapFX_VERTEX.OnResetDevice( pd3dDevice );
		g_sLightMapFX_VERTEXNCT.OnResetDevice( pd3dDevice );

		hr = pd3dDevice->CreateTexture ( 64, 64, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &g_pTextureChecker, NULL );
		if( FAILED(hr) )
			return hr;

		D3DLOCKED_RECT stLocked;
		hr = g_pTextureChecker->LockRect( 0, &stLocked, NULL, 0 );
		if( FAILED(hr) )
			return hr;

		DWORD* pData = (DWORD*)stLocked.pBits;
		INT iPitch = stLocked.Pitch / 4;
		if ( pData )
		{
			// Note : 실제 텍스쳐에 그린다.
			NSCOMBINETEXTOOLS::DrawTexel_CheckTile
				( 
				pData, 
				iPitch, 
				64,
				64
				);
		}
		g_pTextureChecker->UnlockRect( 0 );

        return hr;
    }

    HRESULT OnLostDevice()
    {
        HRESULT hr(S_OK);

		g_sLightMapFX_General.OnLostDevice();
		g_sLightMapFX_Color.OnLostDevice();
		g_sLightMapFX_Test.OnLostDevice();
		g_sLightMapFX_VERTEX.OnLostDevice();
		g_sLightMapFX_VERTEXNCT.OnLostDevice();

		SAFE_RELEASE( g_pTextureChecker );

        return hr;
    }

    HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice )
    {
        HRESULT hr(S_OK);

        CleanUp();

		g_sLightMapFX_General.LoadFX( pd3dDevice );
		g_sLightMapFX_Color.LoadFX( pd3dDevice );
		g_sLightMapFX_Test.LoadFX( pd3dDevice );
		g_sLightMapFX_VERTEX.LoadFX( pd3dDevice );
		g_sLightMapFX_VERTEXNCT.LoadFX( pd3dDevice );

        {
            g_emMode = ENRM_ZERO;

            D3DCAPSQ d3dCaps;
	        pd3dDevice->GetDeviceCaps ( &d3dCaps );

	        D3DDEVICE_CREATION_PARAMETERS d3dCreateParam;
	        pd3dDevice->GetCreationParameters ( &d3dCreateParam );

            //	Note : MIXED
            BOOL bUseMIXED(TRUE);
	        if ( !(d3dCreateParam.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) )
	        {
		        bUseMIXED = FALSE;
	        }

	        //	Note : Mode Check
	        if ( bUseMIXED && d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1) )
	        {
		        g_emMode = ENRM_VS;
	        }
            if ( (g_emMode == ENRM_VS) && d3dCaps.PixelShaderVersion >= D3DVS_VERSION(1, 1) )
	        {
		        g_emMode = ENRM_VS_PS_1_1;
	        }
            if ( (g_emMode == ENRM_VS_PS_1_1) && d3dCaps.PixelShaderVersion >= D3DVS_VERSION(1, 4) )
	        {
		        g_emMode = ENRM_VS_PS_1_4;
	        }
        }

	    return hr;
    }

	void RenderGeneral( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh )
	{
		g_sLightMapFX_General.Render( pd3dDevice, pMesh );
	}

	void RenderColor( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh )
	{
		g_sLightMapFX_Color.Render( pd3dDevice, pMesh );
	}

	BOOL RenderTest( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bVertexColor, DxTangentMesh* pTangentMesh, const DWORD dwAttrib )
	{
		return g_sLightMapFX_Test.Render( pd3dDevice, bVertexColor, pTangentMesh, dwAttrib );
	}

	BOOL RenderVERTEX( LPDIRECT3DDEVICEQ pd3dDevice,  LPD3DXMESH pMesh, DWORD dwMaterials )
	{
		return g_sLightMapFX_VERTEX.Render( pd3dDevice, pMesh, dwMaterials );
	}

	BOOL RenderInexact( LPDIRECT3DDEVICEQ pd3dDevice,  LPD3DXMESH pMesh, DWORD dwMaterials, int nIndex )
	{
		switch ( pMesh->GetFVF() )
		{
		case VERTEX::FVF:
			return g_sLightMapFX_VERTEX.RenderInexact( pd3dDevice, pMesh, dwMaterials, nIndex );
			break;

		case VERTEXNORCOLORTEX::FVF:
			return g_sLightMapFX_VERTEXNCT.RenderInexact( pd3dDevice, pMesh, dwMaterials, nIndex );
			break;
		}

		return FALSE;
	}

	BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, 
				LPDIRECT3DVERTEXBUFFERQ	pVB,
				LPDIRECT3DINDEXBUFFERQ pIB,
				int nFVFSize,
				DWORD dwFVF,
				DWORD dwVertNum, 
				DWORD dwFaceNum )
	{
		switch ( dwFVF )
		{
		case VERTEX::FVF:
			return g_sLightMapFX_VERTEX.Render( pd3dDevice, pVB, pIB, nFVFSize, dwFVF, dwVertNum, dwFaceNum );
			break;

		case VERTEXNORCOLORTEX::FVF:
			return g_sLightMapFX_VERTEXNCT.Render( pd3dDevice, pVB, pIB, nFVFSize, dwFVF, dwVertNum, dwFaceNum );
			break;
		}

		return FALSE;
	}

    void FrameMove( const D3DXMATRIX& matView, const D3DXMATRIX& matProj, float fLightMapDetail )
    {
		g_fLightMapDetail_MAIN = fLightMapDetail;

		D3DXMATRIX matVP;
		D3DXMatrixMultiply ( &matVP, &matView, &matProj );

		g_sLightMapFX_General.FrameMove( matVP );
		g_sLightMapFX_Color.FrameMove( matVP );
		g_sLightMapFX_Test.FrameMove( matVP );
		g_sLightMapFX_VERTEX.FrameMove( matVP );
		g_sLightMapFX_VERTEXNCT.FrameMove( matVP );
    }

    void ShortBlackFog()
    {
		g_sLightMapFX_General.ShortBlackFog();
		g_sLightMapFX_Color.ShortBlackFog();
		g_sLightMapFX_Test.ShortBlackFog();
		g_sLightMapFX_VERTEX.ShortBlackFog();
		g_sLightMapFX_VERTEXNCT.ShortBlackFog();
    }

    void SetBaseTexture( LPDIRECT3DTEXTUREQ pTexture )
    {
		g_sLightMapFX_General.SetBaseTexture( pTexture );
		g_sLightMapFX_Color.SetBaseTexture( pTexture );
    }

    void SetLightMapTexture( LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight )
    {
		D3DXVECTOR4 vUV_Multiply(1.f,1.f,1.f,1.f);
		if ( g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
		{
			GetLightMapCheker( pTextureDay, vUV_Multiply );
		}

		g_sLightMapFX_General.SetLightMapTexture( pTextureDay, pTextureNight, vUV_Multiply );
		g_sLightMapFX_Color.SetLightMapTexture( pTextureDay, pTextureNight, vUV_Multiply );
		g_sLightMapFX_Test.SetLightMapTexture( pTextureDay, pTextureNight, vUV_Multiply );
		g_sLightMapFX_VERTEX.SetLightMapTexture( pTextureDay, pTextureNight, vUV_Multiply );
		g_sLightMapFX_VERTEXNCT.SetLightMapTexture( pTextureDay, pTextureNight, vUV_Multiply );
    }

	void GetLightMapCheker( LPDIRECT3DTEXTUREQ& pTextureCombined, D3DXVECTOR4& vUV_Multiply )
	{
		if ( g_textureResChecker.IsValid() )
		{
			if ( pTextureCombined )
			{
				D3DSURFACE_DESC pDescSrc;
				D3DSURFACE_DESC pDescSample;
				pTextureCombined->GetLevelDesc( 0, &pDescSrc );
				g_textureResChecker.GetTexture()->GetLevelDesc( 0, &pDescSample );

				vUV_Multiply.x = static_cast<float>(pDescSrc.Width) / pDescSample.Width;
				vUV_Multiply.y = static_cast<float>(pDescSrc.Height) / pDescSample.Height;
			}

			// 텍스쳐 주소변경
			pTextureCombined = g_textureResChecker.GetTexture();
		}
		else
		{
			if ( pTextureCombined )
			{
				D3DSURFACE_DESC pDescSrc;
				D3DSURFACE_DESC pDescSample;
				pTextureCombined->GetLevelDesc( 0, &pDescSrc );
				g_pTextureChecker->GetLevelDesc( 0, &pDescSample );

				vUV_Multiply.x = static_cast<float>(pDescSrc.Width) / pDescSample.Width;
				vUV_Multiply.y = static_cast<float>(pDescSrc.Height) / pDescSample.Height;
			}

			// 텍스쳐 주소변경
			pTextureCombined = g_pTextureChecker;
		}
	}

	// Piece에서 Export를 안하고 Checker 를 보기 위해 셋팅하는 것.
	void SetLightMapUV_Multiply( WORDWORD wwLightMapUserUV_TextureSize )
	{
		if ( wwLightMapUserUV_TextureSize.wA < 1 )
			wwLightMapUserUV_TextureSize.wA = 1;
		if ( wwLightMapUserUV_TextureSize.wB < 1 )
			wwLightMapUserUV_TextureSize.wB = 1;

		LPDIRECT3DTEXTUREQ pTextureDay(NULL);
		LPDIRECT3DTEXTUREQ pTextureNight(NULL);
		D3DXVECTOR4 vUV_Multiply(1.f,1.f,1.f,1.f);
		if ( g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
		{
			if ( g_textureResChecker.IsValid() )
			{
				{
					D3DSURFACE_DESC pDescSample;
					g_textureResChecker.GetTexture()->GetLevelDesc( 0, &pDescSample );

					vUV_Multiply.x = static_cast<float>(wwLightMapUserUV_TextureSize.wA) / pDescSample.Width * g_fLightMapDetail_MAIN;
					vUV_Multiply.y = static_cast<float>(wwLightMapUserUV_TextureSize.wB) / pDescSample.Height * g_fLightMapDetail_MAIN;
				}

				// 텍스쳐 주소변경
				pTextureDay = g_textureResChecker.GetTexture();
				pTextureNight = g_textureResChecker.GetTexture();
			}
			else
			{
				{
					D3DSURFACE_DESC pDescSample;
					g_pTextureChecker->GetLevelDesc( 0, &pDescSample );

					vUV_Multiply.x = static_cast<float>(wwLightMapUserUV_TextureSize.wA) / pDescSample.Width * g_fLightMapDetail_MAIN;
					vUV_Multiply.y = static_cast<float>(wwLightMapUserUV_TextureSize.wB) / pDescSample.Height * g_fLightMapDetail_MAIN;
				}

				// 텍스쳐 주소변경
				pTextureDay = g_pTextureChecker;
				pTextureNight = g_pTextureChecker;
			}
		}

		g_vUV_Multiply = vUV_Multiply;
		g_sLightMapFX_General.SetLightMapTexture( pTextureDay, pTextureNight, vUV_Multiply );
		g_sLightMapFX_Color.SetLightMapTexture( pTextureDay, pTextureNight, vUV_Multiply );
		g_sLightMapFX_Test.SetLightMapTexture( pTextureDay, pTextureNight, vUV_Multiply );
		g_sLightMapFX_VERTEX.SetLightMapTexture( pTextureDay, pTextureNight, vUV_Multiply );
		g_sLightMapFX_VERTEXNCT.SetLightMapTexture( pTextureDay, pTextureNight, vUV_Multiply );
	}

	void GetLightMapUV_Multiply( D3DXVECTOR4& vUV_Multiply )
	{
		vUV_Multiply = g_vUV_Multiply;
	}

    void SetLightMapUV_Offset( const D3DXVECTOR2& vLightMapUV_Offset )
    {
		g_sLightMapFX_General.SetLightMapUV_Offset( vLightMapUV_Offset );
		g_sLightMapFX_Color.SetLightMapUV_Offset( vLightMapUV_Offset );
		g_sLightMapFX_Test.SetLightMapUV_Offset( vLightMapUV_Offset );
		g_sLightMapFX_VERTEX.SetLightMapUV_Offset( vLightMapUV_Offset );
		g_sLightMapFX_VERTEXNCT.SetLightMapUV_Offset( vLightMapUV_Offset );
    }

    void SetWorld( const D3DXMATRIX& matWorld, float fAlpha )
    {
		g_sLightMapFX_General.SetWorld( matWorld, fAlpha );
		g_sLightMapFX_Color.SetWorld( matWorld, fAlpha );
		g_sLightMapFX_Test.SetWorld( matWorld, fAlpha );
		g_sLightMapFX_VERTEX.SetWorld( matWorld, fAlpha );
		g_sLightMapFX_VERTEXNCT.SetWorld( matWorld, fAlpha );
    }

	void LightMapDetail( float fLightMapDetail )
	{
		g_fLightMapDetail_MAIN = fLightMapDetail;
	}

	void SetRenderMode( NSLIGHTMAPDEF::LIGHTMAP_RENDERTYPE emType )	// Rendering 모드
	{
		g_emRenderType = emType;
	}

	void SetLightMapCheckerName( const TSTRING& strName )
	{
		g_strCheckerName = strName;

		if ( !g_strCheckerName.empty() )
		{
			g_textureResChecker = TextureManager::GetInstance().LoadTexture( g_strCheckerName,
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
				1,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_ETC,
				false,
				FALSE );
		}
	}

	LPDIRECT3DTEXTUREQ GetTextureChecker()
	{
		return g_pTextureChecker;
	}
}