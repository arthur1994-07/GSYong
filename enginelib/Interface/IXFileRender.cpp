#include "pch.h"

#include "IXFileRender.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IXFileRender::IXFileRender()
	: DxFrameMesh(TRUE)
{
}

IXFileRender::~IXFileRender()
{
}

void IXFileRender::CleanUpVM()
{
    DxFrameMesh::CleanUp();
}

void IXFileRender::SetFile( const char* szPath, const char *szFileName )
{
    DxFrameMesh::SetFile( szPath, szFileName );
}

void IXFileRender::SetFVF( DWORD dwFVF )
{
    DxFrameMesh::SetFVF( dwFVF );
}

void IXFileRender::CreateVM( LPDIRECT3DDEVICEQ pd3dDevice )
{
    DxFrameMesh::Create( pd3dDevice );
}

BOOL IXFileRender::ExistXFile()
{
    return DxFrameMesh::ExistXFile();
}

HRESULT IXFileRender::CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
    return DxFrameMesh::CreateTangentMesh( pd3dDevice );
}

HRESULT IXFileRender::CompressTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
    return DxFrameMesh::CompressTangentMesh( pd3dDevice );
}

HRESULT IXFileRender::RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice,
                                            ID3DXEffect* pFX,
                                            LPDIRECT3DVERTEXDECLARATION9 pDCRT,
                                            const D3DXMATRIX& matView, 
                                            const D3DXMATRIX& matProj )
{
    return DxFrameMesh::RenderVisualMaterial( pd3dDevice, pFX, pDCRT, matView, matProj );
}

std::auto_ptr< ic::XFileRenderInterface > IXFileRender_Func()
{
    return std::auto_ptr< ic::XFileRenderInterface >( new IXFileRender );
}