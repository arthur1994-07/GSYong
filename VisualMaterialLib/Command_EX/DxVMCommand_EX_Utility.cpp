#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "DxVMCommand_EX_Utility.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Time
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Time::DxVMCommand_EX_Time(void)
    : m_fTime(0.f)
{
}

DxVMCommand_EX_Time::~DxVMCommand_EX_Time(void)
{
}

void DxVMCommand_EX_Time::FrameMove( float fTime )
{
    m_fTime = fTime;
}

D3DXVECTOR4 DxVMCommand_EX_Time::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    return D3DXVECTOR4( m_fTime, 1.f, 1.f, 1.f );
}

void DxVMCommand_EX_Time::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Time::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Load( pSFile );

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
}


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Clamp
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Clamp::DxVMCommand_EX_Clamp(void)
{
}

DxVMCommand_EX_Clamp::~DxVMCommand_EX_Clamp(void)
{
}

D3DXVECTOR4 DxVMCommand_EX_Clamp::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    return D3DXVECTOR4( 1.f, 1.f, 1.f, 1.f );
}

void DxVMCommand_EX_Clamp::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Clamp::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Load( pSFile );

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
}