#include "../pch.h"
#include "../PK/GLSchoolFreePK.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool GLGaeaServer::SchoolFreePkOn() const
{
    return m_pSchoolFreePk->IsON();
}

void GLGaeaServer::SchoolFreePkSet(bool Set)
{
    m_pSchoolFreePk->SET(Set);
}