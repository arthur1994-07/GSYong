#include "stdafx.h"

#include "DebugSetInterface.h"

// ----------------------------------------------------------------------------
#include "../debuginclude.h"
// ----------------------------------------------------------------------------


namespace ic
{

    DebugSetInterface* DebugSetInterface::g_pInstance = NULL;

    void DebugSetInterface::SetInstance( DebugSetInterface* pInstance )
    { 
        g_pInstance = pInstance; 
    }

    DebugSetInterface* DebugSetInterface::GetInstance()
    { 
        return g_pInstance;
    }

    DebugSetInterface::DebugSetInterface()
    {
    }

    DebugSetInterface::~DebugSetInterface()
    {
    }

};