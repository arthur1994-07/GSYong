#include "pch.h"

#include "../../TextTexture/TextUtil.h"
#include "../DxBackUpRendTarget.h"
#include "../TextureManager.h"
#include "../DxVertexFVF.h"

#include "DxOcclusionQuery.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const UINT TEXTURE_WIDTH = 256;
	const UINT TEXTURE_HEIGHT = 256;

	const DWORD CLEAR_FLAGS = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
	const D3DCOLOR CLEAR_COLOR = D3DCOLOR_XRGB( 200, 200, 200 );
}

namespace DxOcclusionQuery
{
	BOOL g_bForceDisable = TRUE;
	DWORD g_dwCullingDelay = 0;

	LPDIRECT3DDEVICEQ g_pd3dDevice = NULL;

	LPDIRECT3DSTATEBLOCK9 g_pSave = NULL;
	LPDIRECT3DSTATEBLOCK9 g_pDraw = NULL;

	LPDIRECT3DTEXTUREQ g_pd3dTexture = NULL;
	LPDIRECT3DSURFACEQ g_pd3dSurface = NULL;
	//LPDIRECT3DSURFACEQ g_pZSurface = NULL;
	LPD3DXRENDERTOSURFACE g_pd3dRenderSurface = NULL;
}

namespace DxOcclusionQuery
{
	//DxBackupTarget g_sBackupTarget;

	BOOL IsEnable()
	{
		return ( g_pd3dRenderSurface != NULL );
	}

	BOOL BeginScene()
	{
		if( !g_pd3dDevice )
			return FALSE;

		if( !g_pd3dRenderSurface )
			return FALSE;

		//if ( !g_pZSurface )
		//	return FALSE;

		if( g_bForceDisable )
			return FALSE;

		//g_dwCullingDelay = timeGetTime();

		//g_sBackupTarget.Backup( g_pd3dDevice );

		HRESULT hr = g_pd3dRenderSurface->BeginScene( g_pd3dSurface, NULL );
		if( FAILED( hr ) )
		{
			g_pd3dRenderSurface->EndScene( 0 );

			hr = g_pd3dRenderSurface->BeginScene( g_pd3dSurface, NULL );
			if( FAILED( hr ) )
			{
				g_dwCullingDelay = 0;
				return FALSE;
			}
		}

		//g_pd3dDevice->SetDepthStencilSurface( g_pZSurface );

		//g_dwCullingDelay = 0;

		return TRUE;
	}

	VOID ClearSurface()
	{
		g_pd3dDevice->Clear( 0, NULL, CLEAR_FLAGS, CLEAR_COLOR, 1.0f, 0 );
	}

	VOID CaptureState()
	{
		g_pSave->Capture();
		g_pDraw->Apply();
	}

	VOID RollbackState()
	{
		g_pSave->Apply();
	}

	VOID EndScene()
	{
		g_pd3dRenderSurface->EndScene( 0 );

		//g_sBackupTarget.Restore();

		//g_dwCullingDelay = timeGetTime() - g_dwCullingDelay;
	}

	VOID DrawSurfaceForDebug()
	{
		if( !g_pd3dDevice )
			return;

		if( !g_pd3dTexture )
			return;

		if( g_bForceDisable )
			return;

		CRect rtWindow;
		GetWindowRect( DXUTGetHWND(), &rtWindow );

		FLOAT fPosY = FLOAT( rtWindow.Height() - TEXTURE_HEIGHT );

		VERTEXRHW vVertex[ 4 ];
		vVertex[ 0 ].vPos = D3DXVECTOR4( 0.0f, fPosY, 1.0f, 1.0f );
		vVertex[ 1 ].vPos = D3DXVECTOR4( FLOAT( TEXTURE_WIDTH ), fPosY, 1.0f, 1.0f );
		vVertex[ 2 ].vPos = D3DXVECTOR4( 0.0f, fPosY + FLOAT( TEXTURE_HEIGHT ), 1.0f, 1.0f );
		vVertex[ 3 ].vPos = D3DXVECTOR4( FLOAT( TEXTURE_WIDTH ), fPosY + FLOAT( TEXTURE_HEIGHT ), 1.0f, 1.0f );
		vVertex[ 0 ].vTex = D3DXVECTOR2( 0.0f, 0.0f );
		vVertex[ 1 ].vTex = D3DXVECTOR2( 1.0f, 0.0f );
		vVertex[ 2 ].vTex = D3DXVECTOR2( 0.0f, 1.0f );
		vVertex[ 3 ].vTex = D3DXVECTOR2( 1.0f, 1.0f );

		CTextUtil::Get()->SetStateBlock( TRUE );

		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		g_pd3dDevice->SetTexture( 0, g_pd3dTexture );
		g_pd3dDevice->SetFVF( VERTEXRHW::FVF );
		g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vVertex, sizeof( VERTEXRHW ) );

