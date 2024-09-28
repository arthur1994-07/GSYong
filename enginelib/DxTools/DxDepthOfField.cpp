#include "pch.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "DxFogMan.h"
#include "DxViewPort.h"
#include "../DxTools/DxInputDevice.h"
#include ".\dxdepthoffield.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define V(x)           { hr = x; }

struct SCUSTOMVERTEX
{
	enum _FVF { FVF=(D3DFVF_XYZRHW|D3DFVF_TEX1) };
	float x, y, z, rhw;
	float tu, tv;
};

const DWORD DOFVERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX6;

static const char* pDOFHLSL = 
{
	"texture RenderTargetTexture;\r\n"
	"texture MeshTexture;\r\n"

	"float4x4 mWorld;\r\n"
	"float4x4 mWorldView;\r\n"
	"float4x4 mWorldViewProjection;\r\n"

	"float4 vFocalPlane;\r\n"
	"float  fHyperfocalDistance = 0.01f;\r\n"
	"float  MaxBlurFactor = 3.0f / 4.0f;\r\n"

	"float4 LightDir;\r\n"

	"float4 LightAmbient;\r\n"
	"float4 LightDiffuse;\r\n"

	"sampler RenderTargetSampler = \r\n"
	"sampler_state\r\n"
	"{\r\n"
	"	Texture = <RenderTargetTexture>;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"

	"	AddressU = Clamp;\r\n"
	"	AddressV = Clamp;\r\n"
	"};\r\n"

	"sampler MeshTextureSampler = \r\n"
	"sampler_state\r\n"
	"{\r\n"
	"	Texture = <MeshTexture>;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"
	"	MipFilter = LINEAR;\r\n"
	"};\r\n"

	"struct VS_OUTPUT\r\n"
	"{\r\n"
	"	float4 Position : POSITION;\r\n"
	"	float4 Diffuse : COLOR;\r\n"
	"	float2 TextureUV : TEXCOORD0;\r\n"
	"};\r\n"

	"VS_OUTPUT WorldVertexShader( float4 vPos : POSITION, \r\n"
	"							float3 vNormal : NORMAL,\r\n"
	"							float2 vTexCoord0 : TEXCOORD0 )\r\n"
	"{\r\n"
	"	VS_OUTPUT Output;\r\n"
	"	float3 vViewPosition;\r\n"
	"	float3 vWorldNormal;\r\n"
	"	float  fBlurFactor;\r\n"

	"	Output.Position = mul(vPos, mWorldViewProjection);\r\n"
	"	vViewPosition = mul(vPos, (float4x3)mWorldView);\r\n"
	"	vWorldNormal = mul(vNormal, (float3x3)mWorld);       \r\n"

    "	Output.Diffuse.rgb = (LightDiffuse * max(0,dot(vWorldNormal, -LightDir)) * LightAmbient)/1.32f;\r\n"

	"	fBlurFactor      = dot(float4(vViewPosition, 1.0), vFocalPlane)*fHyperfocalDistance;\r\n"
	"	Output.Diffuse.a = fBlurFactor*fBlurFactor;\r\n"
	"	Output.Diffuse.a = min(Output.Diffuse.a, MaxBlurFactor);\r\n"

	"	Output.Diffuse.a = 1.0f - Output.Diffuse.a;\r\n"

	"	Output.TextureUV = vTexCoord0;\r\n"

	"	return Output;    \r\n"
	"}\r\n"


	"float4 WorldPixelShader( VS_OUTPUT In ) : COLOR\r\n"
	"{ \r\n"
	"	return tex2D(MeshTextureSampler, In.TextureUV) * In.Diffuse;\r\n"
	"}\r\n"

	"float4 DepthOfFieldFourTexcoords( in float2 OriginalUV : TEXCOORD0,\r\n"
	"								 in float2 JitterUV[3] : TEXCOORD1 ) : COLOR\r\n"
	"{    \r\n"
	"	float4 Original = tex2D(RenderTargetSampler, OriginalUV);\r\n"
	"	float Alpha = 1.0f - Original.a;\r\n"
	"	float4 Jitter[3];\r\n"
	"	float3 Blurred;\r\n"

	"	for(int i = 0; i < 3; i++)\r\n"
	"	{\r\n"
	"		Jitter[i] = tex2D(RenderTargetSampler, JitterUV[i]);\r\n"
	"		Jitter[i].rgb = lerp(Original.rgb, Jitter[i].rgb, Alpha);\r\n"

	"	}\r\n"

	"	Blurred = lerp(Jitter[0].rgb, Jitter[1].rgb, 0.5);\r\n"
	"	Blurred = lerp(Jitter[2].rgb, Blurred, 0.66666);\r\n"

	"	float4 returnValue = float4(Blurred, 1.0f);\r\n"        
	"	if( Original.a <= 0.02f ) returnValue = Original;\r\n"

	"	return returnValue;\r\n"
	"}\r\n"

	"float4 RenderBlurFactor( in float2 OriginalUV : TEXCOORD0 ) : COLOR \r\n"
	"{\r\n"
	"	float4 Original = tex2D(RenderTargetSampler, OriginalUV);\r\n"
	"	float4 returnValue = Original.a;        \r\n"
	"	if( Original.a <= 0.02f ) returnValue = 1.0f;\r\n"

	"	return returnValue; // Invert so the screen isn't pure white\r\n"
	"}\r\n"

	"technique WorldWithBlurFactor\r\n"
	"{\r\n"
	"	pass P0\r\n"
	"	{        \r\n"
	"		VertexShader = compile vs_1_1 WorldVertexShader();\r\n"
	"		PixelShader  = compile ps_1_1 WorldPixelShader();\r\n"
	"	}\r\n"
	"}\r\n"

	"technique UsePS11FourTexcoordsWithRings<float MaxBlurFactor = 3.0f / 4.0f;>\r\n"
	"{\r\n"
	"	pass P0\r\n"
	"	{        \r\n"
	"		PixelShader = compile ps_2_0 DepthOfFieldFourTexcoords();\r\n"
	"	}\r\n"
	"}\r\n"
	"technique ShowBlurFactor\r\n"
	"{\r\n"
	"	pass P0\r\n"
	"	{        \r\n"
	"		PixelShader = compile ps_2_0 RenderBlurFactor();\r\n"
	"	}\r\n"
	"}\r\n"
};

