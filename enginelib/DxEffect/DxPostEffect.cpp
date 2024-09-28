#include "pch.h"

#include "../DxResponseMan.h"
#include "../DxTools/DxViewPort.h"
#include "../DxTools/DxFogMan.h"
#include "../DxTools/DxSurfaceTex.h"
#include "./EffectLua/NSEffectLuaPlay.h"

#include "DxPostEffect.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float DELTA_OF_WIDTH = 0.1f;

static const char* pRADIAL_BLUR_HLSL = 
{
	"struct VS_OUTPUT_FOR_RADIAL_BLUR\r\n"
		"{\r\n"
		"float4 pos		: POSITION0;\r\n"
		"float2 tex		: TEXCOORD0;\r\n"
		"};\r\n"

		"uniform extern texture g_RadialBlurTex;\r\n"

		"uniform extern float2 g_vecCenter = { 0.5f, 0.5f };\r\n"
		"uniform extern float g_fBlurStart = 0.0f; // 블러를 시작할 방향.\r\n"
		"uniform extern float g_fBlurWidth = 0.1f; // 샘플링 할 벡터 길이.\r\n"
		"uniform extern float g_fBlurStrength = 0.0f;\r\n"

		"sampler RadialBlurSampler = sampler_state\r\n"
		"{\r\n"
		"Texture   = <g_RadialBlurTex>;\r\n"
		"MinFilter = LINEAR;\r\n"
		"MagFilter = LINEAR;\r\n"
		"MipFilter = NONE;\r\n"
		"};\r\n"

		"// ------------------------------------------------------------\r\n"
		"// vertex shader for radial blur by luxes.\r\n"
		"// ------------------------------------------------------------\r\n"
		"VS_OUTPUT_FOR_RADIAL_BLUR VS_pass2 (\r\n"
		"float4 inPos    : POSITION0				// 모델정점.\r\n"
		",float2 inTex : TEXCOORD0				// texUV.\r\n"
		")\r\n"
		"{\r\n"
		"VS_OUTPUT_FOR_RADIAL_BLUR output = (VS_OUTPUT_FOR_RADIAL_BLUR)0;\r\n"

		"// 위치좌표.\r\n"
		"inPos.xy = sign( inPos.xy );\r\n"
		"output.pos = float4( inPos.xy, 0, 1 );\r\n"

		"output.tex.x = g_vecCenter.x * ( 1 + inPos.x );\r\n"
		"output.tex.y = g_vecCenter.y * ( 1 - inPos.y );\r\n"

		"return output;\r\n"
		"}\r\n"
		"//\r\n"

		"// ------------------------------------------------------------\r\n"
		"// pixel shader for radial blur by luxes.\r\n"
		"// ------------------------------------------------------------\r\n"

		"float4 PS_RadialBlur( float2 inTex: TEXCOORD0, uniform int nsamples ): COLOR\r\n"

		"{\r\n"
		"// vector from pixel to the center of the screen.\r\n"
		"float2 dir = g_vecCenter - inTex;\r\n"

		"// float dist = distance( float2( 0.5, 0.5 ), inTex );\r\n"
		"float dist = sqrt( dir.x * dir.x + dir.y * dir.y );\r\n"

		"dir = normalize( dir );\r\n"

		"float4 color = tex2D( RadialBlurSampler, inTex );\r\n"

		"float4 sum = 0;\r\n"

		"for ( int i = 0; i < nsamples; i++ )\r\n"
		"{\r\n"
		"float scale = g_fBlurStart + g_fBlurWidth * ( i / (float)(nsamples - 1) );\r\n"
		"sum += tex2D( RadialBlurSampler, inTex + dir * scale );\r\n"
		"}\r\n"

		"sum /= nsamples;\r\n"

		"float t = dist * g_fBlurStrength;\r\n"
		"t = saturate( t );\r\n"

		"return lerp( color, sum, t );\r\n"
		"}\r\n"

		"sampler Texture;\r\n"

		"float4 PS_Tex( float2 Tex : TEXCOORD0 ) : COLOR\r\n"
		"{\r\n"
		"float4 Color = 0;\r\n"
		"Color = tex2D( Texture, Tex );\r\n"

		"return Color;\r\n"
		"}\r\n"

		"technique RadialBlurFX //effect\r\n"
		"{\r\n"
		"pass P0\r\n"
		"{\r\n"
		"VertexShader = compile vs_2_0 VS_pass2();\r\n"
		"pixelShader  = compile ps_2_0 PS_RadialBlur( 24 );\r\n"
		"}\r\n"

		"pass P1\r\n"
		"{\r\n"
		"pixelShader = compile ps_2_0 PS_Tex();\r\n"

		"CullMode = None;\r\n"
		"}\r\n"
		"}\r\n"

};

