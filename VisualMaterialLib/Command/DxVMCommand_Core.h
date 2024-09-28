#pragma once

#include "./DxVMCommand.h"

//--------------------------------------------------------------- DxVMCommand_Core
class DxVMCommand_Core : public DxVMCommand
{
private:
    enum
    {
        VERSION = 0x104,
    };

    // Material
private:
    float           m_fOpacityMaskClipValue;
    int             m_nBlendMode;               // VM_BLENDMODE
    int             m_nTwoSidedMode;
    BOOL            m_bWireframe;
    BOOL            m_bCharUV_MIRROR;
    BOOL            m_bUserColorOverlay;

    // MaterialColorPower
private:
    float   m_fDiffusePower;
    float   m_fAmbientPower;
    float   m_fSpecularPower;
    BOOL    m_bShininessPowerTexture;   // 광택도를 텍스쳐에서 얻어옴
    float   m_fShininessPower;          // 광택도 수치
    float   m_fLevelPower;
    float   m_fLevelFlowPower;

    // Translucency
private:
    BOOL    m_bDisableDepthTest;

    // MaterialInterface
private:
    TSTRING m_strPreviewMesh;

public:
    friend class PropertyGridCore;

private:
    std::string GetStringFX_PowerValue( int nIndex );
    std::string GetStringFX_Wireframe();
    std::string GetStringFX_AlphaREF();
    std::string GetStringFX_DisableDepthTest();

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
    virtual void SetCommentFX( COMMENT_FX& sCommentFX );

public:
	DxVMCommand_Core(void);
	virtual ~DxVMCommand_Core(void);
};
