#include "stdafx.h"

#include "../Command_EX/DxVMCommand_EX.h"
#include "./DxVMCommand.h"

#include "NSVMCommand.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


namespace NSVMCommand
{
    // 
    void MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                        const VEC_VMLINEDATA& vecLineData,
                        vm::VEC_TEXTURE_RESOURCE& vecTextureResource,
                        vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                        vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                        vm::VEC_PIXEL_VECTOR& vecPixelVector,
                        std::string strStringFX[],
                        vm::EM_WHERE_CODE emWhereCode[] )
    {
        for ( DWORD i=0; i<vecLineData.size(); ++i )
        {
            MAP_VM_COMMAND_CITER iter = mapVM_Command.find( vecLineData[i]->m_dwID_Output );
            if ( iter != mapVM_Command.end() )
            {
                int nIndexIn = vecLineData[i]->m_nIndexBox_Input;
                strStringFX[nIndexIn] = (*iter).second->MakeStringFX( mapVM_Command, 
                                                                        vecLineData[i]->m_nIndexBox_Output, 
                                                                        vecTextureResource, 
                                                                        vecCubeTextureResource,
                                                                        vecPixelScalar, 
                                                                        vecPixelVector,
                                                                        emWhereCode[nIndexIn] );
            }
        }
    }

    //
    std::string GetPixelScalarName( DxVMCommand* pVMCommand, 
                                    const MAP_VM_COMMAND& mapVM_Command,
                                    vm::VEC_PIXEL_VECTOR& vecPixelScalar )
    {
        std::string strUniformPixelScalar;
        std::tr1::shared_ptr<DxVMCommand_EX> spVMCommand_EX;

        // 클래스를 만드는 작업.
        if ( !pVMCommand->CreateCommand_EX( mapVM_Command, spVMCommand_EX ) )
        {
            // CORE 는 아니다.
            if ( pVMCommand->GetType() != VMC_CORE )
            {
                // MsgBox를 띄우고 프로그램을 강제종료시키자.
                std::string strErrorMsg;
                strErrorMsg = _T("[ERROR] ");
                strErrorMsg += NSVISUAL_MATERIAL::g_strVisualMaterialCommand[ pVMCommand->GetType() ];
                strErrorMsg += _T(" EX 생성바람.");
                AfxMessageBox( strErrorMsg.c_str() );
                exit(1);
            }
        }

        // 스칼라 이름으로 작업
        {
            char c1='0';
            c1 = static_cast<char>( c1 + vecPixelScalar.size() );

            strUniformPixelScalar = "g_fUniformPixelScalar_";
            strUniformPixelScalar += c1;

            // 정보 셋팅.
            vecPixelScalar.push_back( spVMCommand_EX );
        }

        return strUniformPixelScalar;
    }

    //
    std::string GetUniformPixelVectorName( DxVMCommand* pVMCommand, 
                                            const MAP_VM_COMMAND& mapVM_Command,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector )
    {
        std::string strUniformPixelVector;
        std::tr1::shared_ptr<DxVMCommand_EX> spVMCommand_EX;

        // 클래스를 만드는 작업.
        {
            pVMCommand->CreateCommand_EX( mapVM_Command, spVMCommand_EX );
        }

        // 스칼라 이름으로 작업
        {
            char c1='0';
            c1 = static_cast<char>( c1 + vecPixelVector.size() );

            strUniformPixelVector = "g_vUniformPixelVector_";
            strUniformPixelVector += c1;

            // 정보 셋팅.
            vecPixelVector.push_back( spVMCommand_EX );
        }

        return strUniformPixelVector;
    }

    //
    void PixelScalarName( const MAP_VM_COMMAND& mapVM_Command, 
                            const VEC_VMLINEDATA& vecLineData,
                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                            std::string strStringFX[],
                            vm::EM_WHERE_CODE emWhereCode[] )
    {
        for ( DWORD i=0; i<vecLineData.size(); ++i )
        {
            int nIndexIn = vecLineData[i]->m_nIndexBox_Input;
            MAP_VM_COMMAND_CITER iter = mapVM_Command.find( vecLineData[i]->m_dwID_Output );
            if ( emWhereCode[nIndexIn] == vm::EMWC_SCALAR )
            {
                // 스칼라 이름으로 작업
                strStringFX[nIndexIn] = GetPixelScalarName( (*iter).second.get(), 
                                                            mapVM_Command, 
                                                            vecPixelScalar );
            }
            else if ( emWhereCode[nIndexIn] == vm::EMWC_VECTOR )
            {
                // 스칼라 이름으로 작업
                strStringFX[nIndexIn] = GetUniformPixelVectorName( (*iter).second.get(), 
                                                                    mapVM_Command, 
                                                                    vecPixelVector );
            }
        }
    }
};