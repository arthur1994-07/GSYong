#include "stdafx.h"

#include "../../InternalCommonLib/Interface/TextureManagerInterface.h"
#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "../Command_EX/DxVMCommand_EX.h"
#include "../HLSL/DxVMCommandFX.h"
#include "../Preview/NSVM_PREVIEW.h"
#include "../DxVisualMaterial.h"

#include "VisualMaterialFX.h"

// ----------------------------------------------------------------------------
#include "../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


//------------------------------------------------------------------
//                  VisualMaterialFX
VisualMaterialFX::VisualMaterialFX(void)
    : m_pFX(NULL)
    , m_strFX(_T(""))
    , m_bTwoSideSeperatePass(FALSE)
    , m_emBlendMode(VM_BLENDMODE_OPAQUE)
	, m_bActiveStarGlow(FALSE)
    , m_cLastWriteTime(0)
{
}

VisualMaterialFX::~VisualMaterialFX(void)
{
    CleanUp();
}

void VisualMaterialFX::CleanUp()
{
    SAFE_RELEASE( m_pFX );

    m_vecTextureResource.clear();
    m_vecCubeTextureResource.clear();
}

HRESULT VisualMaterialFX::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
    HRESULT hr(S_OK);
    if( m_pFX )	
        V_RETURN( m_pFX->OnResetDevice() );

    for ( DWORD i=0; i<m_vecTextureResource.size(); ++i )
    {
        m_vecTextureResource[i].m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
            m_vecTextureResource[i].m_strName,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_VISUALMATERIAL,
			false,
			FALSE );
    }

    for ( DWORD i=0; i<m_vecCubeTextureResource.size(); ++i )
    {
        m_vecCubeTextureResource[i].m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
            m_vecCubeTextureResource[i].m_strName,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_VISUALMATERIAL,
			false,
			FALSE );
    }

    return hr;
}

HRESULT VisualMaterialFX::OnLostDevice()
{
    HRESULT hr(S_OK);
    if( m_pFX )	
        V_RETURN( m_pFX->OnLostDevice() );

    return hr;
}

void VisualMaterialFX::SetTexturePointerNULL()
{
    for ( DWORD i=0; i<m_vecTextureResource.size(); ++i )
    {
        if( m_vecTextureResource[i].m_textureRes.IsValid() )
        {
            ic::TextureManagerInterface::GetInstance()->DeleteData(
                m_vecTextureResource[i].m_strName );
        }

        m_vecTextureResource[i].m_textureRes.Clear();
    }

    for ( DWORD i=0; i<m_vecCubeTextureResource.size(); ++i )
    {
        if( m_vecCubeTextureResource[i].m_textureRes.IsValid() )
        {
            ic::TextureManagerInterface::GetInstance()->DeleteData(
                m_vecCubeTextureResource[i].m_strName );
        }

        m_vecCubeTextureResource[i].m_textureRes.Clear();
    }
}

ID3DXEffect* VisualMaterialFX::GetFX()
{
    return m_pFX;
}

DWORD VisualMaterialFX::GetTextureSize()
{
    return static_cast< DWORD >( m_vecTextureResource.size() );
}

const TCHAR* VisualMaterialFX::GetTextureName( size_t i )
{
    if ( i >= m_vecTextureResource.size() )
        return NULL;

    return m_vecTextureResource[i].m_strName.c_str();
}

LPDIRECT3DTEXTUREQ VisualMaterialFX::GetTexture( size_t i )
{
    if ( i >= m_vecTextureResource.size() )
        return NULL;

    return m_vecTextureResource[i].m_textureRes.GetTexture();
}

DWORD VisualMaterialFX::GetCubeTextureSize()
{
    return static_cast< DWORD >( m_vecCubeTextureResource.size() );
}

LPDIRECT3DCUBETEXTUREQ VisualMaterialFX::GetCubeTexture( size_t i )
{
    if ( i >= m_vecCubeTextureResource.size() )
        return NULL;

    return m_vecCubeTextureResource[i].m_textureRes.GetCubeTexture();
}

