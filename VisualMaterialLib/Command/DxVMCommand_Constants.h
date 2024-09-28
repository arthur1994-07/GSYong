#pragma once

#include "./DxVMCommand.h"

//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Constant
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Constant_Data
class VMCommand_Constant_Data : public VMCommand_Data
{
private:
    enum
    {
        VERSION = 0x100,
    };

public:
    float   m_fR;

public:
    virtual void Clone( VMCommand_Data* pData );

public:
    void Save( ic::SerialFileInterface* pSFile );
    void Load( ic::SerialFileInterface* pSFile );

public:
    VMCommand_Constant_Data();
    virtual ~VMCommand_Constant_Data();
};

//--------------------------------------------------------------- DxVMCommand_Constant
class DxVMCommand_Constant : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x101,
    };

private:
    VMCommand_Constant_Data m_sData;

public:
    friend class PropertyGridConstant;

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
	DxVMCommand_Constant(void);
	virtual ~DxVMCommand_Constant(void);
};


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Constant2Vector
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Constant2Vector_Data
class VMCommand_Constant2Vector_Data : public VMCommand_Data
{
private:
    enum
    {
        VERSION = 0x100,
    };

public:
    float   m_fR;
    float   m_fG;

public:
    virtual void Clone( VMCommand_Data* pData );

public:
    void Save( ic::SerialFileInterface* pSFile );
    void Load( ic::SerialFileInterface* pSFile );

public:
    VMCommand_Constant2Vector_Data();
    virtual ~VMCommand_Constant2Vector_Data();
};

//--------------------------------------------------------------- DxVMCommand_Constant2Vector
class DxVMCommand_Constant2Vector : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    VMCommand_Constant2Vector_Data  m_sData;

public:
    friend class PropertyGridConstant2Vector;

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
	DxVMCommand_Constant2Vector(void);
	virtual ~DxVMCommand_Constant2Vector(void);
};


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Constant3Vector
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Constant2Vector_Data
class VMCommand_Constant3Vector_Data : public VMCommand_Data
{
private:
    enum
    {
        VERSION = 0x100,
    };

public:
    float   m_fR;
    float   m_fG;
    float   m_fB;

public:
    virtual void Clone( VMCommand_Data* pData );

public:
    void Save( ic::SerialFileInterface* pSFile );
    void Load( ic::SerialFileInterface* pSFile );

public:
    VMCommand_Constant3Vector_Data();
    virtual ~VMCommand_Constant3Vector_Data();
};

//--------------------------------------------------------------- DxVMCommand_Constant3Vector
class DxVMCommand_Constant3Vector : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    VMCommand_Constant3Vector_Data  m_sData;

public:
    friend class PropertyGridConstant3Vector;

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
	DxVMCommand_Constant3Vector(void);
	virtual ~DxVMCommand_Constant3Vector(void);
};


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Constant4Vector
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Constant4Vector_Data
class VMCommand_Constant4Vector_Data : public VMCommand_Data
{
private:
    enum
    {
        VERSION = 0x100,
    };

public:
    float   m_fR;
    float   m_fG;
    float   m_fB;
    float   m_fA;

public:
    virtual void Clone( VMCommand_Data* pData );

public:
    void Save( ic::SerialFileInterface* pSFile );
    void Load( ic::SerialFileInterface* pSFile );

public:
    VMCommand_Constant4Vector_Data();
    virtual ~VMCommand_Constant4Vector_Data();
};

//--------------------------------------------------------------- DxVMCommand_Constant4Vector
class DxVMCommand_Constant4Vector : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    VMCommand_Constant4Vector_Data  m_sData;

public:
    friend class PropertyGridConstant4Vector;

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
	DxVMCommand_Constant4Vector(void);
	virtual ~DxVMCommand_Constant4Vector(void);
};