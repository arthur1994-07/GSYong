#pragma once

namespace ic
{
    class XFileRenderInterface
    {
    public:
        virtual void CleanUpVM() = 0;

        virtual void SetFile( const char* szPath, const char *szFileName ) = 0;

		virtual void SetFVF( DWORD dwFVF ) = 0;

		virtual void CreateVM( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;

        virtual BOOL ExistXFile() = 0;

        virtual HRESULT CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;

        virtual HRESULT CompressTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;

        virtual HRESULT RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice,
                                                ID3DXEffect* pFX,
                                                LPDIRECT3DVERTEXDECLARATION9 pDCRT,
                                                const D3DXMATRIX& matView, 
                                                const D3DXMATRIX& matProj ) = 0;

    public:
        XFileRenderInterface() {}
        virtual ~XFileRenderInterface() {}
    };

    typedef std::auto_ptr<ic::XFileRenderInterface> (*FUNC_XFILE_RENDER)();
    extern FUNC_XFILE_RENDER g_funcXFileRender;
};