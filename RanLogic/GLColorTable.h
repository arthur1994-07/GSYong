// bjju.colorpicker

#pragma once

#include <string>
#include <vector>

namespace GLCONST_COLORTABLE
{

    extern std::vector<DWORD> dw_COLORSELECTOR_TABLE_DEFAULT;
    extern std::vector<DWORD> dw_COLORSELECTOR_TABLE_RESTRICT;

    BOOL LOADFILE ( const std::string& FileName, BOOL bServer=TRUE );
};
