#include "pch.h"

#include "../../../VisualMaterialLib/Manager/DxVMManager.h"
#include "../../../VisualMaterialLib/Util/DxTangentMesh.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxFogMan.h"
#include "../../G-Logic/GLPeriod.h"
#include "./NSCOMBINETEXTOOLS.h"
#include "./NSLightMapFX.h"

#include "LightMapFX_Test.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

LightMapFX_Test::LightMapFX_Test()
{
}

LightMapFX_Test::~LightMapFX_Test()
{
}

HRESULT LightMapFX_Test::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	if( m_pFX )	
		V_RETURN( m_pFX->OnResetDevice() );

	return hr;
}

HRESULT LightMapFX_Test::OnLostDevice()
{
	HRESULT hr(S_OK);

	if( m_pFX )	
		V_RETURN( m_pFX->OnLostDevice() );

	return hr;
}

void LightMapFX_Test::SetFX( TSTRING& strFX )
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
		"    float4 vPosition       : POSITION; \r\n"
		"    float4 NormalTangent   : TEXCOORD0;	// NormalTangent.xyz - Normal.xyz  // NormalTangent.w  - Tangent.x \r\n"
		"    float2 TangentYZ       : TEXCOORD1;    // NormalTangent.xy  - Tangen.yz \r\n"
		"    float2 TexCoord0       : TEXCOORD2;    // UV \r\n"
		"    float2 TexCoord1       : TEXCOORD3;    // UV \r\n"
		"}; \r\n"
		" \r\n"
		"struct VS_COLOR_INPUT { \r\n"
		"    float4 vPosition       : POSITION; \r\n"
		"    float4 vColor          : COLOR0; \r\n"
		"    float4 NormalTangent   : TEXCOORD0;	// NormalTangent.xyz - Normal.xyz  // NormalTangent.w  - Tangent.x \r\n"
		"    float2 TangentYZ       : TEXCOORD1;    // NormalTangent.xy  - Tangen.yz \r\n"
		"    float2 TexCoord0       : TEXCOORD2;    // UV \r\n"
		"    float2 TexCoord1       : TEXCOORD3;    // UV \r\n"
		"}; \r\n"
		" \r\n"
		"struct VS_OUTPUT  \r\n"
		"{ \r\n"
		"    float4 m_vPosition  : POSITION; \r\n"
		"    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
		"    float  m_fFog       : FOG; \r\n"
		"}; \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//						V	S \r\n"
		"VS_OUTPUT VS( VS_INPUT In )  \r\n"
		"{ \r\n"
		"    VS_OUTPUT Out; \r\n"
		"     \r\n"
		"    // Now that we have the calculated weight, add in the final influence \r\n"
		"    Out.m_vPosition = mul(g_matWVP, In.vPosition); \r\n"
		" \r\n"
		"    // Pass the texture coordinate \r\n"
		"    Out.m_vTexCoord0.xy = In.TexCoord1/32767.f; \r\n"
		"    Out.m_vTexCoord0.xy += g_vLightMapUV_Offset; \r\n"
		"    Out.m_vTexCoord0.xy *= g_vLightMapUV_Multiply; \r\n"
		" \r\n"
		"    Out.m_fFog = saturate((g_vFOG.x - Out.m_vPosition.z) / g_vFOG.y); \r\n"
		"    \r\n"
		"    return Out; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//						V	S \r\n"
		"VS_OUTPUT VS_COLOR( VS_COLOR_INPUT In )  \r\n"
		"{ \r\n"
		"    VS_OUTPUT Out; \r\n"
		"     \r\n"
		"    // Now that we have the calculated weight, add in the final influence \r\n"
		"    Out.m_vPosition = mul(g_matWVP, In.vPosition); \r\n"
		" \r\n"
		"    // Pass the texture coordinate \r\n"
		"    Out.m_vTexCoord0.xy = In.TexCoord1/32767.f; \r\n"
		"    Out.m_vTexCoord0.xy += g_vLightMapUV_Offset; \r\n"
		"    Out.m_vTexCoord0.xy *= g_vLightMapUV_Multiply; \r\n"
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
		"        VertexShader = compile vs_1_1 VS_COLOR(); \r\n"
		"        PixelShader = compile ps_1_4 PS_ONLY_LIGHTMAP(); \r\n"
		"    } \r\n"
		"} \r\n";
}

BOOL LightMapFX_Test::Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bVertexColor, DxTangentMesh* pTangentMesh, const DWORD dwAttrib )
{
	if ( !pTangentMesh )
		return FALSE;
	if ( !pTangentMesh->GetAttrib() )
		return FALSE;

	BOOL bActiveFX(FALSE);
	switch ( NSLightMapFX::g_emRenderType )
	{
	case NSLIGHTMAPDEF::LMRT_LIGHTMAP:
	case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
	case NSLIGHTMAPDEF::LMRT_CHECKER:
		break;
	default:
		return FALSE;
	};

	UINT nIndex(0);
	if ( bVertexColor )
	{
		nIndex = 1;
	}
	else
	{
		nIndex = 0;
	}

	DWORD dwMinFilter_0(0L);
	DWORD dwMinFilter_1(0L);
	pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &dwMinFilter_0 );
	pd3dDevice->GetSamplerState( 1, D3DSAMP_MINFILTER, &dwMinFilter_1 );

	switch ( NSLightMapFX::g_emRenderType )
	{
	case NSLIGHTMAPDEF::LMRT_CHECKER:
	case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		break;
	};

	UINT numPasses;
	m_pFX->Begin( &numPasses, 0 );
	if( nIndex < numPasses )
	{
		m_pFX->BeginPass( nIndex );
		{
			D3DXATTRIBUTERANGE& sAttrib = pTangentMesh->GetAttrib()[dwAttrib];
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 
												0,
												sAttrib.VertexStart,
												sAttrib.VertexCount,
												sAttrib.FaceStart*3,
												sAttrib.FaceCount );
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