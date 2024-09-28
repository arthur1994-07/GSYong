#pragma once

#include <vector>

class CBaseStringParser
{
public:
    static bool Parse( const CString& strFileName, std::vector< CString >& vecString );
    static INT RandomSelect( std::vector< CString >& vecString );
};