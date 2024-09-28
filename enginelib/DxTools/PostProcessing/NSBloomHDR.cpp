#include "pch.h"

#include "../DxBackUpRendTarget.h"
#include "../DxSurfaceTex.h"
#include "../DxVertexFVF.h"
#include "../RENDERPARAM.h"

#include "NSBloomHDR.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSBloomHDR
{
	D3DXVECTOR2				m_sBrightPassXY;			// BrightPass 의 Texture 해상도
	D3DXVECTOR2				m_sDownSample4x4XY;			// DownSample4x4 의 Texture 해상도
	VERTEXRHW				m_sVert_Full[4];
	VERTEXRHW				m_sVert_BrightPass[4];
	VERTEXRHW				m_sVert_DownSample4x4[4];

	LPD3DXEFFECT			g_pEffect(NULL);
	LPDIRECT3DSTATEBLOCK9	g_pSavedSB;
	LPDIRECT3DSTATEBLOCK9	g_pEffectSB;

	D3DXVECTOR4				g_vtcBightPassOffsets[4];
	D3DXVECTOR4				g_vtcDownSampleOffsets[16];
	float					g_fHBloomWeights[9];
	float					g_fHBloomOffsets[9];
	float					g_fVBloomWeights[9];
	float					g_fVBloomOffsets[9];

	float					g_GaussMultiplier = 0.6f;     // Default multiplier
	float					g_GaussMean = 0.0f;           // Default mean for gaussian distribution
	float					g_GaussStdDev = 0.7f;         // Default standard deviation for gaussian distribution

	float ComputeGaussianValue( float x, float mean, float std_deviation )
	{
		// The gaussian equation is defined as such:
		/*    
		  -(x - mean)^2
		  -------------
		  1.0               2*std_dev^2
		  f(x,mean,std_dev) = -------------------- * e^
		  sqrt(2*pi*std_dev^2)
	      
		 */

		return ( 1.0f / sqrt( 2.0f * D3DX_PI * std_deviation * std_deviation ) )
			* expf( ( -( ( x - mean ) * ( x - mean ) ) ) / ( 2.0f * std_deviation * std_deviation ) );
	}

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		// BrightPass
		{
			D3DXVECTOR2 vFullXY;
			vFullXY.x = static_cast<float>( DxSurfaceTex::GetInstance().GetWidth() );
			vFullXY.y = static_cast<float>( DxSurfaceTex::GetInstance().GetHeight() );

			float fMicroWidth = 0.5f/(float)vFullXY.x;
			float fMicroHeight = 0.5f/(float)vFullXY.y;
			fMicroWidth = 0.f;
			fMicroHeight = 0.f;

			m_sVert_Full[0].vPos = D3DXVECTOR4 ( 0.f,		0.f,		1.f, 1.f );
			m_sVert_Full[1].vPos = D3DXVECTOR4 ( vFullXY.x,	0.f,		1.f, 1.f );
			m_sVert_Full[2].vPos = D3DXVECTOR4 ( 0.f,		vFullXY.y,	1.f, 1.f );
			m_sVert_Full[3].vPos = D3DXVECTOR4 ( vFullXY.x,	vFullXY.y,	1.f, 1.f );

			// To correctly map from texels->pixels we offset the coordinates
			// by -0.5f:
			m_sVert_Full[0].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );
			m_sVert_Full[1].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );
			m_sVert_Full[2].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );
			m_sVert_Full[3].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );

			m_sVert_Full[0].vTex = D3DXVECTOR2 ( 0.f+fMicroWidth,	0.f+fMicroHeight );
			m_sVert_Full[1].vTex = D3DXVECTOR2 ( 1.f+fMicroWidth,	0.f+fMicroHeight );
			m_sVert_Full[2].vTex = D3DXVECTOR2 ( 0.f+fMicroWidth,	1.f+fMicroHeight );
			m_sVert_Full[3].vTex = D3DXVECTOR2 ( 1.f+fMicroWidth,	1.f+fMicroHeight );
		}

		// BrightPass
		{
			m_sBrightPassXY.x = static_cast<float>( DxSurfaceTex::GetInstance().GetWidth() ) / 2;
			m_sBrightPassXY.y = static_cast<float>( DxSurfaceTex::GetInstance().GetHeight() ) / 2;

			float fMicroWidth = 0.5f/(float)m_sBrightPassXY.x;
			float fMicroHeight = 0.5f/(float)m_sBrightPassXY.y;
			fMicroWidth = 0.f;
			fMicroHeight = 0.f;

			m_sVert_BrightPass[0].vPos = D3DXVECTOR4 ( 0.f,					0.f,				1.f, 1.f );
			m_sVert_BrightPass[1].vPos = D3DXVECTOR4 ( m_sBrightPassXY.x,	0.f,				1.f, 1.f );
			m_sVert_BrightPass[2].vPos = D3DXVECTOR4 ( 0.f,					m_sBrightPassXY.y,	1.f, 1.f );
			m_sVert_BrightPass[3].vPos = D3DXVECTOR4 ( m_sBrightPassXY.x,	m_sBrightPassXY.y,	1.f, 1.f );

			// To correctly map from texels->pixels we offset the coordinates
			// by -0.5f:
			m_sVert_BrightPass[0].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );
			m_sVert_BrightPass[1].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );
			m_sVert_BrightPass[2].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );
			m_sVert_BrightPass[3].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );

			m_sVert_BrightPass[0].vTex = D3DXVECTOR2 ( 0.f+fMicroWidth,	0.f+fMicroHeight );
			m_sVert_BrightPass[1].vTex = D3DXVECTOR2 ( 1.f+fMicroWidth,	0.f+fMicroHeight );
			m_sVert_BrightPass[2].vTex = D3DXVECTOR2 ( 0.f+fMicroWidth,	1.f+fMicroHeight );
			m_sVert_BrightPass[3].vTex = D3DXVECTOR2 ( 1.f+fMicroWidth,	1.f+fMicroHeight );
		}
		
		// DownSample4x4
		{
			m_sDownSample4x4XY.x = static_cast<float>( DxSurfaceTex::GetInstance().GetWidth() ) / 8;
			m_sDownSample4x4XY.y = static_cast<float>( DxSurfaceTex::GetInstance().GetHeight() ) / 8;

			float fMicroWidth = 0.5f/(float)m_sDownSample4x4XY.x;
			float fMicroHeight = 0.5f/(float)m_sDownSample4x4XY.y;
			fMicroWidth = 0.f;
			fMicroHeight = 0.f;

			m_sVert_DownSample4x4[0].vPos = D3DXVECTOR4 ( 0.f,					0.f,					1.f, 1.f );
			m_sVert_DownSample4x4[1].vPos = D3DXVECTOR4 ( m_sDownSample4x4XY.x,	0.f,					1.f, 1.f );
			m_sVert_DownSample4x4[2].vPos = D3DXVECTOR4 ( 0.f,					m_sDownSample4x4XY.y,	1.f, 1.f );
			m_sVert_DownSample4x4[3].vPos = D3DXVECTOR4 ( m_sDownSample4x4XY.x,	m_sDownSample4x4XY.y,	1.f, 1.f );

			// To correctly map from texels->pixels we offset the coordinates
			// by -0.5f:
			m_sVert_DownSample4x4[0].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );
			m_sVert_DownSample4x4[1].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );
			m_sVert_DownSample4x4[2].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );
			m_sVert_DownSample4x4[3].vPos -= D3DXVECTOR4 ( 0.5f,	0.5f,	0.f, 0.f );

			m_sVert_DownSample4x4[0].vTex = D3DXVECTOR2 ( 0.f+fMicroWidth,	0.f+fMicroHeight );
			m_sVert_DownSample4x4[1].vTex = D3DXVECTOR2 ( 1.f+fMicroWidth,	0.f+fMicroHeight );
			m_sVert_DownSample4x4[2].vTex = D3DXVECTOR2 ( 0.f+fMicroWidth,	1.f+fMicroHeight );
			m_sVert_DownSample4x4[3].vTex = D3DXVECTOR2 ( 1.f+fMicroWidth,	1.f+fMicroHeight );
		}

		// Base
		for( UINT which=0; which<2; which++ )
		{
			pd3dDevice->BeginStateBlock();

			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		FALSE );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			FALSE );
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,				FALSE );
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );

			pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_CLAMP );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_CLAMP );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );

			if( which==0 )	pd3dDevice->EndStateBlock( &g_pSavedSB );
			else			pd3dDevice->EndStateBlock( &g_pEffectSB );
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

			// HDR 은 불가능한 상황이다.
			RENDERPARAM::g_bEnableHW_HDR = FALSE;
			RENDERPARAM::g_bEnable_HDR = FALSE;

			return;
		}

		// Because the source and destination are NOT the same sizes, we
		// need to provide offsets to correctly map between them.
		float sU = ( 1.0f / static_cast< float >( DxSurfaceTex::GetInstance().GetWidth() ) );	// 사용되는 텍스쳐는 백버퍼 를 쓰기 때문이다.
		float sV = ( 1.0f / static_cast< float >( DxSurfaceTex::GetInstance().GetHeight() ) );	// 사용되는 텍스쳐는 백버퍼 를 쓰기 때문이다.

		//////////////////////////////////////////////////////////////////////////
		//	g_vtcBightPassOffsets 구하기
		// The last two components (z,w) are unused. This makes for simpler code, but if
		// constant-storage is limited then it is possible to pack 4 offsets into 2 float4's
		g_vtcBightPassOffsets[0] = D3DXVECTOR4( -0.5f * sU, 0.5f * sV, 0.0f, 0.0f );
		g_vtcBightPassOffsets[1] = D3DXVECTOR4( 0.5f * sU, 0.5f * sV, 0.0f, 0.0f );
		g_vtcBightPassOffsets[2] = D3DXVECTOR4( -0.5f * sU, -0.5f * sV, 0.0f, 0.0f );
		g_vtcBightPassOffsets[3] = D3DXVECTOR4( 0.5f * sU, -0.5f * sV, 0.0f, 0.0f );

		g_pEffect->SetVectorArray( "g_vtcBightPassOffsets", g_vtcBightPassOffsets, 4 );



		//////////////////////////////////////////////////////////////////////////
		//	g_vtcDownSampleOffsets 구하기.
		D3DXVECTOR4 dsOffsets[16];
		int idx = 0;
		for( int i = -2; i < 2; i++ )
		{
			for( int j = -2; j < 2; j++ )
			{
				g_vtcDownSampleOffsets[idx++] = D3DXVECTOR4(
					( static_cast< float >( i ) + 0.5f ) * ( 1.0f / m_sBrightPassXY.x ),	// 사용되는 텍스쳐는 m_sBrightPassXY 를 쓰기 때문이다.
					( static_cast< float >( j ) + 0.5f ) * ( 1.0f / m_sBrightPassXY.y ),	// 사용되는 텍스쳐는 m_sBrightPassXY 를 쓰기 때문이다.
					0.0f, // unused 
					0.0f  // unused
					);
			}
		}

		g_pEffect->SetVectorArray( "g_vtcDownSampleOffsets", g_vtcDownSampleOffsets, 16 );



		//////////////////////////////////////////////////////////////////////////
		//	g_fHBloomWeights, g_fHBloomOffsets 구하기
		for( int i = 0; i < 9; i++ )
		{
			// Compute the offsets. We take 9 samples - 4 either side and one in the middle:
			//     i =  0,  1,  2,  3, 4,  5,  6,  7,  8
			//Offset = -4, -3, -2, -1, 0, +1, +2, +3, +4
			g_fHBloomOffsets[i] = ( static_cast< float >( i ) - 4.0f ) * ( 1.0f / m_sDownSample4x4XY.x );	// 사용되는 텍스쳐는 m_sDownSample4x4XY 를 사용한다.

			// 'x' is just a simple alias to map the [0,8] range down to a [-1,+1]
			float x = ( static_cast< float >( i ) - 4.0f ) / 4.0f;

			// Use a gaussian distribution. Changing the standard-deviation
			// (second parameter) as well as the amplitude (multiplier) gives
			// distinctly different results.
			g_fHBloomWeights[i] = g_GaussMultiplier * ComputeGaussianValue( x, g_GaussMean, g_GaussStdDev );
		}

		g_pEffect->SetFloatArray( "g_fHBloomWeights", g_fHBloomWeights, 9 );
		g_pEffect->SetFloatArray( "g_fHBloomOffsets", g_fHBloomOffsets, 9 );



		//////////////////////////////////////////////////////////////////////////
		//	g_fVBloomWeights, g_fVBloomOffsets 구하기
		for( int i = 0; i < 9; i++ )
		{
			// Compute the offsets. We take 9 samples - 4 either side and one in the middle:
			//     i =  0,  1,  2,  3, 4,  5,  6,  7,  8
			//Offset = -4, -3, -2, -1, 0, +1, +2, +3, +4
			g_fVBloomOffsets[i] = ( static_cast< float >( i ) - 4.0f ) * ( 1.0f / m_sDownSample4x4XY.y );	// 사용되는 텍스쳐는 m_sDownSample4x4XY 를 사용한다.

			// 'x' is just a simple alias to map the [0,8] range down to a [-1,+1]
			float x = ( static_cast< float >( i ) - 4.0f ) / 4.0f;

			// Use a gaussian distribution. Changing the standard-deviation
			// (second parameter) as well as the amplitude (multiplier) gives
			// distinctly different results.
			g_fVBloomWeights[i] = g_GaussMultiplier * ComputeGaussianValue( x, g_GaussMean, g_GaussStdDev );
		}

		g_pEffect->SetFloatArray( "g_fVBloomWeights", g_fVBloomWeights, 9 );
		g_pEffect->SetFloatArray( "g_fVBloomOffsets", g_fVBloomOffsets, 9 );
	}

	void OnLostDevice()
	{
		SAFE_RELEASE( g_pEffect );

		SAFE_RELEASE( g_pSavedSB );
		SAFE_RELEASE( g_pEffectSB );
	}

	void OnRender( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if ( !RENDERPARAM::g_bEnable_HDR )
			return;

		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice );

		UINT uPasses(0);

		pd3dDevice->SetDepthStencilSurface( NULL );

		//DWORD dwFVF;
		//pd3dDevice->GetFVF( &dwFVF );
		pd3dDevice->SetFVF( VERTEXRHW::FVF );

		//D3DXHANDLE hTechniqueHandle = g_pEffect->GetTechniqueByName("tec0");
		//if ( !hTechniqueHandle )
		//	return;

		////////////////////////////////////////////////////////////////////////////
		////						SetTechnique
		//g_pEffect->SetTechnique(hTechniqueHandle);


		g_pSavedSB->Capture();
		g_pEffectSB->Apply();


		//DWORD dwAlphaBlendEnable;
		//DWORD dwZWriteEnable;
		//DWORD dwZEnable;
		//DWORD dwFogEnable;
		//pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,		&dwAlphaBlendEnable );
		//pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,			&dwZWriteEnable );
		//pd3dDevice->GetRenderState( D3DRS_ZENABLE,				&dwZEnable );
		//pd3dDevice->GetRenderState( D3DRS_FOGENABLE,			&dwFogEnable );

		//DWORD dwAddressU;
		//DWORD dwAddressV;
		//DWORD dwMagFilter;
		//DWORD dwMinFilter;
		//pd3dDevice->GetSamplerState( 0, D3DSAMP_ADDRESSU,		&dwAddressU );
		//pd3dDevice->GetSamplerState( 0, D3DSAMP_ADDRESSV,		&dwAddressV );
		//pd3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER,		&dwMagFilter );
		//pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER,		&dwMinFilter );


		//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		FALSE );
		//pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			FALSE );
		//pd3dDevice->SetRenderState( D3DRS_ZENABLE,				FALSE );
		//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );

		//pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_CLAMP );
		//pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_CLAMP );
		//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
		//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );




		//////////////////////////////////////////////////////////////////////////
		//	BrightPass 진행
		// 컬러 버퍼를 되돌려준다.
		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pBloomBrightSuf );

		g_pEffect->SetTexture( "g_BaseTex", DxSurfaceTex::GetInstance().m_pColorTex16F_HDR );

		if (D3D_OK == g_pEffect->Begin(&uPasses, 0))  // The 0 specifies that ID3DXEffect::Begin and ID3DXEffect::End will save and restore all state modified by the effect.
		{ 
			{
				g_pEffect->BeginPass(0);     // Set the state for a particular pass in a technique.
				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sVert_BrightPass, sizeof(VERTEXRHW) );
				g_pEffect->EndPass();
			}
			g_pEffect->End();
		}



		//////////////////////////////////////////////////////////////////////////
		//	DownSample4x4 진행
		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pBloomDownSampleSuf );

		g_pEffect->SetTexture( "g_BaseTex", DxSurfaceTex::GetInstance().m_pBloomBrightTex );

		if (D3D_OK == g_pEffect->Begin(&uPasses, 0))  // The 0 specifies that ID3DXEffect::Begin and ID3DXEffect::End will save and restore all state modified by the effect.
		{ 
			{
				g_pEffect->BeginPass(1);     // Set the state for a particular pass in a technique.
				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sVert_DownSample4x4, sizeof(VERTEXRHW) );
				g_pEffect->EndPass();
			}
			g_pEffect->End();
		}


		
		//////////////////////////////////////////////////////////////////////////
		//	HORIZONTAL BLUR 진행
		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pBloomDownSampleSufH );

		g_pEffect->SetTexture( "g_BaseTex", DxSurfaceTex::GetInstance().m_pBloomDownSampleTex );

		if (D3D_OK == g_pEffect->Begin(&uPasses, 0))  // The 0 specifies that ID3DXEffect::Begin and ID3DXEffect::End will save and restore all state modified by the effect.
		{ 
			{
				g_pEffect->BeginPass(2);     // Set the state for a particular pass in a technique.
				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sVert_DownSample4x4, sizeof(VERTEXRHW) );
				g_pEffect->EndPass();
			}
			g_pEffect->End();
		}



		//////////////////////////////////////////////////////////////////////////
		//	VERTICAL BLUR 진행
		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pBloomDownSampleSuf );

		g_pEffect->SetTexture( "g_BaseTex", DxSurfaceTex::GetInstance().m_pBloomDownSampleTexH );

		if (D3D_OK == g_pEffect->Begin(&uPasses, 0))  // The 0 specifies that ID3DXEffect::Begin and ID3DXEffect::End will save and restore all state modified by the effect.
		{ 
			{
				g_pEffect->BeginPass(3);     // Set the state for a particular pass in a technique.
				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sVert_DownSample4x4, sizeof(VERTEXRHW) );
				g_pEffect->EndPass();
			}
			g_pEffect->End();
		}



		//////////////////////////////////////////////////////////////////////////
		//	백버퍼에 Bloom 값을 추가로 기록
		pd3dDevice->SetRenderTarget( 0, sBackupTarget.m_pTarget );

		g_pEffect->SetTexture( "g_BaseTex", DxSurfaceTex::GetInstance().m_pColorTex16F_HDR );
		g_pEffect->SetTexture( "g_BloomTex", DxSurfaceTex::GetInstance().m_pBloomDownSampleTex );

		if (D3D_OK == g_pEffect->Begin(&uPasses, 0))  // The 0 specifies that ID3DXEffect::Begin and ID3DXEffect::End will save and restore all state modified by the effect.
		{ 
			{
				g_pEffect->BeginPass(4);     // Set the state for a particular pass in a technique.
				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sVert_Full, sizeof(VERTEXRHW) );
				g_pEffect->EndPass();
			}
			g_pEffect->End();
		}


		//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		dwAlphaBlendEnable );
		//pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			dwZWriteEnable );
		//pd3dDevice->SetRenderState( D3DRS_ZENABLE,				dwZEnable );
		//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			dwFogEnable );

		//pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,		dwAddressU );
		//pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,		dwAddressV );
		//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,		dwMagFilter );
		//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,		dwMinFilter );


		g_pSavedSB->Apply();

		//pd3dDevice->SetFVF( dwFVF );
	}
}