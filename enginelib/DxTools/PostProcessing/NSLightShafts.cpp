#include "pch.h"

//#include "../../../EngineBaseLib/Define/DxVertexFVF.h"
//#include "../../../LogicCommonLib/RANPARAM.h"

#include "../../DxTools/DxVertexFVF.h"
#include "../Light/DxLightMan.h"
#include "../TextureManager.h"
#include "../DxInputDevice.h"
#include "../DxSurfaceTex.h"
#include "../RENDERPARAM.h"
#include "../DxViewPort.h"

#include "NSLightShafts.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//Shaft
namespace NSLightShafts
{
	#define MAX_GAUSS_SAMPLE 13

	VERTEXRHW				m_sVert_Full[4];
	VERTEXRHW				m_sVert_256[4];
	LPD3DXEFFECT			g_pEffect(NULL);
	LPDIRECT3DTEXTUREQ		g_pTestTex(NULL);
	LPDIRECT3DSTATEBLOCK9	g_pSavedSB;
	LPDIRECT3DSTATEBLOCK9	g_pEffectSB;
	LPDIRECT3DSTATEBLOCK9	g_pSavedSB_FLARE_ADD;
	LPDIRECT3DSTATEBLOCK9	g_pEffectSB_FLARE_ADD;
	LPDIRECT3DSTATEBLOCK9	g_pSavedSB_FLARE;
	LPDIRECT3DSTATEBLOCK9	g_pEffectSB_FLARE;
	LPDIRECT3DSTATEBLOCK9	g_pSavedSB_ADD;
	LPDIRECT3DSTATEBLOCK9	g_pEffectSB_ADD;
	BOOL					g_bDeviceEnable(TRUE);
	BOOL					g_bActiveLightShafts(FALSE);
	float					g_fMicroWidth(0.f);
	float					g_fMicroHeight(0.f);
	float					g_fLightShaftsWeight(0.f);
	float					g_fLightShaftsDecay(0.f);
	float					g_fLightShaftsLast(0.f);
	D3DXVECTOR3				g_vLightShaftsDir(0.f,0.f,0.f);
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

			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			FALSE );
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );
			pd3dDevice->SetRenderState( D3DRS_LIGHTING,				FALSE );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		FALSE );

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

			if( which==0 )	pd3dDevice->EndStateBlock( &g_pSavedSB );
			else			pd3dDevice->EndStateBlock( &g_pEffectSB );
		}

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

        // FX 외부 로딩
        {
		    //TSTRING strShaderFullPath = g_strFullPath + _T("LightShafts.fx");

		    //INT SrcDataSize = 0;
		    //CTypeSmartPtr pSrcData = CORE_COMMON::getFileSystem()->getFile( strShaderFullPath, SrcDataSize, FALSE, FALSE );
		    //if( pSrcData == NULL )
		    //	return;

		    //LPDIRECT3DTEXTUREQ pTexture = NULL;
		    //hr = D3DXCreateEffect( pd3dDevice, pSrcData.get(), (UINT)SrcDataSize, NULL, NULL, 0, NULL, &g_pEffect, &pBuffer );

		    //CORE_COMMON::getFileSystem()->releaseMemory( strShaderFullPath );
        }


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

		SAFE_RELEASE( g_pSavedSB );
		SAFE_RELEASE( g_pEffectSB );
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

	// [2010.08.24]  이 부분이 성공해야 다른 작업들도 다 할 수 있다.
	BOOL OnRenderStart( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		// 기능동작이 가능한지 확인한다.
		if( !DxSurfaceTex::GetInstance().m_pTempTex16F_256_1st || !DxSurfaceTex::GetInstance().m_pTempTex16F_256_2nd )
			return FALSE;

		if( !DxSurfaceTex::GetInstance().m_pColorSuf_Post )
			return FALSE;

		// 2.0 버전보다 낮으면 작동 불가.
		if( !RENDERPARAM::g_bPixelShader_2 )
			return FALSE;

		// 옵션으로 끌수 있다.
		if( !RENDERPARAM::bVolumeLight )
			return FALSE;

		HRESULT hr(S_OK);

		// LightFlare를 기입할 버퍼를 셋팅한다.
		hr = pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorSuf_Post );
		//hr = pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pZBuffer );
		hr = pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0L, 1.0, 0 );

		g_pSavedSB->Capture();
		g_pEffectSB->Apply();

		return TRUE;
	}

	// Shafts 수치 셋팅
	void OnSetting( LPDIRECT3DDEVICEQ pd3dDevice, 
					float fAlpha, 
					float fLightShaftsWeight, 
					float fLightShaftsDecay, 
					float fLightShaftsLast, 
					const D3DXVECTOR3& vLightShaftsDir )
	{
		// LightShafts 작동.
		g_bActiveLightShafts = TRUE;

		g_fLightShaftsWeight = (fLightShaftsWeight * fAlpha) + (g_fLightShaftsWeight * (1.f-fAlpha));
		g_fLightShaftsDecay = (fLightShaftsDecay * fAlpha) + (g_fLightShaftsDecay * (1.f-fAlpha));
		g_fLightShaftsLast = (fLightShaftsLast * fAlpha) + (g_fLightShaftsLast * (1.f-fAlpha));
		g_vLightShaftsDir = (vLightShaftsDir * fAlpha) + (g_vLightShaftsDir * (1.f-fAlpha));

        DXLIGHT* pDxLight = DxLightMan::GetInstance()->GetDirectLight();
        const D3DLIGHT9* pLight = &pDxLight->m_Light;
		D3DXVECTOR3 vLightPos = g_vLightShaftsDir;
		D3DXVec3Normalize( &vLightPos, &vLightPos );
		vLightPos = -vLightPos;
		float fDot = D3DXVec3Dot( &vLightPos, &DxViewPort::GetInstance().GetLookDir() );

        // 빛의 반대에서도 빛이 남아 있을 수 있도록 작업을 해준다.
        {
            fDot += 1.f;    // -1 ~ 1 -> 0 ~ 2
            fDot *= 0.8f;   // 0 ~ 2 -> 0 ~ 1.6
            fDot -= 0.3f;   // 0 ~ 1.6 -> -0.3 ~ 1.3
            if ( fDot > 1.f )
            {
			    fDot = 1.f;
            }
            if ( fDot < 0.f )
            {
			    fDot = 0.f;
            }
        }

		DWORD dwColor = static_cast<DWORD>( fDot * 255.f * fAlpha );
		dwColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );
	}

    void SetLowLevel( float fAlpha )
    {
        if( !g_bActiveLightShafts )
            return;

        g_fLightShaftsLast = (g_fLightShaftsLast * (1.f-fAlpha));
    }

	void OnRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACEQ	pSrcZBuffer )
	{
		LPDIRECT3DSURFACEQ	pColorBuffer(NULL);
		if ( RENDERPARAM::g_bEnable_HDR )
		{
			pColorBuffer = DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR;
		}
		else
		{
			pColorBuffer = DxSurfaceTex::GetInstance().m_pColorBuffer;
		}

		// 컬러 버퍼를 되돌려준다.
		HRESULT hr(S_OK);
		hr = pd3dDevice->SetRenderTarget( 0, pColorBuffer );
		hr = pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );

		g_pSavedSB->Apply();
	}

	void OnRender( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if ( !DxSurfaceTex::GetInstance().m_pTempSuf16F_256_1st )
			return;

		if ( !DxSurfaceTex::GetInstance().m_pTempSuf16F_256_2nd )
			return;

		if ( RENDERPARAM::g_bEnable_HDR && !DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR )
			return;

		if ( !DxSurfaceTex::GetInstance().m_pColorBuffer )
			return;

		// LightShafts 작동할 경우만 동작.
		if( !g_bActiveLightShafts )
			return;

		HRESULT hr(S_OK);

		//m_pLightFlareSuf
		LPDIRECT3DTEXTUREQ	pLightShaftsTex(NULL);
		LPDIRECT3DSURFACEQ	pLightShaftsSuf(NULL);
		LPDIRECT3DTEXTUREQ	pGaussBlurTex(NULL);
		LPDIRECT3DSURFACEQ	pGaussBlurSuf(NULL);
		LPDIRECT3DSURFACEQ	pSrcZBuffer(NULL);
		{
			LPDIRECT3DTEXTUREQ	pTempTex[2] = { DxSurfaceTex::GetInstance().m_pTempTex16F_256_1st, DxSurfaceTex::GetInstance().m_pTempTex16F_256_2nd };
			LPDIRECT3DSURFACEQ	pTempSuf[2] = { DxSurfaceTex::GetInstance().m_pTempSuf16F_256_1st, DxSurfaceTex::GetInstance().m_pTempSuf16F_256_2nd };

			hr = pd3dDevice->GetDepthStencilSurface( &pSrcZBuffer );
			hr = pd3dDevice->SetDepthStencilSurface(NULL);

			g_pSavedSB_FLARE_ADD->Capture();
			g_pEffectSB_FLARE_ADD->Apply();

			D3DXVECTOR4 vOffset(0.f,0.f,0.f,0.f);
			vOffset.x = g_fMicroWidth * 2.f;	//static_cast<float>( powl(2,i) );

			D3DXVECTOR3 vLightPos = g_vLightShaftsDir;
			vLightPos *= -100000.f;
			vLightPos += DxViewPort::GetInstance().GetFromPt();

			D3DXVECTOR2 vScreenLightPos2;
			D3DXVECTOR3 vScreenLightPos3;
			D3DVIEWPORT9* pViewPort(NULL);
			D3DXMATRIX matIdentity;
			D3DXMATRIX* pmatView(NULL);
			D3DXMATRIX* pmatProj(NULL);
			D3DXMatrixIdentity( &matIdentity );
			pViewPort = &DxViewPort::GetInstance().GetViewPort();
			pmatView = &DxViewPort::GetInstance().GetMatView();
			pmatProj = &DxViewPort::GetInstance().GetMatProj();
			D3DXVec3Project( &vScreenLightPos3, &vLightPos, pViewPort, pmatProj, pmatView, &matIdentity );
			vScreenLightPos2.x = vScreenLightPos3.x / static_cast<float>( DxSurfaceTex::GetInstance().GetWidth() );
			vScreenLightPos2.y = vScreenLightPos3.y / static_cast<float>( DxSurfaceTex::GetInstance().GetHeight() );

            // 범위를 -0.5 에서 1.5로 제한하도록 한다.
            {
			    if( vScreenLightPos2.x < -0.5f )
                {
                    float fLength = -vScreenLightPos2.x - 0.5f;
				    float fRate = 1.f / (fLength+1.f);
                    vScreenLightPos2 -= D3DXVECTOR2(0.5f,0.5f);
                    vScreenLightPos2 *= fRate;
                    vScreenLightPos2 += D3DXVECTOR2(0.5f,0.5f);
                }
			    if( vScreenLightPos2.x > 1.5f )
                {
                    float fLength = vScreenLightPos2.x - 1.5f;
				    float fRate = 1.f / (fLength+1.f);
                    vScreenLightPos2 -= D3DXVECTOR2(0.5f,0.5f);
                    vScreenLightPos2 *= fRate;
                    vScreenLightPos2 += D3DXVECTOR2(0.5f,0.5f);
                }
			    if( vScreenLightPos2.y < -0.5f )
                {
                    float fLength = -vScreenLightPos2.y - 0.5f;
				    float fRate = 1.f / (fLength+1.f);
                    vScreenLightPos2 -= D3DXVECTOR2(0.5f,0.5f);
                    vScreenLightPos2 *= fRate;
                    vScreenLightPos2 += D3DXVECTOR2(0.5f,0.5f);
                }
			    if( vScreenLightPos2.y > 1.5f )
                {
                    float fLength = vScreenLightPos2.y - 1.5f;
				    float fRate = 1.f / (fLength+1.f);
                    vScreenLightPos2 -= D3DXVECTOR2(0.5f,0.5f);
                    vScreenLightPos2 *= fRate;
                    vScreenLightPos2 += D3DXVECTOR2(0.5f,0.5f);
                }
            }
            //if( vScreenLightPos2.x < 0.f )
			//	vScreenLightPos2.x = 0.f;
			//if( vScreenLightPos2.x > 1.f )
			//	vScreenLightPos2.x = 1.f;
			//if( vScreenLightPos2.y < 0.f )
			//	vScreenLightPos2.y = 0.f;
			//if( vScreenLightPos2.y > 1.f )
			//	vScreenLightPos2.y = 1.f;
			
            // 빛과 카메라의 Dot 가 - 가 되면,
            // 빛이 반대로 나오는 현상이 생긴다.
            // 그래서 그것을 뒤집어준다.
            {
                vLightPos = g_vLightShaftsDir;
		        D3DXVec3Normalize( &vLightPos, &vLightPos );
		        vLightPos = -vLightPos;
		        float fDot = D3DXVec3Dot( &vLightPos, &DxViewPort::GetInstance().GetLookDir() );
                if ( fDot < 0.f )
                {
                    //          | -0.5f ~ 1.5f
                    // 1. +0.5f | 0.f ~ 2.f
                    // 2. *0.5f | 0.f ~ 1.f
                    // 3. Rever | 1.f ~ 0.f
                    // 4. *2.f  | 2.f ~ 0.f
                    // 5. -0.5f | 1.5f ~ -0.5f

                    // Note : 1
                    vScreenLightPos2.x += 0.5f;
                    vScreenLightPos2.y += 0.5f;

                    // Note : 2
                    vScreenLightPos2.x *= 0.5f;
                    vScreenLightPos2.y *= 0.5f;

                    // Note : 3
                    vScreenLightPos2.x = 1.f - vScreenLightPos2.x;
                    vScreenLightPos2.y = 1.f - vScreenLightPos2.y;

                    // Note : 4
                    vScreenLightPos2.x *= 2.f;
                    vScreenLightPos2.y *= 2.f;

                    // Note : 5
                    vScreenLightPos2.x -= 0.5f;
                    vScreenLightPos2.y -= 0.5f;

                    //if ( vScreenLightPos2.x >= 0.5f )
                    //{
                    //    // 0.5 ~ 1.5 -> 2.5 ~ 1.5f
                    //    vScreenLightPos2.x += (1.5f - vScreenLightPos2.x)*2.f;
                    //}
                    //else
                    //{
                    //    // -0.5 ~ 0.5 -> -0.5 ~ -1.5f
                    //    vScreenLightPos2.x -= (vScreenLightPos2.x + 0.5f)*2.f;
                    //}
                }
            }

			g_pEffect->SetValue( "ScreenLightPos", &vScreenLightPos2, sizeof(vScreenLightPos2) );
			g_pEffect->SetFloat( "Weight", g_fLightShaftsWeight );
			g_pEffect->SetFloat( "Decay", g_fLightShaftsDecay );

			// 0(4) 1(16) 2(64) 3(256)
			// 0(1) 1(2) 2(4) 3(8) 4(16) 5(32) 6(64) 7(128) 8(256)
			static DWORD g_dwSTART_NUM(0);
			static DWORD g_dwLOOP_NUM(11);
			DWORD dwArrayWidth[] = { 4, 8, 16, 32, 64, 96, 128, 160, 192, 224, 256 };
			//static DWORD g_dwLOOP_NUM(8);
			//DWORD dwArrayWidth[] = { 4, 8, 16, 32, 64, 128, 192, 256 };
			for( DWORD i=0; i<g_dwLOOP_NUM; ++i )
			{
				int nIndex = i%2;
				if( i == g_dwSTART_NUM )
				{
					hr = pd3dDevice->SetRenderTarget(0, pTempSuf[nIndex] );
					g_pEffect->SetTexture( "BaseTex", DxSurfaceTex::GetInstance().m_pColorTex_Post );
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
				vOffset.x = dwArrayWidth[i] * g_fMicroWidth * 2.f;
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
			LPDIRECT3DSURFACEQ	pColorBuffer(NULL);
			if ( RENDERPARAM::g_bEnable_HDR )
			{
				pColorBuffer = DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR;
			}
			else
			{
				pColorBuffer = DxSurfaceTex::GetInstance().m_pColorBuffer;
			}

			// 컬러 버퍼를 되돌려준다.
			hr = pd3dDevice->SetRenderTarget( 0, pColorBuffer );
			hr = pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );

			g_pSavedSB_ADD->Capture();
			g_pEffectSB_ADD->Apply();

			DWORD dwColor(0L);
			dwColor = static_cast<DWORD>( g_fLightShaftsLast * 255.f );
            // V547 Expression 'dwColor < 0' is always false. Unsigned type value is never < 0. nslightshafts.cpp 614
			if( dwColor < 0 )			dwColor = 0;
			else if( dwColor > 255 )	dwColor = 255;
			dwColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );
			g_fLightShaftsLast;

			// LightFlare Tex를 렌더링한다.
			pd3dDevice->SetTexture( 0, pGaussBlurTex );
			pd3dDevice->SetFVF( VERTEXRHW::FVF );
			//pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sVert_Full, sizeof(VERTEXRHW) );

			g_pSavedSB_ADD->Apply();
		}

		// 정리
		g_fLightShaftsWeight = 0.f;
		g_fLightShaftsDecay = 0.f;
		g_bActiveLightShafts = FALSE;
		SAFE_RELEASE ( pSrcZBuffer );
	}
}