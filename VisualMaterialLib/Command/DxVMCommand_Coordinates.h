#pragma once

#include "./DxVMCommand.h"


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Panner
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Panner_Data
class VMCommand_Panner_Data : public VMCommand_Data
{
private:
    enum
    {
        VERSION = 0x100,
    };

public:
    float   m_fSpeedX;
    float   m_fSpeedY;

public:
    virtual void Clone( VMCommand_Data* pData );

public:
    void Save( ic::SerialFileInterface* pSFile );
    void Load( ic::SerialFileInterface* pSFile );

public:
    VMCommand_Panner_Data();
    virtual ~VMCommand_Panner_Data();
};

//--------------------------------------------------------------- DxVMCommand_Panner
class DxVMCommand_Panner : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    VMCommand_Panner_Data m_sData;

public:
    friend class PropertyGridPanner;

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
	DxVMCommand_Panner(void);
	virtual ~DxVMCommand_Panner(void);
};



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Rotator
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_Rotator_Data
class VMCommand_Rotator_Data : public VMCommand_Data
{
private:
    enum
    {
        VERSION = 0x100,
    };

public:
    float   m_fCenterX;
    float   m_fCenterY;
    float   m_fSpeed;

public:
    virtual void Clone( VMCommand_Data* pData );

public:
    void Save( ic::SerialFileInterface* pSFile );
    void Load( ic::SerialFileInterface* pSFile );

public:
    VMCommand_Rotator_Data();
    virtual ~VMCommand_Rotator_Data();
};

//--------------------------------------------------------------- DxVMCommand_Rotator
class DxVMCommand_Rotator : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x100,
    };
   
private:
    VMCommand_Rotator_Data m_sData;

public:
    friend class PropertyGridRotator;

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
	DxVMCommand_Rotator(void);
	virtual ~DxVMCommand_Rotator(void);
};



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_TextureCoordinate
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_TextureCoordinate
class DxVMCommand_TextureCoordinate : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    float   m_fUTiling;
    float   m_fVTiling;

public:
    friend class PropertyGridTextureCoordinate;

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
	DxVMCommand_TextureCoordinate(void);
	virtual ~DxVMCommand_TextureCoordinate(void);
};