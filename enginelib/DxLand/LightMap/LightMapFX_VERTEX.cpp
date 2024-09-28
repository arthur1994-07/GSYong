#include "pch.h"

#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxFogMan.h"
#include "../../G-Logic/GLPeriod.h"
#include "./NSLightMapFX.h"

#include "LightMapFX_VERTEX.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


LightMapFX_VERTEX::LightMapFX_VERTEX()
	: m_pDCRT(NULL)
{
}

LightMapFX_VERTEX::~LightMapFX_VERTEX()
{
}

HRESULT LightMapFX_VERTEX::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	const D3DVERTEXELEMENT9 VertexDecl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		D3DDECL_END()
	};

	pd3dDevice->CreateVertexDeclaration( VertexDecl,  &m_pDCRT );

	if( m_pFX )	
		V_RETURN( m_pFX->OnResetDevice() );

	return hr;
}

HRESULT LightMapFX_VERTEX::OnLostDevice()
{
	HRESULT hr(S_OK);

	if( m_pFX )	
		V_RETURN( m_pFX->OnLostDevice() );

	SAFE_RELEASE( m_pDCRT );

	return hr;
}

void LightMapFX_VERTEX::SetFX( TSTRING& strFX )
{
	strFX = 
		"//-------------------------------------------------------------------------------------- \r\n"
		"// File: LightMap_Piece.fx \r\n"
		"//       shhan \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		" \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		"// Global variables \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		"float4x4 g_matWORLD; \r\n"
		"float4x4 g_matWVP; \r\n"
		"float2   g_vFOG;	// FogEnd, FogDist \r\n"
		"float2   g_vLightMapUV_Offset; \r\n"
		"float2   g_vLightMapUV_Multiply; \r\n"
		"float2   g_vDayNightWeight; \r\n"
		" \r\n"
		"texture LightMapDayTex; \r\n"
		"texture LightMapNightTex; \r\n"
		" \r\n"
		"sampler LightMapDayTexSampler = sampler_state  \r\n"
		"{ \r\n"
		"    Texture = (LightMapDayTex); \r\n"
		"}; \r\n"
		" \r\n"
		"sampler LightMapNightTexSampler = sampler_state  \r\n"
		"{ \r\n"
		"    Texture = (LightMapNightTex); \r\n"
		"}; \r\n"
		" \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		"// Basic vertex transformation \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		"struct VS_INPUT { \r\n"
		"    float4 m_vPosition  : POSITION; \r\n"
		"    float3 m_vNormal    : NORMAL; \r\n"
		"    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
		"}; \r\n"
		" \r\n"
		"struct VS_OUTPUT  \r\n"
		"{ \r\n"
		"    float4 m_vPosition  : POSITION; \r\n"
		"    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
		"    float  m_fFog      : FOG; \r\n"
		"}; \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//						V	S \r\n"
		"VS_OUTPUT VS( VS_INPUT In )  \r\n"
		"{ \r\n"
		"    VS_OUTPUT Out; \r\n"
		"     \r\n"
		"    // Now that we have the calculated weight, add in the final influence \r\n"
		"    Out.m_vPosition = mul(g_matWVP, In.m_vPosition); \r\n"
		" \r\n"
		"    // Pass the texture coordinate \r\n"
		"    Out.m_vTexCoord0 = In.m_vTexCoord0; \r\n"
		"    Out.m_vTexCoord0 += g_vLightMapUV_Offset; \r\n"
		"    Out.m_vTexCoord0 *= g_vLightMapUV_Multiply; \r\n"
		" \r\n"
		"    Out.m_fFog = saturate((g_vFOG.x - Out.m_vPosition.z) / g_vFOG.y); \r\n"
		"    \r\n"
		"    return Out; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//						V	S \r\n"
		"VS_OUTPUT VS_XY( VS_INPUT In )  \r\n"
		"{ \r\n"
		"    VS_OUTPUT Out; \r\n"
		"     \r\n"
		"    // Now that we have the calculated weight, add in the final influence \r\n"
		"    float4 vPos = mul(g_matWORLD, In.m_vPosition); \r\n"
		"    Out.m_vPosition = mul(g_matWVP, In.m_vPosition); \r\n"
		" \r\n"
		"    // Pass the texture coordinate \r\n"
		"    Out.m_vTexCoord0.x = vPos.x; \r\n"
		"    Out.m_vTexCoord0.y = vPos.y; \r\n"
		"    Out.m_vTexCoord0 += g_vLightMapUV_Offset; \r\n"
		"    Out.m_vTexCoord0 *= g_vLightMapUV_Multiply; \r\n"
		" \r\n"
		"    Out.m_fFog = saturate((g_vFOG.x - Out.m_vPosition.z) / g_vFOG.y); \r\n"
		"    \r\n"
		"    return Out; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//						V	S \r\n"
		"VS_OUTPUT VS_XZ( VS_INPUT In )  \r\n"
		"{ \r\n"
		"    VS_OUTPUT Out; \r\n"
		"     \r\n"
		"    // Now that we have the calculated weight, add in the final influence \r\n"
		"    float4 vPos = mul(g_matWORLD, In.m_vPosition); \r\n"
		"    Out.m_vPosition = mul(g_matWVP, In.m_vPosition); \r\n"
		" \r\n"
		"    // Pass the texture coordinate \r\n"
		"    Out.m_vTexCoord0.x = vPos.x; \r\n"
		"    Out.m_vTexCoord0.y = vPos.z; \r\n"
		"    Out.m_vTexCoord0 += g_vLightMapUV_Offset; \r\n"
		"    Out.m_vTexCoord0 *= g_vLightMapUV_Multiply; \r\n"
		" \r\n"
		"    Out.m_fFog = saturate((g_vFOG.x - Out.m_vPosition.z) / g_vFOG.y); \r\n"
		"    \r\n"
		"    return Out; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//						V	S \r\n"
		"VS_OUTPUT VS_YZ( VS_INPUT In )  \r\n"
		"{ \r\n"
		"    VS_OUTPUT Out; \r\n"
		"     \r\n"
		"    // Now that we have the calculated weight, add in the final influence \r\n"
		"    float4 vPos = mul(g_matWORLD, In.m_vPosition); \r\n"
		"    Out.m_vPosition = mul(g_matWVP, In.m_vPosition); \r\n"
		" \r\n"
		"    // Pass the texture coordinate \r\n"
		"    Out.m_vTexCoord0.x = vPos.y; \r\n"
		"    Out.m_vTexCoord0.y = vPos.z; \r\n"
		"    Out.m_vTexCoord0 += g_vLightMapUV_Offset; \r\n"
		"    Out.m_vTexCoord0 *= g_vLightMapUV_Multiply; \r\n"
		" \r\n"
		"    Out.m_fFog = saturate((g_vFOG.x - Out.m_vPosition.z) / g_vFOG.y); \r\n"
		"    \r\n"
		"    return Out; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//					P	S	_	Only LightMap \r\n"
		"// 원본 \r\n"
		"float4 PS_ONLY_LIGHTMAP( VS_OUTPUT In ) : COLOR0  \r\n"
		"{ \r\n"
		"    // Texture \r\n"
		"    float4 vColor = 1.f; \r\n"
		"    float4 vLightMapDay = tex2D( LightMapDayTexSampler, In.m_vTexCoord0 ); \r\n"
		"    float4 vLightMapNight = tex2D( LightMapNightTexSampler, In.m_vTexCoord0 ); \r\n"
		" \r\n"
		"    // Basic N dot L lighting \r\n"
		"    float4 vLightColor = vLightMapDay*g_vDayNightWeight.x + vLightMapNight*g_vDayNightWeight.y; \r\n"
		"    vColor.xyz *= vLightColor*2.f; \r\n"
		" \r\n"
		"    return vColor; \r\n"
		"} \r\n"
		" \r\n"
		"technique Base \r\n"
		"{ \r\n"
		"    //------------------------------------------------------------------------------------- \r\n"
		"    // Only LightMap (Edit시에만 사용)	- VS_1_1 이상 지원 \r\n"
		"    //									- PS_1_4 이상 지원 \r\n"
		"    pass p0 \r\n"
		"    { \r\n"
		"        VertexShader = compile vs_1_1 VS(); \r\n"
		"        PixelShader = compile ps_1_4 PS_ONLY_LIGHTMAP(); \r\n"
		"    } \r\n"
		" \r\n"
		"    //------------------------------------------------------------------------------------- \r\n"
		"    // Only LightMap (Edit시에만 사용)	- VS_1_1 이상 지원 \r\n"
		"    //									- PS_1_4 이상 지원 \r\n"
		"    pass p1 \r\n"
		"    { \r\n"
		"        VertexShader = compile vs_1_1 VS_XY(); \r\n"
		"        PixelShader = compile ps_1_4 PS_ONLY_LIGHTMAP(); \r\n"
		"    } \r\n"
		" \r\n"
		"    //------------------------------------------------------------------------------------- \r\n"
		"    // Only LightMap (Edit시에만 사용)	- VS_1_1 이상 지원 \r\n"
		"    //									- PS_1_4 이상 지원 \r\n"
		"    pass p2 \r\n"
		"    { \r\n"
		"        VertexShader = compile vs_1_1 VS_XZ(); \r\n"
		"        PixelShader = compile ps_1_4 PS_ONLY_LIGHTMAP(); \r\n"
		"    } \r\n"
		" \r\n"
		"    //------------------------------------------------------------------------------------- \r\n"
		"    // Only LightMap (Edit시에만 사용)	- VS_1_1 이상 지원 \r\n"
		"    //									- PS_1_4 이상 지원 \r\n"
		"    pass p3 \r\n"
		"    { \r\n"
		"        VertexShader = compile vs_1_1 VS_YZ(); \r\n"
		"        PixelShader = compile ps_1_4 PS_ONLY_LIGHTMAP(); \r\n"
		"    } \r\n"
		"} \r\n";
}

