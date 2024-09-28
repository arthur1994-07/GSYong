#include "pch.h"

#include "./GLTriggerManagerAG.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace triggermanager;

// ----------------------------------------------------------------------------

GLTriggerManagerAG::GLTriggerManagerAG()
{
    //Blank
}

GLTriggerManagerAG::~GLTriggerManagerAG()
{
    //Blank
}

GLTriggerManagerAG& GLTriggerManagerAG::GetInstance()
{
    static GLTriggerManagerAG sInstance;
    return sInstance;
}
