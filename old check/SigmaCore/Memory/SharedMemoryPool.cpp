#include "stdafx.h"
#include "./SharedMemoryPool.h"

// ----------------------------------------------------------------------------
#include "../DebugInclude.h"
// ----------------------------------------------------------------------------

namespace sc
{
    template <class T, class LOCK_TYPE>
    SharedPtrFactorySingletone<T, LOCK_TYPE>* SharedPtrFactorySingletone<T, LOCK_TYPE>::SelfInstance = NULL;
}