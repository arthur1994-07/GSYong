#include "stdafx.h"

#include <sstream>

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "./DxVMCommand_EX_Constants.h"
#include "./DxVMCommand_EX_Coordinates.h"
#include "./DxVMCommand_EX_Math.h"
#include "./DxVMCommand_EX_Parameters.h"
#include "./DxVMCommand_EX_Utility.h"

#include "DxVMCommand_EX.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_EX
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_EX::DxVMCommand_EX(void)
{
}

DxVMCommand_EX::~DxVMCommand_EX(void)
{
}

VMC_TYPE DxVMCommand_EX::GetType()
{
    return m_emType;
}

std::tr1::shared_ptr<DxVMCommand_EX> DxVMCommand_EX::NEW_COMMAND_STATIC( VMC_TYPE emType )
{
    // Note : Add Command
    std::tr1::shared_ptr<DxVMCommand_EX> spNew;
    switch ( emType )
    {
    case VMC_CONSTANT:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_Constant);
        }
        break;

    case VMC_CONSTANT2VECTOR:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_Constant2Vector);
        }
        break;

    case VMC_CONSTANT3VECTOR:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_Constant3Vector);
        }
        break;

    case VMC_CONSTANT4VECTOR:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_Constant4Vector);
        }
        break;

    case VMC_PANNER:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_Panner);
        }
        break;

    case VMC_ROTATOR:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_Rotator);
        }
        break;

    case VMC_ADD:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_Add);
        }
        break;

    case VMC_MULTIPLY:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_Multiply);
        }
        break;

    case VMC_SINE:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_Sine);
        }
        break;

    case VMC_SUBTRACT:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_Sub);
        }
        break;

    case VMC_VECTOR_PARAMETER:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_VectorParameter);
        }
        break;

    case VMC_TIME:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand_EX>(new DxVMCommand_EX_Time);
        }
        break;


    default:
        return spNew;
    };

    spNew->m_emType = emType;

    return spNew;
}

void DxVMCommand_EX::FrameMove( float fTime )
{
    for( DWORD i=0; i<m_vecChildCommandEX.size(); ++i )
    {
        m_vecChildCommandEX[i]->FrameMove( fTime );
    }
}

void DxVMCommand_EX::SetLink( std::tr1::shared_ptr<DxVMCommand_EX>& spVMCommand_EX )
{
    m_vecChildCommandEX.push_back( spVMCommand_EX );
}

void DxVMCommand_EX::Save( ic::SerialFileInterface* pSFile )
{
    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << static_cast<DWORD>( m_vecChildCommandEX.size() );
        for ( DWORD i=0; i<m_vecChildCommandEX.size(); ++i )
        {
            (*pSFile) << static_cast<DWORD>( m_vecChildCommandEX[i]->GetType() );
            m_vecChildCommandEX[i]->Save( pSFile );
        }
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_EX::Load( ic::SerialFileInterface* pSFile )
{
    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        DWORD dwSize(0);
        DWORD dwType(0);
        (*pSFile) >> dwSize;
        for ( DWORD i=0; i<dwSize; ++i )
        {
            (*pSFile) >> dwType;
            std::tr1::shared_ptr<DxVMCommand_EX> spNew = NEW_COMMAND_STATIC( static_cast<VMC_TYPE>( dwType ) );
            spNew->Load( pSFile );
            m_vecChildCommandEX.push_back( spNew );
        }
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }
}




