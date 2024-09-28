#include "../pch.h"
#include "./NetCacheClientMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

NetCacheClientMan::NetCacheClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax)
    : NetClientManager(nMaxClient, hIOServer, nReferenceCountMax)
{

}

NetCacheClientMan::~NetCacheClientMan()
{
}