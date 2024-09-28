#include "pch.h"

#include "NSMaterialSkinManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSMaterialSkinManager
{

	//////////////////////////////////////////////////////////////////////////
	//
	void SetDefaultFX_SM1( std::string& strFX )
	{
		strFX = 
			"//-------------------------------------------------------------------------------------- \r\n"
			"//       shhan \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"

			"float g_fAlpha=1.f;\r\n"
			"int g_nCurNumBones = 2;\r\n"
			"float2 g_vFOG;	// FogEnd, FogDist \r\n"
			"float4 g_vlhtDirViewSpace = {0.0f, 0.0f, -1.0f, 1.0f};    //light Direction\r\n"
			"float4 g_vLightAmbient : MATERIALAMBIENT = {0.1f, 0.1f, 0.1f, 1.0f};\r\n"
			"float4 g_vLightDiffuseSaturation : MATERIALDIFFUSE = {0.8f, 0.8f, 0.8f, 1.0f};\r\n"	// xyz - Diffuse, w - Saturation
			"float4 g_vLightDiffuseDynamic; \r\n"
			"float	g_fInverseNormal = 1.f; \r\n"

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
			"	float4  m_vDiffuse	: COLOR0;\r\n"
			//"	float4  m_vAmbient	: COLOR1;\r\n"
			"	float2  m_vTex		: TEXCOORD0;\r\n"
			"	float	m_fFog		: FOG; \r\n"
			"};\r\n"


			//////////////////////////////////////////////////////////////////////////
			// Util
			"float3 DiffuseDot(float3 vNormal)\r\n"
			"{\r\n"
			"	float CosTheta;\r\n"

			// N.L Clamped
			"	CosTheta = max(0.0f, dot(vNormal, g_vlhtDirViewSpace.xyz));\r\n"

			// propogate scalar result to vector
			"	return (CosTheta);\r\n"
			"}\r\n"


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
			"	vNormal *= g_fInverseNormal; \r\n"
			"	vNormal = mul(vNormal, g_matView); \r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// Shade (Ambient + etc.)
			//"	Out.m_vDiffuse.xyz = DiffuseDot(normalize(vNormal)) * g_vLightDiffuseSaturation.xyz;\r\n"
			"	Out.m_vDiffuse.xyz = DiffuseDot(normalize(vNormal));\r\n"
			"	Out.m_vDiffuse.w = 1.0f;\r\n"
			//"	Out.m_vAmbient.xyz = g_vLightAmbient.xyz; \r\n"
			//"	Out.m_vAmbient.w = 1.0f;\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex.xy;\r\n"

			// Fog
			"	Out.m_fFog = saturate((g_vFOG.x - Out.m_vPos.z) / g_vFOG.y); \r\n"

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
			"	vNormal *= g_fInverseNormal; \r\n"
			"	vNormal = mul(vNormal, g_matView); \r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// Shade (Ambient + etc.)
			//"	Out.m_vDiffuse.xyz = DiffuseDot(normalize(vNormal)) * g_vLightDiffuseSaturation.xyz;\r\n"
			"	Out.m_vDiffuse.xyz = DiffuseDot(normalize(vNormal));\r\n"
			"	Out.m_vDiffuse.w = 1.0f;\r\n"
			//"	Out.m_vAmbient.xyz = g_vLightAmbient.xyz; \r\n"
			//"	Out.m_vAmbient.w = 1.0f;\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex.xy;\r\n"

			// Fog
			"	Out.m_fFog = saturate((g_vFOG.x - Out.m_vPos.z) / g_vFOG.y); \r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			//			PS_1
			//
			// 여기선 채도값 계산을 하지는 않아서, 텍스쳐가 어두울 경우 다른 옵션보다는 어둡게 나올 수 있다.
			//
			"float4 PS_1( VS_OUTPUT In ) : COLOR0  \r\n"
			"{ \r\n"

			// Texture
			"	float4 vAlbedo = tex2D( BaseTexSampler, In.m_vTex ); \r\n"

			// Albedo 와 Alpha 를 곱한다.
			// Diffuse Color
			"	vAlbedo.xyz *= In.m_vDiffuse.xyz * (g_vLightDiffuseSaturation.xyz + g_vLightDiffuseDynamic.xyz) + g_vLightAmbient.xyz; \r\n"
			"	vAlbedo.w *= g_fAlpha; \r\n"

			"	return vAlbedo; \r\n"
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
			"	pass low\r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = (vsArray[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_1();\r\n"
			"	}\r\n"
			"}\r\n"
			"technique runtime_Object\r\n"
			"{\r\n"
			"	pass low\r\n"		// 1.1
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT_1();\r\n"
			"		PixelShader = compile ps_2_0 PS_1();\r\n"
			"	}\r\n"
			"}\r\n"
			"\r\n";
	}


	//////////////////////////////////////////////////////////////////////////
	//
	void SetDefaultFX( std::string& strFX )
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
			//"float4 g_vLightAmbientORG; \r\n"
			"float4 g_vLightAmbient : MATERIALAMBIENT = {0.1f, 0.1f, 0.1f, 1.0f};\r\n"
			"float4 g_vLightDiffuseSaturation : MATERIALDIFFUSE = {0.8f, 0.8f, 0.8f, 1.0f};\r\n"	// xyz - Diffuse, w - Saturation
			"float4 g_vLightDiffuseDynamic; \r\n"
			"float4 g_vSpecColor_Power; \r\n"
			//"float4 g_vSpec2Color_Power; \r\n"
			"float3 g_vSSS_Color; \r\n"
			"float4 g_vLightLevel; \r\n"
			"float4 g_vFogColor; \r\n"
			"float3 g_vLevelFlowColor; \r\n"
			"float3 g_vCameraFrom; \r\n"
			"float  g_fCubeMapPowerForOldMap = 1.f; \r\n"
			"float  g_fFresnelForSSS = 3.f; \r\n"
			"float	g_fInverseLightIntensity = 0.3f; \r\n"
			"float3 g_vLightPosWorldSpace; \r\n"
			"float3 g_vShadow; \r\n"
			"float3 g_vFakeSpecular; \r\n"
			"float	g_fInverseNormal = 1.f; \r\n"

			// Matrix Pallette
			"static const int MAX_MATRICES = 60;\r\n"
			"float4x3   g_matWorld;\r\n"
			"float4x3   g_matView;\r\n"
			"float4x3   g_matWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\r\n"
			"float4x4   g_matViewProj : VIEWPROJECTION;\r\n"
			"float4x4	g_matShadow_Clip2Tex; \r\n"
			"float4x4	g_matShadow_View; \r\n"

			"texture g_BaseTexture; \r\n"
			"sampler BaseTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_BaseTexture); \r\n"
			"}; \r\n"

			"texture g_ShadowTexture; \r\n"
			"sampler ShadowTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_ShadowTexture); \r\n"
			" \r\n"
			"	MinFilter = Point; \r\n"
			"	MagFilter = Point; \r\n"
			"}; \r\n"

			"texture g_ShadowTexture_PCF; \r\n"
			"sampler ShadowTexSampler_PCF = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_ShadowTexture_PCF); \r\n"
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
			"}; \r\n"

			"texture g_LevelFlowTexture; \r\n"
			"sampler LevelFlowTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_LevelFlowTexture); \r\n"
			"   AddressU = MIRROR; \r\n"
			"   AddressV = MIRROR; \r\n"
			"}; \r\n"

			"texture g_MultiTexture; \r\n"
			"sampler MultiTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_MultiTexture); \r\n"
			"}; \r\n"

			"texture g_pTexUpDownTexture; \r\n"
			"sampler TexUpDownSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_pTexUpDownTexture); \r\n"
			"}; \r\n"

			"texture g_SphereTexture; \r\n"
			"sampler SphereTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_SphereTexture); \r\n"
			"	AddressU = Clamp; \r\n"
			"	AddressV = Clamp; \r\n"
			"}; \r\n"

			"texture g_GlobalCubeTexture; \r\n"
			"sampler CubeTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_GlobalCubeTexture); \r\n"
			"	AddressU = Clamp; \r\n"
			"	AddressV = Clamp; \r\n"
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
			"	float4  m_vPos				: POSITION; \r\n"
			"	float2  m_vTex				: TEXCOORD0; \r\n"
			"	float3  m_vNor				: TEXCOORD1; \r\n"
			"	float3  m_vPosWorldSpace	: TEXCOORD2; \r\n"

			// USE_CUBEMAP
			"   #ifdef USE_CUBEMAP \r\n"
			"		float3  m_vCubeTexcoord		: TEXCOORD3; \r\n"
			"	#endif \r\n"

			"	float	m_fFog		: FOG; \r\n"
			"};\r\n"


			//////////////////////////////////////////////////////////////////////////
			// Util
			"float3 DiffuseDot(float3 vNormal)\r\n"
			"{\r\n"
			"	float CosTheta;\r\n"

			// N.L Clamped
			"	CosTheta = max(0.0f, dot(vNormal, g_vlhtDirViewSpace.xyz));\r\n"

			// propogate scalar result to vector
			"	return (CosTheta);\r\n"
			"}\r\n"


			//////////////////////////////////////////////////////////////////////////
			//							옵션 - 중
			//////////////////////////////////////////////////////////////////////////
			// VS_2_0 Skin
			"VS_OUTPUT VShadeVS(VS_INPUT In, uniform int nNumBones)\r\n"
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

			"		vPos += mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[iBone]])  * BlendWeightsArray[iBone];\r\n"
			"		vNormal += mul(In.m_vNormal, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
			"	}\r\n"
			"	LastWeight = 1.0f - LastWeight; \r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos += (mul(In.m_vPos, g_matWorldMatrixArray[IndexArray[nNumBones-1]]) * LastWeight);\r\n"
			"	vNormal += (mul(In.m_vNormal, g_matWorldMatrixArray[IndexArray[nNumBones-1]]) * LastWeight); \r\n"
			"	vNormal *= g_fInverseNormal; \r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPosWorldSpace = vPos; \r\n"
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// Normal
			"	Out.m_vNor = mul(vNormal.xyz, g_matView);\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex.xy;\r\n"

			// USE_CUBEMAP
			"   #ifdef USE_CUBEMAP \r\n"
			"	{ \r\n"
			"		float3 vEyeToVector = normalize(vPos.xyz - g_vCameraFrom); \r\n"
			"		Out.m_vCubeTexcoord = reflect(vEyeToVector, vNormal); \r\n"
			"	} \r\n"
			"	#endif \r\n"

			// Fog
			"	Out.m_fFog = saturate((g_vFOG.x - Out.m_vPos.z) / g_vFOG.y); \r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			// VS_2_0 Skin
			"VS_OUTPUT VS_OBJECT(VS_INPUT_OBJECT In)\r\n"
			"{\r\n"
			"	VS_OUTPUT  Out;\r\n"
			"	float3			vPos = 0.0f;\r\n"
			"	float3			vNormal = 0.0f;    \r\n"
			"	float			LastWeight = 0.0f;\r\n"

			// Now that we have the calculated weight, add in the final influence
			"	vPos = mul(In.m_vPos, g_matWorld);\r\n"
			"	vNormal = mul(In.m_vNormal, g_matWorld); \r\n"
			"	vNormal *= g_fInverseNormal; \r\n"

			// transform position from world space into view and then projection space
			"	Out.m_vPosWorldSpace = vPos; \r\n"
			"	Out.m_vPos = mul(float4(vPos.xyz, 1.0f), g_matViewProj);\r\n"

			// Normal
			"	Out.m_vNor = mul(vNormal.xyz, g_matView);\r\n"

			// copy the input texture coordinate through
			"	Out.m_vTex  = In.m_vTex.xy;\r\n"

			// USE_CUBEMAP
			"   #ifdef USE_CUBEMAP \r\n"
			"	{ \r\n"
			"		float3 vEyeToVector = normalize(vPos.xyz-g_vCameraFrom); \r\n"
			"		Out.m_vCubeTexcoord = reflect(vEyeToVector, vNormal); \r\n"
			"	} \r\n"
			"	#endif \r\n"

			// Fog
			"	Out.m_fFog = saturate((g_vFOG.x - Out.m_vPos.z) / g_vFOG.y); \r\n"

			"	return Out;\r\n"
			"}\r\n"

			//////////////////////////////////////////////////////////////////////////
			// VSM
			"float2 btex2D_rg(sampler2D shadowMap, float2 uv, float radius, float2 offset)   \r\n"
			"{  \r\n"
			// simple 3x3 filter
			"	float2 o = offset;  \r\n"
			"	float2 c = tex2D(shadowMap, uv.xy).rg ; \r\n" // center
			"	c += tex2D(shadowMap, uv.xy - o.xy).rg; \r\n"  // top left
			"	c += tex2D(shadowMap, uv.xy + o.xy).rg; \r\n"  // bottom right
			"	c += tex2D(shadowMap, float2(uv.x - o.x, uv.y)).rg; \r\n"  // left
			"	c += tex2D(shadowMap, float2(uv.x + o.x, uv.y)).rg; \r\n"  // right
			"	c += tex2D(shadowMap, float2(uv.x, uv.y + o.y)).rg; \r\n"  // bottom
			"	c += tex2D(shadowMap, float2(uv.x, uv.y - o.y)).rg; \r\n"  // top
			"	c += tex2D(shadowMap, float2(uv.x - o.x, uv.y + o.y)).rg; \r\n"  // bottom left
			"	c += tex2D(shadowMap, float2(uv.x + o.x, uv.y - o.y)).rg; \r\n"  // top right
			"	return c / 9;  \r\n"
			"}  \r\n"

			"float linstep(float min, float max, float v)   \r\n"
			"{   \r\n"
			"	return clamp((v - min) / (max - min), 0, 1);   \r\n"
			"}   \r\n"

			"float ReduceLightBleeding(float p_max, float Amount)   \r\n"
			"{   \r\n"
			// Remove the [0, Amount] tail and linearly rescale (Amount, 1]. 
			"	return linstep(Amount, 1, p_max);  \r\n" 
			"}   \r\n"

			// 이 수치가 낮으면 쉽게 검게 변한다. ( 각지는 현상이 나타난다. 지글거리는 현상이 나타난다. )
			// 이 수치가 높으면 쉽게 흰색으로 변한다. ( 그림자가 나와야할곳에서 안나온다 )
			// 적당한 값을 변경하면서 찾아야 한다.
			//"#define MIN_VARIANCE 0.001  \r\n"
			// variance 값을 기준으로 그림자 Zero 가 만들어지면 안되고, d*d 를기준으로 그림자 Zero 가 만들어져야 한다.
			"#define MIN_VARIANCE 1  \r\n"		// 현재 캐릭터는 1 이상은 되야한다.

			// @param t In this context its the distance to light.
			"float ChebyshevUpperBound(float2 moments, float t)  \r\n"
			"{  \r\n"
			// one tail ineqality valid if t > moments.x
			"	float p = (t <= moments.x);  \r\n"

			// compute variance 
			"	float variance = moments.y - (moments.x*moments.x);  \r\n"
			"	variance = max(variance, MIN_VARIANCE);  \r\n"

			// compute probabilistic upper bound
			"	float d = t - moments.x;  \r\n"
			"	float p_max = variance / (variance + d*d);  \r\n"
			//"	float p_max = variance / (variance + pow(d,4));  \r\n"	// 캐릭터는 바닥이 아니라서 Jiggle 거리는 현상이 생길 수 있어 수치를 빠르게 Zero로 바꾸도록 한다.

			"	return max(p, p_max);  \r\n"
			"}  \r\n"

			// @param lightTexCoord The uv to sample in the shadowMap
			// @param radius The radius to blur
			// @param offset The inverse shadow map size -> how much to move when bluring
			//float ShadowContribution(sampler2D shadowMap, float2 lightTexCoord, float distToLight, float radius, float2 offset) 
			"float ShadowContribution(sampler __ShadowTexSampler, float4 vShadowProjectionPos, float distToLight)  \r\n"
			"{  \r\n"
			//------------------------------------------ ShadowMap 
			//transform from RT space to texture space. 
			"	float2 ShadowTexC = vShadowProjectionPos.xy / vShadowProjectionPos.w;  \r\n"
			//"	float2 ShadowTexC = 0.5 * vShadowProjectionPos.xy / vShadowProjectionPos.w + float2( 0.5, 0.5 );  \r\n"
			//"	ShadowTexC.y = 1.0f - ShadowTexC.y;  \r\n"

			// read the moments from the variance shadow map
			"	float2 moments = btex2D_rg( __ShadowTexSampler, ShadowTexC, 2.f, float2(g_vShadow.x,g_vShadow.x) );  \r\n"
			//"	float2 moments = tex2D(__ShadowTexSampler, ShadowTexC).rg;  \r\n"

			// compute the Chebyshev upper bound
			"	float p_max = ChebyshevUpperBound(moments, distToLight);  \r\n"

			"	return min(p_max,1);  \r\n"
			//"	return ReduceLightBleeding(p_max, 0.6);  \r\n"
			"}  \r\n"

			//////////////////////////////////////////////////////////////////////////
			// PCF - Percentage-Closer Filtering
			"float2 Poisson4[] = { \r\n"
			"	{-0.5, -0.5}, \r\n"
			"	{-0.5, 0.5}, \r\n"
			"	{0.5, -0.5}, \r\n"
			"	{0.5, 0.5}, \r\n"
			"}; \r\n"	// 망함

			"float2 Poisson9[] = { \r\n"
			"	{-0.66666,	-0.66666}, \r\n"
			"	{-0.66666,	0.0}, \r\n"
			"	{-0.66666,	0.66666}, \r\n"
			"	{-0.0,		-0.66666}, \r\n"
			"	{-0.0,		0.0}, \r\n"
			"	{-0.0,		0.66666}, \r\n"
			"	{0.66666,	-0.66666}, \r\n"
			"	{0.66666,	0.0}, \r\n"
			"	{0.66666,	0.66666}, \r\n"
			"}; \r\n"	// 망함

			//"float2 Poisson16[] = { \r\n"
			////"	{-0.978698, -0.0884121}, \r\n"
			////"	{-0.841121, 0.521165}, \r\n"
			//"	{-0.71746, -0.50322}, \r\n"
			////"	{-0.702933, 0.903134}, \r\n"
			//"	{-0.663198, 0.15482}, \r\n"
			//"	{-0.495102, -0.232887}, \r\n"
			////"	{-0.364238, -0.961791}, \r\n"
			//"	{-0.345866, -0.564379}, \r\n"
			//"	{-0.325663, 0.64037}, \r\n"
			//"	{-0.182714, 0.321329}, \r\n"
			//"	{-0.142613, -0.0227363}, \r\n"
			//"	{-0.0564287, -0.36729}, \r\n"
			////"	{-0.0185858, 0.918882}, \r\n"
			//"	{0.0381787, -0.728996}, \r\n"
			//"	{0.16599, 0.093112}, \r\n"
			//"	{0.253639, 0.719535}, \r\n"
			////"	{0.369549, -0.655019}, \r\n"
			//"	{0.423627, 0.429975}, \r\n"
			//"	{0.530747, -0.364971}, \r\n"
			////"	{0.566027, -0.940489}, \r\n"
			//"	{0.639332, 0.0284127}, \r\n"
			//"	{0.652089, 0.669668}, \r\n"
			//"	{0.773797, 0.345012}, \r\n"
			////"	{0.968871, 0.840449}, \r\n"
			////"	{0.991882, -0.657338}, \r\n"
			//"}; \r\n"
			"float2 Poisson16[] = { \r\n"
			//"	{-0.978698, -0.0884121}, \r\n"
			//"	{-0.841121, 0.521165}, \r\n"
			"	{-0.71746*2, -0.50322*2}, \r\n"
			//"	{-0.702933, 0.903134}, \r\n"
			"	{-0.663198*2, 0.15482*2}, \r\n"
			"	{-0.495102*2, -0.232887*2}, \r\n"
			//"	{-0.364238, -0.961791}, \r\n"
			"	{-0.345866*2, -0.564379*2}, \r\n"
			"	{-0.325663*2, 0.64037*2}, \r\n"
			"	{-0.182714*2, 0.321329*2}, \r\n"
			"	{-0.142613*2, -0.0227363*2}, \r\n"
			"	{-0.0564287*2, -0.36729*2}, \r\n"
			//"	{-0.0185858, 0.918882}, \r\n"
			"	{0.0381787*2, -0.728996*2}, \r\n"
			"	{0.16599*2, 0.093112*2}, \r\n"
			"	{0.253639*2, 0.719535*2}, \r\n"
			//"	{0.369549, -0.655019}, \r\n"
			"	{0.423627*2, 0.429975*2}, \r\n"
			"	{0.530747*2, -0.364971*2}, \r\n"
			//"	{0.566027, -0.940489}, \r\n"
			"	{0.639332*2, 0.0284127*2}, \r\n"
			"	{0.652089*2, 0.669668*2}, \r\n"
			"	{0.773797*2, 0.345012*2}, \r\n"
			//"	{0.968871, 0.840449}, \r\n"
			//"	{0.991882, -0.657338}, \r\n"
			"}; \r\n"

			"float2 Poisson25[] = { \r\n"
			"	{-0.978698, -0.0884121}, \r\n"
			"	{-0.841121, 0.521165}, \r\n"
			"	{-0.71746, -0.50322}, \r\n"
			"	{-0.702933, 0.903134}, \r\n"
			"	{-0.663198, 0.15482}, \r\n"
			"	{-0.495102, -0.232887}, \r\n"
			"	{-0.364238, -0.961791}, \r\n"
			"	{-0.345866, -0.564379}, \r\n"
			"	{-0.325663, 0.64037}, \r\n"
			"	{-0.182714, 0.321329}, \r\n"
			"	{-0.142613, -0.0227363}, \r\n"
			"	{-0.0564287, -0.36729}, \r\n"
			"	{-0.0185858, 0.918882}, \r\n"
			"	{0.0381787, -0.728996}, \r\n"
			"	{0.16599, 0.093112}, \r\n"
			"	{0.253639, 0.719535}, \r\n"
			"	{0.369549, -0.655019}, \r\n"
			"	{0.423627, 0.429975}, \r\n"
			"	{0.530747, -0.364971}, \r\n"
			"	{0.566027, -0.940489}, \r\n"
			"	{0.639332, 0.0284127}, \r\n"
			"	{0.652089, 0.669668}, \r\n"
			"	{0.773797, 0.345012}, \r\n"
			"	{0.968871, 0.840449}, \r\n"
			"	{0.991882, -0.657338}, \r\n"
			"}; \r\n"

			"float2 Poisson32[] = { \r\n"
			"	{-0.975402, -0.0711386}, \r\n"
			"	{-0.920347, -0.41142}, \r\n"
			"	{-0.883908, 0.217872}, \r\n"
			"	{-0.884518, 0.568041}, \r\n"
			"	{-0.811945, 0.90521}, \r\n"
			"	{-0.792474, -0.779962}, \r\n"
			"	{-0.614856, 0.386578}, \r\n"
			"	{-0.580859, -0.208777}, \r\n"
			"	{-0.53795, 0.716666}, \r\n"
			"	{-0.515427, 0.0899991}, \r\n"
			"	{-0.454634, -0.707938}, \r\n"
			"	{-0.420942, 0.991272}, \r\n"
			"	{-0.261147, 0.588488}, \r\n"
			"	{-0.211219, 0.114841}, \r\n"
			"	{-0.146336, -0.259194}, \r\n"
			"	{-0.139439, -0.888668}, \r\n"
			"	{0.0116886, 0.326395}, \r\n"
			"	{0.0380566, 0.625477}, \r\n"
			"	{0.0625935, -0.50853}, \r\n"
			"	{0.125584, 0.0469069}, \r\n"
			"	{0.169469, -0.997253}, \r\n"
			"	{0.320597, 0.291055}, \r\n"
			"	{0.359172, -0.633717}, \r\n"
			"	{0.435713, -0.250832}, \r\n"
			"	{0.507797, -0.916562}, \r\n"
			"	{0.545763, 0.730216}, \r\n"
			"	{0.56859, 0.11655}, \r\n"
			"	{0.743156, -0.505173}, \r\n"
			"	{0.736442, -0.189734}, \r\n"
			"	{0.843562, 0.357036}, \r\n"
			"	{0.865413, 0.763726}, \r\n"
			"	{0.872005, -0.927}, \r\n"
			"}; \r\n"

			"#define PCF_POISSON_COUNT 16 \r\n"
			"#define PCF_POISSON Poisson16 \r\n"

			// Performs PCF filtering on the shadow map using multiple taps in the filter region.
			"float2 PCF_SoftShadow( sampler __ShadowTexSampler, float4 vShadowProjectionPos, float fTexelOffset ) \r\n"
			"{ \r\n"
			"   //------------------------------------------ ShadowMap \r\n"
			"   //transform from RT space to texture space. \r\n"
			"	float2 ShadowTexC = vShadowProjectionPos.xy / vShadowProjectionPos.w;  \r\n"

			"   float2 vLightAmount = 0; \r\n"

			"   float fShadowProjZdivW = vShadowProjectionPos.z / vShadowProjectionPos.w; \r\n"
			"	for ( int i = 0; i < PCF_POISSON_COUNT; ++i ) \r\n"
			"	{ \r\n"
			"		float2 offset = PCF_POISSON[i] * fTexelOffset; \r\n"
			"		vLightAmount.x += (tex2D( __ShadowTexSampler, ShadowTexC + offset ) + g_vShadow.y < fShadowProjZdivW)? 0.f: 1.0f; \r\n"
			"	} \r\n"
			"	vLightAmount.x /= PCF_POISSON_COUNT; \r\n"

			"   // 끝부분이 어색한게 있어서 부드럽게 처리함. \r\n"
			"   float2 vLowUV = 1.f - saturate( ShadowTexC.xy * 5.f );		// UV 범위 0 ~ 0.2 ~ 1 -> 알파수치 1 ~ 0 ~ 0 로 변환 \r\n"
			"   float2 vHighUV = 1.f - saturate( (1.f-ShadowTexC.xy) * 5.f );	// UV 범위 0 ~ 0.8 ~ 1 -> 알파수치 0 ~ 0 ~ 1 로 변환 \r\n"
			"   vLightAmount.y = vLowUV.x + vLowUV.y + vHighUV.x + vHighUV.y; \r\n"
			"   vLightAmount.y = saturate(vLightAmount.y); \r\n"

			"   return vLightAmount; \r\n"
			"} \r\n"


			//////////////////////////////////////////////////////////////////////////
			// PCSS
//
//#define SEARCH_POISSON_COUNT 25
//#define SEARCH_POISSON Poisson25
//#define PCF_POISSON_COUNT 25
//#define PCF_POISSON Poisson25
//
//			// Using similar triangles from the surface point to the area light
//			float2 SearchRegionRadiusUV(float zWorld)
//			{
//				return g_LightRadiusUV * (zWorld - g_LightZNear) / zWorld;
//			}
//
//			float BiasedZ(float z0, float2 dz_duv, float2 offset)
//			{
//				return z0 + dot(dz_duv, offset);
//			}
//
//			// Returns average blocker depth in the search region, as well as the number of found blockers.
//			// Blockers are defined as shadow-map samples between the surface point and the light.
//			void FindBlocker(out float avgBlockerDepth, 
//								out float numBlockers,
//								Texture2D<float> tDepthMap,
//								float2 uv,
//								float z0,
//								float2 dz_duv,
//								float2 searchRegionRadiusUV)
//			{
//				float blockerSum = 0;
//				numBlockers = 0;
//
////#ifdef USE_POISSON
//				for ( int i = 0; i < SEARCH_POISSON_COUNT; ++i )
//				{
//					float2 offset = SEARCH_POISSON[i] * searchRegionRadiusUV;
//					float shadowMapDepth = tDepthMap.SampleLevel(PointSampler, uv + offset, 0);
//					float z = BiasedZ(z0, dz_duv, offset);
//					if ( shadowMapDepth < z )
//					{
//						blockerSum += shadowMapDepth;
//						numBlockers++;
//					}
//				}
////#else
////				float2 stepUV = searchRegionRadiusUV / BLOCKER_SEARCH_STEP_COUNT;
////				for( float x = -BLOCKER_SEARCH_STEP_COUNT; x <= BLOCKER_SEARCH_STEP_COUNT; ++x )
////					for( float y = -BLOCKER_SEARCH_STEP_COUNT; y <= BLOCKER_SEARCH_STEP_COUNT; ++y )
////					{
////						float2 offset = float2( x, y ) * stepUV;
////						float shadowMapDepth = tDepthMap.SampleLevel(PointSampler, uv + offset, 0);
////						float z = BiasedZ(z0, dz_duv, offset);
////						if ( shadowMapDepth < z )
////						{
////							blockerSum += shadowMapDepth;
////							numBlockers++;
////						}
////					}
////#endif
//					avgBlockerDepth = blockerSum / numBlockers;
//			}
//
//			float ZClipToZEye(float zClip)
//			{
//				return g_LightZFar*g_LightZNear / (g_LightZFar - zClip*(g_LightZFar-g_LightZNear));   
//			}
//
//			// Using similar triangles between the area light, the blocking plane and the surface point
//			float2 PenumbraRadiusUV(float zReceiver, float zBlocker)
//			{
//				return g_LightRadiusUV * (zReceiver - zBlocker) / zBlocker;
//			}
//
//			// Project UV size to the near plane of the light
//			float2 ProjectToLightUV(float2 sizeUV, float zWorld)
//			{
//				return sizeUV * g_LightZNear / zWorld;
//			}
//
//			// Performs PCF filtering on the shadow map using multiple taps in the filter region.
//			float PCF_Filter( float2 uv, float z0, float2 dz_duv, float2 filterRadiusUV )
//			{
//				float sum = 0;
//
////#ifdef USE_POISSON
//				for ( int i = 0; i < PCF_POISSON_COUNT; ++i )
//				{
//					float2 offset = PCF_POISSON[i] * filterRadiusUV;
//					float z = BiasedZ(z0, dz_duv, offset);
//					sum += tDepthMap.SampleCmpLevelZero(PCF_Sampler, uv + offset, z);
//				}
//				return sum / PCF_POISSON_COUNT;
////#else
////				float2 stepUV = filterRadiusUV / PCF_FILTER_STEP_COUNT;
////				for( float x = -PCF_FILTER_STEP_COUNT; x <= PCF_FILTER_STEP_COUNT; ++x )
////					for( float y = -PCF_FILTER_STEP_COUNT; y <= PCF_FILTER_STEP_COUNT; ++y )
////					{
////						float2 offset = float2( x, y ) * stepUV;
////						float z = BiasedZ(z0, dz_duv, offset);
////						sum += tDepthMap.SampleCmpLevelZero(PCF_Sampler, uv + offset, z);
////					}
////					float numSamples = (PCF_FILTER_STEP_COUNT*2+1);
////					return sum / (numSamples*numSamples);
////#endif
//			}
//
//			float PCSS_Shadow(float2 uv, float z, float2 dz_duv, float zEye)
//			{
//				// ------------------------
//				// STEP 1: blocker search
//				// ------------------------
//				float avgBlockerDepth = 0;
//				float numBlockers = 0;
//				float2 searchRegionRadiusUV = SearchRegionRadiusUV(zEye);
//				FindBlocker( avgBlockerDepth, numBlockers, tDepthMap, uv, z, dz_duv, searchRegionRadiusUV );
//
//				// Early out if no blocker found
//				if (numBlockers == 0) return 1.0;
//
//				// ------------------------
//				// STEP 2: penumbra size
//				// ------------------------
//				float avgBlockerDepthWorld = ZClipToZEye(avgBlockerDepth);
//				float2 penumbraRadiusUV = PenumbraRadiusUV(zEye, avgBlockerDepthWorld);
//				float2 filterRadiusUV = ProjectToLightUV(penumbraRadiusUV, zEye);
//
//				// ------------------------
//				// STEP 3: filtering
//				// ------------------------
//				return PCF_Filter(uv, z, dz_duv, filterRadiusUV);
//			}
//
//			// Derivatives of light-space depth with respect to texture coordinates
//			float2 DepthGradient(float2 uv, float z)
//			{
//				float2 dz_duv = 0;
//
//				float3 duvdist_dx = ddx(float3(uv,z));
//				float3 duvdist_dy = ddy(float3(uv,z));
//
//				dz_duv.x = duvdist_dy.y * duvdist_dx.z;
//				dz_duv.x -= duvdist_dx.y * duvdist_dy.z;
//
//				dz_duv.y = duvdist_dx.x * duvdist_dy.z;
//				dz_duv.y -= duvdist_dy.x * duvdist_dx.z;
//
//				float det = (duvdist_dx.x * duvdist_dy.y) - (duvdist_dx.y * duvdist_dy.x);
//				dz_duv /= det;
//
//				return dz_duv;
//			}
//
//			float EyeRenderPS ( float3 vPosWorldSpace, float4 vShadowProjectionPos )
//			{
//				float2 uv = vShadowProjectionPos.xy / vShadowProjectionPos.w;
//				float z = vShadowProjectionPos.z / vShadowProjectionPos.w;
//
//				// Compute gradient using ddx/ddy before any branching
//				float2 dz_duv = DepthGradient(uv, z);
//
//				// Eye-space z from the light's point of view
//				float zEye = mul(float4(vPosWorldSpace,1.f), g_matShadow_View).z;
//				//if (usePCSS)
//					return PCSS_Shadow(uv, z, dz_duv, zEye);
//				//else
//				//	return color * PCF_Shadow(uv, z, dz_duv, zEye);
//			}

			//////////////////////////////////////////////////////////////////////////
			//			PS_2
			"float4 PS_2( VS_OUTPUT In, uniform bool bVSM ) : COLOR0  \r\n"
			"{ \r\n"

			// Texture
			"	float4 vColor; \r\n"
			"	float4 vAlbedo = tex2D( BaseTexSampler, In.m_vTex ); \r\n"
			"	vColor.w = vAlbedo.w;  \r\n"

			// 정말 값이 Zero 는 안되도록 만듬.
			"   vAlbedo.xyz += 0.001f;  \r\n"

			// Normalize 
			"	float3 vNor = normalize(In.m_vNor); \r\n"

			// Mask
			"	float4 vMask = tex2D( MaskTexSampler, In.m_vTex ); \r\n"
			"	float4 vMask2 = tex2D( Mask2TexSampler, In.m_vTex ); \r\n"
			
			// Lambert
			"	float wrap = 0.f; \r\n"
			//"	if ( !bVSM ) \r\n"
			//"		wrap = 0.2f; \r\n"

			"	float fNDotL = dot(vNor, g_vlhtDirViewSpace.xyz); \r\n"
			//"	float fNdotL_wrap = fNDotL; \r\n"
			"	float fNdotL_wrap = max(0.0f, fNDotL); \r\n"
			//"	float fNDotL_Ambient = (fNDotL+3.f)*0.4f; \r\n"		// (-1~1) -> (2~4) -> (0.8~1.6)

			//"	if ( !bVSM ) \r\n"
			//"		fNdotL_wrap = (fNDotL + wrap) / (1 + wrap); \r\n"		// wrap lighting

			// Shade (Ambient + etc.)
			//"	float3 vDiffuse = fNdotL_wrap * g_vLightDiffuseSaturation.xyz ; \r\n"
			"	float3 vAmbient = g_vLightAmbient.xyz; \r\n"

			// Normal 과 Camera. Dot
			"	float3 vCameraDir = float3(0.f,0.f,-1.f);  \r\n"
			"	float fNdotC = dot(vNor,vCameraDir); \r\n"
			"	float fFresnel = (1.f-saturate(fNdotC)); \r\n"

			"	float4 vShadowProjectionPos = mul(g_matShadow_Clip2Tex, float4(In.m_vPosWorldSpace,1.f)); \r\n"

			// 채도값을 구한다.
			"	float fSaturation = (0.2125*vAlbedo.x)+(0.7154*vAlbedo.y)+(0.0721*vAlbedo.z); \r\n"
			//"	float fSaturation = (0.2729*vAlbedo.x)+(0.5244*vAlbedo.y)+(0.2027*vAlbedo.z); \r\n"
			"	float fSaturationINV = 1.f-fSaturation; \r\n"

			// Diffuse + DiffuseDynamic
			// ( (fSaturationINV*fSaturationINV*4.f) + 1.f ) :	텍스쳐가 어두울수록 Diffuse 강도가 더 밝게 보일수 있게 작업함.
			// g_vLightDiffuseSaturation.xyz    : 색 채도 변형없이 Diffuse 색을 높인다.
			// g_vLightDiffuseDynamic.xyz		: 색 채도에 변형을 줌. Virtual Specular 임.
			"	float fSaturationInvColorUp = (fSaturationINV*fSaturationINV*4.f) + 1.f; \r\n"	//	(1~5)
			"	float3 vDiffuse = g_vLightDiffuseSaturation.xyz + (g_vLightDiffuseDynamic.xyz*fSaturationInvColorUp) ; \r\n"
			"	vDiffuse *= fNdotL_wrap ; \r\n"

			"	float fLightAmount = 1.f; \r\n"
			"	if ( bVSM ) \r\n"
			"		fLightAmount = PCF_SoftShadow( ShadowTexSampler_PCF, vShadowProjectionPos, g_vShadow.x );  \r\n"
			//"		fLightAmount = ShadowContribution( ShadowTexSampler_VSM, vShadowProjectionPos, length(g_vLightPosWorldSpace-In.m_vPosWorldSpace));  \r\n"

			// Diffuse + Ambient 를 더한다.
			//"	vColor.xyz = (vDiffuse*fLightAmount) + (vAmbient); \r\n"
			// min( (1.0f-fNdotL_wrap)+0.f+fLightAmount*1.f, 1.f ) Diffuse 받는 값이 클수록 그 옆의 그림자를 더욱 어둡게 만든다.
			// min( (1.0f-fNdotL_wrap) :			빛 많이 받는 부분일 수록 0, 빛을 안받으면 1로 셋팅.
			// fSaturation :						텍스쳐가 밝을 수록 그림자 음영도 정상으로 나오도록 함.
			// (fLightAmount*(1.f-fSaturation)) :	그림자 받는 부분을 0 으로 셋팅. 텍스쳐가 어두울수록 음영을 더욱 어둡게 만듬.
			//
			//"	vColor.xyz = vAmbient * min( (1.0f-fNdotL_wrap)+(fSaturation)+(fLightAmount*fSaturationINV), 1.f ); \r\n"
			//"	vColor.xyz = vAmbient * fSaturation; \r\n"
			//"	vColor.xyz = vAmbient*fNDotL_Ambient; \r\n"
			"	vColor.xyz = vAmbient; \r\n"

			//"	vColor.xyz = vMask.r*g_vLightAmbientORG + (1.f-vMask.r)*vAmbient; \r\n"

			//"	vColor.xyz *= 0.5f + (fLightAmount*0.5f); \r\n"
			//"	vColor.xyz = (vAmbient); \r\n"

			//"	vColor.xyz = (vAmbient); \r\n"
			//"	vColor.xyz += (vDiffuse*fLightAmount); \r\n"
			//"	vColor.xyz = (vDiffuse) + (vAmbient); \r\n"

			// USE_LEVEL
			"	float fLevelBlend = g_vLightLevel.w * vMask2.r; \r\n"
			"   #ifdef USE_LEVEL \r\n"
			"	{ \r\n"
			"		vColor.xyz = (g_vLightLevel.xyz*fLevelBlend) + (vColor.xyz)*(1.f-fLevelBlend);  // Level적용 \r\n"
			"	} \r\n"
			"	#endif \r\n"

			// Level 때문에 그림자가 안그려지는 문제가 없도록 한다.
			// ( (fSaturationINV*fSaturationINV*4.f) + 1.f ) :	텍스쳐가 어두울수록 Diffuse 강도가 더 밝게 보일수 있게 작업함.
			//"	float fSaturationInvColorUp = (fSaturationINV*fSaturationINV*4.f) + 1.f; \r\n"	//	(1~5)
			//"	vColor.xyz += vDiffuse * fLightAmount * fSaturationInvColorUp; \r\n"

			// 텍스쳐가 어두울수록 Diffuse 강도를 밝게 함으로써 그래픽 디자이너의 결과물 퀄리티가 떨어지게 되어서 fSaturationInvColorUp 값을 뺌.
			//
			// 그림자에 빛이 영향 받도록 한다.
			"	vColor.xyz += vDiffuse * fLightAmount; \r\n"

			// SSS 용 Fresnel
			"	float fFresnelSSS = pow( fFresnel, g_fFresnelForSSS ); \r\n"

			// 역광.
			// 역광이 g_vLightDiffuseSaturation.xyz 값과 연동되어서 특정 각도에서 밝게 되는 느낌을 많이 준다.
			// fSaturation		: 채도가 어두울 수록 역광을 밝게 해줌.
			// fFresnelSSS		: fFresnel 에 power 값을 따로 계산할 필요는 없을꺼 같아서 fFresnelSSS 값을 그냥 사용함.
			// (1.f - vMask.r)	: 피부쪽 역광은 따로 계산된다.
			//"	vColor.xyz *= 1.f + (vAmbient * ((1.f-vMask.r) * fFresnelSSS / fSaturation) * 0.4f); \r\n
			"	vColor.xyz *= 1.f + ((1.f-vMask.r) * fFresnelSSS / fSaturation) * g_fInverseLightIntensity; \r\n"

			// Specular 를 위한 벡터
			"	float fNdotS = dot(vNor,g_vSpecularDirViewSpace); \r\n"

			// Skin 이외의 Specular
			// 스펙큘러를 따로 더 하도록 한다.
			"	float3 vSpecular = pow(fNdotS,vMask2.b*100.f) * vMask2.g; \r\n"		// pow(vMask2.b*10.f,2)게 하면 작은값을 세세히 컨트롤 가능. 
			"	vColor.xyz += vSpecular; \r\n"

			// Albedo 와 Alpha 를 곱한다.
			// Diffuse Color
			"	vColor.xyz *= vAlbedo.xyz; \r\n"
			//"	vColor.xyz += g_vFakeSpecular * 2.f * vAlbedo.xyz * (0.7f*3/(vAlbedo.x+vAlbedo.y+vAlbedo.z)) * vDiffuse * fLightAmount; \r\n"		// FakeSpecular
			"	vColor.w = vAlbedo.w * g_fAlpha; \r\n"

			// USE_LEVEL_FLOW
			"   #ifdef USE_LEVEL_FLOW \r\n"
			"	{ \r\n"
			"		float3 vLevelFlow = tex2D( LevelFlowTexSampler, float2(vNor.x, vNor.y) ); \r\n"
			"		vLevelFlow *= g_vLevelFlowColor.xyz; \r\n"
			"		vColor.xyz += vLevelFlow * vMask2.r; \r\n"
			"	} \r\n"
			"	#endif \r\n"

			// USE_MULTI
			"   #ifdef USE_MULTI \r\n"
			"	{ \r\n"
			"		float3 vMulti = tex2D( MultiTexSampler, In.m_vTex ); \r\n"

			"		#ifdef USE_LEVEL \r\n"
			"		{ \r\n"
			"			vMulti *= (1.f-fLevelBlend); \r\n"
			"		} \r\n"
			"		#endif \r\n"

			"		if ( g_nMulti_ADD == 0 ) \r\n"
			"			vColor.xyz += vMulti*vMulti; \r\n"	// 원본은 이렇게 되어있다. 기존에 이렇게 되어 있어서 어쩔수가 없다.
			"		else if ( g_nMulti_ADD = 2 ) \r\n"
			"			vColor.xyz += vMulti; \r\n"	// 원본은 이렇게 되어있다. 기존에 이렇게 되어 있어서 어쩔수가 없다.
			"		else \r\n"
			"			vColor.xyz *= vMulti; \r\n"
			"	} \r\n"
			"	#endif \r\n"

			// USE_CUBEMAP
			"   #ifdef USE_CUBEMAP \r\n"
			"	{ \r\n"
					// pow 두번째 인자에 0 이 나오면 결과값은 0 이 아니다.
			"		float fCubeBlend = pow( fFresnel, vMask.b*4.f ) * vMask.g; \r\n"
			"		vColor.xyz = vColor.xyz*(1.f-fCubeBlend) + texCUBE( CubeTexSampler, In.m_vCubeTexcoord ).xyz * fCubeBlend * g_fCubeMapPowerForOldMap; \r\n"
			"	} \r\n"
			"	#endif \r\n"

			// USE_TEXUPDOWN
			"   #ifdef USE_TEXUPDOWN \r\n"
			"	{ \r\n"
			"		vColor.xyz += tex2D( TexUpDownSampler, In.m_vTex ) * g_fTexDiffColor; \r\n"
			"	} \r\n"
			"	#endif \r\n"

			// Fake SSS
			"	float3 vSSS = g_vSSS_Color + vMask2.g; \r\n"		// Spec Intesity 값을 바탕으로 SSS Texture 를 만든다.
			"	vSSS *= vMask.r * fFresnelSSS; \r\n"												// SSS Texture 를 Masking 하고, Fresnel 값도 연결한다.
			"	vColor.xyz += vSSS; \r\n"

			// 역광
			//"	vColor.xyz += (vAlbedo.xyz*0.3f+0.1f) * (1.f-vMask.r) * fFresnelSSS * 0.3f; \r\n"
			//"	vColor.xyz += vAlbedo.xyz * ((1.f-vMask.r) * fFresnelSSS / fSaturation) * 0.3f; \r\n"

			////  Fake Subsurface Scattering
			//"	float scatterWidth = 0.3f; \r\n"
			//"	float scatter = smoothstep(0.0, scatterWidth, fNdotL_wrap) * smoothstep(scatterWidth * 2.0, scatterWidth, fNdotL_wrap); \r\n"
			//"	scatter *= vMask.r; \r\n"
			//"	vColor.x += g_vSSS_Color * scatter * g_vLightDiffuseSaturation.x; \r\n"

			// Skin 쪽 Specular
			"	vColor.xyz += pow(fNdotS,g_vSpecColor_Power.w) * vMask.r * g_vSpecColor_Power.xyz * g_vLightDiffuseSaturation.w; \r\n"

			// SM3.0 에서는 Fog를 직접해줘야한다.
			"	if ( bVSM ) \r\n"
			"		vColor.xyz = (vColor.xyz*In.m_fFog) + g_vFogColor*(1.f-In.m_fFog); \r\n"

			// 마무리
			"	return vColor; \r\n"
			"} \r\n"


			//strName[0] = _T("USE_LEVEL");
			//strName[1] = _T("USE_LEVEL_FLOW");
			//strName[2] = _T("USE_CUBEMAP");
			//strName[3] = _T("USE_MULTI");
			//strName[4] = _T("USE_TEXUPDOWN");

			//////////////////////////////////////////////////////////////////////////
			//	
			//////////////////////////////////////////////////////////////////////////
			"VertexShader vsArray_2[4] = { compile vs_2_0 VShadeVS(1), \r\n"
										"compile vs_2_0 VShadeVS(2),\r\n"
										"compile vs_2_0 VShadeVS(3),\r\n"
										"compile vs_2_0 VShadeVS(4) }; \r\n"

			"VertexShader vsArray[4] = { compile vs_3_0 VShadeVS(1), \r\n"
										"compile vs_3_0 VShadeVS(2),\r\n"
										"compile vs_3_0 VShadeVS(3),\r\n"
										"compile vs_3_0 VShadeVS(4) }; \r\n"


			//////////////////////////////////////
			// Techniques specs follow
			//////////////////////////////////////
			"technique runtime_Skin\r\n"
			"{\r\n"
			"	pass base \r\n"		// 2.0
			"	{\r\n"
			"		VertexShader = (vsArray_2[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_2_0 PS_2( false );\r\n"
			"	}\r\n"
			"	pass SelfShadowVSM_CSM \r\n"		// 3.0
			"	{\r\n"
			"		VertexShader = (vsArray[g_nCurNumBones]);\r\n"
			"		PixelShader = compile ps_3_0 PS_2( true );\r\n"
			"	}\r\n"
			"}\r\n"
			"technique runtime_Object\r\n"
			"{\r\n"
			"	pass base \r\n"	// 2.0
			"	{\r\n"
			"		VertexShader = compile vs_2_0 VS_OBJECT();\r\n"
			"		PixelShader = compile ps_2_0 PS_2( false );\r\n"
			"	}\r\n"
			"	pass SelfShadowVSM_CSM \r\n"	// 3.0
			"	{\r\n"
			"		VertexShader = compile vs_3_0 VS_OBJECT();\r\n"
			"		PixelShader = compile ps_3_0 PS_2( true );\r\n"
			"	}\r\n"
			"}\r\n"
			"\r\n";
	}
};