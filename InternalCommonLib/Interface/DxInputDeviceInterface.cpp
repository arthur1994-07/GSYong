#include "stdafx.h"

#include "DxInputDeviceInterface.h"

// ----------------------------------------------------------------------------
#include "../debuginclude.h"
// ----------------------------------------------------------------------------


namespace ic
{

    DxInputDeviceInterface* DxInputDeviceInterface::g_pInstance = NULL;

    void DxInputDeviceInterface::SetInstance( DxInputDeviceInterface* pInstance )
    { 
        g_pInstance = pInstance; 
    }

    DxInputDeviceInterface* DxInputDeviceInterface::GetInstance()
    { 
        return g_pInstance;
    }

    DxInputDeviceInterface::DxInputDeviceInterface()
    {
    }

    DxInputDeviceInterface::~DxInputDeviceInterface()
    {
    }

};