#include "pch.h"
#include "./RenderInterface.h"

#include "../../InternalCommonLib/Core/HitProxy.h"

#include "../DxTools/D3DFontX.h"
#include "../DxTools/DxFontMan.h"
#include "../DxTools/DxDynamicVB.h"
#include "../TextTexture/TextUtil.h"

#include "./D3DViewport.h"
#include "./RenderDevice.h"

#include "../../SigmaCore/DebugInclude.h"

CRenderInterface* g_pRI = NULL;

CRenderInterface::CRenderInterface()
    : m_pd3dDevice( NULL )

    , m_fZoom( 1.0f )

    , m_emBatchedType( D3DPT_LINELIST )
    , m_pBatchedTexture( NULL )
    , m_bBatchedAlphaBlend( FALSE )
{
    memset( &m_sSurfaceDesc, 0, sizeof( m_sSurfaceDesc ) );
}

CRenderInterface::~CRenderInterface()
{
}

TSTRING CRenderInterface::DefaultFontName()
{
    return _DEFAULT_FONT;
}

DWORD CRenderInterface::DefaultFontSize()
{
    return 9;
}

DWORD CRenderInterface::DefaultFontFlags()
{
    return _DEFAULT_FONT_FLAG;
}

DWORD CRenderInterface::DefaultFontShadow()
{
    return _DEFAULT_FONT_SHADOW_FLAG;
}

DWORD CRenderInterface::DefaultFontZoom()
{
    return m_fZoom < 1.0f ? DefaultFontFlags() : DefaultFontShadow();
}

HRESULT CRenderInterface::SetViewport(
    IDirect3DDevice9* pd3dDevice,
    DWORD nX,
    DWORD nY,
    DWORD nWidth,
    DWORD nHeight,
    FLOAT fMinZ,
    FLOAT fMaxZ )
{
    m_sViewport.X = nX;
    m_sViewport.Y = nY;
    m_sViewport.Width = nWidth;
    m_sViewport.Height = nHeight;
    m_sViewport.MinZ = fMinZ;
    m_sViewport.MaxZ = fMaxZ;

    return pd3dDevice->SetViewport( &m_sViewport );
}

VOID CRenderInterface::SetHitProxy( ic::SHitProxy* pHitProxy )
{
    if( CRenderDevice::s_bHitTesting )
	{
        FlushBatchedPrimitives();

		if( pHitProxy )
		{
            CRenderDevice::s_vecHitProxy.push_back( pHitProxy );

            CRenderDevice::s_nHitProxyIndex = (DWORD)CRenderDevice::s_vecHitProxy.size();
            pHitProxy->nOrder = CRenderDevice::s_nHitProxyIndex;
		}
		else
        {
			CRenderDevice::s_nHitProxyIndex = 0;
        }
	}
	else
    {
        SAFE_DELETE( pHitProxy );
    }
}

BOOL CRenderInterface::IsHitTesting()
{
    return CRenderDevice::s_bHitTesting;
}

HRESULT CRenderInterface::GetTextExtent(
    SIZE& sSize,
    const TSTRING& strText,
    const TSTRING& strFont,
    DWORD nFontSize,
    DWORD nFontFlags )
{
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont(
        strFont.c_str(),
        nFontSize,
        nFontFlags );

    if( !pFont )
        return E_FAIL;

    return pFont->GetTextExtent( strText.c_str(), sSize );
}

HRESULT CRenderInterface::DrawText(
    FLOAT fX,
    FLOAT fY,
    const TSTRING& strText,
    DWORD nColor,
    const TSTRING& strFont,
    DWORD nFontSize,
    DWORD nFontFlags )
{
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont(
        strFont.c_str(),
        nFontSize,
        nFontFlags );

    // 줌 기능 때문에 CD3DFontX 폰트가 필요하다
    CD3DFontX* pFontX = dynamic_cast< CD3DFontX* >( pFont );
    if( !pFontX )
        return E_FAIL;

    FlushBatchedPrimitives();

    BOOL bDrawText = CTextUtil::Get()->DrawText(
        strText.c_str(),
        pFontX,
        GetVertexColor( nColor ),
        FLOAT( m_sOrigin.x ) + fX * m_fZoom,
        FLOAT( m_sOrigin.y ) + fY * m_fZoom,
        m_fZoom );

    return bDrawText ? S_OK : E_FAIL;
}

VOID CRenderInterface::DrawLine(
    const ic::Int2& sPos0,
    const ic::Int2& sPos1,
    DWORD nColor )
{
    SetBatchState( D3DPT_LINELIST, NULL, FALSE );

    WORD nBaseVertexIndex = static_cast< WORD >( m_vecBatchedVertices.size() );

    m_sTempVertex.diffuse = GetVertexColor( nColor );

    SetVertexInfo( sPos0 );
    m_vecBatchedVertices.push_back( m_sTempVertex );

    SetVertexInfo( sPos1 );
    m_vecBatchedVertices.push_back( m_sTempVertex );

    m_vecBatchedIndices.push_back( nBaseVertexIndex + 0 );
    m_vecBatchedIndices.push_back( nBaseVertexIndex + 1 );
}

