#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"
#include "../Command_EX/DxVMCommand_EX.h"
#include "../Edit/DxEditVM.h"
#include "./NSVMCommand.h"

#include "DxVMCommand_VectorOps.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Fresnel
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_Fresnel::DxVMCommand_Fresnel(void)
    : m_fExponent(1.f)
{ 
    m_emType = VMC_FRESNEL;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Fresnel::~DxVMCommand_Fresnel(void)
{
}

BOOL DxVMCommand_Fresnel::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Fresnel") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddChannel(_T("Normal"));
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Fresnel::Refresh()
{
}

std::string DxVMCommand_Fresnel::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                            DWORD dwOutputIndex, 
                                            vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                            vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                            vm::EM_WHERE_CODE& emWhereCode )
{
    Refresh();

    if ( m_vecLineData.size() >= 2 )
    {
        return "";
    }

    std::string strNormal("");
    std::string strStringFX_A("");
    vm::EM_WHERE_CODE emWhereCode_A(vm::EMWC_NEUTRAL);
    MAP_VM_COMMAND_CITER iter_0;
    if ( m_vecLineData.size() == 1 )
    {
        iter_0 = mapVM_Command.find( m_vecLineData[0]->m_dwID_Output );
        if ( iter_0 != mapVM_Command.end() )
        {
            strStringFX_A = (*iter_0).second->MakeStringFX( mapVM_Command, 
                                                            m_vecLineData[0]->m_nIndexBox_Output, 
                                                            vecTextureResource, 
                                                            vecCubeTextureResource,
                                                            vecPixelScalar, 
                                                            vecPixelVector,
                                                            emWhereCode_A );
        }

        // Scalar가 종료된다.
        if ( ( emWhereCode_A == vm::EMWC_SCALAR ) )
        {
            // 스칼라 이름으로 작업
            strStringFX_A = NSVMCommand::GetPixelScalarName( (*iter_0).second.get(), 
                                                                mapVM_Command, 
                                                                vecPixelScalar );
        }
        // Vector가 종료된다.
        else if ( emWhereCode == vm::EMWC_VECTOR )
        {
            // Vector 이름으로 작업
            strStringFX_A = NSVMCommand::GetUniformPixelVectorName( (*iter_0).second.get(), 
                                                                    mapVM_Command,
                                                                    vecPixelVector );
        }

        strNormal = strStringFX_A;
    }
    else    // m_vecLineData.size() == 0
    {
        strNormal = "g_vWorldNormal";
    }

    // emWhereCode 확인
    {
        emWhereCode = vm::EMWC_HLSL;
    }

    std::stringstream strStream;
	strStream << " pow(((1.00000000) - max((0.00000000),dot(In.vEyeVector,";
    strStream << strNormal;
    strStream << "))),(";
    strStream << m_fExponent;
    strStream << ")) ";

    return strStream.str();
}

void DxVMCommand_Fresnel::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << m_fExponent;
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Fresnel::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Load( pSFile );

    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        (*pSFile) >> m_fExponent;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }

    Refresh();
}