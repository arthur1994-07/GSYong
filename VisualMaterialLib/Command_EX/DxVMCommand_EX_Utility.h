#pragma once

#include "./DxVMCommand_EX.h"


//--------------------------------------------------------------- DxVMCommand_EX_Time
class DxVMCommand_EX_Time : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    float m_fTime;

    // virtual
public:
    virtual void FrameMove( float fTime );
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_Time(void);
	virtual ~DxVMCommand_EX_Time(void);
};


//--------------------------------------------------------------- DxVMCommand_EX_Clamp
class DxVMCommand_EX_Clamp : public DxVMCommand_EX
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
	DxVMCommand_EX_Clamp(void);
	virtual ~DxVMCommand_EX_Clamp(void);
};