#include "stdafx.h"

#include "DxVMCommandFX.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


void DxVMCommandFX::SetStringFX_Common( BOOL bUserColorOverlay )
{
    //---------------------------------------------------------------------------------------------------- m_strReflectionVectorFX
    m_strReflectionVectorFX =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					R e f l e c t i o n  V e c t o r \r\n"
    "float3 GetReflectionVector( VS_OUTPUT In ) \r\n"
    "{ \r\n"
    "    float fDot = dot( In.vEyeVector, g_vWorldNormal ); \r\n"
    "    return (2*fDot*g_vWorldNormal) - In.vEyeVector; \r\n"
    "} \r\n"
    "\r\n";



    //---------------------------------------------------------------------------------------------------- m_strDiffuseFX[0]
    m_strDiffuseFX[0] =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					D i f f u s e \r\n"
    "float4 GetDiffuse( VS_OUTPUT In ) \r\n"
    "{ \r\n"
    "    float4 color; \r\n"
    "    color = 1.f; \r\n";


    //---------------------------------------------------------------------------------------------------- m_strDiffuseFX[1]
    m_strDiffuseFX[1] =
    "    return color; \r\n"
    "} \r\n"
    "\r\n";



    //---------------------------------------------------------------------------------------------------- m_strDiffuseFX[0]
    m_strUserColorFX[0] =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					U s e r C o l o r 1 \r\n"
    "float4 GetUserColor1( VS_OUTPUT In ) \r\n"
    "{ \r\n"
    "    float4 color = 0.f; \r\n";



    //---------------------------------------------------------------------------------------------------- m_strDiffuseFX[1]
    m_strUserColorFX[1] =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					U s e r C o l o r 2 \r\n"
    "float4 GetUserColor2( VS_OUTPUT In ) \r\n"
    "{ \r\n"
    "    float4 color = 0.f; \r\n";



    if ( bUserColorOverlay )
    {
         //---------------------------------------------------------------------------------------------------- m_strAddUserColorFX
        m_strAddUserColorFX =
        "//--------------------------------------------------------------------------------------------- \r\n"
        "//					U s e r  C o l o r \r\n"
        "float4 AddUserColor( float4 vDiffuseColor, float4 vUserTexMask1, float4 vUserTexMask2 ) \r\n"
        "{ \r\n"
        "	float4 Color1, Color2;       \r\n"

	    "	Color1 = ( 2 * vDiffuseColor ) * g_vUserColor1;\r\n"
	    "	Color2 = ( 2 * vDiffuseColor ) * g_vUserColor2;\r\n"

	    "	if ( vDiffuseColor.x > 0.5 )\r\n"
	    "		Color1.x = 1 - ( 1 - ( 2 * (vDiffuseColor.x-0.5) ) ) * (1-g_vUserColor1.x);\r\n"

	    "	if ( vDiffuseColor.y > 0.5 )\r\n"
	    "		Color1.y = 1 - ( 1 - ( 2 * (vDiffuseColor.y-0.5) ) ) * (1-g_vUserColor1.y);\r\n"

	    "	if ( vDiffuseColor.z > 0.5 )\r\n"
	    "		Color1.z = 1 - ( 1 - ( 2 * (vDiffuseColor.z-0.5) ) ) * (1-g_vUserColor1.z);\r\n"

	    "	if ( vDiffuseColor.x > 0.5 )\r\n"
	    "		Color2.x = 1 - ( 1 - ( 2 * (vDiffuseColor.x-0.5) ) ) * (1-g_vUserColor2.x);\r\n"

	    "	if ( vDiffuseColor.y > 0.5 )\r\n"
	    "		Color2.y = 1 - ( 1 - ( 2 * (vDiffuseColor.y-0.5) ) ) * (1-g_vUserColor2.y);\r\n"

	    "	if ( vDiffuseColor.z > 0.5 )\r\n"
	    "		Color2.z = 1 - ( 1 - ( 2 * (vDiffuseColor.z-0.5) ) ) * (1-g_vUserColor2.z);\r\n"

	    "	float4 vDestColor = vDiffuseColor - ( ( vDiffuseColor * vUserTexMask1 ) + ( vDiffuseColor * vUserTexMask2 ) );\r\n"
	    "	float4 vOverlay = ( Color1 * vUserTexMask1 ) + ( Color2 * vUserTexMask2 );\r\n"

	    "	float4 color;\r\n"
	    "	color = vDestColor + vOverlay;\r\n"
        "    return color; \r\n"
        "} \r\n"
        "\r\n";
    }
    else
    {
        //---------------------------------------------------------------------------------------------------- m_strAddUserColorFX
        m_strAddUserColorFX =
        "//--------------------------------------------------------------------------------------------- \r\n"
        "//					U s e r  C o l o r \r\n"
        "float4 AddUserColor( float4 vDiffuseColor, float4 vUserTexMask1, float4 vUserTexMask2 ) \r\n"
        "{ \r\n"
        "    float4 color; \r\n"
        "    float4 vColor1, vColor2;       \r\n"
        "    vColor1 = ( 2 * vDiffuseColor ) * g_vUserColor1; \r\n"
	    "    vColor2 = ( 2 * vDiffuseColor ) * g_vUserColor2; \r\n"
        "    float4 vDestColor = vDiffuseColor - ( ( vDiffuseColor * vUserTexMask1 ) + ( vDiffuseColor * vUserTexMask2 ) ); \r\n"
	    "    float4 vOverlay = ( vColor1 * vUserTexMask1 ) + ( vColor2 * vUserTexMask2 ); \r\n"
	    "    color = vDestColor + vOverlay; \r\n"
        "    return color; \r\n"
        "} \r\n"
        "\r\n";
    }


    //---------------------------------------------------------------------------------------------------- m_strDiffusePowerFX
    m_strDiffusePowerFX =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					D i f f u s e P o w e r \r\n"
    "float GetDiffusePower()  \r\n"
    "{ \r\n";



    //---------------------------------------------------------------------------------------------------- m_strAmbientPowerFX
    m_strAmbientPowerFX =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					A m b i e n t P o w e r \r\n"
    "float GetAmbientPower()  \r\n"
    "{ \r\n";



    //---------------------------------------------------------------------------------------------------- m_strSpecularPowerFX
    m_strSpecularPowerFX =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					S p e c u l a r P o w e r \r\n"
    "float GetSpecularPower()  \r\n"
    "{ \r\n";



    //---------------------------------------------------------------------------------------------------- m_strShininessPowerFX
    m_strShininessPowerFX =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					S h i n i n e s s P o w e r \r\n"
    "float GetShininessPower()  \r\n"
    "{ \r\n";


    //---------------------------------------------------------------------------------------------------- m_strLevelPowerFX
    m_strLevelPowerFX =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					Level P o w e r \r\n"
    "float GetLevelPower()  \r\n"
    "{ \r\n";



    //---------------------------------------------------------------------------------------------------- m_strLevelFlowPowerFX
    m_strLevelFlowPowerFX =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					L e v e l F l o w P o w e r \r\n"
    "float GetLevelFlowPower()  \r\n"
    "{ \r\n";



    //----------------------------------------------------------------------------------------------------m_strPowerEndFX
    m_strPowerEndFX =
    "} \r\n"
    "\r\n";




    //---------------------------------------------------------------------------------------------------- m_strShininessFX[0]
    m_strShininessFX[0] = 
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					S h i n i n e s s \r\n"
    "float GetShininess( VS_OUTPUT In ) \r\n"
    "{ \r\n"
    "    float ShininessMap = 1.f;  \r\n"
    " \r\n"
    "    // 내적 계산   \r\n"
    "    float fDot = saturate( dot( In.vHalfVector, g_vWorldNormal ) ); \r\n"
    " \r\n"
    "    // ShininessMap \r\n";



    //---------------------------------------------------------------------------------------------------- m_strShininessFX[1]
    m_strShininessFX[1] =
    "}   \r\n"
    " \r\n";





    //---------------------------------------------------------------------------------------------------- m_strShininessPowerTextureFX[0]
    m_strShininessPowerTextureFX[0] = 
    " \r\n"
    "    // 최종 계산 \r\n"
    "    return tex2D( ShininessTexSampler, float2(fDot,GetShininessPower()*ShininessMap) );  \r\n";


    //---------------------------------------------------------------------------------------------------- m_strShininessPowerTextureFX[1]
    m_strShininessPowerTextureFX[1] =
    " \r\n"
    "    // 최종 계산 \r\n"
    "    return pow(fDot,GetShininessPower()*ShininessMap);  \r\n";





    //---------------------------------------------------------------------------------------------------- m_strSpecularFX[0]
    m_strSpecularFX[0] = 
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					S p e c u l a r \r\n"
    "float3 GetSpecular( VS_OUTPUT In ) \r\n"
    "{ \r\n"
    "    float3 Specular = 1.f;  \r\n"
    " \r\n"
    "    // Specular * Shininess map  \r\n";



    //---------------------------------------------------------------------------------------------------- m_strSpecularFX[1]
    m_strSpecularFX[1] =
    "    return Specular * GetShininess( In ) * GetSpecularPower() * g_vLightSpecular; \r\n"
    "}   \r\n"
    " \r\n";







    //---------------------------------------------------------------------------------------------------- m_strSpecularFX[0]
    m_strLevelFX[0] = 
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					L e v e l \r\n"
    "float GetLevel( VS_OUTPUT In ) \r\n"
    "{ \r\n"
    "    float fLevel = 0.f;  \r\n"
    " \r\n";



    //---------------------------------------------------------------------------------------------------- m_strSpecularFX[1]
    m_strLevelFX[1] =
    "    return fLevel; \r\n"
    "}   \r\n"
    " \r\n";






    //---------------------------------------------------------------------------------------------------- m_strEmissiveFX[0]
    m_strEmissiveFX[0] =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					E m i s s i v e \r\n"
    "float3 GetEmissive( VS_OUTPUT In ) \r\n"
    "{ \r\n"
    "    float3 color; \r\n"
    "    color = 0.f; \r\n";


    //---------------------------------------------------------------------------------------------------- m_strEmissiveFX[1]
    m_strEmissiveFX[1] =
    "    return color; \r\n"
    "} \r\n"
    "\r\n";



	//---------------------------------------------------------------------------------------------------- m_strStarGlowFX[0]
	m_strStarGlowFX[0] =
	"//--------------------------------------------------------------------------------------------- \r\n"
	"//					S t a r   G l o w   \r\n"
	"float3 GetStarGlow( VS_OUTPUT In ) \r\n"
	"{ \r\n"
	"    float3 color; \r\n"
	"    color = 0.f; \r\n";


	//---------------------------------------------------------------------------------------------------- m_strStarGlowFX[1]
	m_strStarGlowFX[1] =
	"    return color; \r\n"
	"} \r\n"
	"\r\n";


	

    //---------------------------------------------------------------------------------------------------- m_strOpacityFX[0]
    m_strOpacityFX[0] =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					O p a c i t y \r\n"
    "float GetOpacity( VS_OUTPUT In ) \r\n"
    "{ \r\n"
    "    float color = 1.f; \r\n";


    //---------------------------------------------------------------------------------------------------- m_strOpacityFX[1]
    m_strOpacityFX[1] =
    "    return color; \r\n"
    "} \r\n"
    "\r\n";



    //---------------------------------------------------------------------------------------------------- m_strNormalFX[0]
    m_strNormalFX[0] = 
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					N o r m a l \r\n"
    "float3 GetNormal( VS_OUTPUT In ) \r\n"
    "{ \r\n"
    "    float4 color; \r\n"
    "    color = 1.f; \r\n";


    //---------------------------------------------------------------------------------------------------- m_strNormalFX[1]
    m_strNormalFX[1] =
    " \r\n"
    "    float3 Normal; \r\n"
    "    Normal = normalize( In.Normal ); \r\n"
    " \r\n"
    "    return Normal;  \r\n"
    "}  \r\n"
    " \r\n";


    //---------------------------------------------------------------------------------------------------- m_strNormalFX[2]
    m_strNormalFX[2] =
    "     \r\n"
    "    // If using a signed texture, we must unbias the normal map data \r\n"
    "    float3 Normal; \r\n"
    "    Normal.xy = (color.wy * 2) - 1; \r\n"
    "    Normal.z = 1.f - saturate( dot( Normal.xy, Normal.xy ) ); \r\n"
    "    \r\n"
    "    // Move the normal from tangent space to world space \r\n"
    "    float3x3 mTangentFrame = { In.Tangent, In.Binormal, In.Normal }; \r\n"
    "    Normal = mul( Normal, mTangentFrame ); \r\n"
    "    Normal = normalize( Normal ); \r\n"
    " \r\n"
    "    return Normal;  \r\n"
    "}  \r\n"
    " \r\n";




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
    " \r\n"
    "    // UserColor \r\n"
    "    vColor = AddUserColor( vColor, GetUserColor1( In ), GetUserColor2( In ) ); \r\n"
    " \r\n"
    "    // Specular값 계산      \r\n"
    "    float3 vSpecular = GetSpecular( In ); \r\n"
    "     \r\n"
    "    // EmissiveTexture \r\n"
    "    float3 vEmissive = GetEmissive( In ); \r\n"
    "     \r\n"
    "    // NormalTexture  \r\n"
    "    float fOpacity = GetOpacity( In ); \r\n"
    "     \r\n"
    "    // 내적 계산 \r\n"
    "    float fDiffuseDot = saturate( dot( g_vLightDir, g_vWorldNormal ) ); \r\n"
    "    float3 vDiffuse = g_vLightDiffuse * fDiffuseDot * GetDiffusePower(); \r\n"
    " \r\n"
    "    // Ambient + Specular \r\n"
    "    float3 vColorTemp = (g_vLightAmbient * GetAmbientPower()) + vDiffuse; // Diffuse + Ambient\r\n"
    "    vColorTemp += In.vColor0.xyz;                                       // (Diff+Ambient+Point) PointLight 색 추가 \r\n"
    "    vColor.xyz *= vColorTemp;                                         // Texture * Level \r\n"
    "    vColor.xyz += vSpecular;                                          // Texture * Level + Spec \r\n"
    "    vColor.xyz += vEmissive;                                          // Texture * Level + Spec + Emissive \r\n"
    "    vColor.w = fOpacity * g_vLightDiffuse.w; \r\n"
    " \r\n"
    "    return vColor; \r\n"
    "} \r\n"
    "\r\n";

    //---------------------------------------------------------------------------------------------------- m_strPixelShaderLevelFX
    m_strPixelShaderLevelFX = 
    "//---------------------------------------------------------------------------------------------------------- \r\n"
    "//					P	S	_	2	0 \r\n"
    "float4 PS_Level( VS_OUTPUT In ) : COLOR0  \r\n"
    "{ \r\n"
    "    // NormalTexture \r\n"
    "    g_vWorldNormal = GetNormal( In ); \r\n"
    " \r\n"
    "    // Level \r\n"
    "    float fLevel = GetLevel( In ); \r\n"
    " \r\n"
    "    // DiffuseTexture \r\n"
    "    float4 vColor = GetDiffuse( In ); \r\n"
     " \r\n"
    "    // UserColor \r\n"
    "    vColor = AddUserColor( vColor, GetUserColor1( In ), GetUserColor2( In ) ); \r\n"
    " \r\n"
    "    // Specular값 계산      \r\n"
    "    float3 vSpecular = GetSpecular( In ); \r\n"
    "     \r\n"
    "    // EmissiveTexture \r\n"
    "    float3 vEmissive = GetEmissive( In ); \r\n"
    "     \r\n"
    "    // NormalTexture  \r\n"
    "    float fOpacity = GetOpacity( In ); \r\n"
    "     \r\n"
    "    // 내적 계산 \r\n"
    "    float fDiffuseDot = saturate( dot( g_vLightDir, g_vWorldNormal ) ); \r\n"
    "    float3 vDiffuse = g_vLightDiffuse * fDiffuseDot * GetDiffusePower(); \r\n"
    " \r\n"
    "    // Ambient + Specular \r\n"
    "    float fLevelA = min( 1.f, fLevel ); \r\n"
    "    float3 vColorTemp = (g_vLightAmbient * GetAmbientPower()) + vDiffuse; // Diffuse + Ambient\r\n"
    "    vColorTemp += In.vColor0.xyz;                                       // (Diff+Ambient+Point) PointLight 색 추가 \r\n"
    "    vColorTemp = (g_vLightLevel*GetLevelPower()*fLevelA) + (vColorTemp)*(1-fLevelA);  // Level적용(Diff+Ambient+Point) \r\n"
    "    vColor.xyz *= vColorTemp;                                         // Texture * Level \r\n"
    "    vColor.xyz += vSpecular;                                          // Texture * Level + Spec \r\n"
    "    vColor.xyz += vEmissive;                                          // Texture * Level + Spec + Emissive \r\n"
    "    vColor.w = fOpacity; \r\n"
    " \r\n"
    "    return vColor; \r\n"
    "} \r\n"
    "\r\n";

    //---------------------------------------------------------------------------------------------------- m_strPixelShaderFlowFX
    m_strPixelShaderFlowFX = 
    "//---------------------------------------------------------------------------------------------------------- \r\n"
    "//					P	S	_	2	0 \r\n"
    "float4 PS_Flow( VS_OUTPUT In ) : COLOR0  \r\n"
    "{ \r\n"
    "    // NormalTexture \r\n"
    "    g_vWorldNormal = GetNormal( In ); \r\n"
    " \r\n"
    "    // Level \r\n"
    "    float fLevel = GetLevel( In ); \r\n"
    " \r\n"
    "    // DiffuseTexture \r\n"
    "    float4 vColor = GetDiffuse( In ); \r\n"
     " \r\n"
    "    // UserColor \r\n"
    "    vColor = AddUserColor( vColor, GetUserColor1( In ), GetUserColor2( In ) ); \r\n"
    " \r\n"
    "    // Specular값 계산      \r\n"
    "    float3 vSpecular = GetSpecular( In ); \r\n"
    "     \r\n"
    "    // EmissiveTexture \r\n"
    "    float3 vEmissive = GetEmissive( In ); \r\n"
    "     \r\n"
    "    // NormalTexture  \r\n"
    "    float fOpacity = GetOpacity( In ); \r\n"
    "     \r\n"
    "    // 내적 계산 \r\n"
    "    float fDiffuseDot = saturate( dot( g_vLightDir, g_vWorldNormal ) ); \r\n"
    "    float3 vDiffuse = g_vLightDiffuse * fDiffuseDot * GetDiffusePower(); \r\n"
    " \r\n"
    "    // Ambient + Specular \r\n"
    "    float fLevelA = min( 1.f, fLevel ); \r\n"
    "    float3 vColorTemp = (g_vLightAmbient * GetAmbientPower()) + vDiffuse; // Diffuse + Ambient\r\n"
    "    vColorTemp += In.vColor0.xyz;                                       // (Diff+Ambient+Point) PointLight 색 추가 \r\n"
    "    vColorTemp = (g_vLightLevel*GetLevelPower()*fLevelA) + (vColorTemp)*(1-fLevelA);  // Level적용(Diff+Ambient+Point) \r\n"
    "    vColor.xyz *= vColorTemp;                                         // Texture * Level \r\n"
    "    vColor.xyz += vSpecular;                                          // Texture * Level + Spec \r\n"
    "    vColor.xyz += vEmissive;                                          // Texture * Level + Spec + Emissive \r\n"
    "    vColor.w = fOpacity; \r\n"
    " \r\n"
    "    // LevelFlow \r\n"
    "    fLevelA = min( 1.f, fLevel ); \r\n"
    "    float3 vLevelFlow = tex2D( LevelFlowTexSampler, float2(g_vWorldNormal.x, g_vWorldNormal.y) ); \r\n"
    "    vLevelFlow *= g_vLevelFlowColor * GetLevelFlowPower() * fLevelA; \r\n"
    "    vColor.xyz += vLevelFlow; \r\n"
    "    return vColor; \r\n"
    "} \r\n"
    "\r\n";
}