void VisualMaterialFX::FrameMove( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData, float fTime )
{
    if ( !m_pFX )
        return;

    // g_fUniformPixelScalar_0 ... 설정
    {
        char c1='0';
        std::string strUniformPixelScalarORIG = "g_fUniformPixelScalar_";
        std::string strUniformPixelScalar = "";
        for ( size_t i=0; i<m_vecPixelScalar.size(); ++i )
        {
            m_vecPixelScalar[i]->FrameMove( fTime );

            strUniformPixelScalar = strUniformPixelScalarORIG + static_cast<char>( c1 + i );
            m_pFX->SetFloat( strUniformPixelScalar.c_str(), fabsf( m_vecPixelScalar[i]->GetPixelVector( mapParamaterNameData ).x ) );
        }
    }

    // g_vUniformPixelVector_0 ... 설정
    {
        char c1='0';
        std::string strParameterORIG = "g_vUniformPixelVector_";
        std::string strParameter = "";
        for ( size_t i=0; i<m_vecPixelVector.size(); ++i )
        {
            m_vecPixelVector[i]->FrameMove( fTime );

            strParameter = strParameterORIG + static_cast<char>( c1 + i );
            m_pFX->SetVector( strParameter.c_str(), &m_vecPixelVector[i]->GetPixelVector( mapParamaterNameData ) );
        }
    }
}