DxPostEffect& DxPostEffect::GetInstance()
{
	static DxPostEffect Instance;
	return Instance;
}

DxPostEffect::DxPostEffect()
	: m_pBaseSurface(NULL),
	m_pBaseSurfaceZ(NULL),
	m_pCurrentSurface(NULL),
	m_pCurrentSurfaceZ(NULL),
	m_pTexFinal(NULL),
	m_pSurFinal(NULL),
	m_pSurFinalZ(NULL),
	m_pRadialBlurEFF(NULL),
	m_hTechRadialBlur(NULL),
	m_hTexRadialBlur(NULL),
	m_hVecCenter(NULL),
	m_hRadialBlurStart(NULL),
	m_hRadialBlurWidth(NULL),
	m_hRadialBlurStrength(NULL),
	//m_pTexRadialBlur(NULL),
	//m_pSurRadialBlur(NULL),
	m_pSavedSB(NULL),
	m_pDrawSB(NULL)
	, m_bActivePostSurface(FALSE)
{
	// setting variables.
	m_bEnableHW = false;

	m_bRadialBlurSwitch = false;
	m_bRadialBlurFade = false;
	m_vecRadialBlurCenter = D3DXVECTOR2( 0.5f, 0.5f );
	m_fRadialBlurStart = 0.0f;
	m_fRadialBlurWidth = 0.0f;
	m_fRadialBlurWidthMAX = 0.1f;
	m_fRadialBlurStrength = 2.2f;
	m_fRadialBlurStrengthMAX = 2.2f;
	m_fRadialBlurRenderSpeed = 1.0f;

	m_bFadeConinue = false;
}

DxPostEffect::~DxPostEffect()
{
	SAFE_RELEASE( m_pBaseSurface );
	SAFE_RELEASE( m_pBaseSurfaceZ );
	SAFE_RELEASE( m_pCurrentSurface );
	SAFE_RELEASE( m_pCurrentSurfaceZ );
	SAFE_RELEASE( m_pTexFinal );
	SAFE_RELEASE( m_pSurFinal );
	SAFE_RELEASE( m_pSurFinalZ );
	SAFE_RELEASE( m_pRadialBlurEFF );
	//SAFE_RELEASE( m_pTexRadialBlur );
	//SAFE_RELEASE( m_pSurRadialBlur );
	//SAFE_RELEASE( m_pSurRadialBlurZ );
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );
}