DxDepthOfField& DxDepthOfField::GetInstance()
{
	static DxDepthOfField Instance;
	return Instance;
}

DxDepthOfField::DxDepthOfField(void)
				: m_pd3dDevice(NULL)

				, m_pEffect(NULL)
				, m_vFocalPlane(0.0f, 0.0f, 1.0f, -120.0f)


				, m_fBlurConst(4.0f)

				, m_hFocalPlane(NULL)
				, m_hWorld(NULL)
				, m_hWorldView(NULL)
				, m_hWorldViewProjection(NULL)
				, m_hMeshTexture(NULL)
				, m_hTechWorldWithBlurFactor(NULL)
				, m_hTechDepthOfField(NULL)
				, m_hTechShowBlurFactor(NULL)
{
}

DxDepthOfField::~DxDepthOfField(void)
{
}

HRESULT DxDepthOfField::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pFullScreenTextureSurf);
	SAFE_RELEASE(m_pFullScreenTexture);
	SAFE_RELEASE(m_pRenderToSurface);
	SAFE_RELEASE( m_pEffect );

	return S_OK;
}

HRESULT DxDepthOfField::InvalidateDeviceObjects()
{
	m_pEffect->OnLostDevice();
	return S_OK;
}

HRESULT DxDepthOfField::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{

	m_pd3dDevice = pd3dDevice;

	if( !m_pEffect )
	{

		// ¼ÎÀÌ´õ ÀÐ±â
		LPD3DXBUFFER pErr = NULL;	
		UINT size = (UINT)strlen(pDOFHLSL);
		if( FAILED(D3DXCreateEffect( pd3dDevice, pDOFHLSL, size, NULL, NULL, 0, NULL, &m_pEffect, &pErr ) ) )
		{
			CDebugSet::ToListView ( "[ERROR]D3DXCreateEffect FAILED ERROR: %s", pErr->GetBufferPointer() );

#ifdef _RELEASED
			MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK );
#endif

			SAFE_RELEASE(pErr);
			return E_FAIL;
		}
	}

	return S_OK;

}


