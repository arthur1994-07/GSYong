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

        // Ŭ������ ����� �۾�.
        if ( !pVMCommand->CreateCommand_EX( mapVM_Command, spVMCommand_EX ) )
        {
            // CORE �� �ƴϴ�.
            if ( pVMCommand->GetType() != VMC_CORE )
            {
                // MsgBox�� ���� ���α׷��� ���������Ű��.
                std::string strErrorMsg;
                strErrorMsg = _T("[ERROR] ");
                strErrorMsg += NSVISUAL_MATERIAL::g_strVisualMaterialCommand[ pVMCommand->GetType() ];
                strErrorMsg += _T(" EX �����ٶ�.");
                AfxMessageBox( strErrorMsg.c_str() );
                exit(1);
            }
        }

        // ��Į�� �̸����� �۾�
        {
            char c1='0';
            c1 = static_cast<char>( c1 + vecPixelScalar.size() );

            strUniformPixelScalar = "g_fUniformPixelScalar_";
            strUniformPixelScalar += c1;

            // ���� ����.
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

        // Ŭ������ ����� �۾�.
        {
            pVMCommand->CreateCommand_EX( mapVM_Command, spVMCommand_EX );
        }

        // ��Į�� �̸����� �۾�
        {
            char c1='0';
            c1 = static_cast<char>( c1 + vecPixelVector.size() );

            strUniformPixelVector = "g_vUniformPixelVector_";
            strUniformPixelVector += c1;

            // ���� ����.
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
                // ��Į�� �̸����� �۾�
                strStringFX[nIndexIn] = GetPixelScalarName( (*iter).second.get(), 
                                                            mapVM_Command, 
                                                            vecPixelScalar );
            }
            else if ( emWhereCode[nIndexIn] == vm::EMWC_VECTOR )
            {
                // ��Į�� �̸����� �۾�
                strStringFX[nIndexIn] = GetUniformPixelVectorName( (*iter).second.get(), 
                                                                    mapVM_Command, 
                                                                    vecPixelVector );
            }
        }
    }
};