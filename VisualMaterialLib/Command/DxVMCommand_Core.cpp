#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "../Preview/NSVM_PREVIEW.h"
#include "../Edit/DxEditVM.h"
#include "./NSVMCommand.h"

#include "DxVMCommand_Core.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Core
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_Core::DxVMCommand_Core(void)
    : m_fOpacityMaskClipValue(0.333f)
    , m_nBlendMode(VM_BLENDMODE_OPAQUE)
    , m_nTwoSidedMode(VM_TWOSIDEDMODE_OFF)
    , m_bWireframe(FALSE)
    , m_bCharUV_MIRROR(FALSE)
    , m_bUserColorOverlay(FALSE)
    , m_fDiffusePower(1.f)
    , m_fAmbientPower(1.f)
    , m_fSpecularPower(1.f)
    , m_bShininessPowerTexture(TRUE)
    , m_fShininessPower(20.f)
    , m_fLevelPower(1.5f)
    , m_fLevelFlowPower(0.7f)
    , m_bDisableDepthTest(FALSE)
    , m_strPreviewMesh(_T(""))
{
    m_emType = VMC_CORE;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_CORE_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_CORE_HEIGHT;
}

DxVMCommand_Core::~DxVMCommand_Core(void)
{
}

BOOL DxVMCommand_Core::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Material") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );     // 0
    m_spEditVM_Command->AddChannel(_T("Diffuse"));

    m_spEditVM_Command->Create( pd3dDevice, FALSE );     // 1
    m_spEditVM_Command->AddChannel(_T("Emissive"));

	m_spEditVM_Command->Create( pd3dDevice, FALSE );     // 2
	m_spEditVM_Command->AddChannel(_T("StarGlow"));

    m_spEditVM_Command->Create( pd3dDevice, FALSE );     // 3
    m_spEditVM_Command->AddChannel(_T("Specular"));

    m_spEditVM_Command->Create( pd3dDevice, FALSE );     // 4
    m_spEditVM_Command->AddChannel(_T("Shininess"));

    m_spEditVM_Command->Create( pd3dDevice, FALSE );     // 5
    m_spEditVM_Command->AddChannel(_T("Opacity"));

    m_spEditVM_Command->Create( pd3dDevice, FALSE );     // 6
    m_spEditVM_Command->AddChannel(_T("Normal"));

    m_spEditVM_Command->Create( pd3dDevice, FALSE );     // 7
    m_spEditVM_Command->AddChannel(_T("Level"));

    m_spEditVM_Command->Create( pd3dDevice, FALSE );     // 8
    m_spEditVM_Command->AddChannel(_T("UserColorMask1"));

    m_spEditVM_Command->Create( pd3dDevice, FALSE );     // 9
    m_spEditVM_Command->AddChannel(_T("UserColorMask2"));

    return TRUE;
}

void DxVMCommand_Core::Refresh()
{
    NSVM_PREVIEW::LoadCustomMesh( m_strPreviewMesh );
}

std::string DxVMCommand_Core::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                            DWORD dwOutputIndex,
                                            vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                            vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                            vm::EM_WHERE_CODE& emWhereCode )
{
    // Sting을 만든다.
    std::string strStringFX("");

    for ( DWORD i=0; i<m_vecLineData.size(); ++i )
    {
        if ( m_vecLineData[i]->m_nIndexBox_Input == dwOutputIndex )
        {
            MAP_VM_COMMAND_CITER iter = mapVM_Command.find( m_vecLineData[i]->m_dwID_Output );
            if ( iter != mapVM_Command.end() )
            {
                strStringFX = (*iter).second->MakeStringFX( mapVM_Command, 
                                                            m_vecLineData[i]->m_nIndexBox_Output,
                                                            vecTextureResource, 
                                                            vecCubeTextureResource,
                                                            vecPixelScalar, 
                                                            vecPixelVector,
                                                            emWhereCode );

                // Scalar라면,
                if ( emWhereCode == vm::EMWC_SCALAR )
                {
                    // 스칼라 이름으로 작업
                    strStringFX = NSVMCommand::GetPixelScalarName( (*iter).second.get(), 
                                                                    mapVM_Command,
                                                                    vecPixelScalar );

                    // 셋팅을 했으므로 초기화 한다.
                    emWhereCode = vm::EMWC_NEUTRAL;
                }
                // Vector라면,
                else if ( emWhereCode == vm::EMWC_VECTOR )
                {
                    // Vector 이름으로 작업
                    strStringFX = NSVMCommand::GetUniformPixelVectorName( (*iter).second.get(), 
                                                                            mapVM_Command,
                                                                            vecPixelVector );

                    // 셋팅을 했으므로 초기화 한다.
                    emWhereCode = vm::EMWC_NEUTRAL;
                }
            }
            break;
        }
    }    

    return strStringFX;
}