void DxVMCommandFX::MakeStringFX_Common( const COMMENT_FX& sCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM,
                                      std::string& strTexDefineFX, std::string& strTechOpacityFX, std::string& strTwoSidedFX, 
                                      std::string& strShininessPowerTextureFX, std::string& strNormalFX_End )
{
    // Define
    strTexDefineFX = "\r\n";
    for ( size_t i=0; i<nPixelScalarNUM; ++i )
    {
        char c1 = '0';
        strTexDefineFX += "float g_fUniformPixelScalar_";
        strTexDefineFX += static_cast<char>(c1+i);
        strTexDefineFX += "; \r\n";
    }
    for ( size_t i=0; i<nPixelVectorNUM; ++i )
    {
        char c1 = '0';
        strTexDefineFX += "float4 g_vUniformPixelVector_";
        strTexDefineFX += static_cast<char>(c1+i);
        strTexDefineFX += "; \r\n";
    }
    strTexDefineFX += " \r\n";

    for ( size_t i=0; i<nTextureNUM; ++i )
    {
        char c1 = '0';
        strTexDefineFX += "texture Texture_";
        strTexDefineFX += static_cast<char>(c1+i);
        strTexDefineFX += "; \r\n";
    }
    strTexDefineFX += "texture ShininessTex; \r\n";
    strTexDefineFX += " \r\n";

    for ( size_t i=0; i<nTextureNUM; ++i )
    {
        char c1 = '0';
        strTexDefineFX += "sampler Texture2D_";
        strTexDefineFX += static_cast<char>(c1+i);
        strTexDefineFX += " = sampler_state \r\n";
        strTexDefineFX += "{ \r\n";
        strTexDefineFX += "    Texture = ";
        strTexDefineFX += "(Texture_";
        strTexDefineFX += static_cast<char>(c1+i);
        strTexDefineFX += "); \r\n";
        strTexDefineFX += "}; \r\n";
        strTexDefineFX += " \r\n";
    }

    for ( size_t i=0; i<nCubeTextureNUM; ++i )
    {
        char c1 = '0';
        strTexDefineFX += "texture CubeTexture_";
        strTexDefineFX += static_cast<char>(c1+i);
        strTexDefineFX += "; \r\n";
    }
    strTexDefineFX += " \r\n";

    for ( size_t i=0; i<nCubeTextureNUM; ++i )
    {
        char c1 = '0';
        strTexDefineFX += "sampler TextureCube_";
        strTexDefineFX += static_cast<char>(c1+i);
        strTexDefineFX += " = sampler_state \r\n";
        strTexDefineFX += "{ \r\n";
        strTexDefineFX += "    Texture = ";
        strTexDefineFX += "(CubeTexture_";
        strTexDefineFX += static_cast<char>(c1+i);
        strTexDefineFX += "); \r\n";
        strTexDefineFX += "}; \r\n";
        strTexDefineFX += " \r\n";
    }

    strTexDefineFX += 
    "sampler ShininessTexSampler = sampler_state  \r\n"
    "{ \r\n"
    "    Texture = (ShininessTex); \r\n"
    "    AddressU  = MIRROR; \r\n"
    "    AddressV  = MIRROR; \r\n"
    "}; \r\n"
    " \r\n";

    // LevelFlow
    {
        strTexDefineFX += "texture LevelFlowTex; \r\n";
        strTexDefineFX += " \r\n";

        strTexDefineFX += 
        "sampler LevelFlowTexSampler = sampler_state  \r\n"
        "{ \r\n"
        "    Texture = (LevelFlowTex); \r\n"
        "    AddressU  = MIRROR; \r\n"
        "    AddressV  = MIRROR; \r\n"
        "}; \r\n"
        " \r\n";
    }

    // TextureDay
    {
        strTexDefineFX += "texture LightMapDayTex; \r\n";
        strTexDefineFX += " \r\n";

        strTexDefineFX += 
        "sampler LightMapDayTexSampler = sampler_state  \r\n"
        "{ \r\n"
        "    Texture = (LightMapDayTex); \r\n"
        "    AddressU  = MIRROR; \r\n"
        "    AddressV  = MIRROR; \r\n"
        "}; \r\n"
        " \r\n";
    }

    // TextureNight
    {
        strTexDefineFX += "texture LightMapNightTex; \r\n";
        strTexDefineFX += " \r\n";

        strTexDefineFX += 
        "sampler LightMapNightTexSampler = sampler_state  \r\n"
        "{ \r\n"
        "    Texture = (LightMapNightTex); \r\n"
        "    AddressU  = MIRROR; \r\n"
        "    AddressV  = MIRROR; \r\n"
        "}; \r\n"
        " \r\n";
    }

    // LightMapAvg
    {
        strTexDefineFX += "texture LightMapAvgTex; \r\n";
        strTexDefineFX += " \r\n";

        strTexDefineFX += 
        "sampler LightMapAvgTexSampler = sampler_state  \r\n"
        "{ \r\n"
        "    Texture = (LightMapAvgTex); \r\n"
        "    AddressU  = MIRROR; \r\n"
        "    AddressV  = MIRROR; \r\n"
        "}; \r\n"
        " \r\n";
    }

    // Opacity
    strTechOpacityFX = "";
    m_emBlendMode = sCommentFX.m_emBlendMode;
    if ( sCommentFX.m_strOpacityFX.empty() )
    {
        switch ( sCommentFX.m_emBlendMode )
        {
        case VM_BLENDMODE_ADDITIVE:
            strTechOpacityFX = 
            "        AlphaBlendEnable = TRUE; \r\n"
            "        SrcBlend = One; \r\n"
            "        DestBlend = One; \r\n";
            break;
    
        case VM_BLENDMODE_MODULATE:
            strTechOpacityFX = 
            "        AlphaBlendEnable = TRUE; \r\n"
            "        SrcBlend = DestColor; \r\n"
            "        DestBlend = Zero; \r\n";
            break;
        };
    }
    else
    {
        switch ( sCommentFX.m_emBlendMode )
        {
        case VM_BLENDMODE_MASKED:
            strTechOpacityFX = 
            "        ALPHATESTENABLE = TRUE; \r\n"
            "        ALPHAFUNC = GREATEREQUAL; \r\n";
            strTechOpacityFX += sCommentFX.m_strAlphaREFFX;
            break;
    
        case VM_BLENDMODE_TRANSLUCENT:
            strTechOpacityFX = 
            "        AlphaBlendEnable = TRUE; \r\n"
            "        SrcBlend = SrcAlpha; \r\n"
            "        DestBlend = InvSrcAlpha; \r\n";
            break;
    
        case VM_BLENDMODE_ADDITIVE:
            strTechOpacityFX = 
            "        AlphaBlendEnable = TRUE; \r\n"
            "        SrcBlend = SrcAlpha; \r\n"
            "        DestBlend = One; \r\n";
            break;
    
        case VM_BLENDMODE_MODULATE:
            strTechOpacityFX = 
            "        AlphaBlendEnable = TRUE; \r\n"
            "        SrcBlend = DestColor; \r\n"
            "        DestBlend = Zero; \r\n";
            break;
        };
    }

    // TwoSided
    strTwoSidedFX = "";
    switch ( sCommentFX.m_emTwoSidedMode )
    {
    case VM_TWOSIDEDMODE_OFF:
        m_bTwoSideSeperatePass = FALSE;
        break;

    case VM_TWOSIDEDMODE_ON:
        m_bTwoSideSeperatePass = FALSE;
        strTwoSidedFX = "        CULLMODE = NONE; \r\n";
        break;
    
    case VM_TWOSIDEDMODE_SEPERATE_PASS:
        m_bTwoSideSeperatePass = TRUE;
        break;
    };

	// StarGlow
	if ( sCommentFX.m_strStarGlowFX.empty() )
	{
		m_bActiveStarGlow = FALSE;
	}
	else
	{
		m_bActiveStarGlow = TRUE;
	}
	


    // Shininess Power 
    if ( sCommentFX.m_bShininessPowerTexture )
    {
        strShininessPowerTextureFX = m_strShininessPowerTextureFX[0]; // Used Texture
    }
    else
    {
        strShininessPowerTextureFX = m_strShininessPowerTextureFX[1]; // Used pow()
    }

    // End
    strNormalFX_End = sCommentFX.m_strNormalFX.empty() ? m_strNormalFX[1] : m_strNormalFX[2];
}