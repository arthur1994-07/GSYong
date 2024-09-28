#pragma once

#define _XTP_STATICLINK
#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components

class DxVMCommand;
class DxVMCommand_Core;

class PropertyGridCore
{
private:
    DxVMCommand_Core*    m_rVMCommand;
    
public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridCore();
    ~PropertyGridCore();
};