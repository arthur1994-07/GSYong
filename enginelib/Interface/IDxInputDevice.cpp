#include "pch.h"

#include "../DxTools/DxInputDevice.h"

#include "IDxInputDevice.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IDxInputDevice::IDxInputDevice()
{
}

IDxInputDevice::~IDxInputDevice()
{
}

DWORD IDxInputDevice::GetKeyState ( BYTE ScanCode )
{
    return DxInputDevice::GetInstance().GetKeyState( ScanCode );
}

DWORD IDxInputDevice::GetMouseState ( ic::DX_MOUSEKEYS MKey )
{
    DX_MOUSEKEYS emMKey = static_cast<DX_MOUSEKEYS>( MKey );
    return DxInputDevice::GetInstance().GetMouseState( emMKey );
}

void IDxInputDevice::GetMouseMove ( int &_ndx, int &_ndy, int &_ndz )
{
    return DxInputDevice::GetInstance().GetMouseMove( _ndx, _ndy, _ndz );
}