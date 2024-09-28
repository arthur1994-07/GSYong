#include "pch.h"

#include "./GLTriggerManagerFD.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace triggermanager
{

// ----------------------------------------------------------------------------

GLTriggerManagerFD::GLTriggerManagerFD()
{
    //Blank
}

GLTriggerManagerFD::~GLTriggerManagerFD()
{
    //Blank
}

GLTriggerManagerFD& GLTriggerManagerFD::GetInstance()
{
    static GLTriggerManagerFD sInstance;
    return sInstance;
}

} // namespace triggermanager