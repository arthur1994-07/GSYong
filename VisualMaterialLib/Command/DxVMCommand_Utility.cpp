#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"
#include "../Command_EX/DxVMCommand_EX.h"
#include "../Edit/DxEditVM.h"
#include "./NSVMCommand.h"

#include "DxVMCommand_Utility.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------




//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Clamp
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_Clamp
DxVMCommand_Clamp::DxVMCommand_Clamp(void)
{ 
    m_emType = VMC_CLAMP;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Clamp::~DxVMCommand_Clamp(void)
{
}

BOOL DxVMCommand_Clamp::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Clamp") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddChannel(_T("Sorce"));
    m_spEditVM_Command->AddChannel(_T("Min"));
    m_spEditVM_Command->AddChannel(_T("Max"));
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Clamp::Refresh()
{
}

std::string DxVMCommand_Clamp::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                            DWORD dwOutputIndex, 
                                            vm::VEC_TEXTURE_RESOURCE& vecTextureResource,
                                            vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                            vm::EM_WHERE_CODE& emWhereCode )
{
    if ( m_vecLineData.size() != 3 )
    {
        return "";
    }

    std::string strStringFX[3];
    vm::EM_WHERE_CODE emWhereCodeArray[3] = { vm::EMWC_NEUTRAL, vm::EMWC_NEUTRAL, vm::EMWC_NEUTRAL };
    {
        NSVMCommand::MakeStringFX
        ( 
            mapVM_Command, 
            m_vecLineData, 
            vecTextureResource,
            vecCubeTextureResource,
            vecPixelScalar, 
            vecPixelVector,
            strStringFX, 
            emWhereCodeArray
        );
    }

    // emWhereCode 확인
    {
        emWhereCode = vm::EMWC_HLSL;
    }

    // Scalar가 종료된다.
    {
        NSVMCommand::PixelScalarName
        (
            mapVM_Command, 
            m_vecLineData,
            vecPixelScalar, 
            vecPixelVector,
            strStringFX, 
            emWhereCodeArray
        );
    }

    std::string strFX = "min(max((";
    strFX += strStringFX[0] + "),(" + strStringFX[1] + ")),(" + strStringFX[2] + "))";

    return strFX;
}

void DxVMCommand_Clamp::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Clamp::Load( ic::SerialFileInterface* pSFile )
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



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_LinearInterpolate
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_LinearInterpolate
DxVMCommand_LinearInterpolate::DxVMCommand_LinearInterpolate(void)
{ 
    m_emType = VMC_LINEAR_INTERPOLATE;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_LinearInterpolate::~DxVMCommand_LinearInterpolate(void)
{
}

BOOL DxVMCommand_LinearInterpolate::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Lerp") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddChannel(_T("A"));
    m_spEditVM_Command->AddChannel(_T("B"));
    m_spEditVM_Command->AddChannel(_T("Alpha"));
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_LinearInterpolate::Refresh()
{
}

std::string DxVMCommand_LinearInterpolate::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                            DWORD dwOutputIndex, 
                                            vm::VEC_TEXTURE_RESOURCE& vecTextureResource,
                                            vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                            vm::EM_WHERE_CODE& emWhereCode )
{
    if ( m_vecLineData.size() != 3 )
    {
        return "";
    }

    std::string strStringFX[3];
    vm::EM_WHERE_CODE emWhereCodeArray[3] = { vm::EMWC_NEUTRAL, vm::EMWC_NEUTRAL, vm::EMWC_NEUTRAL };
    {
        NSVMCommand::MakeStringFX
        ( 
            mapVM_Command, 
            m_vecLineData, 
            vecTextureResource,
            vecCubeTextureResource,
            vecPixelScalar, 
            vecPixelVector,
            strStringFX, 
            emWhereCodeArray
        );
    }

    // emWhereCode 확인
    {
        emWhereCode = vm::EMWC_HLSL;
    }

    // Scalar가 종료된다.
    {
        NSVMCommand::PixelScalarName
        (
            mapVM_Command, 
            m_vecLineData,
            vecPixelScalar, 
            vecPixelVector,
            strStringFX, 
            emWhereCodeArray
        );
    }

    std::string strFX = " (lerp((";
    strFX += strStringFX[0] + "),(" + strStringFX[1] + "),(" + strStringFX[2] + "))) ";

    return strFX;
}

void DxVMCommand_LinearInterpolate::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_LinearInterpolate::Load( ic::SerialFileInterface* pSFile )
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



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Time
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_Time::DxVMCommand_Time(void)
{ 
    m_emType = VMC_TIME;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Time::~DxVMCommand_Time(void)
{
}

BOOL DxVMCommand_Time::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Time") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Time::Refresh()
{
}

std::string DxVMCommand_Time::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                            DWORD dwOutputIndex, 
                                            vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                            vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                            vm::EM_WHERE_CODE& emWhereCode )
{
    Refresh();

    // emWhereCode 확인
    {
        emWhereCode = vm::EMWC_SCALAR;
    }

    return "g_fTime";
}

void DxVMCommand_Time::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Time::Load( ic::SerialFileInterface* pSFile )
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