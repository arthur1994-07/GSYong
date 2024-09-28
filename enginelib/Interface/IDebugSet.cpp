#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../DxTools/DebugSet.h"

#include "IDebugSet.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IDebugSet::IDebugSet()
{
}

IDebugSet::~IDebugSet()
{
}

void IDebugSet::ToLogFile( const char *szFormat, ... )
{
    #define MAX_STRLENGTH	(256)

    va_list argList;
    char szBuffer[MAX_STRLENGTH] = "";

	va_start(argList, szFormat);
	StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
	va_end(argList);

    sc::writeLogError(std::string(szBuffer));
}