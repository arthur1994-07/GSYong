#pragma once

#include "../Define/DxVMDefine.h"

class DxVMCommandFX
{
private:
    ID3DXEffect*    m_pFX;
    std::string     m_strFX_Piece;          // Piece LM
    std::string     m_strFX_PieceColor;     // Piece Color
    std::string     m_strFX_PieceColorLM;   // Piece Color LM
    std::string     m_strFX_SKIN_VS;        // Skin
    std::string     m_strFX_SkinObject;     // SkinObject
    std::string     m_strFX_Simple;         // Simple
    std::string     m_strDefindFX;
    std::string     m_strBaseFX;
    std::string     m_strVertexShaderFX;
    std::string     m_strDiffuseFX[2];
    std::string     m_strUserColorFX[2];
    std::string     m_strAddUserColorFX;
    std::string     m_strDiffusePowerFX;
    std::string     m_strAmbientPowerFX;
    std::string     m_strSpecularPowerFX;
    std::string     m_strShininessPowerFX;
    std::string     m_strLevelPowerFX;
    std::string     m_strLevelFlowPowerFX;
    std::string     m_strPowerEndFX;
    std::string     m_strSpecularFX[2];
    std::string     m_strLevelFX[2];
    std::string     m_strShininessFX[2];
    std::string     m_strShininessPowerTextureFX[2];    // 0 - Used Texture, 1 - pow
    std::string     m_strEmissiveFX[2];
	std::string     m_strStarGlowFX[2];
    std::string     m_strOpacityFX[2];
    std::string     m_strNormalFX[3];
    std::string     m_strNormalOtherFX;
    std::string     m_strReflectionVectorFX;
    std::string     m_strPixelShaderFX;
    std::string     m_strPixelShaderLevelFX;
    std::string     m_strPixelShaderFlowFX;
    std::string     m_strTechniqueFX[2];
    std::string     m_strTechniqueBaseFX[2];
    std::string     m_strTechniqueLevelFX[2];
    std::string     m_strTechniqueFlowFX[2];
    std::string     m_strTechniqueRealPLFX[2];
    std::string     m_strTechniqueLightMapFX[2];
	std::string     m_strTechniqueStarGlowFX[2];

private:
    BOOL            m_bTwoSideSeperatePass;
    VM_BLENDMODE    m_emBlendMode;
	BOOL			m_bActiveStarGlow;

public:
    HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice, 
                    const COMMENT_FX& strCommentFX,
                    size_t dwTextureNUM,
                    size_t nCubeTextureNUM,
                    size_t nPixelScalarNUM,
                    size_t nPixelVectorNUM,
                    VMFX emModeFX );
    HRESULT OnResetDevice();
    HRESULT OnLostDevice();

    ID3DXEffect* GetFX();
    const std::string& GetSourceString_Piece();
    const std::string& GetSourceString_PieceColor();
    const std::string& GetSourceString_PieceColorLM();
    const std::string& GetSourceString_SKIN_VS();
    const std::string& GetSourceString_SkinObject();
    const std::string& GetSourceString_Simple();

    BOOL GetTwoSideSeperatePass()   { return m_bTwoSideSeperatePass; }
    VM_BLENDMODE GetBlendMode()     { return m_emBlendMode; }
	BOOL IsActiveStarGlow()			{ return m_bActiveStarGlow; }

private:
    void SetStringFX_Common( BOOL bUserColorOverlay );
    void MakeStringFX_Common( const COMMENT_FX& sCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM,
                            std::string& strTexDefineFX, std::string& strTechOpacityFX, std::string& strTwoSidedFX, 
                            std::string& strShininessPowerTextureFX, std::string& strNormalFX_End );

    // FX를 만든다.
    void SetStringFX_Command();
    void SetStringFX_Piece();
    void SetStringFX_PieceColor();
    void SetStringFX_PieceColorLM();
    void SetStringFX_SkinVS( BOOL bMirror );
    void SetStringFX_SkinObject( BOOL bMirror );

    // FX를 최종적으로 조합한다.
    void MakeStringFX_Command( const COMMENT_FX& strCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM );
    void MakeStringFX_Piece( TSTRING& strFX, const COMMENT_FX& strCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM );
    void MakeStringFX_PieceColor( TSTRING& strFX, const COMMENT_FX& strCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM );
    void MakeStringFX_PieceColorLM( TSTRING& strFX, const COMMENT_FX& strCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM );
    void MakeStringFX_SkinVS( const COMMENT_FX& strCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM );
    void MakeStringFX_SkinObject( TSTRING& strFX, const COMMENT_FX& strCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM );
    void MakeStringFX_Simple( const COMMENT_FX& strCommentFX, size_t nTextureNUM, size_t nCubeTextureNUM, size_t nPixelScalarNUM, size_t nPixelVectorNUM );

public:
    void CleanUp();

public:
	DxVMCommandFX(void);
	virtual ~DxVMCommandFX(void);
};