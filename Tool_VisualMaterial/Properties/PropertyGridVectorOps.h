#pragma once

#define _XTP_STATICLINK
#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components

class DxVMCommand;
class DxVMCommand_Fresnel;


//---------------------------------------------------------------------------------------------------------- PropertyGridFresnel
class PropertyGridFresnel
{
private:
    DxVMCommand_Fresnel*    m_rVMCommand;

public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridFresnel();
    ~PropertyGridFresnel();
};