std::string DxVMCommand_Core::GetStringFX_PowerValue( int nIndex )
{
    std::stringstream strStream;
    std::string strFX("");

    switch ( nIndex )
    {
    case 0: strStream << m_fDiffusePower;   break;
    case 1: strStream << m_fAmbientPower;   break;
    case 2: strStream << m_fSpecularPower;  break;
    case 3: 
        if ( m_bShininessPowerTexture )
        {
            strStream << m_fShininessPower/200.f;
        }
        else
        {
            strStream << m_fShininessPower; 
        }
        break;
    case 4: strStream << m_fLevelPower;     break;
    case 5: strStream << m_fLevelFlowPower; break;
    };

    strFX = static_cast<std::string>("    return ") + strStream.str() + "; \r\n";
    return strFX;
}

std::string DxVMCommand_Core::GetStringFX_Wireframe()
{
    if ( m_bWireframe )
    {
        return "	FillMode = Wireframe; \r\n";
    }
    else
    {
        return "";
    }
}

std::string DxVMCommand_Core::GetStringFX_AlphaREF()
{
    DWORD dwValue = static_cast<int>( m_fOpacityMaskClipValue*255.f );
    if ( dwValue > 255 )    
        dwValue = 255;

    std::stringstream strStream;
	strStream << dwValue;

    std::string strFX("");
    strFX = static_cast<std::string>("        ALPHAREF = ") + strStream.str() + "; \r\n";
    return strFX;
}

std::string DxVMCommand_Core::GetStringFX_DisableDepthTest()
{
    if ( m_bDisableDepthTest )
    {
        return "	ZENABLE = FALSE; \r\n";
    }
    else
    {
        return "";
    }
}

void DxVMCommand_Core::SetCommentFX( COMMENT_FX& sCommentFX )
{
    sCommentFX.m_strDiffusePowerFX      = GetStringFX_PowerValue( 0 );
    sCommentFX.m_strAmbientPowerFX      = GetStringFX_PowerValue( 1 );
    sCommentFX.m_strSpecularPowerFX     = GetStringFX_PowerValue( 2 );
    sCommentFX.m_strShininessPowerFX    = GetStringFX_PowerValue( 3 );
    sCommentFX.m_strLevelPowerFX        = GetStringFX_PowerValue( 4 );
    sCommentFX.m_strLevelFlowPowerFX    = GetStringFX_PowerValue( 5 );
    sCommentFX.m_strWireframeFX         = GetStringFX_Wireframe();
    sCommentFX.m_strAlphaREFFX          = GetStringFX_AlphaREF();
    sCommentFX.m_strDisableDepthTest    = GetStringFX_DisableDepthTest();
    sCommentFX.m_emBlendMode            = static_cast<VM_BLENDMODE>( m_nBlendMode );
    sCommentFX.m_emTwoSidedMode         = static_cast<VM_TWOSIDEDMODE>( m_nTwoSidedMode );
    sCommentFX.m_bShininessPowerTexture = m_bShininessPowerTexture;
    sCommentFX.m_bCharUV_MIRROR         = m_bCharUV_MIRROR;
    sCommentFX.m_bUserColorOverlay      = m_bUserColorOverlay;
}

