#include "stdafx.h"

#include "DxVMCommandFX.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


void DxVMCommandFX::SetStringFX_SkinVS( BOOL bMirror )
{
    //----------------------------------------------------------------------------- m_strDefindFX
    m_strDefindFX =
    "//-------------------------------------------------------------------------------------- \r\n"
    "// Global variables \r\n"
    "//-------------------------------------------------------------------------------------- \r\n"
    "float4x4 g_matVP; \r\n"
    "float4   g_vLightDiffuse; \r\n"
    "float3   g_vLightAmbient; \r\n"
    "float3   g_vLightSpecular; \r\n"
    "float4   g_vLightDir; \r\n"
    "float3   g_vCameraFrom; \r\n"
    "float3   g_vWorldNormal; \r\n"
    "float3   g_vLightLevel; \r\n"       // rgb - LightLevel
    "float3   g_vLevelFlowColor; \r\n"   // rgb - LevelFlowColor
    "float4   g_vUserColor1; \r\n"
    "float4   g_vUserColor2; \r\n"
    "int      g_nPointLightNum; \r\n"
    "float4   g_vPos_Range_PL[4]; \r\n"
    "float4   g_vDiff_PL[4]; \r\n"
    "int      g_nCurNumBones; \r\n"
	"float2   g_vFog; \r\n"
    " \r\n"
    "static const int MAX_MATRICES = 60;\r\n"
    "float4x3 g_matWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\r\n";

    //----------------------------------------------------------------------------- m_strBaseFX
    m_strBaseFX = 
    "//-------------------------------------------------------------------------------------- \r\n"
    "// Basic vertex transformation \r\n"
    "//-------------------------------------------------------------------------------------- \r\n"
    "struct VS_INPUT { \r\n"
    "    float4 Position        : POSITION; \r\n"
    "    float4 NormalTangent   : TEXCOORD0;	// NormalTangent.xyz - Normal.xyz   // NormalTangent.w  - Tangent.x \r\n"
    "    float4 TangentTexCoord : TEXCOORD1;    // TangentTexCoord.xy  - Tangen.yz  // TangentTexCoord.zw - TexCoord.xy \r\n"
    "    float4 BlendWeights    : BLENDWEIGHT;  \r\n"
	"    float4 BlendIndices    : BLENDINDICES; \r\n"	
    "}; \r\n"
    " \r\n"
    "struct VS_OUTPUT  \r\n"
    "{ \r\n"
    "    float4 Position    : POSITION; \r\n"
    "    float4 vColor0     : COLOR0; \r\n"
    "    float2 TexCoord    : TEXCOORD0; \r\n"
    "    float3 Normal      : TEXCOORD1; \r\n"
    "    float3 Tangent     : TEXCOORD2; \r\n"
    "    float3 Binormal    : TEXCOORD3; \r\n"
    "    float3 vHalfVector : TEXCOORD4; \r\n"
    "    float3 vEyeVector  : TEXCOORD5; \r\n"
	"    float  fFog        : FOG; \r\n"
    "}; \r\n"
    " \r\n";



    //----------------------------------------------------------------------------- m_strVertexShaderFX
    m_strVertexShaderFX = 
    "VS_OUTPUT VS( VS_INPUT In, uniform int NumBones )  \r\n"
    "{ \r\n"
    "    VS_OUTPUT Out; \r\n"
    " \r\n"
    "    float3 vPos = 0.0f; \r\n"
    "    float3 vNormal = 0.0f; \r\n"
    "    float3 vTangent = 0.0f; \r\n"
    "    float3 vNormal_IN = In.NormalTangent.xyz/32767.f; \r\n"
	"    float3 vTangent_IN = float3( In.NormalTangent.w, In.TangentTexCoord.x, In.TangentTexCoord.y ); \r\n"
    "    vTangent_IN = vTangent_IN.xyz/32767.f * g_vLightDir.w; \r\n"
	"    float  fLastWeight = 0.0f; \r\n"
    " \r\n"
	"    float BlendWeightsArray[4] = (float[4])In.BlendWeights; \r\n"
	"    int   IndexArray[4]        = (int[4])In.BlendIndices; \r\n"
    "    for (int i = 0; i < 4; ++i) \r\n"
	"    { \r\n"
    "        BlendWeightsArray[i] = BlendWeightsArray[i]/32767.f; \r\n"
    "    } \r\n"
    " \r\n"
	"    for (int iBone = 0; iBone < NumBones-1; iBone++) \r\n"
	"    { \r\n"
	"        fLastWeight = fLastWeight + BlendWeightsArray[iBone]; \r\n"
	"        vPos += mul(In.Position, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone]; \r\n"
	"        vNormal += mul(vNormal_IN, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone]; \r\n"
	"        vTangent += mul(vTangent_IN, g_matWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone]; \r\n"
	"    } \r\n"
	"    fLastWeight = 1.0f - fLastWeight; \r\n"
    " \r\n"
	"    vPos += (mul(In.Position, g_matWorldMatrixArray[IndexArray[NumBones-1]]) * fLastWeight); \r\n"
	"    vNormal += (mul(vNormal_IN, g_matWorldMatrixArray[IndexArray[NumBones-1]]) * fLastWeight); \r\n"
	"    vTangent += (mul(vTangent_IN, g_matWorldMatrixArray[IndexArray[NumBones-1]]) * fLastWeight); \r\n"
	" \r\n"
	"    Out.Position = mul(float4(vPos.xyz, 1.0f), g_matVP); \r\n"
    " \r\n"
    "    // Set up the tangent frame in world space \r\n"
    "    Out.Normal = normalize( vNormal ); \r\n"
    "    Out.Tangent = normalize( vTangent ); \r\n"
    "    Out.Binormal = cross( Out.Normal, Out.Tangent ); \r\n"
    " \r\n";
    if ( bMirror )
    {
        m_strVertexShaderFX +=
        "    if( In.TangentTexCoord.z < 0.f )\r\n"
	    "    {\r\n"
	    "        Out.Binormal = -Out.Binormal; \r\n"
	    "    }\r\n"
        " \r\n";
    }
    m_strVertexShaderFX +=
    "    // Pass the texture coordinate \r\n"
    "    Out.TexCoord.xy = In.TangentTexCoord.zw/32767.f; \r\n"
    " \r\n"
    "    float3 vPointColor = 0.f; \r\n"
    "    for ( int i=0; i<g_nPointLightNum; ++i ) \r\n"
    "    { \r\n"
    "        float3 afPosDir = g_vPos_Range_PL[i].xyz - vPos; \r\n"
    "        float fLength = length( afPosDir ); \r\n"
    "        fLength = (g_vPos_Range_PL[i].w-fLength)/g_vPos_Range_PL[i].w; \r\n"
    "        fLength = max( fLength, 0 ); \r\n"
    "        afPosDir = normalize( afPosDir ); \r\n"
    "        float fDotPL = saturate( dot( afPosDir, Out.Normal ) ); \r\n"
    "        vPointColor += g_vDiff_PL[i].xyz * fLength * fDotPL; \r\n"
    "    } \r\n"
    "    Out.vColor0.xyz = vPointColor; \r\n"
    "    Out.vColor0.w = 1.f; \r\n"
    " \r\n"
    "    // HalfVector \r\n"
    "    float3 vHalf = g_vCameraFrom - vPos; \r\n"
    "    vHalf = normalize( vHalf ); \r\n"
    "    Out.vEyeVector = vHalf; \r\n"
    "    vHalf += g_vLightDir; \r\n"
    "    Out.vHalfVector = normalize( vHalf ); \r\n"
	" \r\n"
	"    Out.fFog = saturate((g_vFog.x - Out.Position.z) / g_vFog.y); \r\n"
    " \r\n"
    "    return Out; \r\n"
    "} \r\n"
    "\r\n";



    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[0]
    m_strTechniqueFX[0] = 
    " VertexShader g_vsArray[4] = { compile vs_2_0 VS(1), \r\n"
	"                               compile vs_2_0 VS(2), \r\n"
	"                               compile vs_2_0 VS(3), \r\n"
    "                               compile vs_2_0 VS(4) }; \r\n"
    " \r\n"
    "///////////////////////////////////////////////////////////////////////  \r\n"
    "//	Techniques \r\n"
    "technique VisualMaterial \r\n"
    "{ \r\n";


    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[1]
    m_strTechniqueFX[1] = 
    "} \r\n";



    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[0]
    m_strTechniqueBaseFX[0] = 
    "    pass P0 \r\n"
    "    {"
    " \r\n";


    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[1]
    m_strTechniqueBaseFX[1] = 
    " \r\n"
    "        VertexShader = (g_vsArray[g_nCurNumBones]); \r\n"
    "        PixelShader  = compile ps_2_0 PS(); \r\n"
    "    } \r\n";



    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[0]
    m_strTechniqueLevelFX[0] = 
    "    pass P1 \r\n"
    "    {"
    " \r\n";


    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[1]
    m_strTechniqueLevelFX[1] = 
    " \r\n"
    "        VertexShader = (g_vsArray[g_nCurNumBones]); \r\n"
    "        PixelShader  = compile ps_2_0 PS_Level(); \r\n"
    "    } \r\n";



    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[0]
    m_strTechniqueFlowFX[0] = 
    "    pass P2 \r\n"
    "    {"
    " \r\n";


    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[1]
    m_strTechniqueFlowFX[1] = 
    " \r\n"
    "        VertexShader = (g_vsArray[g_nCurNumBones]); \r\n"
    "        PixelShader  = compile ps_2_0 PS_Flow(); \r\n"
    "    } \r\n";
}

