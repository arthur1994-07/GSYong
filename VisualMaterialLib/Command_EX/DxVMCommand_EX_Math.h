#pragma once

#include "../Command/DxVMCommand_Math.h"
#include "./DxVMCommand_EX.h"

class DxVMCommand;



//--------------------------------------------------------------- DxVMCommand_EX_Add
class DxVMCommand_EX_Add : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

    // virtual
public:
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_Add(void);
	virtual ~DxVMCommand_EX_Add(void);
};


//--------------------------------------------------------------- DxVMCommand_EX_Multiply
class DxVMCommand_EX_Multiply : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

    // virtual
public:
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_Multiply(void);
	virtual ~DxVMCommand_EX_Multiply(void);
};


//--------------------------------------------------------------- DxVMCommand_EX_Sine
class DxVMCommand_EX_Sine : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    VMCommand_Sine_Data m_sData;

    // virtual
public:
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Clone( DxVMCommand* pVMCommand );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_Sine(void);
	virtual ~DxVMCommand_EX_Sine(void);
};

//--------------------------------------------------------------- DxVMCommand_EX_Sub
class DxVMCommand_EX_Sub : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

    // virtual
public:
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_Sub(void);
	virtual ~DxVMCommand_EX_Sub(void);
};