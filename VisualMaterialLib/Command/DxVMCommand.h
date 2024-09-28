#pragma once

#include <boost/tr1/memory.hpp>

#include "../Define/DxVMCommandDef.h"
#include "../Define/DxVMDefine.h"

class DxEditVM;
class DxVMCommandFX;

namespace ic
{
    class SerialFileInterface;
};

//--------------------------------------------------------------- VMCommand_Data
class VMCommand_Data
{
public:
    virtual void Clone( VMCommand_Data* pData ) = 0;

public:
    VMCommand_Data()            {}
    virtual ~VMCommand_Data()   {}
};

//--------------------------------------------------------------- DxVMCommand
class DxVMCommand
{
private:
    enum
    {
        VERSION = 0x102,
    };

    // Save & Load
protected:
    VMC_TYPE            m_emType;
    DWORD               m_dwID;     // Command ID
    D3DXVECTOR2         m_vPos;
    VEC_VMLINEDATA      m_vecLineData;
    TSTRING             m_strDesc;

    // Data
protected:
    float                               m_fTime;
    LPDIRECT3DDEVICEQ                   m_pd3dDevice;
    std::tr1::shared_ptr<DxVMCommandFX>    m_spVMCommandFX;
    vm::VEC_TEXTURE_RESOURCE            m_vecTextureResource;       // TextureResource
    vm::VEC_CUBE_TEXTURE_RESOURCE       m_vecCubeTextureResource;   // 
    vm::VEC_PIXEL_VECTOR                m_vecPixelScalar;
    vm::VEC_PIXEL_VECTOR                m_vecPixelVector;

    // Edit
protected:
    std::tr1::shared_ptr<DxEditVM> m_spEditVM_Title;
    std::tr1::shared_ptr<DxEditVM> m_spEditVM_Command;
    float           m_fBaseWidth;
    float           m_fBaseHeight;

    // static
public:
    static std::tr1::shared_ptr<DxVMCommand> NEW_COMMAND_STATIC( VMC_TYPE emType );

    // virtual
public:
    // = 0;
    virtual void Refresh() = 0;
    virtual std::string MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                        DWORD dwOutputIndex,
                                        vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                                        vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                        vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                        vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                        vm::EM_WHERE_CODE& emWhereCode ) = 0;

    // 전부사용
    virtual void Save( ic::SerialFileInterface* pSFile );
    virtual void Load( ic::SerialFileInterface* pSFile );
    virtual void SetCommentFX( COMMENT_FX& sCommentFX )         {}
    virtual VMCommand_Data* GetData()                           { return NULL; }
    virtual BOOL Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos );

    // DxVMCommand 에서만 사용
    BOOL OnResetDevice();
    void OnLostDevice();
    void FrameMove( float fTime );
    void Render( VM_CAN_SELECT_INDEX& sCanSelectIndex, const D3DXVECTOR2& vMousePt );
    void SetYellowBox( VM_CAN_SELECT_INDEX& sCanSelectIndex );
    const VEC_VMLINEDATA& GetVMLineData();
    void SetID( DWORD dwID );
    DWORD GetID();
    VMC_TYPE GetType();
    void LineLink( int nIndexBox, const VM_CAN_SELECT_INDEX& sCanSelectIndex );
    void GetPointInput( D3DXVECTOR3& vPoint, int nIndexBox );
    void GetPointOutput( D3DXVECTOR3& vPoint, int nIndexBox );
    void DeleteLineLink( const VM_CAN_SELECT_INDEX& sSelectIndexBox );
    void DeleteLineLink( DWORD dwID );
    void SetSelectBaseBox( BOOL bSelect );
    void Move( const D3DXVECTOR2& vMove );
    BOOL IsSelectCommandAtBox( const D3DXVECTOR2& vMin, const D3DXVECTOR2& vMax );
    BOOL MakeFX( const MAP_VM_COMMAND& mapVM_Command );
    BOOL MakeFX( DxVMCommandFX* pVMCommandFX,
                    const MAP_VM_COMMAND& mapVM_Command, 
                    vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                    vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                    vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                    vm::VEC_PIXEL_VECTOR& vecPixelVector,
                    VMFX emModeFX );
    BOOL CreateCommand_EX( const MAP_VM_COMMAND& mapVM_Command, std::tr1::shared_ptr<DxVMCommand_EX>& spVMCommandEX );
    void ClearFX();
    TSTRING* GetDescPointer();

private:
    void CleanUp();

public:
	DxVMCommand(void);
	virtual ~DxVMCommand(void);
};
