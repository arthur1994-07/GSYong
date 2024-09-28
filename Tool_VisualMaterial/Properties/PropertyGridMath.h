#pragma once

#define _XTP_STATICLINK
#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components

class DxVMCommand;
class DxVMCommand_Sine;


//---------------------------------------------------------------------------------------------------------- PropertyGridSine
class PropertyGridSine
{
private:
    DxVMCommand_Sine*    m_rVMCommand;

public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridSine();
    ~PropertyGridSine();
};
