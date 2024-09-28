#pragma once

#include "../Command/DxVMCommand_Coordinates.h"
#include "./DxVMCommand_EX.h"


//--------------------------------------------------------------- DxVMCommand_EX_Panner
class DxVMCommand_EX_Panner : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    D3DXVECTOR2 m_vUV;

private:
    VMCommand_Panner_Data m_sData;

    // virtual
public:
    virtual void FrameMove( float fTime );
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Clone( DxVMCommand* pVMCommand );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_Panner(void);
	virtual ~DxVMCommand_EX_Panner(void);
};


//--------------------------------------------------------------- DxVMCommand_EX_Rotator
class DxVMCommand_EX_Rotator : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    D3DXVECTOR2 m_vVectorX;
    D3DXVECTOR2 m_vVectorY;

private:
    VMCommand_Rotator_Data m_sData;

    // virtual
public:
    virtual void FrameMove( float fTime );
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Clone( DxVMCommand* pVMCommand );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_Rotator(void);
	virtual ~DxVMCommand_EX_Rotator(void);
};