HRESULT DxPostEffect::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);


	////////////////////////////////////////////////////////////////////////////
	//// 작동시키지 않는다.
	//m_bEnable = false;
	//return E_FAIL;



	//D3DCAPSQ d3dCaps;
	//pd3dDevice->GetDeviceCaps ( &d3dCaps );

	// Note : VB를 생성한다.
	m_sScreenVB[ 0 ].pos =	D3DXVECTOR4( 0-0.5f,						m_d3dsdBackBuffer.Height-0.5f,	0, 1 );
	m_sScreenVB[ 1 ].pos =	D3DXVECTOR4( 0-0.5f,						0-0.5f,							0, 1 );
	m_sScreenVB[ 2 ].pos =	D3DXVECTOR4( m_d3dsdBackBuffer.Width-0.5f,	m_d3dsdBackBuffer.Height-0.5f,	0, 1 );
	m_sScreenVB[ 3 ].pos =	D3DXVECTOR4( m_d3dsdBackBuffer.Width-0.5f,	0-0.5f,							0, 1 );

	m_sScreenVB[ 0 ].tex = D3DXVECTOR2( 0, 1 );
	m_sScreenVB[ 1 ].tex = D3DXVECTOR2( 0, 0 );
	m_sScreenVB[ 2 ].tex = D3DXVECTOR2( 1, 1 );
	m_sScreenVB[ 3 ].tex = D3DXVECTOR2( 1, 0 );

	m_sRadialBlurVB[ 0 ].pos =	D3DXVECTOR4( 0-0.5f,						m_d3dsdBackBuffer.Height-0.5f,	0, 1 );
	m_sRadialBlurVB[ 1 ].pos =	D3DXVECTOR4( 0-0.5f,						0-0.5f,							0, 1 );
	m_sRadialBlurVB[ 2 ].pos =	D3DXVECTOR4( m_d3dsdBackBuffer.Width-0.5f,	m_d3dsdBackBuffer.Height-0.5f,	0, 1 );
	m_sRadialBlurVB[ 3 ].pos =	D3DXVECTOR4( m_d3dsdBackBuffer.Width-0.5f,    0-0.5f,							0, 1 );

	m_sRadialBlurVB[ 0 ].tex = D3DXVECTOR2( 0, 1 );
	m_sRadialBlurVB[ 1 ].tex = D3DXVECTOR2( 0, 0 );
	m_sRadialBlurVB[ 2 ].tex = D3DXVECTOR2( 1, 1 );
	m_sRadialBlurVB[ 3 ].tex = D3DXVECTOR2( 1, 0 );

	//DxViewPort::GetInstance().SetFarPlane ( 1000000.f );				// View Cliping -> DxViewPort FrameMove Setting
	//DxViewPort::GetInstance().SetProjection_FarPlane ( 1000000.f );	// Far 클리핑을 멀리 보낸다.

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		/*pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,				TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,				D3DZB_TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,				D3DCMP_LESSEQUAL );
		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,			0 );
		pd3dDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS,	0 );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,				D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,		TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,				0 );
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_ALWAYS );
		pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,			TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_ZERO );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,				0 );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,				0 );
		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,			0 );
		pd3dDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS,	0 );
		pd3dDevice->SetRenderState( D3DRS_FOGSTART,				fFogStart );
		pd3dDevice->SetRenderState( D3DRS_FOGEND,				fFogEnd );
		pd3dDevice->SetRenderState( D3DRS_FOGDENSITY,			fFongDensity );
		pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,		D3DFOG_LINEAR  );
		pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,			D3DFOG_NONE );
		pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,		FALSE );*/

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	hr = InitShader( pd3dDevice );
	if ( hr == E_FAIL )
	{
		m_bEnableHW = false;

		return hr;
	}
	else
	{
		m_bEnableHW = true;
	}

	hr = InitFinalTarget( pd3dDevice );
	if ( hr == E_FAIL )
		m_bEnableHW = false;
	else
		m_bEnableHW = true;

	//// init shaders.
	//if ( InitRadialBlur( pd3dDevice ) == E_FAIL )
	//	m_bRadialBlurEnable = false;
	//else
	//	m_bRadialBlurEnable = true;

	return hr;
}

