#include "stdafx.h"

#include "./IconPreview.h"

#include "../EngineLib/DxBase/RenderInterface.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/TextTexture/TextUtil.h"

#include "../SigmaCore/DebugInclude.h"

CIconPreview::CIconPreview()
	: m_hWndTarget( NULL )

	, m_sIconID( false )
{
}

CIconPreview::~CIconPreview()
{
}

void CIconPreview::AttachWnd( HWND hWndTarget )
{
	m_hWndTarget = hWndTarget;

	CD3DViewport::WindowProc( WM_CREATE, 0, 0 );
}

void CIconPreview::DetachWnd()
{
	CD3DViewport::WindowProc( WM_DESTROY, 0, 0 );
}

VOID CIconPreview::OnFrameRender(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
	if( m_strFileName.IsEmpty() )
		return;

	//if( m_sIconID == NATIVEID_NULL() )
	//	return;

	const CRect& cSourceRect = GetSourceRect();

    BeginScene( pd3dDevice, D3DCOLOR_XRGB( 0, 0, 0 ) );

	CTextUtil::Get()->Render( TRUE );

	g_pRI->SetOrigin( m_sOrigin );
    g_pRI->SetZoom( m_fZoom );

    g_pRI->SetViewport(
        pd3dDevice,
        cSourceRect.left,
        cSourceRect.top,
        cSourceRect.Width(),
        cSourceRect.Height() );

	TextureResource textureRes = TextureManager::GetInstance().LoadTexture(
		m_strFileName.GetString(),
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		0,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_UI,
		false,
		FALSE );

	if( textureRes.GetTexture() )
	{
		const D3DXIMAGE_INFO& sImageInfo = textureRes.GetInfo();

		D3DXVECTOR2 vTexPos( 0.0f, 0.0f );
		//vTexPos.x = FLOAT( m_sIconID.wMainID * EIconSize ) / FLOAT( sImageInfo.Width );
		//vTexPos.y = FLOAT( m_sIconID.wSubID * EIconSize ) / FLOAT( sImageInfo.Height );

		D3DXVECTOR2 vTexSize( 1.0f, 1.0f );
		//vTexSize.x = FLOAT( EIconSize ) / FLOAT( sImageInfo.Width );
		//vTexSize.y = FLOAT( EIconSize ) / FLOAT( sImageInfo.Height );

		g_pRI->DrawTile(
			ic::Int2( 0, 0 ),
			ic::Int2( EIconSize, EIconSize ),
			vTexPos,
			vTexSize,
			D3DCOLOR_XRGB( 255, 255, 255 ),
			textureRes.GetTexture(),
			FALSE );

		g_pRI->FlushBatchedPrimitives();
	}

	CTextUtil::Get()->Render( FALSE );

    EndScene( pd3dDevice );
}