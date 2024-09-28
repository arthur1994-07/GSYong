#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "../Command/DxVMCommand_Constants.h"
#include "DxVMCommand_EX_Constants.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Constant
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Constant::DxVMCommand_EX_Constant(void)
{ 
    m_emType = VMC_CONSTANT;
}

DxVMCommand_EX_Constant::~DxVMCommand_EX_Constant(void)
{
}

D3DXVECTOR4 DxVMCommand_EX_Constant::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    return D3DXVECTOR4( m_sData.m_fR, 0.f, 0.f, 0.f );
}

void DxVMCommand_EX_Constant::Clone( DxVMCommand* pVMCommand )
{
    m_sData.Clone( pVMCommand->GetData() );
}

void DxVMCommand_EX_Constant::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Constant::Load( ic::SerialFileInterface* pSFile )
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
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Constant2Vector
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Constant2Vector::DxVMCommand_EX_Constant2Vector(void)
{ 
    m_emType = VMC_CONSTANT2VECTOR;
}

DxVMCommand_EX_Constant2Vector::~DxVMCommand_EX_Constant2Vector(void)
{
}

D3DXVECTOR4 DxVMCommand_EX_Constant2Vector::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    return D3DXVECTOR4( m_sData.m_fR, m_sData.m_fG, 0.f, 0.f );
}

void DxVMCommand_EX_Constant2Vector::Clone( DxVMCommand* pVMCommand )
{
    m_sData.Clone( pVMCommand->GetData() );
}

void DxVMCommand_EX_Constant2Vector::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Constant2Vector::Load( ic::SerialFileInterface* pSFile )
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
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Constant3Vector
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Constant3Vector::DxVMCommand_EX_Constant3Vector(void)
{ 
    m_emType = VMC_CONSTANT3VECTOR;
}

DxVMCommand_EX_Constant3Vector::~DxVMCommand_EX_Constant3Vector(void)
{
}

D3DXVECTOR4 DxVMCommand_EX_Constant3Vector::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    return D3DXVECTOR4( m_sData.m_fR, m_sData.m_fG, m_sData.m_fB, 0.f );
}

void DxVMCommand_EX_Constant3Vector::Clone( DxVMCommand* pVMCommand )
{
    m_sData.Clone( pVMCommand->GetData() );
}

void DxVMCommand_EX_Constant3Vector::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Constant3Vector::Load( ic::SerialFileInterface* pSFile )
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
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Constant4Vector
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Constant4Vector::DxVMCommand_EX_Constant4Vector(void)
{ 
    m_emType = VMC_CONSTANT4VECTOR;
}

DxVMCommand_EX_Constant4Vector::~DxVMCommand_EX_Constant4Vector(void)
{
}

D3DXVECTOR4 DxVMCommand_EX_Constant4Vector::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    return D3DXVECTOR4( m_sData.m_fR, m_sData.m_fG, m_sData.m_fB, m_sData.m_fA );
}

void DxVMCommand_EX_Constant4Vector::Clone( DxVMCommand* pVMCommand )
{
    m_sData.Clone( pVMCommand->GetData() );
}

void DxVMCommand_EX_Constant4Vector::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Constant4Vector::Load( ic::SerialFileInterface* pSFile )
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