#include "stdafx.h"

#include "DxVMCommandFX.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


void DxVMCommandFX::SetStringFX_PieceColor()
{
    //----------------------------------------------------------------------------- m_strDefindFX
    m_strDefindFX =
    "//-------------------------------------------------------------------------------------- \r\n"
    "// Global variables \r\n"
    "//-------------------------------------------------------------------------------------- \r\n"
    "float4x4 g_matWorld; \r\n"
    "float4x4 g_matWVP; \r\n"
    "float4   g_vLightDiffuse; \r\n"
    "float3   g_vLightAmbient; \r\n"
    "float3   g_vLightSpecular; \r\n"
    "float4   g_vLightDir; \r\n"
    "float2   g_vFog; \r\n"
    "float3   g_vCameraFrom; \r\n"
    "float3   g_vWorldNormal; \r\n"
    "int      g_nPointLightNum; \r\n"
    "float4   g_vPos_Range_PL[7]; \r\n"
    "float4   g_vDiff_PL[7]; \r\n"
    "float4   g_vAtt_PL[7]; \r\n";

    //----------------------------------------------------------------------------- m_strBaseFX
    m_strBaseFX = 
    "//-------------------------------------------------------------------------------------- \r\n"
    "// Basic vertex transformation \r\n"
    "//-------------------------------------------------------------------------------------- \r\n"
    "struct VS_INPUT { \r\n"
    "    float4 Position        : POSITION; \r\n"
    "    float4 vColor          : COLOR0; \r\n"
    "    float4 NormalTangent   : TEXCOORD0;	// NormalTangent.xyz - Normal.xyz  // NormalTangent.w  - Tangent.x \r\n"
    "    float2 TangentYZ       : TEXCOORD1;    // NormalTangent.xy  - Tangen.yz \r\n"
    "    float2 TexCoord        : TEXCOORD2;    // UV \r\n"
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
    " \r\n"
    "struct VS_OUTPUT_REAL_PL  \r\n"
    "{ \r\n"
    "    float4 Position    : POSITION; \r\n"
    "    float4 vColor1     : COLOR0; \r\n"
    "    float2 TexCoord    : TEXCOORD0; \r\n"
    "    float3 Normal      : TEXCOORD1; \r\n"
    "    float3 Tangent     : TEXCOORD2; \r\n"
    "    float3 Binormal    : TEXCOORD3; \r\n"
    "    float3 vHalfVector : TEXCOORD4; \r\n"
    "    float3 vEyeVector  : TEXCOORD5; \r\n"
    "    float3 vColor      : TEXCOORD6; \r\n"
    "    float  fFog        : FOG; \r\n"
    "}; \r\n"
    " \r\n";



    //----------------------------------------------------------------------------- m_strVertexShaderFX
    m_strVertexShaderFX = 
	"//---------------------------------------------------------------------------------------------------------- \r\n"
	"//					V	S    \r\n"
    "VS_OUTPUT VS( VS_INPUT In )  \r\n"
    "{ \r\n"
    "    VS_OUTPUT Out; \r\n"
    "     \r\n"
    "    // Now that we have the calculated weight, add in the final influence \r\n"
    "    Out.Position = mul(g_matWVP, In.Position); \r\n"
    "    Out.Normal.xyz = mul(g_matWorld, In.NormalTangent.xyz/32767.f); \r\n"
    "    Out.Normal = normalize( Out.Normal ) * g_vLightDir.w; \r\n"
    " \r\n"
    "    // Tangent \r\n"
    "    float3 Tangent = float3( In.NormalTangent.w, In.TangentYZ.x, In.TangentYZ.y ); \r\n"
    "    Out.Tangent = mul(g_matWorld, Tangent/32767.f); \r\n"
    "    Out.Tangent = normalize( Out.Tangent ); \r\n"
    " \r\n"
    "    // Set up the tangent frame in world space \r\n"
    "    Out.Binormal = cross( Out.Normal, Out.Tangent ); \r\n"
    " \r\n"
    "    // Pass the texture coordinate \r\n"
    "    Out.TexCoord.xy = In.TexCoord; \r\n"
    " \r\n"
    "    Out.vColor0 = In.vColor; \r\n"
    " \r\n"
    "    // HalfVector \r\n"
    "    float3 vHalf = g_vCameraFrom - mul(g_matWorld, In.Position); \r\n"
    "    vHalf = normalize( vHalf ); \r\n"
    "    Out.vEyeVector = vHalf; \r\n"
    "    vHalf += g_vLightDir; \r\n"
    "    Out.vHalfVector = normalize( vHalf ); \r\n"
    " \r\n"
    "    Out.fFog = saturate((g_vFog.x - Out.Position.z) / g_vFog.y); \r\n"
    "    return Out; \r\n"
    "} \r\n"
    "\r\n";


    //----------------------------------------------------------------------------- m_strVertexShaderFX
    m_strVertexShaderFX += 
	"//---------------------------------------------------------------------------------------------------------- \r\n"
	"//					V	S   Real Time Point Light  \r\n"
    "VS_OUTPUT_REAL_PL VS_REAL_PL( VS_INPUT In )  \r\n"
    "{ \r\n"
    "    VS_OUTPUT_REAL_PL Out; \r\n"
    "     \r\n"
    "    // Now that we have the calculated weight, add in the final influence \r\n"
    "    Out.Position = mul(g_matWVP, In.Position); \r\n"
    "    Out.Normal.xyz = mul(g_matWorld, In.NormalTangent.xyz/32767.f); \r\n"
    "    Out.Normal = normalize( Out.Normal ) * g_vLightDir.w; \r\n"
    " \r\n"
    "    // Tangent \r\n"
    "    float3 Tangent = float3( In.NormalTangent.w, In.TangentYZ.x, In.TangentYZ.y ); \r\n"
    "    Out.Tangent = mul(g_matWorld, Tangent/32767.f); \r\n"
    "    Out.Tangent = normalize( Out.Tangent ); \r\n"
    " \r\n"
    "    // Set up the tangent frame in world space \r\n"
    "    Out.Binormal = cross( Out.Normal, Out.Tangent ); \r\n"
    " \r\n"
    "    // Pass the texture coordinate \r\n"
    "    Out.TexCoord.xy = In.TexCoord; \r\n"
    " \r\n"
    "    float3 vPos = mul(g_matWorld, In.Position); \r\n"
    "    float3 vPointColor = 0.f; \r\n"
    "    for ( int i=0; i<g_nPointLightNum; ++i ) \r\n"
    "    { \r\n"
    "        float3 afPosDir = g_vPos_Range_PL[i].xyz - vPos; \r\n"
    "        float fLength = length( afPosDir ); \r\n"
    "        // 내적 구함. \r\n"
    "        afPosDir = normalize( afPosDir ); \r\n"
    "        float fDotPL = saturate( dot( afPosDir, Out.Normal ) ); \r\n"
    "        // 선형 감쇠, 2차 감쇠, 지수 감쇠 적용 \r\n"
    "        float fAttenuationSub = g_vAtt_PL[i].x; \r\n"
    "        fAttenuationSub += g_vAtt_PL[i].y * fLength; \r\n"
    "        fAttenuationSub += g_vAtt_PL[i].z * fLength * fLength; \r\n"
    "        // 최종 감쇠값 \r\n"
    "        float fAttenuation = 1.f / fAttenuationSub; \r\n"
    "        // 거리 체크 \r\n"
    "        float fEnableLength = step(fLength, g_vPos_Range_PL[i].w); \r\n"
    "        // 마지막 \r\n"
    "        vPointColor += g_vDiff_PL[i].xyz * fAttenuation * fDotPL * fEnableLength; \r\n"
    "    } \r\n"
    //"    { \r\n"
    //"        float3 afPosDir = g_vPos_Range_PL[i].xyz - vPos; \r\n"
    //"        float fLength = length( afPosDir ); \r\n"
    //"        fLength = (g_vPos_Range_PL[i].w-fLength)/g_vPos_Range_PL[i].w; \r\n"
    //"        fLength = max( fLength, 0 ); \r\n"
    //"        afPosDir = normalize( afPosDir ); \r\n"
    //"        float fDotPL = saturate( dot( afPosDir, Out.Normal ) ); \r\n"
    //"        vPointColor += g_vDiff_PL[i].xyz * fLength * fDotPL; \r\n"
    //"    } \r\n"
    "    Out.vColor.xyz = vPointColor; \r\n"
    "    Out.vColor1 = In.vColor; \r\n"
    " \r\n"
    "    // HalfVector \r\n"
    "    float3 vHalf = g_vCameraFrom - vPos; \r\n"
    "    vHalf = normalize( vHalf ); \r\n"
    "    Out.vEyeVector = vHalf; \r\n"
    "    vHalf += g_vLightDir; \r\n"
    "    Out.vHalfVector = normalize( vHalf ); \r\n"
    " \r\n"
    "    Out.fFog = saturate((g_vFog.x - Out.Position.z) / g_vFog.y); \r\n"
    "    return Out; \r\n"
    "} \r\n"
    "\r\n";


	m_strVertexShaderFX += 
	"//---------------------------------------------------------------------------------------------------------- \r\n"
	"//					V	S   STAR_GLOW  \r\n"
	"VS_OUTPUT VS_STARGLOW( VS_INPUT In )  \r\n"
	"{ \r\n"
	"    VS_OUTPUT Out; \r\n"
	"     \r\n"
	"    // Now that we have the calculated weight, add in the final influence \r\n"
	"    Out.Position = mul(g_matWVP, In.Position); \r\n"
	" \r\n"
	"    // Pass the texture coordinate \r\n"
	"    Out.TexCoord.xy = In.TexCoord; \r\n"
	" \r\n"
	"    Out.vColor0 = In.vColor; \r\n"
	" \r\n"
	"    Out.fFog = saturate((g_vFog.x - Out.Position.z) / g_vFog.y); \r\n"
	" \r\n"
	"    // 임시데이터를 넣는다. \r\n"
	"    Out.Normal = 1.f; \r\n"
	"    Out.Tangent = 1.f; \r\n"
	"    Out.Binormal = 1.f; \r\n"
	"    Out.vHalfVector = 1.f; \r\n"
	"    Out.vEyeVector = 1.f; \r\n"
	" \r\n"
	"    return Out; \r\n"
	"} \r\n"
	"\r\n";

    //---------------------------------------------------------------------------------------------------- m_strPixelShaderFX
    m_strPixelShaderFX = 
    "//---------------------------------------------------------------------------------------------------------- \r\n"
    "//					P	S	_	2	0 \r\n"
    "float4 PS( VS_OUTPUT In ) : COLOR0  \r\n"
    "{ \r\n"
    "    // NormalTexture \r\n"
    "    g_vWorldNormal = GetNormal( In ); \r\n"
    " \r\n"
    "    // DiffuseTexture \r\n"
    "    float4 vColor = GetDiffuse( In ); \r\n"
    "    //vColor *= In.vColor0; \r\n"
    " \r\n"
    "    // Specular값 계산      \r\n"
    "    float3 vSpecular = GetSpecular( In ); \r\n"
    " \r\n"
    "    // EmissiveTexture \r\n"
    "    float3 vEmissive = GetEmissive( In ); \r\n"
    " \r\n"
    "    // NormalTexture  \r\n"
    "    float fOpacity = GetOpacity( In ); \r\n"
    "    fOpacity *= In.vColor0.w; \r\n"
    " \r\n"
    "    // Ambient + Specular \r\n"
    "    vColor.xyz *= In.vColor0.xyz;                                     // Texture * (Diff+Ambient+Point) \r\n"
    "    vColor.xyz += vSpecular;                                          // Texture * (Diff+Ambient+Point) + Spec \r\n"
    "    vColor.xyz += vEmissive;                                          // Texture * (Diff+Ambient+Point) + Spec + Emissive \r\n"
    "    vColor.w = fOpacity ; \r\n"
    " \r\n"
    "    return vColor; \r\n"
    "} \r\n"
    "\r\n";



    //---------------------------------------------------------------------------------------------------- m_strPixelShaderFX
    m_strPixelShaderFX += 
    " \r\n"
    "//---------------------------------------------------------------------------------------------------------- \r\n"
    "//					P	S	_	2	0 \r\n"
    "float4 PS_REAL_PL( VS_OUTPUT_REAL_PL In ) : COLOR0  \r\n"
    "{ \r\n"
    "    VS_OUTPUT In2; \r\n"
    "    In2.Position = In.Position; \r\n" 
    "    In2.vColor0 = In.vColor1; \r\n"
    "    In2.TexCoord = In.TexCoord; \r\n"
    "    In2.Normal = In.Normal; \r\n"
    "    In2.Tangent = In.Tangent; \r\n"
    "    In2.Binormal = In.Binormal; \r\n"
    "    In2.vHalfVector = In.vHalfVector; \r\n"
    "    In2.vEyeVector = In.vEyeVector; \r\n"
    "    In2.fFog = In.fFog; \r\n"
    " \r\n"
    "    // NormalTexture \r\n"
    "    g_vWorldNormal = GetNormal( In2 ); \r\n"
    " \r\n"
    "    // DiffuseTexture \r\n"
    "    float4 vColor = GetDiffuse( In2 ); \r\n"
    "    vColor *= In.vColor1; \r\n"
    " \r\n"
    "    // Specular값 계산      \r\n"
    "    float3 vSpecular = GetSpecular( In2 ); \r\n"
    " \r\n"
    "    // EmissiveTexture \r\n"
    "    float3 vEmissive = GetEmissive( In2 ); \r\n"
    " \r\n"
    "    // NormalTexture  \r\n"
    "    float fOpacity = GetOpacity( In2 ); \r\n"
    "    fOpacity = fOpacity * In.vColor1.w; \r\n"
    " \r\n"
    "    // 내적 계산 \r\n"
    "    float fDiffuseDot = saturate( dot( g_vLightDir, g_vWorldNormal ) ); \r\n"
    "    float3 vDiffuse = g_vLightDiffuse * fDiffuseDot * GetDiffusePower(); \r\n"
    " \r\n"
    "    // Ambient + Specular \r\n"
    "    float3 vColorTemp = (g_vLightAmbient * GetAmbientPower()) + vDiffuse; // Diffuse + Ambient\r\n"
    "    vColorTemp += In.vColor.xyz;                                      // (Diff+Ambient+Point) PointLight 색 추가 \r\n"
    "    vColorTemp = min( vColorTemp, float3(2.f,2.f,2.f) );              // 최대값을 2로 제한한다. 현재 LightMap OverLighting 강도가 2배임 \r\n"
    "    vColor.xyz *= vColorTemp;                                         // Texture * (Diff+Ambient+Point) \r\n"
    "    vColor.xyz += vSpecular;                                          // Texture * (Diff+Ambient+Point) + Spec \r\n"
    "    vColor.xyz += vEmissive;                                          // Texture * (Diff+Ambient+Point) + Spec + Emissive \r\n"
    "    vColor.w = fOpacity * g_vLightDiffuse.w; \r\n"
    " \r\n"
    "    return vColor; \r\n"
    "} \r\n"
    "\r\n";


	m_strPixelShaderFX += 
	"//---------------------------------------------------------------------------------------------------------- \r\n"
	"//					P	S   STAR_GLOW	_	2	0 \r\n"
	"float4 PS_STARGLOW( VS_OUTPUT In ) : COLOR0  \r\n"
	"{ \r\n"
	"    // EmissiveTexture \r\n"
	"    float3 vEmissive = GetStarGlow( In ); \r\n"
	"    vEmissive *= In.vColor0; \r\n"
	" \r\n"
	"    // NormalTexture  \r\n"
	"    float fOpacity = GetOpacity( In ); \r\n"
	"    fOpacity = fOpacity * In.vColor0.w; \r\n"
	" \r\n"
	"	 float4 vColor; \r\n"
	"    vColor.xyz = vEmissive * fOpacity;                                   // Emissive \r\n"
	"    vColor.w = 1.f; \r\n"
	" \r\n"
	"    return vColor; \r\n"
	"} \r\n"
	"\r\n";



    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[0]
    m_strTechniqueFX[0] = 
    "///////////////////////////////////////////////////////////////////////  \r\n"
    "//	Techniques \r\n"
    "technique VisualMaterial \r\n"
    "{ \r\n";


    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[1]
    m_strTechniqueFX[1] = 
    "} \r\n";



    //---------------------------------------------------------------------------------------------------- m_strTechniqueBaseFX[0]
    m_strTechniqueBaseFX[0] = 
    "    pass P0 \r\n"
    "    {"
    " \r\n";


    //---------------------------------------------------------------------------------------------------- m_strTechniqueBaseFX[1]
    m_strTechniqueBaseFX[1] = 
    " \r\n"
    "        VertexShader = compile vs_2_0 VS(); \r\n"
    "        PixelShader  = compile ps_2_0 PS(); \r\n"
    "    } \r\n";



    //---------------------------------------------------------------------------------------------------- m_strTechniqueRealPLFX[0]
    m_strTechniqueRealPLFX[0] = 
    "    pass P1 \r\n"
    "    {"
    " \r\n";


    //---------------------------------------------------------------------------------------------------- m_strTechniqueRealPLFX[1]
    m_strTechniqueRealPLFX[1] = 
    " \r\n"
    "        VertexShader = compile vs_2_0 VS_REAL_PL(); \r\n"
    "        PixelShader  = compile ps_2_0 PS_REAL_PL(); \r\n"
    "    } \r\n";


	//---------------------------------------------------------------------------------------------------- m_strTechniqueStarGlowFX[0]
	m_strTechniqueStarGlowFX[0] = 
	"    pass P2 \r\n"
	"    {"
	" \r\n";


	//---------------------------------------------------------------------------------------------------- m_strTechniqueStarGlowFX[1]
	m_strTechniqueStarGlowFX[1] = 
	" \r\n"
	"        VertexShader = compile vs_2_0 VS_STARGLOW(); \r\n"
	"        PixelShader  = compile ps_2_0 PS_STARGLOW(); \r\n"
	"    } \r\n";
}