void DxDepthOfField::SetupQuad()
{
	//--------------------------------------------------------------------------------------
	// Sets up a quad to render the fullscreen render target to the backbuffer
	// so it can run a fullscreen pixel shader pass that blurs based
	// on the depth of the objects.  It set the texcoords based on the blur factor
	//--------------------------------------------------------------------------------------

	D3DSURFACE_DESC BackBufferSurfaceDesc;
	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc( &BackBufferSurfaceDesc );
	pBackBuffer->Release();

	D3DSURFACE_DESC desc;
	m_pFullScreenTextureSurf->GetDesc(&desc);


	FLOAT fWidth5 = (FLOAT)BackBufferSurfaceDesc.Width/2.0f - 0.5f;
	FLOAT fHeight5 = (FLOAT)BackBufferSurfaceDesc.Height - 0.5f;

	FLOAT fHalf = m_fBlurConst;
	FLOAT fOffOne = fHalf * 0.5f;
	FLOAT fOffTwo = fOffOne * sqrtf(3.0f);

	FLOAT fTexWidth1 = (FLOAT)BackBufferSurfaceDesc.Width/2.0f / (FLOAT)desc.Width;
	FLOAT fTexHeight1 = (FLOAT)BackBufferSurfaceDesc.Height / (FLOAT)desc.Height;

	FLOAT fWidthMod = 1.0f / (FLOAT)desc.Width ;
	FLOAT fHeightMod = 1.0f / (FLOAT)desc.Height;

	// Create vertex buffer.  
	// m_Vertex[0].tex1 == full texture coverage
	// m_Vertex[0].tex2 == full texture coverage, but shifted y by -fHalf*fHeightMod
	// m_Vertex[0].tex3 == full texture coverage, but shifted x by -fOffTwo*fWidthMod & y by -fOffOne*fHeightMod
	// m_Vertex[0].tex4 == full texture coverage, but shifted x by +fOffTwo*fWidthMod & y by -fOffOne*fHeightMod
	// m_Vertex[0].tex5 == full texture coverage, but shifted x by -fOffTwo*fWidthMod & y by +fOffOne*fHeightMod
	// m_Vertex[0].tex6 == full texture coverage, but shifted x by +fOffTwo*fWidthMod & y by +fOffOne*fHeightMod
	m_Vertex[0].pos = D3DXVECTOR4(fWidth5, -0.5f, 0.0f, 1.0f);
	m_Vertex[0].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	m_Vertex[0].tex1 = D3DXVECTOR2(fTexWidth1, 0.0f);
	m_Vertex[0].tex2 = D3DXVECTOR2(fTexWidth1, 0.0f - fHalf*fHeightMod);
	m_Vertex[0].tex3 = D3DXVECTOR2(fTexWidth1 - fOffTwo*fWidthMod, 0.0f - fOffOne*fHeightMod);
	m_Vertex[0].tex4 = D3DXVECTOR2(fTexWidth1 + fOffTwo*fWidthMod, 0.0f - fOffOne*fHeightMod);
	m_Vertex[0].tex5 = D3DXVECTOR2(fTexWidth1 - fOffTwo*fWidthMod, 0.0f + fOffOne*fHeightMod);
	m_Vertex[0].tex6 = D3DXVECTOR2(fTexWidth1 + fOffTwo*fWidthMod, 0.0f + fOffOne*fHeightMod);

	m_Vertex[1].pos = D3DXVECTOR4(fWidth5, fHeight5, 0.0f, 1.0f);
	m_Vertex[1].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	m_Vertex[1].tex1 = D3DXVECTOR2(fTexWidth1, fTexHeight1);
	m_Vertex[1].tex2 = D3DXVECTOR2(fTexWidth1, fTexHeight1 - fHalf*fHeightMod);
	m_Vertex[1].tex3 = D3DXVECTOR2(fTexWidth1 - fOffTwo*fWidthMod, fTexHeight1 - fOffOne*fHeightMod);
	m_Vertex[1].tex4 = D3DXVECTOR2(fTexWidth1 + fOffTwo*fWidthMod, fTexHeight1 - fOffOne*fHeightMod);
	m_Vertex[1].tex5 = D3DXVECTOR2(fTexWidth1 - fOffTwo*fWidthMod, fTexHeight1 + fOffOne*fHeightMod);
	m_Vertex[1].tex6 = D3DXVECTOR2(fTexWidth1 + fOffTwo*fWidthMod, fTexHeight1 + fOffOne*fHeightMod);

	m_Vertex[2].pos = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
	m_Vertex[2].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	m_Vertex[2].tex1 = D3DXVECTOR2(0.0f, 0.0f);
	m_Vertex[2].tex2 = D3DXVECTOR2(0.0f, 0.0f - fHalf*fHeightMod);
	m_Vertex[2].tex3 = D3DXVECTOR2(0.0f - fOffTwo*fWidthMod, 0.0f - fOffOne*fHeightMod);
	m_Vertex[2].tex4 = D3DXVECTOR2(0.0f + fOffTwo*fWidthMod, 0.0f - fOffOne*fHeightMod);
	m_Vertex[2].tex5 = D3DXVECTOR2(0.0f - fOffTwo*fWidthMod, 0.0f + fOffOne*fHeightMod);
	m_Vertex[2].tex6 = D3DXVECTOR2(0.0f + fOffTwo*fWidthMod, 0.0f + fOffOne*fHeightMod);

	m_Vertex[3].pos = D3DXVECTOR4(-0.5f, fHeight5, 0.0f, 1.0f);
	m_Vertex[3].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	m_Vertex[3].tex1 = D3DXVECTOR2(0.0f, fTexHeight1);
	m_Vertex[3].tex2 = D3DXVECTOR2(0.0f, fTexHeight1 - fHalf*fHeightMod);
	m_Vertex[3].tex3 = D3DXVECTOR2(0.0f - fOffTwo*fWidthMod, fTexHeight1 - fOffOne*fHeightMod);
	m_Vertex[3].tex4 = D3DXVECTOR2(0.0f + fOffTwo*fWidthMod, fTexHeight1 - fOffOne*fHeightMod);
	m_Vertex[3].tex5 = D3DXVECTOR2(0.0f + fOffTwo*fWidthMod, fTexHeight1 + fOffOne*fHeightMod);
	m_Vertex[3].tex6 = D3DXVECTOR2(0.0f - fOffTwo*fWidthMod, fTexHeight1 + fOffOne*fHeightMod);
}

