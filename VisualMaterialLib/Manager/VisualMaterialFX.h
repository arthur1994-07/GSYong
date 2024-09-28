#pragma once

#include "../Define/DxVisualMaterialDef.h"
#include "../Define/DxVMCommandDef.h"
#include "../Define/DxVMDefine.h"

namespace ic
{
    class SerialFileInterface;
};

//------------------------------------------------------------------
//                  VisualMaterialFX
class VisualMaterialFX
{
private:
    ID3DXEffect*                    m_pFX;
    std::string                     m_strFX;
    vm::VEC_TEXTURE_RESOURCE        m_vecTextureResource;
    vm::VEC_CUBE_TEXTURE_RESOURCE   m_vecCubeTextureResource;
    vm::VEC_PIXEL_VECTOR            m_vecPixelScalar;
    vm::VEC_PIXEL_VECTOR            m_vecPixelVector;
    BOOL                            m_bTwoSideSeperatePass;
    VM_BLENDMODE                    m_emBlendMode;
	BOOL							m_bActiveStarGlow;

public:
    CTime                   m_cLastWriteTime;      // Don't Save & Load

public:
    ID3DXEffect* GetFX();

    DWORD GetTextureSize();
    const TCHAR* GetTextureName( size_t i );
    LPDIRECT3DTEXTUREQ GetTexture( size_t i );

    DWORD GetCubeTextureSize();
    LPDIRECT3DCUBETEXTUREQ GetCubeTexture( size_t i );

    BOOL GetTwoSideSeperatePass()
    { 
        return m_bTwoSideSeperatePass; 
    }

    VM_BLENDMODE GetBlendMode()     
    { 
        return m_emBlendMode; 
    }

	BOOL IsActiveStarGlow()
	{
		return m_bActiveStarGlow;
	}

    void FrameMove( const vm::MAP_PARAMETER_NAME_DATA& mapParamaterNameData, float fTime );

    HRESULT Load( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strFileName, ic::SerialFileInterface* pSFile, NS_VM_MANAGER::RENDER_TYPE emRenderTYPE );

    HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
    HRESULT OnLostDevice();

    void SetTexturePointerNULL();

    void CleanUp();

private:
    HRESULT LoadDxVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strFileName, NS_VM_MANAGER::RENDER_TYPE emRenderTYPE );

public:
	VisualMaterialFX(void);
	~VisualMaterialFX(void);
};