HRESULT VisualMaterialFX::Load( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strFileName, ic::SerialFileInterface* pSFile, NS_VM_MANAGER::RENDER_TYPE emRenderTYPE )
{
    CleanUp();


    DWORD dwVer, dwVerFX, dwBufferSize(0);
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == 0x105 )
    {
        (*pSFile) >> dwVerFX;

        if ( dwVerFX == DxVisualMaterial::VERSION_FX )
        {
            DWORD dwJumpBufferSize;
            (*pSFile) >> dwJumpBufferSize;
            
            (*pSFile).SetOffSet( (*pSFile).GetfTell() + dwJumpBufferSize );

            // HLSL
            {
                TSTRING strObjectFX;
                TSTRING strObjectColorFX;
                TSTRING strObjectColorLMFX;
                TSTRING strSkinVSFX;
                TSTRING strSkinObjectFX;
                (*pSFile) >> strObjectFX;
                (*pSFile) >> strObjectColorFX;
                (*pSFile) >> strObjectColorLMFX;
                (*pSFile) >> strSkinVSFX;
                (*pSFile) >> strSkinObjectFX;

                switch ( emRenderTYPE )
                {
                case NS_VM_MANAGER::EMRT_OBJECT:
                    m_strFX = strObjectFX;
                    break;

                case NS_VM_MANAGER::EMRT_OBJECT_COLOR:
                    m_strFX = strObjectColorFX;
                    break;

                case NS_VM_MANAGER::EMRT_OBJECT_COLOR_LM:
                    m_strFX = strObjectColorLMFX;
                    break;

                case NS_VM_MANAGER::EMRT_SKIN_VS:
                    m_strFX = strSkinVSFX;
                    break;

                case NS_VM_MANAGER::EMRT_SKIN_OBJECT:
                    m_strFX = strSkinObjectFX;
                    break;
                };

                DWORD dwSize(0);
                (*pSFile) >> dwSize;
                for ( DWORD i=0; i<dwSize; ++i )
                {
                    vm::NAME_TEXTURE sTextureResource;
                    (*pSFile) >> sTextureResource.m_strName;

                    sTextureResource.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                        sTextureResource.m_strName,
                        false,
                        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                        0,
                        D3DFMT_UNKNOWN,
                        TEXTURE_RESIZE_NONE,
                        TEXTURE_VISUALMATERIAL,
						false,
						FALSE );

                    m_vecTextureResource.push_back( sTextureResource );
                }

                (*pSFile) >> dwSize;
                for ( DWORD i=0; i<dwSize; ++i )
                {
                    vm::NAME_CUBE_TEXTURE sTextureResource;
                    (*pSFile) >> sTextureResource.m_strName;

                    sTextureResource.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                        sTextureResource.m_strName,
                        false,
                        TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE,
                        0,
                        D3DFMT_UNKNOWN,
                        TEXTURE_RESIZE_NONE,
                        TEXTURE_VISUALMATERIAL,
						false,
						FALSE );

                    m_vecCubeTextureResource.push_back( sTextureResource );
                }

                HRESULT hr(S_OK);
                UINT dwBufferSize = static_cast<UINT>( m_strFX.size() ) + 1; 
                V_RETURN( D3DXCreateEffect( pd3dDevice, m_strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pFX, NULL ) );
            }

            // PixelScalar
            {
                DWORD dwSize(0);
                DWORD dwType(0);
                (*pSFile) >> dwSize;
                for ( DWORD i=0; i<dwSize; ++i )
                {
                    (*pSFile) >> dwType;
                    std::tr1::shared_ptr<DxVMCommand_EX> spNew = DxVMCommand_EX::NEW_COMMAND_STATIC( static_cast<VMC_TYPE>( dwType ) );
                    spNew->Load( &(*pSFile) );
                    m_vecPixelScalar.push_back( spNew );
                }
            }

            // PixelVector
            {
                DWORD dwSize(0);
                DWORD dwType(0);
                (*pSFile) >> dwSize;
                for ( DWORD i=0; i<dwSize; ++i )
                {
                    (*pSFile) >> dwType;
                    std::tr1::shared_ptr<DxVMCommand_EX> spNew = DxVMCommand_EX::NEW_COMMAND_STATIC( static_cast<VMC_TYPE>( dwType ) );
                    spNew->Load( &(*pSFile) );
                    m_vecPixelVector.push_back( spNew );
                }
            }

            // Etc
            {
                DWORD dwTemp;
                (*pSFile) >> m_bTwoSideSeperatePass;
                (*pSFile) >> dwTemp;                    m_emBlendMode = static_cast<VM_BLENDMODE>( dwTemp );
				(*pSFile) >> m_bActiveStarGlow;
            }
        }
        else
        {
            if ( FAILED( LoadDxVisualMaterial( pd3dDevice, strFileName, emRenderTYPE ) ) )
			{
				return E_FAIL;
			}
        }
    }
	else if ( dwVer == 0x104 )
	{
		(*pSFile) >> dwVerFX;

		if ( dwVerFX == 0x106 )
		{
			DWORD dwJumpBufferSize;
			(*pSFile) >> dwJumpBufferSize;

			(*pSFile).SetOffSet( (*pSFile).GetfTell() + dwJumpBufferSize );

			// HLSL
			{
				TSTRING strObjectFX;
				TSTRING strObjectColorFX;
				TSTRING strObjectColorLMFX;
				TSTRING strSkinVSFX;
				TSTRING strSkinObjectFX;
				(*pSFile) >> strObjectFX;
				(*pSFile) >> strObjectColorFX;
				(*pSFile) >> strObjectColorLMFX;
				(*pSFile) >> strSkinVSFX;
				(*pSFile) >> strSkinObjectFX;

				switch ( emRenderTYPE )
				{
				case NS_VM_MANAGER::EMRT_OBJECT:
					m_strFX = strObjectFX;
					break;

				case NS_VM_MANAGER::EMRT_OBJECT_COLOR:
					m_strFX = strObjectColorFX;
					break;

				case NS_VM_MANAGER::EMRT_OBJECT_COLOR_LM:
					m_strFX = strObjectColorLMFX;
					break;

				case NS_VM_MANAGER::EMRT_SKIN_VS:
					m_strFX = strSkinVSFX;
					break;

				case NS_VM_MANAGER::EMRT_SKIN_OBJECT:
					m_strFX = strSkinObjectFX;
					break;
				};

				DWORD dwSize(0);
				(*pSFile) >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
					vm::NAME_TEXTURE sTextureResource;
					(*pSFile) >> sTextureResource.m_strName;

					sTextureResource.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
						sTextureResource.m_strName,
						false,
						TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
						0,
						D3DFMT_UNKNOWN,
						TEXTURE_RESIZE_NONE,
						TEXTURE_VISUALMATERIAL,
						false,
						FALSE );

					m_vecTextureResource.push_back( sTextureResource );
				}

				(*pSFile) >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
					vm::NAME_CUBE_TEXTURE sTextureResource;
					(*pSFile) >> sTextureResource.m_strName;

					sTextureResource.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
						sTextureResource.m_strName,
						false,
						TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE,
						0,
						D3DFMT_UNKNOWN,
						TEXTURE_RESIZE_NONE,
						TEXTURE_VISUALMATERIAL,
						false,
						FALSE );

					m_vecCubeTextureResource.push_back( sTextureResource );
				}

				HRESULT hr(S_OK);
				UINT dwBufferSize = static_cast<UINT>( m_strFX.size() ) + 1; 
				V_RETURN( D3DXCreateEffect( pd3dDevice, m_strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pFX, NULL ) );
			}

			// PixelScalar
			{
				DWORD dwSize(0);
				DWORD dwType(0);
				(*pSFile) >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
					(*pSFile) >> dwType;
					std::tr1::shared_ptr<DxVMCommand_EX> spNew = DxVMCommand_EX::NEW_COMMAND_STATIC( static_cast<VMC_TYPE>( dwType ) );
					spNew->Load( &(*pSFile) );
					m_vecPixelScalar.push_back( spNew );
				}
			}

			// PixelVector
			{
				DWORD dwSize(0);
				DWORD dwType(0);
				(*pSFile) >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
					(*pSFile) >> dwType;
					std::tr1::shared_ptr<DxVMCommand_EX> spNew = DxVMCommand_EX::NEW_COMMAND_STATIC( static_cast<VMC_TYPE>( dwType ) );
					spNew->Load( &(*pSFile) );
					m_vecPixelVector.push_back( spNew );
				}
			}

			// Etc
			{
				DWORD dwTemp;
				(*pSFile) >> m_bTwoSideSeperatePass;
				(*pSFile) >> dwTemp;                    m_emBlendMode = static_cast<VM_BLENDMODE>( dwTemp );
			}
		}
		else
		{
			if ( FAILED( LoadDxVisualMaterial( pd3dDevice, strFileName, emRenderTYPE ) ) )
			{
				return E_FAIL;
			}
		}
	}
    else
    {
		if ( FAILED( LoadDxVisualMaterial( pd3dDevice, strFileName, emRenderTYPE ) ) )
		{
			return E_FAIL;
		}

        (*pSFile).SetOffSet ( (*pSFile).GetfTell()+dwBufferSize );
    }

    return S_OK;
}

