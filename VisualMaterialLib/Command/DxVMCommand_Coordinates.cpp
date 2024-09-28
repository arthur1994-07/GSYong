#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"
#include "../Command_EX/DxVMCommand_EX.h"
#include "../Command/NSVMCommand.h"
#include "../Edit/DxEditVM.h"

#include "DxVMCommand_Coordinates.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Panner
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Panner_Data
VMCommand_Panner_Data::VMCommand_Panner_Data()
    : m_fSpeedX(0.f)
    , m_fSpeedY(0.f)
{
}

VMCommand_Panner_Data::~VMCommand_Panner_Data()
{
}

void VMCommand_Panner_Data::Clone( VMCommand_Data* _pData )
{
    VMCommand_Panner_Data* pData = dynamic_cast<VMCommand_Panner_Data*>( _pData );
    *this = *pData;
}

void VMCommand_Panner_Data::Save( ic::SerialFileInterface* pSFile )
{
    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << m_fSpeedX;
        (*pSFile) << m_fSpeedY;
    }
    pSFile->EndBlock();
}

void VMCommand_Panner_Data::Load( ic::SerialFileInterface* pSFile )
{
    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        (*pSFile) >> m_fSpeedX;
        (*pSFile) >> m_fSpeedY;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }
}

//--------------------------------------------------------------- DxVMCommand_Panner
DxVMCommand_Panner::DxVMCommand_Panner(void)
{ 
    m_emType = VMC_PANNER;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Panner::~DxVMCommand_Panner(void)
{
}

BOOL DxVMCommand_Panner::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Panner") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddChannel(_T("Coordinate"));
    m_spEditVM_Command->AddChannel(_T("Unknown"));
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Panner::Refresh()
{
}

std::string DxVMCommand_Panner::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                            DWORD dwOutputIndex, 
                                            vm::VEC_TEXTURE_RESOURCE& vecTextureResource,
                                            vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                            vm::EM_WHERE_CODE& emWhereCode )
{
    Refresh();

    if ( m_vecLineData.size() != 1 )
    {
        return "";
    }

    std::string strStringFX[2];
    vm::EM_WHERE_CODE emWhereCodeArray[2] = { vm::EMWC_NEUTRAL, vm::EMWC_NEUTRAL };
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

    // Vector를 얻어온다.
    std::string strUniformPixelVector = NSVMCommand::GetUniformPixelVectorName
    (
        this,
        mapVM_Command,
        vecPixelVector
    );

    std::stringstream strStream;
    strStream << "(";
    strStream << strUniformPixelVector;
    strStream << ".xy + (";
    strStream << strStringFX[0];
    strStream << "))";

    return strStream.str();
}

void DxVMCommand_Panner::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Panner::Load( ic::SerialFileInterface* pSFile )
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
//----------------------------------------------------------------------------------------------- DxVMCommand_Rotator
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Rotator_Data
VMCommand_Rotator_Data::VMCommand_Rotator_Data()
    : m_fCenterX(0.5f)
    , m_fCenterY(0.5f)
    , m_fSpeed(0.25f)
{
}

VMCommand_Rotator_Data::~VMCommand_Rotator_Data()
{
}

void VMCommand_Rotator_Data::Clone( VMCommand_Data* _pData )
{
    VMCommand_Rotator_Data* pData = dynamic_cast<VMCommand_Rotator_Data*>( _pData );
    *this = *pData;
}

void VMCommand_Rotator_Data::Save( ic::SerialFileInterface* pSFile )
{
    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << m_fCenterX;
        (*pSFile) << m_fCenterY;
        (*pSFile) << m_fSpeed;
    }
    pSFile->EndBlock();
}

void VMCommand_Rotator_Data::Load( ic::SerialFileInterface* pSFile )
{
    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        (*pSFile) >> m_fCenterX;
        (*pSFile) >> m_fCenterY;
        (*pSFile) >> m_fSpeed;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }
}

//--------------------------------------------------------------- DxVMCommand_Rotator
DxVMCommand_Rotator::DxVMCommand_Rotator(void)
{ 
    m_emType = VMC_ROTATOR;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Rotator::~DxVMCommand_Rotator(void)
{
}

BOOL DxVMCommand_Rotator::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Rotator") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddChannel(_T("Coordinate"));
    m_spEditVM_Command->AddChannel(_T("Unknown"));
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Rotator::Refresh()
{
}

std::string DxVMCommand_Rotator::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                            DWORD dwOutputIndex, 
                                            vm::VEC_TEXTURE_RESOURCE& vecTextureResource,
                                            vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                            vm::EM_WHERE_CODE& emWhereCode )
{
    Refresh();

    if ( m_vecLineData.size() != 1 )
    {
        return "";
    }

    std::string strStringFX[2];
    vm::EM_WHERE_CODE emWhereCodeArray[2] = { vm::EMWC_NEUTRAL, vm::EMWC_NEUTRAL };
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

    // Vector를 얻어온다.
    std::string strUniformPixelVector = NSVMCommand::GetUniformPixelVectorName
    (
        this,
        mapVM_Command,
        vecPixelVector
    );

    std::stringstream strStream;
    strStream << "(float2(dot(";
    strStream << strUniformPixelVector;
    strStream << ".xy,(";
    strStream << strStringFX[0];
    strStream << " - float2(";
    strStream << m_sData.m_fCenterX;
    strStream << ",";
    strStream << m_sData.m_fCenterY;
    strStream << "))),dot(";
    strStream << strUniformPixelVector;
    strStream << ".zw,(";
    strStream << strStringFX[0];
    strStream << " - float2(";
    strStream << m_sData.m_fCenterX;
    strStream << ",";
    strStream << m_sData.m_fCenterY;
    strStream << ")))) + float2(";
    strStream << m_sData.m_fCenterX;
    strStream << ",";
    strStream << m_sData.m_fCenterY;
    strStream << "))";

    return strStream.str();
}

void DxVMCommand_Rotator::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Rotator::Load( ic::SerialFileInterface* pSFile )
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
//----------------------------------------------------------------------------------------------- DxVMCommand_TextureCoordinate
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_TextureCoordinate::DxVMCommand_TextureCoordinate(void)
    : m_fUTiling(1.f)
    , m_fVTiling(1.f)
{ 
    m_emType = VMC_TEXTURE_COORDINATE;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_TextureCoordinate::~DxVMCommand_TextureCoordinate(void)
{
}

BOOL DxVMCommand_TextureCoordinate::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("TexCoord") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_TextureCoordinate::Refresh()
{
}

std::string DxVMCommand_TextureCoordinate::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
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
        emWhereCode = vm::EMWC_HLSL;
    }

    std::stringstream strStream;
    strStream << "(In.TexCoord.xy)*float2(";
    strStream << m_fUTiling;
    strStream << ",";
    strStream << m_fVTiling;
    strStream << ")";

    return strStream.str();
}

void DxVMCommand_TextureCoordinate::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << m_fUTiling;
        (*pSFile) << m_fVTiling;
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_TextureCoordinate::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Load( pSFile );

    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        (*pSFile) >> m_fUTiling;
        (*pSFile) >> m_fVTiling;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }

    Refresh();
}