/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

#include <string>

namespace ic
{
    struct SHitProxy;

    class IRenderInterface
    {
    public:
        virtual TSTRING DefaultFontName() = 0;
        virtual DWORD DefaultFontSize() = 0;
        virtual DWORD DefaultFontFlags() = 0;
        virtual DWORD DefaultFontShadow() = 0;
        virtual DWORD DefaultFontZoom() = 0;

    public:
        virtual HRESULT SetViewport(
            IDirect3DDevice9* pd3dDevice,
            DWORD nX,
            DWORD nY,
            DWORD nWidth,
            DWORD nHeight,
            FLOAT fMinZ = 0.0f,
            FLOAT fMaxZ = 1.0f ) = 0;

    public:
        virtual VOID SetHitProxy( SHitProxy* pHitProxy ) = 0;
        virtual BOOL IsHitTesting() = 0;

    public:
        virtual HRESULT GetTextExtent(
            SIZE& sSize,
            const TSTRING& strText,
            const TSTRING& strFont,
            DWORD nFontSize,
            DWORD nFontFlags ) = 0;

        virtual HRESULT DrawText(
            FLOAT fX,
            FLOAT fY,
            const TSTRING& strText,
            DWORD nColor,
            const TSTRING& strFont,
            DWORD nFontSize,
            DWORD nFontFlags ) = 0;

    public:
        virtual VOID DrawLine(
            const Int2& sPos0,
            const Int2& sPos1,
            DWORD nColor ) = 0;

		virtual VOID DrawLineBox(
			const ic::Int2& sPos,
			const ic::Int2& sSize,
			DWORD nColor ) = 0;

        virtual VOID DrawTriangle(
            const Int2& sPos0, const D3DXVECTOR2& sTex0,
            const Int2& sPos1, const D3DXVECTOR2& sTex1,
            const Int2& sPos2, const D3DXVECTOR2& sTex2,
            DWORD nColor,
            LPDIRECT3DTEXTUREQ pTexture = NULL,
            BOOL bAlphaBlend = TRUE ) = 0;

        virtual VOID DrawTile(
            const Int2& sPos,
            const Int2& sSize,
            const D3DXVECTOR2& sTex,
            const D3DXVECTOR2& sTexSize,
            DWORD nColor,
            LPDIRECT3DTEXTUREQ pTexture = NULL,
            BOOL bAlphaBlend = TRUE ) = 0;

    public:
        virtual VOID SetBatchState(
            D3DPRIMITIVETYPE emType,
            LPDIRECT3DTEXTUREQ pTexture,
            BOOL bAlphaBlend ) = 0;

        virtual BOOL IsFlushBatchedPrimitives(
            D3DPRIMITIVETYPE emType,
            LPDIRECT3DTEXTUREQ pTexture,
            BOOL bAlphaBlend ) = 0;

        virtual VOID FlushBatchedPrimitives() = 0;

    public:
        virtual HRESULT OnDeviceReset(
            IDirect3DDevice9* pd3dDevice,
            const D3DSURFACE_DESC* pBackBufferSurfaceDesc ) = 0;

    public:
        virtual VOID SetOrigin( const Int2& sOrigin ) = 0;
        virtual VOID SetZoom( FLOAT fZoom ) = 0;

    public:
        virtual Int2& GetOrigin() = 0;
        virtual FLOAT GetZoom() = 0;
    };
}