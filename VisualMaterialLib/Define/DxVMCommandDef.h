#pragma once

#include <boost/tr1/memory.hpp>
#include <vector>
#include <map>
#include "../../InternalCommonLib/Interface/TextureManagerInterface.h"

class DxVMCommand;
class DxVMCommand_EX;

typedef std::map<DWORD,std::tr1::shared_ptr<DxVMCommand>>  MAP_VM_COMMAND;
typedef MAP_VM_COMMAND::iterator                        MAP_VM_COMMAND_ITER;
typedef MAP_VM_COMMAND::const_iterator                  MAP_VM_COMMAND_CITER;

namespace vm
{
    struct NAME_TEXTURE
    {
        TSTRING             m_strName;
        ic::TextureResourceInterface m_textureRes;
    };

    typedef std::vector<NAME_TEXTURE>       VEC_TEXTURE_RESOURCE;
    typedef VEC_TEXTURE_RESOURCE::iterator  VEC_TEXTURE_RESOURCE_ITER;


    struct NAME_CUBE_TEXTURE
    {
        TSTRING                 m_strName;
        ic::TextureResourceInterface m_textureRes;
    };

    typedef std::vector<NAME_CUBE_TEXTURE>       VEC_CUBE_TEXTURE_RESOURCE;
    typedef VEC_CUBE_TEXTURE_RESOURCE::iterator  VEC_CUBE_TEXTURE_RESOURCE_ITER;

    enum EM_WHERE_CODE
    {
        EMWC_NEUTRAL = 0,   // 일단 중립    - EMWC_SCALAR가 연결이 안되면 HLSL에 기입.
        EMWC_SCALAR = 1,    // 스칼라로 빠짐
        EMWC_VECTOR = 2,    // Vector로 빠짐
        EMWC_HLSL = 3,      // HLSL로 됨
    };

    typedef std::vector<std::tr1::shared_ptr<DxVMCommand_EX>>   VEC_PIXEL_VECTOR;
    typedef VEC_PIXEL_VECTOR::iterator                          VEC_PIXEL_VECTOR_ITER;

    typedef std::map<TSTRING,D3DXVECTOR4>           MAP_PARAMETER_NAME_DATA;
    typedef MAP_PARAMETER_NAME_DATA::iterator       MAP_PARAMETER_NAME_DATA_ITER;
    typedef MAP_PARAMETER_NAME_DATA::const_iterator MAP_PARAMETER_NAME_DATA_CITER;
};
