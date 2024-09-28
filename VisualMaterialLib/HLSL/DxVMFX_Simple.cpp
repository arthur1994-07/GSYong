#include "stdafx.h"

#include "DxVMCommandFX.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


void DxVMCommandFX::MakeStringFX_Simple( const COMMENT_FX& sCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM )
{
    m_strFX_Simple.clear();

    std::string strTexDefineFX;
    std::string strTechOpacityFX;
    std::string strTwoSidedFX;
    std::string strShininessPowerTextureFX;
    std::string strNormalFX_End;
    MakeStringFX_Common( sCommentFX, nTextureNUM, nCubeTextureNUM, nPixelScalarNUM, nPixelVectorNUM,
                        strTexDefineFX, strTechOpacityFX, strTwoSidedFX, strShininessPowerTextureFX, strNormalFX_End );

    // ÇÕ¼º
    m_strFX_Simple = 
                m_strDiffuseFX[0] + sCommentFX.m_strDiffuseFX + m_strDiffuseFX[1] + 
                m_strUserColorFX[0] + sCommentFX.m_strUserColor1FX + m_strDiffuseFX[1] + 
                m_strUserColorFX[1] + sCommentFX.m_strUserColor2FX + m_strDiffuseFX[1] + 
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
                m_strLevelFX[0] + sCommentFX.m_strLevelFX + m_strLevelFX[1];
}