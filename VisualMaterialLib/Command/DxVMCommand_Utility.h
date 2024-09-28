#pragma once

#include "./DxVMCommand.h"



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Clamp
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_Clamp
class DxVMCommand_Clamp : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x100,
    };

public:
    friend class PropertyGridClamp;

    // virtual
public:
    virtual BOOL Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos );
    virtual void Refresh();
    virtual std::string MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                        DWORD dwOutputIndex, 
                                        vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                        vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                        vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                        vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                        vm::EM_WHERE_CODE& emWhereCode );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_Clamp(void);
	virtual ~DxVMCommand_Clamp(void);
};



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_LinearInterpolate
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_LinearInterpolate
class DxVMCommand_LinearInterpolate : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x100,
    };

public:
    friend class PropertyGridSubtract;

    // virtual
public:
    virtual BOOL Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos );
    virtual void Refresh();
    virtual std::string MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                        DWORD dwOutputIndex, 
                                        vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                        vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                        vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                        vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                        vm::EM_WHERE_CODE& emWhereCode );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_LinearInterpolate(void);
	virtual ~DxVMCommand_LinearInterpolate(void);
};




//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Time
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_Time
class DxVMCommand_Time : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x101,
    };

public:
    friend class PropertyGridTime;

    // virtual
public:
    virtual BOOL Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos );
    virtual void Refresh();
    virtual std::string MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                        DWORD dwOutputIndex, 
                                        vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                        vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                        vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                        vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                        vm::EM_WHERE_CODE& emWhereCode );
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );

public:
	DxVMCommand_Time(void);
	virtual ~DxVMCommand_Time(void);
};




