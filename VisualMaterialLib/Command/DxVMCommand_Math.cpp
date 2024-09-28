#include "stdafx.h"

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "../Command_EX/DxVMCommand_EX.h"
#include "../Command/NSVMCommand.h"
#include "../Edit/DxEditVM.h"

#include "DxVMCommand_Math.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Add
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_Add::DxVMCommand_Add(void)
{ 
    m_emType = VMC_ADD;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Add::~DxVMCommand_Add(void)
{
}

BOOL DxVMCommand_Add::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Add") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddChannel(_T("A"));
    m_spEditVM_Command->AddChannel(_T("B"));
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Add::Refresh()
{
}

std::string DxVMCommand_Add::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                            DWORD dwOutputIndex,
                                            vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                            vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                            vm::EM_WHERE_CODE& emWhereCode )
{
    if ( m_vecLineData.size() != 2 )
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
        // 어느 하나라도 HLSL 기입이면 무조건 HLSL 기입이다.
        if ( emWhereCodeArray[0] == vm::EMWC_HLSL || emWhereCodeArray[1] == vm::EMWC_HLSL )
        {
            emWhereCode = vm::EMWC_HLSL;
        }
        // HLSL 기입이 아니고, 어느 하나라도 VECTOR 기입이면 무조건 VECTOR 기입이다.
        else if ( emWhereCodeArray[0] == vm::EMWC_VECTOR || emWhereCodeArray[1] == vm::EMWC_VECTOR )
        {
            emWhereCode = vm::EMWC_VECTOR;
        }
        // HLSL, VECTOR 기입이 아니고, 어느 하나라도 SCALAR 기입이면 무조건 SCALAR 기입이다.
        else if ( emWhereCodeArray[0] == vm::EMWC_SCALAR || emWhereCodeArray[1] == vm::EMWC_SCALAR )
        {
            emWhereCode = vm::EMWC_SCALAR;
        }
        else
        {
            emWhereCode = vm::EMWC_NEUTRAL;
        }
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

    std::string strFX("");
    strFX = strFX + "(" + strStringFX[0] + " + " + strStringFX[1] + ")";
    return strFX;
}

void DxVMCommand_Add::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Add::Load( ic::SerialFileInterface* pSFile )
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
//----------------------------------------------------------------------------------------------- DxVMCommand_Multiply
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_Multiply::DxVMCommand_Multiply(void)
{ 
    m_emType = VMC_MULTIPLY;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Multiply::~DxVMCommand_Multiply(void)
{
}

BOOL DxVMCommand_Multiply::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Multiply") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddChannel(_T("A"));
    m_spEditVM_Command->AddChannel(_T("B"));
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Multiply::Refresh()
{
}

std::string DxVMCommand_Multiply::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                                DWORD dwOutputIndex,
                                                vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                                vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                                vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                                vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                                vm::EM_WHERE_CODE& emWhereCode )
{
    if ( m_vecLineData.size() != 2 )
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
        // 어느 하나라도 HLSL 기입이면 무조건 HLSL 기입이다.
        if ( emWhereCodeArray[0] == vm::EMWC_HLSL || emWhereCodeArray[1] == vm::EMWC_HLSL )
        {
            emWhereCode = vm::EMWC_HLSL;
        }
        // HLSL 기입이 아니고, 어느 하나라도 VECTOR 기입이면 무조건 VECTOR 기입이다.
        else if ( emWhereCodeArray[0] == vm::EMWC_VECTOR || emWhereCodeArray[1] == vm::EMWC_VECTOR )
        {
            emWhereCode = vm::EMWC_VECTOR;
        }
        // HLSL, VECTOR 기입이 아니고, 어느 하나라도 SCALAR 기입이면 무조건 SCALAR 기입이다.
        else if ( emWhereCodeArray[0] == vm::EMWC_SCALAR || emWhereCodeArray[1] == vm::EMWC_SCALAR )
        {
            emWhereCode = vm::EMWC_SCALAR;
        }
        else
        {
            emWhereCode = vm::EMWC_NEUTRAL;
        }
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

    std::string strFX("");
    strFX = strFX + "(" + strStringFX[0] + " * " + strStringFX[1] + ")";
    return strFX;
}

void DxVMCommand_Multiply::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Multiply::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Load( pSFile );

    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
    }
    else if ( dwVer == 0x100 )
    {
        (*pSFile) >> m_strDesc;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }

    Refresh();
}


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Sine
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Sine_Data
VMCommand_Sine_Data::VMCommand_Sine_Data()
    : m_fPeriod(1.f)
{
}