BOOL LightMapFX_VERTEX::Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, DWORD dwMaterials )
{
	if ( !m_pFX )
		return FALSE;

	switch ( NSLightMapFX::g_emRenderType )
	{
	case NSLIGHTMAPDEF::LMRT_CHECKER:
		break;
	default:
		return FALSE;
	};

	pd3dDevice->SetFVF( 0 );
	pd3dDevice->SetVertexDeclaration( m_pDCRT ); 

	DWORD dwMinFilter_0(0L);
	DWORD dwMinFilter_1(0L);
	pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &dwMinFilter_0 );
	pd3dDevice->GetSamplerState( 1, D3DSAMP_MINFILTER, &dwMinFilter_1 );

	switch ( NSLightMapFX::g_emRenderType )
	{
	case NSLIGHTMAPDEF::LMRT_CHECKER:
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		break;
	};

	UINT nIndex(0);
	UINT numPasses;
	m_pFX->Begin( &numPasses, 0 );
	if( nIndex < numPasses )
	{
		m_pFX->BeginPass( nIndex );
		{
			for( DWORD i=0; i<dwMaterials; i++ )
			{
				pMesh->DrawSubset( i );
			}			
		}
		m_pFX->EndPass();
	}
	m_pFX->End();

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter_0 );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, dwMinFilter_1 );

	return TRUE;
}