HRESULT DxPostEffect::InitShader( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_pRadialBlurEFF )
	{
		// 셰이더 읽기
		LPD3DXBUFFER pErr = NULL;	
		UINT size = (UINT)strlen(pRADIAL_BLUR_HLSL);
		if( FAILED(D3DXCreateEffect( pd3dDevice, pRADIAL_BLUR_HLSL, size, NULL, NULL, 0, NULL, &m_pRadialBlurEFF, &pErr ) ) )
		{
			CDebugSet::ToListView ( "[ERROR]D3DXCreateEffect FAILED ERROR: %s", pErr->GetBufferPointer() );

#ifdef _RELEASED
			MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK );
#endif

			SAFE_RELEASE(pErr);
			return E_FAIL;
		}

		m_pRadialBlurEFF->OnResetDevice();

		m_hTechRadialBlur		= m_pRadialBlurEFF->GetTechniqueByName( "RadialBlurFX" );

		m_hTexRadialBlur		= m_pRadialBlurEFF->GetParameterByName( NULL, "g_RadialBlurTex" );
		m_hVecCenter			= m_pRadialBlurEFF->GetParameterByName( NULL, "g_vecCenter" );
		m_hRadialBlurStart		= m_pRadialBlurEFF->GetParameterByName( NULL, "g_fBlurStart" );
		m_hRadialBlurWidth		= m_pRadialBlurEFF->GetParameterByName( NULL, "g_fBlurWidth" );
		m_hRadialBlurStrength	= m_pRadialBlurEFF->GetParameterByName( NULL, "g_fBlurStrength" );

		m_pRadialBlurEFF->SetTechnique( m_hTechRadialBlur );
	}

	return S_OK;
}

HRESULT DxPostEffect::InvalidateDeviceObjects()
{
	SAFE_RELEASE( m_pBaseSurface );
	SAFE_RELEASE( m_pBaseSurfaceZ );
	SAFE_RELEASE( m_pCurrentSurface );
	SAFE_RELEASE( m_pCurrentSurfaceZ );
	SAFE_RELEASE( m_pTexFinal );
	SAFE_RELEASE( m_pSurFinal );
	SAFE_RELEASE( m_pSurFinalZ );
	//SAFE_RELEASE( m_pTexRadialBlur );
	//SAFE_RELEASE( m_pSurRadialBlur );
	//SAFE_RELEASE( m_pSurRadialBlurZ );
	SAFE_RELEASE( m_pRadialBlurEFF );
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	return S_OK;
}

HRESULT DxPostEffect::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	//if ( !m_bEnable )
	//	return E_FAIL;

	LPDIRECT3DSURFACEQ pBackBuffer;
	hr = pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

	pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
	m_nWidth = (m_d3dsdBackBuffer.Width);
	m_nHeight = (m_d3dsdBackBuffer.Height);

	if ( hr != D3D_OK )
	{
		m_bEnableHW = false;

		return E_FAIL;
	}

	pBackBuffer->Release();

	if ( DxSurfaceTex::GetInstance().m_pColorSuf_Post )
	{
		// Note : Device 초기화.
		hr = InitDeviceObjects( pd3dDevice );
	}
	else
	{
		m_bEnableHW = false;
	}

	return hr;
}
//
//void DxPostEffect::SetTargetNull( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 ); // 한번 깨끗히 지워준다.
//
//	pd3dDevice->SetRenderTarget( 0, NULL );
//	pd3dDevice->SetDepthStencilSurface( NULL );
//}

void DxPostEffect::ChangeFinalTarget( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bZClear )
{
	// HW 상 post effect 불가.
	if ( !m_bEnableHW )
		return;

	// 후처리 Surface 동작 하는지 확인.
	if ( !m_bActivePostSurface )
		return;

	pd3dDevice->GetRenderTarget( 0, &m_pBaseSurface ); // 현재 target 저장.
	pd3dDevice->GetDepthStencilSurface( &m_pBaseSurfaceZ );

	pd3dDevice->SetRenderTarget( 0, m_pSurFinal ); // post effect를 위한 타겟 장착.

	D3DCOLOR colorClear = DxFogMan::GetInstance().GetFogColor();
	if ( bZClear )
	{
		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, colorClear, 1.0f, 0 ); // 한번 깨끗히 지워준다.
	}
	else
	{
		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, colorClear, 1.0f, 0 ); // 한번 깨끗히 지워준다.
	}
}

