#include "stdafx.h"
#include "./ObjectPool.h"

// ----------------------------------------------------------------------------
#include "../DebugInclude.h"
// ----------------------------------------------------------------------------

namespace sc
{
    PoolAllocMgr::POOL_TABLE PoolAllocMgr::s_poolTable;
    DefaultLock PoolAllocMgr::m_lock;
}
