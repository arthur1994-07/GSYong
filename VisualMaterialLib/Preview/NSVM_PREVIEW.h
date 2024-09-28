#pragma once

#include <boost/tr1/memory.hpp>

#include "../Define/DxVMCommandDef.h"

class DxVMCommandFX;
class DxVMCommand_EX;

namespace NSVM_PREVIEW
{
    enum
    {
        MODEL_CYLINDER  = 0,
        MODEL_CUBE      = 1,
        MODEL_SPHERE    = 2,
        MODEL_PLANE     = 3,
    };

    extern UINT                                 g_nPreviewRenderStyle;
    extern D3DXVECTOR3                          g_vLightPosition;
    extern vm::VEC_TEXTURE_RESOURCE             g_vecTextureResource;
    extern vm::VEC_CUBE_TEXTURE_RESOURCE        g_vecCubeTextureResource;
    extern vm::VEC_PIXEL_VECTOR                 g_vecPixelScalar;
    extern vm::VEC_PIXEL_VECTOR                 g_vecPixelVector;
    extern std::tr1::shared_ptr<DxVMCommandFX>  g_spVMCommandFX;
    extern BOOL                                 g_bGridToggle;
    extern D3DXVECTOR4                          g_vUserColor1;
    extern D3DXVECTOR4                          g_vUserColor2;

    HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
    HRESULT OnLostDevice();
    HRESULT OnFrameMove( BOOL bTarPosWin, float fElapsedTime );
    HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bActivePL );
	HRESULT Render_StarGlow( LPDIRECT3DDEVICEQ pd3dDevice );	// Render ÈÄ µ¿ÀÛ
    void Reset();

    void LoadCustomMesh( const TSTRING& strName );
    const std::string& GetSourceString();
    const std::string& GetSourceString_Simple();

    void DrawMeshFX( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh );
};