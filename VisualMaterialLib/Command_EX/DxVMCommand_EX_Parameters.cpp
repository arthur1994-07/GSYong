#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "../Command/DxVMCommand_Parameters.h"
#include "DxVMCommand_EX_Parameters.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_EX_VectorParameter
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX_VectorParameter::DxVMCommand_EX_VectorParameter(void)
{ 
}

DxVMCommand_EX_VectorParameter::~DxVMCommand_EX_VectorParameter(void)
{
}

D3DXVECTOR4 DxVMCommand_EX_VectorParameter::GetPixelVector(  const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData )
{
    vm::MAP_PARAMETER_NAME_DATA_CITER iter = mapParamaterNameData.find( m_sData.m_strParameter.c_str() );
    if ( iter!=mapParamaterNameData.end() )
    {
        return D3DXVECTOR4( (*iter).second.x, (*iter).second.y, (*iter).second.z, 1.f );        
    }
    else
    {
        return D3DXVECTOR4( m_sData.m_fR, m_sData.m_fG, m_sData.m_fB, 1.f );
    }
}

void DxVMCommand_EX_VectorParameter::Clone( DxVMCommand* pVMCommand )
{
    m_sData.Clone( pVMCommand->GetData() );
}

void DxVMCommand_EX_VectorParameter::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand_EX::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX_VectorParameter::Load( ic::SerialFileInterface* pSFile )
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