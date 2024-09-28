#include "stdafx.h"

#include "DxVMCommandFX.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------

void DxVMCommandFX::SetStringFX_Command()
{
    //----------------------------------------------------------------------------- m_strBaseFX
    m_strBaseFX = 
    " \r\n"
    "/////////////////////////////////////////////////////////////////////// \r\n"
    "//	PS \r\n"
    "struct SVM_Output \r\n"
    "{ \r\n"
    "    float4 Position   : POSITION; \r\n"
    "    float2 TexCoord   : TEXCOORD0; \r\n"
    "}; \r\n"
    " \r\n"
    "/////////////////////////////////////////////////////////////////////// \r\n"
    "//	PS \r\n"
    " \r\n";


    //----------------------------------------------------------------------------- m_strReflectionVectorFX
    m_strReflectionVectorFX =
    "//--------------------------------------------------------------------------------------------- \r\n"
    "//					R e f l e c t i o n  V e c t o r \r\n"
    "float3 GetReflectionVector( SVM_Output In ) \r\n"
    "{ \r\n"
    "    return float3(In.TexCoord.x,In.TexCoord.y,1.f); \r\n"
    "} \r\n"
    "\r\n";


    //----------------------------------------------------------------------------- m_strDiffuseFX[0]
    m_strDiffuseFX[0] =
    "float4 PS( SVM_Output In ) : COLOR \r\n"
    "{ \r\n"
    "    float4 color; \r\n"
    "    color = 1.f; \r\n";


    //----------------------------------------------------------------------------- m_strDiffuseFX[1]
    m_strDiffuseFX[1] =
    "    return color; \r\n"
    "} \r\n";


    //----------------------------------------------------------------------------- m_strTechniqueFX
    m_strTechniqueFX[0] = 
    "///////////////////////////////////////////////////////////////////////  \r\n"
    "//	Techniques \r\n"
    " \r\n"
    "technique VisualMaterial \r\n"
    "{ \r\n"
    "    pass P0 \r\n"
    "    { \r\n"
    "        PixelShader  = compile ps_2_0 PS(); \r\n"
    "    } \r\n"
    "} \r\n";
}

void DxVMCommandFX::MakeStringFX_Command( const COMMENT_FX& sCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM )
{
    m_strFX_Piece.clear();

    // Define
    std::string strTexDefineFX = "\r\n";
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

    // ÇÕ¼º
    m_strFX_Piece = strTexDefineFX +
                m_strBaseFX +
                m_strReflectionVectorFX +
                m_strDiffuseFX[0] + sCommentFX.m_strDiffuseFX + m_strDiffuseFX[1] + 
                m_strTechniqueFX[0];
}