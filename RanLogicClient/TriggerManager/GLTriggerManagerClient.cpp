#include "pch.h"

#include "./GLTriggerManagerClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace triggermanager;

// ----------------------------------------------------------------------------

GLTriggerManagerClient::GLTriggerManagerClient()
{
    //Blank
}

GLTriggerManagerClient::~GLTriggerManagerClient()
{
    //Blank
}

GLTriggerManagerClient& GLTriggerManagerClient::GetInstance()
{
    static GLTriggerManagerClient sInstance;
    return sInstance;
}