BOOL LightMapFX_VERTEX::RenderInexact( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, DWORD dwMaterials, UINT nIndex )
{
	if ( !m_pFX )
		return FALSE;

	switch ( NSLightMapFX::g_emRenderType )
	{
	case NSLIGHTMAPDEF::LMRT_CHECKER:
		break;
	default:
		return FALSE;
	};

	pd3dDevice->SetFVF( 0 );
	pd3dDevice->SetVertexDeclaration( m_pDCRT ); 

	DWORD dwMinFilter_0(0L);
	DWORD dwMinFilter_1(0L);
	pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &dwMinFilter_0 );
	pd3dDevice->GetSamplerState( 1, D3DSAMP_MINFILTER, &dwMinFilter_1 );

	switch ( NSLightMapFX::g_emRenderType )
	{
	case NSLIGHTMAPDEF::LMRT_CHECKER:
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		break;
	};

	// 수치를 더해놓아야 한다.
	nIndex += 1;

	UINT numPasses;
	m_pFX->Begin( &numPasses, 0 );
	if( nIndex < numPasses )
	{
		m_pFX->BeginPass( nIndex );
		{
			for( DWORD i=0; i<dwMaterials; i++ )
			{
				pMesh->DrawSubset( i );
			}			
		}
		m_pFX->EndPass();
	}
	m_pFX->End();

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter_0 );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, dwMinFilter_1 );

	return TRUE;
}

BOOL LightMapFX_VERTEX::Render( LPDIRECT3DDEVICEQ pd3dDevice, 
								LPDIRECT3DVERTEXBUFFERQ	pVB,
								LPDIRECT3DINDEXBUFFERQ pIB,
								int nFVFSize,
								DWORD dwFVF,
								DWORD dwVertNum, 
								DWORD dwFaceNum )
{
	if ( !m_pFX )
		return FALSE;

	switch ( NSLightMapFX::g_emRenderType )
	{
	case NSLIGHTMAPDEF::LMRT_CHECKER:
		break;
	default:
		return FALSE;
	};

	pd3dDevice->SetFVF( 0 );
	pd3dDevice->SetVertexDeclaration( m_pDCRT );

	DWORD dwMinFilter_0(0L);
	DWORD dwMinFilter_1(0L);
	pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &dwMinFilter_0 );
	pd3dDevice->GetSamplerState( 1, D3DSAMP_MINFILTER, &dwMinFilter_1 );

	switch ( NSLightMapFX::g_emRenderType )
	{
	case NSLIGHTMAPDEF::LMRT_CHECKER:
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		break;
	};

	UINT numPasses;
	m_pFX->Begin( &numPasses, 0 );
	if( 0 < numPasses )
	{
		m_pFX->BeginPass( 0 );
		{
			pd3dDevice->SetFVF( 0L );
			pd3dDevice->SetStreamSource( 0, pVB, 0, nFVFSize );
			pd3dDevice->SetIndices( pIB );
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, dwVertNum, 0, dwFaceNum );
		}
		m_pFX->EndPass();
	}
	m_pFX->End();

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter_0 );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, dwMinFilter_1 );

	return TRUE;
}

void LightMapFX_VERTEX::SetWorld( const D3DXMATRIX& matWorld, float fAlpha )
{
	if ( !m_pFX )
		return;

	LightMapFX_Base::SetWorld( matWorld, fAlpha );
}