VOID CRenderInterface::DrawLineBox(
	const ic::Int2& sPos,
	const ic::Int2& sSize,
	DWORD nColor )
{
	DrawLine( sPos, ic::Int2( sPos.x + sSize.x, sPos.y ), nColor );
	DrawLine( ic::Int2( sPos.x + sSize.x, sPos.y ), sPos + sSize, nColor );
	DrawLine( sPos, ic::Int2( sPos.x, sPos.y + sSize.y ), nColor );
	DrawLine( ic::Int2( sPos.x, sPos.y + sSize.y ), sPos + sSize, nColor );
}

VOID CRenderInterface::DrawTriangle(
    const ic::Int2& sPos0, const D3DXVECTOR2& sTex0,
    const ic::Int2& sPos1, const D3DXVECTOR2& sTex1,
    const ic::Int2& sPos2, const D3DXVECTOR2& sTex2,
    DWORD nColor,
    LPDIRECT3DTEXTUREQ pTexture,
    BOOL bAlphaBlend )
{
    SetBatchState( D3DPT_TRIANGLELIST, pTexture, bAlphaBlend );

    WORD nBaseVertexIndex = static_cast< WORD >( m_vecBatchedVertices.size() );

    m_sTempVertex.diffuse = GetVertexColor( nColor );

    SetVertexInfo( sPos0, sTex0 );
    m_vecBatchedVertices.push_back( m_sTempVertex );

    SetVertexInfo( sPos1, sTex1 );
    m_vecBatchedVertices.push_back( m_sTempVertex );

    SetVertexInfo( sPos2, sTex2 );
    m_vecBatchedVertices.push_back( m_sTempVertex );

    m_vecBatchedIndices.push_back( nBaseVertexIndex + 0 );
    m_vecBatchedIndices.push_back( nBaseVertexIndex + 1 );
    m_vecBatchedIndices.push_back( nBaseVertexIndex + 2 );
}

VOID CRenderInterface::DrawTile(
    const ic::Int2& sPos,
    const ic::Int2& sSize,
    const D3DXVECTOR2& sTex,
    const D3DXVECTOR2& sTexSize,
    DWORD nColor,
    LPDIRECT3DTEXTUREQ pTexture,
    BOOL bAlphaBlend )
{
    CalcOriginZoom( sPos );

    if( m_sTempVertex.x + FLOAT( sSize.x ) * m_fZoom < 0.0f ||
        m_sTempVertex.y + FLOAT( sSize.y ) * m_fZoom < 0.0f )
        return;

    SetBatchState( D3DPT_TRIANGLELIST, pTexture, bAlphaBlend );

    WORD nBaseVertexIndex = static_cast< WORD >( m_vecBatchedVertices.size() );

    m_sTempVertex.diffuse = GetVertexColor( nColor );

    SetVertexInfo( sPos, sTex );
    m_vecBatchedVertices.push_back( m_sTempVertex );

    SetVertexInfo( ic::Int2( sPos.x + sSize.x, sPos.y ), D3DXVECTOR2( sTex.x + sTexSize.x, sTex.y ) );
    m_vecBatchedVertices.push_back( m_sTempVertex );

    SetVertexInfo( ic::Int2( sPos.x, sPos.y + sSize.y ), D3DXVECTOR2( sTex.x, sTex.y + sTexSize.y ) );
    m_vecBatchedVertices.push_back( m_sTempVertex );

    SetVertexInfo( ic::Int2( sPos.x + sSize.x, sPos.y + sSize.y ), D3DXVECTOR2( sTex.x + sTexSize.x, sTex.y + sTexSize.y ) );
    m_vecBatchedVertices.push_back( m_sTempVertex );

    m_vecBatchedIndices.push_back( nBaseVertexIndex + 0 );
    m_vecBatchedIndices.push_back( nBaseVertexIndex + 1 );
    m_vecBatchedIndices.push_back( nBaseVertexIndex + 3 );

    m_vecBatchedIndices.push_back( nBaseVertexIndex + 0 );
    m_vecBatchedIndices.push_back( nBaseVertexIndex + 3 );
    m_vecBatchedIndices.push_back( nBaseVertexIndex + 2 );
}