void DxPostEffect::RecoveryTargetFromFinal( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//if ( !m_bEnableHW ) // post effect 불가.
	//	return;

	pd3dDevice->SetRenderTarget( 0, m_pBaseSurface ); // target 장착.
	pd3dDevice->SetDepthStencilSurface( m_pBaseSurfaceZ );

	SAFE_RELEASE( m_pBaseSurface );
	SAFE_RELEASE( m_pBaseSurfaceZ );
}

HRESULT DxPostEffect::RenderPostEffect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// HW 상 post effect 불가.
	if ( !m_bEnableHW )
		return E_FAIL;

	// 후처리 Surface 동작 하는지 확인.
	if ( !m_bActivePostSurface )
		return S_OK;

	if ( !m_pSurFinal )
		return S_OK;

	if ( !DxSurfaceTex::GetInstance().m_pColorSuf_Post )
		return S_OK;

	// 현재의 surface 는 m_pSurFinal
	// 
	LPDIRECT3DSURFACEQ	pCurrentSurface(m_pSurFinal);
	NSEffectPlay::RenderSourceBackBuffer
	( 
		pd3dDevice, 
		m_pSurFinal, 
		m_pTexFinal, 
		DxSurfaceTex::GetInstance().m_pColorSuf_Post,
		DxSurfaceTex::GetInstance().m_pColorTex_Post, 
		pCurrentSurface 
	);

	LPDIRECT3DTEXTUREQ pCurrentTexture(NULL);
	LPDIRECT3DSURFACEQ pBackUpSurface(NULL);
	LPDIRECT3DTEXTUREQ pBackUpTexture(NULL);

	LPDIRECT3DSURFACEQ	pUsedTexture(NULL);
	if ( pCurrentSurface == m_pSurFinal )
	{
		//pCurrentSurface = m_pSurFinal; 이 값이니 안해도 됨.
		pCurrentTexture = m_pTexFinal;
		pBackUpSurface = DxSurfaceTex::GetInstance().m_pColorSuf_Post; 
		pBackUpTexture = DxSurfaceTex::GetInstance().m_pColorTex_Post;
	}
	else
	{
		//pCurrentSurface = DxSurfaceTex::GetInstance().m_pColorSuf_Post; 이 값이니 안해도 됨.
		pCurrentTexture = DxSurfaceTex::GetInstance().m_pColorTex_Post;
		pBackUpSurface = m_pSurFinal;
		pBackUpTexture = m_pTexFinal;
	}

	RecoveryTargetFromFinal( pd3dDevice ); // 렌더 타겟 되돌림.

    DWORD dwZEnable, dwZWriteEnable;
    pd3dDevice->GetRenderState( D3DRS_ZENABLE, &dwZEnable );
    pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE, &dwZWriteEnable );
    pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	if ( IsRadialBlurON() )
		RenderRadialBlur( pd3dDevice, pCurrentTexture, pBackUpSurface, pBackUpTexture );
	else
		RenderFinal( pd3dDevice, pCurrentTexture );

    pd3dDevice->SetRenderState( D3DRS_ZENABLE, dwZEnable );
    pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, dwZWriteEnable );

	return S_OK;
}

