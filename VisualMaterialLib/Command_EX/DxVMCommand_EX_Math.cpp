#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "../Command/DxVMCommand_Math.h"
#include "DxVMCommand_EX_Math.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Add
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Add::DxVMCommand_EX_Add(void)
{ 
}

DxVMCommand_EX_Add::~DxVMCommand_EX_Add(void)
{
}

D3DXVECTOR4 DxVMCommand_EX_Add::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    if ( m_vecChildCommandEX.size() != 2 )
        return D3DXVECTOR4( 1.f, 1.f, 1.f, 1.f );

    return D3DXVECTOR4( m_vecChildCommandEX[0]->GetPixelVector( mapParamaterNameData ).x + m_vecChildCommandEX[1]->GetPixelVector( mapParamaterNameData ).x, 1.f, 1.f, 1.f );
}

void DxVMCommand_EX_Add::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Add::Load( ic::SerialFileInterface* pSFile )
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
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Multiply
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Multiply::DxVMCommand_EX_Multiply(void)
{ 
}

DxVMCommand_EX_Multiply::~DxVMCommand_EX_Multiply(void)
{
}

D3DXVECTOR4 DxVMCommand_EX_Multiply::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    if ( m_vecChildCommandEX.size() != 2 )
        return D3DXVECTOR4( 1.f, 1.f, 1.f, 1.f );

    return D3DXVECTOR4( m_vecChildCommandEX[0]->GetPixelVector( mapParamaterNameData ).x * m_vecChildCommandEX[1]->GetPixelVector( mapParamaterNameData ).x, 1.f, 1.f, 1.f );
}

void DxVMCommand_EX_Multiply::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Multiply::Load( ic::SerialFileInterface* pSFile )
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
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Sine
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Sine::DxVMCommand_EX_Sine(void)
{
}

DxVMCommand_EX_Sine::~DxVMCommand_EX_Sine(void)
{
}

D3DXVECTOR4 DxVMCommand_EX_Sine::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    if ( m_vecChildCommandEX.size() != 1 )
        return D3DXVECTOR4( 1.f, 1.f, 1.f, 1.f );

    float fPeriod(1.f);
    if ( m_sData.m_fPeriod > 0.f )
    {
        fPeriod = 2.f * D3DX_PI / m_sData.m_fPeriod;
    }

    return D3DXVECTOR4( sinf( m_vecChildCommandEX[0]->GetPixelVector( mapParamaterNameData ).x * fPeriod ), 1.f, 1.f, 1.f );
}

void DxVMCommand_EX_Sine::Clone( DxVMCommand* pVMCommand )
{
    m_sData.Clone( pVMCommand->GetData() );
}

void DxVMCommand_EX_Sine::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Sine::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Load( pSFile );

    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        m_sData.Load( pSFile );
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }
}


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Sub
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Sub::DxVMCommand_EX_Sub(void)
{ 
}

DxVMCommand_EX_Sub::~DxVMCommand_EX_Sub(void)
{
}

D3DXVECTOR4 DxVMCommand_EX_Sub::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    if ( m_vecChildCommandEX.size() != 2 )
        return D3DXVECTOR4( 1.f, 1.f, 1.f, 1.f );

    return D3DXVECTOR4( m_vecChildCommandEX[0]->GetPixelVector( mapParamaterNameData ).x - m_vecChildCommandEX[1]->GetPixelVector( mapParamaterNameData ).x, 1.f, 1.f, 1.f );
}

void DxVMCommand_EX_Sub::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Sub::Load( ic::SerialFileInterface* pSFile )
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