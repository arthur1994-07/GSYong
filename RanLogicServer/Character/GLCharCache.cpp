#include "../pch.h"
#include "./GLCharCache.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLCharCache::GLCharCache(DWORD UserDbNum, DWORD ChaDbNum)
    : m_UserDbNum(UserDbNum)
    , m_ChaDbNum(ChaDbNum)
{
}

GLCharCache::~GLCharCache()
{
}