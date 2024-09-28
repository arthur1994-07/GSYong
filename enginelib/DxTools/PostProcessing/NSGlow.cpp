#include "pch.h"

#include "../../DxTools/DxVertexFVF.h"
#include "../TextureManager.h"
#include "../DxInputDevice.h"
#include "../DxSurfaceTex.h"
#include "../RENDERPARAM.h"
#include "../DxViewPort.h"

#include "NSGlow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//Shaft
namespace NSGlow
{
	#define MAX_GAUSS_SAMPLE 13

	VERTEXRHW				m_sVert_Full[4];
	VERTEXRHW				m_sVert_256[4];
	LPD3DXEFFECT			g_pEffect(NULL);
	LPDIRECT3DTEXTUREQ		g_pTestTex(NULL);
	LPDIRECT3DSTATEBLOCK9	g_pSavedSB_FLARE_ADD;
	LPDIRECT3DSTATEBLOCK9	g_pEffectSB_FLARE_ADD;
	LPDIRECT3DSTATEBLOCK9	g_pSavedSB_FLARE;
	LPDIRECT3DSTATEBLOCK9	g_pEffectSB_FLARE;
	LPDIRECT3DSTATEBLOCK9	g_pSavedSB_ADD;
	LPDIRECT3DSTATEBLOCK9	g_pEffectSB_ADD;
	BOOL					g_bDeviceEnable(TRUE);
	BOOL					g_bActiveGlow(FALSE);
	float					g_fMicroWidth(0.f);
	float					g_fMicroHeight(0.f);
	float					g_fLightShaftsWeight(0.1f);
	float					g_fLightShaftsDecay(0.1f);
	float					g_fLightShaftsLast(1.f);
	D3DXVECTOR3				g_avTexCoordOffset_Weight[MAX_GAUSS_SAMPLE];

	TSTRING g_strFullPath;
	void SetPath( const TCHAR* szFullPath )
	{
		g_strFullPath = szFullPath;
	}