HRESULT DxPostEffect::InitFinalTarget( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	// Note : 최종 타겟 생성.
	D3DFORMAT emMainFormat(m_d3dsdBackBuffer.Format);
	D3DFORMAT emFormat(D3DFMT_X8R8G8B8);

	if( FAILED ( pd3dDevice->CreateTexture( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 0, D3DUSAGE_RENDERTARGET,			// 1. D3DFMT_X8R8G8B8
		emFormat, D3DPOOL_DEFAULT, &m_pTexFinal, NULL ) ) )
	{
		emFormat = D3DFMT_A8R8G8B8;
		if( FAILED ( pd3dDevice->CreateTexture( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 0, D3DUSAGE_RENDERTARGET,		// 2. D3DFMT_A8R8G8B8
			emFormat, D3DPOOL_DEFAULT, &m_pTexFinal, NULL ) ) )	
		{
			emFormat = emMainFormat;
			hr = pd3dDevice->CreateTexture( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 0, D3DUSAGE_RENDERTARGET,			// 3. 현재 Format
				emFormat, D3DPOOL_DEFAULT, &m_pTexFinal, NULL );
			if( FAILED(hr) )
			{
				m_bEnableHW = false;

				return E_FAIL;
			}
		}
	}
	m_pTexFinal->GetSurfaceLevel( 0, &m_pSurFinal );
	//if( FAILED ( pd3dDevice->CreateDepthStencilSurface( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE
	//	, 0, TRUE, &m_pSurFinalZ, NULL ) ) )
	//{
	//	if( FAILED ( pd3dDevice->CreateDepthStencilSurface( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, D3DFMT_D16, D3DMULTISAMPLE_NONE
	//		, 0, TRUE, &m_pSurFinalZ, NULL ) ) )
	//	{
	//		return E_FAIL;
	//	}
	//}

	return S_OK;
}

//HRESULT DxPostEffect::InitRadialBlur( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	HRESULT hr(S_OK);
//
//	D3DFORMAT emMainFormat(m_d3dsdBackBuffer.Format);
//	D3DFORMAT emFormat(D3DFMT_X8R8G8B8);
//
//	if( FAILED ( pd3dDevice->CreateTexture( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 0, D3DUSAGE_RENDERTARGET,			// 1. D3DFMT_X8R8G8B8
//		emFormat, D3DPOOL_DEFAULT, &m_pTexRadialBlur, NULL ) ) )
//	{
//		emFormat = D3DFMT_A8R8G8B8;
//		if( FAILED ( pd3dDevice->CreateTexture( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 0, D3DUSAGE_RENDERTARGET,		// 2. D3DFMT_A8R8G8B8
//			emFormat, D3DPOOL_DEFAULT, &m_pTexRadialBlur, NULL ) ) )	
//		{
//			emFormat = emMainFormat;
//			hr = pd3dDevice->CreateTexture( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 0, D3DUSAGE_RENDERTARGET,			// 3. 현재 Format
//				emFormat, D3DPOOL_DEFAULT, &m_pTexRadialBlur, NULL );
//			if( FAILED(hr) )	return E_FAIL;
//		}
//	}
//	m_pTexRadialBlur->GetSurfaceLevel( 0, &m_pSurRadialBlur );
//	//if( FAILED ( pd3dDevice->CreateDepthStencilSurface( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE
//	//	, 0, TRUE, &m_pSurRadialBlurZ, NULL ) ) )
//	//{
//	//	if( FAILED ( pd3dDevice->CreateDepthStencilSurface( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, D3DFMT_D16, D3DMULTISAMPLE_NONE
//	//		, 0, TRUE, &m_pSurRadialBlurZ, NULL ) ) )
//	//	{
//	//		return E_FAIL;
//	//	}
//	//}
//
//	return S_OK;
//}
//
//void DxPostEffect::PostEffectActiveOn()
//{
//	//m_bEnable = true;
//}
//
//void DxPostEffect::PostEffectActiveOff()
//{
//	//m_bEnable = false;
//}

HRESULT DxPostEffect::RenderFinal( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSourceTexture )
{
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

	pd3dDevice->SetTexture( 0, pSourceTexture );

	pd3dDevice->SetFVF( VERTEX_OF_POSTEFFECT::FVF );
	m_pRadialBlurEFF->Begin( NULL, 0 );
	m_pRadialBlurEFF->BeginPass(1);
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sScreenVB, sizeof( VERTEX_OF_POSTEFFECT ) );
	m_pRadialBlurEFF->EndPass();
	m_pRadialBlurEFF->End();

	return S_OK;
}

