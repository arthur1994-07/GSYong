#include "pch.h"
#include "./OutInterfaceBase.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

OutInterfaceBase::OutInterfaceBase(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : m_pGaeaClient(pGaeaClient)
    , m_pEngineDevice(pEngineDevice)
{
}

OutInterfaceBase::~OutInterfaceBase()
{
}