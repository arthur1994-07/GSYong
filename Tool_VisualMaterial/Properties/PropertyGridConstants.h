#pragma once

#define _XTP_STATICLINK
#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components

class DxVMCommand;
class DxVMCommand_Constant;
class DxVMCommand_Constant2Vector;
class DxVMCommand_Constant3Vector;
class DxVMCommand_Constant4Vector;

//---------------------------------------------------------------------------------------------------------- PropertyGridConstant
class PropertyGridConstant
{
private:
    DxVMCommand_Constant*    m_rVMCommand;

public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridConstant();
    ~PropertyGridConstant();
};


//---------------------------------------------------------------------------------------------------------- PropertyGridConstant2Vector
class PropertyGridConstant2Vector
{
private:
    DxVMCommand_Constant2Vector*    m_rVMCommand;

public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridConstant2Vector();
    ~PropertyGridConstant2Vector();
};


//---------------------------------------------------------------------------------------------------------- PropertyGridConstant3Vector
class PropertyGridConstant3Vector
{
private:
    DxVMCommand_Constant3Vector*    m_rVMCommand;

public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridConstant3Vector();
    ~PropertyGridConstant3Vector();
};


//---------------------------------------------------------------------------------------------------------- PropertyGridConstant4Vector
class PropertyGridConstant4Vector
{
private:
    DxVMCommand_Constant4Vector*    m_rVMCommand;

public:
    void CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand );

public:
    PropertyGridConstant4Vector();
    ~PropertyGridConstant4Vector();
};