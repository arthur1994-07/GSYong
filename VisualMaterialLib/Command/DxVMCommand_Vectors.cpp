#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"
#include "../Command_EX/DxVMCommand_EX.h"
#include "../Edit/DxEditVM.h"

#include "DxVMCommand_Vectors.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_ReflectionVector
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_ReflectionVector::DxVMCommand_ReflectionVector(void)
{ 
    m_emType = VMC_REFLECTION_VECTOR;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_ReflectionVector::~DxVMCommand_ReflectionVector(void)
{
}

BOOL DxVMCommand_ReflectionVector::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Reflection Vector") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_ReflectionVector::Refresh()
{
}

std::string DxVMCommand_ReflectionVector::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                            DWORD dwOutputIndex, 
                                            vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                            vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                            vm::EM_WHERE_CODE& emWhereCode )
{
    Refresh();

    // emWhereCode »Æ¿Œ
    {
        emWhereCode = vm::EMWC_HLSL;
    }

    return "GetReflectionVector(In)";
}

void DxVMCommand_ReflectionVector::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_ReflectionVector::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Load( pSFile );

    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }

    Refresh();
}