void DxVMCommandFX::MakeStringFX_SkinVS( const COMMENT_FX& sCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM )
{
    m_strFX_SKIN_VS.clear();

    std::string strTexDefineFX;
    std::string strTechOpacityFX;
    std::string strTwoSidedFX;
    std::string strShininessPowerTextureFX;
    std::string strNormalFX_End;
    MakeStringFX_Common( sCommentFX, nTextureNUM, nCubeTextureNUM, nPixelScalarNUM, nPixelVectorNUM,
                        strTexDefineFX, strTechOpacityFX, strTwoSidedFX, strShininessPowerTextureFX, strNormalFX_End );

	// Opacity
	// 캐릭터에서는 MASKED 가 쓰인 텍스쳐인 경우, 
	// 캐릭터 반투명효과가 들어갈 시 문제점이 있어서 외부에서 ALPHATESTENABLE=TRUE; 셋팅을 해준다.
	if ( !sCommentFX.m_strOpacityFX.empty() && sCommentFX.m_emBlendMode==VM_BLENDMODE_MASKED )
	{
		strTechOpacityFX = "        ALPHAFUNC = GREATEREQUAL; \r\n";
		strTechOpacityFX += sCommentFX.m_strAlphaREFFX;
	}

    // 합성
    m_strFX_SKIN_VS = m_strDefindFX +
                strTexDefineFX +
                m_strBaseFX +
                m_strVertexShaderFX +
                m_strReflectionVectorFX +
                m_strDiffuseFX[0] + sCommentFX.m_strDiffuseFX + m_strDiffuseFX[1] + 
                m_strUserColorFX[0] + sCommentFX.m_strUserColor1FX + m_strDiffuseFX[1] + 
                m_strUserColorFX[1] + sCommentFX.m_strUserColor2FX + m_strDiffuseFX[1] + 
                m_strAddUserColorFX +
                m_strDiffusePowerFX + sCommentFX.m_strDiffusePowerFX + m_strPowerEndFX + 
                m_strAmbientPowerFX + sCommentFX.m_strAmbientPowerFX + m_strPowerEndFX + 
                m_strSpecularPowerFX + sCommentFX.m_strSpecularPowerFX + m_strPowerEndFX + 
                m_strShininessPowerFX + sCommentFX.m_strShininessPowerFX + m_strPowerEndFX + 
                m_strLevelPowerFX + sCommentFX.m_strLevelPowerFX + m_strPowerEndFX + 
                m_strLevelFlowPowerFX + sCommentFX.m_strLevelFlowPowerFX + m_strPowerEndFX + 
                m_strShininessFX[0] + sCommentFX.m_strShininessFX + strShininessPowerTextureFX + m_strShininessFX[1] +
                m_strSpecularFX[0] + sCommentFX.m_strSpecularFX + m_strSpecularFX[1] +
                m_strEmissiveFX[0] + sCommentFX.m_strEmissiveFX + m_strEmissiveFX[1] +
                m_strOpacityFX[0] + sCommentFX.m_strOpacityFX + m_strOpacityFX[1] +
                m_strNormalFX[0] + sCommentFX.m_strNormalFX + strNormalFX_End +
                m_strLevelFX[0] + sCommentFX.m_strLevelFX + m_strLevelFX[1] +
                m_strPixelShaderFX +
                m_strPixelShaderLevelFX +
                m_strPixelShaderFlowFX +
                m_strTechniqueFX[0] + 
                m_strTechniqueBaseFX[0] +
                strTechOpacityFX + sCommentFX.m_strWireframeFX + strTwoSidedFX + sCommentFX.m_strDisableDepthTest +
                m_strTechniqueBaseFX[1] +
                m_strTechniqueLevelFX[0] +
                strTechOpacityFX + sCommentFX.m_strWireframeFX + strTwoSidedFX + sCommentFX.m_strDisableDepthTest +
                m_strTechniqueLevelFX[1] +
                m_strTechniqueFlowFX[0] +
                strTechOpacityFX + sCommentFX.m_strWireframeFX + strTwoSidedFX + sCommentFX.m_strDisableDepthTest +
                m_strTechniqueFlowFX[1] +
                m_strTechniqueFX[1];
}