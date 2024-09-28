#pragma once

class DxTangentMesh
{
private:
    LPDIRECT3DVERTEXBUFFERQ	m_pVB;
	LPDIRECT3DINDEXBUFFERQ	m_pIB;
    LPD3DXATTRIBUTERANGE	m_pAttrib;

    DWORD					m_dwVert;
	DWORD					m_dwFace;
	DWORD					m_dwAttrib;

public:
    HRESULT CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, DWORD dwFVF, DWORD dwVert, DWORD dwFace );
    HRESULT CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pSrcMesh );
    HRESULT CreateColorMeshLM( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pSrcMesh );
    HRESULT CreateSkinMeshObject( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pSrcMesh );
    HRESULT CreateColorMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, DWORD dwFVF, DWORD dwVert, DWORD dwFace );
    HRESULT CreateColorMesh( LPDIRECT3DDEVICEQ pd3dDevice, BYTE* pVertices, DWORD dwFVF, DWORD dwVert, DWORD dwFace );
    HRESULT CreateColorMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pSrcMesh );
    LPDIRECT3DVERTEXBUFFERQ GetVB()         { return m_pVB; }
    LPDIRECT3DINDEXBUFFERQ  GetIB()         { return m_pIB; }
    LPD3DXATTRIBUTERANGE    GetAttrib()     { return m_pAttrib; }
    DWORD                   GetAttribSize() { return m_dwAttrib; }

private:
    HRESULT CreateVBIBATT( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pTempMesh, DWORD dwVertexSize, DWORD dwFVF );

public:
    DxTangentMesh();
    ~DxTangentMesh();
};