VOID CRenderInterface::SetBatchState(
    D3DPRIMITIVETYPE emType,
    LPDIRECT3DTEXTUREQ pTexture,
    BOOL bAlphaBlend )
{
    if( !m_vecBatchedIndices.empty() )
    {
        if( IsFlushBatchedPrimitives( emType, pTexture, bAlphaBlend ) )
            FlushBatchedPrimitives();
    }

    if( m_vecBatchedIndices.empty() )
    {
        m_emBatchedType = emType;
        m_pBatchedTexture = pTexture;
        m_bBatchedAlphaBlend = bAlphaBlend;
    }
}

BOOL CRenderInterface::IsFlushBatchedPrimitives(
    D3DPRIMITIVETYPE emType,
    LPDIRECT3DTEXTUREQ pTexture,
    BOOL bAlphaBlend )
{
    const INT MaxBufferSize = 64 * 1024; // 64K

    if( CRenderDevice::s_bHitTesting ||
        ( m_emBatchedType != emType ) ||
        ( m_pBatchedTexture != pTexture ) ||
        ( m_bBatchedAlphaBlend != bAlphaBlend ) )
    {
        return TRUE;
    }

    if( ( m_vecBatchedVertices.size() + 4 ) * sizeof( UIVERTEX ) >= MaxBufferSize )
        return TRUE;

    if( ( m_vecBatchedIndices.size() + 6 ) * sizeof( WORD ) >= MaxBufferSize )
        return TRUE;

    return FALSE;
}

VOID CRenderInterface::FlushBatchedPrimitives()
{
    if( m_vecBatchedIndices.empty() )
        return;

    UINT nNumPrimitives = 0;
    switch( m_emBatchedType )
    {
    case D3DPT_TRIANGLELIST:
        nNumPrimitives = static_cast< UINT >( m_vecBatchedIndices.size() / 3 );
        break;

    case D3DPT_LINELIST:
        nNumPrimitives = static_cast< UINT >( m_vecBatchedIndices.size() / 2 );
        break;
    }

    DWORD ALPHABLENDENABLE, ALPHATESTENABLE, ALPHAREF, ALPHAFUNC;

    if( CRenderDevice::s_bHitTesting )
    {
        m_pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &ALPHATESTENABLE );
		m_pd3dDevice->GetRenderState( D3DRS_ALPHAREF, &ALPHAREF );
		m_pd3dDevice->GetRenderState( D3DRS_ALPHAFUNC, &ALPHAFUNC );

        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );

        m_pd3dDevice->SetTexture( 0, NULL );
    }
    else
    {
        m_pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &ALPHABLENDENABLE );

        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, m_bBatchedAlphaBlend );

        m_pd3dDevice->SetTexture( 0, m_pBatchedTexture );
    }

    m_pd3dDevice->SetFVF( UIVERTEX::D3DFVF_UIVERTEX );

    m_pd3dDevice->DrawIndexedPrimitiveUP(
        m_emBatchedType,
        0,
        m_vecBatchedVertices.size(),
        nNumPrimitives,
        &m_vecBatchedIndices.at( 0 ),
        D3DFMT_INDEX16,
        &m_vecBatchedVertices.at( 0 ),
        sizeof( UIVERTEX ) );

    if( CRenderDevice::s_bHitTesting )
    {
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, ALPHATESTENABLE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, ALPHAREF );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, ALPHAFUNC );
    }
    else
    {
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, ALPHABLENDENABLE );
    }

    m_vecBatchedVertices.clear();
    m_vecBatchedIndices.clear();
    m_pBatchedTexture = NULL;
}

HRESULT CRenderInterface::OnDeviceReset(
    IDirect3DDevice9* pd3dDevice,
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    m_pd3dDevice = pd3dDevice;
    memcpy( &m_sSurfaceDesc, pBackBufferSurfaceDesc, sizeof( m_sSurfaceDesc ) );

    return S_OK;
}

DWORD CRenderInterface::GetVertexColor( DWORD& nColor )
{
    if( CRenderDevice::s_bHitTesting )
    {
        D3DXCOLOR sColor = CRenderDevice::s_nHitProxyIndex;
        sColor.a = 1.0f;
        nColor = sColor;
    }

    return nColor;
}

VOID CRenderInterface::CalcOriginZoom( const ic::Int2& sPos )
{
    m_sTempVertex.x = FLOAT( m_sOrigin.x ) + FLOAT( sPos.x ) * m_fZoom;
    m_sTempVertex.y = FLOAT( m_sOrigin.y ) + FLOAT( sPos.y ) * m_fZoom;
}

VOID CRenderInterface::SetVertexInfo( const ic::Int2& sPos )
{
    CalcOriginZoom( sPos );

    m_sTempVertex.x -= 0.5f;
    m_sTempVertex.y -= 0.5f;
}

VOID CRenderInterface::SetVertexInfo( const ic::Int2& sPos, const D3DXVECTOR2& sTex )
{
    SetVertexInfo( sPos );

    m_sTempVertex.tu1 = sTex.x;
    m_sTempVertex.tv1 = sTex.y;
}