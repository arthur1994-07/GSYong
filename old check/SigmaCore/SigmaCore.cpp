#include "stdafx.h"
#include "./Log/LogMan.h"
#include "./Math/Random.h"
#include "./Util/SystemInfo.h"
#include "./Lua/MinLua.h"
#include "./DebugSet.h"
#include "./SigmaCore.h"
#include "./String/StringUtils.h"

// ----------------------------------------------------------------------------
#include "DebugInclude.h"
// ----------------------------------------------------------------------------

namespace sc
{
    void initialize()
    {
        Random::getInstance();
        LogMan::getInstance();
		setLowFragmentationHeap();
        lua_init();
        STRUTIL::InitThaiDLL();
    }

    void finalize()
    {
        lua_destroyState();
		DebugSet::ReleaseInstance();
        LogMan::releaseInstance();
        STRUTIL::ReleaseThaiDLL();
    }
}