#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "../Command_EX/DxVMCommand_EX.h"
#include "../Edit/DxEditVM.h"

#include "DxVMCommand_Constants.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Constant
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Constant_Data
VMCommand_Constant_Data::VMCommand_Constant_Data()
    : m_fR(1.f)
{
}

VMCommand_Constant_Data::~VMCommand_Constant_Data()
{
}

void VMCommand_Constant_Data::Clone( VMCommand_Data* _pData )
{
    VMCommand_Constant_Data* pData = dynamic_cast<VMCommand_Constant_Data*>( _pData );
    *this = *pData;
}

void VMCommand_Constant_Data::Save( ic::SerialFileInterface* pSFile )
{
    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << m_fR;
    }
    pSFile->EndBlock();
}

void VMCommand_Constant_Data::Load( ic::SerialFileInterface* pSFile )
{
    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        (*pSFile) >> m_fR;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }
}

//--------------------------------------------------------------- DxVMCommand_Constant
DxVMCommand_Constant::DxVMCommand_Constant(void)
{ 
    m_emType = VMC_CONSTANT;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Constant::~DxVMCommand_Constant(void)
{
}

BOOL DxVMCommand_Constant::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Constant") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Constant::Refresh()
{
    // Constant에 정보를 Update 한다.
    TSTRINGSTREAM strStream;
	strStream << m_sData.m_fR;

    m_spEditVM_Title->ChangeChannel( strStream.str().c_str() );
}

std::string DxVMCommand_Constant::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                                DWORD dwOutputIndex,
                                                vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                                vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                                vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                                vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                                vm::EM_WHERE_CODE& emWhereCode )
{
    Refresh();

    std::stringstream strStream;
	strStream << m_sData.m_fR;

    // emWhereCode 확인
    {
        emWhereCode = vm::EMWC_NEUTRAL;
    }

    return strStream.str();
}

void DxVMCommand_Constant::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Constant::Load( ic::SerialFileInterface* pSFile )
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
        m_sData.Load( pSFile );
        (*pSFile) >> m_strDesc;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }

    Refresh();
}


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Constant2Vector
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Constant2Vector_Data
VMCommand_Constant2Vector_Data::VMCommand_Constant2Vector_Data()
    : m_fR(1.f)
    , m_fG(1.f)
{
}

VMCommand_Constant2Vector_Data::~VMCommand_Constant2Vector_Data()
{
}

void VMCommand_Constant2Vector_Data::Clone( VMCommand_Data* _pData )
{
    VMCommand_Constant2Vector_Data* pData = dynamic_cast<VMCommand_Constant2Vector_Data*>( _pData );
    *this = *pData;
}

void VMCommand_Constant2Vector_Data::Save( ic::SerialFileInterface* pSFile )
{
    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << m_fR;
        (*pSFile) << m_fG;
    }
    pSFile->EndBlock();
}

void VMCommand_Constant2Vector_Data::Load( ic::SerialFileInterface* pSFile )
{
    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        (*pSFile) >> m_fR;
        (*pSFile) >> m_fG;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }
}

//--------------------------------------------------------------- DxVMCommand_Constant2Vector
DxVMCommand_Constant2Vector::DxVMCommand_Constant2Vector(void)
{ 
    m_emType = VMC_CONSTANT2VECTOR;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Constant2Vector::~DxVMCommand_Constant2Vector(void)
{
}

BOOL DxVMCommand_Constant2Vector::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Constant2Vector") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Constant2Vector::Refresh()
{
    // Constant에 정보를 Update 한다.
    TSTRINGSTREAM strStream;
	strStream << m_sData.m_fR;
    strStream << _T(" , ");
    strStream << m_sData.m_fG;

    m_spEditVM_Title->ChangeChannel( strStream.str().c_str() );
}

std::string DxVMCommand_Constant2Vector::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
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
    strStream << ",0.f,0.f))";

    // emWhereCode 확인
    {
        emWhereCode = vm::EMWC_NEUTRAL;
    }

    return strStream.str();
}

void DxVMCommand_Constant2Vector::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Constant2Vector::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Load( pSFile );

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

    Refresh();
}



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Constant3Vector
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Constant3Vector_Data
VMCommand_Constant3Vector_Data::VMCommand_Constant3Vector_Data()
    : m_fR(1.f)
    , m_fG(1.f)
    , m_fB(1.f)
{
}

