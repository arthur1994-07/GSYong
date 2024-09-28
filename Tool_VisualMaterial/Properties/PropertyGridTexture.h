#pragma once

#define _XTP_STATICLINK
#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components

class DxVMCommand;
class DxVMCommand_TextureSample;

class PropertyGridTextureSample
{
private:
    DxVMCommand_TextureSample*    m_rVMCommand;

public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridTextureSample();
    ~PropertyGridTextureSample();
};