#include "pch.h"
#include "DeviceMan.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

EngineDeviceMan::EngineDeviceMan(EMSERVICE_PROVIDER emServiceProvider)
    : m_emServiceProvider(emServiceProvider)
{
}

EngineDeviceMan::~EngineDeviceMan()
{
}