void DxVMCommandFX::MakeStringFX_PieceColor( TSTRING& strFX, const COMMENT_FX& sCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM )
{
    strFX.clear();

    std::string strTexDefineFX;
    std::string strTechOpacityFX;
    std::string strTwoSidedFX;
    std::string strShininessPowerTextureFX;
    std::string strNormalFX_End;
    MakeStringFX_Common( sCommentFX, nTextureNUM, nCubeTextureNUM, nPixelScalarNUM, nPixelVectorNUM,
                        strTexDefineFX, strTechOpacityFX, strTwoSidedFX, strShininessPowerTextureFX, strNormalFX_End );
    
	std::string strTechniqueStarGlow = m_strTechniqueStarGlowFX[0] +
										strTechOpacityFX + sCommentFX.m_strWireframeFX + strTwoSidedFX + sCommentFX.m_strDisableDepthTest +
										m_strTechniqueStarGlowFX[1];

    // 합성
    strFX = m_strDefindFX +
            strTexDefineFX +
            m_strBaseFX +
            m_strVertexShaderFX +
            m_strReflectionVectorFX +
            m_strDiffuseFX[0] + sCommentFX.m_strDiffuseFX + m_strDiffuseFX[1] +
            m_strDiffusePowerFX + sCommentFX.m_strDiffusePowerFX + m_strPowerEndFX + 
            m_strAmbientPowerFX + sCommentFX.m_strAmbientPowerFX + m_strPowerEndFX + 
            m_strSpecularPowerFX + sCommentFX.m_strSpecularPowerFX + m_strPowerEndFX + 
            m_strShininessPowerFX + sCommentFX.m_strShininessPowerFX + m_strPowerEndFX + 
            m_strLevelPowerFX + sCommentFX.m_strLevelPowerFX + m_strPowerEndFX + 
            m_strLevelFlowPowerFX + sCommentFX.m_strLevelFlowPowerFX + m_strPowerEndFX + 
            m_strShininessFX[0] + sCommentFX.m_strShininessFX + strShininessPowerTextureFX + m_strShininessFX[1] +
            m_strSpecularFX[0] + sCommentFX.m_strSpecularFX + m_strSpecularFX[1] +
            m_strEmissiveFX[0] + sCommentFX.m_strEmissiveFX + m_strEmissiveFX[1] +
			m_strStarGlowFX[0] + sCommentFX.m_strStarGlowFX + m_strStarGlowFX[1] +
            m_strOpacityFX[0] + sCommentFX.m_strOpacityFX + m_strOpacityFX[1] +
            m_strNormalFX[0] + sCommentFX.m_strNormalFX + strNormalFX_End +
            m_strPixelShaderFX +
            m_strTechniqueFX[0] + 

            m_strTechniqueBaseFX[0] +
            strTechOpacityFX + sCommentFX.m_strWireframeFX + strTwoSidedFX + sCommentFX.m_strDisableDepthTest +
            m_strTechniqueBaseFX[1] +

            m_strTechniqueRealPLFX[0] +
            strTechOpacityFX + sCommentFX.m_strWireframeFX + strTwoSidedFX + sCommentFX.m_strDisableDepthTest +
            m_strTechniqueRealPLFX[1] +

			strTechniqueStarGlow +

            m_strTechniqueFX[1];
}