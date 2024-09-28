#pragma once

#include "./DxVMCommand.h"

//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Add
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_Add
class DxVMCommand_Add : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x100,
    };

public:
    friend class PropertyGridAdd;

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
	DxVMCommand_Add(void);
	virtual ~DxVMCommand_Add(void);
};



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Multiply
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_Multiply
class DxVMCommand_Multiply : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x101,
    };

public:
    friend class PropertyGridMultiply;

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
	DxVMCommand_Multiply(void);
	virtual ~DxVMCommand_Multiply(void);
};



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Sine
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Sine_Data
class VMCommand_Sine_Data : public VMCommand_Data
{
private:
    enum
    {
        VERSION = 0x100,
    };

public:
    float   m_fPeriod;

public:
    virtual void Clone( VMCommand_Data* pData );

public:
    void Save( ic::SerialFileInterface* pSFile );
    void Load( ic::SerialFileInterface* pSFile );

public:
    VMCommand_Sine_Data();
    virtual ~VMCommand_Sine_Data();
};

//--------------------------------------------------------------- DxVMCommand_Sine
class DxVMCommand_Sine : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x101,
    };

private:
    VMCommand_Sine_Data m_sData;

public:
    friend class PropertyGridSine;

    // virtual
public:
    virtual BOOL Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos );
    virtual VMCommand_Data* GetData()                          { return &m_sData; }
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
	DxVMCommand_Sine(void);
	virtual ~DxVMCommand_Sine(void);
};

//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Subtract
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_Subtract
class DxVMCommand_Subtract : public DxVMCommand
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
	DxVMCommand_Subtract(void);
	virtual ~DxVMCommand_Subtract(void);
};