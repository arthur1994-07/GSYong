#pragma once

#include "../Command/DxVMCommand_Parameters.h"
#include "./DxVMCommand_EX.h"

class DxVMCommand;


//--------------------------------------------------------------- DxVMCommand_EX_VectorParameter
class DxVMCommand_EX_VectorParameter : public DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    VMCommand_VectorParameter_Data m_sData;

    // virtual
public:
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData );
    virtual void Clone( DxVMCommand* pVMCommand );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_EX_VectorParameter(void);
	virtual ~DxVMCommand_EX_VectorParameter(void);
};