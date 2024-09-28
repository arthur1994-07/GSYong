#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"
#include "../Command_EX/DxVMCommand_EX.h"
#include "../Edit/DxEditVM.h"

#include "DxVMCommand_Parameters.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_VectorParameter
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_VectorParameter_Data
VMCommand_VectorParameter_Data::VMCommand_VectorParameter_Data()
    : m_fR(0.f)
    , m_fG(0.f)
    , m_fB(0.f)
    , m_strParameter(_T("None"))
{
}

VMCommand_VectorParameter_Data::~VMCommand_VectorParameter_Data()
{
}

void VMCommand_VectorParameter_Data::Clone( VMCommand_Data* _pData )
{
    VMCommand_VectorParameter_Data* pData = dynamic_cast<VMCommand_VectorParameter_Data*>( _pData );
    *this = *pData;
}

void VMCommand_VectorParameter_Data::Save( ic::SerialFileInterface* pSFile )
{
    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << m_fR;
        (*pSFile) << m_fG;
        (*pSFile) << m_fB;
        (*pSFile) << m_strParameter;
    }
    pSFile->EndBlock();
}

void VMCommand_VectorParameter_Data::Load( ic::SerialFileInterface* pSFile )
{
    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        (*pSFile) >> m_fR;
        (*pSFile) >> m_fG;
        (*pSFile) >> m_fB;
        (*pSFile) >> m_strParameter;
    }
    else if ( dwVer == 0x100 )
    {
        (*pSFile) >> m_fR;
        (*pSFile) >> m_fG;
        (*pSFile) >> m_fB;
        (*pSFile) >> m_strParameter;

        m_fG = m_fB = m_fR;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }
}

//--------------------------------------------------------------- VMCommand_VectorParameter
DxVMCommand_VectorParameter::DxVMCommand_VectorParameter(void)
{ 
    m_emType = VMC_VECTOR_PARAMETER;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_VectorParameter::~DxVMCommand_VectorParameter(void)
{
}

BOOL DxVMCommand_VectorParameter::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Param") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddOutputColor();
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_VectorParameter::Refresh()
{
    // Constant에 정보를 Update 한다.
    TSTRINGSTREAM strStream;
	strStream << _T("Param '");
    strStream << m_sData.m_strParameter;
    strStream << _T("' (") ;
    strStream << m_sData.m_fR;
    strStream << _T(" , ");
    strStream << m_sData.m_fG;
    strStream << _T(" , ");
    strStream << m_sData.m_fB;
    strStream << _T(")") ;

    m_spEditVM_Title->ChangeChannel( strStream.str().c_str() );
}

std::string DxVMCommand_VectorParameter::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                            DWORD dwOutputIndex, 
                                            vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                            vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                            vm::EM_WHERE_CODE& emWhereCode )
{
    Refresh();

    std::stringstream strStream;
    strStream << "(float4(";
	strStream << m_sData.m_fR;
    strStream << ",";
    strStream << m_sData.m_fG;
    strStream << ",";
    strStream << m_sData.m_fB;
    strStream << ",0.f))";

    // emWhereCode 확인
    emWhereCode = vm::EMWC_VECTOR;

    return strStream.str();
}

void DxVMCommand_VectorParameter::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_VectorParameter::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Load( pSFile );

    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        m_sData.Load( pSFile );
    }
    else if ( dwVer == 0x100 )
    {
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }

    Refresh();
}