HRESULT DxDepthOfField::RestoreDeviceObjects()
{
	if( !m_pEffect ) return E_FAIL;

	m_pEffect->OnResetDevice();

	m_pd3dDevice->GetViewport(&m_ViewportFB);

	// Backbuffer viewport is identical to frontbuffer, except starting at 0, 0
	m_ViewportOffscreen = m_ViewportFB;
	m_ViewportOffscreen.X = 0;
	m_ViewportOffscreen.Y = 0;

	HRESULT hr;
	D3DSURFACE_DESC BackBufferSurfaceDesc;
	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	V_RETURN(m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ));
	V_RETURN(pBackBuffer->GetDesc( &BackBufferSurfaceDesc ));
	pBackBuffer->Release();
	
	// Create fullscreen renders target texture
	hr = D3DXCreateTexture( m_pd3dDevice, BackBufferSurfaceDesc.Width, BackBufferSurfaceDesc.Height, 
		1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pFullScreenTexture);
	if( FAILED(hr) )
	{
		// Fallback to a non-RT texture
		V_RETURN( D3DXCreateTexture( m_pd3dDevice, BackBufferSurfaceDesc.Width, BackBufferSurfaceDesc.Height, 
			1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pFullScreenTexture ) );
	}

	D3DSURFACE_DESC desc;
	m_pFullScreenTexture->GetSurfaceLevel(0, &m_pFullScreenTextureSurf);
	m_pFullScreenTextureSurf->GetDesc(&desc);

	// Create a ID3DXRenderToSurface to help render to a texture on cards 
	// that don't support render targets
	V_RETURN( D3DXCreateRenderToSurface( m_pd3dDevice, desc.Width, desc.Height, 
		desc.Format, TRUE, D3DFMT_D16, &m_pRenderToSurface ) );

	// clear the surface alpha to 0 so that it does not bleed into a "blurry" background
	//   this is possible because of the avoidance of blurring in a non-blurred texel
	if(SUCCEEDED(m_pRenderToSurface->BeginScene(m_pFullScreenTextureSurf, NULL)))
	{
		m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xff, 1.0f, 0);
		m_pRenderToSurface->EndScene( 0 );
	}

	// Get D3DXHANDLEs to the parameters/techniques that are set every frame so 
	// D3DX doesn't spend time doing string compares.  Doing this likely won't affect
	// the perf of this simple sample but it should be done in complex engine.
	m_hFocalPlane               = m_pEffect->GetParameterByName( NULL, "vFocalPlane" );
	m_hWorld                    = m_pEffect->GetParameterByName( NULL, "mWorld" );
	m_hWorldView                = m_pEffect->GetParameterByName( NULL, "mWorldView" );
	m_hWorldViewProjection      = m_pEffect->GetParameterByName( NULL, "mWorldViewProjection" );
	m_hMeshTexture              = m_pEffect->GetParameterByName( NULL, "MeshTexture" );
	m_hLightDir					= m_pEffect->GetParameterByName(NULL,"LightDir");
	m_hLightAmbient				= m_pEffect->GetParameterByName(NULL,"LightAmbient");
	m_hLightDiffuse				= m_pEffect->GetParameterByName(NULL,"LightDiffuse");

	m_hTechWorldWithBlurFactor  = m_pEffect->GetTechniqueByName("WorldWithBlurFactor");
	m_hTechShowBlurFactor       = m_pEffect->GetTechniqueByName("ShowBlurFactor");
	m_hTechDepthOfField			= m_pEffect->GetTechniqueByName("UsePS11FourTexcoordsWithRings");

	V_RETURN( m_pEffect->SetTexture("RenderTargetTexture", m_pFullScreenTexture) );


	SetupQuad();


	return S_OK;
}

