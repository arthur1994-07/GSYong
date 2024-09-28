#pragma once

#include "./DxVMCommand.h"

//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_TextureSample
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_TextureSample
class DxVMCommand_TextureSample : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x102,
    };

private:
    BOOL    m_bCubeTexture;
    TSTRING m_strTexture;

    ic::TextureResourceInterface m_texrureRes2D;
    ic::TextureResourceInterface m_textureResCube;

public:
    friend class PropertyGridTextureSample;

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
	DxVMCommand_TextureSample(void);
	virtual ~DxVMCommand_TextureSample(void);
};


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_TextureRefract
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_TextureRefract
class DxVMCommand_TextureRefract : public DxVMCommand
{
private:
	enum
	{
		VERSION = 0x100,
	};

public:
	friend class PropertyGridTextureRefract;

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
	DxVMCommand_TextureRefract(void);
	virtual ~DxVMCommand_TextureRefract(void);
};