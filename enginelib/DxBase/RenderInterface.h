/**
 * \date	2011/01/24
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

#include <vector>

#include "../../InternalCommonLib/Interface/RenderInterface.h"
#include "../GUInterface/UIDataType.h"
#include "./D3DObject.h"

class CRenderInterface : public ic::IRenderInterface, public CD3DObject
{
public:
    typedef std::vector< UIVERTEX > VertexVec;
    typedef std::vector< WORD > IndexVec;

protected:
    IDirect3DDevice9* m_pd3dDevice;
    D3DSURFACE_DESC m_sSurfaceDesc;

    ic::Int2 m_sOrigin;
    FLOAT m_fZoom;
    D3DVIEWPORT9 m_sViewport;

    VertexVec m_vecBatchedVertices;
    IndexVec m_vecBatchedIndices;
    D3DPRIMITIVETYPE m_emBatchedType;
    LPDIRECT3DTEXTUREQ m_pBatchedTexture;
    BOOL m_bBatchedAlphaBlend;
    UIVERTEX m_sTempVertex;

public:
    CRenderInterface();
    virtual ~CRenderInterface();

public:
    TSTRING DefaultFontName();
    DWORD DefaultFontSize();
    DWORD DefaultFontFlags();
    DWORD DefaultFontShadow();
    DWORD DefaultFontZoom();

public:
    virtual HRESULT SetViewport(
        IDirect3DDevice9* pd3dDevice,
        DWORD nX,
        DWORD nY,
        DWORD nWidth,
        DWORD nHeight,
        FLOAT fMinZ = 0.0f,
        FLOAT fMaxZ = 1.0f );

public:
    virtual VOID SetHitProxy( ic::SHitProxy* pHitProxy );
    virtual BOOL IsHitTesting();

public:
    virtual HRESULT GetTextExtent(
        SIZE& sSize,
        const TSTRING& strText,
        const TSTRING& strFont,
        DWORD nFontSize,
        DWORD nFontFlags );

    virtual HRESULT DrawText(
        FLOAT fX,
        FLOAT fY,
        const TSTRING& strText,
        DWORD nColor,
        const TSTRING& strFont,
        DWORD nFontSize,
        DWORD nFontFlags );

public:
    virtual VOID DrawLine(
        const ic::Int2& sPos0,
        const ic::Int2& sPos1,
        DWORD nColor );

	virtual VOID DrawLineBox(
		const ic::Int2& sPos,
		const ic::Int2& sSize,
		DWORD nColor );

    virtual VOID DrawTriangle(
        const ic::Int2& sPos0, const D3DXVECTOR2& sTex0,
        const ic::Int2& sPos1, const D3DXVECTOR2& sTex1,
        const ic::Int2& sPos2, const D3DXVECTOR2& sTex2,
        DWORD nColor,
        LPDIRECT3DTEXTUREQ pTexture = NULL,
        BOOL bAlphaBlend = TRUE );

    virtual VOID DrawTile(
        const ic::Int2& sPos,
        const ic::Int2& sSize,
        const D3DXVECTOR2& sTex,
        const D3DXVECTOR2& sTexSize,
        DWORD nColor,
        LPDIRECT3DTEXTUREQ pTexture = NULL,
        BOOL bAlphaBlend = TRUE );

public:
    virtual VOID SetBatchState(
        D3DPRIMITIVETYPE emType,
        LPDIRECT3DTEXTUREQ pTexture,
        BOOL bAlphaBlend );

    virtual BOOL IsFlushBatchedPrimitives(
        D3DPRIMITIVETYPE emType,
        LPDIRECT3DTEXTUREQ pTexture,
        BOOL bAlphaBlend );

    virtual VOID FlushBatchedPrimitives();

public:
    virtual HRESULT OnDeviceReset(
        IDirect3DDevice9* pd3dDevice,
        const D3DSURFACE_DESC* pBackBufferSurfaceDesc );

private:
    DWORD GetVertexColor( DWORD& nColor );
    VOID CalcOriginZoom( const ic::Int2& sPos );
    VOID SetVertexInfo( const ic::Int2& sPos );
    VOID SetVertexInfo( const ic::Int2& sPos, const D3DXVECTOR2& sTex );

public:
    virtual VOID SetOrigin( const ic::Int2& sOrigin ) { m_sOrigin = sOrigin; }
    virtual VOID SetZoom( FLOAT fZoom ) { m_fZoom = fZoom; }

public:
    virtual ic::Int2& GetOrigin() { return m_sOrigin; }
    virtual FLOAT GetZoom() { return m_fZoom; }
};

extern CRenderInterface* g_pRI;