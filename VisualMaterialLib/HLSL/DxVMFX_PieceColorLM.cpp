#include "stdafx.h"

#include "DxVMCommandFX.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


// LightMap 용으로 용도 변환
void DxVMCommandFX::SetStringFX_PieceColorLM()
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
    "float2   g_vLightMapUV_Offset; \r\n"
    "float2   g_vDayNightWeight; \r\n";

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
    "    float2 TexCoord0       : TEXCOORD2;    // UV \r\n"
    "    float2 TexCoord1       : TEXCOORD3;    // UV \r\n"
    "}; \r\n"
    " \r\n"
    "struct VS_OUTPUT  \r\n"
    "{ \r\n"
    "    float4 Position    : POSITION; \r\n"
    "    float4 vColor0     : COLOR0; \r\n"
    "    float2 TexCoord    : TEXCOORD0; \r\n"
    "    float2 TexCoordLM  : TEXCOORD1; \r\n"
    "    float3 Normal      : TEXCOORD2; \r\n"
    "    float3 Tangent     : TEXCOORD3; \r\n"
    "    float3 Binormal    : TEXCOORD4; \r\n"
    "    float3 vHalfVector : TEXCOORD5; \r\n"
    "    float3 vEyeVector  : TEXCOORD6; \r\n"
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
    "    Out.TexCoord.xy = In.TexCoord0; \r\n"
    " \r\n"
    "    // Pass the texture coordinate \r\n"
    "    Out.TexCoordLM.xy = In.TexCoord1/32767.f; \r\n"
    "    Out.TexCoordLM.xy += g_vLightMapUV_Offset; \r\n"
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
	"    Out.TexCoord.xy = In.TexCoord0; \r\n"
	" \r\n"
	"    Out.vColor0 = In.vColor; \r\n"
	" \r\n"
	"    Out.fFog = saturate((g_vFog.x - Out.Position.z) / g_vFog.y); \r\n"
	" \r\n"
	"    // 임시데이터를 넣는다. \r\n"
	"    Out.TexCoordLM = 1.f; \r\n"
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
    "    vColor *= In.vColor0; \r\n"
    " \r\n"
    "    // Specular값 계산      \r\n"
    "    float3 vSpecular = GetSpecular( In ); \r\n"
    "     \r\n"
    "    // EmissiveTexture \r\n"
    "    float3 vEmissive = GetEmissive( In ); \r\n"
    "     \r\n"
    "    // NormalTexture  \r\n"
    "    float fOpacity = GetOpacity( In ); \r\n"
    "    fOpacity = fOpacity * In.vColor0.w; \r\n"
    "     \r\n"
    "    // 내적 계산 \r\n"
    "    float fDiffuseDot = saturate( dot( g_vLightDir, g_vWorldNormal ) ); \r\n"
    "    float3 vDiffuse = g_vLightDiffuse * fDiffuseDot * GetDiffusePower(); \r\n"
    " \r\n"
    "    // Ambient + Specular \r\n"
    "    float3 vColorTemp = (g_vLightAmbient * GetAmbientPower()) + vDiffuse; // Diffuse + Ambient\r\n"
    "    vColor.xyz *= vColorTemp;                                         // Texture * (Diff+Ambient+Point) \r\n"
    "    vColor.xyz += vSpecular;                                          // Texture * (Diff+Ambient+Point) + Spec \r\n"
    "    vColor.xyz += vEmissive;                                          // Texture * (Diff+Ambient+Point) + Spec + Emissive \r\n"
    "    vColor.w = fOpacity * g_vLightDiffuse.w; \r\n"
    " \r\n"
    "    return vColor; \r\n"
    "} \r\n"
    " \r\n";

    m_strPixelShaderFX += 
    "//---------------------------------------------------------------------------------------------------------- \r\n"
    "//					P	S   LightMap	_	2	0 \r\n"
    "float4 PS_LM( VS_OUTPUT In ) : COLOR0  \r\n"
    "{ \r\n"
    "    // NormalTexture \r\n"
    "    g_vWorldNormal = GetNormal( In ); \r\n"
    " \r\n"
    "    // DiffuseTexture \r\n"
    "    float4 vColor = GetDiffuse( In ); \r\n"
    "    vColor *= In.vColor0; \r\n"
    " \r\n"
    "    float4 vLightMapDay = tex2D( LightMapDayTexSampler, In.TexCoordLM ); \r\n"
    "    float4 vLightMapNight = tex2D( LightMapNightTexSampler, In.TexCoordLM ); \r\n"
    " \r\n"
    "    // Specular값 계산      \r\n"
    "    float3 vSpecular = GetSpecular( In ); \r\n"
    " \r\n"
    "    // EmissiveTexture \r\n"
    "    float3 vEmissive = GetEmissive( In ); \r\n"
    " \r\n"
    "    // NormalTexture  \r\n"
    "    float fOpacity = GetOpacity( In ); \r\n"
    "    fOpacity = fOpacity * In.vColor0.w; \r\n"
    " \r\n"
    "    // 기본 내적 계산 \r\n"
    "    float3 vDiffuseColor = g_vLightDiffuse * GetDiffusePower(); \r\n"
    "    float3 vAmbientColor = g_vLightAmbient * GetAmbientPower(); \r\n"
    //"    float fDiffuseDot = dot( g_vLightDir, normalize( In.Normal ) ) + 1.f; \r\n"
    //"    float3 vBaseDA = (vDiffuseColor * fDiffuseDot); \r\n"
    "    float fBaseDot = dot( g_vLightDir, normalize( In.Normal ) ); \r\n"
    " \r\n"
    "    // NormalMap 내적 계산 \r\n"
    //"    fDiffuseDot = dot( g_vLightDir, g_vWorldNormal ) + 1.f; \r\n"
    //"    float3 vNormalDA = (vDiffuseColor * fDiffuseDot); \r\n"
    "    float fNormalDot = dot( g_vLightDir, normalize( g_vWorldNormal ) ); \r\n"// + 1.f; \r\n"
    " \r\n"
    "    // Ambient + Specular \r\n"
    "    float4 vLightColor = ( vLightMapDay*g_vDayNightWeight.x + vLightMapNight*g_vDayNightWeight.y ) * 2; \r\n"
    //"    float3 vColorTemp = vLightColor.xyz * 2 * max( min( vNormalDA/vBaseDA, vDiffuseColor+vAmbientColor ), vAmbientColor ); // LightMap에 알맞게 색을 정함. \r\n"
    //"    float3 vColorTemp = vLightColor.xyz * max( min( vNormalDA/vBaseDA, 1 ), vAmbientColor ); // LightMap에 알맞게 색을 정함. \r\n"
    //"    float3 vColorTemp = vLightColor.xyz * max( min( fNormalDot/fBaseDot, 1 ), vAmbientColor ); // LightMap에 알맞게 색을 정함. \r\n"
    //"    float3 vColorTemp = vLightColor.xyz * fNormalDot; // LightMap에 알맞게 색을 정함. \r\n"
    //"    fNormalDot = ((fNormalDot - fBaseDot) * 0.5f) + 1.f;       // -2~2 -> -1~1 -> 0~2 \r\n"  
    "    fNormalDot = (fNormalDot - fBaseDot)*1.f; \r\n"  
    "    fNormalDot = max( fNormalDot, -1.f ) + 1.f;       // -1~1 -> 0~2 \r\n"  
    //"    fNormalDot = 1.f;       // -1~2 -> 0~3 \r\n"  
    "    vColor.xyz *= vLightColor * fNormalDot;                                  // Texture * (Diff+Ambient+Point) \r\n"
    //"    vColor.xyz += (vLightColor.xyz*0.85+0.15) * vSpecular;       // Texture * (Diff+Ambient+Point) + Spec \r\n"
    "    vColor.xyz += vLightColor.xyz * vSpecular;       // Texture * (Diff+Ambient+Point) + Spec \r\n"
    "    vColor.xyz += vEmissive;                                   // Texture * (Diff+Ambient+Point) + Spec + Emissive \r\n"
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



    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[0]
    m_strTechniqueBaseFX[0] = 
    "    pass P0 \r\n"
    "    {"
    " \r\n";


    //---------------------------------------------------------------------------------------------------- m_strTechniqueFX[1]
    m_strTechniqueBaseFX[1] = 
    " \r\n"
    "        VertexShader = compile vs_2_0 VS(); \r\n"
    "        PixelShader  = compile ps_2_0 PS_LM(); \r\n"
    "    } \r\n";



    //---------------------------------------------------------------------------------------------------- m_strTechniqueLightMapFX[0]
    m_strTechniqueLightMapFX[0] = 
    "    pass P1 \r\n"
    "    {"
    " \r\n";


    //---------------------------------------------------------------------------------------------------- m_strTechniqueLightMapFX[1]
    m_strTechniqueLightMapFX[1] = 
    " \r\n"
    "        VertexShader = compile vs_2_0 VS(); \r\n"
    "        PixelShader  = compile ps_2_0 PS(); \r\n"
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

void DxVMCommandFX::MakeStringFX_PieceColorLM( TSTRING& strFX, const COMMENT_FX& sCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM )
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

            m_strTechniqueLightMapFX[0] +
            strTechOpacityFX + sCommentFX.m_strWireframeFX + strTwoSidedFX + sCommentFX.m_strDisableDepthTest +
            m_strTechniqueLightMapFX[1] +

			strTechniqueStarGlow +

            m_strTechniqueFX[1];
}