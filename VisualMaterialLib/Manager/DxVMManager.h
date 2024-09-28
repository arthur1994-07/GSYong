#pragma once

#include <boost/tr1/memory.hpp>

#include "../../InternalCommonLib/Interface/TextureManagerInterface.h"
#include "../Define/DxVisualMaterialDef.h"

namespace sc
{
    class SerialFile;
}

class VisualMaterialFX;

//------------------------------------------------------------------
//                  DxVMManager
// VisualMaterialFX 를 관리하는 namespace
namespace NS_VM_MANAGER
{   
    extern TSTRING                      g_strFullPath;
    extern LPDIRECT3DVERTEXDECLARATION9 g_pDCRT_PIECE;
    extern LPDIRECT3DVERTEXDECLARATION9 g_pDCRT_PIECE_COLOR;
    extern LPDIRECT3DVERTEXDECLARATION9 g_pDCRT_PIECE_COLOR_LM;
    extern LPDIRECT3DVERTEXDECLARATION9 g_pDCRT_SKIN;
    extern LPDIRECT3DVERTEXDECLARATION9 g_pDCRT_SKIN_OBJECT;
    extern ic::TextureResourceInterface g_textureResShininess;

    void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
    void OnLostDevice();

    void CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh );
    void CompressTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh );

    void CreateTangentColorMeshLM( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh );
    void CompressTangentColorMeshLM( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh );

    void CreateTangentColorMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh );
    void CompressTangentColorMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh, BOOL bHaveDiffuse );

    void CreateTangentSkinMeshObject( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh );
    void CompressTangentSkinMeshObject( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh );

    void CreateTangentSkinMeshVS( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh );
    void CompressTangentSkinMeshVS( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH* ppMesh );

    std::tr1::shared_ptr<VisualMaterialFX> Load( LPDIRECT3DDEVICEQ pd3dDevice, TSTRING strName, RENDER_TYPE emRenderTYPE );

    BOOL NeedReload( float fElapsedTime );

    // Tool 에서만 쓰이는 함수임.
    void ClearUpVMFX( BOOL bReleaseTexture );
};
