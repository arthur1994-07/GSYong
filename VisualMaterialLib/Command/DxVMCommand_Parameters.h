#pragma once

#include "./DxVMCommand.h"

//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_VectorParameter
//----------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------- VMCommand_VectorParameter_Data
class VMCommand_VectorParameter_Data : public VMCommand_Data
{
private:
    enum
    {
        VERSION = 0x101,
    };

public:
    float   m_fR;
    float   m_fG;
    float   m_fB;
    TSTRING m_strParameter;

public:
    virtual void Clone( VMCommand_Data* pData );

public:
    void Save( ic::SerialFileInterface* pSFile );
    void Load( ic::SerialFileInterface* pSFile );

public:
    VMCommand_VectorParameter_Data();
    virtual ~VMCommand_VectorParameter_Data();
};

//--------------------------------------------------------------- VMCommand_VectorParameter
class DxVMCommand_VectorParameter : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x101,
    };

private:
    VMCommand_VectorParameter_Data  m_sData;

public:
    friend class PropertyGridVectorParameter;

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
	DxVMCommand_VectorParameter(void);
	virtual ~DxVMCommand_VectorParameter(void);
};