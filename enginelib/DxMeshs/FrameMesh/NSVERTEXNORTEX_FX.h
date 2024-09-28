#pragma once


namespace NSVERTEXNORTEX_FX
{
    void CleanUp();

    HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
    HRESULT OnLostDevice();

//    ID3DXEffect* GetFX();

    HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice );

    void Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, DWORD AttribId );

    void Render( LPDIRECT3DDEVICEQ pd3dDevice, 
                    LPDIRECT3DVERTEXBUFFERQ	pVB,
                    LPDIRECT3DINDEXBUFFERQ pIB,
                    int nFVFSize,
                    DWORD dwFVF,
                    DWORD dwVertNum, 
                    DWORD dwFaceNum );

    void Render( LPDIRECT3DDEVICEQ pd3dDevice, 
                DWORD dwFVF,
                UINT PrimitiveCount,
                CONST void* pVertexStreamZeroData,
                UINT VertexStreamZeroStride );

    // FX Param ¼ÂÆÃ.
    void FrameMove();
    void SetBaseTexture( LPDIRECT3DTEXTUREQ pTexture );
    void SetWorld( const D3DXMATRIX& matWorld, float fAlpha );

    void SetEnable();
    void SetDisable();
}