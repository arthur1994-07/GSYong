#pragma once

#define _XTP_STATICLINK
#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components

class DxVMCommand;


//---------------------------------------------------------------------------------------------------------- PropertyGridDesc
class PropertyGridDesc
{
public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridDesc();
    ~PropertyGridDesc();
};