VMCommand_Sine_Data::~VMCommand_Sine_Data()
{
}

void VMCommand_Sine_Data::Clone( VMCommand_Data* _pData )
{
    VMCommand_Sine_Data* pData = dynamic_cast<VMCommand_Sine_Data*>( _pData );
    *this = *pData;
}

void VMCommand_Sine_Data::Save( ic::SerialFileInterface* pSFile )
{
    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << m_fPeriod;
    }
    pSFile->EndBlock();
}

void VMCommand_Sine_Data::Load( ic::SerialFileInterface* pSFile )
{
    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        (*pSFile) >> m_fPeriod;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }
}

//--------------------------------------------------------------- DxVMCommand_Sine
DxVMCommand_Sine::DxVMCommand_Sine(void)
{ 
    m_emType = VMC_SINE;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Sine::~DxVMCommand_Sine(void)
{
}

BOOL DxVMCommand_Sine::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Sine") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddChannel(_T("A"));
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Sine::Refresh()
{
}

std::string DxVMCommand_Sine::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
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

    std::string strStringFX_A("");
    MAP_VM_COMMAND_CITER iter = mapVM_Command.find( m_vecLineData[0]->m_dwID_Output );
    if ( iter != mapVM_Command.end() )
    {
        strStringFX_A = (*iter).second->MakeStringFX( mapVM_Command, 
                                                        m_vecLineData[0]->m_nIndexBox_Output, 
                                                        vecTextureResource, 
                                                        vecCubeTextureResource,
                                                        vecPixelScalar, 
                                                        vecPixelVector,
                                                        emWhereCode );
    }

    float fPeriod(1.f);
    if ( m_sData.m_fPeriod > 0.f )
    {
        fPeriod = 2.f * D3DX_PI / m_sData.m_fPeriod;
    }

    std::stringstream strStream;
    strStream << "sin(";
    strStream << fPeriod;
    strStream << "*";
    strStream << strStringFX_A.c_str();
    strStream << ")";

    return strStream.str();
}

void DxVMCommand_Sine::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        m_sData.Save( pSFile );
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Sine::Load( ic::SerialFileInterface* pSFile )
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
//----------------------------------------------------------------------------------------------- DxVMCommand_Subtract
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_Subtract
DxVMCommand_Subtract::DxVMCommand_Subtract(void)
{ 
    m_emType = VMC_SUBTRACT;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_Subtract::~DxVMCommand_Subtract(void)
{
}

BOOL DxVMCommand_Subtract::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("Subtract") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddChannel(_T("A"));
    m_spEditVM_Command->AddChannel(_T("B"));
    m_spEditVM_Command->AddImage();

    return TRUE;
}

void DxVMCommand_Subtract::Refresh()
{
}

std::string DxVMCommand_Subtract::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                            DWORD dwOutputIndex, 
                                            vm::VEC_TEXTURE_RESOURCE& vecTextureResource,
                                            vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                            vm::EM_WHERE_CODE& emWhereCode )
{
    if ( m_vecLineData.size() != 2 )
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
        // 어느 하나라도 HLSL 기입이면 무조건 HLSL 기입이다.
        if ( emWhereCodeArray[0] == vm::EMWC_HLSL || emWhereCodeArray[1] == vm::EMWC_HLSL )
        {
            emWhereCode = vm::EMWC_HLSL;
        }
        // HLSL 기입이 아니고, 어느 하나라도 VECTOR 기입이면 무조건 VECTOR 기입이다.
        else if ( emWhereCodeArray[0] == vm::EMWC_VECTOR || emWhereCodeArray[1] == vm::EMWC_VECTOR )
        {
            emWhereCode = vm::EMWC_VECTOR;
        }
        // HLSL, VECTOR 기입이 아니고, 어느 하나라도 SCALAR 기입이면 무조건 SCALAR 기입이다.
        else if ( emWhereCodeArray[0] == vm::EMWC_SCALAR || emWhereCodeArray[1] == vm::EMWC_SCALAR )
        {
            emWhereCode = vm::EMWC_SCALAR;
        }
        else
        {
            emWhereCode = vm::EMWC_NEUTRAL;
        }
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

    std::string strFX("");
    strFX = strFX + "(" + strStringFX[0] + " - " + strStringFX[1] + ")";
    return strFX;
}

void DxVMCommand_Subtract::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_Subtract::Load( ic::SerialFileInterface* pSFile )
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