VMCommand_Constant3Vector_Data::~VMCommand_Constant3Vector_Data()
{
}

void VMCommand_Constant3Vector_Data::Clone( VMCommand_Data* _pData )
{
    VMCommand_Constant3Vector_Data* pData = dynamic_cast<VMCommand_Constant3Vector_Data*>( _pData );
    *this = *pData;
}

void VMCommand_Constant3Vector_Data::Save( ic::SerialFileInterface* pSFile )
{
    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << m_fR;
        (*pSFile) << m_fG;
        (*pSFile) << m_fB;
    }
    pSFile->EndBlock();
}

void VMCommand_Constant3Vector_Data::Load( ic::SerialFileInterface* pSFile )
{
    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        (*pSFile) >> m_fR;
        (*pSFile) >> m_fG;
        (*pSFile) >> m_fB;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }
}

//--------------------------------------------------------------- DxVMCommand_Constant3Vector
DxVMCommand_Constant3Vector::DxVMCommand_Constant3Vector(void)
{ 
    m_emType = VMC_CONSTANT3VECTOR;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Constant3Vector::~DxVMCommand_Constant3Vector(void)
{
}

BOOL DxVMCommand_Constant3Vector::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Constant3Vector") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Constant3Vector::Refresh()
{
    // Constant에 정보를 Update 한다.
    TSTRINGSTREAM strStream;
	strStream << m_sData.m_fR;
    strStream << _T(" , ");
    strStream << m_sData.m_fG;
    strStream << _T(" , ");
    strStream << m_sData.m_fB;

    m_spEditVM_Title->ChangeChannel( strStream.str().c_str() );
}

std::string DxVMCommand_Constant3Vector::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
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
    {
        emWhereCode = vm::EMWC_NEUTRAL;
    }

    return strStream.str();
}

void DxVMCommand_Constant3Vector::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Constant3Vector::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Load( pSFile );

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

    Refresh();
}



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Constant4Vector
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Constant4Vector_Data
VMCommand_Constant4Vector_Data::VMCommand_Constant4Vector_Data()
    : m_fR(1.f)
    , m_fG(1.f)
    , m_fB(1.f)
    , m_fA(1.f)
{
}

VMCommand_Constant4Vector_Data::~VMCommand_Constant4Vector_Data()
{
}

void VMCommand_Constant4Vector_Data::Clone( VMCommand_Data* _pData )
{
    VMCommand_Constant4Vector_Data* pData = dynamic_cast<VMCommand_Constant4Vector_Data*>( _pData );
    *this = *pData;
}

void VMCommand_Constant4Vector_Data::Save( ic::SerialFileInterface* pSFile )
{
    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << m_fR;
        (*pSFile) << m_fG;
        (*pSFile) << m_fB;
        (*pSFile) << m_fA;
    }
    pSFile->EndBlock();
}

void VMCommand_Constant4Vector_Data::Load( ic::SerialFileInterface* pSFile )
{
    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        (*pSFile) >> m_fR;
        (*pSFile) >> m_fG;
        (*pSFile) >> m_fB;
        (*pSFile) >> m_fA;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }
}

//--------------------------------------------------------------- DxVMCommand_Constant4Vector
DxVMCommand_Constant4Vector::DxVMCommand_Constant4Vector(void)
{ 
    m_emType = VMC_CONSTANT4VECTOR;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Constant4Vector::~DxVMCommand_Constant4Vector(void)
{
}

BOOL DxVMCommand_Constant4Vector::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Constant4Vector") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Constant4Vector::Refresh()
{
    // Constant에 정보를 Update 한다.
    TSTRINGSTREAM strStream;
	strStream << m_sData.m_fR;
    strStream << _T(" , ");
    strStream << m_sData.m_fG;
    strStream << _T(" , ");
    strStream << m_sData.m_fB;
    strStream << _T(" , ");
    strStream << m_sData.m_fA;

    m_spEditVM_Title->ChangeChannel( strStream.str().c_str() );
}

std::string DxVMCommand_Constant4Vector::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
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
    strStream << ",";
    strStream << m_sData.m_fA;
    strStream << "))";

    // emWhereCode 확인
    {
        emWhereCode = vm::EMWC_NEUTRAL;
    }

    return strStream.str();
}

void DxVMCommand_Constant4Vector::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Constant4Vector::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Load( pSFile );

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

    Refresh();
}