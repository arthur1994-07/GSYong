#include "stdafx.h"

#include "DxVMCommandFX.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



DxVMCommandFX::DxVMCommandFX()
    : m_pFX(NULL)
    , m_bTwoSideSeperatePass(FALSE)
    , m_emBlendMode(VM_BLENDMODE_OPAQUE)
	, m_bActiveStarGlow(FALSE)
{
}

DxVMCommandFX::~DxVMCommandFX()
{
    CleanUp();
}

void DxVMCommandFX::CleanUp()
{
    SAFE_RELEASE( m_pFX );
}

HRESULT DxVMCommandFX::LoadFX( LPDIRECT3DDEVICEQ pd3dDevice, 
                              const COMMENT_FX& sCommentFX,
                              size_t nTextureNUM,
                              size_t nCubeTextureNUM,
                              size_t nPixelScalarNUM,
                              size_t nPixelVectorNUM,
                              VMFX emModeFX )
{
    HRESULT hr(S_OK);

    CleanUp();

	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

	#ifdef DEBUG_VS
		dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
	#endif
	#ifdef DEBUG_PS
		dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
	#endif

    switch ( emModeFX )
    {
    case VMFX_COMMAND:
        SetStringFX_Command();
        MakeStringFX_Command( sCommentFX, nTextureNUM, nCubeTextureNUM, nPixelScalarNUM, nPixelVectorNUM );
        break;

    case VMFX_PIECE:
        // Object 용으로 만든다.
        {
            SetStringFX_Common( sCommentFX.m_bUserColorOverlay );
            SetStringFX_Piece();
            MakeStringFX_Piece( m_strFX_Piece, sCommentFX, nTextureNUM, nCubeTextureNUM, nPixelScalarNUM, nPixelVectorNUM );
        }

        // Object Color 용으로 만든다.
        {
            SetStringFX_Common( sCommentFX.m_bUserColorOverlay );
            SetStringFX_PieceColor();
            MakeStringFX_PieceColor( m_strFX_PieceColor, sCommentFX, nTextureNUM, nCubeTextureNUM, nPixelScalarNUM, nPixelVectorNUM );
        }

        // Object Color LM 용으로 만든다.
        {
            SetStringFX_Common( sCommentFX.m_bUserColorOverlay );
            SetStringFX_PieceColorLM();
            MakeStringFX_PieceColorLM( m_strFX_PieceColorLM, sCommentFX, nTextureNUM, nCubeTextureNUM, nPixelScalarNUM, nPixelVectorNUM );
        }

        // Skin Object 용으로 만든다.
        {
            SetStringFX_Common( sCommentFX.m_bUserColorOverlay );
            SetStringFX_SkinObject( sCommentFX.m_bCharUV_MIRROR );
            MakeStringFX_SkinObject( m_strFX_SkinObject, sCommentFX, nTextureNUM, nCubeTextureNUM, nPixelScalarNUM, nPixelVectorNUM );
        }

        // Skin VS 용으로 만든다.
        {
            SetStringFX_Common( sCommentFX.m_bUserColorOverlay );
            SetStringFX_SkinVS( sCommentFX.m_bCharUV_MIRROR );
            MakeStringFX_SkinVS( sCommentFX, nTextureNUM, nCubeTextureNUM, nPixelScalarNUM, nPixelVectorNUM );
        }

        // VisualMaterial에 보여주기 위한 정보
        {
            MakeStringFX_Simple( sCommentFX, nTextureNUM, nCubeTextureNUM, nPixelScalarNUM, nPixelVectorNUM );
        }

        break;
    };

    UINT dwBufferSize = static_cast<UINT>( m_strFX_Piece.size() ) + 1; 
    V_RETURN( D3DXCreateEffect( pd3dDevice, m_strFX_Piece.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pFX, NULL ) );

	return hr;
}

HRESULT DxVMCommandFX::OnResetDevice()
{
    HRESULT hr(S_OK);

    if( m_pFX )	
        V_RETURN( m_pFX->OnResetDevice() );

    return hr;
}

HRESULT DxVMCommandFX::OnLostDevice()
{
    HRESULT hr(S_OK);

    if( m_pFX )	
        V_RETURN( m_pFX->OnLostDevice() );

    return hr;
}

ID3DXEffect* DxVMCommandFX::GetFX()
{
    return m_pFX;
}

const std::string& DxVMCommandFX::GetSourceString_Piece()
{
    return m_strFX_Piece;
}

const std::string& DxVMCommandFX::GetSourceString_PieceColor()
{
    return m_strFX_PieceColor;
}

const std::string& DxVMCommandFX::GetSourceString_PieceColorLM()
{
    return m_strFX_PieceColorLM;
}

const std::string& DxVMCommandFX::GetSourceString_SKIN_VS()
{
    return m_strFX_SKIN_VS;
}

const std::string& DxVMCommandFX::GetSourceString_SkinObject()
{
    return m_strFX_SkinObject;
}

const std::string& DxVMCommandFX::GetSourceString_Simple()
{
    return m_strFX_Simple;
}







