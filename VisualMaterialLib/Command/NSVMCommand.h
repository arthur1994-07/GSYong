#pragma once

#include "../Define/DxVMCommandDef.h"
#include "../Define/DxVMDefine.h"

namespace NSVMCommand
{
    void MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                        const VEC_VMLINEDATA& vecLineData,
                        vm::VEC_TEXTURE_RESOURCE& vecTextureResource,
                        vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                        vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                        vm::VEC_PIXEL_VECTOR& vecPixelVector,
                        std::string strStringFX[],
                        vm::EM_WHERE_CODE emWhereCode[] );

    std::string GetPixelScalarName( DxVMCommand* pVMCommand, 
                                    const MAP_VM_COMMAND& mapVM_Command,
                                    vm::VEC_PIXEL_VECTOR& vecPixelScalar );

    std::string GetUniformPixelVectorName( DxVMCommand* pVMCommand, 
                                            const MAP_VM_COMMAND& mapVM_Command,
                                            vm::VEC_PIXEL_VECTOR& vecPixelVector );

    void PixelScalarName( const MAP_VM_COMMAND& mapVM_Command, 
                            const VEC_VMLINEDATA& vecLineData,
                            vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                            vm::VEC_PIXEL_VECTOR& vecPixelVector,
                            std::string strStringFX[],
                            vm::EM_WHERE_CODE emWhereCode[] );
};