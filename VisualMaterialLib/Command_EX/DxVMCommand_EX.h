#pragma once

#include <boost/tr1/memory.hpp>

#include "../Define/DxVMCommandDef.h"
#include "../Define/DxVMDefine.h"

class DxEditVM;
class DxVMCommand;
class DxVMCommandFX;

namespace ic
{
    class SerialFileInterface;
};

//--------------------------------------------------------------- DxVMCommand_EX
class DxVMCommand_EX
{
private:
    enum
    {
        VERSION = 0x100,
    };

    typedef std::vector<std::tr1::shared_ptr<DxVMCommand_EX>>  VEC_COMMAND_EX;
    typedef VEC_COMMAND_EX::iterator                        VEC_COMMAND_EX_ITER;

    // Save & Load
protected:
    VMC_TYPE        m_emType;
    VEC_COMMAND_EX  m_vecChildCommandEX;

    // static
public:
    static std::tr1::shared_ptr<DxVMCommand_EX> NEW_COMMAND_STATIC( VMC_TYPE emType );

    // virtual
public:
    virtual D3DXVECTOR4 GetPixelVector( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData ) = 0;
    virtual void Clone( DxVMCommand* pVMCommand )   {}
    virtual void FrameMove( float fTime );
    virtual void SetLink( std::tr1::shared_ptr<DxVMCommand_EX>& spVMCommand_EX );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
    VMC_TYPE GetType();

public:
	DxVMCommand_EX(void);
	virtual ~DxVMCommand_EX(void);
};