bool DxPostEffect::IsPostEffectEnable()
{
	if ( m_bEnableHW )
		return true;

	return false;
}

bool DxPostEffect::IsRadialBlurON()
{
	if ( !m_bEnableHW
		|| ( !m_bRadialBlurSwitch && !m_bRadialBlurFade ) ) // radial blur 꺼진 상황.
		return false;

	return true;
}

bool DxPostEffect::IsRadialBlurActiveON()
{
	if ( m_bRadialBlurSwitch )
		return true;

	return false;
}

bool DxPostEffect::IsRadialBlurActiveOff()
{
	if ( !m_bRadialBlurSwitch )
		return true;

	return false;
}

void DxPostEffect::RadialBlurActiveOn( float fStart /* = 0.0f  */, float fWidthMAX /* = 0.1f */
									  , float fStrength /* = 2.2f */, float fSpeed /* = 1.0f */ )
{
	if ( !m_bEnableHW )	
		return;

	if ( IsRadialBlurActiveON() )	
		return; // radial blur 켯으면 다시 켜지 않는다.

	m_bActivePostSurface = TRUE;

	m_bRadialBlurSwitch = true;
	m_bRadialBlurFade = false; // fade in.

	m_fRadialBlurStart = fStart;
	m_fRadialBlurWidth = 0.0f;
	m_fRadialBlurWidthMAX = fWidthMAX;
	m_fRadialBlurStrength = fStrength;
	m_fRadialBlurStrengthMAX = fStrength;
	m_fRadialBlurRenderSpeed = fSpeed;
}

void DxPostEffect::RadialBlurActiveOff()
{
	if ( !m_bEnableHW )	return;
	if ( IsRadialBlurActiveOff() )	return; // radial blur 껏으면 다시 끄지 않는다.

	m_bRadialBlurSwitch = false;
	m_bRadialBlurFade = true; // fade out.
}

HRESULT DxPostEffect::FrameMoveRadialBlur( float m_fTime, float m_fElapsedTime )
{
	if ( !m_bEnableHW )
		return E_FAIL;

	if ( !m_bRadialBlurSwitch && !m_bRadialBlurFade ) // radial blur를 하는 상황이 아니다.
		return S_OK;

	float deltaWidth = m_fElapsedTime * m_fRadialBlurRenderSpeed * DELTA_OF_WIDTH;

	if ( m_bRadialBlurSwitch && !m_bRadialBlurFade ) // radial blur를 시작하여 fade in 상황.
	{
		m_fRadialBlurWidth += deltaWidth;

		if ( m_fRadialBlurWidth >= m_fRadialBlurWidthMAX )
		{
			m_bRadialBlurFade = true;

			m_fRadialBlurWidth = m_fRadialBlurWidthMAX;

			m_bFadeConinue = false; // 감소 상태.
		}
	}

	if ( m_bRadialBlurSwitch && m_bRadialBlurFade ) // radial blur를 시작하여 fade in까지 완료 상황.
	{
		//if ( !m_bFadeConinue )
		//{	// 감소 상태.
		//	m_fRadialBlurStrength -= deltaStrength;

		//	if ( m_fRadialBlurStrength < m_fRadialBlurStrengthMAX * 0.9f )
		//		m_bFadeConinue = true; // 증가 상태로 바꿈.
		//}

		//else
		//{	// 증가 상태.
		//	m_fRadialBlurStrength += deltaStrength;
		//	if ( m_fRadialBlurStrength > m_fRadialBlurStrengthMAX * 1.1f )
		//		m_bFadeConinue = false; // 증가 상태로 바꿈.
		//}
	}

	if ( !m_bRadialBlurSwitch && m_bRadialBlurFade ) // radial blur를 종료할때, fade out 상황.
	{
		m_fRadialBlurWidth -= deltaWidth;

		if ( m_fRadialBlurWidth <= 0.0f )
		{
			m_bRadialBlurFade = false;

			m_fRadialBlurWidth = 0.0f;

			Check_SetDeactivePostSurface();
		}
	}

	return S_OK;
}

