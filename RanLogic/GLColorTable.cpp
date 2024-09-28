#include "pch.h"
#include "./GLColorTable.h"
#include "./GLogicData.h"
#include "./GLogicDataMan.h"

#include "../SigmaCore/String/basestring.h"
#include "../SigmaCore/Container/MList.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/String/StringFormat.h"

#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/Common/GLTexFile.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace GLCONST_COLORTABLE
{
    std::vector<DWORD> dw_COLORSELECTOR_TABLE_DEFAULT;
    std::vector<DWORD> dw_COLORSELECTOR_TABLE_RESTRICT;

    BOOL LOADFILE ( const std::string& FileName, BOOL bServer )
    {
        if( strlen(FileName.c_str()) == 0 )	return FALSE;

        std::string strPath;
        strPath = GLOGIC::GetPath();
        strPath += FileName;

        gltexfile cFILE;
        cFILE.reg_sep("\t ,{}[]()|\"");

        if (GLOGIC::UseLogicZipFile())
            cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

        if (!cFILE.open(strPath, true, GLOGIC::UseLogicPackFile()))
        {
            sc::writeLogError(
                sc::string::format(
                "GLCONST_CHAR LOADFILE %1%", strPath));            
            return FALSE;
        }

        int nSize = 0;

        //------------------------------------------------------------------------------//
        if ( !cFILE.getflag( "dw_COLORSELECTOR_TABLE_DEFAULT", 1, 1, nSize, true ) )
        {
            return FALSE;
        }

        GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_DEFAULT.resize(nSize);

        for ( int i=0; i<nSize; ++i )
        {
            DWORD dwColor = 0;

            for ( int j=0; j<3; ++j )
            {
                int nColor = 0;

                if ( cFILE.getflag( i, "CTD_RGB", (j+1), 3, nColor, true ) )
                {
                    dwColor |= ((nColor) << (8*(3-j-1)));
                }
            }

            GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_DEFAULT[i] = 0xFF000000 | dwColor;
        }

        //------------------------------------------------------------------------------//
        if ( !cFILE.getflag( "dw_COLORSELECTOR_TABLE_RESTRICT", 1, 1, nSize, true ) )
        {
            return FALSE;
        }

        GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_RESTRICT.resize(nSize);

        for ( int i=0; i<nSize; ++i )
        {
            DWORD dwColor = 0;

            for ( int j=0; j<3; ++j )
            {
                int nColor = 0;

                if ( cFILE.getflag( i, "CTR_RGB", (j+1), 3, nColor, true ) )
                {
                    dwColor |= ((nColor) << (8*(3-j-1)));
                }
            }

            GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_RESTRICT[i] = 0xFF000000 | dwColor;
        }

        //-------------------------------------------------------------------------------------------------//

        return TRUE;
    }
};
