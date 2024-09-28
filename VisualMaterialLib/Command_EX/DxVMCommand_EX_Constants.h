#pragma once

#include "../Command/DxVMCommand_Constants.h"
#include "./DxVMCommand_EX.h"

//--------------------------------------------------------------- DxVMCommand_EX_Constant
class DxVMCommand_EX_Constant : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    VMCommand_Constant_Data m_sData;

    // virtual
public:
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Clone( DxVMCommand* pVMCommand );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_Constant(void);
	virtual ~DxVMCommand_EX_Constant(void);
};

//--------------------------------------------------------------- DxVMCommand_EX_Constant2Vector
class DxVMCommand_EX_Constant2Vector : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    VMCommand_Constant2Vector_Data m_sData;

    // virtual
public:
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Clone( DxVMCommand* pVMCommand );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_Constant2Vector(void);
	virtual ~DxVMCommand_EX_Constant2Vector(void);
};

//--------------------------------------------------------------- DxVMCommand_EX_Constant3Vector
class DxVMCommand_EX_Constant3Vector : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    VMCommand_Constant3Vector_Data m_sData;

    // virtual
public:
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Clone( DxVMCommand* pVMCommand );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_Constant3Vector(void);
	virtual ~DxVMCommand_EX_Constant3Vector(void);
};

//--------------------------------------------------------------- DxVMCommand_EX_Constant4Vector
class DxVMCommand_EX_Constant4Vector : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    VMCommand_Constant4Vector_Data m_sData;

    // virtual
public:
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Clone( DxVMCommand* pVMCommand );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_Constant4Vector(void);
	virtual ~DxVMCommand_EX_Constant4Vector(void);
};