HRESULT DxDepthOfField::FrameMove ()
{
	if( DxInputDevice::GetInstance().GetKeyState( DIK_INSERT ) & DXKEY_DOWNED)
	{
		m_vFocalPlane.w += 0.5f;
	}else if( DxInputDevice::GetInstance().GetKeyState( DIK_DELETE ) & DXKEY_DOWNED)
	{
		m_vFocalPlane.w -= 0.5f;
	}


	if( DxInputDevice::GetInstance().GetKeyState( DIK_HOME ) & DXKEY_DOWNED)
	{
		m_fBlurConst += 0.01f;
		SetupQuad();
	}else if( DxInputDevice::GetInstance().GetKeyState( DIK_END ) & DXKEY_DOWNED)
	{
		m_fBlurConst -= 0.01f;
		SetupQuad();
	}

	CDebugSet::ToView( 1, "m_fBlurConst: %f m_vFocalPlane: %f", m_fBlurConst, m_vFocalPlane.w );

	return S_OK;
}

HRESULT DxDepthOfField::BeginDOF()
{
	HRESULT hr;

	D3DCOLOR colorClear = D3DCOLOR_XRGB(89,135,179);
	DxFogMan::GetInstance().RenderFogSB ( m_pd3dDevice, FALSE );
	colorClear = DxFogMan::GetInstance().GetFogColor();


	if( SUCCEEDED( m_pRenderToSurface->BeginScene(m_pFullScreenTextureSurf, &m_ViewportOffscreen) ) )
	{
		V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, colorClear, 1.0f, 0) );


		// Update focal plane
		m_pEffect->SetVector( m_hFocalPlane, &m_vFocalPlane);

		// Set world render technique
		V( m_pEffect->SetTechnique( m_hTechWorldWithBlurFactor ) );


	}else{
		return E_FAIL;
	}

	return hr;
}

HRESULT DxDepthOfField::EndDOF()
{
	if( !m_pEffect ) return E_FAIL;
	HRESULT hr;	
	V( m_pRenderToSurface->EndScene( 0 ) );
	return hr;
}

HRESULT DxDepthOfField::BeginDOFRender(D3DXMATRIX matWorld)
{
	HRESULT hr;
	D3DXMATRIX matView, matProj;
	m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

	D3DLIGHT9 light;
	m_pd3dDevice->GetLight( 0, &light );

	D3DXMATRIXA16 matViewProj = matView * matProj;	
	D3DXMATRIXA16 matWorldViewProj = matWorld * matViewProj;
	D3DXMATRIXA16 matWorldView = matWorld * matView;

	D3DXVECTOR4 vLightDir	   = D3DXVECTOR4( light.Direction.x, light.Direction.y, light.Direction.z, 1.0f );
	//D3DXVECTOR4 vLightAmbient  = D3DXVECTOR4( light.Ambient.r, light.Ambient.g, light.Ambient.b, light.Ambient.a );
	//D3DXVECTOR4 vLightDiffuse  = D3DXVECTOR4( light.Diffuse.r, light.Diffuse.g, light.Diffuse.b, light.Diffuse.a );		
	D3DXVECTOR4 vLightAmbient  = D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
	D3DXVECTOR4 vLightDiffuse  = D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );		

	m_pEffect->SetVector( m_hLightDir, &vLightDir );
	m_pEffect->SetVector( m_hLightAmbient, &vLightAmbient );
	m_pEffect->SetVector( m_hLightDiffuse, &vLightDiffuse );




	V( m_pEffect->SetMatrix( m_hWorld, &matWorld) );
	V( m_pEffect->SetMatrix( m_hWorldView, &matWorldView) );
	V( m_pEffect->SetMatrix( m_hWorldViewProjection, &matWorldViewProj) );

	// Draw the mesh on the render target
	V( m_pEffect->Begin(NULL, 0) );
	V( m_pEffect->BeginPass(0) );

	return hr;
}

