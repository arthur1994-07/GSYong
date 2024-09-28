#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "DxVMCommand_EX_Coordinates.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Panner
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Panner::DxVMCommand_EX_Panner(void)
    : m_vUV(0.f,0.f)
{
}

DxVMCommand_EX_Panner::~DxVMCommand_EX_Panner(void)
{
}

void DxVMCommand_EX_Panner::FrameMove( float fTime )
{
    m_vUV.x = m_sData.m_fSpeedX * fTime;
    m_vUV.y = m_sData.m_fSpeedY * fTime;
}

D3DXVECTOR4 DxVMCommand_EX_Panner::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    return D3DXVECTOR4( m_vUV.x, m_vUV.y, 1.f, 1.f );
}

void DxVMCommand_EX_Panner::Clone( DxVMCommand* pVMCommand )
{
    m_sData.Clone( pVMCommand->GetData() );
}

void DxVMCommand_EX_Panner::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Panner::Load( ic::SerialFileInterface* pSFile )
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
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_Rotator
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_Rotator::DxVMCommand_EX_Rotator(void)
    : m_vVectorX(1.f,0.f)
    , m_vVectorY(0.f,1.f)
{
}

DxVMCommand_EX_Rotator::~DxVMCommand_EX_Rotator(void)
{
}

void DxVMCommand_EX_Rotator::FrameMove( float fTime )
{
    D3DXMATRIX matRotateZ;
    D3DXMatrixRotationZ( &matRotateZ, D3DX_PI*fTime*m_sData.m_fSpeed/180.f );

    D3DXVECTOR2 vBaseVectorX(1.f,0.f);
    D3DXVECTOR2 vBaseVectorY(0.f,1.f);
    D3DXVec2TransformCoord( &m_vVectorX, &vBaseVectorX, &matRotateZ );
    D3DXVec2TransformCoord( &m_vVectorY, &vBaseVectorY, &matRotateZ );
}

D3DXVECTOR4 DxVMCommand_EX_Rotator::GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    return D3DXVECTOR4( m_vVectorX.x, m_vVectorX.y, m_vVectorY.x, m_vVectorY.y );
}

void DxVMCommand_EX_Rotator::Clone( DxVMCommand* pVMCommand )
{
    m_sData.Clone( pVMCommand->GetData() );
}

void DxVMCommand_EX_Rotator::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_Rotator::Load( ic::SerialFileInterface* pSFile )
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