	void SetGaussBlur5x5()
	{
		DWORD dwWidth = 256;	//DxSurfaceTex::GetInstance().GetWidth();
		DWORD dwHeight = 256;	//DxSurfaceTex::GetInstance().GetHeight();

		float tu = 1.f / static_cast<float>( dwWidth );
		float tv = 1.f / static_cast<float>( dwHeight );

		float fTotalWeight = 0.f;
		int nIndex(0);
		for( int x=-2; x<=2; ++x )
		{
			for( int y=-2; y<=2; ++y )
			{
				// 계수가 작아질 부분 소거
				if( 2 < abs(x) + abs(y) )
					continue;

				// 범위를 넘어서면 안된다.
				if( nIndex >= MAX_GAUSS_SAMPLE )
					continue;

				g_avTexCoordOffset_Weight[nIndex].x = x*tu;
				g_avTexCoordOffset_Weight[nIndex].y = y*tv;
				float fx = static_cast<float>(x);
				float fy = static_cast<float>(y);
				g_avTexCoordOffset_Weight[nIndex].z = expf( -(fx*fx + fy*fy) / (2*1.f) );
				fTotalWeight += g_avTexCoordOffset_Weight[nIndex].z;

				++nIndex;
			}
		}

		for( int i=0; i<nIndex && i<MAX_GAUSS_SAMPLE; ++i )
		{
			g_avTexCoordOffset_Weight[i].z *= 1.f/fTotalWeight;
		}
	}

	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		SetGaussBlur5x5();
	}

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		// 전체 화면에 쓰이는 것.
		DWORD dwWidth = DxSurfaceTex::GetInstance().GetWidth();
		DWORD dwHeight = DxSurfaceTex::GetInstance().GetHeight();

		float fMicroWidth = 0.5f/(float)dwWidth;
		float fMicroHeight = 0.5f/(float)dwHeight;

		m_sVert_Full[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				1.f, 1.f );
		m_sVert_Full[1].vPos = D3DXVECTOR4 ( (float)dwWidth,	0.f,				1.f, 1.f );
		m_sVert_Full[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	1.f, 1.f );
		m_sVert_Full[3].vPos = D3DXVECTOR4 ( (float)dwWidth,	(float)dwHeight,	1.f, 1.f );

		m_sVert_Full[0].vTex = D3DXVECTOR2 ( 0.f+fMicroWidth,	0.f+fMicroHeight );
		m_sVert_Full[1].vTex = D3DXVECTOR2 ( 1.f+fMicroWidth,	0.f+fMicroHeight );
		m_sVert_Full[2].vTex = D3DXVECTOR2 ( 0.f+fMicroWidth,	1.f+fMicroHeight );
		m_sVert_Full[3].vTex = D3DXVECTOR2 ( 1.f+fMicroWidth,	1.f+fMicroHeight );

		dwWidth = 256;
		dwHeight = 256;

		g_fMicroWidth = 0.5f/(float)dwWidth;
		g_fMicroHeight = 0.5f/(float)dwHeight;

		m_sVert_256[0].vPos = D3DXVECTOR4 ( 0.f,			0.f,				1.f, 1.f );
		m_sVert_256[1].vPos = D3DXVECTOR4 ( (float)dwWidth,	0.f,				1.f, 1.f );
		m_sVert_256[2].vPos = D3DXVECTOR4 ( 0.f,			(float)dwHeight,	1.f, 1.f );
		m_sVert_256[3].vPos = D3DXVECTOR4 ( (float)dwWidth,	(float)dwHeight,	1.f, 1.f );

		m_sVert_256[0].vTex = D3DXVECTOR2 ( 0.f+g_fMicroWidth,	0.f+g_fMicroHeight );
		m_sVert_256[1].vTex = D3DXVECTOR2 ( 1.f+g_fMicroWidth,	0.f+g_fMicroHeight );
		m_sVert_256[2].vTex = D3DXVECTOR2 ( 0.f+g_fMicroWidth,	1.f+g_fMicroHeight );
		m_sVert_256[3].vTex = D3DXVECTOR2 ( 1.f+g_fMicroWidth,	1.f+g_fMicroHeight );


		// Base
		for( UINT which=0; which<2; which++ )
		{
			pd3dDevice->BeginStateBlock();

			pd3dDevice->SetRenderState( D3DRS_ZENABLE,				FALSE );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			FALSE );
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );
			pd3dDevice->SetRenderState( D3DRS_LIGHTING,				FALSE );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		FALSE );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,		0xffaaaaaa );	//404040 );

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

            pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP );
            pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP );
			pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP );
            pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP );
			pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP );
            pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP );
			pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP );

			//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
			//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2,	D3DTA_CURRENT );
			//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_ADD );

			if( which==0 )	pd3dDevice->EndStateBlock( &g_pSavedSB_FLARE_ADD );
			else			pd3dDevice->EndStateBlock( &g_pEffectSB_FLARE_ADD );
		}

		// Base
		for( UINT which=0; which<2; which++ )
		{
			pd3dDevice->BeginStateBlock();

			pd3dDevice->SetRenderState( D3DRS_ZENABLE,				FALSE );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			FALSE );
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );
			pd3dDevice->SetRenderState( D3DRS_LIGHTING,				FALSE );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		TRUE );

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

			if( which==0 )	pd3dDevice->EndStateBlock( &g_pSavedSB_FLARE );
			else			pd3dDevice->EndStateBlock( &g_pEffectSB_FLARE );
		}

		// Base
		for( UINT which=0; which<2; which++ )
		{
			pd3dDevice->BeginStateBlock();

			pd3dDevice->SetRenderState( D3DRS_ZENABLE,				FALSE );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			FALSE );
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );
			pd3dDevice->SetRenderState( D3DRS_LIGHTING,				FALSE );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		TRUE );

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

            pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP );

			if( which==0 )	pd3dDevice->EndStateBlock( &g_pSavedSB_ADD );
			else			pd3dDevice->EndStateBlock( &g_pEffectSB_ADD );
		}

		// FX 내부 로딩
		HRESULT hr(S_OK);
		LPD3DXBUFFER pBuffer = NULL;
		{
			TSTRING strFX;
			SetFX( strFX );

			UINT dwBufferSize = static_cast<UINT>( strFX.size() ) + 1; 
			hr = D3DXCreateEffect( pd3dDevice, strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &g_pEffect, &pBuffer );
		}

		//// FX 외부 로딩
		//{
		//	LPD3DXBUFFER pBuffer = NULL;
		//	TSTRING strShaderFullPath = g_strFullPath + _T("Glow.fx");

		//	INT SrcDataSize = 0;
		//	CTypeSmartPtr pSrcData = CORE_COMMON::getFileSystem()->getFile( strShaderFullPath, SrcDataSize, FALSE, FALSE );
		//	if( pSrcData == NULL )
		//	{
		//		g_bDeviceEnable = FALSE;
		//		return;
		//	}

		//	HRESULT hr(S_OK);
		//	LPDIRECT3DTEXTUREQ pTexture = NULL;
		//	hr = D3DXCreateEffect( pd3dDevice, pSrcData.get(), (UINT)SrcDataSize, NULL, NULL, 0, NULL, &g_pEffect, &pBuffer );

		//	CORE_COMMON::getFileSystem()->releaseMemory( strShaderFullPath );
		//}


		if (FAILED(hr))
		{
			std::string strError;
			if (pBuffer)
            {
                LPVOID pCompileErrors = pBuffer->GetBufferPointer( );
                strError = (const char*) pCompileErrors;
            }
            SAFE_RELEASE( g_pEffect );

			MessageBoxA(NULL, strError.c_str(), "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
			g_bDeviceEnable = FALSE;
			return;
		}
	}

	void OnLostDevice()
	{
		SAFE_RELEASE( g_pEffect );

		SAFE_RELEASE( g_pSavedSB_FLARE_ADD );
		SAFE_RELEASE( g_pEffectSB_FLARE_ADD );
		SAFE_RELEASE( g_pSavedSB_FLARE );
		SAFE_RELEASE( g_pEffectSB_FLARE );
		SAFE_RELEASE( g_pSavedSB_ADD );
		SAFE_RELEASE( g_pEffectSB_ADD );
	}

	void OnDestroyDevice()
	{
	}

    void SetActiveGlow()
    {
        g_bActiveGlow = TRUE;
    }

    DWORD g_dwSTART_NUM(0);
	DWORD g_dwLOOP_NUM(7);
    HRESULT OnRender2( LPDIRECT3DDEVICEQ pd3dDevice, 
                        const D3DXVECTOR2& vDir, 
                        LPDIRECT3DTEXTUREQ	pBaseTex,
                        LPDIRECT3DTEXTUREQ&	pLightShaftsTex,
		                LPDIRECT3DSURFACEQ&	pLightShaftsSuf,
		                LPDIRECT3DTEXTUREQ&	pGaussBlurTex,
		                LPDIRECT3DSURFACEQ&	pGaussBlurSuf )
	{
		if ( !DxSurfaceTex::GetInstance().m_pTempSuf16F_256_1st )
			return S_OK;

		if ( !DxSurfaceTex::GetInstance().m_pTempSuf16F_256_2nd )
			return S_OK;

        HRESULT hr(S_OK);

        D3DXVECTOR4 vOffset(0.f,0.f,0.f,0.f);
	    vOffset.x = g_fMicroWidth * 2.f;	//static_cast<float>( powl(2,i) );

        LPDIRECT3DTEXTUREQ	pTempTex[2] = { DxSurfaceTex::GetInstance().m_pTempTex16F_256_1st, DxSurfaceTex::GetInstance().m_pTempTex16F_256_2nd };
		LPDIRECT3DSURFACEQ	pTempSuf[2] = { DxSurfaceTex::GetInstance().m_pTempSuf16F_256_1st, DxSurfaceTex::GetInstance().m_pTempSuf16F_256_2nd };

        if( pBaseTex == pTempTex[ 0 ] )
        {
            std::swap( pTempTex[ 0 ], pTempTex[ 1 ] );
            std::swap( pTempSuf[ 0 ], pTempSuf[ 1 ] );
        }

        //DWORD dwArrayWidth[] = { 4, 8, 16, 32, 64, 96, 128, 160, 192, 224, 256 };
        DWORD dwArrayWidth[] = { 4, 6, 8, 12, 16, 24, 32, 64, 96, 128, 160, 192, 224, 256 };
        for( DWORD i=0; i<g_dwLOOP_NUM; ++i )
		{
			int nIndex = i%2;
			if( i == g_dwSTART_NUM )
			{
				hr = pd3dDevice->SetRenderTarget(0, pTempSuf[nIndex] );
				g_pEffect->SetTexture( "BaseTex", pBaseTex );
			}
			else if( i == g_dwLOOP_NUM-1 )
			{
				// 마지막은 여기에 온다.
				pLightShaftsTex = pTempTex[nIndex];
				pLightShaftsSuf = pTempSuf[nIndex];
				hr = pd3dDevice->SetRenderTarget(0, pTempSuf[nIndex] );
				if( nIndex == 0)
				{
					pGaussBlurTex = pTempTex[1];
					pGaussBlurSuf = pTempSuf[1];
					g_pEffect->SetTexture( "BaseTex", pTempTex[1] );
				}
				else
				{
					pGaussBlurTex = pTempTex[0];
					pGaussBlurSuf = pTempSuf[0];
					g_pEffect->SetTexture( "BaseTex", pTempTex[0] );
				}
			}
			else
			{
				hr = pd3dDevice->SetRenderTarget(0, pTempSuf[nIndex] );
				if( nIndex == 0)
				{
					g_pEffect->SetTexture( "BaseTex", pTempTex[1] );
				}
				else
				{
					g_pEffect->SetTexture( "BaseTex", pTempTex[0] );
				}
			}

			// 
			// 일정수치씩 늘어난다.
			vOffset.x = vDir.x * dwArrayWidth[i] * g_fMicroWidth * 2.f;
            vOffset.y = vDir.y * dwArrayWidth[i] * g_fMicroWidth * 2.f;
			g_pEffect->SetVector( "g_vOffset", &vOffset );

			pd3dDevice->SetFVF( VERTEXRHW::FVF );

			// Flare 작업을 해준다.
			UINT uPasses;
			if (D3D_OK == g_pEffect->Begin(&uPasses, 0))// The 0 specifies that ID3DXEffect::Begin and ID3DXEffect::End will save and restore all state modified by the effect.
			{
				{
					g_pEffect->BeginPass(0);     // Set the state for a particular pass in a technique.
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sVert_256, sizeof(VERTEXRHW) );
					g_pEffect->EndPass();
				}
				g_pEffect->End();
			}
		}

        return hr;
    }

	void OnRender( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACE9 pBackBuffer )
	{
		// LightShafts 작동할 경우만 동작.
		if( !g_bActiveGlow )
			return;

        if( !g_bDeviceEnable )
            return;

        // 기능동작이 가능한지 확인한다.
		if( !DxSurfaceTex::GetInstance().m_pTempTex16F_256_1st || !DxSurfaceTex::GetInstance().m_pTempTex16F_256_2nd )
			return;

		// 1.4 버전보다 낮으면 작동 불가.
		if( !RENDERPARAM::g_bPixelShader_1_4 )
			return;

		HRESULT hr(S_OK);

		//m_pLightFlareSuf
		LPDIRECT3DTEXTUREQ	pLightShaftsTex(NULL);
		LPDIRECT3DSURFACEQ	pLightShaftsSuf(NULL);
		LPDIRECT3DTEXTUREQ	pGaussBlurTex(NULL);
		LPDIRECT3DSURFACEQ	pGaussBlurSuf(NULL);
		LPDIRECT3DSURFACEQ	pSrcZBuffer(NULL);
		{
			hr = pd3dDevice->GetDepthStencilSurface( &pSrcZBuffer );
			hr = pd3dDevice->SetDepthStencilSurface(NULL);

			g_pSavedSB_FLARE_ADD->Capture();
			g_pEffectSB_FLARE_ADD->Apply();
			
			g_pEffect->SetFloat( "Weight", g_fLightShaftsWeight );
			g_pEffect->SetFloat( "Decay", g_fLightShaftsDecay );

			hr = OnRender2( pd3dDevice, D3DXVECTOR2(1.f,1.f), DxSurfaceTex::GetInstance().m_pGlowTex_SRC, pLightShaftsTex, pLightShaftsSuf, pGaussBlurTex, pGaussBlurSuf );
            hr = OnRender2( pd3dDevice, D3DXVECTOR2(-1.f,-1.f), pLightShaftsTex, pLightShaftsTex, pLightShaftsSuf, pGaussBlurTex, pGaussBlurSuf );
            hr = OnRender2( pd3dDevice, D3DXVECTOR2(1.f,-1.f), pLightShaftsTex, pLightShaftsTex, pLightShaftsSuf, pGaussBlurTex, pGaussBlurSuf );
            hr = OnRender2( pd3dDevice, D3DXVECTOR2(-1.f,1.f), pLightShaftsTex, pLightShaftsTex, pLightShaftsSuf, pGaussBlurTex, pGaussBlurSuf );

			g_pSavedSB_FLARE_ADD->Apply();
		}

		// GaussBlur
		// 2.0 이상만 Blur 동작이 되도록 한다.
		if( RENDERPARAM::g_bPixelShader_2 )
		{
			g_pSavedSB_FLARE_ADD->Capture();
			g_pEffectSB_FLARE_ADD->Apply();

			g_pEffect->SetValue( "g_avTexCoordOffset_Weight", g_avTexCoordOffset_Weight, sizeof(g_avTexCoordOffset_Weight) );

			pd3dDevice->SetFVF( VERTEXRHW::FVF );

			// GaussBlur #1
			hr = pd3dDevice->SetRenderTarget(0, pGaussBlurSuf );
			g_pEffect->SetTexture( "BaseTex", pLightShaftsTex );

			UINT uPasses;
			if (D3D_OK == g_pEffect->Begin(&uPasses, 0))  // The 0 specifies that ID3DXEffect::Begin and ID3DXEffect::End will save and restore all state modified by the effect.
			{ 
				{
					g_pEffect->BeginPass(1);     // Set the state for a particular pass in a technique.
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sVert_256, sizeof(VERTEXRHW) );
					g_pEffect->EndPass();
				}
				g_pEffect->End();
			}

			g_pSavedSB_FLARE_ADD->Apply();
		}

		// m_pColorBuffer
		{
			// 컬러 버퍼를 되돌려준다.
			hr = pd3dDevice->SetRenderTarget( 0, pBackBuffer );	//DxSurfaceTex::GetInstance().m_pColorBuffer );
			hr = pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );

			g_pSavedSB_ADD->Capture();
			g_pEffectSB_ADD->Apply();

			DWORD dwColor(0L);
			dwColor = static_cast<DWORD>( g_fLightShaftsLast * 255.f );
            // V547 Expression 'dwColor < 0' is always false. Unsigned type value is never < 0. nsglow.cpp 444
			if( dwColor < 0 )			dwColor = 0;
			else if( dwColor > 255 )	dwColor = 255;
			dwColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );

			// LightFlare Tex를 렌더링한다.
			pd3dDevice->SetTexture( 0, pGaussBlurTex );
			pd3dDevice->SetFVF( VERTEXRHW::FVF );
			//pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sVert_Full, sizeof(VERTEXRHW) );

			g_pSavedSB_ADD->Apply();
		}

		// 정리
		g_bActiveGlow = FALSE;
		SAFE_RELEASE ( pSrcZBuffer );
	}
}