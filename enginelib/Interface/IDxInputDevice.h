#pragma once

#include "../../InternalCommonLib/Interface/DxInputDeviceInterface.h"

class IDxInputDevice : public ic::DxInputDeviceInterface
{
public:
    virtual DWORD GetKeyState ( BYTE ScanCode );
    virtual DWORD GetMouseState ( ic::DX_MOUSEKEYS MKey );
    virtual void GetMouseMove ( int &_ndx, int &_ndy, int &_ndz );

public:
    IDxInputDevice();
    virtual ~IDxInputDevice();
};