void DxVMCommand_Core::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        // Material
        (*pSFile) << m_fOpacityMaskClipValue;
        (*pSFile) << m_nBlendMode;              // VM_BLENDMODE
        (*pSFile) << m_nTwoSidedMode;
        (*pSFile) << m_bWireframe;
        (*pSFile) << m_bCharUV_MIRROR;
        (*pSFile) << m_bUserColorOverlay;

        // MaterialColorPower
        (*pSFile) << m_fDiffusePower;
        (*pSFile) << m_fAmbientPower;
        (*pSFile) << m_fSpecularPower;
        (*pSFile) << m_bShininessPowerTexture;
        (*pSFile) << m_fShininessPower;
        (*pSFile) << m_fLevelPower;
        (*pSFile) << m_fLevelFlowPower;

        // Translucency
        (*pSFile) << m_bDisableDepthTest;

        // MaterialInterface
        (*pSFile) << m_strPreviewMesh;
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Core::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Load( pSFile );

    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        // Material
        (*pSFile) >> m_fOpacityMaskClipValue;
        (*pSFile) >> m_nBlendMode;              // VM_BLENDMODE
        (*pSFile) >> m_nTwoSidedMode;
        (*pSFile) >> m_bWireframe;
        (*pSFile) >> m_bCharUV_MIRROR;
        (*pSFile) >> m_bUserColorOverlay;

        // MaterialColorPower
        (*pSFile) >> m_fDiffusePower;
        (*pSFile) >> m_fAmbientPower;
        (*pSFile) >> m_fSpecularPower;
        (*pSFile) >> m_bShininessPowerTexture;
        (*pSFile) >> m_fShininessPower;
        (*pSFile) >> m_fLevelPower;
        (*pSFile) >> m_fLevelFlowPower;

        // Translucency
        (*pSFile) >> m_bDisableDepthTest;

        // MaterialInterface
        (*pSFile) >> m_strPreviewMesh;
    }
    else if ( dwVer == 0x103 )
    {
        // Material
        (*pSFile) >> m_fOpacityMaskClipValue;
        (*pSFile) >> m_nBlendMode;              // VM_BLENDMODE
        (*pSFile) >> m_nTwoSidedMode;
        (*pSFile) >> m_bWireframe;
        (*pSFile) >> m_bCharUV_MIRROR;

        // MaterialColorPower
        (*pSFile) >> m_fDiffusePower;
        (*pSFile) >> m_fAmbientPower;
        (*pSFile) >> m_fSpecularPower;
        (*pSFile) >> m_bShininessPowerTexture;
        (*pSFile) >> m_fShininessPower;
        (*pSFile) >> m_fLevelPower;
        (*pSFile) >> m_fLevelFlowPower;

        // Translucency
        (*pSFile) >> m_bDisableDepthTest;

        // MaterialInterface
        (*pSFile) >> m_strPreviewMesh;
    }
    else if ( dwVer == 0x102 )
    {
        // Material
        (*pSFile) >> m_fOpacityMaskClipValue;
        (*pSFile) >> m_nBlendMode;              // VM_BLENDMODE
        (*pSFile) >> m_nTwoSidedMode;
        (*pSFile) >> m_bWireframe;

        // MaterialColorPower
        (*pSFile) >> m_fDiffusePower;
        (*pSFile) >> m_fAmbientPower;
        (*pSFile) >> m_fSpecularPower;
        (*pSFile) >> m_bShininessPowerTexture;
        (*pSFile) >> m_fShininessPower;
        (*pSFile) >> m_fLevelPower;
        (*pSFile) >> m_fLevelFlowPower;

        // Translucency
        (*pSFile) >> m_bDisableDepthTest;

        // MaterialInterface
        (*pSFile) >> m_strPreviewMesh;
    }
    else if ( dwVer == 0x101 )
    {
        // Material
        (*pSFile) >> m_fOpacityMaskClipValue;
        (*pSFile) >> m_nBlendMode;              // VM_BLENDMODE
        (*pSFile) >> m_nTwoSidedMode;
        (*pSFile) >> m_bWireframe;

        // MaterialColorPower
        (*pSFile) >> m_fDiffusePower;
        (*pSFile) >> m_fAmbientPower;
        (*pSFile) >> m_fSpecularPower;
        (*pSFile) >> m_bShininessPowerTexture;
        (*pSFile) >> m_fShininessPower;

        // Translucency
        (*pSFile) >> m_bDisableDepthTest;

        // MaterialInterface
        (*pSFile) >> m_strPreviewMesh;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }

    Refresh();
}
