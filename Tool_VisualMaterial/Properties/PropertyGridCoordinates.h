#pragma once

#define _XTP_STATICLINK
#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components

class DxVMCommand;
class DxVMCommand_Panner;
class DxVMCommand_Rotator;
class DxVMCommand_TextureCoordinate;


//---------------------------------------------------------------------------------------------------------- PropertyGridPanner
class PropertyGridPanner
{
private:
    DxVMCommand_Panner*    m_rVMCommand;

public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridPanner();
    ~PropertyGridPanner();
};


//---------------------------------------------------------------------------------------------------------- PropertyGridRotator
class PropertyGridRotator
{
private:
    DxVMCommand_Rotator*    m_rVMCommand;

public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridRotator();
    ~PropertyGridRotator();
};


//---------------------------------------------------------------------------------------------------------- PropertyGridTextureCoordinate
class PropertyGridTextureCoordinate
{
private:
    DxVMCommand_TextureCoordinate*    m_rVMCommand;

public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridTextureCoordinate();
    ~PropertyGridTextureCoordinate();
};