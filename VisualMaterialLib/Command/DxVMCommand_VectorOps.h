#pragma once

#include "./DxVMCommand.h"


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_Fresnel
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- DxVMCommand_Fresnel
class DxVMCommand_Fresnel : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    float   m_fExponent;

public:
    friend class PropertyGridFresnel;

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
	DxVMCommand_Fresnel(void);
	virtual ~DxVMCommand_Fresnel(void);
};