HRESULT DxDepthOfField::BeginDOFRender()
{
	HRESULT hr;
	D3DXMATRIX matView, matProj, matWorld;
	D3DXMatrixIdentity( &matWorld );
	m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

	D3DLIGHT9 light;
	m_pd3dDevice->GetLight( 0, &light );

	D3DXMATRIXA16 matViewProj = matView * matProj;	
	D3DXMATRIXA16 matWorldViewProj = matWorld * matViewProj;
	D3DXMATRIXA16 matWorldView = matWorld * matView;

	D3DXVECTOR4 vLightDir	   = D3DXVECTOR4( light.Direction.x, light.Direction.y, light.Direction.z, 1.0f );
	//D3DXVECTOR4 vLightAmbient  = D3DXVECTOR4( light.Ambient.r, light.Ambient.g, light.Ambient.b, light.Ambient.a );
	//D3DXVECTOR4 vLightDiffuse  = D3DXVECTOR4( light.Diffuse.r, light.Diffuse.g, light.Diffuse.b, light.Diffuse.a );		
	D3DXVECTOR4 vLightAmbient  = D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
	D3DXVECTOR4 vLightDiffuse  = D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );		

	m_pEffect->SetVector( m_hLightDir, &vLightDir );
	m_pEffect->SetVector( m_hLightAmbient, &vLightAmbient );
	m_pEffect->SetVector( m_hLightDiffuse, &vLightDiffuse );




	V( m_pEffect->SetMatrix( m_hWorld, &matWorld) );
	V( m_pEffect->SetMatrix( m_hWorldView, &matWorldView) );
	V( m_pEffect->SetMatrix( m_hWorldViewProjection, &matWorldViewProj) );

	// Draw the mesh on the render target
	V( m_pEffect->Begin(NULL, 0) );
	V( m_pEffect->BeginPass(0) );

	return hr;
}

HRESULT DxDepthOfField::EndDOFRender()
{
	if( !m_pEffect ) return E_FAIL;
	HRESULT hr;
	V( m_pEffect->EndPass() );
	V( m_pEffect->End() );
	return hr;
}


HRESULT DxDepthOfField::RenderDOF()
{
	if( !m_pEffect ) return E_FAIL;

	HRESULT hr;
	UINT cPasses, iPass;

	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	m_pEffect->SetTechnique( m_hTechDepthOfField );
	//m_pEffect->SetTechnique( m_hTechShowBlurFactor );

	V( m_pEffect->Begin(&cPasses, 0) );
	for (iPass = 0; iPass < cPasses; iPass++)
	{
		m_pd3dDevice->SetViewport(&m_ViewportFB);

		V( m_pEffect->BeginPass(iPass) );
		V( m_pd3dDevice->SetFVF(DOFVERTEX::FVF) );
		V( m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, m_Vertex, sizeof(DOFVERTEX)) );
		V( m_pEffect->EndPass() );
	}
	V( m_pEffect->End() );

	D3DSURFACE_DESC BackBufferSurfaceDesc;
	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc( &BackBufferSurfaceDesc );
	pBackBuffer->Release();

	D3DXVECTOR2 vMin = D3DXVECTOR2( 0.0, 0.0f);
	D3DXVECTOR2 vMax = D3DXVECTOR2( static_cast<FLOAT>(BackBufferSurfaceDesc.Width)
        , static_cast<FLOAT>(BackBufferSurfaceDesc.Height) );

	SCUSTOMVERTEX pVertices[4] = {
		{ vMin.x, vMin.y, 0.5f, 1.0f, 0.0f, 0.0f },
		{ vMax.x, vMin.y, 0.5f, 1.0f, 1.0f, 0.0f },
		{ vMin.x, vMax.y, 0.5f, 1.0f, 0.0f, 1.0f },
		{ vMax.x, vMax.y, 0.5f, 1.0f, 1.0f, 1.0f }};



		m_pd3dDevice->SetTexture( 0, m_pFullScreenTexture );
		m_pd3dDevice->SetFVF( SCUSTOMVERTEX::FVF );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(SCUSTOMVERTEX) );

	return S_OK;
}
