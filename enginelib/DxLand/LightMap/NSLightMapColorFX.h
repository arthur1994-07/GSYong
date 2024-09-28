//#pragma once
//
//#include "./LightMapDef.h"
//
//namespace NSLightMapColorFX
//{
//    void CleanUp();
//
//    HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
//    HRESULT OnLostDevice();
//
//    ID3DXEffect* GetFX();
//
//    HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice );
//
//    void Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh );
//
//    // FX Param ¼ÂÆÃ.
//    void FrameMove();
//    void SetBaseTexture( LPDIRECT3DTEXTUREQ pTexture );
//    void SetLightMapTexture( LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight );
//    void SetLightMapUV_Offset( const D3DXVECTOR2& vLightMapUV_Offset );
//    void SetWorld( const D3DXMATRIX& matWorld, float fAlpha );
//
//    // Edit
//    void ShortBlackFog();
//}