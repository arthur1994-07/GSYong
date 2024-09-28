#pragma once

#include "../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../InternalCommonLib/Interface/XFileRenderInterface.h"

class IXFileRender : public DxFrameMesh, public ic::XFileRenderInterface
{
public:
    virtual void CleanUpVM();

    virtual void SetFile( const char* szPath, const char *szFileName );

	virtual void SetFVF( DWORD dwFVF );

	virtual void CreateVM( LPDIRECT3DDEVICEQ pd3dDevice );

    virtual BOOL ExistXFile();

    virtual HRESULT CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice );

    virtual HRESULT CompressTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice );

    virtual HRESULT RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice,
                                            ID3DXEffect* pFX,
                                            LPDIRECT3DVERTEXDECLARATION9 pDCRT,
                                            const D3DXMATRIX& matView, 
                                            const D3DXMATRIX& matProj );

public:
    IXFileRender();
    virtual ~IXFileRender();
};

std::auto_ptr< ic::XFileRenderInterface > IXFileRender_Func();