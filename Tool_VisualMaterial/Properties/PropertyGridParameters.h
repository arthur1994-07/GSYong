#pragma once

#define _XTP_STATICLINK
#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components

class DxVMCommand;
class DxVMCommand_VectorParameter;

//---------------------------------------------------------------------------------------------------------- PropertyGridVectorParameter
class PropertyGridVectorParameter
{
private:
    DxVMCommand_VectorParameter*    m_rVMCommand;

public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridVectorParameter();
    ~PropertyGridVectorParameter();
};