#include "pch.h"
#include "./EmulDbMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLEmulatorDBMan::GLEmulatorDBMan(NetServer* pServer, db::SERVER_TYPE ServerType, EMSERVICE_PROVIDER ServiceProvider)
    : db::IDbManager(pServer, ServerType, ServiceProvider, db::EMULATOR)
{
}

GLEmulatorDBMan::~GLEmulatorDBMan()
{
}