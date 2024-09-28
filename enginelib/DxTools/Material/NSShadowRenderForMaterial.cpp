#include "pch.h"

#include "../Light/DxLightMan.h"
#include "../RENDERPARAM.h"
#include "../DxViewPort.h"

#include "./NSMaterialManager.h"	// 지금한개 사용중이나 지울수 있으면 지우자.

#include "NSShadowRenderForMaterial.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSShadowRenderForMaterial
{
	ID3DXEffect*					g_rFX(NULL);			// 참조만 한다.
	ID3DXEffect*					g_pFX_R32F(NULL);
	ID3DXEffect*					g_pFX_G32R32F(NULL);
	D3DXMATRIX						g_matVP;
	LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_ONLYPOS(NULL);
	LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_POSTEX(NULL);
	LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_ONLYPOS_INSTANCING(NULL);
	LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_POSTEX_INSTANCING(NULL);

	void CleanUp()
	{
		SAFE_RELEASE( g_rFX );
		SAFE_RELEASE( g_pFX_G32R32F );
	}

	HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		HRESULT hr(S_OK);

		// 
		{
			// 12 byte
			D3DVERTEXELEMENT9 VertexDecl[] =
			{        
				{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				D3DDECL_END()
			};
			pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT_ONLYPOS );
		}

		// 
		{
			// 12 + 52 byte
			D3DVERTEXELEMENT9 VertexDecl[] =
			{        
				{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				{ 1, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				{ 1, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
				{ 1, 24,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
				{ 1, 36,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
				D3DDECL_END()
			};
			pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT_ONLYPOS_INSTANCING );
		}

		// 
		{
			// 20 byte
			D3DVERTEXELEMENT9 VertexDecl[] =
			{        
				{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				D3DDECL_END()
			};
			pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT_POSTEX );
		}

		// 
		{
			// 20 + 52 byte
			D3DVERTEXELEMENT9 VertexDecl[] =
			{        
				{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				{ 1, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
				{ 1, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
				{ 1, 24,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
				{ 1, 36,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
				D3DDECL_END()
			};
			pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT_POSTEX_INSTANCING );
		}

		if( g_pFX_R32F )	
			V_RETURN( g_pFX_R32F->OnResetDevice() );
		if( g_pFX_G32R32F )	
			V_RETURN( g_pFX_G32R32F->OnResetDevice() );

		return hr;
	}

	HRESULT OnLostDevice()
	{
		HRESULT hr(S_OK);

		SAFE_RELEASE( g_pDCRT_ONLYPOS );
		SAFE_RELEASE( g_pDCRT_ONLYPOS_INSTANCING );
		SAFE_RELEASE( g_pDCRT_POSTEX );
		SAFE_RELEASE( g_pDCRT_POSTEX_INSTANCING );

		if( g_pFX_R32F )	
			V_RETURN( g_pFX_R32F->OnLostDevice() );
		if( g_pFX_G32R32F )	
			V_RETURN( g_pFX_G32R32F->OnLostDevice() );

		return hr;
	}

	void SetFX_R32F( LPDIRECT3DDEVICEQ pd3dDevice )
    {
        TSTRING strFX = 
        "//-------------------------------------------------------------------------------------- \r\n"
        "// File: FrameMesh.fx \r\n"
        "//       shhan \r\n"
        "//-------------------------------------------------------------------------------------- \r\n"
        " \r\n"
        "//-------------------------------------------------------------------------------------- \r\n"
        "// Global variables \r\n"
        "//-------------------------------------------------------------------------------------- \r\n"
		"float4x4 g_matWorld; \r\n"
        "float4x4 g_matWVP; \r\n"
		"float4x4 g_matVP; \r\n"
		" \r\n"
		"texture g_BaseTexture; \r\n"
		"sampler BaseTexSampler = sampler_state  \r\n"
		"{ \r\n"
		"	Texture = (g_BaseTexture); \r\n"
		"}; \r\n"
		" \r\n"
        "//-------------------------------------------------------------------------------------- \r\n"
        "// Basic vertex transformation \r\n"
        "//-------------------------------------------------------------------------------------- \r\n"
        "struct VS_INPUT_ONLYPOS { \r\n"
        "   float4 m_vPosition		: POSITION; \r\n"
        "}; \r\n"
        " \r\n"
		"struct VS_INPUT_ONLYPOS_INSTANCING { \r\n"
		"   float4 m_vPosition		: POSITION; \r\n"
		"   float3 m_vMatrix1		: TEXCOORD0; \r\n"
		"   float3 m_vMatrix2		: TEXCOORD1; \r\n"
		"   float3 m_vMatrix3		: TEXCOORD2; \r\n"
		"   float4 m_vMatrix4		: TEXCOORD3; \r\n"
		"}; \r\n"
		" \r\n"
		"struct VS_OUTPUT_ONLYPOS  \r\n"
		"{ \r\n"
		"   float4 m_vPosition  : POSITION; \r\n"
		"	float2 m_vDepth     : TEXCOORD0;\r\n"
		"}; \r\n"
		" \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		"// Basic vertex transformation \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		"struct VS_INPUT { \r\n"
		"   float4 m_vPosition  : POSITION; \r\n"
		"	float2 m_vTexCoord0 : TEXCOORD0; \r\n"
		"}; \r\n"
		" \r\n"
		"struct VS_INPUT_POSTEX_INSTANCING { \r\n"
		"   float4 m_vPosition  : POSITION; \r\n"
		"	float2 m_vTexCoord0 : TEXCOORD0; \r\n"
		"   float3 m_vMatrix1	: TEXCOORD1; \r\n"
		"   float3 m_vMatrix2	: TEXCOORD2; \r\n"
		"   float3 m_vMatrix3	: TEXCOORD3; \r\n"
		"   float4 m_vMatrix4	: TEXCOORD4; \r\n"
		"}; \r\n"
		" \r\n"
        "struct VS_OUTPUT  \r\n"
        "{ \r\n"
        "   float4 m_vPosition  : POSITION; \r\n"
		"	float2 m_vTexCoord0 : TEXCOORD0; \r\n"
		"	float2 m_vDepth     : TEXCOORD1;\r\n"
        "}; \r\n"
        " \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//						V	S \r\n"
		"VS_OUTPUT_ONLYPOS VS_ONLYPOS( VS_INPUT_ONLYPOS In )  \r\n"
		"{ \r\n"
		"   VS_OUTPUT_ONLYPOS Out; \r\n"
		" \r\n"
		"   // Now that we have the calculated weight, add in the final influence \r\n"
		"   Out.m_vPosition = mul(In.m_vPosition,g_matWVP); \r\n"
		" \r\n"
		"	// copy the input texture coordinate through \r\n"
		"	Out.m_vDepth.xy = Out.m_vPosition.zw; \r\n"
		" \r\n"
		"    return Out; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//					P	S \r\n"
		"// 원본 \r\n"
		"float4 PS_ONLYPOS( VS_OUTPUT_ONLYPOS In ) : COLOR0  \r\n"
		"{ \r\n"
		"   // Texture \r\n"
		"	float4 vColor = In.m_vDepth.x / In.m_vDepth.y;\r\n"
		" \r\n"
		"   return vColor; \r\n"
		"} \r\n"
		" \r\n"
        "//---------------------------------------------------------------------------------------------------------- \r\n"
        "//						V	S \r\n"
        "VS_OUTPUT VS( VS_INPUT In )  \r\n"
        "{ \r\n"
        "   VS_OUTPUT Out; \r\n"
        "    \r\n"
        "   // Now that we have the calculated weight, add in the final influence \r\n"
        "   Out.m_vPosition = mul(In.m_vPosition,g_matWVP); \r\n"
		" \r\n"
		"	// Pass the texture coordinate \r\n"
		"	Out.m_vTexCoord0 = In.m_vTexCoord0; \r\n"
		" \r\n"
		"	// copy the input texture coordinate through \r\n"
		"	Out.m_vDepth.xy = Out.m_vPosition.zw; \r\n"
        " \r\n"
        "    return Out; \r\n"
        "} \r\n"
        " \r\n"
        "//---------------------------------------------------------------------------------------------------------- \r\n"
        "//					P	S \r\n"
        "// 원본 \r\n"
        "float4 PS( VS_OUTPUT In ) : COLOR0  \r\n"
        "{ \r\n"
        "   // Texture \r\n"
        "	float4 vColor = In.m_vDepth.x / In.m_vDepth.y;\r\n"
		" \r\n"
		"	// Texture - Alpha Texture 일 경우만 쓰이도록 해야한다. \r\n"
		"	vColor.w = tex2D( BaseTexSampler, In.m_vTexCoord0 ).w; \r\n"
		" \r\n"
        "   return vColor; \r\n"
        "} \r\n"
        " \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//						V	S \r\n"
		"VS_OUTPUT_ONLYPOS VS_ONLYPOS_INSTANCING( VS_INPUT_ONLYPOS_INSTANCING In )  \r\n"
		"{ \r\n"
		"   VS_OUTPUT_ONLYPOS Out; \r\n"
		" \r\n"
		"	g_matWorld[0].xyz = In.m_vMatrix1; \r\n"
		"	g_matWorld[1].xyz = In.m_vMatrix2; \r\n"
		"	g_matWorld[2].xyz = In.m_vMatrix3; \r\n"
		"	g_matWorld[3].xyzw = In.m_vMatrix4; \r\n"
		" \r\n"
		"   g_matWVP = mul(g_matWorld,g_matVP); \r\n"
		" \r\n"
		"   // Now that we have the calculated weight, add in the final influence \r\n"
		"   Out.m_vPosition = mul(In.m_vPosition,g_matWVP); \r\n"
		" \r\n"
		"	// copy the input texture coordinate through \r\n"
		"	Out.m_vDepth.xy = Out.m_vPosition.zw; \r\n"
		" \r\n"
		"    return Out; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//					P	S \r\n"
		"// 원본 \r\n"
		"float4 PS_ONLYPOS_INSTANCING( VS_OUTPUT_ONLYPOS In ) : COLOR0  \r\n"
		"{ \r\n"
		"   // Texture \r\n"
		"	float4 vColor = In.m_vDepth.x / In.m_vDepth.y;\r\n"
		" \r\n"
		"   return vColor; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//						V	S \r\n"
		"VS_OUTPUT VS_POSTEX_INSTANCING( VS_INPUT_POSTEX_INSTANCING In )  \r\n"
		"{ \r\n"
		"   VS_OUTPUT Out; \r\n"
		" \r\n"
		"	g_matWorld[0].xyz = In.m_vMatrix1; \r\n"
		"	g_matWorld[1].xyz = In.m_vMatrix2; \r\n"
		"	g_matWorld[2].xyz = In.m_vMatrix3; \r\n"
		"	g_matWorld[3].xyzw = In.m_vMatrix4; \r\n"
		" \r\n"
		"   g_matWVP = mul(g_matWorld,g_matVP); \r\n"
		" \r\n"
		"   // Now that we have the calculated weight, add in the final influence \r\n"
		"   Out.m_vPosition = mul(In.m_vPosition,g_matWVP); \r\n"
		" \r\n"
		"	// Pass the texture coordinate \r\n"
		"	Out.m_vTexCoord0 = In.m_vTexCoord0; \r\n"
		" \r\n"
		"	// copy the input texture coordinate through \r\n"
		"	Out.m_vDepth.xy = Out.m_vPosition.zw; \r\n"
		" \r\n"
		"    return Out; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//					P	S \r\n"
		"// 원본 \r\n"
		"float4 PS_POSTEX_INSTANCING( VS_OUTPUT In ) : COLOR0  \r\n"
		"{ \r\n"
		"   // Texture \r\n"
		"	float4 vColor = In.m_vDepth.x / In.m_vDepth.y;\r\n"
		" \r\n"
		"	// Texture - Alpha Texture 일 경우만 쓰이도록 해야한다. \r\n"
		"	vColor.w = tex2D( BaseTexSampler, In.m_vTexCoord0 ).w; \r\n"
		" \r\n"
		"   return vColor; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//										technique \r\n"
        "technique Base \r\n"
        "{ \r\n"
        "   //------------------------------------------------------------------------------------- \r\n"
        "   // 기본 _ 0 - VS_1_1 이상 지원 \r\n"
        "   //          - PS_2_0 이상 지원 \r\n"
        "   pass p0_ONLYPOS \r\n"
        "   { \r\n"
        "       VertexShader = compile vs_1_1 VS_ONLYPOS(); \r\n"
        "		PixelShader = compile ps_2_0 PS_ONLYPOS(); \r\n"
        "   } \r\n"
		"   //------------------------------------------------------------------------------------- \r\n"
		"   // 기본 _ 0 - VS_1_1 이상 지원 \r\n"
		"   //          - PS_2_0 이상 지원 \r\n"
		"   pass p0 \r\n"
		"   { \r\n"
		"       VertexShader = compile vs_1_1 VS(); \r\n"
		"		PixelShader = compile ps_2_0 PS(); \r\n"
		"   } \r\n"
		"   //------------------------------------------------------------------------------------- \r\n"
		"   // 기본 _ 0 - VS_2_0 이상 지원 \r\n"
		"   //          - PS_2_0 이상 지원 \r\n"
		"   pass p0_ONLYPOS_INSTANCING \r\n"
		"   { \r\n"
		"       VertexShader = compile vs_2_0 VS_ONLYPOS_INSTANCING(); \r\n"
		"		PixelShader = compile ps_2_0 PS_ONLYPOS_INSTANCING(); \r\n"
		"   } \r\n"
		"   //------------------------------------------------------------------------------------- \r\n"
		"   // 기본 _ 0 - VS_2_0 이상 지원 \r\n"
		"   //          - PS_2_0 이상 지원 \r\n"
		"   pass p0_POSTEX_INSTANCING \r\n"
		"   { \r\n"
		"       VertexShader = compile vs_2_0 VS_POSTEX_INSTANCING(); \r\n"
		"		PixelShader = compile ps_2_0 PS_POSTEX_INSTANCING(); \r\n"
		"   } \r\n"
        "} \r\n";

        UINT dwBufferSize = static_cast<UINT>( strFX.size() ) + 1; 
        D3DXCreateEffect( pd3dDevice, strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &g_pFX_R32F, NULL );
    }

	void SetFX_G32R32F( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		TSTRING strFX = 
			"//-------------------------------------------------------------------------------------- \r\n"
			"// File: FrameMesh.fx \r\n"
			"//       shhan \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"
			" \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"
			"// Global variables \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"
			"float4x4	g_matWorld; \r\n"
			"float4x4	g_matWVP; \r\n"
			"float4x4	g_matVP; \r\n"
			"float3		g_vLightPosWorldSpace; \r\n"
			" \r\n"
			"texture g_BaseTexture; \r\n"
			"sampler BaseTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_BaseTexture); \r\n"
			"}; \r\n"
			" \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"
			"// Basic vertex transformation \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"
			"struct VS_INPUT_ONLYPOS { \r\n"
			"   float4 m_vPosition		: POSITION; \r\n"
			"}; \r\n"
			" \r\n"
			"struct VS_INPUT_ONLYPOS_INSTANCING { \r\n"
			"   float4 m_vPosition		: POSITION; \r\n"
			"   float3 m_vMatrix1		: TEXCOORD0; \r\n"
			"   float3 m_vMatrix2		: TEXCOORD1; \r\n"
			"   float3 m_vMatrix3		: TEXCOORD2; \r\n"
			"   float4 m_vMatrix4		: TEXCOORD3; \r\n"
			"}; \r\n"
			" \r\n"
			"struct VS_OUTPUT_ONLYPOS  \r\n"
			"{ \r\n"
			"   float4 m_vPosition  : POSITION; \r\n"
			"	float m_fDepth     : TEXCOORD0;\r\n"
			"}; \r\n"
			" \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"
			"// Basic vertex transformation \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"
			"struct VS_INPUT { \r\n"
			"   float4 m_vPosition  : POSITION; \r\n"
			"	float2 m_vTexCoord0 : TEXCOORD0; \r\n"
			"}; \r\n"
			" \r\n"
			"struct VS_INPUT_POSTEX_INSTANCING { \r\n"
			"   float4 m_vPosition  : POSITION; \r\n"
			"	float2 m_vTexCoord0 : TEXCOORD0; \r\n"
			"   float3 m_vMatrix1	: TEXCOORD1; \r\n"
			"   float3 m_vMatrix2	: TEXCOORD2; \r\n"
			"   float3 m_vMatrix3	: TEXCOORD3; \r\n"
			"   float4 m_vMatrix4	: TEXCOORD4; \r\n"
			"}; \r\n"
			" \r\n"
			"struct VS_OUTPUT  \r\n"
			"{ \r\n"
			"   float4 m_vPosition  : POSITION; \r\n"
			"	float2 m_vTexCoord0 : TEXCOORD0; \r\n"
			"	float  m_fDepth     : TEXCOORD1;\r\n"
			"}; \r\n"
			" \r\n"

			"float2 ComputeMoments(float depth) \r\n"
			"{ \r\n"
			"	float2 moments; \r\n"

				// First moment is the depth itself
			"	moments.x = depth; \r\n"

			//	// Compute partial derivatives of depth
			//"	float dx = ddx(depth); \r\n"
			//"	float dy = ddy(depth); \r\n"

			//"	moments.y = depth*depth + 0.25*(dx*dx + dy*dy); \r\n"
			"	moments.y = depth*depth; \r\n"

			"	return moments; \r\n"
			"} \r\n"

			"//---------------------------------------------------------------------------------------------------------- \r\n"
			"//						V	S \r\n"
			"VS_OUTPUT_ONLYPOS VS_ONLYPOS( VS_INPUT_ONLYPOS In )  \r\n"
			"{ \r\n"
			"   VS_OUTPUT_ONLYPOS Out; \r\n"
			" \r\n"
			"   // Now that we have the calculated weight, add in the final influence \r\n"
			"   float3 vPos = mul(In.m_vPosition,g_matWorld); \r\n"
			"   Out.m_vPosition = mul(In.m_vPosition,g_matWVP); \r\n"
			" \r\n"
			"	Out.m_fDepth = length( g_vLightPosWorldSpace - vPos.xyz ); \r\n"
			" \r\n"
			"    return Out; \r\n"
			"} \r\n"
			" \r\n"
			"//---------------------------------------------------------------------------------------------------------- \r\n"
			"//					P	S \r\n"
			"// 원본 \r\n"
			"float4 PS_ONLYPOS( VS_OUTPUT_ONLYPOS In ) : COLOR0  \r\n"
			"{ \r\n"
			"	return float4( ComputeMoments(In.m_fDepth), 0.f, 1.f );\r\n"
			//"	return float4( ComputeMoments(0.f), 0.f, 1.f );\r\n"
			"} \r\n"
			" \r\n"
			"//---------------------------------------------------------------------------------------------------------- \r\n"
			"//						V	S \r\n"
			"VS_OUTPUT VS( VS_INPUT In )  \r\n"
			"{ \r\n"
			"   VS_OUTPUT Out; \r\n"
			"    \r\n"
			"   // Now that we have the calculated weight, add in the final influence \r\n"
			"   float3 vPos = mul(In.m_vPosition,g_matWorld); \r\n"
			"   Out.m_vPosition = mul(In.m_vPosition,g_matWVP); \r\n"
			" \r\n"
			"	// Pass the texture coordinate \r\n"
			"	Out.m_vTexCoord0 = In.m_vTexCoord0; \r\n"
			" \r\n"
			"	Out.m_fDepth = length( g_vLightPosWorldSpace - vPos.xyz ); \r\n"
			" \r\n"
			"    return Out; \r\n"
			"} \r\n"
			" \r\n"
			"//---------------------------------------------------------------------------------------------------------- \r\n"
			"//					P	S \r\n"
			"// 원본 \r\n"
			"float4 PS( VS_OUTPUT In ) : COLOR0  \r\n"
			"{ \r\n"
			"	// Texture - Alpha Texture 일 경우만 쓰이도록 해야한다. \r\n"
			"	return float4( ComputeMoments(In.m_fDepth), 0.f, tex2D( BaseTexSampler, In.m_vTexCoord0 ).w );\r\n"
			//"	return float4( ComputeMoments(0.f), 0.f, tex2D( BaseTexSampler, In.m_vTexCoord0 ).w );\r\n"
			" \r\n"
			"} \r\n"
			" \r\n"
			"//---------------------------------------------------------------------------------------------------------- \r\n"
			"//						V	S \r\n"
			"VS_OUTPUT_ONLYPOS VS_ONLYPOS_INSTANCING( VS_INPUT_ONLYPOS_INSTANCING In )  \r\n"
			"{ \r\n"
			"   VS_OUTPUT_ONLYPOS Out; \r\n"
			" \r\n"
			"	g_matWorld[0].xyz = In.m_vMatrix1; \r\n"
			"	g_matWorld[1].xyz = In.m_vMatrix2; \r\n"
			"	g_matWorld[2].xyz = In.m_vMatrix3; \r\n"
			"	g_matWorld[3].xyzw = In.m_vMatrix4; \r\n"
			" \r\n"
			"   g_matWVP = mul(g_matWorld,g_matVP); \r\n"
			" \r\n"
			"   // Now that we have the calculated weight, add in the final influence \r\n"
			"   float3 vPos = mul(In.m_vPosition,g_matWorld); \r\n"
			"   Out.m_vPosition = mul(In.m_vPosition,g_matWVP); \r\n"
			" \r\n"
			"	Out.m_fDepth = length( g_vLightPosWorldSpace - vPos.xyz ); \r\n"
			" \r\n"
			"    return Out; \r\n"
			"} \r\n"
			" \r\n"
			"//---------------------------------------------------------------------------------------------------------- \r\n"
			"//					P	S \r\n"
			"// 원본 \r\n"
			"float4 PS_ONLYPOS_INSTANCING( VS_OUTPUT_ONLYPOS In ) : COLOR0  \r\n"
			"{ \r\n"
			"	return float4( ComputeMoments(In.m_fDepth), 0.f, 1.f );\r\n"
			"} \r\n"
			" \r\n"
			"//---------------------------------------------------------------------------------------------------------- \r\n"
			"//						V	S \r\n"
			"VS_OUTPUT VS_POSTEX_INSTANCING( VS_INPUT_POSTEX_INSTANCING In )  \r\n"
			"{ \r\n"
			"   VS_OUTPUT Out; \r\n"
			" \r\n"
			"	g_matWorld[0].xyz = In.m_vMatrix1; \r\n"
			"	g_matWorld[1].xyz = In.m_vMatrix2; \r\n"
			"	g_matWorld[2].xyz = In.m_vMatrix3; \r\n"
			"	g_matWorld[3].xyzw = In.m_vMatrix4; \r\n"
			" \r\n"
			"   g_matWVP = mul(g_matWorld,g_matVP); \r\n"
			" \r\n"
			"   // Now that we have the calculated weight, add in the final influence \r\n"
			"   float3 vPos = mul(In.m_vPosition,g_matWorld); \r\n"
			"   Out.m_vPosition = mul(In.m_vPosition,g_matWVP); \r\n"
			" \r\n"
			"	// Pass the texture coordinate \r\n"
			"	Out.m_vTexCoord0 = In.m_vTexCoord0; \r\n"
			" \r\n"
			"	// copy the input texture coordinate through \r\n"
			"	Out.m_fDepth = length( g_vLightPosWorldSpace - vPos.xyz ); \r\n"
			" \r\n"
			"    return Out; \r\n"
			"} \r\n"
			" \r\n"
			"//---------------------------------------------------------------------------------------------------------- \r\n"
			"//					P	S \r\n"
			"// 원본 \r\n"
			"float4 PS_POSTEX_INSTANCING( VS_OUTPUT In ) : COLOR0  \r\n"
			"{ \r\n"
			"	// Texture - Alpha Texture 일 경우만 쓰이도록 해야한다. \r\n"
			"	return float4( ComputeMoments(In.m_fDepth), 0.f, tex2D( BaseTexSampler, In.m_vTexCoord0 ).w );\r\n"
			"} \r\n"
			" \r\n"
			"//---------------------------------------------------------------------------------------------------------- \r\n"
			"//										technique \r\n"
			"technique Base \r\n"
			"{ \r\n"
			"   //------------------------------------------------------------------------------------- \r\n"
			"   // 기본 _ 0 - VS_1_1 이상 지원 \r\n"
			"   //          - PS_2_0 이상 지원 \r\n"
			"   pass p0_ONLYPOS \r\n"
			"   { \r\n"
			"       VertexShader = compile vs_3_0 VS_ONLYPOS(); \r\n"
			"		PixelShader = compile ps_3_0 PS_ONLYPOS(); \r\n"
			"   } \r\n"
			"   //------------------------------------------------------------------------------------- \r\n"
			"   // 기본 _ 0 - VS_1_1 이상 지원 \r\n"
			"   //          - PS_2_0 이상 지원 \r\n"
			"   pass p0 \r\n"
			"   { \r\n"
			"       VertexShader = compile vs_3_0 VS(); \r\n"
			"		PixelShader = compile ps_3_0 PS(); \r\n"
			"   } \r\n"
			"   //------------------------------------------------------------------------------------- \r\n"
			"   // 기본 _ 0 - VS_2_0 이상 지원 \r\n"
			"   //          - PS_2_0 이상 지원 \r\n"
			"   pass p0_ONLYPOS_INSTANCING \r\n"
			"   { \r\n"
			"       VertexShader = compile vs_3_0 VS_ONLYPOS_INSTANCING(); \r\n"
			"		PixelShader = compile ps_3_0 PS_ONLYPOS_INSTANCING(); \r\n"
			"   } \r\n"
			"   //------------------------------------------------------------------------------------- \r\n"
			"   // 기본 _ 0 - VS_2_0 이상 지원 \r\n"
			"   //          - PS_2_0 이상 지원 \r\n"
			"   pass p0_POSTEX_INSTANCING \r\n"
			"   { \r\n"
			"       VertexShader = compile vs_3_0 VS_POSTEX_INSTANCING(); \r\n"
			"		PixelShader = compile ps_3_0 PS_POSTEX_INSTANCING(); \r\n"
			"   } \r\n"
			"} \r\n";

		UINT dwBufferSize = static_cast<UINT>( strFX.size() ) + 1; 
		D3DXCreateEffect( pd3dDevice, strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &g_pFX_G32R32F, NULL );
	}

	HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		CleanUp();

		SetFX_G32R32F( pd3dDevice );
		SetFX_R32F( pd3dDevice );

		// 등록은 해 놓는다.
		g_rFX = g_pFX_R32F;

		return S_OK;
	}

	void SetWorld( const D3DXMATRIX& matWorld )
	{
		D3DXMATRIX matWVP;
		D3DXMatrixMultiply( &matWVP, &matWorld, &g_matVP );
		//g_rFX->SetMatrixTranspose( "g_matWVP",	&matWVP );
		g_rFX->SetMatrix( "g_matWVP",	&matWVP );
		g_rFX->SetMatrix( "g_matWorld",	&matWorld );
		g_rFX->SetMatrix( "g_matVP",	&g_matVP );
	}

	void SetBaseFX( const D3DXMATRIX& matVP )
	{
		g_matVP = matVP;

		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		g_rFX->SetMatrix( "g_matWorld",	&matIdentity );	// 기본 World 는 Identity
		g_rFX->SetMatrix( "g_matVP",	&g_matVP );
		g_rFX->SetMatrix( "g_matWVP",	&g_matVP );	// 기본 g_matWVP 의 World 는 Identity 라 이렇게 함.
													// Piece 의 경우는 SetWorld 이 호출된다.

		//DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectLight();
		//if ( !pLight )
		//	return;

		//// 위치값
		//D3DXVECTOR4 vPos4;
		//vPos4.x = DxViewPort::GetInstance().GetLookatPt().x - pLight->m_Light.Direction.x*1500.f;
		//vPos4.y = DxViewPort::GetInstance().GetLookatPt().y - pLight->m_Light.Direction.y*1500.f;
		//vPos4.z = DxViewPort::GetInstance().GetLookatPt().z - pLight->m_Light.Direction.z*1500.f;
		////vPos4.x = -pLight->m_Light.Direction.x*1500.f;
		////vPos4.y = -pLight->m_Light.Direction.y*1500.f;
		////vPos4.z = -pLight->m_Light.Direction.z*1500.f;
		//vPos4.w = 1.f;
		//g_rFX->SetVector( "g_vLightPosWorldSpace", &vPos4 );

		g_rFX->SetVector( "g_vLightPosWorldSpace", &NSMaterialManager::g_vLightPosWorldSpace );
	}

	void SetMode_R32F()
	{
		g_rFX = g_pFX_R32F;
	}

	void SetMode_G32R32F()
	{
		g_rFX = g_pFX_G32R32F;
	}

	void SetTexture( LPDIRECT3DTEXTUREQ pTexture )
	{
		g_rFX->SetTexture( "g_BaseTexture", pTexture );
	}

	void CommitChanges()
	{
		g_rFX->CommitChanges();
	}

	BOOL FX_Begin()
	{
		UINT numPasses;
		if ( g_rFX->Begin( &numPasses, 0 ) != D3D_OK )
			return FALSE;

		return TRUE;
	}

	BOOL FX_BeginPass( UINT unPass )
	{
		if ( unPass > 3 )
		{
			g_rFX->End();
			return FALSE;
		}

		if ( g_rFX->BeginPass( unPass ) != D3D_OK )
		{
			g_rFX->End();
			return FALSE;
		}

		return TRUE;
	}

	void FX_EndPass()
	{
		g_rFX->EndPass();
	}

	void FX_End()
	{
		g_rFX->End();
	}
}