HRESULT DxPostEffect::RenderRadialBlur( LPDIRECT3DDEVICEQ pd3dDevice, 
										LPDIRECT3DTEXTUREQ pSourceTexture,
										LPDIRECT3DSURFACEQ pBackUpSurface,
										LPDIRECT3DTEXTUREQ pBackUpTexture )
{
	LPDIRECT3DSURFACEQ pCurrentSurface;
	//LPDIRECT3DSURFACEQ m_pCurrentSurfaceZ;

	pd3dDevice->GetRenderTarget( 0, &pCurrentSurface ); // 현재 target 저장.
	//pd3dDevice->GetDepthStencilSurface( &m_pCurrentSurfaceZ );

	pd3dDevice->SetRenderTarget( 0, pBackUpSurface );
	//pd3dDevice->SetRenderTarget( 0, m_pSurRadialBlur );
	//pd3dDevice->SetDepthStencilSurface( m_pSurRadialBlurZ );

	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

	m_pRadialBlurEFF->SetTexture( m_hTexRadialBlur, pSourceTexture );	//m_pTexFinal );
	m_pRadialBlurEFF->SetValue( m_hVecCenter, &( m_vecRadialBlurCenter ), sizeof( D3DXVECTOR2) );
	m_pRadialBlurEFF->SetValue( m_hRadialBlurStrength, &( m_fRadialBlurStrength ), sizeof( float) );
	m_pRadialBlurEFF->SetValue( m_hRadialBlurStart, &( m_fRadialBlurStart ), sizeof( float) );
	m_pRadialBlurEFF->SetValue( m_hRadialBlurWidth, &( m_fRadialBlurWidth ), sizeof( float) );
	m_pRadialBlurEFF->CommitChanges();


	pd3dDevice->SetFVF( VERTEX_OF_POSTEFFECT::FVF );
	m_pRadialBlurEFF->Begin( NULL, 0 );
	m_pRadialBlurEFF->BeginPass(0);
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sRadialBlurVB, sizeof( VERTEX_OF_POSTEFFECT ) );
	m_pRadialBlurEFF->EndPass();
	m_pRadialBlurEFF->End();

	pd3dDevice->SetRenderTarget( 0, pCurrentSurface );
	//pd3dDevice->SetDepthStencilSurface( m_pCurrentSurfaceZ );

	SAFE_RELEASE( pCurrentSurface );
	//SAFE_RELEASE( m_pCurrentSurfaceZ );

	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

	pd3dDevice->SetTexture( 0, pBackUpTexture );
	//pd3dDevice->SetTexture( 0, m_pTexRadialBlur );

	pd3dDevice->SetFVF( VERTEX_OF_POSTEFFECT::FVF );
	m_pRadialBlurEFF->Begin( NULL, 0 );
	m_pRadialBlurEFF->BeginPass(1);
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sScreenVB, sizeof( VERTEX_OF_POSTEFFECT ) );
	m_pRadialBlurEFF->EndPass();
	m_pRadialBlurEFF->End();

	return S_OK;
}

void DxPostEffect::Check_SetDeactivePostSurface()
{
	// 켜져있으면 활성화중.
	if ( m_bRadialBlurSwitch )
	{
		m_bActivePostSurface = TRUE;
		return;
	}

	// 켜져있으면 활성화중.
	if ( m_bRadialBlurFade )
	{
		m_bActivePostSurface = TRUE;
		return;
	}

	// 0 보다 크면 활성화 중.
	if ( m_fRadialBlurWidth > 0.0f )
	{
		m_bActivePostSurface = TRUE;
		return;	
	}

	if ( NSEffectPlay::IsTextureShader() )
	{
		m_bActivePostSurface = TRUE;
		return;	
	}

	m_bActivePostSurface = FALSE;
}