HRESULT VisualMaterialFX::LoadDxVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strFileName, NS_VM_MANAGER::RENDER_TYPE emRenderTYPE )
{
    DxVisualMaterial sVisualMaterial;
    sVisualMaterial.Create( pd3dDevice );
    if ( !sVisualMaterial.Load( pd3dDevice, strFileName ) )
		return E_FAIL;

    sVisualMaterial.CreateDataFX();

    // HLSL 관련
    {
        switch ( emRenderTYPE )
        {
        case NS_VM_MANAGER::EMRT_OBJECT:
            m_strFX = NSVM_PREVIEW::g_spVMCommandFX->GetSourceString_Piece();
            break;

        case NS_VM_MANAGER::EMRT_OBJECT_COLOR:
            m_strFX = NSVM_PREVIEW::g_spVMCommandFX->GetSourceString_PieceColor();
            break;

        case NS_VM_MANAGER::EMRT_OBJECT_COLOR_LM:
            m_strFX = NSVM_PREVIEW::g_spVMCommandFX->GetSourceString_PieceColorLM();
            break;

        case NS_VM_MANAGER::EMRT_SKIN_VS:
            m_strFX = NSVM_PREVIEW::g_spVMCommandFX->GetSourceString_SKIN_VS();
            break;

        case NS_VM_MANAGER::EMRT_SKIN_OBJECT:
            m_strFX = NSVM_PREVIEW::g_spVMCommandFX->GetSourceString_SkinObject();
            break;
        };

        for ( DWORD i=0; i<NSVM_PREVIEW::g_vecTextureResource.size(); ++i )
        {
            vm::NAME_TEXTURE sTextureResource;
            sTextureResource.m_strName = NSVM_PREVIEW::g_vecTextureResource[i].m_strName;

            sTextureResource.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sTextureResource.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
				false,
				FALSE );

            m_vecTextureResource.push_back( sTextureResource );
        }

        for ( DWORD i=0; i<NSVM_PREVIEW::g_vecCubeTextureResource.size(); ++i )
        {
            vm::NAME_CUBE_TEXTURE sTextureResource;
            sTextureResource.m_strName = NSVM_PREVIEW::g_vecCubeTextureResource[i].m_strName;

            sTextureResource.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sTextureResource.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
				false,
				FALSE );

            m_vecCubeTextureResource.push_back( sTextureResource );
        }

        HRESULT hr(S_OK);
        UINT dwBufferSize = static_cast<UINT>( m_strFX.size() ) + 1; 
        V_RETURN( D3DXCreateEffect( pd3dDevice, m_strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pFX, NULL ) );
    }

    // PixelScalar
    {
        for ( DWORD i=0; i<NSVM_PREVIEW::g_vecPixelScalar.size(); ++i )
        {
            std::tr1::shared_ptr<DxVMCommand_EX> spNew = NSVM_PREVIEW::g_vecPixelScalar[i];
            m_vecPixelScalar.push_back( spNew );
        }
    }

    // PixelVector
    {
        for ( DWORD i=0; i<NSVM_PREVIEW::g_vecPixelVector.size(); ++i )
        {
            std::tr1::shared_ptr<DxVMCommand_EX> spNew = NSVM_PREVIEW::g_vecPixelVector[i];
            m_vecPixelVector.push_back( spNew );
        }
    }

    // Etc
    {
        m_bTwoSideSeperatePass = NSVM_PREVIEW::g_spVMCommandFX->GetTwoSideSeperatePass();
        m_emBlendMode = NSVM_PREVIEW::g_spVMCommandFX->GetBlendMode();
		m_bActiveStarGlow = NSVM_PREVIEW::g_spVMCommandFX->IsActiveStarGlow();
    }

    return S_OK;
}