		CTextUtil::Get()->SetStateBlock( FALSE );
	}

	VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		g_pd3dDevice = pd3dDevice;
	}

	VOID RestoreDeviceObjects()
	{
		if( !g_pd3dDevice )
			return;

        if( g_bForceDisable )
			return;

		LPDIRECT3DQUERY9 pd3dQuery = NULL;
		HRESULT hr = g_pd3dDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, &pd3dQuery );
		SAFE_RELEASE( pd3dQuery );
		if( FAILED( hr ) )
			return;

		for( UINT which=0; which<2; which++ )
		{
			g_pd3dDevice->BeginStateBlock();

			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
			g_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
			g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
			g_pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );
			g_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, FALSE );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF );
			//g_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, FALSE );

			g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
			g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

			g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

			g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

			if( which == 0 )	g_pd3dDevice->EndStateBlock( &g_pSave );
			else				g_pd3dDevice->EndStateBlock( &g_pDraw );
		}

		D3DDISPLAYMODE mode;
		D3DSURFACE_DESC desc;

		hr = g_pd3dDevice->GetDisplayMode( 0, &mode );
		if( FAILED( hr ) )
			goto GOTO_FAILED;

		UINT uiWidth( TEXTURE_WIDTH );
		UINT uiHeight( TEXTURE_HEIGHT );

		hr = TextureManager::GetInstance().CreateTexture(
			g_pd3dDevice,
			uiWidth,
			uiHeight,
			1,
			D3DUSAGE_RENDERTARGET,
			mode.Format,
			D3DPOOL_DEFAULT,
			&g_pd3dTexture,
            TEXTURE_RESIZE_NONE,
            TEXTURE_ETC );

		if( FAILED( hr ) )
			goto GOTO_FAILED;

		g_pd3dTexture->GetSurfaceLevel( 0, &g_pd3dSurface );
		g_pd3dSurface->GetDesc( &desc );

		hr = D3DXCreateRenderToSurface(
			g_pd3dDevice,
			desc.Width,
			desc.Height,
			desc.Format,
			TRUE,
			D3DFMT_D16,
			&g_pd3dRenderSurface );

		if( FAILED( hr ) )
			goto GOTO_FAILED;

		//D3DFORMAT cZFormat(D3DFMT_D16);
		//hr = g_pd3dDevice->CreateDepthStencilSurface( 
		//	uiWidth, 
		//	uiHeight, 
		//	cZFormat, 
		//	D3DMULTISAMPLE_NONE, 
		//	0,
		//	TRUE, 
		//	&g_pZSurface,
		//	NULL );

		//if( FAILED(hr) )
		//	goto GOTO_FAILED;

		return;

GOTO_FAILED:
		InvalidateDeviceObjects();
	}

	VOID InvalidateDeviceObjects()
	{
		SAFE_RELEASE( g_pd3dRenderSurface );
		SAFE_RELEASE( g_pd3dSurface );
		//SAFE_RELEASE( g_pZSurface );
        TextureManager::GetInstance().ReleaseTexture( g_pd3dTexture, TEXTURE_ETC );

		SAFE_RELEASE( g_pDraw );
		SAFE_RELEASE( g_pSave );
	}

	VOID DeleteDeviceObjects()
	{
		g_pd3dDevice = NULL;
	}
}