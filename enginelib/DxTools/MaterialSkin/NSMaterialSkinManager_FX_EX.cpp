#include "pch.h"

#include "NSMaterialSkinManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSMaterialSkinManager
{
	//////////////////////////////////////////////////////////////////////////
	//
	void SetDefaultFX_Glow( std::string& strFX )
	{
		strFX = 
			"//-------------------------------------------------------------------------------------- \r\n"
			"//       shhan \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"

			"float g_fAlpha=1.f;\r\n"
			"int g_nCurNumBones = 2;\r\n"
			"float4 g_vlhtDirViewSpace = {0.0f, 0.0f, -1.0f, 1.0f};    //light Direction\r\n"
			"float4 g_vLightAmbient : MATERIALAMBIENT = {0.1f, 0.1f, 0.1f, 1.0f};\r\n"
			"float4 g_vLightDiffuse : MATERIALDIFFUSE = {0.8f, 0.8f, 0.8f, 1.0f};\r\n"
			"float4 g_vGlowColor; \r\n"

			// Matrix Pallette
			"static const int MAX_MATRICES = 60;\r\n"
			"float4x3   g_matWorld;\r\n"
			"float4x3   g_matView;\r\n"
			"float4x3   g_matWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\r\n"
			"float4x4   g_matViewProj : VIEWPROJECTION;\r\n"

			"texture g_BaseTexture; \r\n"
			"sampler BaseTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_BaseTexture); \r\n"
			"}; \r\n"


			///////////////////////////////////////////////////////
			"struct VS_INPUT\r\n"
			"{\r\n"
			"	float4  m_vPos				: POSITION;\r\n"
			"	float4  m_vBlendWeights		: BLENDWEIGHT;\r\n"
			"	float4  m_vBlendIndices		: BLENDINDICES;\r\n"
			"	float3  m_vNormal			: NORMAL;\r\n"
			"	float2  m_vTex				: TEXCOORD0;\r\n"
			"} Vertex : SEMANTIC_VS_INPUT; \r\n"
			" \r\n"

			"struct VS_INPUT_OBJECT \r\n"
			"{\r\n"
			"	float4  m_vPos				: POSITION;\r\n"
			"	float3  m_vNormal			: NORMAL;\r\n"
			"	float2  m_vTex				: TEXCOORD0;\r\n"
			"} VertexObject : SEMANTIC_VS_INPUT_OBJECT; \r\n"
			" \r\n"

			"struct VS_OUTPUT \r\n"
			"{\r\n"
			"	float4  m_vPos		: POSITION;\r\n"
			"	float2  m_vTex		: TEXCOORD0;\r\n"
			"};\r\n"



			//////////////////////////////////////////////////////////////////////////
			//							옵션 - 저
			//////////////////////////////////////////////////////////////////////////
			// VS Skin
			"VS_OUTPUT VShade(VS_INPUT In, uniform int nNumBones)\r\n"
			"{\r\n"
			"	VS_OUTPUT   Out;\r\n"
			"	float3      vPos = 0.0f;\r\n"
			"	float3      vNormal = 0.0f;    \r\n"
			"	float       LastWeight = 0.0f;\r\n"

			// Compensate for lack of UBYTE4 on Geforce3
			"	int4 IndexVector = D3DCOLORtoUBYTE4(In.m_vBlendIndices);\r\n"

			// cast the vectors to arrays for use in the for loop below
			"	float BlendWeightsArray[4] = (float[4])In.m_vBlendWeights;\r\n"
			"	int   IndexArray[4]        = (int[4])IndexVector;\r\n"

			// calculate the pos/normal using the "normal" weights 
			//        and accumulate the weights to calculate the last weight
			"	for (int iBone = 0; iBone < nNumBones-1; iBone++)\r\n"
			"	{\r\n"
			"		LastWeight = LastWeight + BlendWeightsArray[iBone];\r\n"

			"		vPos += mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
			"		vNormal += mul(In.m_vNormal, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
			"	}\r\n"
			"	LastWeight = 1.0f - LastWeight; \r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos += (mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[nNumBones-1]]) * LastWeight);\r\n"
			"	vNormal += (mul(In.m_vNormal, g_matWorldMatrixArray[IndexArray[nNumBones-1]]) * LastWeight); \r\n"
			"	vNormal = mul(vNormal, g_matView); \r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex.xy;\r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			// VS Object
			"VS_OUTPUT VS_OBJECT_1(VS_INPUT_OBJECT In)\r\n"
			"{\r\n"
			"	VS_OUTPUT   Out;\r\n"
			"	float3      vPos = 0.0f;\r\n"
			"	float3      vNormal = 0.0f;    \r\n"
			"	float       LastWeight = 0.0f;\r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos = mul(In.m_vPos, g_matWorld);\r\n"
			"	vNormal = mul(In.m_vNormal, g_matWorld); \r\n"
			"	vNormal = mul(vNormal, g_matView); \r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex.xy;\r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			//			PS_1
			"float4 PS_1( VS_OUTPUT In ) : COLOR0  \r\n"
			"{ \r\n"

			// Texture
			"	float4 vAlbedo = tex2D( BaseTexSampler, In.m_vTex ); \r\n"

			// Albedo 와 Alpha 를 곱한다.
			// Diffuse Color
			"	vAlbedo.xyz = g_vGlowColor.xyz * vAlbedo.xyz; \r\n"
			"	vAlbedo.w *= g_fAlpha; \r\n"

			"	return vAlbedo; \r\n"
			"} \r\n"

			//////////////////////////////////////////////////////////////////////////
			//			PS_1
			"float4 PS_GlowInLevel( VS_OUTPUT In ) : COLOR0  \r\n"
			"{ \r\n"

			// Texture
			"	float4 vMask2 = tex2D( BaseTexSampler, In.m_vTex ); \r\n"

			// Albedo 와 Alpha 를 곱한다.
			// Diffuse Color
			// vAlbedo.xyz 로 하지 않고 vAlbedo.z 이렇게 쓰는게 Level에서 Glow 를 쓸 경우, mask2.r 가 mask 로 쓰이기 때문이다.
			"	vMask2.xyz = g_vGlowColor.xyz * vMask2.r; \r\n"
			"	vMask2.w *= g_fAlpha; \r\n"

			"	return vMask2; \r\n"
			"} \r\n"


			//////////////////////////////////////////////////////////////////////////
			//	
			//////////////////////////////////////////////////////////////////////////
			"VertexShader vsArray[4] = { compile vs_2_0 VShade(1), \r\n"
										"compile vs_2_0 VShade(2),\r\n"
										"compile vs_2_0 VShade(3),\r\n"
										"compile vs_2_0 VShade(4) }; \r\n"


			//////////////////////////////////////
			// Techniques specs follow
			//////////////////////////////////////
			"technique runtime_Skin\r\n"
			"{\r\n"
			"	pass low \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = (vsArray[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_1();\r\n"
			"	} \r\n"
			"	pass GlowInLevel \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = (vsArray[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_GlowInLevel();\r\n"
			"	} \r\n"
			"}\r\n"
			"technique runtime_Object\r\n"
			"{\r\n"
			"	pass low \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT_1();\r\n"
			"		PixelShader = compile ps_2_0 PS_1();\r\n"
			"	} \r\n"
			"	pass GlowInLevel \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT_1();\r\n"
			"		PixelShader = compile ps_2_0 PS_GlowInLevel();\r\n"
			"	} \r\n"
			"}\r\n"
			"\r\n";
	}



	//////////////////////////////////////////////////////////////////////////
	//
	void SetDefaultFX_Shadow_R32F( std::string& strFX )
	{
		strFX = 
			"//-------------------------------------------------------------------------------------- \r\n"
			"//       shhan \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"

			"float		g_fAlpha=1.f;\r\n"
			"int		g_nCurNumBones = 2;\r\n"
			"float      g_fDepthOffset = 0.001f; \r\n"
			"float		g_fShadowColor = 0.25f; \r\n"

			// Matrix Pallette
			"static const int MAX_MATRICES = 60;\r\n"
			"float4x3   g_matWorld;\r\n"
			"float4x3   g_matWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\r\n"
			"float4x4   g_matViewProj : VIEWPROJECTION;\r\n"


			"texture g_BaseTexture; \r\n"
			"sampler BaseTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_BaseTexture); \r\n"
			"}; \r\n"


			///////////////////////////////////////////////////////
			"struct VS_INPUT\r\n"
			"{\r\n"
			"	float4  m_vPos				: POSITION;\r\n"
			"	float4  m_vBlendWeights		: BLENDWEIGHT;\r\n"
			"	float4  m_vBlendIndices		: BLENDINDICES;\r\n"
			"	float3  m_vNormal			: NORMAL;\r\n"
			"	float2  m_vTex				: TEXCOORD0;\r\n"
			"} Vertex : SEMANTIC_VS_INPUT; \r\n"
			" \r\n"

			"struct VS_INPUT_OBJECT \r\n"
			"{\r\n"
			"	float4  m_vPos				: POSITION;\r\n"
			"	float3  m_vNormal			: NORMAL;\r\n"
			"	float2  m_vTex				: TEXCOORD0;\r\n"
			"} VertexObject : SEMANTIC_VS_INPUT_OBJECT; \r\n"
			" \r\n"

			"struct VS_OUTPUT \r\n"
			"{\r\n"
			"	float4  m_vPos		: POSITION;\r\n"
			"	float2  m_vTex		: TEXCOORD0;\r\n"
			"	float2  m_vDepth	: TEXCOORD1;\r\n"
			"};\r\n"



			//////////////////////////////////////////////////////////////////////////
			//							옵션 - 저
			//////////////////////////////////////////////////////////////////////////
			// VS Skin
			"VS_OUTPUT VShade(VS_INPUT In, uniform int nNumBones)\r\n"
			"{\r\n"
			"	VS_OUTPUT   Out;\r\n"
			"	float3      vPos = 0.0f;\r\n"
			"	float3      vNormal = 0.0f;    \r\n"
			"	float       LastWeight = 0.0f;\r\n"

			// Compensate for lack of UBYTE4 on Geforce3
			"	int4 IndexVector = D3DCOLORtoUBYTE4(In.m_vBlendIndices);\r\n"

			// cast the vectors to arrays for use in the for loop below
			"	float BlendWeightsArray[4] = (float[4])In.m_vBlendWeights;\r\n"
			"	int   IndexArray[4]        = (int[4])IndexVector;\r\n"

			// calculate the pos/normal using the "normal" weights 
			//        and accumulate the weights to calculate the last weight
			"	for (int iBone = 0; iBone < nNumBones-1; iBone++)\r\n"
			"	{\r\n"
			"		LastWeight = LastWeight + BlendWeightsArray[iBone];\r\n"

			"		vPos += mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
			"	}\r\n"
			"	LastWeight = 1.0f - LastWeight; \r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos += (mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[nNumBones-1]]) * LastWeight);\r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex;\r\n"
			"	Out.m_vDepth  = Out.m_vPos.zw;\r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			// VS Object
			"VS_OUTPUT VS_OBJECT_1_2(VS_INPUT_OBJECT In)\r\n"
			"{\r\n"
			"	VS_OUTPUT   Out;\r\n"
			"	float3      vPos = 0.0f;\r\n"
			"	float       LastWeight = 0.0f;\r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos = mul(In.m_vPos, g_matWorld);\r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex;\r\n"
			"	Out.m_vDepth  = Out.m_vPos.zw;\r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			//			PS_1
			"float4 PS_1( VS_OUTPUT In, uniform bool bAlpha ) : COLOR0  \r\n"
			"{ \r\n"

			"	float4 Color = g_fShadowColor; \r\n"

			"	if ( bAlpha ) \r\n"
			"	{ \r\n"
			"		float4 vAlbedo = tex2D( BaseTexSampler, In.m_vTex ); \r\n"
			"		Color.a = vAlbedo.a; \r\n"			// SoftAlpha 랑 상관없이 날카롭게나옴.
			//"		Color.xyz *= vAlbedo.a; \r\n"		// SoftAlpha가 좋게 나온다
			"	} \r\n"

			" return Color;\r\n"
			"} \r\n"

			//////////////////////////////////////////////////////////////////////////
			//			PS_2
			"float4 PS_2( VS_OUTPUT In, uniform bool bAlpha ) : COLOR0  \r\n"
			"{ \r\n"

			// Depth is z / w
			// 캐릭터는 약간만 앞쪽에 있다고 셋팅하자.
			// -0.001f 이 안쓰였을 시에는 발쪽 부분의 그림자가 안나오는 문제가 생겼었음.
			"	float4 Color = (In.m_vDepth.x-g_fDepthOffset) / In.m_vDepth.y; \r\n"

			"	if ( bAlpha ) \r\n"
			"	{ \r\n"
			"		float4 vAlbedo = tex2D( BaseTexSampler, In.m_vTex ); \r\n"
			"		Color.a = vAlbedo.a; \r\n"			// SoftAlpha 랑 상관없이 날카롭게나옴.
			"	} \r\n"

			" return Color;\r\n"
			"} \r\n"

			//////////////////////////////////////////////////////////////////////////
			//	
			//////////////////////////////////////////////////////////////////////////
			"VertexShader vsArray_1[4] = { compile vs_2_0 VShade(1), \r\n"
			"compile vs_2_0 VShade(2),\r\n"
			"compile vs_2_0 VShade(3),\r\n"
			"compile vs_2_0 VShade(4) }; \r\n"

			"VertexShader vsArray[4] = { compile vs_2_0 VShade(1), \r\n"
			"compile vs_2_0 VShade(2),\r\n"
			"compile vs_2_0 VShade(3),\r\n"
			"compile vs_2_0 VShade(4) }; \r\n"


			//////////////////////////////////////
			// Techniques specs follow
			//////////////////////////////////////
			"technique runtime_Skin\r\n"
			"{\r\n"
			"	pass low \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = (vsArray_1[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_1( false );\r\n"
			"	} \r\n"
			"	pass mat \r\n"		// 2.0
			"	{\r\n"
			"		VertexShader = (vsArray[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_2( false );\r\n"
			"	} \r\n"
			"	pass low_alpha \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = (vsArray_1[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_1(true );\r\n"
			"	} \r\n"
			"	pass mat_alpha \r\n"		// 2.0
			"	{\r\n"
			"		AlphaTestEnable = TRUE;\r\n"
			"		VertexShader = (vsArray[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_2(true );\r\n"
			"	} \r\n"
			"}\r\n"
			"technique runtime_Object\r\n"
			"{\r\n"
			"	pass low \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT_1_2();\r\n"
			"		PixelShader = compile ps_2_0 PS_1( false );\r\n"
			"	} \r\n"
			"	pass mat \r\n"		// 2.0
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT_1_2();\r\n"
			"		PixelShader = compile ps_2_0 PS_2( false );\r\n"
			"	} \r\n"
			"	pass low_alpha \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT_1_2();\r\n"
			"		PixelShader = compile ps_2_0 PS_1( true );\r\n"
			"	} \r\n"
			"	pass mat_alpha \r\n"		// 2.0
			"	{\r\n"
			"		AlphaTestEnable = TRUE;\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT_1_2();\r\n"
			"		PixelShader = compile ps_2_0 PS_2(true );\r\n"
			"	} \r\n"
			"}\r\n"
			"\r\n";
	}



	//////////////////////////////////////////////////////////////////////////
	//
	void SetDefaultFX_Shadow_G32R32F( std::string& strFX )
	{
		strFX = 
			"//-------------------------------------------------------------------------------------- \r\n"
			"//       shhan \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"

			"float		g_fAlpha=1.f;\r\n"
			"int		g_nCurNumBones = 2;\r\n"
			"float		g_fShadowColor = 0.25f; \r\n"
			"float3		g_vLightPosWorldSpace; \r\n"

			// Matrix Pallette
			"static const int MAX_MATRICES = 60;\r\n"
			"float4x3   g_matWorld;\r\n"
			"float4x3   g_matWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\r\n"
			"float4x4   g_matViewProj : VIEWPROJECTION;\r\n"


			"texture g_BaseTexture; \r\n"
			"sampler BaseTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_BaseTexture); \r\n"
			"}; \r\n"


			///////////////////////////////////////////////////////
			"struct VS_INPUT \r\n"
			"{\r\n"
			"	float4  m_vPos				: POSITION;\r\n"
			"	float4  m_vBlendWeights		: BLENDWEIGHT;\r\n"
			"	float4  m_vBlendIndices		: BLENDINDICES;\r\n"
			"	float3  m_vNormal			: NORMAL;\r\n"
			"	float2  m_vTex				: TEXCOORD0;\r\n"
			"} Vertex : SEMANTIC_VS_INPUT; \r\n"
			" \r\n"

			"struct VS_INPUT_OBJECT \r\n"
			"{\r\n"
			"	float4  m_vPos				: POSITION;\r\n"
			"	float3  m_vNormal			: NORMAL;\r\n"
			"	float2  m_vTex				: TEXCOORD0;\r\n"
			"} VertexObject : SEMANTIC_VS_INPUT_OBJECT; \r\n"
			" \r\n"

			"struct VS_OUTPUT \r\n"
			"{\r\n"
			"	float4  m_vPos		: POSITION;\r\n"
			"	float2  m_vTex		: TEXCOORD0;\r\n"
			"	float2  m_vDepth	: TEXCOORD1;\r\n"
			"};\r\n"

			"struct VS_OUTPUT_VSM \r\n"
			"{\r\n"
			"	float4  m_vPos		: POSITION;\r\n"
			"	float2  m_vTex		: TEXCOORD0;\r\n"
			"	float   m_fDepth	: TEXCOORD1;\r\n"
			"};\r\n"



			//////////////////////////////////////////////////////////////////////////
			//							옵션 - 저
			//////////////////////////////////////////////////////////////////////////
			// VS Skin
			"VS_OUTPUT VShade_1(VS_INPUT In, uniform int nNumBones)\r\n"
			"{\r\n"
			"	VS_OUTPUT   Out;\r\n"
			"	float3      vPos = 0.0f;\r\n"
			"	float3      vNormal = 0.0f;    \r\n"
			"	float       LastWeight = 0.0f;\r\n"

			// Compensate for lack of UBYTE4 on Geforce3
			"	int4 IndexVector = D3DCOLORtoUBYTE4(In.m_vBlendIndices);\r\n"

			// cast the vectors to arrays for use in the for loop below
			"	float BlendWeightsArray[4] = (float[4])In.m_vBlendWeights;\r\n"
			"	int   IndexArray[4]        = (int[4])IndexVector;\r\n"

			// calculate the pos/normal using the "normal" weights 
			//        and accumulate the weights to calculate the last weight
			"	for (int iBone = 0; iBone < nNumBones-1; iBone++)\r\n"
			"	{\r\n"
			"		LastWeight = LastWeight + BlendWeightsArray[iBone];\r\n"

			"		vPos += mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
			"	}\r\n"
			"	LastWeight = 1.0f - LastWeight; \r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos += (mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[nNumBones-1]]) * LastWeight);\r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex;\r\n"
			"	Out.m_vDepth  = Out.m_vPos.zw;\r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			// VS Object
			"VS_OUTPUT VS_OBJECT_1(VS_INPUT_OBJECT In)\r\n"
			"{\r\n"
			"	VS_OUTPUT   Out;\r\n"
			"	float3      vPos = 0.0f;\r\n"
			"	float       LastWeight = 0.0f;\r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos = mul(In.m_vPos, g_matWorld);\r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex; \r\n"
			"	Out.m_vDepth  = Out.m_vPos.zw;\r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			//			PS_1
			"float4 PS_1( VS_OUTPUT In, uniform bool bAlpha ) : COLOR0  \r\n"
			"{ \r\n"

			"	float4 Color = g_fShadowColor; \r\n"

			"	if ( bAlpha ) \r\n"
			"	{ \r\n"
			"		float4 vAlbedo = tex2D( BaseTexSampler, In.m_vTex ); \r\n"
			"		Color.xyz *= vAlbedo.a; \r\n"		// SoftAlpha가 좋게 나온다
			"	} \r\n"

			" return Color;\r\n"
			"} \r\n"

			//////////////////////////////////////////////////////////////////////////
			//							옵션 - VSM
			//////////////////////////////////////////////////////////////////////////
			// VS Skin
			"VS_OUTPUT_VSM VShade_2(VS_INPUT In, uniform int nNumBones)\r\n"
			"{\r\n"
			"	VS_OUTPUT_VSM   Out;\r\n"
			"	float3      vPos = 0.0f;\r\n"
			"	float3      vNormal = 0.0f;    \r\n"
			"	float       LastWeight = 0.0f;\r\n"

			// Compensate for lack of UBYTE4 on Geforce3
			"	int4 IndexVector = D3DCOLORtoUBYTE4(In.m_vBlendIndices);\r\n"

			// cast the vectors to arrays for use in the for loop below
			"	float BlendWeightsArray[4] = (float[4])In.m_vBlendWeights;\r\n"
			"	int   IndexArray[4]        = (int[4])IndexVector;\r\n"

			// calculate the pos/normal using the "normal" weights 
			//        and accumulate the weights to calculate the last weight
			"	for (int iBone = 0; iBone < nNumBones-1; iBone++)\r\n"
			"	{\r\n"
			"		LastWeight = LastWeight + BlendWeightsArray[iBone];\r\n"

			"		vPos += mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
			"	}\r\n"
			"	LastWeight = 1.0f - LastWeight; \r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos += (mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[nNumBones-1]]) * LastWeight);\r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex;\r\n"
			"	Out.m_fDepth = length( g_vLightPosWorldSpace - vPos.xyz ); \r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			// VS Object
			"VS_OUTPUT_VSM VS_OBJECT_2(VS_INPUT_OBJECT In)\r\n"
			"{\r\n"
			"	VS_OUTPUT_VSM   Out;\r\n"
			"	float3      vPos = 0.0f;\r\n"
			"	float       LastWeight = 0.0f;\r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos = mul(In.m_vPos, g_matWorld);\r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex; \r\n"
			"	Out.m_fDepth = length( g_vLightPosWorldSpace - vPos.xyz ); \r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			//			PS_2
			"float4 PS_2( VS_OUTPUT_VSM In, uniform bool bAlpha ) : COLOR0  \r\n"
			"{ \r\n"

			//// Compute partial derivatives of depth
			//"	float dx = ddx(In.m_fDepth); \r\n"
			//"	float dy = ddy(In.m_fDepth); \r\n"

			//"	float yy = In.m_fDepth*In.m_fDepth + 0.25*(dx*dx + dy*dy); \r\n"

			//"	return float4( In.m_fDepth, yy, 0.f, 1.f );\r\n"

			"   float fAlpha=1.f; \r\n"
			"	if ( bAlpha ) \r\n"
			"	{ \r\n"
			"		fAlpha = tex2D( BaseTexSampler, In.m_vTex ).a; \r\n"
			"	} \r\n"

			"	return float4( In.m_fDepth, In.m_fDepth*In.m_fDepth, 0.f, fAlpha );\r\n"

			//"	return float4( In.m_fDepth, In.m_fDepth*In.m_fDepth, 0.f, 1.f );\r\n"
			"} \r\n"

			//////////////////////////////////////////////////////////////////////////
			//	
			//////////////////////////////////////////////////////////////////////////
			"VertexShader vsArray_1[4] = { compile vs_2_0 VShade_1(1), \r\n"
			"compile vs_2_0 VShade_1(2),\r\n"
			"compile vs_2_0 VShade_1(3),\r\n"
			"compile vs_2_0 VShade_1(4) }; \r\n"

			"VertexShader vsArray[4] = { compile vs_3_0 VShade_2(1), \r\n"
			"compile vs_3_0 VShade_2(2),\r\n"
			"compile vs_3_0 VShade_2(3),\r\n"
			"compile vs_3_0 VShade_2(4) }; \r\n"


			//////////////////////////////////////
			// Techniques specs follow
			//////////////////////////////////////
			"technique runtime_Skin\r\n"
			"{\r\n"
			"	pass low \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = (vsArray_1[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_1( false );\r\n"
			"	} \r\n"
			"	pass mat \r\n"		// 2.0
			"	{\r\n"
			"		VertexShader = (vsArray[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_3_0 PS_2( false );\r\n"
			"	} \r\n"
			"	pass low_alpha \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = (vsArray_1[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_1( true );\r\n"
			"	} \r\n"
			"	pass mat_alpha \r\n"		// 2.0
			"	{\r\n"
			"		AlphaTestEnable = TRUE;\r\n"
			"		VertexShader = (vsArray[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_3_0 PS_2( true );\r\n"
			"	} \r\n"
			"}\r\n"
			"technique runtime_Object\r\n"
			"{\r\n"
			"	pass low \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT_1();\r\n"
			"		PixelShader = compile ps_2_0 PS_1( false );\r\n"
			"	} \r\n"
			"	pass mat \r\n"		// 2.0
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT_2();\r\n"
			"		PixelShader = compile ps_3_0 PS_2( false );\r\n"
			"	} \r\n"
			"	pass low_alpha \r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT_1();\r\n"
			"		PixelShader = compile ps_2_0 PS_1( true );\r\n"
			"	} \r\n"
			"	pass mat_alpha \r\n"		// 2.0
			"	{\r\n"
			"		AlphaTestEnable = TRUE;\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT_2();\r\n"
			"		PixelShader = compile ps_3_0 PS_2( true );\r\n"
			"	} \r\n"
			"}\r\n"
			"\r\n";
	}



	//////////////////////////////////////////////////////////////////////////
	//
	void SetDefaultFX_Kajiya_Kay_Hair( std::string& strFX )
	{
		strFX = 
			"//-------------------------------------------------------------------------------------- \r\n"
			"//       shhan \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"

			"float	g_fAlpha=1.f;\r\n"
			"float	g_fTexDiffColor; \r\n"
			"int	g_nMulti_ADD;\r\n"
			"float2 g_vFOG;		// FogEnd, FogDist \r\n"
			"int	g_nCurNumBones = 2;\r\n"
			"float4 g_vlhtDirViewSpace = {0.0f, 0.0f, -1.0f, 1.0f};    //light Direction\r\n"
			"float3 g_vSpecularDirViewSpace; \r\n"
			"float4 g_vLightAmbient : MATERIALAMBIENT = {0.1f, 0.1f, 0.1f, 1.0f};\r\n"
			"float4 g_vLightDiffuse : MATERIALDIFFUSE = {0.8f, 0.8f, 0.8f, 1.0f};\r\n"
			"float3 g_vCameraFrom; \r\n"
			"float4 g_vHairShiftUV_SpecExp_DL_CL; \r\n"
			"float4 g_vHairSpecColor_DirectionIntensity; \r\n"

			// Matrix Pallette
			"static const int MAX_MATRICES = 60;\r\n"
			"float4x3   g_matWorld;\r\n"
			"float4x3   g_matView;\r\n"
			"float4x4   g_matInverseView; \r\n"
			"float4x3   g_matWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\r\n"
			"float4x4   g_matViewProj : VIEWPROJECTION;\r\n"

			"texture g_BaseTexture; \r\n"
			"sampler BaseTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_BaseTexture); \r\n"
			"}; \r\n"

			"texture g_MaskTexture; \r\n"
			"sampler MaskTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_MaskTexture); \r\n"
			"}; \r\n"

			"texture g_Mask2Texture; \r\n"
			"sampler Mask2TexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_Mask2Texture); \r\n"
			"	AddressU = Mirror; \r\n"
			"	AddressV = Mirror; \r\n"
			"}; \r\n"

			

			///////////////////////////////////////////////////////
			"struct VS_INPUT\r\n"
			"{\r\n"
			"	float4  m_vPos				: POSITION;\r\n"
			"	float4  m_vBlendWeights		: BLENDWEIGHT;\r\n"
			"	float4  m_vBlendIndices		: BLENDINDICES;\r\n"
			"	float3  m_vNormal			: NORMAL;\r\n"
			"	float3	m_vBinormal			: BINORMAL; \r\n"
			//"	float3	m_vTangent			: TANGENT; \r\n"
			"	float2  m_vTex				: TEXCOORD0;\r\n"
			"} Vertex : SEMANTIC_VS_INPUT; \r\n"
			" \r\n"

			"struct VS_INPUT_OBJECT \r\n"
			"{\r\n"
			"	float4  m_vPos				: POSITION;\r\n"
			"	float3  m_vNormal			: NORMAL;\r\n"
			"	float3	m_vBinormal			: BINORMAL; \r\n"
			//"	float3	m_vTangent			: TANGENT; \r\n"
			"	float2  m_vTex				: TEXCOORD0;\r\n"
			"} VertexObject : SEMANTIC_VS_INPUT_OBJECT; \r\n"
			" \r\n"

			"struct VS_OUTPUT \r\n"
			"{\r\n"
			"	float4  m_vPos			: POSITION; \r\n"
			"	float2  m_vTex			: TEXCOORD0; \r\n"
			"	float3  m_vNor			: TEXCOORD1; \r\n"
			"	float3  m_vBinormal		: TEXCOORD2; \r\n"
			//"	float3  m_vTangent		: TEXCOORD3; \r\n"
			"	float	m_fFog			: FOG; \r\n"
			"};\r\n"


			//////////////////////////////////////////////////////////////////////////
			//							옵션 - 중
			//////////////////////////////////////////////////////////////////////////
			// VS_2_0 Skin
			"VS_OUTPUT VShadeVS(VS_INPUT In, uniform int nNumBones)\r\n"
			"{\r\n"
			"	VS_OUTPUT   Out;\r\n"
			"	float3      vPos = 0.0f;\r\n"
			"	float3      vNormal = 0.0f;    \r\n"
			"	float3      vBinormal = 0.0f;    \r\n"
			//"	float3      vTangent = 0.0f;    \r\n"
			"	float       LastWeight = 0.0f;\r\n"

			// Compensate for lack of UBYTE4 on Geforce3
			"	int4 IndexVector = D3DCOLORtoUBYTE4(In.m_vBlendIndices);\r\n"

			// cast the vectors to arrays for use in the for loop below
			"	float BlendWeightsArray[4] = (float[4])In.m_vBlendWeights;\r\n"
			"	int   IndexArray[4]        = (int[4])IndexVector;\r\n"

			// calculate the pos/normal using the "normal" weights 
			//        and accumulate the weights to calculate the last weight
			"	for (int iBone = 0; iBone < nNumBones-1; iBone++)\r\n"
			"	{\r\n"
			"		LastWeight = LastWeight + BlendWeightsArray[iBone];\r\n"

			"		vPos += mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
			"		vNormal += mul(In.m_vNormal, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
			"		vBinormal += mul(In.m_vBinormal, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
			//"		vTangent += mul(In.m_vTangent, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
			"	}\r\n"
			"	LastWeight = 1.0f - LastWeight; \r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos += (mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[nNumBones-1]]) * LastWeight);\r\n"
			"	vNormal += (mul(In.m_vNormal, g_matWorldMatrixArray[IndexArray[nNumBones-1]]) * LastWeight); \r\n"
			"	vBinormal += (mul(In.m_vBinormal, g_matWorldMatrixArray[IndexArray[nNumBones-1]]) * LastWeight); \r\n"
			//"	vTangent += (mul(In.m_vTangent, g_matWorldMatrixArray[IndexArray[nNumBones-1]]) * LastWeight); \r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// Normal
			"	Out.m_vNor = mul(vNormal.xyz, g_matView);\r\n"
			"	Out.m_vBinormal = mul(vBinormal, g_matView); \r\n"
			//"	Out.m_vTangent = mul(vTangent, g_matView); \r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex.xy;\r\n"

			// Fog
			"	Out.m_fFog = saturate((g_vFOG.x - Out.m_vPos.z) / g_vFOG.y); \r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			// VS_2_0 Skin
			"VS_OUTPUT VS_OBJECT(VS_INPUT_OBJECT In)\r\n"
			"{\r\n"
			"	VS_OUTPUT  Out;\r\n"
			"	float3		vPos = 0.0f;\r\n"
			"	float3		vNormal = 0.0f;    \r\n"
			"	float3      vBinormal = 0.0f;    \r\n"
			"	float3      vTangent = 0.0f;    \r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos = mul(In.m_vPos, g_matWorld);\r\n"
			"	vNormal = mul(In.m_vNormal, g_matWorld); \r\n"
			"	vBinormal = normalize( mul(In.m_vBinormal, g_matWorld) ); \r\n"
			//"	vTangent = normalize( mul(In.m_vTangent, g_matWorld) ); \r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// Normal
			"	Out.m_vNor = mul(vNormal.xyz, g_matView);\r\n"
			"	Out.m_vBinormal = mul(vBinormal, g_matView); \r\n"
			//"	Out.m_vTangent = mul(vTangent, g_matView); \r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex.xy;\r\n"

			// Fog
			"	Out.m_fFog = saturate((g_vFOG.x - Out.m_vPos.z) / g_vFOG.y); \r\n"

			"	return Out;\r\n"
			"}\r\n"


			//////////////////////////////////////////////////////////////////////////
			//			ShiftTangent
			"float3 ShiftTangent ( float3 vTangent, float3 vNormal, float fShift )  \r\n"
			"{  \r\n"
			"	float3 vShiftedTangent = vTangent + (fShift * vNormal);  \r\n"
			"	return normalize( vShiftedTangent );  \r\n"
			"}  \r\n"


			//////////////////////////////////////////////////////////////////////////
			//			StrandSpecular
			"float StrandSpecular2( float3 vTangent, float3 vHalf, float fExponent )  \r\n"
			"{  \r\n"
			"	float fDotTangentHalf = dot( vTangent, vHalf );  \r\n"
			"	float fSinTangentHalf = sqrt(1.f - (fDotTangentHalf*fDotTangentHalf));  \r\n"
			"	float fDirAtten = smoothstep( -1.f, 0.f, fDotTangentHalf );  \r\n"
			"	return fDirAtten * pow( fSinTangentHalf, fExponent );  \r\n"
			"}  \r\n"
			

			//////////////////////////////////////////////////////////////////////////
			//			StrandSpecular
			"float StrandSpecular ( float3 vTangent, float3 vView, float3 vLight, float fExponent )  \r\n"
			"{  \r\n"
			"	float3 vHalf = normalize( vLight + vView );  \r\n"
			"	float fDotTangentHalf = dot( vTangent, vHalf );  \r\n"
			"	float fSinTangentHalf = sqrt(1.f - (fDotTangentHalf*fDotTangentHalf));  \r\n"
			"	float fDirAtten = smoothstep( -1.f, 0.f, fDotTangentHalf );  \r\n"
			"	return fDirAtten * pow( fSinTangentHalf, fExponent );  \r\n"
			"}  \r\n"


			//////////////////////////////////////////////////////////////////////////
			//			PS_2
			"float4 PS_2( VS_OUTPUT In ) : COLOR0  \r\n"
			"{ \r\n"

			// Shift Tangent의 Texture 를 얻는다.
			"	float4 vMask = tex2D( MaskTexSampler, In.m_vTex*g_vHairShiftUV_SpecExp_DL_CL.xy ); \r\n"
			"	float fShiftTex = vMask.r - 0.5f; \r\n"

			// Mask 를 얻는다.
			"	float fSpecMaskTex = tex2D( MaskTexSampler, In.m_vTex ).g; \r\n"

			// Shift Tangents
			"	float3 vShiftTangent = ShiftTangent( In.m_vBinormal, In.m_vNor, fShiftTex ); \r\n"

			// diffuse lighting 
			"	float fNDotL = dot(In.m_vNor, g_vlhtDirViewSpace.xyz); \r\n"

			// Shade (Ambient + etc.)
			"	float3 vDiffuse = max(0.0f, fNDotL) * g_vLightDiffuse.xyz; \r\n"
			"	float3 vAmbient = g_vLightAmbient.xyz; \r\n"

			// specular lighting
			"	float3 vViewDir = float3(0.f,0.f,-1.f); \r\n"
			"	float3 vSpecular = StrandSpecular( vShiftTangent, vViewDir, g_vlhtDirViewSpace.xyz, g_vHairShiftUV_SpecExp_DL_CL.z ); \r\n"
			"	vSpecular *= g_vHairSpecColor_DirectionIntensity.w; \r\n"
			"	float3 vSpecularCamera = StrandSpecular2( vShiftTangent, g_vSpecularDirViewSpace.xyz, g_vHairShiftUV_SpecExp_DL_CL.w ); \r\n"
			"	vSpecular += vSpecularCamera * g_vHairSpecColor_DirectionIntensity.xyz * fSpecMaskTex; \r\n"		// 카메라에 달려있는 Light 만 Mask 맵과 Color를 쓰도록 한다.

			// Final Color Assembly
			"	float4 vOut; \r\n"
			"	vOut = tex2D( BaseTexSampler, In.m_vTex ); \r\n"
			"	vOut.rgb *= (vDiffuse+vAmbient+vSpecular); \r\n"

			"	return vOut; \r\n"
			"} \r\n"

			//////////////////////////////////////////////////////////////////////////
			//	
			//////////////////////////////////////////////////////////////////////////
			"VertexShader vsArray[4] = { compile vs_2_0 VShadeVS(1), \r\n"
										"compile vs_2_0 VShadeVS(2),\r\n"
										"compile vs_2_0 VShadeVS(3),\r\n"
										"compile vs_2_0 VShadeVS(4) }; \r\n"


			//////////////////////////////////////
			// Techniques specs follow
			//////////////////////////////////////
			"technique runtime_Skin\r\n"
			"{\r\n"
			"	pass middle\r\n"	// 2.0
			"	{\r\n"
			"		VertexShader = (vsArray[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_2();\r\n"
			"	}\r\n"
			"	pass high\r\n"		// 3.0
			"	{\r\n"
			"		VertexShader = (vsArray[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_2();\r\n"
			"	}\r\n"
			"}\r\n"
			"technique runtime_Object\r\n"
			"{\r\n"
			"	pass middle\r\n"	// 2.0
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT();\r\n"
			"		PixelShader = compile ps_2_0 PS_2();\r\n"
			"	}\r\n"
			"	pass high\r\n"	// 3.0
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT();\r\n"
			"		PixelShader = compile ps_2_0 PS_2();\r\n"
			"	}\r\n"
			"}\r\n"
			"\r\n";
	}
};